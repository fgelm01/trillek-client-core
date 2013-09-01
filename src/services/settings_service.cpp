#include "services/settings_service.h"
#include <cstdlib>

namespace trillek
{

    int settings_service::setting::to_int(int def)
    {
        if(this->value.size()>0)
            return atoi(this->value.c_str());
        else
            return def;
    }

    float settings_service::setting::to_float(float def)
    {
        if(this->value.size()>0)
            return atof(this->value.c_str());
        else
            return def;
    }

    std::string settings_service::setting::to_string(std::string def)
    {
        if(this->value.size()>0)
            return this->value;
        else
            return def;
    }

    std::shared_ptr<settings_service::setting>
    settings_service::setting::get(std::string name)
    {
        if(this->childs.find(name)!=this->childs.end()) {
            return this->childs[name];
        }
        return std::make_shared<setting>();
    }

    std::shared_ptr<settings_service::setting>
    settings_service::get(std::string name)
    {
        return this->master->get(name);
    }

    settings_service::settings_service(client* _client)
        : service(_client)
    {
        this->master = std::make_shared<setting>();
    }
}
