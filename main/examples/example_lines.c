#include <esp_log.h>
#include <rndl/color.h>
#include <rndl/graphics/line.h>
#include <rndl/graphics/surface.h>
#include <rndl/utils.h>

static const char *TAG = "example";

// Draw angled lines on the surface
RNDL_MAYBE_UNUSED void example_lines(rndl_surface_handle_t surface, const rndl_surface_config_t *surface_config) {
    ESP_LOGI(TAG, "angled lines");

    const rndl_color24_t color_off = {0};
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