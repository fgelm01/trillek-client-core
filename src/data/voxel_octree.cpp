#include "data/voxel_octree.h"

namespace trillek
{

voxel_octree::voxel_octree()
{
    //ctor
}

voxel_octree::~voxel_octree()
{
    //dtor
}

vector3d<std::size_t> voxel_octree::get_size() const
{
    return vector3d<std::size_t>(size,size,size);
}
const voxel& voxel_octree::get_voxel(std::size_t x,
                                     std::size_t y,
                                     std::size_t z) const
{
    return voxel();
}

void voxel_octree::set_voxel(std::size_t x,
                             std::size_t y,
                             std::size_t z,
                             const voxel& v)
{
}

}
