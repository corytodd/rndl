#include <esp_log.h>
#include <esp_random.h>
#include <rndl/color.h>
#include <rndl/graphics/circle.h>
#include <rndl/graphics/surface.h>
#include <rndl/utils.h>
#include <stdbool.h>

static const char *TAG = "example";

// Create a random, valid circle
static void rnd_circle(const rndl_surface_config_t *surface_config, rndl_circle_t *circle) {
    esp_fill_random(circle, sizeof(rndl_circle_t));
    circle->center.x %= surface_config->width;
    circle->center.y %= surface_config->height;
    circle->radius %= RNDL_MAX(1, surface_config->width / 2);
}

// If more than half the circle is off the surface, return true
static bool shape_off_surface(const rndl_surface_config_t *surface_config, const rndl_circle_t *circle) {
    return (surface_config->width - circle->center.x < circle->radius &&
            surface_config->height - circle->center.y < circle->radius);
}

// Draw concentric circles on the surface
RNDL_MAYBE_UNUSED void example_raindrops(rndl_surface_handle_t surface, const rndl_surface_config_t *surface_config) {
    ESP_LOGI(TAG, "raindrops");

    const rndl_color24_t color_off = {0};
    rndl_hsv_t hsv = {.hue = 100, .saturation = 100, .value = 50};
    rndl_color24_t color = {0};

    // Generate concentric circles
    const int ripple_count = 3;
    rndl_circle_t circles[12] = {0};
    for (int i = 0; i < RNDL_COUNT_OF(circles); i += ripple_count) {
        rnd_circle(surface_config, &circles[i]);
        circles[i + 1].radius += 2;
        circles[i + 2].radius += 4;
    }

    while (1) {
        rndl_color_hsv2color24(&hsv, &color);

        surface->clear(surface, &color_off);

        for (int i = 0; i < RNDL_COUNT_OF(circles); ++i) {
            surface->draw_circle(surface, &circles[i], &color, NULL, NULL);
            circles[i].radius = (circles[i].radius + 1) % CONFIG_RNDL_COLS;

            // When a cirlce is more out of bounds than in bounds, generate a new one
            if (shape_off_surface(surface_config, &circles[i])) {
                rnd_circle(surface_config, &circles[i]);
                ESP_LOGI(TAG, "circle %d off surface", i);
            }
        }
        surface->render(surface);
        hsv.hue++;
    }
}
