#include "services\event_service.h"

namespace trillek
{

event_service::event_service(client* _client)
    :service(_client)
{
    //ctor
}

event_service::~event_service()
{
    //dtor
}

void event_service::send_event(std::shared_ptr<event> e)
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

void event_service::register_for_event(event::event_type type, service* who)
{
    this->registered_services.insert(
                    std::pair<event::event_type,service*>(type,who)
                                    );
}

}
