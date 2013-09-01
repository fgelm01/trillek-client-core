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
        voxel_data(){};
        virtual ~voxel_data(){};
        virtual vector3d<unsigned int> get_size()=0;
        virtual voxel get_voxel(unsigned int x,
                                 unsigned int y,
                                 unsigned int z)=0;
        virtual void set_voxel(unsigned int x,
                               unsigned int y,
                               unsigned int z,
                               voxel v)=0;
        data_type get_type(){return dt_voxel_octree;}
    protected:
    private:
};

}

#endif // VOXEL_DATA_H
