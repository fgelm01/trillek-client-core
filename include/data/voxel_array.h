#ifndef VOXEL_ARRAY_H
#define VOXEL_ARRAY_H

#include "data/voxel_data.h"
#include "data/voxel_octree.h"

namespace trillek
{

class voxel_array_base : public voxel_data {
public:
    using voxel_data::size_vector3d;
    voxel_array_base();
    virtual ~voxel_array_base();
    size_vector3d get_size() const override;
    virtual void reserve_space(std::size_t x, 
                               std::size_t y, 
                               std::size_t z) = 0;
    virtual void reserve_space(size_vector3d xyz) {
        reserve_space(xyz.x, xyz.y, xyz.z);
    }
    virtual voxel_octree to_octree() const;
    explicit operator voxel_octree () const {return to_octree();}
protected:
    size_vector3d _size;
};

class voxel_array : public voxel_array_base
{
    public:
        voxel_array(std::size_t w, std::size_t h, std::size_t d);
        virtual ~voxel_array();
        vector3d<std::size_t> get_size() const override {return size;}
        const voxel& get_voxel(std::size_t x, 
                               std::size_t y, 
                               std::size_t z) const override;
        void set_voxel(std::size_t x, 
                       std::size_t y, 
                       std::size_t z, const voxel& v) override;
        void reserve_space(std::size_t w, 
                           std::size_t h, 
                           std::size_t d) override;
    protected:
        vector3d<std::size_t> size;
        std::vector<std::vector<std::vector<voxel>>> data;
    private:
};

class voxel_array_alternate : public voxel_array_base {
public:
    using voxel_array_base::size_vector3d;
    voxel_array_alternate();
    virtual ~voxel_array_alternate();
    voxel_array_alternate(size_vector3d size);
    voxel_array_alternate(std::size_t x, std::size_t y, std::size_t z);
    const voxel& get_voxel(std::size_t x, 
                           std::size_t y, 
                           std::size_t z) const override;
    void set_voxel(std::size_t x, 
                   std::size_t y, 
                   std::size_t z, 
                   const voxel& v) override;
    void reserve_space(std::size_t x, std::size_t y, std::size_t z) override;
protected:
    std::size_t compute_index(std::size_t x, 
                              std::size_t y, 
                              std::size_t z) const;
    typedef std::vector<voxel> voxel_vector;
    voxel_vector _data;
};

}

#endif // VOXEL_ARRAY_H
