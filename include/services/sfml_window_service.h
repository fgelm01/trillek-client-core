#ifndef SFML_WINDOW_SERVICE_H
#define SFML_WINDOW_SERVICE_H

#include <SFML\Window.hpp>
#include "services\window_service.h"

namespace trillek
{

class sfml_window_service : public window_service
{
    public:
        sfml_window_service(client* _client);
        virtual ~sfml_window_service();
        bool is_open();
        void open();
        void activate();
        void finish_frame();
        void process();
    protected:
    private:
        sf::Window sfml_window;
};

}

#endif // SFML_WINDOW_SERVICE_H
