#pragma once

#include "rndl/led_driver/led_driver.h"

#include <esp_err.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of led panel driver configuration
 */
typedef struct {
    struct led_driver_config; /*!< Anonymous structure to inherit from led_driver_config_t */
} led_panel_driver_config_t;

/**
 * @brief Create LED panel driver
 * @param[in] config LED panel driver configuration
 * @param[out] handle Returned LED driver handle
 * @return ESP_OK on success
 */
esp_err_t led_panel_driver_new(const led_panel_driver_config_t *config, led_driver_handle_t *handle);

#ifdef __cplusplus
}
#endif
