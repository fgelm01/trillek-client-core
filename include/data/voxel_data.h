#ifndef VOXEL_DATA_H
#define VOXEL_DATA_H

#include "data/render_data.h"
#include "data/voxel.h"
#include "math/vector.h"

namespace trillek
{

class voxel_data
    : public render_data
{
public:
    typedef vector3d<std::size_t> index_vector;
    voxel_data(){};
    virtual ~voxel_data(){};
    virtual vector3d<std::size_t> get_size() const = 0;
    virtual const voxel& get_voxel(std::size_t x,
                                   std::size_t y,
                                   std::size_t z) const = 0;
    virtual const voxel& get_voxel(const index_vector& xyz) {
        return get_voxel(xyz.x, xyz.y, xyz.z);
    }
    virtual void set_voxel(std::size_t x,
                           std::size_t y,
                           std::size_t z,
                           const voxel& v) = 0;
    virtual void set_voxel(std::size_t x, 
                           std::size_t y, 
                           std::size_t z, 
                           voxel&& v) { set_voxel(x, y, z, v); }
    virtual void set_voxel(const index_vector& xyz, const voxel& v) {
        set_voxel(xyz.x, xyz.y, xyz.z, v);
    }
    virtual void set_voxel(const index_vector& xyz, voxel&& v) {
        set_voxel(xyz.x, xyz.y, xyz.z, std::move(v));
    }
    data_type get_type() const override {return dt_voxel_octree;}
protected:
private:
};

}

#endif // VOXEL_DATA_H
