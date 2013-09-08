#include "voxelize_ray_trace.h"
#include <iostream>
#include <limits>
#include <functional>
#include <algorithm>
#include <utility>
#include <omp.h>

namespace trillek {

typedef std::map<int_vector2d, std::vector<
        std::reference_wrapper<const float_triangle3d> > >
        bucket_map;
typedef std::vector<std::reference_wrapper<const bucket_map::value_type> >
        bucket_reference_vector;

/**
 * @brief union zero octrees
 * @param args nothing
 * @return empty octree
 */
template <typename... Args>
voxel_octree voxelize_octree_union(Args&&... args) {
    return voxel_octree();
}
/**
 * @brief union one octree
 * @param lhs an octree
 * @param args nothing
 * @return lhs
 */
template <typename Lhs, typename... Args>
voxel_octree voxelize_octree_union(Lhs&& lhs, Args&&... args) {
    return std::forward<Lhs>(lhs);
}
/**
 * @brief Union two or more octrees
 * @param lhs octrees must have same size
 * @param rhs octrees must have same size
 * @param args
 * @return union (lhs union rhs) and the rest
 * 
 * Inputs assumed to have equal size
 */
template <typename Lhs, typename Rhs, typename... Args>
voxel_octree voxelize_octree_union(Lhs&& lhs, Rhs&& rhs, Args&&... args) {
    const voxel_octree& l = lhs;
    const voxel_octree& r = rhs;
    voxel_octree ret;
    ret.reserve_space(l.get_size());
    ret.set_offset(l.get_offset());
    if(l.has_children() && !r.has_children()) {
        if(r.get_voxel().is_opaque()) {
            ret.set_voxel(r.get_voxel());
        } else {
            for(std::size_t i = 0; i < 8; ++i) {
                ret.set_child(i, *l.get_child(i));
            }
        }
    } else if(r.has_children() && !l.has_children()) {
        if(l.get_voxel().is_opaque()) {
            ret.set_voxel(l.get_voxel());
        } else {
            for(std::size_t i = 0; i < 8; ++i) {
                ret.set_child(i, *r.get_child(i));
            }
        }
    } else if(l.has_children() && r.has_children()) {
        for(std::size_t i = 0; i < 8; ++i) {
            ret.set_child(i, 
                    voxelize_octree_union(*l.get_child(i), 
                    *r.get_child(i)));
        }
    } else {
        ret.set_voxel(voxel(true, 
                l.get_voxel().is_opaque() || 
                r.get_voxel().is_opaque()));
    }
    return voxelize_octree_union(std::move(ret), 
            std::forward<Args>(args)...);
}
bool z_ray_intersection(const float_triangle3d& triangle, 
        const float_vector2d& xy, float& out) {
    const float_triangle2d triangle2d = {{
            float_vector2d(triangle[0].x, triangle[0].y),
            float_vector2d(triangle[1].x, triangle[1].y),
            float_vector2d(triangle[2].x, triangle[2].y)}};
    if(consistent_point_in_triangle(triangle2d, xy)) {
        const float_vector3d& norm = triangle[3];
        float_vector2d point = xy - triangle2d[0];
        out = triangle[0].z - point.x * (norm.x / norm.z) - 
                point.y * (norm.y / norm.z);
        return true;
    } else {
        return false;
    }
}
bool y_ray_intersection(const float_triangle3d& triangle, 
        const float_vector2d& zx, float& out) {
    const float_triangle2d triangle2d = {{
            float_vector2d(triangle[0].z, triangle[0].x),
            float_vector2d(triangle[1].z, triangle[1].x),
            float_vector2d(triangle[2].z, triangle[2].x)}};
    if(consistent_point_in_triangle(triangle2d, zx)) {
        const float_vector3d& norm = triangle[3];
        float_vector2d point = zx - triangle2d[0];
        out = triangle[0].y - point.x * (norm.z / norm.y) - 
                point.y * (norm.x / norm.y);
        return true;
    } else {
        return false;
    }
}
bool x_ray_intersection(const float_triangle3d& triangle, 
        const float_vector2d& yz, float& out) {
    const float_triangle2d triangle2d = {{
            float_vector2d(triangle[0].y, triangle[0].z),
            float_vector2d(triangle[1].y, triangle[1].z),
            float_vector2d(triangle[2].y, triangle[2].z)}};
    if(consistent_point_in_triangle(triangle2d, yz)) {
        const float_vector3d& norm = triangle[3];
        float_vector2d point = yz - triangle2d[0];
        out = triangle[0].x - point.x * (norm.y / norm.x) - 
                point.y * (norm.z / norm.x);
        return true;
    } else {
        return false;
    }
}

int x_compute_winding(const float_triangle2d& triangle, 
        const float_vector2d& xy) {
    int ret = 0;
    for(std::size_t i = 0; i < 3; ++i) {
        std::size_t j = (i + 1) % 3;
        const float_vector2d a = triangle[i];
        const float_vector2d b = triangle[j];
        if(a.y <= xy.y) {
            if(b.y > xy.y) {
                if((b - a).cross(xy - a) > 0)
                    ++ret;
            }
        } else if(b.y <= xy.y) {
            if((b - a).cross(xy - a) < 0)
                --ret;
        }
    }
    return ret;
}
int y_compute_winding(const float_triangle2d& triangle, 
        const float_vector2d& xy) {
    //swap xy and invoke x_compute_winding
    auto swapxy = [](const float_vector2d& arg)->float_vector2d {
        return make_vector2d(arg.y, arg.x);
    };
    const float_triangle2d triangle_yx = {{
            swapxy(triangle[0]),
            swapxy(triangle[1]),
            swapxy(triangle[2])}};
    const float_vector2d yx = swapxy(xy);
    return x_compute_winding(triangle_yx, yx);
}
bool consistent_point_in_triangle(const float_triangle2d& triangle, 
        const float_vector2d& xy) {
    const bool x = x_compute_winding(triangle, xy) != 0;
    const bool y = y_compute_winding(triangle, xy) != 0;
    return x || y;
}

voxel_octree voxelize_mesh_ray_trace(const triangle3d_vector& all_triangles, 
        const std::size_t magnify, const bool multi_axis) {
    //float or double or similar
    static const auto MIN_VAL = std::numeric_limits<
            triangle3d_vector::value_type::value_type::value_type>::min();
    //float or double or similar
    static const auto MAX_VAL = std::numeric_limits<
            triangle3d_vector::value_type::value_type::value_type>::max();
    //signed integral type
    static const auto MIN_INT_VAL = std::numeric_limits<
            int_vector3d::value_type>::min();
    static const auto MAX_INT_VAL = std::numeric_limits<
            int_vector3d::value_type>::max();
    typedef std::vector<float> crossing_vector;
    typedef std::vector<int_vector3d::value_type> discrete_crossing_vector;
    //bucket sort triangles along each plane
    bucket_map xyb, yzb, zxb;
    //integral extents
    int_vector3d min_xyz(MAX_INT_VAL, MAX_INT_VAL, MAX_INT_VAL);
    int_vector3d max_xyz(MIN_INT_VAL, MIN_INT_VAL, MIN_INT_VAL);
    auto triangle_extents = [](const float_triangle3d& arg)
            -> std::pair<float_vector3d, float_vector3d> {
        float_vector3d min_extents(MAX_VAL, MAX_VAL, MAX_VAL);
        float_vector3d max_extents(MIN_VAL, MIN_VAL, MIN_VAL);
        for(std::size_t i = 0; i < 3; ++i) {
            const float_vector3d& cur_vertex = arg[i];
            min_extents.x = std::min(min_extents.x, cur_vertex.x);
            min_extents.y = std::min(min_extents.y, cur_vertex.y);
            min_extents.z = std::min(min_extents.z, cur_vertex.z);
            max_extents.x = std::max(max_extents.x, cur_vertex.x);
            max_extents.y = std::max(max_extents.y, cur_vertex.y);
            max_extents.z = std::max(max_extents.z, cur_vertex.z);
        }
        return std::make_pair(min_extents, max_extents);
    };
    auto bucket_triangle = [&xyb, &yzb, &zxb, &min_xyz, &max_xyz, 
            multi_axis](
            const float_vector3d& min_extents,
            const float_vector3d& max_extents, 
            const float_triangle3d& arg)->void {
        int_vector3d min_int(
                std::floor(min_extents.x),
                std::floor(min_extents.y),
                std::floor(min_extents.z));
        int_vector3d max_int(
                std::ceil(max_extents.x),
                std::ceil(max_extents.y),
                std::ceil(max_extents.z));
        max_int += int_vector3d(2,2,2);
        min_int += int_vector3d(-1, -1, -1);
        //update overall extents
        min_xyz.x = std::min(min_xyz.x, min_int.x);
        min_xyz.y = std::min(min_xyz.y, min_int.y);
        min_xyz.z = std::min(min_xyz.z, min_int.z);
        max_xyz.x = std::max(max_xyz.x, max_int.x);
        max_xyz.y = std::max(max_xyz.y, max_int.y);
        max_xyz.z = std::max(max_xyz.z, max_int.z);
        for(int_vector3d::value_type x = min_int.x; 
                x != max_int.x; ++x) {
            for(int_vector3d::value_type y = min_int.y; 
                    y != max_int.y; ++y) {
#pragma omp critical(xyb_crit)
                xyb[int_vector2d(x,y)].push_back(std::ref(arg));
            }
        }
        if(multi_axis) {
            for(int_vector3d::value_type y = min_int.y; 
                    y != max_int.y; ++y) {
                for(int_vector3d::value_type z = min_int.z; 
                        z != max_int.z; ++z) {
#pragma omp critical(yzb_crit)
                    yzb[int_vector2d(y, z)].push_back(std::ref(arg));
                }
            }
            for(int_vector3d::value_type z = min_int.z; 
                    z != max_int.z; ++z) {
                for(int_vector3d::value_type x = min_int.x; 
                        x != max_int.x; ++x) {
#pragma omp critical(zxb_crit)
                    zxb[int_vector2d(z, x)].push_back(std::ref(arg));
                }
            }
        }
    };
    std::cerr << "Building buckets" << std::endl;
#pragma omp parallel default(shared)
    {
#pragma omp for schedule(auto)
        for(std::size_t i = 0; i < all_triangles.size(); ++i) {
            const float_triangle3d& triangle = all_triangles[i];
            float_vector3d min_extents, max_extents;
            std::tie(min_extents, max_extents) = triangle_extents(triangle);
            //std::cerr << 
                    bucket_triangle(min_extents, max_extents, triangle);
            //std::cerr << ", ";
        }
    }
    std::cerr << "Bucket counts are " << 
            "\n\t" << xyb.size();
    if(multi_axis) {
        std::cerr << "\n\t" << yzb.size() << 
                "\n\t" << zxb.size();
    }
    std::cerr << std::endl;
    voxel_data::size_vector3d size_extent = (max_xyz - min_xyz);
    voxel_octree xyvox, yzvox, zxvox;
    const voxel_octree::size_vector3d ret_size = size_extent * magnify;
    xyvox.reserve_space(ret_size);
    if(multi_axis) {
        yzvox.reserve_space(ret_size);
        zxvox.reserve_space(ret_size);
    }
    typedef bool (* const cast_func)(const float_triangle3d&, 
            const float_vector2d&, float&);
    /**
     * @brief Cast a ray through these triangles at this position
     * @param vt an element of the bucket map we examine
     * @param ray_intersection the ray casting function - x, y, or z variant
     * @param offset_from_vt if we want to offset from the integer coords
     * @return sorted vector of intersection points along the appropriate
     * axis
     */
    auto cast_ray = [](const bucket_map::value_type& vt, 
            cast_func ray_intersection, 
            const float_vector2d& offset_from_vt)->crossing_vector {
        crossing_vector ret;
        const float_vector2d cast_at = vt.first + offset_from_vt;
        for(const float_triangle3d& triangle : vt.second) {
            float crossing;
            if(ray_intersection(triangle, cast_at, crossing)) {
                ret.push_back(crossing);
            }
        }
        std::sort(ret.begin(), ret.end());
        return ret;
    };
    /**
     * @brief Transform model space casts into voxel space coordinates
     * @param arg a sequence of model space intersections
     * @param min_val the voxel origin for that axis in model space
     * @return a sequence of transformed discretized voxel coordinates
     */
    auto discretize_cast = [](const crossing_vector& arg, 
            int_vector3d::value_type min_val)->discrete_crossing_vector {
        discrete_crossing_vector ret;
        for(std::size_t i = 0; i + 1 < arg.size(); i += 2) {
            if(arg[i + 1] > arg[i]) {
                for(float coord = arg[i]; coord < arg[i + 1]; 
                        ++coord) {
                    int_vector3d::value_type coord_i = 
                            static_cast<int_vector3d::value_type>(coord + 0.5);
                    ret.push_back(coord_i - min_val);
                }
            } else {
                for(float coord = arg[i + 1]; coord < arg[i]; 
                        ++coord) {
                    int_vector3d::value_type coord_i = 
                            static_cast<int_vector3d::value_type>(coord + 0.5);
                    ret.push_back(coord_i - min_val);
                }
            }
        }
        return ret;
    };
    auto multicast_ray = [&cast_ray, &discretize_cast](
            const bucket_map::value_type& vt, 
            const int_vector2d& offset, 
            const std::size_t scale,
            cast_func ray_intersection, 
            int_vector3d::value_type min_val)->discrete_crossing_vector {
        auto offset_scale = [&offset, &scale](
                const float_vector2d& arg)->float_vector2d {
            return (offset + arg) / scale;
        };
        const std::array<float_vector2d, 4> offsets = {{
                offset_scale(float_vector2d(0.20, 0.20)), 
                offset_scale(float_vector2d(0.80, 0.20)),
                offset_scale(float_vector2d(0.20, 0.80)), 
                offset_scale(float_vector2d(0.80, 0.80))}};
        std::array<discrete_crossing_vector, 4> crossings;
        std::size_t current_crossing_index = 0;
        for(const float_vector2d& offset : offsets) {
            crossing_vector cv = cast_ray(vt, ray_intersection, offset);
            for(float& c : cv) {
                c *= scale;
            }
            discrete_crossing_vector dcv1, dcv2;
            dcv1 = discretize_cast(cv, min_val * scale);
            std::reverse(cv.begin(), cv.end());
            dcv2 = discretize_cast(cv, min_val * scale);
            std::sort(dcv1.begin(), dcv1.end());
            std::sort(dcv2.begin(), dcv2.end());
            std::set_intersection(dcv1.begin(), dcv1.end(), 
                    dcv2.begin(), dcv2.end(), 
                    std::back_inserter(crossings[current_crossing_index]));
            ++current_crossing_index;
        }
        discrete_crossing_vector merge1, merge2, merge3;
        std::merge(crossings[0].begin(), crossings[0].end(), 
                crossings[1].begin(), crossings[1].end(), 
                std::back_inserter(merge1));
        std::merge(crossings[2].begin(), crossings[2].end(), 
                crossings[3].begin(), crossings[3].end(), 
                std::back_inserter(merge2));
        std::merge(merge1.begin(), merge1.end(), 
                merge2.begin(), merge2.end(), 
                std::back_inserter(merge3));
        discrete_crossing_vector ret;
        for(discrete_crossing_vector::const_iterator first = merge3.begin(); 
                first != merge3.end(); ++first) {
            discrete_crossing_vector::const_iterator last = first;
            for(; last != merge3.end() && *last == *first; ++last);
            const ptrdiff_t count = last - first;
            if(count > 1) {
                ret.push_back(*first);
            }
            first = --last;
        }
        return ret;
    };
    std::vector<int_vector2d> offsets(magnify * magnify);
    const std::size_t offset_scale = magnify;
    for(std::size_t i = 0; i != offsets.size(); ++i) {
        offsets[i] = int_vector2d(i % magnify, 
                i / magnify);
    }
    auto make_bucket_reference = [](
            const bucket_map& bm)->bucket_reference_vector {
        bucket_reference_vector ret;
        ret.reserve(bm.size());
        for(const bucket_map::value_type& vt : bm) {
            ret.push_back(std::ref(vt));
        }
        return ret;
    };
#pragma omp parallel sections default(shared)
    {
#pragma omp section
        {
#pragma omp critical
            std::cerr << "Voxelizing x-y plane" << std::endl;
            bucket_reference_vector xybr = make_bucket_reference(xyb);
#pragma omp parallel default(shared)
            {
#pragma omp for schedule(auto)
                for(std::size_t i = 0; i < xybr.size(); ++i) {
                    const bucket_map::value_type& xyvt = xybr[i];
                    {
                        for(std::size_t offset_i = 0; 
                                offset_i < offsets.size(); 
                                ++offset_i) {
                            const float_vector2d& offset = offsets[offset_i];
                            const std::size_t voxel_x = offset.x + 
                                    (xyvt.first.x - min_xyz.x) * offset_scale;
                            const std::size_t voxel_y = offset.y + 
                                    (xyvt.first.y - min_xyz.y) * offset_scale;
                            for(int_vector3d::value_type cz : 
                                    multicast_ray(xyvt, offset, offset_scale, 
                                    &z_ray_intersection, min_xyz.z)) {
#pragma omp critical(xyvox_crit)
                                xyvox.set_voxel(voxel_x, voxel_y, cz, 
                                        voxel(true, true));
                            }
                        }
                    }
                }
            }
#pragma omp critical
            std::cerr << "Done voxelizing x-y plane" << std::endl;
            xybr.clear();
            xyb.clear();
        }
#pragma omp section
        {
            if(multi_axis) {
#pragma omp critical
                std::cerr << "Voxelizing y-z plane" << std::endl;
            }
            bucket_reference_vector yzbr = make_bucket_reference(yzb);
#pragma omp parallel default(shared)
            {
#pragma omp for schedule(auto)
                for(std::size_t i = 0; i < yzbr.size(); ++i) {
                    const bucket_map::value_type& yzvt = yzbr[i];
                    {
                        for(std::size_t offset_i = 0; 
                                offset_i < offsets.size(); 
                                ++offset_i) {
                            const float_vector2d& offset = offsets[offset_i];
                            const std::size_t voxel_y = offset.x + 
                                    (yzvt.first.x - min_xyz.y) * offset_scale;
                            const std::size_t voxel_z = offset.y + 
                                    (yzvt.first.y - min_xyz.z) * offset_scale;
                            for(int_vector3d::value_type cx : 
                                    multicast_ray(yzvt, offset, offset_scale, 
                                    &x_ray_intersection, min_xyz.x)) {
#pragma omp critical(yzvox_crit)
                                yzvox.set_voxel(cx, voxel_y,voxel_z, 
                                        voxel(true, true));
                            }
                        }
                    }
                }
            }
            if(multi_axis) {
#pragma omp critical
                std::cerr << "Done voxelizing y-z plane" << std::endl;
            }
            yzbr.clear();
            yzb.clear();
        }
#pragma omp section
        {
            if(multi_axis) {
#pragma omp critical
                std::cerr << "Voxelizing z-x plane" << std::endl;
            }
            bucket_reference_vector zxbr = make_bucket_reference(zxb);
#pragma omp parallel default(shared)
            {
#pragma omp for schedule(auto)
                for(std::size_t i = 0; i < zxbr.size(); ++i) {
                    const bucket_map::value_type& zxvt = zxbr[i];
                    {
                        for(std::size_t offset_i = 0; 
                                offset_i < offsets.size(); 
                                ++offset_i) {
                            const float_vector2d& offset = offsets[offset_i];
                            const std::size_t voxel_x = offset.y + 
                                    (zxvt.first.y - min_xyz.x) * offset_scale;
                            const std::size_t voxel_z = offset.x + 
                                    (zxvt.first.x - min_xyz.z) * offset_scale;
                            for(int_vector3d::value_type cy : 
                                    multicast_ray(zxvt, offset, offset_scale, 
                                    &y_ray_intersection, min_xyz.y)) {
#pragma omp critical(zxvox_crit)
                                zxvox.set_voxel(voxel_x, cy, voxel_z, 
                                        voxel(true, true));
                            }
                        }
                    }
                }
            }
            if(multi_axis) {
#pragma omp critical
                std::cerr << "Done voxelizing z-x plane" << std::endl;
            }
            zxbr.clear();
            zxb.clear();
        }
    }
    if(multi_axis) {
        std::cerr << "Starting voxel union" << std::endl;
        voxel_octree ret = voxelize_octree_union(xyvox, yzvox, zxvox);
        std::cerr << "Done voxel union" << std::endl;
        std::cerr << "Voxel counts are: " << 
                "\n\t" << xyvox.get_opaque_volume() << 
                "\n\t" << yzvox.get_opaque_volume() << 
                "\n\t" << zxvox.get_opaque_volume() << std::endl;
        std::cerr << "Final result has filled volume " << 
                ret.get_opaque_volume()
                << std::endl;
        std::cerr << "Final result has nodes " << 
                ret.get_num_nodes() << std::endl;
        return ret;
    } else {
        std::cerr << "Final result has filled volume " << 
                xyvox.get_opaque_volume()
                << std::endl;
        std::cerr << "Final result has nodes " << 
                xyvox.get_num_nodes() << std::endl;
        return xyvox;
    }
}

}
