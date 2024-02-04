#pragma once

#include "utils.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t green;
    uint8_t blue;
    uint8_t red;
} PACKED color_t;

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
