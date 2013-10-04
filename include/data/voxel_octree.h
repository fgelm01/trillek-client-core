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
    typedef vector3d<float> offset_vector3d;
    voxel_octree();
    voxel_octree(const voxel_octree& other);
    voxel_octree(voxel_octree&& other);
    voxel_octree(voxel_data* other);
    virtual ~voxel_octree();
    /**
     * @brief Get the voxel at the indicated position
     * relative to this node's origin
     * @param x relative x coordinate
     * @param y relative y coordinate
     * @param z relative z coordinate
     * @return the voxel at that position
     * If this is a leaf node, returns its voxel. Otherwise 
     * recurse into the appropriate child and get the voxel with 
     * the modified offsets
     */
    const voxel& get_voxel(std::size_t x,
                    std::size_t y,
                    std::size_t z) const override;
    const voxel& get_voxel() const;
    void set_voxel(std::size_t x,
                   std::size_t y,
                   std::size_t z,
                   const voxel& v) override;
    /**
     * @brief Fill the entire volume of this node with v
     * If had children, they will be removed
     */
    void set_voxel(const voxel& v);
    offset_vector3d get_child_offset_by_index(std::size_t index) const;
    offset_vector3d get_offset() const {return _offset;}
    void set_offset(const offset_vector3d& offset) { _offset = offset; }
    size_vector3d get_size() const override;
    data_type get_type() const override {return dt_voxel_octree;}
    /**
     * @brief Get the number of elements in the whole tree
     */
    std::size_t get_num_nodes() const;
    /**
     * @brief Gets the volume of opaque voxels.
     * As though all data was represented by 1x1x1 voxels
     */
    std::size_t get_opaque_volume() const;
    /**
     * @brief Get the height from here to the deepest child
     * A node with no children has height 0
     */
    std::size_t get_height() const;
    /**
     * @brief Become at least as large as x,y,z
     * Final size is smallest power-of-two cube larger than
     * the maximum of x,y,z.
     * It is not valid to do this operation on an octree with children
     * Doing it on a child of another octree produces undefined behaviour
     * @param x
     * @param y
     * @param z
     */
    void reserve_space(std::size_t x,
                       std::size_t y,
                       std::size_t z);
    void reserve_space(const size_vector3d& xyz);
    static voxel_octree* convert(voxel_data* data);

    bool has_children() const {return _has_children;}
    const voxel_octree* get_child(std::size_t n) const;
    const voxel_octree* get_child(std::size_t x, 
            std::size_t y, std::size_t z) const;
    voxel_octree* get_child(std::size_t n);
    voxel_octree* get_child(std::size_t x, std::size_t y, std::size_t z);
    void set_child(std::size_t n, voxel_octree&& c);
    void set_child(std::size_t x, std::size_t y, std::size_t z, 
            voxel_octree&& c);
    void set_child(std::size_t n, const voxel_octree& c);
    void set_child(std::size_t x, std::size_t y, std::size_t z, 
            const voxel_octree& c);
protected:
private:
    ///@brief construct an octree with this size having data
    explicit voxel_octree(std::size_t size_exp, const voxel& data,
                          offset_vector3d offset);
    typedef std::unique_ptr<voxel_octree> voxel_octree_ptr;
    typedef std::array<voxel_octree_ptr, 8> voxel_child_array;
    std::size_t compute_child_index(std::size_t x,
                                    std::size_t y,
                                    std::size_t z) const;
    static std::size_t compute_child_index(std::size_t x,
                                           std::size_t y,
                                           std::size_t z,
                                           std::size_t generation);
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
    offset_vector3d _offset;
};

}

#endif // VOXEL_OCTREE_H
