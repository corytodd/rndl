#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Platform delay definition
 */
typedef void (*fn_delay_ms)(uint32_t ms);

/**
 * @brief Delay implementation for this platform
 * @param delay_ms Time to delay in milliseconds
 */
void platform_delay_ms(uint32_t delay_ms);

#ifdef __cplusplus
}
#endif
