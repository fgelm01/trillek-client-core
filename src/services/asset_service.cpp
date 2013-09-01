#include "services/asset_service.h"
#include <iostream>
namespace trillek
{

asset_service::asset_service(client* _client)
    : service(_client)
{
    //ctor
}

asset_service::~asset_service()
{
    //dtor
}

data* asset_service::load(std::string file)
{
    std::string extension=file.substr(file.find_first_of('.')+1);
    if(asset_loaders.find(extension)==asset_loaders.end())
        std::cerr << "Error: Could not load: " << file << std::endl;
    else
        return asset_loaders[extension]->load(file);
    return NULL;
}

void asset_service::register_asset_loader(std::string extension,
                                          asset_loader* new_asset_loader)
{
    this->asset_loaders[extension] = std::unique_ptr<asset_loader>(new_asset_loader);
}

}
