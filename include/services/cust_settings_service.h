#ifndef CUST_SETTINGS_SERVICE_H
#define CUST_SETTINGS_SERVICE_H

#include "services\settings_service.h"

namespace trillek
{

class cust_settings_service : public settings_service
{
    public:
        cust_settings_service(client* _client);
        virtual ~cust_settings_service();
        void load(std::string file);
    protected:
    private:
};

}

#endif // CUST_SETTINGS_SERVICE_H
