#include "voxelize_mesh_utils.h"
#include <limits>

namespace trillek {

const float_vector3d::value_type MIN_VAL = std::numeric_limits<
        triangle3d_vector::value_type::value_type::value_type>::min();
const float_vector3d::value_type MAX_VAL = std::numeric_limits<
        triangle3d_vector::value_type::value_type::value_type>::max();

float_vector2d project_cube_axis(const float_vector3d& cube_min, 
        const float_vector3d& cube_max, const float_vector3d& axis, 
        const float_vector3d& origin = float_vector3d());
float_vector2d project_triangle_axis(const float_triangle3d& triangle, 
        const float_vector3d& axis, 
        const float_vector3d& origin = float_vector3d());

bool point_in_cube(const float_vector3d& point, 
        const float_vector3d& cube_min, const float_vector3d& cube_max) {
    return (cube_min.x < point.x && 
            cube_min.y < point.y && 
            cube_min.z < point.z &&
            point.x < cube_max.x && 
            point.y < cube_max.y && 
            point.z < cube_max.z);
}

bool point_in_cube_threshold(const float_vector3d& point, 
        const float_vector3d& cube_min, const float_vector3d& cube_max, 
        const float_vector3d::value_type threshold) {
    auto to_plane = [](const float_vector3d& arg, std::size_t i)
            ->float_vector2d {
        switch(i) {
        case 0:
            return float_vector2d(arg.x, arg.y);
            break;
        case 1:
            return float_vector2d(arg.y, arg.z);
            break;
        case 2:
            return float_vector2d(arg.z, arg.x);
            break;
        default:
            throw std::logic_error(
                    "Lambda to_plane should have i value < 3");
        }
    };
    auto to_plane_axis = [](const float_vector3d& arg, std::size_t i)
            ->float_vector2d::value_type {
        switch(i) {
        case 0:
            return arg.z;
            break;
        case 1:
            return arg.x;
            break;
        case 2:
            return arg.y;
            break;
        default:
            throw std::logic_error(
                    "Lambda to_plane_axis should have i value < 3");
        }
    };
    const float_vector3d::value_type threshold2 = threshold * threshold;
    for(std::size_t i = 0; i < 3; ++i) {
        const float_vector2d min_plane = to_plane(cube_min, i);
        const float_vector2d max_plane = to_plane(cube_max, i);
        const float_vector2d point_plane = to_plane(point, i);
        const float_vector2d::value_type point_axis = 
                to_plane_axis(point, i);
        const float_vector2d x_range(min_plane.x, max_plane.x);
        const float_vector2d y_range(min_plane.y, max_plane.y);
        const float_vector2d z_range(to_plane_axis(cube_min, i), 
                to_plane_axis(cube_max, i));
        const float_vector2d point_x_range(point_plane.x, point_plane.x);
        const float_vector2d point_y_range(point_plane.y, point_plane.y);
        const float_vector2d point_z_range(point_axis, point_axis);
        /*
         * Check if along two axes the point falls inside the cube
         * and along the third it is within the threshold distance 
         * to the faces perpendicular to that plane
         */
        if(ranges_overlap(point_x_range, x_range) && 
                ranges_overlap(point_y_range, y_range) && 
                -threshold < range_overlap_dinstance(point_z_range, z_range)) {
            return true;
        }
        const float_vector2d::value_type x_distance = std::min(
                float_vector2d::value_type(0), 
                range_overlap_dinstance(point_x_range, x_range));
        const float_vector2d::value_type y_distance = std::min(
                float_vector2d::value_type(0), 
                range_overlap_dinstance(point_y_range, y_range));
        const float_vector2d::value_type xy_squared_magnitude = 
                x_distance * x_distance + y_distance * y_distance;
        /*
         * Check if along the third axis the point falls inside the cube
         * and along the other two, it is within the threshold distance 
         * to an edge
         */
        if(ranges_overlap(point_z_range, z_range) && 
                xy_squared_magnitude < threshold2) {
            return true;
        }
    }
    /*
     * Check if the point is within threshold distance to corner
     */
    for(std::size_t i = 0; i < 8; ++i) {
        const float_vector3d corner = float_vector3d(
                i & 1 ? cube_max.x : cube_min.x, 
                i & 2 ? cube_max.y : cube_min.y, 
                i & 4 ? cube_max.z : cube_min.z);
        const float_vector3d offset = point - corner;
        if(offset.squared_magnitude() < threshold2) {
            return true;
        }
    }
    return false;
}

bool triangle_in_cube1(const float_triangle3d& triangle, 
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

bool triangle_in_cube2(const float_triangle3d& triangle, 
        const float_vector3d& cube_min, const float_vector3d& cube_max) {
    //compute the limits of the triangle
    float_vector3d triangle_min, triangle_max;
    std::tie(triangle_min, triangle_max) = triangle_limits(triangle);
    const std::array<const float_vector3d, 20> axes = {{
        float_vector3d(1,0,0),
        float_vector3d(0,1,0),
        float_vector3d(0,0,1),
        float_vector3d(1,1,0),
        float_vector3d(1,-1,0),
        float_vector3d(0,1,1),
        float_vector3d(0,1,-1),
        float_vector3d(1,0,1),
        float_vector3d(-1,0,1),
        float_vector3d(1,1,1),
        float_vector3d(-1,1,1),
        float_vector3d(1,-1,1),
        float_vector3d(1,1,-1),
        triangle[3],
        (triangle[1]-triangle[0]),
        (triangle[2]-triangle[1]),
        (triangle[0]-triangle[2]),
        (triangle[1]-triangle[0]).cross(triangle[3]),
        (triangle[2]-triangle[1]).cross(triangle[3]),
        (triangle[0]-triangle[2]).cross(triangle[3])}};
    const float_vector3d origin;
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

bool triangle_in_cube_threshold1(const float_triangle3d& triangle, 
        const float_vector3d& cube_min, const float_vector3d& cube_max, 
        const float_vector3d::value_type threshold) {
    for(std::size_t i = 0; i < 3; ++i) {
        if(point_in_cube_threshold(triangle[i], cube_min, cube_max, 
                threshold)) {
            return true;
        }
    }
    return false;
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

bool ranges_overlap(const float_vector2d& lhs, const float_vector2d& rhs) {
    return !(lhs.y < rhs.x || rhs.y < lhs.x);
}

bool ranges_overlap(const std::pair<float, float>& lhs, 
        const std::pair<float, float>& rhs) {
    return ranges_overlap(float_vector2d(lhs.first, lhs.second), 
            float_vector2d(rhs.first, rhs.second));
}

/**
 * @brief Get the overlapping distance of the two ranges
 * @param lhs x is minimum and y is maximum of this range
 * @param rhs x is minimum and y is maximum of this range
 * @return Positive overlapping distance if they overlap, 
 * negative separation distance if they are separate, and 0
 * for contact
 */
float_vector2d::value_type range_overlap_dinstance(const float_vector2d& lhs, 
        const float_vector2d& rhs) {
    return std::min(lhs.y, rhs.y) - std::max(lhs.x, rhs.x);
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


}
