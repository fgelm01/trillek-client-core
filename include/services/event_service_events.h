#ifndef EVENT_SERVICE_EVENTS_H
#define EVENT_SERVICE_EVENTS_H

#include "services\service.h"

namespace trillek
{

struct event
{
    enum event_type
    {
        window_resized
    };
    virtual event_type get_type()=0;
};

struct window_resized_event
    : event
{
    float width;
    float height;
    window_resized_event(float w, float h)
    {
        width=w;
        height=h;
    }
    event_type get_type(){return event::window_resized;}
};

}

#endif // EVENT_SERVICE_EVENTS_H
