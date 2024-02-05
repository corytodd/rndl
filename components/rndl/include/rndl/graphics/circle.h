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
    rndl_point_t center;
    uint16_t radius; /*!< Radius of the circle in pixels */
} rndl_circle_t;

#ifdef __cplusplus
}
#endif
