#include "data/voxel.h"

namespace trillek {

voxel::voxel() : standard(true), opaque(false) {}

voxel::voxel(bool standard, bool opaque)
        : standard(standard), opaque(opaque) {}
bool voxel::is_standard() const {return standard;}
bool voxel::is_opaque() const {return opaque;}
voxel::~voxel() {}

uint16_t voxel::value() const
{
    if(is_standard()&&is_opaque())
        return 1;
    else
        return 0;
}

bool operator ==(const voxel& lhs, const voxel& rhs) {
    return lhs.is_opaque() == rhs.is_opaque() &&
            lhs.is_standard() == rhs.is_standard();
}
bool operator !=(const voxel& lhs, const voxel& rhs) {
    return !(lhs == rhs);
}

}
