#pragma once

#include "utils.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Point in 2D space
 * @details
 * Origin is at the top-left corner of the screen
 */
typedef struct {
    uint16_t x;
    uint16_t y;
} point_t;

#ifdef __cplusplus
}
#endif