/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "rndl/color.h"

#include <stdint.h>

void rndl_color_hsv2color24(const rndl_hsv_t *hsv, rndl_color24_t *color) {
    const uint32_t hue = hsv->hue % 360; // h -> [0,360]
    uint32_t rgb_max = hsv->value * 2.55f;
    uint32_t rgb_min = rgb_max * (100 - hsv->saturation) / 100.0f;

    uint32_t i = hue / 60;
    uint32_t diff = hue % 60;

    // RGB adjustment amount by hue
    uint32_t rgb_adj = (rgb_max - rgb_min) * diff / 60;

    switch (i) {
        case 0:
            color->red = rgb_max;
            color->green = rgb_min + rgb_adj;
            color->blue = rgb_min;
            break;
        case 1:
            color->red = rgb_max - rgb_adj;
            color->green = rgb_max;
            color->blue = rgb_min;
            break;
        case 2:
            color->red = rgb_min;
            color->green = rgb_max;
            color->blue = rgb_min + rgb_adj;
            break;
        case 3:
            color->red = rgb_min;
            color->green = rgb_max - rgb_adj;
            color->blue = rgb_max;
            break;
        case 4:
            color->red = rgb_min + rgb_adj;
            color->green = rgb_min;
            color->blue = rgb_max;
            break;
        default:
            color->red = rgb_max;
            color->green = rgb_min;
            color->blue = rgb_max - rgb_adj;
            break;
    }
}
