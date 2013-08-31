#include "rendering/voxel_model.h"
#include "data/voxel_array.h"

namespace trillek
{

voxel_model::voxel_model(int x, int y, int z)
{
    this->data = std::make_shared<voxel_array>(x,y,z);
}

voxel_model::~voxel_model()
{
    //dtor
}

void voxel_model::set_voxel(int x, int y, int z, voxel v)
{
    std::dynamic_pointer_cast<voxel_data>(this->data)->set_voxel(x,y,z,v);
}

}
