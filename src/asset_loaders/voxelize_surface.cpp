#include "voxelize_mesh_defines.h"
#include "voxelize_surface.h"
#include "voxelize_mesh_utils.h"
#include <functional>
#include <algorithm>
#include <cassert>
#include <omp.h>

namespace trillek {

typedef std::vector<std::reference_wrapper<const float_triangle3d> >
        triangle_reference_vector;

class converter {
public:
    converter(const float_vector3d& mo, const voxel_data::size_vector3d& vo, 
            const float scale) : m_mesh_origin(mo), m_voxel_origin(vo), 
            m_scale(scale) {}
    voxel_data::size_vector3d operator ()(const float_vector3d& arg) const {
        return voxel_octree::size_vector3d(m_scale * (arg - m_mesh_origin)) - 
                m_voxel_origin;
    }
    float_vector3d operator ()(const voxel_data::size_vector3d& arg) const {
        return m_mesh_origin + (float_vector3d(arg + m_voxel_origin) 
                / m_scale);
    }
    const float_vector3d m_mesh_origin;
    const voxel_data::size_vector3d m_voxel_origin;
    const float m_scale;
};

void voxelize_triangles(voxel_octree& output, 
        const voxel_octree::size_vector3d& size_min, 
        const voxel_octree::size_vector3d& size_max, 
        const float_vector3d& output_min, 
        const float_vector3d& output_max, 
        const triangle_reference_vector& input, 
        const float_vector3d::value_type threshold, 
        const std::size_t reclevel = 0);

voxel_octree voxelize_mesh_surface(const triangle3d_vector& all_triangles, 
        const float voxels_per_unit) {
    const std::size_t threshold_voxels = 1;
    const float_vector3d::value_type threshold_units = 
            (1 + threshold_voxels) / voxels_per_unit;
    float_vector3d min_xyz(MAX_VAL, MAX_VAL, MAX_VAL);
    float_vector3d max_xyz(MIN_VAL, MIN_VAL, MIN_VAL);
    for(const float_triangle3d& triangle : all_triangles) {
        for(std::size_t i = 0; i < 3; ++i) {
            const float_vector3d& cur_vertex = triangle[i];
            min_xyz.x = std::min(min_xyz.x, cur_vertex.x);
            min_xyz.y = std::min(min_xyz.y, cur_vertex.y);
            min_xyz.z = std::min(min_xyz.z, cur_vertex.z);
            max_xyz.x = std::max(max_xyz.x, cur_vertex.x);
            max_xyz.y = std::max(max_xyz.y, cur_vertex.y);
            max_xyz.z = std::max(max_xyz.z, cur_vertex.z);
        }
    }
    min_xyz -= make_vector3d(voxels_per_unit, 
            voxels_per_unit, voxels_per_unit) * (threshold_units);
    max_xyz += make_vector3d(voxels_per_unit, 
            voxels_per_unit, voxels_per_unit) * (threshold_units);
    converter convert(min_xyz, voxel_octree::size_vector3d(0,0,0), 
            voxels_per_unit);
    voxel_octree::size_vector3d msize = convert(max_xyz) + 
            make_vector3d(1,1,1) * (1 + threshold_voxels);
    voxel_octree ret;
    ret.reserve_space(msize);
    std::cerr << "Input size: " << msize << std::endl;
    std::cerr << "Octree size: " << ret.get_size() << std::endl;
    triangle_reference_vector input;
    for(const float_triangle3d& triangle : all_triangles) {
        input.push_back(std::ref(triangle));
    }
    //voxelize_triangles(ret, input, convert);
    voxelize_triangles(ret, voxel_octree::size_vector3d(0,0,0), 
            ret.get_size(), min_xyz, 
            min_xyz + ret.get_size() / voxels_per_unit, 
            input, threshold_units);
    const std::size_t tree_nodes = ret.get_num_nodes();
    const std::size_t tree_volume = ret.get_opaque_volume();
    const std::size_t tree_height = ret.get_height();
    const float ratio = float(tree_nodes) / tree_volume;
    std::cerr << "Octree nodes:  " << tree_nodes << std::endl;
    std::cerr << "Octree volume: " << tree_volume << std::endl;
    std::cerr << "Octree height: " << tree_height << std::endl;
    std::cerr << "Compression:   " << ratio << std::endl;
    std::cerr << "Compression less than 1.0 is good. Greater is bad" 
            << std::endl;
    return ret;
}

void voxelize_triangles(voxel_octree& output, 
        const voxel_octree::size_vector3d& size_min, 
        const voxel_octree::size_vector3d& size_max, 
        const float_vector3d& output_min, 
        const float_vector3d& output_max, 
        const triangle_reference_vector& input, 
        const float_vector3d::value_type threshold, 
        const std::size_t reclevel) {
    auto relevant_triangle = [&output_min, &output_max, &threshold](
            const float_triangle3d& arg)->bool {
        return triangle_in_cube_threshold(arg, output_min, 
                output_max, threshold);
        //return triangle_in_cube3(arg, output_min, output_max);
    };
    const voxel_octree::size_vector3d delta_size = size_max - size_min;
    triangle_reference_vector actual_input;
    std::copy_if(input.begin(), input.end(), 
            std::back_inserter(actual_input), relevant_triangle);
    if(actual_input.empty()) {
        return;
    } else if(delta_size.x == delta_size.y && 
            delta_size.y == delta_size.z && 
            delta_size.z == 1) {
#pragma omp critical(voxelize_triangles_crit)
        output.set_voxel(size_min, voxel(true, true));
    } else {
        const float_vector3d half_real = (output_max - output_min) / 2;
        const voxel_octree::size_vector3d half_voxel = delta_size / 2;
        auto for_loop_impl = [&half_real, &half_voxel, &actual_input, 
                &output, &output_min, &size_min, 
                threshold, reclevel](const std::size_t i)->void {
            const float_vector3d child_min = output_min + 
                    float_vector3d(i & 1 ? half_real.x : 0.f,
                                   i & 2 ? half_real.y : 0.f, 
                                   i & 4 ? half_real.z : 0.f);
            const float_vector3d child_max = child_min + half_real;
            const voxel_octree::size_vector3d child_voxel_min = 
                    size_min + voxel_octree::size_vector3d(
                            i & 1 ? half_voxel.x : 0,
                            i & 2 ? half_voxel.y : 0, 
                            i & 4 ? half_voxel.z : 0);
            const voxel_octree::size_vector3d child_voxel_max = 
                    child_voxel_min + half_voxel;
            voxelize_triangles(output, child_voxel_min, 
                    child_voxel_max, child_min, child_max, actual_input, 
                    threshold, reclevel + 1);
        };
#pragma omp parallel default(shared) if(actual_input.size() >= 1024)
        {
#pragma omp for schedule(auto)
            for(std::size_t i = 0; i < 8; ++i) {
                for_loop_impl(i);
            }
        }
    }
}

float_vector2d project_cube_axis(const float_vector3d& cube_min, 
        const float_vector3d& cube_max, const float_vector3d& axis, 
        const float_vector3d& origin = float_vector3d());
float_vector2d project_triangle_axis(const float_triangle3d& triangle, 
        const float_vector3d& axis, 
        const float_vector3d& origin = float_vector3d());

}

