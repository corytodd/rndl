#pragma once

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
    esp_err_t (*led_driver_write_blocking)(led_driver_t *driver, const void *data, size_t size__bytes,
                                           uint32_t timeout__ms);
};

#ifdef __cplusplus
}
#endif
