#ifndef marching_cubes_render_algorithm_H
#define marching_cubes_render_algorithm_H

#include "rendering/render_algorithm.h"
#include "data/mesh_data.h"
#include "rendering/voxel_model.h"
#include "math/axis_aligned_box.h"

namespace trillek
{

class marching_cubes_render_algorithm
    : public render_algorithm
{
    public:
        marching_cubes_render_algorithm();
        virtual ~marching_cubes_render_algorithm();
        void process(voxel_model* node, graphics_service* service);
        static unsigned char calculate_cube_num(voxel& v0,voxel& v1,
                                                voxel& v2,voxel& v3,
                                                voxel& v4,voxel& v5,
                                                voxel& v6,voxel& v7);
        static void step(vector3d<float> pos,
                         unsigned char num,
                         float size,
                         std::shared_ptr<mesh_data> model);
        static void step(axis_aligned_box& box,
                         unsigned char num,
                         std::shared_ptr<mesh_data> model);
        static void step(vector3d<float> p0,vector3d<float> p1,
                         vector3d<float> p2,vector3d<float> p3,
                         vector3d<float> p4,vector3d<float> p5,
                         vector3d<float> p6,vector3d<float> p7,
                         unsigned char cubeindex,
                         std::shared_ptr<mesh_data> model);
    protected:
    private:
};

}

#endif // marching_cubes_render_algorithm_H
