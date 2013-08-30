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
        void close();
        void activate();
        void finish_frame();
        void process();
        void set_mouse_pos(float x, float y);
        void set_mouse_pos(int x, int y);
        vector2d<unsigned int> get_size();
    protected:
    private:
        sf::Window sfml_window;
};

}

#endif // SFML_WINDOW_SERVICE_H
