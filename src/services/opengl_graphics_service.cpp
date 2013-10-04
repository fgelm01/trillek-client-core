#include "services/opengl_graphics_service.h"
#include "client.h"
#define GLEW_STATIC
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <math.h>
#include "rendering/light_source.h"
#include "rendering/transformation_node.h"
#include <assert.h>

namespace trillek
{

opengl_graphics_service::opengl_graphics_service(client* _client,
    render_algorithm* rendering_algorithm, camera* cam)
    : graphics_service(_client,rendering_algorithm,cam)
{
}

opengl_graphics_service::~opengl_graphics_service()
{
    //dtor
}

void opengl_graphics_service::prepare_rendering()
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    const float ambi = 0.05f;
    const float spec = 0.95f;
    const float LightData[][4]  = {  { ambi, ambi, ambi, 1.0f },
                                     { spec, spec, spec, 1.0f }};
    const float LightMax[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, LightMax);
    glLightfv(GL_LIGHT1, GL_SPECULAR, LightMax);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glMaterialfv(GL_FRONT, GL_AMBIENT, LightData[0]);
    glMaterialfv(GL_FRONT, GL_SPECULAR, LightData[1]);
    glMaterialf(GL_FRONT, GL_SHININESS, 2.f);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
    glLoadIdentity();
}

void setfrustum(unsigned int fov, double aspect, double n_plane,double f_plane)
{
    double top = tan(fov*3.14159f/360.0f) * n_plane;
    double bottom = - top;
    double left = aspect*bottom;
    double right = aspect*top;
    glFrustum(left,right,bottom,top,n_plane,f_plane);
}

void opengl_graphics_service::window_resized(float width, float height)
{
    glMatrixMode(GL_PROJECTION );
    glLoadIdentity();
    float ratio=width/height;
    glViewport(0, 0, width, height);
    setfrustum(60,ratio,0.5,5000);
}

void opengl_graphics_service::receive_event(std::shared_ptr<event> e)
{
    if(e->get_type()==event::event_type::window_resized)
    {
        std::shared_ptr<window_resized_event> wr_e=
        std::dynamic_pointer_cast<window_resized_event>(e);
        this->window_resized(wr_e->width,wr_e->height);
    }else{
        std::cerr<<"Warning: Unknown Event came into opengl_graphics_service: "
        << e->get_type() << std::endl;
    }
}

void opengl_graphics_service::init()
{
    event_service* e_s=this->_client->get_event_service();
    e_s->register_for_event(event::event_type::window_resized,
                            this);
    input_service* i_s=this->_client->get_input_service();
    i_s->register_mouse_listener(this->cam);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::cerr << "Error: Couldn't initialize GLEW because: "
                  << glewGetErrorString(err) << std::endl;
        assert(false);
    }
}

void opengl_graphics_service::rendering()
{
    this->cam->render(this);
    this->rendering(tree->get_master());
}

void opengl_graphics_service::do_translation(vector3d<float> trans)
{
    glTranslatef(trans.x,trans.y,trans.z);
}

void opengl_graphics_service::rendering(render_tree_node* node)
{
    if(node)
    {
        if(node->get_render_type()==render_tree_node::voxel_node)
        {
            if(((voxel_model*)node)->needs_redraw() ||
               !this->is_model_registered((uintptr_t)node))
            {
                this->rendering_algorithm->process((voxel_model*)node,this);
            }else
            {
                this->recall_model((uintptr_t)node);
            }
        }else if(node->get_render_type()==render_tree_node::light_node)
        {
            auto light=(light_source*)node;
            this->render_light(light->get_settings());
        }else if(node->get_render_type()==render_tree_node::transformation_node)
        {
            auto trans=(transformation_node*)(node);
            this->do_translation(trans->get_transformation());
        }
        for(std::size_t i=0; i<node->get_num_childs();++i)
        {
            this->rendering(node->get_child(i));
        }
    }
}

void opengl_graphics_service::render_light(light_settings /*options*/)
{
    GLfloat pos[] = {0, 0, 0, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
}

void opengl_graphics_service::drawPolygon(  vector3d<float>& v1,
                                            vector3d<float>& v2,
                                            vector3d<float>& v3,
                                            bool automatic_normal)
{
    glBegin(GL_TRIANGLES);
        if(automatic_normal)
            this->generate_normal(v1,v2,v3);
        glVertex3f(v1.x,v1.y,v1.z);
        glVertex3f(v2.x,v2.y,v2.z);
        glVertex3f(v3.x,v3.y,v3.z);
    glEnd();
}

void opengl_graphics_service::generate_normal(   vector3d<float>& v1,
                        vector3d<float>& v2,
                        vector3d<float>& v3)
{
    vector3d<float> normal=calculate_normal(v1,v2,v3);
    set_normal(normal);
}

void opengl_graphics_service::set_normal(   vector3d<float>& n)
{
    glNormal3f(n.x,n.y,n.z);
}

vector3d<float> opengl_graphics_service::calculate_normal(vector3d<float>& v1,
                                                          vector3d<float>& v2,
                                                          vector3d<float>& v3)
{
    vector3d<float> u = v2-v1;
    vector3d<float> v = v3-v1;

    return vector3d<float>(
                    u.y*v.z-u.z*v.y,
                    u.z*v.x-u.x*v.z,
                    u.x*v.y-u.y*v.x
                    );
}

void opengl_graphics_service::do_rotation_x(float rot)
{
    glRotatef(rot,1,0,0);
}

void opengl_graphics_service::do_rotation_y(float rot)
{
    glRotatef(rot,0,1,0);
}

void opengl_graphics_service::do_rotation_z(float rot)
{
    glRotatef(rot,0,0,1);
}

void opengl_graphics_service::register_model(uintptr_t ID,
                                             std::shared_ptr<mesh_data> data)
{
    model_recall_data recall_data;

    // Vertex Data
    std::vector<vertex_data>* vertices=data->get_vertex_data();
    glGenBuffers(1,&recall_data.vertices);
    glBindBuffer(GL_ARRAY_BUFFER, recall_data.vertices);
    glBufferData(GL_ARRAY_BUFFER,
                 vertices->size()*sizeof(vertex_data),
                 &(*vertices)[0], GL_STATIC_DRAW);
    recall_data.size=vertices->size();

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(vertex_data), 0);
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, sizeof(vertex_data), (void*)12);

    this->model_recall_buffer[ID] = recall_data;
    std::cerr << "I register object " << ID << " with " <<
            recall_data.size << " elements" << std::endl;
}

bool opengl_graphics_service::is_model_registered(uintptr_t ID)
{
    return this->model_recall_buffer.find(ID)!=this->model_recall_buffer.end();
}

void opengl_graphics_service::recall_model(uintptr_t ID)
{
    model_recall_data recall_data=this->model_recall_buffer[ID];

    glBindBuffer(GL_ARRAY_BUFFER, recall_data.vertices);
    glDrawArrays(GL_TRIANGLES,0,  recall_data.size);
}

}
