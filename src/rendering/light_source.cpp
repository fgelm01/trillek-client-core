#include "rendering/light_source.h"

namespace trillek
{

light_source::light_source(float r, float g, float b)
{
    red=r;
    green=g;
    blue=b;
}

light_source::~light_source()
{
    //dtor
}

light_settings light_source::get_settings()
{
    return light_settings(red,green,blue);
}

}
