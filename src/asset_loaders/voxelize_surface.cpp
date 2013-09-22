#include "voxelize_mesh_defines.h"
#include "voxelize_surface.h"
#include "voxelize_mesh_utils.h"
#include <functional>
#include <algorithm>
#include <cassert>

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

void voxelize_triangles(voxel_octree& output, triangle_reference_vector& 
        input, const converter& convert);
void voxelize_triangles_alternate(voxel_octree& output, 
        const float_vector3d& output_min, const float_vector3d& output_max, 
        const triangle_reference_vector& input);
void voxelize_triangles_alternate2(voxel_octree& output, 
        const voxel_octree::size_vector3d& size_min, 
        const voxel_octree::size_vector3d& size_max, 
        const float_vector3d& output_min, 
        const float_vector3d& output_max, 
        const triangle_reference_vector& input, 
        const std::size_t reclevel = 0);
void voxelize_triangles_alternate3(voxel_octree& output, 
        const voxel_octree::size_vector3d& size_min, 
        const voxel_octree::size_vector3d& size_max, 
        const float_vector3d& output_min, 
        const float_vector3d& output_max, 
        const triangle_reference_vector& input, 
        const float_vector3d::value_type threshold, 
        const std::size_t reclevel = 0);

/**
 * @brief test if two ranges overlap
 * @param lhs x is minimum and y is maximum of this range
 * @param rhs x is minimum and y is maximum of this range
 * @return true if ranges contact or overlap, false otherwise
 */
bool ranges_overlap(const float_vector2d& lhs, const float_vector2d& rhs);
/**
 * @brief test if two ranges overlap
 * @param lhs first is minimum and second is maximum of this range
 * @param rhs first is minimum and second is maximum of this range
 * @return true if ranges contact or overlap, false otherwise
 */
bool ranges_overlap(const std::pair<float, float>& lhs, 
        const std::pair<float, float>& rhs);

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
    voxelize_triangles_alternate3(ret, voxel_octree::size_vector3d(0,0,0), 
            ret.get_size(), min_xyz, 
            min_xyz + ret.get_size() / voxels_per_unit, 
            input, threshold_units);
    const std::size_t tree_nodes = ret.get_num_nodes();
    const std::size_t tree_volume = ret.get_opaque_volume();
    const float ratio = float(tree_nodes) / tree_volume;
    std::cerr << "Octree nodes:  " << tree_nodes << std::endl;
    std::cerr << "Octree volume: " << tree_volume << std::endl;
    std::cerr << "Compression:   " << ratio << std::endl;
    std::cerr << "Compression less than 1.0 is good. Greater is bad" 
            << std::endl;
    return ret;
}

void voxelize_triangles(voxel_octree& output, triangle_reference_vector& 
        input, const converter& convert) {
    const voxel_octree::size_vector3d half_size = output.get_size() / 2;
    auto make_child_convert = [half_size, &convert](std::size_t n)->converter {
        const voxel_octree::size_vector3d child_offset(
                n & 0x1 ? half_size.x : 0, 
                n & 0x2 ? half_size.y : 0, 
                n & 0x4 ? half_size.z : 0);
        const voxel_octree::size_vector3d child_origin = 
                convert.m_voxel_origin + child_offset;
//        std::cerr << "Making a converter at offset " << child_origin 
//                << std::endl;
        return converter(convert.m_mesh_origin, child_origin, convert.m_scale);
    };
    if(input.empty()) {
        output.set_voxel(voxel(true, false));
        return;
    } else if(output.get_size().x == 1) {
        output.set_voxel(voxel(true, true));
        return;
    }
//    std::cerr << "Output has size " << output.get_size() << std::endl;
//    std::cerr << "Input has size " << input.size() << std::endl;
//    std::cerr << "Extents are from " << 
//            convert(voxel_octree::size_vector3d(0,0,0)) << 
//            " to " << convert(output.get_size()) << std::endl;
    std::array<triangle_reference_vector, 8> child_inputs;
    const std::array<const converter, 8> child_converters = {{
            make_child_convert(0),
            make_child_convert(1),
            make_child_convert(2),
            make_child_convert(3),                                            
            make_child_convert(4),
            make_child_convert(5),
            make_child_convert(6),
            make_child_convert(7)}};
    while(!input.empty()) {
        const float_triangle3d& triangle = input.back();
        input.pop_back();
        bool triangle_used = false;
        for(std::size_t i = 0; i < child_inputs.size(); ++i) {
            float_vector3d child_min = child_converters[i](
                    voxel_octree::size_vector3d(0,0,0));
            float_vector3d child_max = child_converters[i](
                    half_size);
            if(triangle_in_cube1(triangle, child_min, child_max)) {
                child_inputs[i].push_back(std::ref(triangle));
                triangle_used = true;
            }
        }
        if(!triangle_used) {
//            std::cerr << "An unused triangle appears!" << 
//                    "\n\t" << triangle[0] <<
//                    "\n\t" << triangle[1] <<
//                    "\n\t" << triangle[2] << std::endl << std::endl;
//            std::cerr << 
//                    "\n\t" << convert(triangle[0]) <<
//                    "\n\t" << convert(triangle[1]) <<
//                    "\n\t" << convert(triangle[2]) << std::endl << std::endl;
        }
    }
    input.clear();
    for(std::size_t i = 0; i < child_inputs.size(); ++i) {
        voxel_octree cur_child;
        cur_child.reserve_space(half_size);
        voxelize_triangles(cur_child, child_inputs[i], child_converters[i]);
        output.set_child(i, std::move(cur_child));
    }
}

void voxelize_triangles_alternate(voxel_octree& output, 
        const float_vector3d& output_min, const float_vector3d& output_max, 
        const triangle_reference_vector& input) {
    auto relevant_triangle = [&output_min, &output_max](
            const float_triangle3d& arg)->bool {
        return triangle_in_cube2(arg, output_min, output_max);
    };
    triangle_reference_vector actual_input;
    std::copy_if(input.begin(), input.end(), 
            std::back_inserter(actual_input), relevant_triangle);
    if(actual_input.empty()) {
        output.set_voxel(voxel(true, false));
    } else if(output.get_size().x == 1) {
        output.set_voxel(voxel(true, true));
    } else {
        std::cerr << "Range " << output_min << ", " << output_max << 
            " contains " << actual_input.size() << " triangles." << std::endl;
        std::cerr << "Range size is " << (output_max - output_min).length() 
            << std::endl;
        const float_vector3d half_size = (output_max - output_min) * 0.5;
        for(std::size_t i = 0; i < 8; ++i) {
            voxel_octree cur_child;
            cur_child.reserve_space(output.get_size() / 2);
            const float_vector3d child_min = output_min + 
                    float_vector3d(i & 1 ? half_size.x : 0.f,
                                   i & 2 ? half_size.y : 0.f, 
                                   i & 4 ? half_size.z : 0.f);
            const float_vector3d child_max = child_min + half_size;
            voxelize_triangles_alternate(cur_child, child_min, child_max, 
                    actual_input);
            output.set_child(i, std::move(cur_child));
        }
    }
}

void voxelize_triangles_alternate2(voxel_octree& output, 
        const voxel_octree::size_vector3d& size_min, 
        const voxel_octree::size_vector3d& size_max, 
        const float_vector3d& output_min, 
        const float_vector3d& output_max, 
        const triangle_reference_vector& input, 
        const std::size_t reclevel) {
    auto relevant_triangle = [&output_min, &output_max](
            const float_triangle3d& arg)->bool {
        return triangle_in_cube2(arg, output_min, output_max);
    };
    const voxel_octree::size_vector3d delta_size = size_max - size_min;
    //std::cerr << "Voxel limits " << size_min << ", " << size_max << std::endl;
    std::cerr << "Voxel magnitude " << size_max - size_min << std::endl;
    triangle_reference_vector actual_input;
    std::copy_if(input.begin(), input.end(), 
            std::back_inserter(actual_input), relevant_triangle);
    if(actual_input.empty()) {
        return;
    } else if(delta_size.x == delta_size.y && 
            delta_size.y == delta_size.z && 
            delta_size.z == 1) {
        //std::cerr << "Leaf has " << actual_input.size() << " tris" << std::endl;
        output.set_voxel(size_min, voxel(true, true));
    } else {
        const float_vector3d half_real = (output_max - output_min) / 2;
        const voxel_octree::size_vector3d half_voxel = delta_size / 2;
        for(std::size_t i = 0; i < 8; ++i) {
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
            voxelize_triangles_alternate2(output, child_voxel_min, 
                    child_voxel_max, child_min, child_max, actual_input, 
                    reclevel + 1);
        }
    }
}

void voxelize_triangles_alternate3(voxel_octree& output, 
        const voxel_octree::size_vector3d& size_min, 
        const voxel_octree::size_vector3d& size_max, 
        const float_vector3d& output_min, 
        const float_vector3d& output_max, 
        const triangle_reference_vector& input, 
        const float_vector3d::value_type threshold, 
        const std::size_t reclevel) {
    auto relevant_triangle = [&output_min, &output_max, &threshold](
            const float_triangle3d& arg)->bool {
        //triangle_in_cube_threshold1(arg, output_min, output_max, threshold);
        return triangle_in_cube3(arg, output_min, output_max);
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
        output.set_voxel(size_min, voxel(true, true));
    } else {
        const float_vector3d half_real = (output_max - output_min) / 2;
        const voxel_octree::size_vector3d half_voxel = delta_size / 2;
        for(std::size_t i = 0; i < 8; ++i) {
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
            voxelize_triangles_alternate3(output, child_voxel_min, 
                    child_voxel_max, child_min, child_max, actual_input, 
                    threshold, reclevel + 1);
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

