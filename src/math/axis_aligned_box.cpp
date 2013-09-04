#include "math/axis_aligned_box.h"

namespace trillek
{

axis_aligned_box::axis_aligned_box(vector3d<float> center,
                                   vector3d<std::size_t> widths)
    : _widths(widths), _center(center) {}

axis_aligned_box::~axis_aligned_box() {}

void axis_aligned_box::set_widths(float w, float d, float h)
{
    set_widths(vector3d<float>(w,d,h));
}

void axis_aligned_box::set_widths(vector3d<float> widths)
{
    _widths=widths;
}

void axis_aligned_box::translate(float x, float y, float z)
{
    this->translate(vector3d<float>(x,y,z));
}

void axis_aligned_box::translate(vector3d<float> by)
{
    _center+=by;
}

vector3d<float> axis_aligned_box::get_corner(unsigned int num)
{
    vector3d<float> corner(-(float)_widths.x/2.0f,
                           -(float)_widths.y/2.0f,
                           -(float)_widths.z/2.0f);
    if(num&0x1) corner.x*=-1;
    if(num&0x2) corner.y*=-1;
    if(num&0x4) corner.z*=-1;
    return (_center+corner);
}

}
