#pragma once

#include "rndl/color.h"
#include "rndl/graphics/point.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Line in 2D space
 */
typedef struct {
    rndl_point_t start;
    rndl_point_t end;
} rndl_line_t;

#ifdef __cplusplus
}
#endif
