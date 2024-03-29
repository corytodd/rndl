#pragma once

#include "rndl/platform.h"

#include <esp_err.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @cond */
typedef struct rndl_led_driver_t rndl_led_driver_t;
typedef struct rndl_led_driver_t *rndl_led_driver_handle_t;
/** @endcond */

/**
 * @brief LED driver interface
 */
struct rndl_led_driver_t {

    /**
     * @brief Convert point to a linear index
     * @details
     * Caller is responsible for bounds checking the result
     * @param driver LED driver instance
     * @param x X coordinate
     * @param y Y coordinate
     * @param y_max Maximum Y coordinate
     * @return Linear index. A negative value means one or both dimensions overflowed.
     */
    int (*point_to_index)(rndl_led_driver_t *driver, uint16_t x, uint16_t y, size_t y_max);

    /**
     * @brief Write data to the LED driver
     * @param driver LED driver instance
     * @param data Data to write
     * @param size__bytes Size of data to write
     * @return ESP_OK on success
     */
    esp_err_t (*write)(rndl_led_driver_t *driver, const void *data, size_t size__bytes);
};

typedef struct led_driver_config {
    uint16_t gpio_num;      /*!< GPIO number */
    uint32_t led_count;     /*!< Number of LEDs */
    uint32_t resolution_hz; /*!< Resolution in Hz */
    int timeout_ms;         /*!< Timeout in milliseconds for blocking operations*/
} rndl_led_driver_config_t;

#ifdef __cplusplus
}
#endif
