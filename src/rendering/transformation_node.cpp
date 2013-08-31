#include "rendering/transformation_node.h"

namespace trillek
{

transformation_node::transformation_node(float x, float y, float z)
    : offset(x,y,z)
{
}

transformation_node::~transformation_node()
{
    //dtor
}

vector3d<float> transformation_node::get_transformation()
{
    return offset;
}

}
