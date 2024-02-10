#include "rndl/led_driver/led_driver.h"
#include "rndl/led_driver/led_encoder.h"
#include "rndl/led_driver/led_panel_driver.h"
#include "rndl/utils.h"

#include <driver/rmt_tx.h>
#include <esp_check.h>
#include <esp_log.h>
#include <math.h>

static const char *TAG = "rndl_led_panel_driver";

typedef struct {
    rndl_led_driver_t base;
    const rndl_led_panel_driver_config_t *config;
    rmt_channel_handle_t led_chan;
    rmt_encoder_handle_t led_encoder;
    rmt_transmit_config_t tx_config;
} led_panel_driver_t;

/**
 * @brief Convert a point to a serpentine raster index
 */
static int led_panel_driver_point_to_index(rndl_led_driver_t *driver, uint16_t x, uint16_t y, size_t y_max) {
    RNDL_UNUSED(driver);
    int result;
    if (x % 2 == 0) {
        result = x * y_max + y;
    } else {
        result = (x * y_max) + (y_max - 1 - y);
    }
    return (int)result;
}

static esp_err_t led_panel_driver_write(rndl_led_driver_t *driver, const void *data, size_t size__bytes) {
    esp_err_t ret = ESP_OK;
    led_panel_driver_t *panel_driver = NULL;
    ESP_GOTO_ON_FALSE(driver && data && size__bytes, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    panel_driver = __containerof(driver, led_panel_driver_t, base);

    ESP_GOTO_ON_ERROR(
        rmt_transmit(panel_driver->led_chan, panel_driver->led_encoder, data, size__bytes, &panel_driver->tx_config),
        err, TAG, "RMT transmit failed");

    if (panel_driver->config->timeout_ms != 0) {
        ESP_GOTO_ON_ERROR(rmt_tx_wait_all_done(panel_driver->led_chan, panel_driver->config->timeout_ms), err, TAG,
                          "RMT wait done failed");
    }
err:
    return ret;
}

esp_err_t rndl_led_panel_driver_new(const rndl_led_panel_driver_config_t *config,
                                    rndl_led_driver_handle_t *driver_handle) {
    esp_err_t ret = ESP_OK;
    led_panel_driver_t *panel_driver = NULL;
    ESP_GOTO_ON_FALSE(config && driver_handle, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");

    panel_driver = calloc(1, sizeof(led_panel_driver_t));
    ESP_GOTO_ON_FALSE(panel_driver, ESP_ERR_NO_MEM, err, TAG, "no mem for led panel driver");

    // RMT docs say channels can borrow extra space from adjacent channels
    // Since we only need 1 channel, take them all.
    const int channel_count = 8;
    const int channel_size = 64;
    const int trans_queue_depth = 4;

    rmt_tx_channel_config_t tx_chan_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .gpio_num = config->gpio_num,
        .mem_block_symbols = channel_size * channel_count,
        .resolution_hz = config->resolution_hz,
        .trans_queue_depth = trans_queue_depth,
    };
    ESP_GOTO_ON_ERROR(rmt_new_tx_channel(&tx_chan_config, &panel_driver->led_chan), err, TAG,
                      "create RMT TX channel failed");

    rndl_led_encoder_config_t encoder_config = {
        .resolution = config->resolution_hz,
    };
    ESP_GOTO_ON_ERROR(rmt_new_led_encoder(&encoder_config, &panel_driver->led_encoder), err, TAG,
                      "install led strip encoder failed");
    ESP_GOTO_ON_ERROR(rmt_enable(panel_driver->led_chan), err, TAG, "enable RMT TX channel failed");

    panel_driver->base.write = led_panel_driver_write;
    panel_driver->base.point_to_index = led_panel_driver_point_to_index;
    panel_driver->tx_config.loop_count = 0; // no transfer loop
    panel_driver->config = config;

    *driver_handle = &panel_driver->base;
    goto out;

err:
    if (panel_driver) {
        if (panel_driver->led_encoder) {
            rmt_del_encoder(panel_driver->led_encoder);
        }
        if (panel_driver->led_chan) {
            rmt_disable(panel_driver->led_chan);
            rmt_del_channel(panel_driver->led_chan);
        }
        free(panel_driver);
    }
out:
    return ret;
}
