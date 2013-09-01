#include "data/mesh_data.h"
#include <iostream>
namespace trillek
{

mesh_data::mesh_data()
{
    num_vertices=0;
}

mesh_data::~mesh_data()
{
    //dtor
}

std::vector<vertex_data>* mesh_data::get_vertex_data()
{
    using vertex_iterator=std::map<vec,unsigned int>::iterator;
    if(vertex_data_buffer.size()!=num_vertices)
    {
        vertex_data_buffer.resize(num_vertices);
        for (vertex_iterator it = vertex_buffer.begin();
                             it != vertex_buffer.end(); ++it)
        {
            vertex_data_buffer[it->second].x=std::get<0>(it->first);
            vertex_data_buffer[it->second].y=std::get<1>(it->first);
            vertex_data_buffer[it->second].z=std::get<2>(it->first);
            vertex_data_buffer[it->second].nx=normal_buffer[it->second].x;
            vertex_data_buffer[it->second].ny=normal_buffer[it->second].y;
            vertex_data_buffer[it->second].nz=normal_buffer[it->second].z;
        }
    }
    return &vertex_data_buffer;
}

void mesh_data::resize_buffers(unsigned int new_size)
{
    indices_buffer.reserve(new_size*3);
    normal_buffer.reserve(new_size);
}

void mesh_data::add_Polygon(vector3d<float> _v1,
                             vector3d<float> _v2,
                             vector3d<float> _v3,
                             bool /*auto_normal*/)
{
    vec v1=vec(_v1.x,_v1.y,_v1.z);
    vec v2=vec(_v2.x,_v2.y,_v2.z);
    vec v3=vec(_v3.x,_v3.y,_v3.z);
    vector3d<float> normal=(_v3-_v1).cross(_v2-_v1);
    if(vertex_buffer.find(v1)==vertex_buffer.end())
    {
        vertex_buffer[v1]=num_vertices;
        normal_buffer.push_back(normal);
        num_vertices++;
    }
    if(vertex_buffer.find(v2)==vertex_buffer.end())
    {
        vertex_buffer[v2]=num_vertices;
        normal_buffer.push_back(normal);
        num_vertices++;
    }
    if(vertex_buffer.find(v3)==vertex_buffer.end())
    {
        vertex_buffer[v3]=num_vertices;
        normal_buffer.push_back(normal);
        num_vertices++;
    }

    indices_buffer.push_back(vertex_buffer[v1]);
    indices_buffer.push_back(vertex_buffer[v2]);
    indices_buffer.push_back(vertex_buffer[v3]);

    vector3d<float> old_normal_1=normal_buffer[vertex_buffer[v1]];
    normal_buffer[vertex_buffer[v1]]=((old_normal_1+normal)/2).normalize();

    vector3d<float> old_normal_2=normal_buffer[vertex_buffer[v2]];
    normal_buffer[vertex_buffer[v2]]=((old_normal_2+normal)/2).normalize();

    vector3d<float> old_normal_3=normal_buffer[vertex_buffer[v3]];
    normal_buffer[vertex_buffer[v3]]=((old_normal_3+normal)/2).normalize();
}

}
