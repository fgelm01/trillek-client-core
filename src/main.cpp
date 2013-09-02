#include "client.h"
#include <iostream>
#include <memory>
#include "rendering/voxel_model.h"
#include "rendering/light_source.h"
#include "rendering/transformation_node.h"
#include <iostream>
#include <math.h>
#include "services/asset_service.h"
#include "asset_loaders/basic_voxel_asset_loader.h"
#include "data/voxel_array.h"

int main(int argc, char **argv)
{
    trillek::client client;

    // Some test-code to load/create something to test the rendering with
        trillek::graphics_service* g_s = client.get_graphics_service();
        trillek::render_tree* r_t = g_s->get_render_tree();
        trillek::asset_service* a_s = client.get_asset_service();
        //basic_voxel_asset_loader* bval=new basic_voxel_asset_loader();
        a_s->register_asset_loader("voxels",new trillek::basic_voxel_asset_loader());

        unsigned int size=50;
        auto v_m=std::make_shared<trillek::voxel_model>(size,size,size);
        std::cerr << "Starting Construction" << std::endl;
        trillek::voxel_data* v_d=(trillek::voxel_data*)a_s->load("test.voxels");
        if(v_d!=NULL)
        {
            v_m->set_render_data(v_d);
        }else
        {
            std::cerr << "Couldn't load test.voxels, creating standard room" << std::endl;
            for(std::size_t x=0;x<size;x++)
            {
                for(std::size_t z=0;z<size;z++)
                {
                    for(std::size_t y=0;y<size;y++)
                    {
                        v_m->set_voxel(x,0,z,trillek::voxel(true,true));
                        v_m->set_voxel(x,size-1,z,trillek::voxel(true,true));
                        v_m->set_voxel(0,x,z,trillek::voxel(true,true));
                        v_m->set_voxel(size-1,x,z,trillek::voxel(true,true));
                        v_m->set_voxel(x,y,0,trillek::voxel(true,true));
                        v_m->set_voxel(x,y,size-1,trillek::voxel(true,true));
                    }
                }
            }
        }
        std::cerr << "Finished Construction" << std::endl;
        r_t->get_master()->add_child(v_m);

        auto light = std::make_shared<trillek::light_source>(1,0,0);
        auto trans = std::make_shared<trillek::transformation_node>(-10,10,-20);

        trans->add_child(light);
        r_t->get_master()->add_child(trans);
    // test-code end

    client.run();
    return 0;
}
