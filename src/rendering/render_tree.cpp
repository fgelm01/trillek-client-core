#include "rendering/render_tree.h"
#include "make_unique.h"
#include <iostream>
namespace trillek
{
unsigned int render_tree_node::get_num_childs()
{
    return this->childs.size();
}

render_tree_node* render_tree_node::get_child(unsigned int index)
{
    if(this->childs.size()>index)
        return this->childs[index].get();
    else
        return NULL;
}

void render_tree_node::add_child(std::shared_ptr<render_tree_node> new_child)
{
    this->childs.push_back(new_child);
}

render_tree::render_tree()
{
    this->master = trillek::make_unique<render_tree_node>();
}

render_tree::~render_tree()
{
    //dtor
}

}
