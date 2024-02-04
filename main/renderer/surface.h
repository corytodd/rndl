#pragma once

#include "color/color.h"
#include "led_driver/led_driver.h"
#include "renderer/point.h"
#include "renderer/rect.h"

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
     * @param color Color to clear the surface with
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_ARG
     */
    esp_err_t (*clear)(surface_t *surface, const color_t *color);

    /**
     * @brief Draw a rectangle
     * @param surface Surface instance
     * @param rect Rectangle to draw
     * @param rect_style Style of rectangle
     * @param color Color to fill the rectangle with
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_ARG
     */
    esp_err_t (*draw_rect)(surface_t *surface, const rect_style_t *rect, const rect_style_t *rect_style);

    /**
     * @brief Render the surface into the LED driver
     * @param surface Surface instance
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_ARG when surface is NULL
     */
    esp_err_t (*render)(surface_t *surface);
};

/**
 * @brief Surface configuration
 */
typedef struct {
    int width;  /*!< Width of the surface in pixels*/
    int height; /*!< Height of the surface in pixels*/
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
