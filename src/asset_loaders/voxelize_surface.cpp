#include "voxelize_mesh_defines.h"
#include "voxelize_surface.h"
#include <functional>
#include <array>
#include <algorithm>

namespace trillek {

typedef std::vector<std::reference_wrapper<const float_triangle3d> >
        triangle_reference_vector;

//float or double or similar
static const auto MIN_VAL = std::numeric_limits<
        triangle3d_vector::value_type::value_type::value_type>::min();
//float or double or similar
static const auto MAX_VAL = std::numeric_limits<
        triangle3d_vector::value_type::value_type::value_type>::max();

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

/**
 * @brief Get the axis aligned bounding box of this triangle
 * @param triangle triangle to analyze
 * @return pair containing too vector3d objects, first having all 
 * minimum coordinates and second having all maximum coordinates
 */
std::pair<float_vector3d, float_vector3d> triangle_limits(
        const float_triangle3d& triangle);
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

bool triangle_in_cube(const float_triangle3d& triangle, 
        const float_vector3d& cube_min, const float_vector3d& cube_max);

voxel_octree voxelize_mesh_surface(const triangle3d_vector& all_triangles, 
        const float voxels_per_unit) {
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
            voxels_per_unit, voxels_per_unit);
    max_xyz += make_vector3d(voxels_per_unit, 
            voxels_per_unit, voxels_per_unit);
    converter convert(min_xyz, voxel_octree::size_vector3d(0,0,0), 
            voxels_per_unit);
    voxel_octree::size_vector3d msize = convert(max_xyz) + 
            make_vector3d(1,1,1);
    voxel_octree ret;
    ret.reserve_space(msize);
    std::cerr << "Octree size: " << ret.get_size() << std::endl;
    triangle_reference_vector input;
    for(const float_triangle3d& triangle : all_triangles) {
        input.push_back(std::ref(triangle));
    }
    //voxelize_triangles(ret, input, convert);
    voxelize_triangles_alternate(ret, min_xyz, 
            ret.get_size() / voxels_per_unit, input);
    std::cerr << "Octree nodes: " << ret.get_num_nodes() << std::endl;
    std::cerr << "Octree full volume: " << ret.get_opaque_volume() 
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
            if(triangle_in_cube(triangle, child_min, child_max)) {
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
        return triangle_in_cube(arg, output_min, output_max);
    };
    triangle_reference_vector actual_input;
    std::copy_if(input.begin(), input.end(), 
            std::back_inserter(actual_input), relevant_triangle);
    if(actual_input.empty()) {
        return;
    } else if(output.get_size().x == 1) {
        output.set_voxel(voxel(true, true));
        std::cerr << "Valid leaf from " << output_min << " to "
                << output_max << std::endl;
    } else {
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

std::pair<float_vector3d, float_vector3d> triangle_limits(
        const float_triangle3d& triangle) {
    float_vector3d low(MAX_VAL, MAX_VAL, MAX_VAL);
    float_vector3d high(MIN_VAL, MIN_VAL, MIN_VAL);
    for(std::size_t i = 0; i < 3; ++i) {
        low = min_vector(low, triangle[i]);
        high = max_vector(high, triangle[i]);
    }
    return std::make_pair(low, high);
}

bool ranges_overlap(const float_vector2d& lhs, const float_vector2d& rhs) {
    return !(lhs.y < rhs.x || rhs.y < lhs.x);
}

bool ranges_overlap(const std::pair<float, float>& lhs, 
        const std::pair<float, float>& rhs) {
    return ranges_overlap(float_vector2d(lhs.first, lhs.second), 
            float_vector2d(rhs.first, rhs.second));
}

bool triangle_in_cube(const float_triangle3d& triangle, 
        const float_vector3d& cube_min, const float_vector3d& cube_max) {
    //compute the limits of the triangle
    float_vector3d triangle_min, triangle_max;
    std::tie(triangle_min, triangle_max) = triangle_limits(triangle);
    //compute the x,y,z axis ranges
    const float_vector2d tri_x(triangle_min.x, triangle_max.x);
    const float_vector2d tri_y(triangle_min.y, triangle_max.y);
    const float_vector2d tri_z(triangle_min.z, triangle_max.z);
    const float_vector2d box_x(cube_min.x, cube_max.x);
    const float_vector2d box_y(cube_min.y, cube_max.y);
    const float_vector2d box_z(cube_min.z, cube_max.z);
    if(!ranges_overlap(tri_x, box_x) || !ranges_overlap(tri_y, box_y) 
            || !ranges_overlap(tri_z, box_z)) {
        return false;
    } else {
        //compute along the triangle normal
        const float_vector3d cmin = cube_min - triangle[0];
        const float_vector3d cmax = cube_max - triangle[0];
        const std::array<const float_vector3d, 8> corners = {{
                float_vector3d(cmin.x, cmin.y, cmin.z),
                float_vector3d(cmax.x, cmin.y, cmin.z),
                float_vector3d(cmax.x, cmax.y, cmin.z),
                float_vector3d(cmin.x, cmax.y, cmin.z),
                float_vector3d(cmin.x, cmin.y, cmax.z),
                float_vector3d(cmax.x, cmin.y, cmax.z),
                float_vector3d(cmax.x, cmax.y, cmax.z),
                float_vector3d(cmin.x, cmax.y, cmax.z)}};
        float cube_min_projected = MAX_VAL;
        float cube_max_projected = MIN_VAL;
        for(const float_vector3d& corner : corners) {
            const float corner_project = corner.dot(triangle[3]);
            cube_min_projected = std::min(corner_project, cube_min_projected);
            cube_max_projected = std::max(corner_project, cube_max_projected);
        }
        return ranges_overlap(std::make_pair(0.f, 0.f), 
                std::minmax(cube_min_projected, cube_max_projected));
    }
}

}

