#include "services\opengl_graphics_service.h"
#include "client.h"
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <math.h>
namespace trillek
{

opengl_graphics_service::opengl_graphics_service(client* _client)
    : graphics_service(_client)
{
    //ctor
}

opengl_graphics_service::~opengl_graphics_service()
{
    //dtor
}

void opengl_graphics_service::prepare_rendering()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
}

}
