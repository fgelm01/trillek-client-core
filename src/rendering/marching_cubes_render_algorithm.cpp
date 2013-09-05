#include "rendering/marching_cubes_render_algorithm.h"
#include <iostream>
#include "data/voxel_data.h"
#include "math/vector.h"
#include <SFML/OpenGL.hpp>
#include <rendering/marching_cubes_lookup_table.h>
#include <services/graphics_service.h>
#include <array>
#include <bitset>
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
    if(v2.is_standard() && v2.is_opaque()) cube_num |= 1 << 4;
    if(v3.is_standard() && v3.is_opaque()) cube_num |= 1 << 5;
    if(v4.is_standard() && v4.is_opaque()) cube_num |= 1 << 3;
    if(v5.is_standard() && v5.is_opaque()) cube_num |= 1 << 2;
    if(v6.is_standard() && v6.is_opaque()) cube_num |= 1 << 7;
    if(v7.is_standard() && v7.is_opaque()) cube_num |= 1 << 6;

    return cube_num;
}


float get_voxel_value(std::size_t x,std::size_t y,
                            std::size_t z,voxel_data* data)
{
    std::size_t _size=data->get_size().x;
    if(x<0||y<0||z<0||
       x>_size-1||y>_size-1||z>_size-1)
    {
        return 0;
    }else
    {
        return data->get_voxel(x,y,z).value();
    }
}

/*
    def defines whether the x,y and z unsigned chars are the values of
    data[n-1] or data[n+1] in the order
    0x1 -> x
    0x2 -> y
    0x4 -> z
    and
    0 -> data[n-1]
    1 -> data[n+1]
*/
vector3d<float> marching_cubes_render_algorithm::calculate_gradient(
                                                        std::size_t pos_x,
                                                        std::size_t pos_y,
                                                        std::size_t pos_z,
                                                         unsigned char x,
                                                         unsigned char y,
                                                         unsigned char z,
                                                         unsigned char def,
                                                         voxel_data* data)
{
    if(!(def&1))
        pos_x++;
    if(!(def&2))
        pos_y++;
    if(!(def&4))
        pos_z++;
    vector3d<float> retval;
    if(def&1) {
        retval.x= get_voxel_value(pos_x-1,pos_y,pos_z,data)-x;
    }else {
        retval.x= x-get_voxel_value(pos_x+1,pos_y,pos_z,data);
    }
    if(def&2) {
        retval.y= get_voxel_value(pos_x,pos_y-1,pos_z,data)-y;
    }else {
        retval.y= y-get_voxel_value(pos_x,pos_y+1,pos_z,data);
    }
    if(def&4) {
        retval.z= get_voxel_value(pos_x,pos_y,pos_z-1,data)-z;
    }else {
        retval.z= z-get_voxel_value(pos_x,pos_y,pos_z+1,data);
    }
    return retval;
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
                                            std::shared_ptr<mesh_data> model,
                                            voxel_data* data)
{
    std::size_t edge_index=edge_table[cubeindex];
    std::array<vector3d<float>,12> edge_vertex_table;
    std::array<vector3d<float>,12> edge_gradient_table;
    unsigned char v0=(cubeindex&0x01)>>0;
    unsigned char v1=(cubeindex&0x02)>>1;
    unsigned char v2=(cubeindex&0x10)>>4;
    unsigned char v3=(cubeindex&0x20)>>5;
    unsigned char v4=(cubeindex&0x08)>>3;
    unsigned char v5=(cubeindex&0x04)>>2;
    unsigned char v6=(cubeindex&0x80)>>7;
    unsigned char v7=(cubeindex&0x40)>>6;
    std::size_t x=p0.x+0.5f;
    std::size_t y=p0.y+0.5f;
    std::size_t z=p0.z+0.5f;

    if(edge_index&0x01) {
        edge_gradient_table[ 0]=interpolate(
            calculate_gradient(x,y,z,v1,v2,v4,0x7,data),
            calculate_gradient(x,y,z,v0,v3,v5,0x6,data));
        edge_vertex_table[ 0]=interpolate(p0,p1);
    }
    if(edge_index&0x02) {
        edge_gradient_table[ 1]=interpolate(
            calculate_gradient(x,y,z,v0,v3,v5,0x6,data),
            calculate_gradient(x,y,z,v4,v7,v1,0x2,data));
        edge_vertex_table[ 1]=interpolate(p1,p5);
    }
    if(edge_index&0x04) {
        edge_gradient_table[ 2]=interpolate(
            calculate_gradient(x,y,z,v4,v7,v1,0x2,data),
            calculate_gradient(x,y,z,v5,v6,v0,0x3,data));
        edge_vertex_table[ 2]=interpolate(p5,p4);
    }
    if(edge_index&0x08) {
        edge_gradient_table[ 3]=interpolate(
            calculate_gradient(x,y,z,v5,v6,v0,0x3,data),
            calculate_gradient(x,y,z,v1,v2,v4,0x7,data));
        edge_vertex_table[ 3]=interpolate(p4,p0);
    }
    if(edge_index&0x10) {
        edge_gradient_table[ 4]=interpolate(
            calculate_gradient(x,y,z,v3,v0,v6,0x5,data),
            calculate_gradient(x,y,z,v2,v1,v7,0x4,data));
        edge_vertex_table[ 4]=interpolate(p2,p3);
    }
    if(edge_index&0x20) {
        edge_gradient_table[ 5]=interpolate(
            calculate_gradient(x,y,z,v2,v1,v7,0x4,data),
            calculate_gradient(x,y,z,v6,v5,v3,0x0,data));
        edge_vertex_table[ 5]=interpolate(p3,p7);
    }
    if(edge_index&0x40) {
        edge_gradient_table[ 6]=interpolate(
            calculate_gradient(x,y,z,v6,v5,v3,0x0,data),
            calculate_gradient(x,y,z,v7,v4,v2,0x1,data));
        edge_vertex_table[ 6]=interpolate(p7,p6);
    }
    if(edge_index&0x80) {
        edge_gradient_table[ 7]=interpolate(
            calculate_gradient(x,y,z,v7,v4,v2,0x1,data),
            calculate_gradient(x,y,z,v3,v0,v6,0x5,data));
        edge_vertex_table[ 7]=interpolate(p6,p2);
    }
    if(edge_index&0x100) {
        edge_gradient_table[ 8]=interpolate(
            calculate_gradient(x,y,z,v3,v0,v6,0x5,data),
            calculate_gradient(x,y,z,v1,v2,v4,0x7,data));
        edge_vertex_table[ 8]=interpolate(p2,p0);
    }
    if(edge_index&0x200) {
        edge_gradient_table[ 9]=interpolate(
            calculate_gradient(x,y,z,v2,v1,v7,0x4,data),
            calculate_gradient(x,y,z,v0,v3,v5,0x6,data));
        edge_vertex_table[ 9]=interpolate(p3,p1);
    }
    if(edge_index&0x400) {
        edge_gradient_table[10]=interpolate(
            calculate_gradient(x,y,z,v6,v5,v3,0x0,data),
            calculate_gradient(x,y,z,v4,v7,v1,0x2,data));
        edge_vertex_table[10]=interpolate(p7,p5);
    }
    if(edge_index&0x800) {
        edge_gradient_table[11]=interpolate(
            calculate_gradient(x,y,z,v7,v4,v2,0x1,data),
            calculate_gradient(x,y,z,v5,v6,v0,0x3,data));
        edge_vertex_table[11]=interpolate(p6,p4);
    }
    for (int n=0; tritable[cubeindex][n] != -1; n+=3)
    {
        model->add_Polygon( edge_vertex_table[tritable[cubeindex][n+2]],
                            edge_gradient_table[tritable[cubeindex][n+2]],
                            edge_vertex_table[tritable[cubeindex][n+1]],
                            edge_gradient_table[tritable[cubeindex][n+1]],
                            edge_vertex_table[tritable[cubeindex][n+0]],
                            edge_gradient_table[tritable[cubeindex][n+0]]);
    }
}

void marching_cubes_render_algorithm::step( axis_aligned_box& box,
                                            unsigned char cubeindex,
                                            std::shared_ptr<mesh_data> model,
                                            voxel_data* data)
{
    step(box.get_corner(0),box.get_corner(1),
         box.get_corner(2),box.get_corner(3),
         box.get_corner(4),box.get_corner(5),
         box.get_corner(6),box.get_corner(7),
         cubeindex, model,data);
}

void marching_cubes_render_algorithm::step( vector3d<float> pos,
                                            unsigned char cubeindex,
                                            float size,
                                            std::shared_ptr<mesh_data> model,
                                            voxel_data* data)
{
        vector3d<float> p0=pos + vector3d<float>(-size/2,-size/2,-size/2);
        vector3d<float> p1=pos + vector3d<float>( size/2,-size/2,-size/2);
        vector3d<float> p2=pos + vector3d<float>(-size/2, size/2,-size/2);
        vector3d<float> p3=pos + vector3d<float>( size/2, size/2,-size/2);
        vector3d<float> p4=pos + vector3d<float>(-size/2,-size/2, size/2);
        vector3d<float> p5=pos + vector3d<float>( size/2,-size/2, size/2);
        vector3d<float> p6=pos + vector3d<float>(-size/2, size/2, size/2);
        vector3d<float> p7=pos + vector3d<float>( size/2, size/2, size/2);
        step(p0,p1,p2,p3,
             p4,p5,p6,p7,
             cubeindex,model,data);
}

void marching_cubes_render_algorithm::process(voxel_model* node,
                                              std::shared_ptr<mesh_data> model)
{

    voxel_data* data=node->get_render_data();
    voxel_data::size_vector3d size=data->get_size();
    model->reserve(size.x*size.y*size.z);
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
                   y<size_i.y-1&&
                   z>=0)
                    n2=data->get_voxel(x+0,y+1,z+0);
                if(x<size_i.x-1&&
                   y<size_i.y-1&&
                   z>=0)
                    n3=data->get_voxel(x+1,y+1,z+0);
                if(x>=0&&
                   y>=0&&
                   z<size_i.z-1)
                    n4=data->get_voxel(x+0,y+0,z+1);
                if(x<size_i.z-1&&
                   y>=0&&
                   z<size_i.z-1)
                    n5=data->get_voxel(x+1,y+0,z+1);
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
                this->step(vector3d<float>(x+0.5f,y+0.5f,z+0.5f),
                           cubeNum,1.0f,model,data);
            }
        }
    }
}

void marching_cubes_render_algorithm::process(voxel_model* node,
                                              graphics_service* service)
{
    std::shared_ptr<mesh_data> model = std::make_shared<mesh_data>();
    process(node,model);
    service->register_model((uintptr_t)node,model);
}

}
