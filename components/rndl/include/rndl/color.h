#pragma once

#include "rndl/utils.h"
#include "sdkconfig.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Color of a single pixel
 * @details
 * By definition, a pixel is represented in this order
 * as exactly CONFIG_LED_CHANNELS * 8 bits.
 */
typedef struct {
    union {
        struct {
            uint8_t green;
            uint8_t blue;
            uint8_t red;
        };
        // TODO: switch to GRB named type
        uint8_t raw[3];
    };
} PACKED color_t;

/**
 * @brief HSV container
 */
typedef struct {
    uint32_t hue;
    uint32_t saturation;
    uint32_t value;
} hsv_t;

/**
 * @brief Convert HSV color space to RGB color space
 *
 * Wiki: https://en.wikipedia.org/wiki/HSL_and_HSV
 *
 */
void color_hsv2rgb(const hsv_t *hsv, color_t *pixel);

#ifdef __cplusplus
}
#endif
