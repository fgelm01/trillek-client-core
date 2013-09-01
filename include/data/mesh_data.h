#ifndef mesh_data_H
#define mesh_data_H

#include "rendering/render_tree.h"
#include <map>
#include "math/vector.h"

namespace trillek
{

struct vertex_data
{
    float x,y,z;        // Vertex
    float nx,ny,nz;     // Normal
    float s0,t0;        // Texture
};

class mesh_data : public render_data
{
    using vec= std::tuple<float,float,float>;
    public:
        mesh_data();
        virtual ~mesh_data();
        void add_Polygon(vector3d<float> v1,
                         vector3d<float> v2,
                         vector3d<float> v3,
                         bool auto_normal=true);
        std::vector<unsigned int>* get_indices(){return &indices_buffer;}
        std::vector<vertex_data>* get_vertex_data();
        void resize_buffers(unsigned int new_size);
        virtual data_type get_type(){return dt_mesh;}
    protected:
    private:
        unsigned int num_vertices;
        std::map<vec,unsigned int> vertex_buffer;
        std::vector<vertex_data> vertex_data_buffer;
        std::vector<unsigned int> indices_buffer;
        std::vector<vector3d<float>> normal_buffer;

};

}

#endif // mesh_data_H
