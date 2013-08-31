#ifndef CAMERA_H
#define CAMERA_H

#include "services/key_codes.h"
#include "rendering/render_tree.h"
#include "services/mouse_listener.h"

namespace trillek
{

class graphics_service;

class camera : public render_tree_node, public mouse_listener
{
    public:
        camera();
        virtual ~camera();
        virtual void render(graphics_service* service)=0;
    protected:
    private:
};

}

#endif // CAMERA_H
