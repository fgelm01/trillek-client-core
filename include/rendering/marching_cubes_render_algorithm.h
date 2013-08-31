#ifndef marching_cubes_render_algorithm_H
#define marching_cubes_render_algorithm_H

#include "rendering/render_algorithm.h"
#include "data/mesh_data.h"
#include "rendering/voxel_model.h"

namespace trillek
{

class marching_cubes_render_algorithm
    : public render_algorithm
{
    public:
        marching_cubes_render_algorithm();
        virtual ~marching_cubes_render_algorithm();
        void process(voxel_model* node, graphics_service* service);
    protected:
    private:
        void step(  vector3d<float> pos,
                    unsigned char num,
                    float size,
                    graphics_service* service,
                    std::shared_ptr<mesh_data> model);
};

}

#endif // marching_cubes_render_algorithm_H
