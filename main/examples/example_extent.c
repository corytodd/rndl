#include <esp_log.h>
#include <rndl/color.h>
#include <rndl/graphics/point.h>
#include <rndl/graphics/surface.h>
#include <rndl/utils.h>

static const char *TAG = "example";

// Draw corners points on extend of surface
RNDL_MAYBE_UNUSED void example_extent(rndl_surface_handle_t surface, const rndl_surface_config_t *surface_config) {
    ESP_LOGI(TAG, "surface extent");

    const rndl_color24_t color_off = {0};
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
