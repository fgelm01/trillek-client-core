#ifndef VOXEL_H
#define VOXEL_H

namespace trillek
{

class voxel
{
    bool standard;
    bool opaque;
    public:
        voxel();
        voxel(bool standard, bool opaque);
        virtual ~voxel();

        bool is_standard(){return standard;}
        bool is_opaque(){return opaque;}
    protected:
    private:
};

}

#endif // VOXEL_H
