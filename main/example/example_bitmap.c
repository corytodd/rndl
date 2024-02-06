#include "sdkconfig.h"

#include <esp_log.h>
#include <esp_random.h>
#include <rndl/color.h>
#include <rndl/graphics/circle.h>
#include <rndl/graphics/surface.h>
#include <rndl/utils.h>
#include <stdbool.h>

static const char *TAG = "example";

const extern rndl_bitmap24_t bitmap_rndl;

// Draw a pre-defined bitmap
RNDL_MAYBE_UNUSED void example_bitmap(rndl_surface_handle_t surface, const rndl_surface_config_t *surface_config) {
    ESP_LOGI(TAG, "bitmap");

    const rndl_point_t origin = {0};
    const rndl_color24_t off = {0};

    surface->clear(surface, &off);
    surface->draw_bitmap(surface, &bitmap_rndl, &origin);
}
