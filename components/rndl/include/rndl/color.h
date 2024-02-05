#pragma once

#include "rndl/utils.h"
#include "sdkconfig.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Color in 24-bits, GBR format
 */
typedef struct {
    uint8_t green;
    uint8_t blue;
    uint8_t red;
} RNDL_PACKED rndl_color24_t;

/**
 * @brief HSV container
 */
typedef struct {
    uint32_t hue;
    uint32_t saturation;
    uint32_t value;
} rndl_hsv_t;

/**
 * @brief Simple helper function, converting HSV color space to 24-bit color space
 *
 * Wiki: https://en.wikipedia.org/wiki/HSL_and_HSV
 *
 */
void rndl_color_hsv2color24(const rndl_hsv_t *hsv, rndl_color24_t *pixel);

#ifdef __cplusplus
}
#endif
