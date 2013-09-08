#ifndef TRILLEK_VOXELIZE_SURFACE_H
#define	TRILLEK_VOXELIZE_SURFACE_H

#include "voxelize_mesh_defines.h"

namespace trillek {

/**
 * @brief Voxelize only the surface of the mesh
 * @param all_triangles all the triangles
 * @param voxels_per_unit number of voxels per unit of distance in the mesh
 * Smaller number means bigger voxels and fewer of them. Low detail.
 * Larger number means smaller voxels and more of them. High detail.
 * @return voxel_octree containing voxelized surface
 */
voxel_octree voxelize_mesh_surface(const triangle3d_vector& all_triangles, 
        const float voxels_per_unit);

}

#endif	/* TRILLEK_VOXELIZE_SURFACE_H */

