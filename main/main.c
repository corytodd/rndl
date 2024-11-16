#include "example/example.h"
#include "rndl_mqtt.h"

#include <esp_err.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/task.h>
#include <rndl/graphics/surface.h>
#include <rndl/led_driver/led_driver.h>
#include <rndl/led_driver/led_panel_driver.h>
#include <rndl/utils.h>
#include <stdint.h>
#include <string.h>

static const char *TAG = "app";

#define MS_TO_US(ms) ((ms) * 1000)

static EventGroupHandle_t app_event;
#define EVENT_STREAMING_IDLE BIT0
#define EVENT_STREAMING_ON   BIT1
#define EVENT_STREAMING_OFF  BIT2

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

static const rndl_color24_t on_air_color = {0, 255, 0};
static const rndl_color24_t off_air_color = {255, 0, 0};
static const rndl_color24_t reset_color = {0};

static const rndl_point_t glyph_start = {CONFIG_RNDL_COLS / 2 - 4, CONFIG_RNDL_ROWS / 2 - 4};

extern const char font_8x8_basic[128][8];
static void draw_char(rndl_surface_handle_t surface, char c, const rndl_point_t *origin, const rndl_color24_t *color) {
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if ((font_8x8_basic[(int)c][y] >> x) & 1) {
                const rndl_point_t point = {origin->x + x, origin->y + y};
                surface->draw_pixel(surface, &point, color);
            }
        }
    }
}

static void draw_on_air_box(rndl_surface_handle_t surface) {
    const rndl_rect_t rect = {
        .top_left = {0},
        .bottom_right = {CONFIG_RNDL_COLS - 1, CONFIG_RNDL_ROWS - 1},
    };
    surface->draw_rect(surface, &rect, &on_air_color, RNDL_LINE_TYPE_SOLID, NULL);

    draw_char(surface, 'X', &glyph_start, &on_air_color);
}

static void draw_off_air_box(rndl_surface_handle_t surface) {
    const rndl_rect_t rect = {
        .top_left = {0},
        .bottom_right = {CONFIG_RNDL_COLS - 1, CONFIG_RNDL_ROWS - 1},
    };
    surface->draw_rect(surface, &rect, &off_air_color, RNDL_LINE_TYPE_SOLID, NULL);

    draw_char(surface, 'O', &glyph_start, &off_air_color);
}

static void on_mqtt_event_data(const char *data) {
    ESP_LOGI(TAG, "MQTT data");
    char *p;
    p = strstr(data, "\"recording\": true");
    if (p != NULL) {
        xEventGroupSetBits(app_event, EVENT_STREAMING_ON);
    } else {
        xEventGroupSetBits(app_event, EVENT_STREAMING_OFF);
    }
}

static void on_timer_elapsed(void *arg) {
    xEventGroupSetBits(app_event, EVENT_STREAMING_IDLE);
}

void app_main(void) {
    ESP_LOGI(TAG, "initializing");

    app_event = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(rndl_mqtt_start(on_mqtt_event_data));

    ESP_ERROR_CHECK(rndl_led_panel_driver_new(&config, &led_driver));
    ESP_ERROR_CHECK(rndl_surface_create(&surface_config, led_driver, &surface));

    esp_timer_handle_t timer;
    esp_timer_create_args_t timer_args = {
        .callback = on_timer_elapsed,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "streaming_timer",
    };
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &timer));

    surface->clear(surface, &reset_color);
    surface->render(surface);

    ESP_LOGI(TAG, "startup complete");

    for (;;) {
        EventBits_t bits = xEventGroupWaitBits(
            app_event, EVENT_STREAMING_IDLE | EVENT_STREAMING_ON | EVENT_STREAMING_OFF, pdTRUE, pdFALSE, portMAX_DELAY);

        surface->clear(surface, &reset_color);
        if (bits & EVENT_STREAMING_IDLE) {
            ESP_LOGI(TAG, "idle");
            surface->clear(surface, &reset_color);
        } else if (bits & EVENT_STREAMING_ON) {
            ESP_LOGI(TAG, "streaming on");
            draw_on_air_box(surface);
            if (esp_timer_is_active(timer)) {
                esp_timer_stop(timer);
            }
        } else if (bits & EVENT_STREAMING_OFF) {
            ESP_LOGI(TAG, "streaming off");
            draw_off_air_box(surface);
            if (!esp_timer_is_active(timer)) {
                esp_timer_start_once(timer, MS_TO_US(20000));
            }
        }

        surface->render(surface);
    }
}
