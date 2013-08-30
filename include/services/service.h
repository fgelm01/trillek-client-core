#ifndef SERVICE_H
#define SERVICE_H
#include <memory>
#include "event_service_events.h"

namespace trillek
{

class client;

class service
{
    public:
        service(client* _client){this->_client=_client;};
        virtual ~service(){};
        virtual void receive_event(std::shared_ptr<event> e){}
        virtual void init(){}
    protected:
        client* _client;
    private:
};

}

#endif // SERVICE_H
