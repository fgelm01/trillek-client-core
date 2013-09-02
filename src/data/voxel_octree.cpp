#include "data/voxel_octree.h"
#include "make_unique.h"
#include <cassert>
#include <algorithm>

namespace trillek
{

voxel_octree::voxel_octree() : _size_exp(0), _has_children(false) {}
voxel_octree::voxel_octree(const voxel_octree& other) 
        : _size_exp(other._size_exp), _data(other._data), 
        _has_children(other._has_children) {
    if(_has_children) {
        for(std::size_t i = 0; i != _children.size(); ++i) {
            _children[i] = make_unique<voxel_octree>(*other._children[i]);
        }
    }
}
voxel_octree::voxel_octree(voxel_octree&& other) 
        : _size_exp(other._size_exp), _data(std::move(other._data)), 
        _has_children(other._has_children) {
    if(_has_children) {
        for(std::size_t i = 0; i != _children.size(); ++i) {
            _children[i] = std::move(other._children[i]);
        }
    }
}
voxel_octree::voxel_octree(std::size_t size_exp, const voxel& data) 
        : _size_exp(size_exp), _data(data), _has_children(false) {}
voxel_octree::~voxel_octree() {}

voxel_octree::size_vector3d voxel_octree::get_size() const {
    const std::size_t actual_size = (1 << _size_exp);
    return make_vector3d(actual_size, actual_size, actual_size);
}
const voxel& voxel_octree::get_voxel(std::size_t x,
                                     std::size_t y,
                                     std::size_t z) const
{
    const std::size_t actual_size = (1 << _size_exp);
    assert(x < actual_size);
    assert(y < actual_size);
    assert(z < actual_size);
    if(!_has_children) {
        return _data;
    } else {
        return _children[compute_child_index(x, y, z)]->get_voxel(
                compute_child_relative_coordinates(x, y, z));
    }
}

void voxel_octree::set_voxel(std::size_t x,
                             std::size_t y,
                             std::size_t z,
                             const voxel& v) {
    const std::size_t actual_size = (1 << _size_exp);
    assert(x < actual_size);
    assert(y < actual_size);
    assert(z < actual_size);
    if(_size_exp == 0) {
        //I am leaf
        _data = v;
    } else if(!_has_children) {
        if(_data != v) {
            //I must split things and set correct child to v
            split_children();
            _children[compute_child_index(x, y, z)]->set_voxel(
                    compute_child_relative_coordinates(x, y, z), v);
        }
    } else {
        _children[compute_child_index(x, y, z)]->set_voxel(
                compute_child_relative_coordinates(x, y, z), v);
        //check if children are equal and combine
        //if not, update data to be average node
        combine_children();
    }
}

std::size_t voxel_octree::compute_child_index(std::size_t x, 
                                              std::size_t y, 
                                              std::size_t z) const {
    const std::size_t size_flag = (1 << (_size_exp -1));
    const std::size_t xbit = (x & size_flag) != 0;
    const std::size_t ybit = (y & size_flag) != 0;
    const std::size_t zbit = (z & size_flag) != 0;
    return (zbit << 2) | (ybit << 1) | (xbit);
}
voxel_octree::size_vector3d voxel_octree::compute_child_relative_coordinates(
    std::size_t x, std::size_t y, std::size_t z) const {
    const std::size_t size_flag = (1 << (_size_exp -1));
    return make_vector3d(x & ~size_flag, y & ~size_flag, z & ~size_flag);
}
void voxel_octree::split_children() {
    assert(!_has_children);
    assert(_size_exp > 0);
    for(voxel_octree_ptr& child : _children) {
        child = (make_unique<voxel_octree>(voxel_octree(
                _size_exp - 1, _data)));
    }
    _has_children = true;
}
void voxel_octree::combine_children() {
    assert(_has_children);
    auto equal_data = [this](const voxel_octree_ptr& arg)->bool {
        return !arg->_has_children && arg->_data == _data;
    };
    if(static_cast<std::size_t>(std::count_if(_children.begin(), 
            _children.end(), equal_data)) == _children.size()) {
        _data = _children[0]->_data;
        for(voxel_octree_ptr& child : _children) {
            child.reset(nullptr);
        }
    } else {
        std::size_t num_standard = 0;
        std::size_t num_opaque = 0;
        for(const voxel_octree_ptr& child : _children) {
            if(child->_data.is_standard()) ++num_standard;
            if(child->_data.is_opaque()) ++num_opaque;
        }
        _data = voxel(num_standard >= _children.size() / 2, 
                num_opaque >= _children.size() / 2);
    }
}

}
