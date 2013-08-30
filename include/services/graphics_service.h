#ifndef GRAPHICS_SERVICE_H
#define GRAPHICS_SERVICE_H
#include "services/service.h"

namespace trillek
{

class graphics_service
    : public service
{
    public:
        graphics_service(client* _client): service(_client){};
        virtual ~graphics_service(){};

        virtual void prepare_rendering()=0;
        virtual void end_rendering()=0;
    protected:
    private:
};

}

#endif // GRAPHICS_SERVICE_H
