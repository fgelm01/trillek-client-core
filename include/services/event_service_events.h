#ifndef EVENT_SERVICE_EVENTS_H
#define EVENT_SERVICE_EVENTS_H

#include "services\service.h"
#include "services\mouse_button_codes.h"
#include "services\key_codes.h"

namespace trillek
{

struct event
{
    enum event_type
    {
        window_resized,
        key,
        mouse_move,
        mouse_button,
        mouse_wheel
    };
    virtual event_type get_type()=0;
};

struct window_resized_event
    : event
{
    float width;
    float height;
    window_resized_event(float width, float height)
    {
        this->width=width;
        this->height=height;
    }
    event_type get_type(){return event::window_resized;}
};

struct key_event
    : event
{
    bool pressed;
    keyboard::key_code code;

    key_event(bool pressed, keyboard::key_code code)
    {
        this->pressed = pressed;
        this->code=code;
    }
    event_type get_type(){return event::key;}
};

struct mouse_move_event
    : event
{
    int x,y;

    mouse_move_event(int x, int y)
    {
        this->x=x;
        this->y=y;
    }
    event_type get_type(){return event::mouse_move;}
};

struct mouse_button_event
    : event
{
    bool pressed;
    mouse::mouse_button_code button;

    mouse_button_event(bool pressed,mouse::mouse_button_code button)
    {
        this->pressed = pressed;
        this->button=button;
    }
    event_type get_type(){return event::mouse_button;}
};

struct mouse_wheel_event
    : event
{
    int amount;

    mouse_wheel_event(int amount)
    {
        this->amount = amount;
    }
    event_type get_type(){return event::mouse_wheel;}
};

}

#endif // EVENT_SERVICE_EVENTS_H
