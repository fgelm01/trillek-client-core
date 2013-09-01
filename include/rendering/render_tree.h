#ifndef RENDER_TREE_H
#define RENDER_TREE_H

#include "data/render_data.h"
#include <vector>
#include <memory>

namespace trillek
{

class render_tree_node
{
    public:
        enum node_type
        {
            grouping_node,
            voxel_node,
            mesh_node,
            transformation_node,
            light_node
        };
        render_tree_node(){};
        virtual ~render_tree_node(){};
        virtual node_type get_render_type(){return grouping_node;}
        virtual render_data* get_render_data(){return nullptr;}
        std::size_t get_num_childs() const;
        render_tree_node* get_child(std::size_t index);
        render_tree_node const* get_child(std::size_t index) const;
        void add_child(std::shared_ptr<render_tree_node> new_child);

    protected:
    private:
        std::vector<std::shared_ptr<render_tree_node>> childs;
};

class render_tree
{
    public:
        render_tree();
        virtual ~render_tree();
        render_tree_node* get_master(){return master.get();}
    protected:
    private:
        std::unique_ptr<render_tree_node> master;
};

}

#endif // RENDER_TREE_H
