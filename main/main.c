#include "color/color.h"
#include "led_driver/led_panel_driver.h"
#include "platform.h"
#include "renderer/point.h"
#include "renderer/surface.h"

#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <string.h>

static const char *TAG = "lighty";

static led_driver_handle_t led_driver = NULL;
const led_panel_driver_config_t config = {
    .gpio_num = CONFIG_LED_GPIO,
    .resolution_hz = MHZ(CONFIG_LED_RESOLUTION_MHZ),
    .led_count = (CONFIG_LED_ROWS * CONFIG_LED_COLS),
    .frame_time_ms = FPS_TO_MS(CONFIG_LED_FPS),
    .fn_delay = platform_delay_ms,
    .timeout_ms = portMAX_DELAY,
};

static surface_handle_t surface = NULL;
const surface_config_t surface_config = {
    .width = CONFIG_LED_COLS,
    .height = CONFIG_LED_ROWS,
};

void app_main(void) {
    const color_t color_off = {0};
    hsv_t hsv = {.hue = 100, .saturation = 100, .value = 50};
    color_t color = {0};
    point_t point = {0};

    ESP_ERROR_CHECK(led_panel_driver_new(&config, &led_driver));
    ESP_ERROR_CHECK(surface_create(&surface_config, led_driver, &surface));

    ESP_LOGI(TAG, "startup complete");

    rect_t rect = {
        .top_left = {0, 0},
        .bottom_right = {CONFIG_LED_COLS - 1, CONFIG_LED_ROWS - 1},
    };
    while (1) {
        color_hsv2rgb(&hsv, &color);

        surface->clear(surface, &color_off);
        surface->draw_rect(surface, &rect, NULL, &color);
        surface->render(surface);
        point.y = (point.y + 1) % CONFIG_LED_ROWS;

        ++hsv.hue;
    }
}
