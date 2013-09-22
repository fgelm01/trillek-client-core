#ifndef TRILLEK_VOXELIZE_MESH_UTILS_H
#define	TRILLEK_VOXELIZE_MESH_UTILS_H

#include "voxelize_mesh_defines.h"

namespace trillek {

extern const float_vector3d::value_type MIN_VAL;
extern const float_vector3d::value_type MAX_VAL;

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
/**
 * @brief Get the overlapping distance of the two ranges
 * @param lhs x is minimum and y is maximum of this range
 * @param rhs x is minimum and y is maximum of this range
 * @return Positive overlapping distance if they overlap, 
 * negative saparation distance if they are separete, and 0
 * for contact
 */
float_vector2d::value_type range_overlap_dinstance(const float_vector2d& lhs, 
        const float_vector2d& rhs);
/**
 * @brief Find the shortest distance between a line and a point
 * @param line_a line start point
 * @param line_b line end point
 * @param point what to find distance to
 * @param line_point will contain a point along the line to which 
 * the point passed in was closest
 * @return distance from line to point
 */
float_vector2d::value_type line_point_nearest_distance(
        const float_vector2d& line_a, const float_vector2d& line_b, 
        const float_vector2d& point, float_vector2d& line_point);
/**
 * @brief Find shortest distance between two line segments
 * @param line1_a line 1 start
 * @param line1_b line 1 end
 * @param line2_a line 2 start
 * @param line2_b line 2 end
 * @param line1_point point on line1 nearest to line2
 * @param line2_point point on line2 nearest to line1
 * @return shortest possible distance between them
 */
float_vector2d::value_type lines_nearest_distance(
        const float_vector2d& line1_a, const float_vector2d& line1_b, 
        const float_vector2d& line2_a, const float_vector2d& line2_b, 
        float_vector2d& line1_point, float_vector2d& line2_point);
/**
 * @brief Find shortest distance between two line segments
 * @param line1_a line 1 start
 * @param line1_b line 1 end
 * @param line2_a line 2 start
 * @param line2_b line 2 end
 * @return shortest possible distance between them
 */
float_vector3d::value_type lines_nearest_distance(
        const float_vector3d& line1_a, const float_vector3d& line1_b, 
        const float_vector3d& line2_a, const float_vector3d& line2_b);
/**
 * @brief Test if the point falls strictly inside the square
 */
bool point_in_square(const float_vector2d& point, 
        const float_vector2d& square_min, const float_vector2d& square_max);
bool point_in_square_threshold(const float_vector2d& point, 
        const float_vector2d& square_min, const float_vector2d& square_max, 
        const float_vector2d::value_type threshold);
/**
 * @brief Test if the point falls strictly inside the cube
 */
bool point_in_cube(const float_vector3d& point, 
        const float_vector3d& cube_min, const float_vector3d& cube_max);
/**
 * @brief Test if the point is within a certain distance of the cube
 */
bool point_in_cube_threshold(const float_vector3d& point, 
        const float_vector3d& cube_min, const float_vector3d& cube_max, 
        const float_vector3d::value_type threshold);
/**
 * @brief Test if the line overlaps the cube
 * Uses point_in_cube on the end points
 */
bool line_in_cube(const float_vector3d& line_a, 
        const float_vector3d& line_b, 
        const float_vector3d& cube_min, const float_vector3d& cube_max);
/**
 * @brief Test if the line is within a certain distance of the cube
 * Uses point_in_cube on the end points
 */
bool line_in_cube_threshold(
        const float_vector3d& line_a, const float_vector3d& line_b, 
        const float_vector3d& cube_min, const float_vector3d& cube_max, 
        const float_vector3d::value_type threshold);
/**
 * Tell if a triangle overlaps the cube
 * @param triangle triangle vertices and normal
 * @param cube_min cube minimum coordinate
 * @param cube_max cube maximum coordinate
 * @return whether they overlap
 * Explicitly tests the canonical axes, the triangle normal, 
 * and the edge perpendicular axes.
 */
bool triangle_in_cube1(const float_triangle3d& triangle, 
        const float_vector3d& cube_min, const float_vector3d& cube_max);
/**
 * Tell if a triangle overlaps the cube
 * @param triangle triangle vertices and normal
 * @param cube_min cube minimum coordinate
 * @param cube_max cube maximum coordinate
 * @return whether they overlap
 * Applies separating axes theorem to a long list of axes:
 * Three canonical axes
 * Combinations of two canonical axes
 * Combinations of three canonical axes
 * Triangle normal
 * Triangle edge parallel
 * Triangle edge perpendicular
 */
bool triangle_in_cube2(const float_triangle3d& triangle, 
        const float_vector3d& cube_min, const float_vector3d& cube_max);
/**
 * Tell if a triangle overlaps the cube
 * @param triangle triangle vertices and normal
 * @param cube_min cube minimum coordinate
 * @param cube_max cube maximum coordinate
 * @return whether they overlap
 * Actually tests the edges of the triangle only
 */
bool triangle_in_cube3(const float_triangle3d& triangle, 
        const float_vector3d& cube_min, const float_vector3d& cube_max);
/**
 * Tell if a triangle overlaps the cube
 * @param triangle triangle vertices and normal
 * @param cube_min cube minimum coordinate
 * @param cube_max cube maximum coordinate
 * @param threshold If minimal distance exceeds this, fail
 * @return whether they overlap
 * Uses distance tests to check if the minimal distance between the 
 * triangle and the edge of the cube falls below a threshold
 */
bool triangle_in_cube_threshold1(const float_triangle3d& triangle, 
        const float_vector3d& cube_min, const float_vector3d& cube_max, 
        const float_vector3d::value_type threshold);

}

#endif	/* TRILLEK_VOXELIZE_MESH_UTILS_H */

