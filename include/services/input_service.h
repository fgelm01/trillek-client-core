#ifndef INPUT_SERVICE_H
#define INPUT_SERVICE_H

#include "services\service.h"
#include "services\key_codes.h"
#include "services\mouse_listener.h"
#include <vector>

namespace trillek
{

class input_service : public service
{
    std::vector<bool> keys_pressed;
    public:
        input_service(client* _client);
        virtual ~input_service();
        void init();
        void receive_event(std::shared_ptr<event> e);
        bool is_key_pressed(keyboard::key_code code){return keys_pressed[code];}
        void register_mouse_listener(std::shared_ptr<mouse_listener> listener);
    protected:
    private:
        std::vector<std::shared_ptr<mouse_listener>> mouse_listeners;
        bool trap_mouse;
        void mouse_move(std::shared_ptr<mouse_move_event> e);
};

}

#endif // INPUT_SERVICE_H
