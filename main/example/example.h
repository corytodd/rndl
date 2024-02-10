#pragma once

#include <rndl/graphics/surface.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Draw a pre-defined bitmap
 */
void example_bitmap(rndl_surface_handle_t surface, const rndl_surface_config_t *surface_config);

/**
 * Turn on the pixels of each corner of a surface to test the extent. The pixels will be turned on
 * in order: top-left, top-right, bottom-right, bottom-left, and repeat.
 */
void example_extent(rndl_surface_handle_t surface, const rndl_surface_config_t *surface_config);

/**
 * Draw angled lines on the surface
 */
void example_lines(rndl_surface_handle_t surface, const rndl_surface_config_t *surface_config);

/**
 * Draw random, concentric circles on the surface
 */
void example_raindrops(rndl_surface_handle_t surface, const rndl_surface_config_t *surface_config);

/**
 * Loop a shrinking rectangle forever
 */
void example_rectangle(rndl_surface_handle_t surface, const rndl_surface_config_t *surface_config);

#ifdef __cplusplus
}
#endif
