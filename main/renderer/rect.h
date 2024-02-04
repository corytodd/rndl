#pragma once

#include "color/color.h"
#include "renderer/point.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    point_t top_left;
    point_t bottom_right;
} rect_t;

typedef enum {
    RECT_FILL_STYLE_NONE,
    RECT_FILL_STYLE_SOLID,
} rect_fill_style_t;

typedef struct {
    rect_fill_style_t fill;
} rect_style_t;

#ifdef __cplusplus
}
#endif
