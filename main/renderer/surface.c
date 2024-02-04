#include "led_driver/led_driver.h"
#include "renderer/surface.h"

#include <esp_check.h>
#include <esp_err.h>
#include <esp_log.h>
#include <math.h>
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

static esp_err_t surface_draw_line(surface_t *surface, const line_t *line, const line_style_t *line_style,
                                   const color_t *line_color) {
    UNUSED(line_style);
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(surface && line && line_color, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    internal_surface_t *internal_surface = __containerof(surface, internal_surface_t, base);

    point_t p1 = line->start;
    point_t p2 = line->end;
    int dx = abs(p2.x - p1.x);
    int dy = abs(p2.y - p1.y);
    int sx = p1.x < p2.x ? 1 : -1;
    int sy = p1.y < p2.y ? 1 : -1;
    int err = dx - dy;

    while (true) {
        const point_t point = {.x = p1.x, .y = p1.y};
        surface->draw_pixel(surface, &point, line_color);
        if (p1.x == p2.x && p1.y == p2.y) {
            break;
        }
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            p1.x += sx;
        }
        if (e2 < dx) {
            err += dx;
            p1.y += sy;
        }
    }
    internal_surface->is_dirty = true;

err:
    return ret;
}

static esp_err_t surface_draw_pixel(surface_t *surface, const point_t *point, const color_t *color) {
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(surface && point && color, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    internal_surface_t *internal_surface = __containerof(surface, internal_surface_t, base);

    uint16_t x = CLAMP(point->x, 0, internal_surface->width - 1);
    uint16_t y = CLAMP(point->y, 0, internal_surface->height - 1);

    int rindex =
        internal_surface->led_driver->point_to_index(internal_surface->led_driver, x, y, internal_surface->height);
    memcpy(&internal_surface->buffer[rindex * sizeof(color_t)], color, sizeof(color_t));
    internal_surface->is_dirty = true;
err:
    return ret;
}

static esp_err_t surface_draw_rect(surface_t *surface, const rect_t *rect, const rect_style_t *rect_style,
                                   const color_t *line_color) {
    UNUSED(rect_style);
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(surface && rect && line_color, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");

    // An unfilled rectangle is 4 lines
    const size_t width = abs(rect->bottom_right.x - rect->top_left.x);
    const size_t height = abs(rect->bottom_right.y - rect->top_left.y);

    line_t top = {
        .start = rect->top_left,
        .end =
            {
                .x = rect->top_left.x + width,
                .y = rect->top_left.y,
            },
    };
    line_t bottom = {
        .start =
            {
                .x = rect->top_left.x,
                .y = rect->top_left.y + height,
            },
        .end = rect->bottom_right,
    };
    line_t left = {
        .start = rect->top_left,
        .end =
            {
                .x = rect->top_left.x,
                .y = rect->top_left.y + height,
            },
    };
    line_t right = {
        .start =
            {
                .x = rect->top_left.x + width,
                .y = rect->top_left.y,
            },
        .end = rect->bottom_right,
    };

    ESP_ERROR_CHECK(surface->draw_line(surface, &top, &rect_style->line_style, line_color));
    ESP_ERROR_CHECK(surface->draw_line(surface, &bottom, &rect_style->line_style, line_color));
    ESP_ERROR_CHECK(surface->draw_line(surface, &left, &rect_style->line_style, line_color));
    ESP_ERROR_CHECK(surface->draw_line(surface, &right, &rect_style->line_style, line_color));
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
    internal_surface->base.draw_line = surface_draw_line;
    internal_surface->base.draw_pixel = surface_draw_pixel;
    internal_surface->base.draw_rect = surface_draw_rect;
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