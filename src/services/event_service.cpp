#include "services\event_service.h"

namespace trillek
{

event_service::event_service(client* _client)
    :service(_client)
{
    event_buffer.reserve(1024);
}

event_service::~event_service()
{
    //dtor
}

void event_service::send_event(std::shared_ptr<event> e)
{
    event_buffer.push_back(e);
}

void event_service::register_for_event(event::event_type type, service* who)
{
    this->registered_services.insert(
                    std::pair<event::event_type,service*>(type,who)
                                    );
}

void event_service::process_events()
{
    for (std::shared_ptr<event> e : event_buffer )
    {
        std::pair<
            std::multimap<event::event_type,service*>::iterator,
            std::multimap<event::event_type,service*>::iterator> ret;
        ret = registered_services.equal_range(e->get_type());

        for (   std::multimap<event::event_type,service*>::iterator it=ret.first;
                it!=ret.second; ++it)
        {
            it->second->receive_event(e);
        }
    }
    event_buffer.clear();
}

}
