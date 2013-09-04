#ifndef AXIS_ALIGNED_BOX_H
#define AXIS_ALIGNED_BOX_H
#include "math/vector.h"

namespace trillek
{

class axis_aligned_box
{
    public:
        axis_aligned_box(vector3d<float> center,
                         vector3d<std::size_t> widths);
        virtual ~axis_aligned_box();
        void set_widths(float w, float d, float h);
        void set_widths(vector3d<float> widths);
        void translate(float x, float y, float z);
        void translate(vector3d<float> by);
        vector3d<float> get_corner(unsigned int num);
    protected:
    private:
        vector3d<std::size_t> _widths;
        vector3d<float> _center;
};

}

#endif // AXIS_ALIGNED_BOX_H
