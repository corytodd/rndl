#pragma once

#include "rndl/color.h"
#include "rndl/graphics/point.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    point_t start;
    point_t end;
} line_t;

typedef enum {
    LINE_STYLE_NONE,
    LINE_STYLE_SOLID,
} line_fill_style_t;

typedef struct {
    line_fill_style_t line_fill;
} line_style_t;

#ifdef __cplusplus
}
#endif
