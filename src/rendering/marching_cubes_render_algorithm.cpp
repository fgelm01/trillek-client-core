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

unsigned char marching_cubes_render_algorithm::calculate_cube_num(voxel& v0,
                                                                  voxel& v1,
                                                                  voxel& v2,
                                                                  voxel& v3,
                                                                  voxel& v4,
                                                                  voxel& v5,
                                                                  voxel& v6,
                                                                  voxel& v7)
{
    unsigned char cube_num=0;
    if(v0.is_standard() && v0.is_opaque()) cube_num |= 1 << 0;
    if(v1.is_standard() && v1.is_opaque()) cube_num |= 1 << 1;
    if(v2.is_standard() && v2.is_opaque()) cube_num |= 1 << 3;
    if(v3.is_standard() && v3.is_opaque()) cube_num |= 1 << 2;
    if(v4.is_standard() && v4.is_opaque()) cube_num |= 1 << 4;
    if(v5.is_standard() && v5.is_opaque()) cube_num |= 1 << 5;
    if(v6.is_standard() && v6.is_opaque()) cube_num |= 1 << 7;
    if(v7.is_standard() && v7.is_opaque()) cube_num |= 1 << 6;

    return cube_num;
}

void marching_cubes_render_algorithm::step( vector3d<float> p0,
                                            vector3d<float> p1,
                                            vector3d<float> p2,
                                            vector3d<float> p3,
                                            vector3d<float> p4,
                                            vector3d<float> p5,
                                            vector3d<float> p6,
                                            vector3d<float> p7,
                                            unsigned char cubeindex,
                                            std::shared_ptr<mesh_data> model)
{
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

void marching_cubes_render_algorithm::step( axis_aligned_box& box,
                                            unsigned char cubeindex,
                                            std::shared_ptr<mesh_data> model)
{
    step(box.get_corner(0),box.get_corner(1),
         box.get_corner(2),box.get_corner(3),
         box.get_corner(4),box.get_corner(5),
         box.get_corner(6),box.get_corner(7),
         cubeindex, model);
}

void marching_cubes_render_algorithm::step( vector3d<float> pos,
                                            unsigned char cubeindex,
                                            float size,
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
        step(p0,p1,p2,p3,
             p4,p5,p6,p7,
             cubeindex,model);
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
                cubeNum = calculate_cube_num(n0,n1,n2,n3,n4,n5,n6,n7);

                if(cubeNum==0||cubeNum==0xFF)
                    continue;
                this->step(vector3d<float>(x+0.5f-size_i.x/2,
                                           y+0.5f-size_i.y/2,
                                           z+0.5f-size_i.z/2),
                           cubeNum,1.0f,model);
            }
        }
    }
    service->register_model((uintptr_t)node,model);
}

}
