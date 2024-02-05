#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <rndl/color.h>
#include <rndl/graphics/line.h>
#include <rndl/graphics/point.h>
#include <rndl/graphics/rect.h>
#include <rndl/graphics/surface.h>
#include <rndl/led_driver/led_driver.h>
#include <rndl/led_driver/led_panel_driver.h>
#include <rndl/utils.h>
#include <string.h>

static const char *TAG = "app";

extern void platform_delay_ms(uint32_t delay_ms);

static rndl_led_driver_handle_t led_driver = NULL;
const rndl_led_panel_driver_config_t config = {
    .gpio_num = CONFIG_RNDL_GPIO,
    .resolution_hz = RNDL_MHZ(CONFIG_RNDL_RESOLUTION_MHZ),
    .led_count = (CONFIG_RNDL_ROWS * CONFIG_RNDL_COLS),
    .frame_time_ms = RNDL_FPS_TO_MS(CONFIG_RNDL_FPS),
    .fn_delay = platform_delay_ms,
    .timeout_ms = portMAX_DELAY,
};

static rndl_surface_handle_t surface = NULL;
const rndl_surface_config_t surface_config = {
    .width = CONFIG_RNDL_COLS,
    .height = CONFIG_RNDL_ROWS,
};

// TODO: the driver should control what "off" means, not the renderer
static const rndl_color24_t color_off = {0};

// Draw corners points on extend of surface
RNDL_MAYBE_UNUSED static void pattern_surface_extent(void) {
    ESP_LOGI(TAG, "surface extent");

    rndl_hsv_t hsv = {.hue = 100, .saturation = 100, .value = 50};
    rndl_color24_t color = {0};
    rndl_point_t point = {0};

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

        rndl_color_hsv2color24(&hsv, &color);

        surface->clear(surface, &color_off);
        surface->draw_pixel(surface, &point, &color);
        surface->render(surface);
        hsv.hue++;
    }
}

// Loop a shrinking rectangle forever
RNDL_MAYBE_UNUSED static void pattern_strobing_rectangle(void) {
    ESP_LOGI(TAG, "strobing rectangle");

    rndl_hsv_t hsv = {.hue = 100, .saturation = 100, .value = 50};
    rndl_color24_t color = {0};

    rndl_rect_t rect = {
        .top_left = {0, 0},
        .bottom_right = {CONFIG_RNDL_COLS - 1, CONFIG_RNDL_ROWS - 1},
    };

    while (1) {
        rndl_color_hsv2color24(&hsv, &color);

        surface->clear(surface, &color_off);
        surface->draw_rect(surface, &rect, &color, NULL, NULL);
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
RNDL_MAYBE_UNUSED static void pattern_angled_lines(void) {
    ESP_LOGI(TAG, "angled lines");

    rndl_hsv_t hsv = {.hue = 100, .saturation = 100, .value = 50};
    rndl_color24_t color = {0};

    rndl_line_t line = {
        .start = {0, 0},
        .end = {CONFIG_RNDL_COLS - 1, CONFIG_RNDL_ROWS - 1},
    };

    while (1) {
        rndl_color_hsv2color24(&hsv, &color);

        surface->clear(surface, &color_off);
        surface->draw_line(surface, &line, &color, NULL);
        surface->render(surface);

        hsv.hue++;
    }
}

// Draw concentric circles on the surface
RNDL_MAYBE_UNUSED static void pattern_circles(void) {
    ESP_LOGI(TAG, "raindrop");

    rndl_hsv_t hsv = {.hue = 100, .saturation = 100, .value = 50};
    rndl_color24_t color = {0};

    const rndl_point_t center = {CONFIG_RNDL_COLS / 2, CONFIG_RNDL_ROWS / 2};
    rndl_circle_t circles[] = {
        {.center = center, .radius = 2},
        {.center = center, .radius = 4},
        {.center = center, .radius = 8},
    };

    while (1) {
        rndl_color_hsv2color24(&hsv, &color);

        surface->clear(surface, &color_off);

        for (int i = 0; i < RNDL_COUNT_OF(circles); i++) {
            surface->draw_circle(surface, &circles[i], &color, NULL, NULL);
            circles[i].radius = (circles[i].radius + 1) % (CONFIG_RNDL_COLS / 2);
        }
        surface->render(surface);
        hsv.hue++;
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "initializing");
    ESP_ERROR_CHECK(rndl_led_panel_driver_new(&config, &led_driver));
    ESP_ERROR_CHECK(rndl_surface_create(&surface_config, led_driver, &surface));
    ESP_LOGI(TAG, "startup complete");

    pattern_strobing_rectangle();

    ESP_LOGI(TAG, "pattern complete");

    for (;;) {
    }
}
