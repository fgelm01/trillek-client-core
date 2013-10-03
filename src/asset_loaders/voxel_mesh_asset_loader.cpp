#include "asset_loaders/voxel_mesh_asset_loader.h"
#include "data/mesh_data.h"
#include "data/voxel_octree.h"
#include "data/voxel_array.h"
#include "data/basic_octree.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include "voxelize_mesh_defines.h"
#include "voxelize_ray_trace.h"
#include "voxelize_surface.h"

namespace trillek {

/**
 * Make sure the maximum extent of the mesh is less than 
 * max_extent
 * @param all_triangles all the triangles
 * @param max_extent maximum extent
 * Absolute positions may change, but proportions are preserved
 */
void limit_mesh_extent(triangle3d_vector& all_triangles, 
        const float max_extent);

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
    //limit_mesh_extent(all_triangles, 496);
    //return new voxel_octree(voxelize_mesh_ray_trace(all_triangles, 1));
    return new voxel_octree(voxelize_mesh_surface(all_triangles, 6.0));
}

void limit_mesh_extent(triangle3d_vector& all_triangles, 
        const float max_extent) {
    //float or double or similar
    static const auto MIN_VAL = std::numeric_limits<
            triangle3d_vector::value_type::value_type::value_type>::min();
    //float or double or similar
    static const auto MAX_VAL = std::numeric_limits<
            triangle3d_vector::value_type::value_type::value_type>::max();
    float_vector3d min_xyz(MAX_VAL, MAX_VAL, MAX_VAL);
    float_vector3d max_xyz(MIN_VAL, MIN_VAL, MIN_VAL);
    for(const float_triangle3d& triangle : all_triangles) {
        for(std::size_t i = 0; i < 3; ++i) {
            const float_vector3d cur_vertex = triangle[i];
            min_xyz.x = std::min(min_xyz.x, cur_vertex.x);
            min_xyz.y = std::min(min_xyz.y, cur_vertex.y);
            min_xyz.z = std::min(min_xyz.z, cur_vertex.z);
            max_xyz.x = std::max(max_xyz.x, cur_vertex.x);
            max_xyz.y = std::max(max_xyz.y, cur_vertex.y);
            max_xyz.z = std::max(max_xyz.z, cur_vertex.z);
        }
    }
    const float actual_extent = std::max(max_xyz.z - min_xyz.z, 
            std::max(max_xyz.y - min_xyz.y, 
            max_xyz.x - min_xyz.x));
    if(actual_extent > max_extent) {
        const float ratio = max_extent / actual_extent;
        for(float_triangle3d& triangle : all_triangles) {
            for(std::size_t i = 0; i < 3; ++i) {
                triangle[i] *= ratio;
            }
        }
    }
}


}
