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

vector3d<unsigned int> voxel_octree::get_size()
{
    return vector3d<unsigned int>(size,size,size);
}
voxel voxel_octree::get_voxel(  unsigned int x,
                                unsigned int y,
                                unsigned int z)
{

}

void voxel_octree::set_voxel(   unsigned int x,
                                unsigned int y,
                                unsigned int z,
                                voxel v)
{
}

}
