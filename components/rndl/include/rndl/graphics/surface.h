#pragma once

#include "rndl/color.h"
#include "rndl/graphics/line.h"
#include "rndl/graphics/point.h"
#include "rndl/graphics/rect.h"
#include "rndl/led_driver/led_driver.h"

#include <esp_err.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @cond */
typedef struct surface_t surface_t;
typedef struct surface_t *surface_handle_t;
/** @endcond */

struct surface_t {
    /**
     * @brief Clear the surface with a color
     * @param surface Surface instance
     * @param color Fill entire surface with this color
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_ARG
     */
    esp_err_t (*clear)(surface_t *surface, const color_t *color);

    /**
     * @brief Draw a circle
     * @param surface Surface instance
     * @param center Center of the circle
     * @param radius Radius of the circle in pixels
     * @param line_style Style of line, optional
     * @param line_color Line color
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_ARG
     */
    esp_err_t (*draw_circle)(surface_t *surface, const point_t *center, uint16_t radius, const line_style_t *line_style,
                             const color_t *line_color);

    /**
     * @brief Draw a line
     * @param surface Surface instance
     * @param line Line to draw
     * @param line_style Style of line, optional
     * @param line_color Line color
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_ARG
     */
    esp_err_t (*draw_line)(surface_t *surface, const line_t *line, const line_style_t *line_style,
                           const color_t *line_color);

    /**
     * @brief Draw a pixel
     * @param surface Surface instance
     * @param point Point to draw
     * @param color Pixel color
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_ARG
     */
    esp_err_t (*draw_pixel)(surface_t *surface, const point_t *point, const color_t *color);

    /**
     * @brief Draw a rectangle
     * @param surface Surface instance
     * @param rect Rectangle to draw
     * @param rect_style Style of rectangle, optional
     * @param line_color Outline color
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_ARG
     */
    esp_err_t (*draw_rect)(surface_t *surface, const rect_t *rect, const rect_style_t *rect_style,
                           const color_t *line_color);

    /**
     * @brief Render the surface into the LED driver
     * @param surface Surface instance
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_ARG
     */
    esp_err_t (*render)(surface_t *surface);
};

/**
 * @brief Surface configuration
 */
typedef struct {
    uint16_t width;  /*!< Width of the surface in pixels*/
    uint16_t height; /*!< Height of the surface in pixels*/
} surface_config_t;

/**
 * @brief Create a surface
 * @param[in] config Surface configuration
 * @param[in] led_driver LED driver handle
 * @param[out] handle Returned surface handle
 */
esp_err_t surface_create(const surface_config_t *config, led_driver_handle_t led_driver, surface_handle_t *handle);

#ifdef __cplusplus
}
#endif
