#include "color/color.h"
#include "led_driver/led_panel_driver.h"
#include "renderer/point.h"

#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>

#define RMT_LED_STRIP_RESOLUTION_HZ 10000000 // 10MHz resolution, 1 tick = 0.1us (led strip needs a high resolution)
#define RMT_LED_STRIP_GPIO_NUM      2

#define LED_CHAN  3
#define LED_ROWS  16
#define LED_COLS  32
#define LED_COUNT (LED_ROWS * LED_COLS)
#define LED_FPS   1

static const char *TAG = "lighty";

static __aligned(4) uint8_t display_buffer[LED_COUNT * LED_CHAN];

void app_main(void) {
    const hsv_t hsv = {.hue = 0, .saturation = 100, .value = 10};
    const color_t color_off = {0};
    color_t pixel = {0};
    point_t point = {.x = 0, .y = 0};

    static led_driver_handle_t led_driver = NULL;
    const led_panel_driver_config_t config = {
        .gpio_num = RMT_LED_STRIP_GPIO_NUM,
        .resolution_hz = RMT_LED_STRIP_RESOLUTION_HZ,
        .led_count = LED_COUNT,
        .fps = LED_FPS,
    };
    led_panel_driver_new(&config, &led_driver);

    ESP_LOGI(TAG, "starting Lighty");
    while (1) {
        color_hsv2rgb(&hsv, &pixel);

        memset(&display_buffer, 0, sizeof(display_buffer));
        for (int i = 0; i < LED_COLS; ++i) {
            point.x = i;
            int rindex = point_to_sraster(&point, LED_ROWS);
            ESP_ERROR_CHECK(rindex < LED_COUNT ? ESP_OK : ESP_FAIL);
            ESP_LOGD(TAG, "point: (%d, %d), rindex: %d", point.x, point.y, rindex);
            memcpy(&display_buffer[rindex * LED_CHAN], &pixel, sizeof(pixel));
        }
        point.x = 0;
        point.y = (point.y + 1) % LED_ROWS;

        ESP_ERROR_CHECK(
            led_driver->led_driver_write_blocking(led_driver, display_buffer, sizeof(display_buffer), portMAX_DELAY));
    }
}