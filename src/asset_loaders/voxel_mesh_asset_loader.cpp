#include "asset_loaders/voxel_mesh_asset_loader.h"
#include "data/mesh_data.h"
#include "data/voxel_octree.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stdexcept>
#include <array>
#include <cassert>
#include <iostream>
#include <limits>

namespace trillek {

typedef vector2d<float> float_vector2d;
typedef vector3d<float> float_vector3d;
typedef std::array<float_vector3d, 4> float_triangle3d;
typedef std::vector<float_triangle3d> triangle3d_vector;

/**
 * @brief Test if z ray at xy intersects triangle
 * @param triangle
 * @param xy
 * @param out Z position of intersection if any
 * @return true if intersected, false otherwise
 */
bool z_ray_intersection(const float_triangle3d& triangle, 
        const float_vector2d& xy, float& out);

voxel_mesh_asset_loader::voxel_mesh_asset_loader() {}
voxel_mesh_asset_loader::~voxel_mesh_asset_loader() {}
data* voxel_mesh_asset_loader::load(const std::string& file) const {
    Assimp::Importer assimporter;
    const aiScene* scene = 
            assimporter.ReadFile(file,
                                 aiProcess_JoinIdenticalVertices |
                                 aiProcess_OptimizeGraph |
                                 aiProcess_OptimizeMeshes |
                                 aiProcess_RemoveComponent |
                                 aiProcess_Triangulate | 
                                 aiProcess_PreTransformVertices);
    if(!scene) {
        throw std::runtime_error(assimporter.GetErrorString());
    }
    triangle3d_vector all_triangles;
    std::cerr << "File has " << scene->mNumMeshes << " meshes" << std::endl;
    //we ignore most things and process the meshes only
    for(std::size_t i = 0; i < scene->mNumMeshes; ++i) {
        const aiMesh& cur_mesh = *(scene->mMeshes[i]);
        std::cerr << "Mesh " << i << " has " << cur_mesh.mNumFaces << 
                " faces" << std::endl;
        for(std::size_t face_i = 0; face_i < cur_mesh.mNumFaces; ++face_i) {
            const aiFace& face = cur_mesh.mFaces[face_i];
            float_triangle3d cur_face;
            for(std::size_t vertex_i = 0; vertex_i < face.mNumIndices; 
                    ++vertex_i) {
                assert(vertex_i < 3);
                const aiVector3D& vertex = 
                        cur_mesh.mVertices[face.mIndices[vertex_i]];
                cur_face[vertex_i] = make_vector3d(vertex.x, 
                                                   vertex.y, 
                                                   vertex.z);
            }
            const float_vector3d normal_vector = 
                    (cur_face[1] - cur_face[0]).cross(
                    cur_face[2] - cur_face[0]).normalize();
            cur_face[3] = normal_vector;
            all_triangles.emplace_back(std::move(cur_face));
        }
    }
    std::cerr << "Extracted " << all_triangles.size() 
            << " triangles" << std::endl;
    const float_vector3d::value_type MIN_VAL = 
            std::numeric_limits<float_vector3d::value_type>::min();
    const float_vector3d::value_type MAX_VAL = 
            std::numeric_limits<float_vector3d::value_type>::max();
    float_vector3d min_extent = make_vector3d(MAX_VAL, MAX_VAL, MAX_VAL);
    float_vector3d max_extent = make_vector3d(MIN_VAL, MIN_VAL, MIN_VAL);
    for(const float_triangle3d& cur_triangle : all_triangles) {
        for(const float_vector3d& cur_vector : cur_triangle) {
            min_extent = make_vector3d(
                    std::min(min_extent.x, cur_vector.x), 
                    std::min(min_extent.y, cur_vector.y), 
                    std::min(min_extent.z, cur_vector.z));
            max_extent = make_vector3d(
                    std::max(max_extent.x, cur_vector.x), 
                    std::max(max_extent.y, cur_vector.y), 
                    std::max(max_extent.z, cur_vector.z));
        }
    }
    std::cerr << "Extents are " << min_extent << ", " << 
            max_extent << std::endl;
    voxel_data::size_vector3d extent = max_extent - min_extent;
    for(std::size_t y = 0; y < extent.y; ++y) {
        for(std::size_t x = 0; x < extent.x; ++x) {
            float_vector2d xy = make_vector2d(x, y);
            std::cerr << "I work at " << xy << std::endl;
            for(const float_triangle3d cur_triangle : all_triangles) {
                float out;
                z_ray_intersection(cur_triangle, 
                        min_extent.to_vector2d() + xy, out);
            }
        }
    }
    return nullptr;
}

bool z_ray_intersection(const float_triangle3d& triangle, 
        const float_vector2d& xy, float& out) {
    if(triangle[3].z == 0) {
        //triangle is vertical, fail
        return false;
    }
    //test containment
    for(std::size_t i = 0; i != 3; ++i) {
        std::size_t j = (i + 1) % 3;
        const float_vector2d ij = triangle[j].to_vector2d() - 
                triangle[i].to_vector2d();
        const float_vector2d ixy = xy - triangle[i].to_vector2d();
        if(ij.cross(ixy) < 0) {
            return false;
        }
    }
    const float_vector3d& norm = triangle[3];
    float_vector2d point = xy - triangle[0].to_vector2d();
    out = triangle[0].z - point.x * (norm.x / norm.z) - 
            point.y * (norm.y / norm.z);
    std::cerr << "A triangle with coordinates " << 
            "\n" << triangle[0] << 
            "\n" << triangle[1] <<
            "\n" << triangle[2] <<
            "\n" << triangle[3] << std::endl;
    std::cerr << "Intersecting at " << xy << std::endl;
    std::cerr << "Results at " << out << std::endl;
    return true;
}

}
