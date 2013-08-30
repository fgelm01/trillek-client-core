#ifndef INPUT_SERVICE_H
#define INPUT_SERVICE_H

#include "services\service.h"

namespace trillek
{

class input_service : public service
{
    public:
        input_service(client* _client);
        virtual ~input_service();
    protected:
    private:
};

}

#endif // INPUT_SERVICE_H
