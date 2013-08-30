#ifndef CLIENT_H
#define CLIENT_H
#include <memory>
#include "services\event_service.h"
#include "services\graphics_service.h"
#include "services\input_service.h"
#include "services\window_service.h"
#include "services\settings_service.h"
#include <iostream>
namespace trillek
{

class client
{
    public:
        client();
        virtual ~client();
        void run();
        event_service* get_event_service(){return events.get();}
        graphics_service* get_graphics_service(){return graphics.get();}
        input_service* get_input_service(){return input.get();}
        settings_service* get_settings_service(){return settings.get();}
        window_service* get_window_service(){return window.get();}
    protected:
    private:
        std::unique_ptr<client> self;
        std::unique_ptr<event_service> events;
        std::unique_ptr<graphics_service> graphics;
        std::unique_ptr<input_service> input;
        std::unique_ptr<settings_service> settings;
        std::unique_ptr<window_service> window;

        bool all_loaded();
};

}
#endif // CLIENT_H
