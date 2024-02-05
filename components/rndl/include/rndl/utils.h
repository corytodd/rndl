#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define RNDL_UNUSED(x)          (void)(x)
#define RNDL_MAYBE_UNUSED       __attribute__((unused))
#define RNDL_MAX(a, b)          ((a) > (b) ? (a) : (b))
#define RNDL_MIN(a, b)          ((a) < (b) ? (a) : (b))
#define RNDL_CLAMP(x, min, max) (RNDL_MAX((min), RNDL_MIN((max), (x))))
#define RNDL_PACKED             __attribute__((packed))
#define RNDL_KHZ(x)             ((x) * 1000)
#define RNDL_MHZ(x)             ((x) * 1000000)
#define RNDL_KB(x)              ((x) * 1024)
#define RNDL_MB(x)              ((x) * 1024 * 1024)
#define RNDL_FPS_TO_MS(fps)     (1000 / (fps))
#define RNDL_COUNT_OF(x)        (sizeof(x) / sizeof(x[0]))

#ifdef __cplusplus
}
#endif
