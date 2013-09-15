#include "voxelize_mesh_defines.h"
#include "voxelize_surface.h"
#include <functional>
#include <array>
#include <algorithm>
#include <cassert>

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
void voxelize_triangles_alternate2(voxel_octree& output, 
        const voxel_octree::size_vector3d& size_min, 
        const voxel_octree::size_vector3d& size_max, 
        const float_vector3d& output_min, 
        const float_vector3d& output_max, 
        const triangle_reference_vector& input, 
        const std::size_t reclevel = 0);

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
bool triangle_in_cube_alternate(const float_triangle3d& triangle, 
        const float_vector3d& cube_min, const float_vector3d& cube_max);
void test_triangle_projection(const float_triangle3d& triangle);

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
    voxelize_triangles_alternate2(ret, voxel_octree::size_vector3d(0,0,0), 
            ret.get_size(), min_xyz, max_xyz, input);
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
        return triangle_in_cube_alternate(arg, output_min, output_max);
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
        const float_vector3d half_real = (output_max - output_min) * 0.5;
        const voxel_octree::size_vector3d half_voxel = delta_size / 2;
        for(std::size_t i = 0; i < 8; ++i) {
            voxel_octree cur_child;
            cur_child.reserve_space(output.get_size() / 2);
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

float_vector2d project_cube_axis(const float_vector3d& cube_min, 
        const float_vector3d& cube_max, const float_vector3d& axis, 
        const float_vector3d& origin = float_vector3d());
float_vector2d project_triangle_axis(const float_triangle3d& triangle, 
        const float_vector3d& axis, 
        const float_vector3d& origin = float_vector3d());

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
    if(!(ranges_overlap(tri_x, box_x) && ranges_overlap(tri_y, box_y) 
            && ranges_overlap(tri_z, box_z))) {
        //std::cerr << "Triangle failure by box" << std::endl;
        return false;
    } else {
        const float_vector2d cube_range = project_cube_axis(cube_min, 
                cube_max, triangle[3], triangle[0]);
        const float_vector2d tri_range = project_triangle_axis(triangle, 
                triangle[3], triangle[0]);
        if(!ranges_overlap(cube_range, tri_range)) {
            //std::cerr << "Triangle failure by normal" << std::endl;
            return false;
        } else {
            for(std::size_t i = 0; i < 3; ++i) {
                std::size_t j = (i + 1) % 3;
                const float_vector3d axis = (triangle[j] - 
                        triangle[i]).normalize().cross(triangle[3]);
                const float_vector2d cube_edge = project_cube_axis(
                        cube_min, cube_max, axis, triangle[0]);
                const float_vector2d triangle_edge = project_triangle_axis(
                        triangle, axis, triangle[0]);
                if(!ranges_overlap(cube_edge, triangle_edge)) {
                    //std::cerr << "Triangle failure by edge" << std::endl;
                    return false;
                }
            }
            return true;
        }
    }
}

bool triangle_in_cube_alternate(const float_triangle3d& triangle, 
        const float_vector3d& cube_min, const float_vector3d& cube_max) {
    test_triangle_projection(triangle);
    //compute the limits of the triangle
    float_vector3d triangle_min, triangle_max;
    std::tie(triangle_min, triangle_max) = triangle_limits(triangle);
    const std::array<const float_vector3d, 7> axes = {{
        float_vector3d(1,0,0),
        float_vector3d(0,1,0),
        float_vector3d(0,0,1),
        triangle[3],
        (triangle[1]-triangle[0]).cross(triangle[3]),
        (triangle[2]-triangle[1]).cross(triangle[3]),
        (triangle[0]-triangle[2]).cross(triangle[3])}};
    const float_vector3d origin = triangle[2];
    for(const float_vector3d& axis : axes) {
        const float_vector2d cube_range = project_cube_axis(
                cube_min, cube_max, axis, origin);
        const float_vector2d tri_range = project_triangle_axis(
                triangle, axis, origin);
        if(!ranges_overlap(cube_range, tri_range))
            return false;
    }
    return true;
}

float_vector2d project_cube_axis(const float_vector3d& cube_min, 
        const float_vector3d& cube_max, const float_vector3d& axis, 
        const float_vector3d& origin) {
    const float_vector3d cmin = cube_min - origin;
    const float_vector3d cmax = cube_max - origin;
    float cube_min_projected = MAX_VAL;
    float cube_max_projected = MIN_VAL;
    for(std::size_t i = 0; i < 8; ++i) {        
        const float_vector3d corner(
                i & 1 ? cmax.x : cmin.x, 
                i & 2 ? cmax.y : cmin.y, 
                i & 4 ? cmax.z : cmin.z);
        const float corner_project = corner.dot(axis);
        cube_min_projected = std::min(corner_project, cube_min_projected);
        cube_max_projected = std::max(corner_project, cube_max_projected);
    }
    return float_vector2d(cube_min_projected, cube_max_projected);
}

float_vector2d project_triangle_axis(const float_triangle3d& triangle, 
        const float_vector3d& axis, const float_vector3d& origin) {
    float tri_min_projected = MAX_VAL;
    float tri_max_projected = MIN_VAL;
    for(std::size_t i = 0; i < 3; ++i) {        
        const float_vector3d vert = triangle[i] - origin;
        const float vert_project = vert.dot(axis);
        tri_min_projected = std::min(vert_project, tri_min_projected);
        tri_max_projected = std::max(vert_project, tri_max_projected);
    }
    return float_vector2d(tri_min_projected, tri_max_projected);
}

void test_triangle_projection(const float_triangle3d& triangle) {
    float_vector3d tri_lo, tri_hi;
    std::tie(tri_lo, tri_hi) = triangle_limits(triangle);
    const float_vector2d trix = project_triangle_axis(triangle, 
        float_vector3d(1,0,0));
    const float_vector2d triy = project_triangle_axis(triangle, 
        float_vector3d(0,1,0));
    const float_vector2d triz = project_triangle_axis(triangle, 
        float_vector3d(0,0,1));
    const float_vector2d triax(tri_lo.x, tri_hi.x);
    const float_vector2d triay(tri_lo.y, tri_hi.y);
    const float_vector2d triaz(tri_lo.z, tri_hi.z);
    
    assert(trix == triax && triy == triay && triz == triaz);
}

}

