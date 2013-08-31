#include "data/voxel.h"

namespace trillek
{

voxel::voxel()
{
    standard=true;
    opaque=false;
}

voxel::voxel(bool standard, bool opaque)
{
    this->standard=standard;
    this->opaque=opaque;
}

voxel::~voxel()
{
    //dtor
}

}
