#pragma once

#include "platform.h"

#include <esp_err.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @cond */
typedef struct led_driver_t led_driver_t;
typedef struct led_driver_t *led_driver_handle_t;
/** @endcond */

/**
 * @brief LED driver interface
 */
struct led_driver_t {
    /**
     * @brief Write data to the LED driver
     * @param driver LED driver instance
     * @param data Data to write
     * @param size__bytes Size of data to write
     * @param timeout__ms Timeout in milliseconds
     * @return ESP_OK on success
     */
    esp_err_t (*write_blocking)(led_driver_t *driver, const void *data, size_t size__bytes);
};

typedef struct led_driver_config {
    uint16_t gpio_num;      /*!< GPIO number */
    uint32_t led_count;     /*!< Number of LEDs */
    uint32_t resolution_hz; /*!< Resolution in Hz */
    uint32_t frame_time_ms; /*!< Time in milliseconds to hold each frame */
    fn_delay_ms fn_delay;   /*!< Delay function */
    int timeout_ms;         /*!< Timeout in milliseconds for blocking operations*/
} led_driver_config_t;

#ifdef __cplusplus
}
#endif
