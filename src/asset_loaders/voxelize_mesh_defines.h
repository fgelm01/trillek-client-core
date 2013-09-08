#ifndef TRILLEK_VOXELIZE_MESH_DEFINES_H
#define	TRILLEK_VOXELIZE_MESH_DEFINES_H

#include "math/vector.h"
#include "data/voxel_octree.h"
#include "data/voxel_array.h"
#include <map>
#include <set>

namespace trillek {

typedef vector2d<float> float_vector2d;
typedef vector3d<float> float_vector3d;
typedef vector2d<std::size_t> size_vector2d;
typedef vector2d<signed long int> int_vector2d;
typedef vector3d<signed long int> int_vector3d;
typedef std::array<float_vector3d, 4> float_triangle3d;
typedef std::array<float_vector2d, 3> float_triangle2d;
typedef std::vector<float_triangle3d> triangle3d_vector;
typedef std::map<int_vector2d, std::vector<
        std::reference_wrapper<const float_triangle3d> > >
        bucket_map;
typedef std::vector<std::reference_wrapper<const bucket_map::value_type> >
        bucket_reference_vector;
}

#endif	/* TRILLEK_VOXELIZE_MESH_DEFINES_H */

