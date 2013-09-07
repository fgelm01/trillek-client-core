#include "client.h"
#include <iostream>
#include "make_unique.h"
#include "services/event_service.h"
#include "services/opengl_graphics_service.h"
#include "services/input_service.h"
#include "services/sfml_window_service.h"
#include <SFML/OpenGL.hpp>
#include "services/cust_settings_service.h"
#include "rendering/dual_marching_cubes_render_algorithm.h"
#include "rendering/marching_cubes_render_algorithm.h"
#include "rendering/first_person_camera.h"

namespace trillek
{

client::client()
{
    // TODO: Abstract this out
    auto algorithm=new marching_cubes_render_algorithm();
    auto camera=new first_person_camera();
    this->events = trillek::make_unique<event_service>(this);
    this->graphics = trillek::make_unique<opengl_graphics_service>(this,
                        algorithm,camera);
    this->input = trillek::make_unique<input_service>(this);
    this->settings = trillek::make_unique<cust_settings_service>(this);
    this->window = trillek::make_unique<sfml_window_service>(this);
    this->assets = trillek::make_unique<asset_service>(this);

    this->events->init();
    this->graphics->init();
    this->input->init();
    this->settings->init();
    this->window->init();
    this->assets->init();

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
    if(!this->assets) return false;
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
    while(this->window->is_open())
    {
        this->window->process();
        this->events->process_events();
        this->window->activate();
        this->graphics->render();
        this->window->finish_frame();
    }
}

}
