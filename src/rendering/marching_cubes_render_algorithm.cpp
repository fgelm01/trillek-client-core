#include "rendering/marching_cubes_render_algorithm.h"
#include <iostream>
#include "data/voxel_data.h"
#include "math/vector.h"
#include <SFML/OpenGL.hpp>
#include <rendering/marching_cubes_lookup_table.h>
#include <services/graphics_service.h>

namespace trillek
{

marching_cubes_render_algorithm::marching_cubes_render_algorithm()
{
    //ctor
}

marching_cubes_render_algorithm::~marching_cubes_render_algorithm()
{
    //dtor
}

void marching_cubes_render_algorithm::step( vector3d<float> pos,
                                            unsigned char cubeindex,
                                            float size,
                                            graphics_service* /*service*/,
                                            std::shared_ptr<mesh_data> model)
{
        vector3d<float> p0=pos + vector3d<float>(-size/2,-size/2,-size/2);
        vector3d<float> p1=pos + vector3d<float>( size/2,-size/2,-size/2);
        vector3d<float> p2=pos + vector3d<float>(-size/2,-size/2, size/2);
        vector3d<float> p3=pos + vector3d<float>( size/2,-size/2, size/2);
        vector3d<float> p4=pos + vector3d<float>(-size/2, size/2,-size/2);
        vector3d<float> p5=pos + vector3d<float>( size/2, size/2,-size/2);
        vector3d<float> p6=pos + vector3d<float>(-size/2, size/2, size/2);
        vector3d<float> p7=pos + vector3d<float>( size/2, size/2, size/2);
        std::vector<vector3d<float>> EdgeTable;
        EdgeTable.resize(12);
		EdgeTable[ 0]=interpolate(p0,p1);
		EdgeTable[ 1]=interpolate(p1,p3);
		EdgeTable[ 2]=interpolate(p3,p2);
		EdgeTable[ 3]=interpolate(p2,p0);
		EdgeTable[ 4]=interpolate(p5,p4);
		EdgeTable[ 5]=interpolate(p5,p7);
		EdgeTable[ 6]=interpolate(p7,p6);
		EdgeTable[ 7]=interpolate(p6,p4);
		EdgeTable[ 8]=interpolate(p0,p4);
		EdgeTable[ 9]=interpolate(p1,p5);
		EdgeTable[10]=interpolate(p3,p7);
		EdgeTable[11]=interpolate(p2,p6);
		for (int n=0; tritable[cubeindex][n] != -1; n+=3)
		{
		    model->add_Polygon( EdgeTable[tritable[cubeindex][n+2]],
                                EdgeTable[tritable[cubeindex][n+1]],
                                EdgeTable[tritable[cubeindex][n+0]]);
		}
}

void marching_cubes_render_algorithm::process(voxel_model* node,
                                              graphics_service* service)
{
    voxel_data* data=node->get_render_data();
    voxel_data::size_vector3d size=data->get_size();
    std::shared_ptr<mesh_data> model = std::make_shared<mesh_data>();
    model->resize_buffers(size.x*size.y*size.z);
    vector3d<int> size_i = size;
    for(int x=-1;x<static_cast<int>(size.x);++x)
    {
        for(int y=-1;y<static_cast<int>(size.y);++y)
        {
            for(int z=-1;z<static_cast<int>(size.z);++z)
            {
                unsigned char cubeNum=0;
                voxel n0,n1,n2,n3,n4,n5,n6,n7;
                if(x>=0&&
                   y>=0&&
                   z>=0)
                    n0=data->get_voxel(x+0,y+0,z+0);
                if(x<size_i.x-1&&
                   y>=0&&
                   z>=0)
                    n1=data->get_voxel(x+1,y+0,z+0);
                if(x>=0&&
                   y>=0&&
                   z<size_i.z-1)
                    n2=data->get_voxel(x+0,y+0,z+1);
                if(x<size_i.x-1&&
                   y>=0&&
                   z<size_i.z-1)
                    n3=data->get_voxel(x+1,y+0,z+1);
                if(x>=0&&
                   y<size_i.y-1&&
                   z>=0)
                    n4=data->get_voxel(x+0,y+1,z+0);
                if(x<size_i.x-1&&
                   y<size_i.y-1&&
                   z>=0)
                    n5=data->get_voxel(x+1,y+1,z+0);
                if(x>=0&&
                   y<size_i.y-1&&
                   z<size_i.z-1)
                    n6=data->get_voxel(x+0,y+1,z+1);
                if(x<size_i.x-1&&
                   y<size_i.y-1&&
                   z<size_i.z-1)
                    n7=data->get_voxel(x+1,y+1,z+1);

                if(n0.is_standard() && n0.is_opaque()) cubeNum |= 1 << 0;
                if(n1.is_standard() && n1.is_opaque()) cubeNum |= 1 << 1;
                if(n2.is_standard() && n2.is_opaque()) cubeNum |= 1 << 3;
                if(n3.is_standard() && n3.is_opaque()) cubeNum |= 1 << 2;
                if(n4.is_standard() && n4.is_opaque()) cubeNum |= 1 << 4;
                if(n5.is_standard() && n5.is_opaque()) cubeNum |= 1 << 5;
                if(n6.is_standard() && n6.is_opaque()) cubeNum |= 1 << 7;
                if(n7.is_standard() && n7.is_opaque()) cubeNum |= 1 << 6;

                if(cubeNum==0||cubeNum==0xFF)
                    continue;
                this->step(vector3d<float>(x+0.5f-size_i.x/2,
                                           y+0.5f-size_i.y/2,
                                           z+0.5f-size_i.z/2),
                           cubeNum,1.0f,service,model);
            }
        }
    }
    service->register_model((uintptr_t)node,model);
}

}
