#include "voxelize_mesh_utils.h"
#include <limits>
#include <algorithm>

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
float_vector2d to_plane(const float_vector3d& arg, std::size_t);
float_vector2d::value_type to_axis(const float_vector3d& arg, std::size_t);

bool point_in_square(const float_vector2d& point, 
        const float_vector2d& square_min, const float_vector2d& square_max) {
    return (square_min.x < point.x && 
            square_min.y < point.y && 
            point.x < square_max.x && 
            point.y < square_max.y);
}

bool point_in_square_threshold(const float_vector2d& point, 
        const float_vector2d& square_min, const float_vector2d& square_max, 
        const float_vector2d::value_type threshold) {
    const float_vector2d::value_type threshold2 = threshold * threshold;
    const float_vector2d square_x_range(square_min.x, square_max.x);
    const float_vector2d square_y_range(square_min.y, square_max.y);
    const float_vector2d point_x_range(point.x, point.x);
    const float_vector2d point_y_range(point.y, point.y);
    if(ranges_overlap(point_x_range, square_x_range) && 
            -threshold < range_overlap_dinstance(
            point_y_range, square_y_range)) {
        return true;
    }
    if(ranges_overlap(point_y_range, square_y_range) && 
            -threshold < range_overlap_dinstance(
            point_x_range, square_x_range)) {
        return true;
    }
    for(std::size_t i = 0; i < 4; ++i) {
        float_vector2d corner(
                i & 1 ? square_max.x : square_min.x, 
                i & 2 ? square_max.y : square_min.y);
        if((point - corner).squared_magnitude() < threshold2) {
            return true;
        }
    }
    return false;
}

bool point_in_cube(const float_vector3d& point, 
        const float_vector3d& cube_min, const float_vector3d& cube_max) {
    return (cube_min.x < point.x && 
            cube_min.y < point.y && 
            cube_min.z < point.z &&
            point.x < cube_max.x && 
            point.y < cube_max.y && 
            point.z < cube_max.z);
}

bool line_in_cube(const float_vector3d& line_a, 
        const float_vector3d& line_b, 
        const float_vector3d& cube_min, const float_vector3d& cube_max) {
    /*
     * Test if either end point falls inside the cube
     */
    if(point_in_cube(line_a, cube_min, cube_max) || 
            point_in_cube(line_b, cube_min, cube_max)) {
        return true;
    }
    for(std::size_t i = 0; i < 3; ++i) {
        const float_vector2d min_plane = to_plane(cube_min, i);
        const float_vector2d max_plane = to_plane(cube_max, i);
        const float_vector2d a_plane = to_plane(line_a, i);
        const float_vector2d b_plane = to_plane(line_b, i);
        const float_vector2d::value_type min_axis = to_axis(cube_min, i);
        const float_vector2d::value_type max_axis = to_axis(cube_max, i);
        const float_vector2d::value_type a_axis = to_axis(line_a, i);
        const float_vector2d::value_type b_axis = to_axis(line_b, i);
        const float_vector2d line_z_range(
                std::min(a_axis, b_axis), 
                std::max(a_axis, b_axis));
        const std::array<const float_vector2d::value_type, 2> axis_points = 
                {{min_axis, max_axis}};
        //test if the line intersects these faces
        for(const float_vector2d::value_type axis_point : axis_points) {
            const float_vector2d axis_range(axis_point, axis_point);
            if(!ranges_overlap(line_z_range, axis_range)) {
                continue;
            }
            const float_vector2d::value_type ratio = (axis_point - a_axis) / 
                    (b_axis - a_axis);
            const float_vector2d plane_point = a_plane + ratio * 
                    (b_plane - a_plane);
            if(point_in_square(plane_point, min_plane, max_plane)) {
                return true;
            }
        }
    }
    return false;
}

bool line_in_cube_threshold(
        const float_vector3d& line_a, const float_vector3d& line_b, 
        const float_vector3d& cube_min, const float_vector3d& cube_max, 
        const float_vector3d::value_type threshold) {
    if(point_in_cube_threshold(line_a, cube_min, cube_max, threshold) || 
            point_in_cube_threshold(line_b, cube_min, cube_max, threshold)) {
        return true;
    }
    for(std::size_t i = 0; i < 3; ++i) {
        const float_vector2d min_plane = to_plane(cube_min, i);
        const float_vector2d max_plane = to_plane(cube_max, i);
        const float_vector2d a_plane = to_plane(line_a, i);
        const float_vector2d b_plane = to_plane(line_b, i);
        const float_vector2d::value_type min_axis = to_axis(cube_min, i);
        const float_vector2d::value_type max_axis = to_axis(cube_max, i);
        const float_vector2d::value_type a_axis = to_axis(line_a, i);
        const float_vector2d::value_type b_axis = to_axis(line_b, i);
        const float_vector2d line_z_range(
                std::min(a_axis, b_axis), 
                std::max(a_axis, b_axis));
        const std::array<const float_vector2d::value_type, 2> axis_points = 
                {{min_axis, max_axis}};
        //test if the line intersects these 2 parallel faces
        for(const float_vector2d::value_type axis_point : axis_points) {
            //continue;
            const float_vector2d axis_range(axis_point, axis_point);
            if(!ranges_overlap(line_z_range, axis_range)) {
                continue;
            }
            const float_vector2d::value_type ratio = (axis_point - a_axis) / 
                    (b_axis - a_axis);
            const float_vector2d plane_point = a_plane + ratio * 
                    (b_plane - a_plane);
            if(point_in_square_threshold(plane_point, min_plane, max_plane, 
                    threshold)) {
                return true;
            }
        }
    }
    return false;
}

bool point_in_triangle_threshold(
        const float_vector3d& point, const float_triangle3d& triangle, 
        const float_vector3d::value_type threshold) {
    auto select_y = [&triangle]()->trillek::float_vector3d {
        std::size_t best_base = 0;
        float_vector3d::value_type best_length = 
                std::numeric_limits<float_vector3d::value_type>::min();
        for(std::size_t i = 0; i < 3; ++i) {
            const std::size_t j = (i + 1) % 3;
            const float_vector3d::value_type length = 
                    (triangle[j] - triangle[i]).squared_magnitude();
            if(length > best_length) {
                best_base = i;
                best_length = length;
            }
        }
        const std::size_t best_other = (best_base + 1) % 3;
        return (triangle[best_other] - triangle[best_base]).normalize();
    };
    const float_vector3d origin = triangle[0];
    const float_vector3d z_axis = triangle[3];
    const float_vector3d x_axis = select_y();
    const float_vector3d y_axis = z_axis.cross(x_axis);
    const float_triangle2d triangle_transformed = 
            {{float_vector2d(
                    (triangle[0] - origin).dot(x_axis),
                    (triangle[0] - origin).dot(y_axis)), 
              float_vector2d(
                    (triangle[1] - origin).dot(x_axis),
                    (triangle[1] - origin).dot(y_axis)), 
              float_vector2d(
                    (triangle[2] - origin).dot(x_axis),
                    (triangle[2] - origin).dot(y_axis))}};
    const float_vector2d point_transformed(
            (point - origin).dot(x_axis), 
            (point - origin).dot(y_axis));
    //determine if the point is inside the 2d triangle
    for(std::size_t i = 0; i < 3; ++i) {
        const std::size_t j = (i + 1) % 3;
        float_vector2d edge = triangle_transformed[j] - 
                triangle_transformed[i];
        const float_vector2d::value_type cross_value = edge.cross(
                point_transformed - triangle_transformed[i]);
        if(cross_value < 0) {
            return false;
        }
    }
    const float_vector3d::value_type point_z = (point - origin).dot(z_axis);
    return std::abs(point_z) < threshold;
}

bool line_in_triangle_threshold(
        const float_vector3d& line_a, const float_vector3d& line_b, 
        const float_triangle3d& triangle, 
        const float_vector3d::value_type threshold) {
    auto select_y = [&triangle]()->trillek::float_vector3d {
        std::size_t best_base = 0;
        float_vector3d::value_type best_length = 
                std::numeric_limits<float_vector3d::value_type>::min();
        for(std::size_t i = 0; i < 3; ++i) {
            const std::size_t j = (i + 1) % 3;
            const float_vector3d::value_type length = 
                    (triangle[j] - triangle[i]).squared_magnitude();
            if(length > best_length) {
                best_base = i;
                best_length = length;
            }
        }
        const std::size_t best_other = (best_base + 1) % 3;
        return (triangle[best_other] - triangle[best_base]).normalize();
    };
    const float_vector3d origin = triangle[0];
    const float_vector3d z_axis = triangle[3];
    const float_vector3d x_axis = select_y();
    const float_vector3d y_axis = z_axis.cross(x_axis);
    const float_triangle2d triangle_transformed = 
            {{float_vector2d(
                    (triangle[0] - origin).dot(x_axis),
                    (triangle[0] - origin).dot(y_axis)), 
              float_vector2d(
                    (triangle[1] - origin).dot(x_axis),
                    (triangle[1] - origin).dot(y_axis)), 
              float_vector2d(
                    (triangle[2] - origin).dot(x_axis),
                    (triangle[2] - origin).dot(y_axis))}};
    const float_vector2d line_a_transformed(
            (line_a - origin).dot(x_axis), 
            (line_a - origin).dot(y_axis));
    const float_vector2d line_b_transformed(
            (line_b - origin).dot(x_axis), 
            (line_b - origin).dot(y_axis));
    const float_vector3d::value_type line_a_z = (line_a - origin).dot(z_axis);
    const float_vector3d::value_type line_b_z = (line_b - origin).dot(z_axis);
    auto perform_test = [&triangle_transformed, threshold](
            const float_vector2d& point, 
            const float_vector3d::value_type point_z)->bool {
        for(std::size_t i = 0; i < 3; ++i) {
            const std::size_t j = (i + 1) % 3;
            float_vector2d edge = triangle_transformed[j] - 
                    triangle_transformed[i];
            const float_vector2d::value_type cross_value = edge.cross(
                    point - triangle_transformed[i]);
            if(cross_value < 0) {
                return false;
            }
        }
        return std::abs(point_z) < threshold;
    };
    if(std::min(line_a_z, line_b_z) < 0 && 
            std::max(line_a_z, line_b_z) > 0) {
        const float_vector3d::value_type intersect_ratio = 
                (0 - line_a_z) / (line_b_z - line_a_z);
        const float_vector2d intersect_point = line_a_transformed + 
                intersect_ratio * (line_b_transformed - 
                line_a_transformed);
        return perform_test(intersect_point, 0);
    } else {
        return perform_test(line_a_transformed, line_a_z) || 
                perform_test(line_b_transformed, line_b_z);
    }
}

bool point_in_cube_threshold(const float_vector3d& point, 
        const float_vector3d& cube_min, const float_vector3d& cube_max, 
        const float_vector3d::value_type threshold) {
    const float_vector3d::value_type threshold2 = threshold * threshold;
    for(std::size_t i = 0; i < 3; ++i) {
        const float_vector2d min_plane = to_plane(cube_min, i);
        const float_vector2d max_plane = to_plane(cube_max, i);
        const float_vector2d point_plane = to_plane(point, i);
        const float_vector2d::value_type point_axis = 
                to_axis(point, i);
        const float_vector2d x_range(min_plane.x, max_plane.x);
        const float_vector2d y_range(min_plane.y, max_plane.y);
        const float_vector2d z_range(to_axis(cube_min, i), 
                to_axis(cube_max, i));
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

bool triangle_in_cube3(const float_triangle3d& triangle, 
        const float_vector3d& cube_min, const float_vector3d& cube_max) {
    for(std::size_t i = 0; i < 3; ++i) {
        const std::size_t j = (i + 1) % 3;
        if(line_in_cube(triangle[i], triangle[j], cube_min, cube_max)) {
            return true;
        }
    }
    return false;
}

bool triangle_in_cube_threshold1(const float_triangle3d& triangle, 
        const float_vector3d& cube_min, const float_vector3d& cube_max, 
        const float_vector3d::value_type threshold) {
    auto make_corner = [&cube_min, cube_max](
            const std::size_t i)->trillek::float_vector3d {
        return float_vector3d(
                i & 1 ? cube_max.x : cube_min.x, 
                i & 2 ? cube_max.y : cube_min.y, 
                i & 4 ? cube_max.z : cube_min.z);
    };
    //this piece checks the edges of the triangle against the cube
    for(std::size_t i = 0; i < 3; ++i) {
        std::size_t j = (i + 1) % 3;
        if(line_in_cube_threshold(triangle[i], triangle[j], 
                cube_min, cube_max, threshold)) {
            return true;
        }
    }
    //this piece checks the edges of the cube against the triangle
    for(std::size_t i = 0; i < 8; ++i) {
        const float_vector3d corner = make_corner(i);
        for(std::size_t shift = 0; shift < 3; ++shift) {
            const std::size_t shift_mask = 1 << shift;
            if(i & shift_mask) {
                continue;
            }
            const std::size_t j = i | shift_mask;
            const float_vector3d corner2 = make_corner(j);
            if(line_in_triangle_threshold(corner, corner2, 
                    triangle, threshold)) {
                return true;
            }
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

float_vector2d::value_type range_overlap_dinstance(const float_vector2d& lhs, 
        const float_vector2d& rhs) {
    return std::min(lhs.y, rhs.y) - std::max(lhs.x, rhs.x);
}

float_vector2d::value_type line_point_nearest_distance(
        const float_vector2d& line_a, const float_vector2d& line_b, 
        const float_vector2d& point, float_vector2d& line_point) {
    const float_vector2d line_dir = line_b - line_a;
    const float_vector2d::value_type projected = 
            (point - line_a).dot(line_dir);
    if(projected < 0) {
        line_point = line_a;
    } else if(projected > 1) {
        line_point = line_b;
    } else {
        line_point = line_a + projected * line_dir;
    }
    return (line_point - point).length();
}

float_vector2d::value_type lines_nearest_distance(
        const float_vector2d& line1_a, const float_vector2d& line1_b, 
        const float_vector2d& line2_a, const float_vector2d& line2_b, 
        float_vector2d& line1_point, float_vector2d& line2_point) {
    /*
    PVector segIntersection(float x1, float y1, float x2, 
    float y2, float x3, float y3, float x4, float y4) 
    { 
      float bx = x2 - x1; 
      float by = y2 - y1; 
      float dx = x4 - x3; 
      float dy = y4 - y3;
      float b_dot_d_perp = bx * dy - by * dx;
      if(b_dot_d_perp == 0) {
        return null;
      }
      float cx = x3 - x1;
      float cy = y3 - y1;
      float t = (cx * dy - cy * dx) / b_dot_d_perp;
      if(t < 0 || t > 1) {
        return null;
      }
      float u = (cx * by - cy * bx) / b_dot_d_perp;
      if(u < 0 || u > 1) { 
        return null;
      }
      return new PVector(x1+t*bx, y1+t*by);
    }
     */
    const float_vector2d line1_relative = line1_b - line1_a;
    const float_vector2d line2_relative = line2_b - line2_a;
    const float_vector2d::value_type rel_cross = line1_relative.cross(
            line2_relative);
    const float_vector2d orig_relative = line2_a - line1_a;
    const float_vector2d::value_type t = orig_relative.cross(line2_relative) / 
            rel_cross;
    const float_vector2d::value_type u = orig_relative.cross(line1_relative) / 
            rel_cross;
    if(t < 0.0 || t > 1.0 || u < 0.0 || u > 1.0) {
        float_vector2d l1a2, l1b2, l2a1, l2b1;
        const float_vector2d::value_type d1a2 = line_point_nearest_distance(
                line1_a, line1_b, line2_a, l1a2);
        const float_vector2d::value_type d1b2 = line_point_nearest_distance(
                line1_a, line1_b, line2_b, l1b2);
        const float_vector2d::value_type d2a1 = line_point_nearest_distance(
                line2_a, line2_b, line1_a, l2a1);
        const float_vector2d::value_type d2b1 = line_point_nearest_distance(
                line2_a, line2_b, line1_b, l2b1);
        const float_vector2d::value_type mindist = std::min(
                std::min(d1a2, d1b2), std::min(d2a1, d2b1));
        if(mindist == d1a2) {
            line1_point = l1a2;
            line2_point = line2_a;
            return d1a2;
        } else if(mindist == d1b2) {
            line1_point = l1b2;
            line2_point = line2_b;
        } else if(mindist == d2a1) {
            line1_point = line1_a;
            line2_point = l2a1;
        } else {
            line1_point = line1_b;
            line2_point = l2b1;
        }
        return mindist;
    } else {
        line1_point = line2_point = line1_a + t * line1_relative;
        return 0;
    }
}

float_vector3d::value_type lines_nearest_distance(
        const float_vector3d& line1_a, const float_vector3d& line1_b, 
        const float_vector3d& line2_a, const float_vector3d& line2_b) {
    const float_vector3d::value_type aa = (line1_a - 
            line2_a).squared_magnitude();
    const float_vector3d::value_type ab = (line1_a - 
            line2_b).squared_magnitude();
    const float_vector3d::value_type ba = (line1_b - 
            line2_a).squared_magnitude();
    const float_vector3d::value_type bb = (line1_b - 
            line2_b).squared_magnitude();
    const float_vector3d::value_type am = std::min(aa, ab);
    const float_vector3d::value_type bm = std::min(ba, bb);
    const float_vector3d::value_type closest_endpoint_distance = 
            std::sqrt(std::min(am, bm));
    const float_vector3d x = line1_b - line1_a;
    const float_vector3d y = line2_b - line2_a;
    const float_vector3d z = x.cross(y);
    if(z.squared_magnitude() == 0) {
        return closest_endpoint_distance;
    }
    //select the longer of these to be the new x
    //to avoid using something with a magnitude of 0.9e-7 or similar
    const float_vector3d x_norm = (x.squared_magnitude() > 
            y.squared_magnitude() ? x.normalize() : y.normalize());
    const float_vector3d z_norm = z.normalize();
    const float_vector3d y_norm = z_norm.cross(x);
    
    //reduce this to a 2d problem
    const float_vector2d line1_a2d(line1_a.dot(x_norm), line1_a.dot(y_norm));
    const float_vector2d line1_b2d(line1_b.dot(x_norm), line1_b.dot(y_norm));
    const float_vector2d line2_a2d(line2_a.dot(x_norm), line2_a.dot(y_norm));
    const float_vector2d line2_b2d(line2_b.dot(x_norm), line2_b.dot(y_norm));
    const float_vector2d line1_relative2d = line1_b2d - line1_a2d;
    const float_vector2d line2_relative2d = line1_b2d - line1_a2d;
    const float_vector2d::value_type line1_length2d = 
            line1_relative2d.length();
    const float_vector2d::value_type line2_length2d = 
            line2_relative2d.length();
    
    //find their nearest points in 2d
    float_vector2d line1_point2d, line2_point2d;
    lines_nearest_distance(line1_a2d, line1_b2d, line2_a2d, line2_b2d, 
            line1_point2d, line2_point2d);
    //compute their distance along the respective line as a ratio
    const float_vector2d::value_type line1_ratio2d = 
            (line1_point2d - line1_a2d).length() / line1_length2d;
    const float_vector2d::value_type line2_ratio2d = 
            (line2_point2d - line2_a2d).length() / line2_length2d;
    //get these points in 3d
    const float_vector3d line1_point = line1_a + line1_ratio2d * x;
    const float_vector3d line2_point = line2_a + line2_ratio2d * y;
    //we're done
    const float_vector3d::value_type line_distance = 
            (line1_point - line2_point).length();
    return std::min(line_distance, closest_endpoint_distance);
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

float_vector2d to_plane(const float_vector3d& arg, std::size_t i) {
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
                "Function to_plane should have i value < 3");
    }
};

float_vector2d::value_type to_axis(const float_vector3d& arg, std::size_t i) {
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
                "Function to_axis should have i value < 3");
    }
};


}
