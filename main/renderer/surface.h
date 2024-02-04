#pragma once

#include "color/color.h"
#include "led_driver/led_driver.h"
#include "renderer/point.h"

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
     * @return ESP_ERR_INVALID_ARG when surface or color is NULL
     */
    esp_err_t (*clear)(surface_t *surface, const color_t *color);

    /**
     * @brief Fill a rectangle with a color
     * @param surface Surface instance
     * @param p1 Top-left corner of the rectangle
     * @param w Width of the rectangle
     * @param h Height of the rectangle
     * @param color Color to fill the rectangle with
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_ARG when surface or color is NULL or
     * if width or height exceeds surface dimensions
     */
    esp_err_t (*fill)(surface_t *surface, const point_t *p1, uint16_t w, uint16_t h, const color_t *color);

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
