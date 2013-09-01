#ifndef ASSET_LOADER_H
#define ASSET_LOADER_H
#include <string>
#include "data/data.h"

namespace trillek
{

class asset_loader
{
    public:
        asset_loader(){};
        virtual ~asset_loader(){};
        virtual data* load(std::string file)=0;
    protected:
    private:
};

}

#endif // ASSET_LOADER_H
