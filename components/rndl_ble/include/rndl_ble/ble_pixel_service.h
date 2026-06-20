#pragma once

#include "rndl/graphics/surface.h"

#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief BLE pixel service configuration
 */
typedef struct {
    rndl_surface_handle_t surface; /*!< Surface to apply received pixel writes to */
    const char *device_name;       /*!< BLE advertised device name */
} rndl_ble_service_config_t;

/**
 * @brief Start the BLE GATT server that receives batched pixel writes
 * @details
 * Exposes a custom GATT service with two characteristics:
 *  - Pixel write: accepts a flat array of 5-byte records [x][y][r][g][b], one
 *    record per pixel, decoded and applied to the surface on every write, followed
 *    by a single render.
 *  - Dimensions: read-only, returns [width_lo][width_hi][height_lo][height_hi] (u16 LE)
 *    so a client can discover panel size.
 * @param[in] config Service configuration
 * @return
 *      - ESP_ERR_INVALID_ARG for any invalid arguments
 *      - ESP_OK if the BLE stack was initialized and advertising was started
 */
esp_err_t rndl_ble_service_start(const rndl_ble_service_config_t *config);

#ifdef __cplusplus
}
#endif
