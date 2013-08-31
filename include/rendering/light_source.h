#ifndef LIGHT_SOURCE_H
#define LIGHT_SOURCE_H

#include "rendering/render_tree.h"
#include "math/vector.h"

namespace trillek
{

struct light_settings
{
    vector3d<float> color;
    light_settings(float r,float g,float b)
    :   color(r,g,b)
    {
    }
};

class light_source : public render_tree_node
{
    public:
        light_source(float r, float g, float b);
        virtual ~light_source();
        node_type get_render_type(){return node_type::light_node;}
        light_settings get_settings();
    protected:
    private:
        float red, green, blue;
};

}

#endif // LIGHT_SOURCE_H
