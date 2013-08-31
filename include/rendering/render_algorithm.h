#ifndef RENDER_ALGORITHM_H
#define RENDER_ALGORITHM_H

#include "rendering/render_tree.h"
#include "rendering/voxel_model.h"
#include "data/voxel_data.h"

namespace trillek
{

class graphics_service;

class render_algorithm
{
    public:
        render_algorithm();
        virtual ~render_algorithm();
        virtual void process(voxel_model* node,
                             graphics_service* service)=0;
    protected:
    private:
};

}

#endif // RENDER_ALGORITHM_H
