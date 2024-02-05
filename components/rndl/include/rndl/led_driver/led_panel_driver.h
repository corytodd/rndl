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
    rndl_led_driver_config_t; /*!< Anonymous structure to inherit from led_driver_config_t */
} rndl_led_panel_driver_config_t;

/**
 * @brief Create LED panel driver
 * @param[in] config LED panel driver configuration
 * @param[out] handle Returned LED driver handle
 * @return
 *      - ESP_ERR_INVALID_ARG for any invalid arguments
 *      - ESP_ERR_NO_MEM out of memory when creating driver
 *      - ESP_OK if creating driver successfully
 */
esp_err_t rndl_led_panel_driver_new(const rndl_led_panel_driver_config_t *config, rndl_led_driver_handle_t *handle);

#ifdef __cplusplus
}
#endif
