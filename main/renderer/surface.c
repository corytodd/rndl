#include "led_driver/led_driver.h"
#include "renderer/surface.h"

#include <esp_check.h>
#include <esp_err.h>
#include <esp_log.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static const char *TAG = "surface";

typedef struct {
    surface_t base;
    led_driver_handle_t led_driver;
    bool is_dirty;
    int width;
    int height;
    size_t buffer_size__bytes;
    uint8_t *buffer;
} internal_surface_t;

static esp_err_t surface_clear(surface_t *surface, const color_t *color) {
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(surface && color, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    internal_surface_t *internal_surface = __containerof(surface, internal_surface_t, base);

    if (color->red == 0 && color->green == 0 && color->blue == 0) {
        memset(internal_surface->buffer, 0, internal_surface->buffer_size__bytes);
    } else {
        for (int i = 0; i < internal_surface->buffer_size__bytes; i += sizeof(color_t)) {
            memcpy(&internal_surface->buffer[i], color, sizeof(color_t));
        }
    }
    internal_surface->is_dirty = true;
err:
    return ret;
}

static esp_err_t surface_fill(surface_t *surface, const point_t *p1, uint16_t w, uint16_t h, const color_t *color) {
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(surface && p1 && color, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    internal_surface_t *internal_surface = __containerof(surface, internal_surface_t, base);

    w = CLAMP(w, 0, internal_surface->width);
    w = MIN(internal_surface->width - p1->x, w);
    const uint16_t end_x = p1->x + w;

    h = CLAMP(h, 0, internal_surface->height);
    h = MIN(internal_surface->height - p1->y, h);
    const uint16_t end_y = p1->y + h;

    for (int x = p1->x; x < end_x; x++) {
        for (int y = p1->y; y < end_y; y++) {
            const point_t p2 = {.x = x, .y = y};
            int rindex = point_to_sraster(&p2, internal_surface->height);
            memcpy(&internal_surface->buffer[rindex * sizeof(color_t)], color, sizeof(color_t));
        }
    }
    internal_surface->is_dirty = true;
err:
    return ret;
}

static esp_err_t surface_render(surface_t *surface) {
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(surface, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument: surface is NULL");
    internal_surface_t *internal_surface = __containerof(surface, internal_surface_t, base);

    if (!internal_surface->is_dirty) {
        ESP_LOGD(TAG, "surface is not dirty, skip rendering");
        goto out;
    }
    ret = internal_surface->led_driver->write_blocking(internal_surface->led_driver, internal_surface->buffer,
                                                       internal_surface->buffer_size__bytes);
    internal_surface->is_dirty = false;

err:
out:
    return ret;
}

esp_err_t surface_create(const surface_config_t *config, led_driver_handle_t led_driver,
                         surface_handle_t *surface_handle) {
    esp_err_t ret = ESP_OK;
    internal_surface_t *internal_surface = NULL;
    ESP_GOTO_ON_FALSE(config && surface_handle, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");

    internal_surface = calloc(1, sizeof(internal_surface_t));
    ESP_GOTO_ON_FALSE(internal_surface, ESP_ERR_NO_MEM, err, TAG, "no mem for surface");

    internal_surface->base.clear = surface_clear;
    internal_surface->base.fill = surface_fill;
    internal_surface->base.render = surface_render;
    internal_surface->led_driver = led_driver;
    internal_surface->is_dirty = false;
    internal_surface->width = config->width;
    internal_surface->height = config->height;
    internal_surface->buffer_size__bytes = config->width * config->height * sizeof(color_t);
    internal_surface->buffer = calloc(1, internal_surface->buffer_size__bytes);
    ESP_GOTO_ON_FALSE(internal_surface->buffer, ESP_ERR_NO_MEM, err, TAG, "no mem for surface buffer");
    *surface_handle = &internal_surface->base;

    ESP_LOGD(TAG, "surface created: %d x %d", config->width, config->height);
err:
    return ret;
}