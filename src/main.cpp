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
#include "asset_loaders/voxel_mesh_asset_loader.h"
#include "data/voxel_array.h"
#include <omp.h>

int main(int argc, char **argv)
{
    omp_set_nested(1);
    omp_set_num_threads(omp_get_num_procs());
    omp_set_dynamic(0);
    trillek::client client;

    // Some test-code to load/create something to test the rendering with
        trillek::graphics_service* g_s = client.get_graphics_service();
        trillek::render_tree* r_t = g_s->get_render_tree();
        trillek::asset_service* a_s = client.get_asset_service();
        //basic_voxel_asset_loader* bval=new basic_voxel_asset_loader();
        std::shared_ptr<trillek::voxel_mesh_asset_loader> mesh_loader =
                std::make_shared<trillek::voxel_mesh_asset_loader>();
        a_s->register_asset_loader("voxels",
                new trillek::basic_voxel_asset_loader());
        a_s->register_asset_loader("obj", mesh_loader);
        a_s->register_asset_loader("OBJ", mesh_loader);
        a_s->register_asset_loader("stl", mesh_loader);
        a_s->register_asset_loader("STL", mesh_loader);
        a_s->register_asset_loader("3DS", mesh_loader);
        mesh_loader.reset();

        unsigned int size=16;
        auto v_m=std::make_shared<trillek::voxel_model>(size,size,size);
        std::cerr << "Starting Construction" << std::endl;
        std::string filename;
        if(argc > 1) {
            filename = argv[1];
        } else {
            filename = "test.voxels";
        }
        std::cerr << "Trying to load file " << filename << std::endl;
        trillek::voxel_data* v_d=(trillek::voxel_data*)a_s->load(filename);
        if(v_d!=NULL)
        {
            v_m->set_render_data(v_d);
        }else
        {
            std::cerr << "Couldn't load " << filename <<
                    ", creating standard room" << std::endl;
            float offset=1;
            for(std::size_t x=0;x<size;x++) {
                for(std::size_t z=0;z<size;z++) {
                    for(std::size_t y=0;y<size;y++) {
                            if(x>offset-1 && x < size-offset &&
                               y>offset-1 && y < size-offset &&
                               z>offset-1 && z < size-offset)
                            {
                                if( x<=offset||x >=size-offset-1||
                                    y<=offset||y >=size-offset-1||
                                    z<=offset||z >=size-offset-1)
                                {
                                    v_m->set_voxel(x,y,z,trillek::voxel(true,true));
                                }
                            }
                    }
                }
            }
        }
        std::cerr << "Finished Construction" << std::endl;
        r_t->get_master()->add_child(v_m);

        auto light = std::make_shared<trillek::light_source>(1,0,0);
        auto trans = std::make_shared<trillek::transformation_node>(0,80,0);

        trans->add_child(light);
        r_t->get_master()->add_child(trans);
    // test-code end

    client.run();
    return 0;
}
