#pragma once

#include "rndl/color.h"
#include "rndl/graphics/bitmap.h"
#include "rndl/graphics/circle.h"
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
typedef struct rndl_surface_t rndl_surface_t;
typedef struct rndl_surface_t *rndl_surface_handle_t;
/** @endcond */

typedef enum {
    RNDL_FILL_SOLID, /*!< Fill the entire area with the color */
} rndfl_fill_type_t;

typedef enum {
    RNDL_LINE_TYPE_SOLID, /*!< Solid line */
} rndl_line_style_t;

typedef struct {
    rndfl_fill_type_t fill_type;  /*!< Fill type */
    rndl_line_style_t line_style; /*!< Line style */
} rndl_style_t;

/**
 * @brief Surface is a 2D canvas to draw on
 */
struct rndl_surface_t {

    /**
     * @brief Draw a bitmap
     * @param surface Surface instance
     * @param bitmap Bitmap to draw
     * @param position Position to draw the bitmap
     * @return
     *     - ESP_ERR_INVALID_ARG for any invalid arguments
     *    - ESP_OK if bitmap written to surface
     */
    esp_err_t (*draw_bitmap)(rndl_surface_t *surface, const rndl_bitmap24_t *bitmap, const rndl_point_t *position);

    /**
     * @brief Clear the surface with a color
     * @param surface Surface instance
     * @param color Fill entire surface with this color
     * @return
     *      - ESP_ERR_INVALID_ARG for any invalid arguments
     *      - ESP_OK if color written to surface
     */
    esp_err_t (*clear)(rndl_surface_t *surface, const rndl_color24_t *color);

    /**
     * @brief Draw a circle
     * @param surface Surface instance
     * @param circle Circle to draw
     * @param line_color Line color
     * @param style Style of circle, optional
     * @param fill_color Fill color, optional
     * @return
     *      - ESP_ERR_INVALID_ARG for any invalid arguments
     *      - ESP_OK if circle written to surface
     */
    esp_err_t (*draw_circle)(rndl_surface_t *surface, const rndl_circle_t *circle, const rndl_color24_t *line_color,
                             const rndl_style_t *style, const rndl_color24_t *fill_color);

    /**
     * @brief Draw a line
     * @param surface Surface instance
     * @param line Line to draw
     * @param line_color Line color
     * @param line_style Style of line, optional
     * @return
     *      - ESP_ERR_INVALID_ARG for any invalid arguments
     *      - ESP_OK if line written to surface
     */
    esp_err_t (*draw_line)(rndl_surface_t *surface, const rndl_line_t *line, const rndl_color24_t *line_color,
                           const rndl_style_t *style);

    /**
     * @brief Draw a pixel
     * @param surface Surface instance
     * @param point Point to draw
     * @param color Pixel color
     * @return
     *      - ESP_ERR_INVALID_ARG for any invalid arguments
     *      - ESP_OK if pixel written to surface
     */
    esp_err_t (*draw_pixel)(rndl_surface_t *surface, const rndl_point_t *point, const rndl_color24_t *color);

    /**
     * @brief Draw a rectangle
     * @param surface Surface instance
     * @param rect Rectangle to draw
     * @param line_color Outline color
     * @param rect_style Style of rectangle, optional
     * @param fill_color Fill color, optional
     * @return
     *      - ESP_ERR_INVALID_ARG for any invalid arguments
     *      - ESP_OK if rectange written to surface
     */
    esp_err_t (*draw_rect)(rndl_surface_t *surface, const rndl_rect_t *rect, const rndl_color24_t *line_color,
                           const rndl_style_t *style, const rndl_color24_t *fill_color);

    /**
     * @brief Render the surface into the LED driver
     * @param surface Surface instance
     * @return
     *      - ESP_ERR_INVALID_ARG for any invalid arguments
     *      - ESP_OK if render started successfully TODO: blocking vs. non-blocking render mode
     */
    esp_err_t (*render)(rndl_surface_t *surface);
};

/**
 * @brief Surface configuration
 */
typedef struct {
    uint16_t width;  /*!< Width of the surface in pixels*/
    uint16_t height; /*!< Height of the surface in pixels*/
} rndl_surface_config_t;

/**
 * @brief Create a surface
 * @param[in] config Surface configuration
 * @param[in] led_driver LED driver handle
 * @param[out] handle Returned surface handle
 * @return
 *      - ESP_ERR_INVALID_ARG for any invalid arguments
 *      - ESP_ERR_NO_MEM out of memory when creating surface
 *      - ESP_OK if creating encoder successfully
 */
esp_err_t rndl_surface_create(const rndl_surface_config_t *config, rndl_led_driver_handle_t led_driver,
                              rndl_surface_handle_t *handle);

#ifdef __cplusplus
}
#endif
