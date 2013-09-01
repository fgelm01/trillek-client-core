#include "asset_loaders/basic_voxel_asset_loader.h"
#include <fstream>
#include <iostream>
#include "data/voxel_array.h"
namespace trillek {

basic_voxel_asset_loader::basic_voxel_asset_loader()
{
}

basic_voxel_asset_loader::~basic_voxel_asset_loader()
{
}

voxel_data* basic_voxel_asset_loader::load(std::string file)
{
    voxelfile_file_header file_header;
    voxelfile_object_header object_header;
    std::ifstream ifile(file,std::ios::binary);
    if(!ifile)
    {
        std::cerr << "Error: Couldn't find file: "<< file << std::endl;
        return NULL;
    }
    ifile.read((char*)&(file_header),sizeof(voxelfile_file_header ));

    ifile.read((char*)&(object_header),sizeof(voxelfile_object_header));
    voxel_array* voxels = new voxel_array(
                                        object_header.voxel_resolution[0],
                                        object_header.voxel_resolution[2],
                                        object_header.voxel_resolution[1]);
    std::size_t real_size= object_header.voxel_resolution[0]*
                            object_header.voxel_resolution[1]*
                            object_header.voxel_resolution[2];
                            std::cerr << real_size << " reserved" << std::endl;
    for(std::size_t i2=0; i2<real_size;i2++)
    {
        char voxel_header;
        ifile.read((char*)(&voxel_header),sizeof(char ));
        if(voxel_header==ASCIIVOXEL_NOVOXEL)
            continue;
        voxelfile_voxel vox;
        ifile.read((char*)(&vox),sizeof(voxelfile_voxel ));
        voxels->set_voxel(vox.i,vox.k,vox.j,trillek::voxel(true,true));
    }
    return voxels;
}

}
