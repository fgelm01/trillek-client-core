#include "data/voxel_octree.h"
#include "make_unique.h"
#include <cassert>
#include <algorithm>
#include "math/math_general.h"
#include <iostream>

namespace trillek
{

voxel_octree::voxel_octree() : _size_exp(0), _has_children(false) {}
voxel_octree::voxel_octree(const voxel_octree& other)
        : _size_exp(other._size_exp), _data(other._data),
        _has_children(other._has_children), _offset(other._offset) {
    if(_has_children) {
        for(std::size_t i = 0; i != _children.size(); ++i) {
            _children[i] = make_unique<voxel_octree>(*other._children[i]);
        }
    }
}
voxel_octree::voxel_octree(voxel_octree&& other)
        : _size_exp(other._size_exp), _data(std::move(other._data)),
        _has_children(other._has_children), _offset(other._offset) {
    if(_has_children) {
        for(std::size_t i = 0; i != _children.size(); ++i) {
            _children[i] = std::move(other._children[i]);
        }
    }
}
voxel_octree::voxel_octree(std::size_t size_exp, const voxel& data,
                           offset_vector3d offset)
        : _size_exp(size_exp), _data(data), _has_children(false),
        _offset(offset){}
voxel_octree::~voxel_octree() {}

voxel_octree::size_vector3d voxel_octree::get_size() const {
    const std::size_t actual_size = (1 << _size_exp);
    return make_vector3d(actual_size, actual_size, actual_size);
}
std::size_t voxel_octree::get_num_nodes() const {
    std::size_t ret = 1;
    if(_has_children) {
        for(const voxel_octree_ptr& child : _children) {
            const voxel_octree* child_ptr = child.get();
            ret += child_ptr->get_num_nodes();
        }
    }
    return ret;
}
std::size_t voxel_octree::get_opaque_volume() const {
    std::size_t ret = 0;
    if(_has_children) {
        for(const voxel_octree_ptr& child : _children) {
            const voxel_octree* child_ptr = child.get();
            ret += child_ptr->get_opaque_volume();
        }
    } else {
        if(_data.is_opaque()) {
            ret += get_volume();
        }
    }
    return ret;
}
std::size_t voxel_octree::get_height() const {
    if(_has_children) {
        std::size_t ret = 0;
        for(const voxel_octree_ptr& child : _children) {
            const voxel_octree* child_ptr = child.get();
            ret = std::max(ret, child_ptr->get_height());
        }
        return ret + 1;
    } else {
        return 0;
    }
}
const voxel& voxel_octree::get_voxel() const
{
    assert(!_has_children);
    return _data;
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

void voxel_octree::set_voxel(const voxel& v) {
    if(_has_children) {
        for(voxel_octree_ptr& child : _children) {
            child.reset(nullptr);
        }
    }
    _has_children = false;
    _data = v;
}

void voxel_octree::reserve_space(std::size_t x, std::size_t y, std::size_t z) {
    assert(!_has_children);
    const std::size_t max_size = std::max(std::max(x, y), z);
    for(_size_exp = 0; static_cast<std::size_t>(1 << _size_exp) < max_size;
            ++_size_exp);
    float _size=get_size().x;
    this->_offset=offset_vector3d(_size/2.0f,_size/2.0f,_size/2.0f);
}

void voxel_octree::reserve_space(const size_vector3d& xyz) {
    reserve_space(xyz.x, xyz.y, xyz.z);
}

voxel_octree::offset_vector3d voxel_octree::get_child_offset_by_index(
        std::size_t num) const {
    size_vector3d _size=this->get_size();
    offset_vector3d corner(-(float)_size.x/4.0f,
                           -(float)_size.y/4.0f,
                           -(float)_size.z/4.0f);
    if(num&0x1) corner.x*=-1;
    if(num&0x2) corner.y*=-1;
    if(num&0x4) corner.z*=-1;

    return corner;
}

std::size_t voxel_octree::compute_child_index(std::size_t x,
                                              std::size_t y,
                                              std::size_t z) const {
    return compute_child_index(x,y,z,_size_exp);
}

std::size_t voxel_octree::compute_child_index(std::size_t x,
                                              std::size_t y,
                                              std::size_t z,
                                              std::size_t generation) {
    const std::size_t size_flag = (1 << (generation -1));
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
    for(std::size_t i=0 ; i<_children.size(); ++i) {
        _children[i] = (make_unique<voxel_octree>(voxel_octree(
                _size_exp - 1, _data, 
                _offset + get_child_offset_by_index(i))));
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
        _has_children = false;
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

voxel_octree* voxel_octree::convert(voxel_data* data) {
    // If it already is an octree, just give it back
    if(data->get_type()==dt_voxel_octree)
        return (voxel_octree*)data;

    voxel_octree* retval = new voxel_octree();
    std::cerr << "size of retval is " << retval->get_size().x << std::endl;
    retval->reserve_space(data->get_size());
    std::cerr << "size of retval is " << retval->get_size().x << std::endl;
    const size_vector3d _size = data->get_size();
//    const std::size_t new_size = retval->get_size().x;
//    size_vector3d offsets((_size.x-new_size)/2,
//                           (_size.y-new_size)/2,
//                           (_size.z-new_size)/2);
    vector3d<int> _size_i = _size;
    for(int z=0;z<_size_i.z;++z) {
        for(int y=0;y<_size_i.y;++y) {
            for(int x=0;x<_size_i.x;++x) {
                retval->set_voxel(x,y,z,data->get_voxel(x,y,z));
            }
        }
    }
    return retval;
}

const voxel_octree* voxel_octree::get_child(std::size_t n) const {
    assert(_has_children);
    return _children[n].get();
}

const voxel_octree* voxel_octree::get_child(std::size_t x, 
                                      std::size_t y, 
                                      std::size_t z) const {
    return get_child(compute_child_index(x,y,z,1));
}

voxel_octree* voxel_octree::get_child(std::size_t n) {
    assert(_has_children);
    return _children[n].get();
}

voxel_octree* voxel_octree::get_child(std::size_t x, 
                                      std::size_t y, 
                                      std::size_t z) {
    return get_child(compute_child_index(x,y,z,1));
}

void voxel_octree::set_child(std::size_t n, voxel_octree&& c) {
    if(!_has_children) {
        split_children();
    }
    c._size_exp = _size_exp - 1;
    c._offset = _offset + get_child_offset_by_index(n);
    _children[n] = make_unique<voxel_octree>(std::move(c));
    combine_children();
}

void voxel_octree::set_child(std::size_t x, std::size_t y, std::size_t z, 
        voxel_octree&& c) {
    set_child(compute_child_index(x,y,z,1), std::move(c));
}

void voxel_octree::set_child(std::size_t n, const voxel_octree& c) {
    set_child(n, voxel_octree(c));
}

void voxel_octree::set_child(std::size_t x, std::size_t y, std::size_t z, 
        const voxel_octree& c) {
    set_child(x, y, z, voxel_octree(c));
}

}
