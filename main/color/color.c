/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "color/color.h"

#include <stdint.h>

/**
 * @brief Simple helper function, converting HSV color space to RGB color space
 *
 * Wiki: https://en.wikipedia.org/wiki/HSL_and_HSV
 *
 */
void color_hsv2rgb(const hsv_t *hsv, color_t *pixel) {
    const uint32_t hue = hsv->hue % 360; // h -> [0,360]
    uint32_t rgb_max = hsv->value * 2.55f;
    uint32_t rgb_min = rgb_max * (100 - hsv->saturation) / 100.0f;

    uint32_t i = hue / 60;
    uint32_t diff = hue % 60;

    // RGB adjustment amount by hue
    uint32_t rgb_adj = (rgb_max - rgb_min) * diff / 60;

    switch (i) {
        case 0:
            pixel->red = rgb_max;
            pixel->green = rgb_min + rgb_adj;
            pixel->blue = rgb_min;
            break;
        case 1:
            pixel->red = rgb_max - rgb_adj;
            pixel->green = rgb_max;
            pixel->blue = rgb_min;
            break;
        case 2:
            pixel->red = rgb_min;
            pixel->green = rgb_max;
            pixel->blue = rgb_min + rgb_adj;
            break;
        case 3:
            pixel->red = rgb_min;
            pixel->green = rgb_max - rgb_adj;
            pixel->blue = rgb_max;
            break;
        case 4:
            pixel->red = rgb_min + rgb_adj;
            pixel->green = rgb_min;
            pixel->blue = rgb_max;
            break;
        default:
            pixel->red = rgb_max;
            pixel->green = rgb_min;
            pixel->blue = rgb_max - rgb_adj;
            break;
    }
}
