#include "data/voxel_array.h"
#include <iostream>
namespace trillek
{

voxel_array::voxel_array(std::size_t w, std::size_t h, std::size_t d)
    : size(w,h,d)
{
    reserve_space(w,h,d);
}

voxel_array::~voxel_array()
{
    //dtor
}
voxel voxel_array::get_voxel(   std::size_t x,
                                std::size_t y,
                                std::size_t z) const
{
    return this->data.at(x).at(y).at(z);
}

void voxel_array::set_voxel(std::size_t x,
                            std::size_t y,
                            std::size_t z,
                            voxel v)
{
    this->data[x][y][z]=v;
}

void voxel_array::reserve_space(std::size_t w,
                                std::size_t h,
                                std::size_t d)
{
    this->data.resize(w);
    for(std::size_t x=0;x<w;++x)
    {
        this->data[x].resize(h);
        for(std::size_t y=0;y<h;++y)
        {
            this->data[x][y].resize(d);
        }
    }
}

}
