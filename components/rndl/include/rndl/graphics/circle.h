#pragma once

#include "rndl/color.h"
#include "rndl/graphics/point.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    rndl_point_t center;
    uint16_t radius;
} rndl_circle_t;

#ifdef __cplusplus
}
#endif
