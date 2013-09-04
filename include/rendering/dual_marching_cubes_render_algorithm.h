#ifndef DUAL_MARCHING_CUBES_RENDER_ALGORITHM_H
#define DUAL_MARCHING_CUBES_RENDER_ALGORITHM_H
#include "rendering/render_algorithm.h"
#include "data/voxel_octree.h"
#include "data/mesh_data.h"

namespace trillek
{

class dual_marching_cubes_render_algorithm
    : public render_algorithm
{
public:
    dual_marching_cubes_render_algorithm();
    ~dual_marching_cubes_render_algorithm();
private:
    void create_dual_cells(voxel_octree* n0,voxel_octree* n1,
                           voxel_octree* n2,voxel_octree* n3,
                           voxel_octree* n4,voxel_octree* n5,
                           voxel_octree* n6,voxel_octree* n7,
                           std::shared_ptr<mesh_data> model);
    void vertProc(voxel_octree* n0,voxel_octree* n1,
                  voxel_octree* n2,voxel_octree* n3,
                  voxel_octree* n4,voxel_octree* n5,
                  voxel_octree* n6,voxel_octree* n7,
                  std::shared_ptr<mesh_data> model);
    void edgeProcX(voxel_octree* n0,voxel_octree* n1,
                   voxel_octree* n2,voxel_octree* n3,
                   std::shared_ptr<mesh_data> model);
    void edgeProcY(voxel_octree* n0,voxel_octree* n1,
                   voxel_octree* n2,voxel_octree* n3,
                   std::shared_ptr<mesh_data> model);
    void edgeProcZ(voxel_octree* n0,voxel_octree* n1,
                   voxel_octree* n2,voxel_octree* n3,
                   std::shared_ptr<mesh_data> model);
    void faceProcXY(voxel_octree* n0,voxel_octree* n1,
                    std::shared_ptr<mesh_data> model);
    void faceProcYZ(voxel_octree* n0,voxel_octree* n1,
                    std::shared_ptr<mesh_data> model);
    void faceProcXZ(voxel_octree* n0,voxel_octree* n1,
                    std::shared_ptr<mesh_data> model);
    void nodeProc(voxel_octree* n,std::shared_ptr<mesh_data> model);
    void process(voxel_model* node,graphics_service* service);
};

}
#endif // DUAL_MARCHING_CUBES_RENDER_ALGORITHM_H
