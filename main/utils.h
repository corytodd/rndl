#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX(a, b)          ((a) > (b) ? (a) : (b))
#define MIN(a, b)          ((a) < (b) ? (a) : (b))
#define CLAMP(x, min, max) (MAX((min), MIN((max), (x))))
#define PACKED             __attribute__((packed))

typedef void (*fn_delay_ms)(uint32_t ms);

#ifdef __cplusplus
}
#endif
