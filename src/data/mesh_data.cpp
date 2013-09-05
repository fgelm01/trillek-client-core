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
    return &vertex_buffer;
}

void mesh_data::reserve(unsigned int new_size)
{
    vertex_buffer.reserve(new_size*3);
}
#include <SFML/OpenGL.hpp>
void mesh_data::add_Polygon(vector3d<float> v1,
                            vector3d<float> n1,
                            vector3d<float> v2,
                            vector3d<float> n2,
                            vector3d<float> v3,
                            vector3d<float> n3)
{
    vertex_data data;
    data.x =v1.x;data.y =v1.y;data.z =v1.z;
    data.nx=n1.x;data.ny=n1.y;data.nz=n1.z;
    vertex_buffer.push_back(data);

    vertex_data data2;
    data2.x =v2.x;data2.y =v2.y;data2.z =v2.z;
    data2.nx=n2.x;data2.ny=n2.y;data2.nz=n2.z;
    vertex_buffer.push_back(data2);

    vertex_data data3;
    data3.x =v3.x;data3.y =v3.y;data3.z =v3.z;
    data3.nx=n3.x;data3.ny=n3.y;data3.nz=n3.z;
    vertex_buffer.push_back(data3);

    //std::cerr << "Adding Polygon" << std::endl;
    //std::cerr << v1.x << "/" << v1.y << "/" << v1.z << std::endl;
    //std::cerr << n1.x << "/" << n1.y << "/" << n1.z << std::endl;
    glBegin(GL_LINES);
        glVertex3f(v1.x     ,v1.y     ,v1.z);
        glVertex3f(v1.x+n1.x,v1.y+n1.y,v1.z+n1.z);
        glVertex3f(v2.x     ,v2.y     ,v2.z);
        glVertex3f(v2.x+n2.x,v2.y+n2.y,v2.z+n2.z);
        glVertex3f(v3.x     ,v3.y     ,v3.z);
        glVertex3f(v3.x+n3.x,v3.y+n3.y,v3.z+n3.z);
    glEnd();
}

}
