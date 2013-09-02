#ifndef TRILLEK_VOXEL_DATA_H
#define TRILLEK_VOXEL_DATA_H

#include "data/render_data.h"
#include "data/voxel.h"
#include "math/vector.h"

namespace trillek
{

/**
 * @brief The base class for all voxel-like things
 */
class voxel_data
    : public render_data
{
public:
    typedef vector3d<std::size_t> size_vector3d;
    voxel_data(){}
    virtual ~voxel_data(){}
    virtual size_vector3d get_size() const = 0;
    virtual std::size_t get_volume() const {
        size_vector3d my_size = get_size();
        return my_size.x * my_size.y * my_size.z;
    }
    virtual const voxel& get_voxel(std::size_t x,
                                   std::size_t y,
                                   std::size_t z) const = 0;
    virtual const voxel& get_voxel(const size_vector3d& xyz) const {
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
    virtual void set_voxel(const size_vector3d& xyz, const voxel& v) {
        set_voxel(xyz.x, xyz.y, xyz.z, v);
    }
    virtual void set_voxel(const size_vector3d& xyz, voxel&& v) {
        set_voxel(xyz.x, xyz.y, xyz.z, std::move(v));
    }
    data_type get_type() const override {return dt_voxel_octree;}
protected:
private:
};

}

#endif // TRILLEK_VOXEL_DATA_H
