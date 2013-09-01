#ifndef VOXEL_ARRAY_H
#define VOXEL_ARRAY_H

#include "data/voxel_data.h"
#include "data/voxel_octree.h"

namespace trillek
{

class voxel_array : public voxel_data
{
    public:
        voxel_array(std::size_t w, std::size_t h, std::size_t d);
        virtual ~voxel_array();
        vector3d<std::size_t> get_size(){return size;}
        voxel get_voxel(std::size_t x, std::size_t y, std::size_t z) const;
        void set_voxel( std::size_t x, std::size_t y, std::size_t z, voxel v);
        void reserve_space( std::size_t w, std::size_t h, std::size_t d);
        voxel_octree to_octree();
    protected:
        vector3d<std::size_t> size;
        std::vector<std::vector<std::vector<voxel>>> data;
    private:
};

}

#endif // VOXEL_ARRAY_H
