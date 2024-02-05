#pragma once

#include "rndl/color.h"
#include "rndl/graphics/line.h"
#include "rndl/graphics/point.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Rectangle defined in 2D space
 */
typedef struct {
    rndl_point_t top_left;
    rndl_point_t bottom_right;
} rndl_rect_t;

#ifdef __cplusplus
}
#endif
