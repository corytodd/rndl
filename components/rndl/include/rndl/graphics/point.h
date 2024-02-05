#pragma once

#include "rndl/utils.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Point in 2D space
 */
typedef struct {
    uint16_t x; /*!< X coordinate in pixels*/
    uint16_t y; /*!< Y coordinate in pixels*/
} rndl_point_t;

#ifdef __cplusplus
}
#endif
