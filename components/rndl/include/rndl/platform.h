#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Platform delay definition
 * @param ms delay in milliseconds
 */
typedef void (*rndl_platform_delay_ms)(uint32_t ms);

#ifdef __cplusplus
}
#endif
