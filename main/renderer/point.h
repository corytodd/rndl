#pragma once

#include "utils.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t x;
    uint16_t y;
} point_t;

/**
 * @brief Convert a point to a serpentine raster index
 */
static inline int point_to_sraster(const point_t *point, size_t rows) {
    if (point->x % 2 == 0) {
        return point->x * rows + point->y;
    } else {
        return (point->x * rows) + (rows - 1 - point->y);
    }
}

#ifdef __cplusplus
}
#endif
