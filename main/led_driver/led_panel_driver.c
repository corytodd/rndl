#include "led_driver/led_driver.h"
#include "led_driver/led_encoder.h"
#include "led_driver/led_panel_driver.h"
#include "utils.h"

#include <driver/rmt_tx.h>
#include <esp_check.h>
#include <esp_log.h>
#include <math.h>

static const char *TAG = "led_panel_driver";

typedef struct {
    led_driver_t base;
    const led_panel_driver_config_t *config;
    rmt_channel_handle_t led_chan;
    rmt_encoder_handle_t led_encoder;
    rmt_transmit_config_t tx_config;
} led_panel_driver_t;

/**
 * @brief Convert a point to a serpentine raster index
 */
static inline int led_panel_driver_point_to_index(int x, int y, int y_max) {
    if (x % 2 == 0) {
        return x * y_max + y;
    } else {
        return (x * y_max) + (y_max - 1 - y);
    }
}

static esp_err_t led_panel_driver_write_blocking(led_driver_t *driver, const void *data, size_t size__bytes) {
    esp_err_t ret = ESP_OK;
    led_panel_driver_t *panel_driver = NULL;
    ESP_GOTO_ON_FALSE(driver && data && size__bytes, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    panel_driver = __containerof(driver, led_panel_driver_t, base);

    ESP_ERROR_CHECK(
        rmt_transmit(panel_driver->led_chan, panel_driver->led_encoder, data, size__bytes, &panel_driver->tx_config));

    ESP_ERROR_CHECK(rmt_tx_wait_all_done(panel_driver->led_chan, panel_driver->config->timeout_ms));

    panel_driver->config->fn_delay(panel_driver->config->frame_time_ms);

err:
    return ret;
}

esp_err_t led_panel_driver_new(const led_panel_driver_config_t *config, led_driver_handle_t *driver_handle) {
    esp_err_t ret = ESP_OK;
    led_panel_driver_t *panel_driver = NULL;
    ESP_GOTO_ON_FALSE(config && driver_handle, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(config->fn_delay, ESP_ERR_INVALID_ARG, err, TAG, "fn_delay is not set");

    panel_driver = calloc(1, sizeof(led_panel_driver_t));
    ESP_GOTO_ON_FALSE(panel_driver, ESP_ERR_NO_MEM, err, TAG, "no mem for led panel driver");

    panel_driver->base.write_blocking = led_panel_driver_write_blocking;
    panel_driver->base.point_to_index = led_panel_driver_point_to_index;
    ESP_LOGD(TAG, "create RMT TX channel");
    rmt_tx_channel_config_t tx_chan_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .gpio_num = config->gpio_num,
        .mem_block_symbols = 64, // increase the block size can make the LED less flickering
        .resolution_hz = config->resolution_hz,
        .trans_queue_depth = 4, // set the number of transactions that can be pending in the background
    };
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &panel_driver->led_chan));

    ESP_LOGD(TAG, "install led strip encoder");
    led_encoder_config_t encoder_config = {
        .resolution = config->resolution_hz,
    };
    ESP_ERROR_CHECK(rmt_new_led_encoder(&encoder_config, &panel_driver->led_encoder));

    ESP_LOGD(TAG, "enable RMT TX channel");
    ESP_ERROR_CHECK(rmt_enable(panel_driver->led_chan));

    panel_driver->tx_config.loop_count = 0; // no transfer loop
    panel_driver->config = config;

    *driver_handle = &panel_driver->base;
err:
    return ret;
}
