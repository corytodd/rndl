#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define MAX(a, b)          ((a) > (b) ? (a) : (b))
#define MIN(a, b)          ((a) < (b) ? (a) : (b))
#define CLAMP(x, min, max) (MAX((min), MIN((max), (x))))
#define PACKED             __attribute__((packed))

#ifdef __cplusplus
}
#endif
