#ifndef TRANSFORMATION_NODE_H
#define TRANSFORMATION_NODE_H

#include "rendering/render_tree.h"
#include "math/vector.h"

namespace trillek
{

class transformation_node :
    public render_tree_node
{
    public:
        transformation_node(float x, float y, float z);
        virtual ~transformation_node();
        node_type get_render_type(){return node_type::transformation_node;}

        vector3d<float> get_transformation();
    protected:
    private:
    vector3d<float> offset;
};

}

#endif // TRANSFORMATION_NODE_H
