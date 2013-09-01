#ifndef DATA_H
#define DATA_H

namespace trillek
{

class data
{
    public:
        enum data_type
        {
            dt_nothing,
            dt_mesh,
            dt_voxel_array,
            dt_voxel_octree
        };

        data(){};
        virtual ~data(){};
        virtual data_type get_type(){return dt_nothing;}
    protected:
    private:
};

}

#endif // DATA_H
