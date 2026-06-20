#include "rndl_ble/ble_pixel_service.h"

#include "rndl/color.h"
#include "rndl/graphics/point.h"
#include "rndl/utils.h"

#include <assert.h>
#include <esp_check.h>
#include <esp_err.h>
#include <esp_log.h>
#include <host/ble_hs.h>
#include <host/util/util.h>
#include <nimble/nimble_port.h>
#include <nimble/nimble_port_freertos.h>
#include <nvs_flash.h>
#include <services/gap/ble_svc_gap.h>
#include <services/gatt/ble_svc_gatt.h>
#include <stdint.h>
#include <string.h>

static const char *TAG = "rndl_ble";

static const ble_uuid128_t pixel_service_uuid =
    BLE_UUID128_INIT(0xfa, 0xa1, 0x35, 0xf8, 0x0d, 0x5b, 0xc6, 0x9e, 0x15, 0x42, 0xfb, 0x18, 0x03, 0x53, 0x71, 0x08);
static const ble_uuid128_t pixel_write_chr_uuid =
    BLE_UUID128_INIT(0x9c, 0x43, 0xca, 0xad, 0xe9, 0x89, 0x29, 0x41, 0x69, 0xa8, 0x61, 0x6f, 0xd3, 0xc5, 0x00, 0x00);
static const ble_uuid128_t dimensions_chr_uuid =
    BLE_UUID128_INIT(0x5e, 0xae, 0x41, 0xbe, 0xad, 0xcb, 0x43, 0xb3, 0x22, 0x49, 0x04, 0x60, 0x11, 0xf6, 0xe0, 0xd6);

#define PIXEL_RECORD_SIZE__BYTES 5

static rndl_surface_handle_t s_surface = NULL;
static uint16_t s_width = 0;
static uint16_t s_height = 0;
static uint8_t s_own_addr_type = 0;

static int pixel_write_access_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt,
                                 void *arg) {
    RNDL_UNUSED(conn_handle);
    RNDL_UNUSED(attr_handle);
    RNDL_UNUSED(arg);
    assert(s_surface != NULL && "surface not set");

    if (ctxt->op != BLE_GATT_ACCESS_OP_WRITE_CHR) {
        return BLE_ATT_ERR_UNLIKELY;
    }

    struct os_mbuf *om = ctxt->om;
    uint16_t len = OS_MBUF_PKTLEN(om);

    if ((len % PIXEL_RECORD_SIZE__BYTES) != 0) {
        ESP_LOGE(TAG, "malformed packet: len=%d, not divisible by %d", len, PIXEL_RECORD_SIZE__BYTES);
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }
    uint16_t record_count = len / PIXEL_RECORD_SIZE__BYTES;

    ESP_LOGD(TAG, "pixel write: len=%d record_count=%d", len, record_count);

    uint8_t record[PIXEL_RECORD_SIZE__BYTES];
    for (uint16_t i = 0; i < record_count; ++i) {
        uint16_t offset = i * PIXEL_RECORD_SIZE__BYTES;
        int rc = os_mbuf_copydata(om, offset, PIXEL_RECORD_SIZE__BYTES, record);
        if (rc != 0) {
            ESP_LOGE(TAG, "os_mbuf_copydata failed at record %d: %d", i, rc);
            return BLE_ATT_ERR_UNLIKELY;
        }

        const rndl_point_t point = {.x = record[0], .y = record[1]};
        const rndl_color24_t color = {.red = record[2], .green = record[3], .blue = record[4]};
        s_surface->draw_pixel(s_surface, &point, &color);
    }

    if (record_count > 0) {
        s_surface->render(s_surface);
    }

    return 0;
}

static int dimensions_access_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt,
                                void *arg) {
    RNDL_UNUSED(conn_handle);
    RNDL_UNUSED(attr_handle);
    RNDL_UNUSED(arg);
    assert(s_width > 0 && s_height > 0 && "one or both width/height are 0");

    if (ctxt->op != BLE_GATT_ACCESS_OP_READ_CHR) {
        return BLE_ATT_ERR_UNLIKELY;
    }

    uint8_t dims[4];
    put_le16(&dims[0], s_width);
    put_le16(&dims[2], s_height);
    int rc = os_mbuf_append(ctxt->om, dims, sizeof(dims));
    if (rc != 0) {
        ESP_LOGE(TAG, "os_mbuf_append: %d", rc);
        return BLE_ATT_ERR_INSUFFICIENT_RES;
    }
    return 0;
}

static const struct ble_gatt_chr_def pixel_service_chrs[] = {
    {
        .uuid = &pixel_write_chr_uuid.u,
        .access_cb = pixel_write_access_cb,
        // TODO: no auth/encryption required - any nearby device can write pixels.
        .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_NO_RSP,
    },
    {
        .uuid = &dimensions_chr_uuid.u,
        .access_cb = dimensions_access_cb,
        .flags = BLE_GATT_CHR_F_READ,
    },
    {0},
};

static const struct ble_gatt_svc_def gatt_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &pixel_service_uuid.u,
        .characteristics = pixel_service_chrs,
    },
    {0},
};

static int gap_event_cb(struct ble_gap_event *event, void *arg);

static void start_advertising(const char *device_name) {
    struct ble_gap_adv_params adv_params = {0};
    struct ble_hs_adv_fields fields = {0};

    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;
    fields.name = (const uint8_t *)device_name;
    fields.name_len = strlen(device_name);
    fields.name_is_complete = 1;

    int rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to set advertising fields: %d", rc);
        return;
    }

    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    rc = ble_gap_adv_start(s_own_addr_type, NULL, BLE_HS_FOREVER, &adv_params, gap_event_cb, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to start advertising: %d", rc);
        return;
    }

    ESP_LOGI(TAG, "advertising started as \"%s\"", device_name);
}

static int gap_event_cb(struct ble_gap_event *event, void *arg) {
    RNDL_UNUSED(arg);

    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            ESP_LOGI(TAG, "connection %s; status=%d", (event->connect.status == 0 ? "established" : "failed"),
                     event->connect.status);
            return 0;
        case BLE_GAP_EVENT_DISCONNECT:
            ESP_LOGI(TAG, "disconnected; reason=%d", event->disconnect.reason);
            start_advertising(ble_svc_gap_device_name());
            return 0;
        case BLE_GAP_EVENT_ADV_COMPLETE:
            // If this ever fires with a "connection established" reason, not just timeout,
            // this races CONNECT's restart and logs a harmless adv-start failure.
            start_advertising(ble_svc_gap_device_name());
            return 0;
        default:
            return 0;
    }
}

static void on_sync(void) {
    int rc = ble_hs_util_ensure_addr(0);
    if (rc != 0) {
        ESP_LOGE(TAG, "ble_hs_util_ensure_addr failed: %d", rc);
        return;
    }

    rc = ble_hs_id_infer_auto(0, &s_own_addr_type);
    if (rc != 0) {
        ESP_LOGE(TAG, "ble_hs_id_infer_auto failed: %d", rc);
        return;
    }

    start_advertising(ble_svc_gap_device_name());
}

static void ble_host_task(void *param) {
    RNDL_UNUSED(param);
    nimble_port_run();
    nimble_port_freertos_deinit();
}

esp_err_t rndl_ble_service_start(const rndl_ble_service_config_t *config) {
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(config && config->surface && config->device_name, ESP_ERR_INVALID_ARG, err, TAG,
                      "invalid argument");

    s_surface = config->surface;
    s_width = CONFIG_RNDL_COLS;
    s_height = CONFIG_RNDL_ROWS;
    assert(s_surface != NULL && "config surface not set");
    assert(s_width > 0 && s_height > 0 && "config width/height not set");

    esp_err_t nvs_ret = nvs_flash_init();
    if (nvs_ret == ESP_ERR_NVS_NO_FREE_PAGES || nvs_ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        nvs_ret = nvs_flash_init();
    }
    ESP_GOTO_ON_ERROR(nvs_ret, err, TAG, "nvs_flash_init failed");

    ESP_GOTO_ON_ERROR(nimble_port_init(), err, TAG, "nimble_port_init failed");

    ble_hs_cfg.sync_cb = on_sync;

    ble_svc_gap_init();
    ble_svc_gatt_init();

    int rc = ble_gatts_count_cfg(gatt_svcs);
    ESP_GOTO_ON_FALSE(rc == 0, ESP_FAIL, err, TAG, "ble_gatts_count_cfg failed: %d", rc);
    rc = ble_gatts_add_svcs(gatt_svcs);
    ESP_GOTO_ON_FALSE(rc == 0, ESP_FAIL, err, TAG, "ble_gatts_add_svcs failed: %d", rc);

    rc = ble_svc_gap_device_name_set(config->device_name);
    ESP_GOTO_ON_FALSE(rc == 0, ESP_FAIL, err, TAG, "ble_svc_gap_device_name_set failed: %d", rc);

    nimble_port_freertos_init(ble_host_task);

err:
    return ret;
}
