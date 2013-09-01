#include "rendering/render_tree.h"
#include "make_unique.h"
#include <iostream>
namespace trillek
{
std::size_t render_tree_node::get_num_childs() const
{
    return this->childs.size();
}

render_tree_node* render_tree_node::get_child(std::size_t index)
{
    //the std::vector<T>.at function does bounds checking and throws
    //an exception on failure.
    return this->childs.at(index).get();
}

render_tree_node const* render_tree_node::get_child(std::size_t index) const
{
    return this->childs.at(index).get();
}

void render_tree_node::add_child(std::shared_ptr<render_tree_node> new_child)
{
    this->childs.push_back(new_child);
}

render_tree::render_tree() : master(trillek::make_unique<render_tree_node>())
{
    //ctor uses intialization list
}

render_tree::~render_tree()
{
    //dtor
}

}
