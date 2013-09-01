#ifndef VOXEL_ARRAY_H
#define VOXEL_ARRAY_H

#include "data/voxel_data.h"
#include "data/voxel_octree.h"

namespace trillek
{

class voxel_array : public voxel_data
{
    public:
        voxel_array(unsigned int w, unsigned int h, unsigned int d);
        virtual ~voxel_array();
        vector3d<unsigned int> get_size(){return size;}
        voxel get_voxel(unsigned int x,
                        unsigned int y,
                        unsigned int z);
        void set_voxel( unsigned int x,
                        unsigned int y,
                        unsigned int z,
                        voxel v);
        void reserve_space( unsigned int w,
                            unsigned int h,
                            unsigned int d);
        voxel_octree to_octree();
    protected:
        vector3d<unsigned int> size;
        std::vector<std::vector<std::vector<voxel>>> data;
    private:
};

}

#endif // VOXEL_ARRAY_H
