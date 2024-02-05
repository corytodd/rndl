#include "rndl/graphics/surface.h"
#include "rndl/led_driver/led_driver.h"

#include <esp_check.h>
#include <esp_err.h>
#include <esp_log.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static const char *TAG = "rndl_surface";

typedef struct {
    rndl_surface_t base;
    rndl_led_driver_handle_t led_driver;
    bool is_dirty;
    uint16_t width;
    uint16_t height;
    size_t buffer_size__bytes;
    uint8_t *buffer;
} internal_surface_t;

static esp_err_t surface_clear(rndl_surface_t *surface, const rndl_color24_t *color) {
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(surface && color, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    internal_surface_t *internal_surface = __containerof(surface, internal_surface_t, base);

    // TODO: is this optimization actually doing anything? Maybe we should aways loop through the buffer
    // and let the compiler decide if it's worth optimizing.
    if (color->red == 0 && color->green == 0 && color->blue == 0) {
        memset(internal_surface->buffer, 0, internal_surface->buffer_size__bytes);
    } else {
        for (int i = 0; i < internal_surface->buffer_size__bytes; i += sizeof(rndl_color24_t)) {
            memcpy(&internal_surface->buffer[i], color, sizeof(rndl_color24_t));
        }
    }
    internal_surface->is_dirty = true;
    goto out;

err:
out:
    return ret;
}

static esp_err_t surface_draw_circle(rndl_surface_t *surface, const rndl_circle_t *circle,
                                     const rndl_color24_t *line_color, const rndl_style_t *style,
                                     const rndl_color24_t *fill_color) {
    RNDL_UNUSED(style);
    RNDL_UNUSED(fill_color);
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(surface && circle && line_color, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    internal_surface_t *internal_surface = __containerof(surface, internal_surface_t, base);

    // Jesko's Method
    // https://en.wikipedia.org/wiki/Midpoint_circle_algorithm#Jesko's_Method
    int x = circle->radius;
    int y = 0;
    int r_err = 1 - x;

    while (x >= y) {
        const rndl_point_t points[] = {
            {.x = circle->center.x + x, .y = circle->center.y + y},
            {.x = circle->center.x + y, .y = circle->center.y + x},
            {.x = circle->center.x - y, .y = circle->center.y + x},
            {.x = circle->center.x - x, .y = circle->center.y + y},
            {.x = circle->center.x - x, .y = circle->center.y - y},
            {.x = circle->center.x - y, .y = circle->center.y - x},
            {.x = circle->center.x + y, .y = circle->center.y - x},
            {.x = circle->center.x + x, .y = circle->center.y - y},
        };
        for (int i = 0; i < sizeof(points) / sizeof(rndl_point_t); ++i) {
            surface->draw_pixel(surface, &points[i], line_color);
        }
        y++;
        if (r_err < 0) {
            r_err += 2 * y + 1;
        } else {
            x--;
            r_err += 2 * (y - x + 1);
        }
    }

    // TODO: handle color fill
    // TODO: handle line_style

    internal_surface->is_dirty = true;
    goto out;

err:
out:
    return ret;
}

static esp_err_t surface_draw_line(rndl_surface_t *surface, const rndl_line_t *line, const rndl_color24_t *line_color,
                                   const rndl_style_t *style) {
    RNDL_UNUSED(style);
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(surface && line && line_color, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    internal_surface_t *internal_surface = __containerof(surface, internal_surface_t, base);

    // Integer Bresenham's line algorithm
    rndl_point_t p1 = line->start;
    rndl_point_t p2 = line->end;
    int dx = abs(p2.x - p1.x);
    int dy = abs(p2.y - p1.y);
    int sx = p1.x < p2.x ? 1 : -1;
    int sy = p1.y < p2.y ? 1 : -1;
    int err = dx - dy;

    while (true) {
        const rndl_point_t point = {.x = p1.x, .y = p1.y};
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

    // TODO: handle style

    internal_surface->is_dirty = true;
    goto out;

err:
out:
    return ret;
}

static esp_err_t surface_draw_pixel(rndl_surface_t *surface, const rndl_point_t *point, const rndl_color24_t *color) {
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(surface && point && color, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    internal_surface_t *internal_surface = __containerof(surface, internal_surface_t, base);

    // TODO: we could add a wrapping feature
    if (point->x >= internal_surface->width || point->y >= internal_surface->height) {
        goto out;
    }
    int rindex = internal_surface->led_driver->point_to_index(internal_surface->led_driver, point->x, point->y,
                                                              internal_surface->height);
    rindex = rindex * sizeof(rndl_color24_t);

    ESP_GOTO_ON_ERROR(rindex < 0 || rindex >= internal_surface->buffer_size__bytes ? ESP_ERR_INVALID_ARG : ESP_OK, err,
                      TAG, "invalid index: %d", rindex);

    memcpy(&internal_surface->buffer[rindex], color, sizeof(rndl_color24_t));
    internal_surface->is_dirty = true;
    goto out;

err:
out:
    return ret;
}

static esp_err_t surface_draw_rect(rndl_surface_t *surface, const rndl_rect_t *rect, const rndl_color24_t *line_color,
                                   const rndl_style_t *style, const rndl_color24_t *fill_color) {
    RNDL_UNUSED(fill_color);
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(surface && rect && line_color, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    internal_surface_t *internal_surface = __containerof(surface, internal_surface_t, base);

    // An unfilled rectangle is 4 lines
    const size_t width = abs(rect->bottom_right.x - rect->top_left.x);
    const size_t height = abs(rect->bottom_right.y - rect->top_left.y);

    rndl_line_t top = {
        .start = rect->top_left,
        .end =
            {
                .x = rect->top_left.x + width,
                .y = rect->top_left.y,
            },
    };
    rndl_line_t bottom = {
        .start =
            {
                .x = rect->top_left.x,
                .y = rect->top_left.y + height,
            },
        .end = rect->bottom_right,
    };
    rndl_line_t left = {
        .start = rect->top_left,
        .end =
            {
                .x = rect->top_left.x,
                .y = rect->top_left.y + height,
            },
    };
    rndl_line_t right = {
        .start =
            {
                .x = rect->top_left.x + width,
                .y = rect->top_left.y,
            },
        .end = rect->bottom_right,
    };

    ESP_GOTO_ON_ERROR(surface->draw_line(surface, &top, line_color, style), err, TAG, "draw top line failed");
    ESP_GOTO_ON_ERROR(surface->draw_line(surface, &bottom, line_color, style), err, TAG, "draw bottom line failed");
    ESP_GOTO_ON_ERROR(surface->draw_line(surface, &left, line_color, style), err, TAG, "draw left line failed");
    ESP_GOTO_ON_ERROR(surface->draw_line(surface, &right, line_color, style), err, TAG, "draw right line failed");

    // TODO: handle color fill
    // TODO: handle line_style

    internal_surface->is_dirty = true;
    goto out;

err:
out:
    return ret;
}

static esp_err_t surface_render(rndl_surface_t *surface) {
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(surface, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument: surface is NULL");
    internal_surface_t *internal_surface = __containerof(surface, internal_surface_t, base);

    if (!internal_surface->is_dirty) {
        ESP_LOGD(TAG, "surface is not dirty, skip rendering");
        goto out;
    }

    // TODO: implement non-blocking write
    ret = internal_surface->led_driver->write_blocking(internal_surface->led_driver, internal_surface->buffer,
                                                       internal_surface->buffer_size__bytes);
    internal_surface->is_dirty = false;
    goto out;

err:
out:
    return ret;
}

esp_err_t rndl_surface_create(const rndl_surface_config_t *config, rndl_led_driver_handle_t led_driver,
                              rndl_surface_handle_t *surface_handle) {
    esp_err_t ret = ESP_OK;
    internal_surface_t *internal_surface = NULL;
    ESP_GOTO_ON_FALSE(config && surface_handle, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");

    internal_surface = calloc(1, sizeof(internal_surface_t));
    ESP_GOTO_ON_FALSE(internal_surface, ESP_ERR_NO_MEM, err, TAG, "no mem for surface");

    internal_surface->base.clear = surface_clear;
    internal_surface->base.draw_circle = surface_draw_circle;
    internal_surface->base.draw_line = surface_draw_line;
    internal_surface->base.draw_pixel = surface_draw_pixel;
    internal_surface->base.draw_rect = surface_draw_rect;
    internal_surface->base.render = surface_render;
    internal_surface->led_driver = led_driver;
    internal_surface->is_dirty = false;
    internal_surface->width = config->width;
    internal_surface->height = config->height;
    internal_surface->buffer_size__bytes = config->width * config->height * sizeof(rndl_color24_t);
    internal_surface->buffer = calloc(1, internal_surface->buffer_size__bytes);
    ESP_GOTO_ON_FALSE(internal_surface->buffer, ESP_ERR_NO_MEM, err, TAG, "no mem for surface buffer");
    *surface_handle = &internal_surface->base;

    ESP_LOGD(TAG, "surface created: %d x %d (buffer %d bytes)", config->width, config->height,
             internal_surface->buffer_size__bytes);
    goto out;

err:
    if (internal_surface) {
        if (internal_surface->buffer) {
            free(internal_surface->buffer);
        }
        free(internal_surface);
    }
out:
    return ret;
}
