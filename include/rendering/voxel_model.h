#ifndef VOXEL_MODEL_H
#define VOXEL_MODEL_H

#include "rendering/render_tree.h"
#include "data/voxel_data.h"
#include <iostream>
namespace trillek
{

class voxel_model : public render_tree_node
{
    public:
        voxel_model(int x, int y, int z);
        virtual ~voxel_model();
        virtual node_type get_render_type(){return voxel_node;};
        voxel_data* get_render_data(){return (voxel_data*)data.get();};
        void set_render_data(voxel_data* _data)
        {this->data=std::shared_ptr<voxel_data>(_data);}
        void set_voxel(int x, int y, int z, voxel v);
        bool needs_redraw(){return false;/*TODO:Implement logic*/}
    protected:
    private:
        std::shared_ptr<render_data> data;
};

}

#endif // VOXEL_MODEL_H
