#ifndef VOXEL_OCTREE_H
#define VOXEL_OCTREE_H

#include "data/voxel_data.h"
#include <memory>
#include <array>

namespace trillek
{

class voxel_octree : public voxel_data
{
public:
    using voxel_data::size_vector3d;
    using voxel_data::get_voxel;
    using voxel_data::set_voxel;
    voxel_octree();
    voxel_octree(const voxel_octree& other);
    voxel_octree(voxel_octree&& other);
    virtual ~voxel_octree();
    const voxel& get_voxel(std::size_t x,
                    std::size_t y,
                    std::size_t z) const override;
    void set_voxel(std::size_t x,
                   std::size_t y,
                   std::size_t z,
                   const voxel& v) override;
    size_vector3d get_size() const override;
    data_type get_type() const override {return dt_voxel_octree;}
    void reserve_space(std::size_t,std::size_t,std::size_t);
protected:
private:
    ///@brief construct an octree with this size having data
    explicit voxel_octree(std::size_t size_exp, const voxel& data);
    typedef std::unique_ptr<voxel_octree> voxel_octree_ptr;
    typedef std::array<voxel_octree_ptr, 8> voxel_child_array;
    std::size_t compute_child_index(std::size_t x, 
                                    std::size_t y, 
                                    std::size_t z) const; 
    size_vector3d compute_child_relative_coordinates(std::size_t x, 
                                                     std::size_t y, 
                                                     std::size_t z) const;
    /**
     * @brief If I have no children, make children exactly like me.
     * If I do, this function is an error
     */
    void split_children();
    /**
     * @brief If all children are same, delete them and store one value here.
     * Otherwise set value to average of children
     */
    void combine_children();
    //size = (1 << size_exp) or equivalently 2^size_exp
    std::size_t _size_exp;
    voxel _data;
    voxel_child_array _children;
    bool _has_children;
};

}

#endif // VOXEL_OCTREE_H
