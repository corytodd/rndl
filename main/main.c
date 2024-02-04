#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <rndl/color.h>
#include <rndl/graphics/line.h>
#include <rndl/graphics/point.h>
#include <rndl/graphics/rect.h>
#include <rndl/graphics/surface.h>
#include <rndl/led_driver/led_driver.h>
#include <rndl/led_driver/led_panel_driver.h>
#include <string.h>

static const char *TAG = "app";

static led_driver_handle_t led_driver = NULL;
const led_panel_driver_config_t config = {
    .gpio_num = CONFIG_RNDL_GPIO,
    .resolution_hz = MHZ(CONFIG_RNDL_RESOLUTION_MHZ),
    .led_count = (CONFIG_RNDL_ROWS * CONFIG_RNDL_COLS),
    .frame_time_ms = FPS_TO_MS(CONFIG_RNDL_FPS),
    .fn_delay = platform_delay_ms,
    .timeout_ms = portMAX_DELAY,
};

static surface_handle_t surface = NULL;
const surface_config_t surface_config = {
    .width = CONFIG_RNDL_COLS,
    .height = CONFIG_RNDL_ROWS,
};

// TODO: the driver should control what "off" means, not the renderer
static const color_t color_off = {0};

// Draw corners points on extend of surface
MAYBE_UNUSED static void pattern_surface_extent(void) {
    ESP_LOGI(TAG, "surface extent");

    hsv_t hsv = {.hue = 100, .saturation = 100, .value = 50};
    color_t color = {0};
    point_t point = {0};

    int state = 0;
    while (1) {

        switch (state) {
            case 0:
                point.x = 0;
                point.y = 0;
                state = 1;
                break;
            case 1:
                point.x = CONFIG_RNDL_COLS - 1;
                point.y = 0;
                state = 2;
                break;
            case 2:
                point.x = CONFIG_RNDL_COLS - 1;
                point.y = CONFIG_RNDL_ROWS - 1;
                state = 3;
                break;
            case 3:
                point.x = 0;
                point.y = CONFIG_RNDL_ROWS - 1;
                state = 0;
                break;
        }

        color_hsv2rgb(&hsv, &color);

        surface->clear(surface, &color_off);
        surface->draw_pixel(surface, &point, &color);
        surface->render(surface);
        hsv.hue++;
    }
}

// Loop a shrinking rectangle forever
MAYBE_UNUSED static void pattern_strobing_rectangle(void) {
    ESP_LOGI(TAG, "strobing rectangle");

    hsv_t hsv = {.hue = 100, .saturation = 100, .value = 50};
    color_t color = {0};

    rect_t rect = {
        .top_left = {0, 0},
        .bottom_right = {CONFIG_RNDL_COLS - 1, CONFIG_RNDL_ROWS - 1},
    };

    while (1) {
        color_hsv2rgb(&hsv, &color);

        surface->clear(surface, &color_off);
        surface->draw_rect(surface, &rect, NULL, &color);
        surface->render(surface);

        rect.top_left.x = (rect.top_left.x + 1) % (CONFIG_RNDL_COLS / 2);
        rect.top_left.y = (rect.top_left.y + 1) % (CONFIG_RNDL_ROWS / 2);

        rect.bottom_right.x = (rect.bottom_right.x - 1);
        rect.bottom_right.y = (rect.bottom_right.y - 1);

        if (rect.bottom_right.x < (CONFIG_RNDL_COLS / 2)) {
            rect.bottom_right.x = CONFIG_RNDL_COLS - 1;
        }
        if (rect.bottom_right.y < (CONFIG_RNDL_ROWS / 2)) {
            rect.bottom_right.y = CONFIG_RNDL_ROWS - 1;
        }
        if (rect.bottom_right.y - rect.top_left.y <= 1 || rect.bottom_right.x - rect.top_left.x <= 1) {
            rect.top_left.x = 0;
            rect.top_left.y = 0;
            rect.bottom_right.x = CONFIG_RNDL_COLS - 1;
            rect.bottom_right.y = CONFIG_RNDL_ROWS - 1;
        }
        hsv.hue++;
    }
}

// Draw angled lines on the surface
MAYBE_UNUSED static void pattern_angled_lines(void) {
    ESP_LOGI(TAG, "angled lines");

    hsv_t hsv = {.hue = 100, .saturation = 100, .value = 50};
    color_t color = {0};

    line_t line = {
        .start = {0, 0},
        .end = {CONFIG_RNDL_COLS - 1, CONFIG_RNDL_ROWS - 1},
    };

    while (1) {
        color_hsv2rgb(&hsv, &color);

        surface->clear(surface, &color_off);
        surface->draw_line(surface, &line, NULL, &color);
        surface->render(surface);

        hsv.hue++;
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "initializing");
    ESP_ERROR_CHECK(led_panel_driver_new(&config, &led_driver));
    ESP_ERROR_CHECK(surface_create(&surface_config, led_driver, &surface));
    ESP_LOGI(TAG, "startup complete");

    pattern_strobing_rectangle();

    ESP_LOGI(TAG, "pattern complete");

    for (;;) {
    }
}
