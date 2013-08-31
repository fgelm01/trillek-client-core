#include "client.h"
#include <iostream>
#include <memory>
#include "rendering/voxel_model.h"
#include "rendering/light_source.h"
#include "rendering/transformation_node.h"
#include <iostream>

int main(int argc, char **argv)
{
    trillek::client client;
    // Some test-code to load/create something to test the rendering with
        trillek::graphics_service* g_s=client.get_graphics_service();
        trillek::render_tree* r_t = g_s->get_render_tree();
        unsigned int size=100;
        auto v_m=std::make_shared<trillek::voxel_model>(size,size,size);
        std::cerr << "Starting Construction" << std::endl;
        for(int x=0;x<size;x++)
        {
            for(int y=0;y<size;y++)
            {
                for(int z=0;z<size;z++)
                {
                    v_m->set_voxel(x,0,z,trillek::voxel(true,true));
                    v_m->set_voxel(x,size-1,z,trillek::voxel(true,true));
                    v_m->set_voxel(0,x,z,trillek::voxel(true,true));
                    v_m->set_voxel(size-1,x,z,trillek::voxel(true,true));
                }
            }
        }
        std::cerr << "Finished Construction" << std::endl;
        r_t->get_master()->add_child(v_m);

        auto light = std::make_shared<trillek::light_source>(1,0,0);
        auto trans = std::make_shared<trillek::transformation_node>(10,10,5);

        trans->add_child(light);
        r_t->get_master()->add_child(trans);
        // test-code end
    client.run();
    return 0;
}
