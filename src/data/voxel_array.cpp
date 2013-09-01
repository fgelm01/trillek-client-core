#include "data/voxel_array.h"
#include <iostream>
namespace trillek
{

voxel_array::voxel_array(unsigned int w, unsigned int h, unsigned int d)
    : size(w,h,d)
{
    reserve_space(w,h,d);
}

voxel_array::~voxel_array()
{
    //dtor
}
voxel voxel_array::get_voxel(   unsigned int x,
                                unsigned int y,
                                unsigned int z)
{
    if(x<0||x>=size.x||
       y<0||y>=size.y||
       z<0||z>=size.z)
        return voxel();
    else
        return this->data[x][y][z];
}

void voxel_array::set_voxel(unsigned int x,
                            unsigned int y,
                            unsigned int z,
                            voxel v)
{
    this->data[x][y][z]=v;
}

void voxel_array::reserve_space(unsigned int w,
                                unsigned int h,
                                unsigned int d)
{
    this->data.resize(w);
    for(int x=0;x<w;++x)
    {
        this->data[x].resize(h);
        for(int y=0;y<h;++y)
        {
            this->data[x][y].resize(d);
        }
    }
}

voxel_octree voxel_array::to_octree()
{
}

}
