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
        return (m_scale * (arg - m_mesh_origin)) - m_voxel_origin;
    }
    float_vector3d operator ()(const voxel_data::size_vector3d& arg) const {
        return m_mesh_origin + ((arg + m_voxel_origin) / m_scale);
    }
    const float_vector3d m_mesh_origin;
    const voxel_data::size_vector3d m_voxel_origin;
    const float m_scale;
};

void voxelize_triangles(voxel_octree& output, triangle_reference_vector& 
        input, const converter& convert);

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
    voxelize_triangles(ret, input, convert);
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
    auto project_triangle = [](const float_triangle3d& arg, 
            const float_vector3d& axis)->float_vector2d {
        float_vector2d ret(MAX_VAL, MIN_VAL);
        for(std::size_t i = 0; i < 3; ++i) {
            float value = axis.dot(arg[i]);
            ret.x = std::min(ret.x, value);
            ret.y = std::max(ret.y, value);
        }
        return ret;
    };
    auto project_box = [](const float_vector3d& mins, 
            const float_vector3d& maxs, 
            const float_vector3d& axis)->float_vector2d {
        float_vector2d ret(MAX_VAL, MIN_VAL);
        for(std::size_t i = 0; i < 8; ++i) {
            const float_vector3d current(
                    i & 0x1 ? maxs.x : mins.x, 
                    i & 0x2 ? maxs.y : mins.y, 
                    i & 0x4 ? maxs.z : mins.z);
            float value = axis.dot(current);
            ret.x = std::min(ret.x, value);
            ret.y = std::max(ret.y, value);
        }
        return ret;
    };
    auto ranges_intersect = [](const float_vector2d& lhs, 
            const float_vector2d& rhs)->bool {
        return !(lhs.y < rhs.x || rhs.y < lhs.x);
    };
    if(input.empty()) {
        output.set_voxel(voxel(true, false));
        return;
    } else if(output.get_size().x == 1) {
        std::cerr << "We set a leaf at offset" << 
                convert.m_voxel_origin << std::endl;
        output.set_voxel(voxel(true, true));
        return;
    }
    std::cerr << "Output has size " << output.get_size() << std::endl;
    std::cerr << "Input has size " << input.size() << std::endl;
    std::cerr << "Extents are from " << 
            convert(voxel_octree::size_vector3d(0,0,0)) << 
            " to " << convert(output.get_size()) << std::endl;
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
        const std::array<const float_vector3d, 4> axes = {{
            float_vector3d(1,0,0),
            float_vector3d(0,1,0),
            float_vector3d(0,0,1),
            float_vector3d(triangle[3])}};
        input.pop_back();
        bool triangle_used = false;
        for(std::size_t i = 0; i < child_inputs.size(); ++i) {
            float_vector3d child_min = child_converters[i](
                    voxel_octree::size_vector3d(0,0,0));
            float_vector3d child_max = child_converters[i](
                    half_size);
            auto axis_separates = [&child_min, &child_max, &triangle, 
                    &ranges_intersect, &project_triangle, &project_box](
                    const float_vector3d& axis) {
                float_vector2d bp = project_box(child_min, 
                        child_max, axis);
                float_vector2d tp = project_triangle(triangle, axis);
                return !ranges_intersect(bp, tp);
            };
            if(!std::any_of(axes.begin(), axes.end(), axis_separates)) {
                child_inputs[i].push_back(std::ref(triangle));
                triangle_used = true;
            }
        }
        if(!triangle_used) {
            std::cerr << "An unused triangle appears!" << 
                    "\n\t" << triangle[0] <<
                    "\n\t" << triangle[1] <<
                    "\n\t" << triangle[2] << std::endl << std::endl;
            std::cerr << 
                    "\n\t" << convert(triangle[0]) <<
                    "\n\t" << convert(triangle[1]) <<
                    "\n\t" << convert(triangle[2]) << std::endl << std::endl;
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

}

