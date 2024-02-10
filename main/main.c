#include "example/example.h"

#include <esp_err.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <rndl/graphics/surface.h>
#include <rndl/led_driver/led_driver.h>
#include <rndl/led_driver/led_panel_driver.h>
#include <rndl/utils.h>
#include <stdint.h>
#include <string.h>

static const char *TAG = "app";

#define MS_TO_US(ms) ((ms) * 1000)

static rndl_led_driver_handle_t led_driver = NULL;
static const rndl_led_panel_driver_config_t config = {
    .gpio_num = CONFIG_RNDL_GPIO,
    .resolution_hz = RNDL_MHZ(CONFIG_RNDL_RESOLUTION_MHZ),
    .led_count = (CONFIG_RNDL_ROWS * CONFIG_RNDL_COLS),
    .timeout_ms = portMAX_DELAY,
};

static rndl_surface_handle_t surface = NULL;
static const rndl_surface_config_t surface_config = {
    .width = CONFIG_RNDL_COLS,
    .height = CONFIG_RNDL_ROWS,
};

void app_main(void) {
    ESP_LOGI(TAG, "initializing");

    ESP_ERROR_CHECK(rndl_led_panel_driver_new(&config, &led_driver));
    ESP_ERROR_CHECK(rndl_surface_create(&surface_config, led_driver, &surface));

    ESP_LOGI(TAG, "startup complete");

    example_raindrops(surface, &surface_config);

    ESP_LOGI(TAG, "pattern complete");

    for (;;) {
        vTaskDelay(portMAX_DELAY);
    }
}
