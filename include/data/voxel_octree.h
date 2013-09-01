#ifndef VOXEL_OCTREE_H
#define VOXEL_OCTREE_H

#include "data/voxel_data.h"

namespace trillek
{

class voxel_octree : public voxel_data
{
    public:
        voxel_octree();
        virtual ~voxel_octree();
        vector3d<unsigned int> get_size();
        voxel get_voxel(unsigned int x,
                                 unsigned int y,
                                 unsigned int z);
        void set_voxel(unsigned int x,
                               unsigned int y,
                               unsigned int z,
                               voxel v);
        data_type get_type(){return dt_voxel_octree;}
        void reserve_space(unsigned int,unsigned int,unsigned int);
    protected:
    private:
        unsigned int size;
};

}

#endif // VOXEL_OCTREE_H
