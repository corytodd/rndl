#pragma once

#include "color/color.h"
#include "renderer/point.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    point_t start;
    point_t end;
} line_t;

typedef struct {
} line_style_t;

#ifdef __cplusplus
}
#endif
