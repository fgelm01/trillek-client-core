#ifndef EVENT_SERVICE_H
#define EVENT_SERVICE_H

#include <map>
#include <vector>
#include "services/service.h"
#include "services/event_service_events.h"

namespace trillek
{

class event_service : public service
{
    public:
        event_service(client* _client);
        virtual ~event_service();

        void send_event(std::shared_ptr<event> e);
        void register_for_event(event::event_type type, service* who);
        void receive_event(std::shared_ptr<event> e){}
        void process_events();
    protected:
    private:
        std::vector<std::shared_ptr<event>> event_buffer;
        std::multimap<event::event_type,service*> registered_services;
};

}

#endif // EVENT_SERVICE_H
