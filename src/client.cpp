#include "client.h"
#include <iostream>
#include "make_unique.h"
#include "services\event_service.h"
#include "services\opengl_graphics_service.h"
#include "services\input_service.h"
#include "services\sfml_window_service.h"
#include <SFML/OpenGL.hpp>
#include "services\cust_settings_service.h"

namespace trillek
{

client::client()
{
    // TODO: Abstract this out
    this->events = std::make_unique<event_service>(this);
    this->graphics = std::make_unique<opengl_graphics_service>(this);
    this->input = std::make_unique<input_service>(this);
    this->settings = std::make_unique<cust_settings_service>(this);
    this->window = std::make_unique<sfml_window_service>(this);

    this->events->init();
    this->graphics->init();
    this->input->init();
    this->settings->init();
    this->window->init();

    settings->load("test.cfg");
}

client::~client()
{
    //dtor
}

bool client::all_loaded()
{
    if(!this->events) return false;
    if(!this->graphics) return false;
    if(!this->input) return false;
    if(!this->settings) return false;
    if(!this->window) return false;
    return true;
}

void client::run()
{
    if(!all_loaded()) {
        std::cerr << "Error: Not all services were loaded before running the client" << std::endl;
        return;
    }

    this->window->open();

    // Main cycle
    float rotation=0;
    while(this->window->is_open())
    {
        rotation+=0.01;
        this->window->process();
        this->events->process_events();
        this->window->activate();
        this->graphics->prepare_rendering();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.f, 0.f, -200.f);
        glRotatef(rotation,0,1,0);
        glBegin(GL_QUADS);

            glVertex3f(-50.f, -50.f, -50.f);
            glVertex3f(-50.f,  50.f, -50.f);
            glVertex3f( 50.f,  50.f, -50.f);
            glVertex3f( 50.f, -50.f, -50.f);

            glVertex3f(-50.f, -50.f, 50.f);
            glVertex3f(-50.f,  50.f, 50.f);
            glVertex3f( 50.f,  50.f, 50.f);
            glVertex3f( 50.f, -50.f, 50.f);

            glVertex3f(-50.f, -50.f, -50.f);
            glVertex3f(-50.f,  50.f, -50.f);
            glVertex3f(-50.f,  50.f,  50.f);
            glVertex3f(-50.f, -50.f,  50.f);

            glVertex3f(50.f, -50.f, -50.f);
            glVertex3f(50.f,  50.f, -50.f);
            glVertex3f(50.f,  50.f,  50.f);
            glVertex3f(50.f, -50.f,  50.f);

            glVertex3f(-50.f, -50.f,  50.f);
            glVertex3f(-50.f, -50.f, -50.f);
            glVertex3f( 50.f, -50.f, -50.f);
            glVertex3f( 50.f, -50.f,  50.f);

            glVertex3f(-50.f, 50.f,  50.f);
            glVertex3f(-50.f, 50.f, -50.f);
            glVertex3f( 50.f, 50.f, -50.f);
            glVertex3f( 50.f, 50.f,  50.f);

        glEnd();
        //this->graphics->end_rendering();
        this->window->finish_frame();
    }
}

}
