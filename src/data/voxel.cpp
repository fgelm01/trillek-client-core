#include "data/voxel.h"

namespace trillek {

voxel::voxel() : standard(true), opaque(false) {}

voxel::voxel(bool standard, bool opaque) 
        : standard(standard), opaque(opaque) {}

voxel::~voxel() {}

}
