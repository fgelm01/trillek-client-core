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
#include <map>
#include <set>
#include <functional>
#include <algorithm>

namespace trillek {

typedef vector2d<float> float_vector2d;
typedef vector3d<float> float_vector3d;
typedef vector2d<std::size_t> size_vector2d;
typedef vector2d<signed long int> int_vector2d;
typedef std::array<float_vector3d, 4> float_triangle3d;
typedef std::vector<float_triangle3d> triangle3d_vector;
typedef std::map<int_vector2d, std::vector<
        std::reference_wrapper<const float_triangle3d> > >
        bucket_map;

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
        size_t index = 0;
        for(const float_vector3d& cur_vector : cur_triangle) {
            if(++index>=3) break;
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
    min_extent -= make_vector3d(4,4,4);
    max_extent += make_vector3d(3,3,3);
    std::cerr << "Extents are " << min_extent << ", " << 
            max_extent << std::endl;
    bucket_map buckets; //bucket triangles by xy coordinates
    for(const float_triangle3d& cur_triangle : all_triangles) {
        float_vector2d min_triextents = make_vector2d(MAX_VAL, MAX_VAL);
        float_vector2d max_triextents = make_vector2d(MIN_VAL, MIN_VAL);
        size_t index = 0;
        for(const float_vector3d& cur_vector : cur_triangle) {
            if(++index>=3) break;
            min_triextents = make_vector2d(
                    std::min(min_triextents.x, cur_vector.x),
                    std::min(min_triextents.y, cur_vector.y));
            max_triextents = make_vector2d(
                    std::max(max_triextents.x, cur_vector.x),
                    std::max(max_triextents.y, cur_vector.y));
        }
        int_vector2d min_bucket = min_triextents;
        int_vector2d max_bucket = max_triextents;
        max_bucket += make_vector2d(1,1);
        min_bucket -= make_vector2d(1,1);
        for(int_vector2d::value_type y = min_bucket.y; 
                y <= max_bucket.y; ++y) {
            for(int_vector2d::value_type x = min_bucket.x; 
                    x <= max_bucket.x; ++x) {
                buckets[make_vector2d(x,y)].push_back(std::ref(cur_triangle));
            }
        }
    }
    voxel_data::size_vector3d extents = (max_extent - min_extent) + 
            make_vector3d(4,4,4);
    voxel_octree* ret = new voxel_octree();
    ret->reserve_space(extents);
    std::cerr << "There are " << buckets.size() << " buckets" << std::endl;
    for(const bucket_map::value_type& vt : buckets) {
        int_vector2d xy = vt.first;
        size_vector2d xyvoxel = xy - min_extent.to_vector2d();
//        std::cerr << "\nI work at " << xy << std::endl;
//        std::cerr << "I see " << vt.second.size() << " triangles" << std::endl;
        std::set<float> crossings;
        for(const float_triangle3d& cur_triangle : vt.second) {
            float out;
            if(z_ray_intersection(cur_triangle, xy, out)) {
                //crossings.push_back(out);
                crossings.insert(out);
            }
        }
//        std::cerr << "\nI see " << crossings.size() << 
//                " crossings" << std::endl;
        //std::sort(crossings.begin(), crossings.end());
        for(std::set<float>::iterator iter = crossings.begin(); 
                iter != crossings.end(); 
                ++iter) {
            std::set<float>::iterator next = iter;
            ++next;
            if(next != crossings.end()) {
                for(std::size_t z = 
                        static_cast<size_t>(*iter - min_extent.z);
                        z <= static_cast<size_t>(*next - min_extent.z); 
                        ++z) {
                    ret->set_voxel(xyvoxel.x, xyvoxel.y, z, 
                            voxel(true, true));
                }
                iter = next;
            } else {
                std::cerr << "MANIFOLD PROBLEM!" << std::endl;
            }
        }
    }
    return ret;
}

bool z_ray_intersection(const float_triangle3d& triangle, 
        const float_vector2d& xy, float& out) {
    static const double ERROR = 1.f / (1 << 18);
    if(std::abs(triangle[3].z) == 0) {
        //triangle is vertical, fail
        return false;
    }
    //test containment
//    std::cerr << "\nA triangle with coordinates " << 
//            "\n" << triangle[0] << 
//            "\n" << triangle[1] <<
//            "\n" << triangle[2] <<
//            "\n" << triangle[3] << std::endl;
//    std::cerr << "Intersecting at " << xy << std::endl;
    typedef vector2d<double> double_vector2d;
    std::size_t neg = 0;
    std::size_t pos = 0;
    for(std::size_t i = 0; i != 3; ++i) {
        std::size_t j = (i + 1) % 3;
        const double_vector2d ij = 
                double_vector2d(triangle[j].to_vector2d()) - 
                double_vector2d(triangle[i].to_vector2d());
        const double_vector2d ixy = 
                double_vector2d(xy) - 
                double_vector2d(triangle[i].to_vector2d());
        const double cross_val = ij.cross(ixy);
//        std::cerr << ij << " x " << ixy << " = " << cross_val << std::endl;
        if(cross_val < -ERROR) {
            ++neg;
        } else if(cross_val > ERROR) {
            ++pos;
        } else {
            std::cerr << "Value is " << cross_val << std::endl;
            ++neg;
            ++pos;
        }
    }
    if(std::max(neg, pos) != 3) {
//        std::cerr << "Fails" << std::endl;
        return false;
    }
    const float_vector3d& norm = triangle[3];
    float_vector2d point = xy - triangle[0].to_vector2d();
    out = triangle[0].z - point.x * (norm.x / norm.z) - 
            point.y * (norm.y / norm.z);
//    std::cerr << "Results at " << out << std::endl;
    return true;
}

}
