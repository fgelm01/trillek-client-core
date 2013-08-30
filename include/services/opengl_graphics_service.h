#ifndef OPENGL_GRAPHICS_SERVICE_H
#define OPENGL_GRAPHICS_SERVICE_H

#include "services\graphics_service.h"
namespace trillek
{

class opengl_graphics_service
    : public graphics_service
{
    public:
        opengl_graphics_service(client* _client);
        virtual ~opengl_graphics_service();
        void receive_event(std::shared_ptr<event> e);
        void init();
        void window_resized(float width, float height);
    protected:
    private:
        void prepare_rendering();
        void end_rendering(){};
};

}

#endif // OPENGL_GRAPHICS_SERVICE_H
