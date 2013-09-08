#ifndef TRILLEK_VOXELIZE_RAY_TRACE_H
#define	TRILLEK_VOXELIZE_RAY_TRACE_H

#include "voxelize_mesh_defines.h"

namespace trillek {

voxel_octree voxelize_mesh_ray_trace(
        const triangle3d_vector& all_triangles, 
        const std::size_t magnify = 1, 
        const bool multi_axis = false);
/**
 * @brief Test if z ray at xy intersects triangle
 * @param triangle
 * @param xy
 * @param out Z position of intersection if any
 * @return true if intersected, false otherwise
 */
bool z_ray_intersection(const float_triangle3d& triangle, 
        const float_vector2d& xy, float& out);
bool y_ray_intersection(const float_triangle3d& triangle, 
        const float_vector2d& zx, float& out);
bool x_ray_intersection(const float_triangle3d& triangle, 
        const float_vector2d& yz, float& out);
int x_compute_winding(const float_triangle2d& triangle, 
        const float_vector2d& xy);
int y_compute_winding(const float_triangle2d& triangle, 
        const float_vector2d& xy);
bool consistent_point_in_triangle(const float_triangle2d& triangle, 
        const float_vector2d& xy);

}


#endif	/* TRILLEK_VOXELIZE_RAY_TRACE_H */

