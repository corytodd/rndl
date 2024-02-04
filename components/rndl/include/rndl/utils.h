#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define UNUSED(x)          (void)(x)
#define MAYBE_UNUSED       __attribute__((unused))
#define MAX(a, b)          ((a) > (b) ? (a) : (b))
#define MIN(a, b)          ((a) < (b) ? (a) : (b))
#define CLAMP(x, min, max) (MAX((min), MIN((max), (x))))
#define PACKED             __attribute__((packed))
#define KHZ(x)             ((x) * 1000)
#define MHZ(x)             ((x) * 1000000)
#define KB(x)              ((x) * 1024)
#define MB(x)              ((x) * 1024 * 1024)
#define FPS_TO_MS(fps)     (1000 / (fps))
#define COUNT_OF(x)        (sizeof(x) / sizeof(x[0]))

#ifdef __cplusplus
}
#endif
