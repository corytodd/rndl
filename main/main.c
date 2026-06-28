#include <driver/gpio.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <rndl/graphics/surface.h>
#include <rndl/led_driver/led_driver.h>
#include <rndl/led_driver/led_panel_driver.h>
#include <rndl/utils.h>
#include <rndl_ble/ble_pixel_service.h>
#include <stdint.h>
#include <string.h>

static const char *TAG = "app";

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

#define MS_TO_US(ms) ((ms) * 1000)

#ifndef CONFIG_RNDL_RELAY_ACTIVE_LOW
#define CONFIG_RNDL_RELAY_ACTIVE_LOW 0
#endif

#if CONFIG_RNDL_RELAY_GPIO >= 0
static bool s_relay_on = false;

static void relay_set(bool on) {
    s_relay_on = on;
    gpio_set_level(CONFIG_RNDL_RELAY_GPIO, CONFIG_RNDL_RELAY_ACTIVE_LOW ? !on : on);
    ESP_LOGI(TAG, "relay %s", on ? "on" : "off");
}
#endif // CONFIG_RNDL_RELAY_GPIO >= 0

#if CONFIG_RNDL_BUTTON_GPIO >= 0
static void button_task(void *arg) {
    (void)arg;
    // Button debouncer
    const uint32_t pressed_mask = (1u << CONFIG_RNDL_BUTTON_DEBOUNCE_BITS) - 1u;
    const uint32_t edge_mask = pressed_mask | (1u << CONFIG_RNDL_BUTTON_DEBOUNCE_BITS);
    const uint32_t edge_target = 1u << CONFIG_RNDL_BUTTON_DEBOUNCE_BITS; // N zeros preceded by a 1

    uint32_t state = UINT32_MAX;
    for (;;) {
        state = (state << 1) | (uint32_t)gpio_get_level(CONFIG_RNDL_BUTTON_GPIO);
        if ((state & edge_mask) == edge_target) {
#if CONFIG_RNDL_RELAY_GPIO >= 0
            relay_set(!s_relay_on);
#endif // CONFIG_RNDL_RELAY_GPIO >= 0
            if (surface) {
                static const rndl_color24_t black = {.red = 0, .green = 0, .blue = 0};
                surface->clear(surface, &black);
                surface->render(surface);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(CONFIG_RNDL_BUTTON_SAMPLE_MS) + 1);
    }
}
#endif // CONFIG_RNDL_BUTTON_GPIO >= 0

void app_main(void) {
    ESP_LOGI(TAG, "initializing");

    ESP_ERROR_CHECK(rndl_led_panel_driver_new(&config, &led_driver));

#if CONFIG_RNDL_RELAY_GPIO >= 0
    gpio_reset_pin(CONFIG_RNDL_RELAY_GPIO);
    gpio_set_direction(CONFIG_RNDL_RELAY_GPIO, GPIO_MODE_OUTPUT);
    relay_set(true);
    ESP_LOGI(TAG, "relay GPIO %d", CONFIG_RNDL_RELAY_GPIO);
    vTaskDelay(pdMS_TO_TICKS(100)); // relay/LED settle
#else
    ESP_LOGI(TAG, "relay disabled");
#endif // CONFIG_RNDL_RELAY_GPIO >= 0

#if CONFIG_RNDL_BUTTON_GPIO >= 0
    gpio_reset_pin(CONFIG_RNDL_BUTTON_GPIO);
    gpio_set_direction(CONFIG_RNDL_BUTTON_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(CONFIG_RNDL_BUTTON_GPIO, GPIO_PULLUP_ONLY);
    xTaskCreate(button_task, "button", 2048, NULL, 5, NULL);
    ESP_LOGI(TAG, "button GPIO %d", CONFIG_RNDL_BUTTON_GPIO);
#else
    ESP_LOGI(TAG, "button disabled");
#endif // CONFIG_RNDL_BUTTON_GPIO >= 0

    ESP_LOGI(TAG, "LED data GPIO %d", CONFIG_RNDL_GPIO);

    ESP_ERROR_CHECK(rndl_surface_create(&surface_config, led_driver, &surface));

    static const rndl_color24_t black = {.red = 0, .green = 0, .blue = 0};
    surface->clear(surface, &black);
    surface->render(surface);

    const rndl_ble_service_config_t ble_config = {
        .surface = surface,
        .device_name = "rndl-panel",
    };
    ESP_ERROR_CHECK(rndl_ble_service_start(&ble_config));

    ESP_LOGI(TAG, "startup complete");

    for (;;) {
        vTaskDelay(portMAX_DELAY);
    }
}
