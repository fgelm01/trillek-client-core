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
        virtual vector3d<std::size_t> get_size()=0;
        virtual voxel get_voxel(std::size_t x,
                                 std::size_t y,
                                 std::size_t z) const = 0;
        virtual void set_voxel(std::size_t x,
                               std::size_t y,
                               std::size_t z,
                               voxel v) = 0;
        virtual data_type get_type(){return dt_voxel;}
        virtual void reserve_space(std::size_t,std::size_t,std::size_t)=0;
    protected:
    private:
};

}

#endif // VOXEL_DATA_H
