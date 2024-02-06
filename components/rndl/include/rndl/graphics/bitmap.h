#pragma once

#include "rndl/color.h"
#include "rndl/graphics/point.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Circle defined in 2D space
 */
typedef struct {
    uint16_t width;         /**< Width of the bitmap */
    uint16_t height;        /**< Height of the bitmap */
    rndl_color24_t *pixels; /**< Pointer to the pixel data */
} rndl_bitmap24_t;

#ifdef __cplusplus
}
#endif
