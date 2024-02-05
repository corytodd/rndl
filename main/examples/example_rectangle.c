#include <esp_log.h>
#include <rndl/color.h>
#include <rndl/graphics/rect.h>
#include <rndl/graphics/surface.h>
#include <rndl/utils.h>

static const char *TAG = "example";

// Loop a shrinking rectangle forever
RNDL_MAYBE_UNUSED static void example_rectangle(rndl_surface_handle_t surface,
                                                const rndl_surface_config_t *surface_config) {
    ESP_LOGI(TAG, "rectangle");

    const rndl_color24_t color_off = {0};
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
