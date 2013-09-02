#include "data/voxel_array.h"
#include <iostream>
#include <cassert>
namespace trillek
{

voxel_array_base::voxel_array_base() {}
voxel_array_base::~voxel_array_base() {}
voxel_array_base::size_vector3d voxel_array_base::get_size() const {
    return _size;
}
voxel_octree voxel_array_base::to_octree() const {
    voxel_octree ret;
    ret.reserve_space(get_size().x, get_size().y, get_size().z);
    for(std::size_t z = 0; z != get_size().z; ++z) {
        for(std::size_t y = 0; y != get_size().y; ++y) {
            for(std::size_t x = 0; x != get_size().x; ++x) {
                ret.set_voxel(x, y, z, get_voxel(x, y, z));
            }
        }
    }
    return ret;
}

voxel_array::voxel_array(std::size_t w, std::size_t h, std::size_t d)
    : size(w,h,d)
{
    reserve_space(w,h,d);
}

voxel_array::~voxel_array()
{
    //dtor
}
const voxel& voxel_array::get_voxel(std::size_t x,
                             std::size_t y,
                             std::size_t z) const
{
    return this->data.at(x).at(y).at(z);
}

void voxel_array::set_voxel(std::size_t x,
                            std::size_t y,
                            std::size_t z,
                            const voxel& v)
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

voxel_array_alternate::voxel_array_alternate() {}
voxel_array_alternate::~voxel_array_alternate() {}
voxel_array_alternate::voxel_array_alternate(size_vector3d size) {
    reserve_space(size.x, size.y, size.z);
}
voxel_array_alternate::voxel_array_alternate(std::size_t x, 
                                             std::size_t y, 
                                             std::size_t z) {
    reserve_space(x, y, z);
}
const voxel& voxel_array_alternate::get_voxel(std::size_t x, 
                                              std::size_t y, 
                                              std::size_t z) const {
    return _data[compute_index(x, y, z)];
}
void voxel_array_alternate::set_voxel(std::size_t x, 
                                      std::size_t y, 
                                      std::size_t z, 
                                      const voxel& v) {
    _data[compute_index(x, y, z)] = v;
}
void voxel_array_alternate::reserve_space(std::size_t x, 
                                          std::size_t y, 
                                          std::size_t z) {
    _data.resize(x * y * z);
    _size = make_vector3d(x, y, z);
}
std::size_t voxel_array_alternate::compute_index(std::size_t x, 
                                                 std::size_t y, 
                                                 std::size_t z) const {
    assert(x < _size.x);
    assert(y < _size.y);
    assert(z < _size.z);
    return z * _size.x * _size.y + y * _size.x + x;
}

}
