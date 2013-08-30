#include "math/vector.h"

namespace trillek
{

    template<typename T>
    vector2d<T>::vector2d(T v1,T v2)
    {
        this->v.resize(2);
        this->v[0]=v1;
        this->v[1]=v2;
    }

    template<typename T>
    vector3d<T>::vector3d(T v1,T v2,T v3)
    {
        this->v.resize(3);
        this->v[0]=v1;
        this->v[1]=v2;
        this->v[2]=v3;
    }

    template vector2d<unsigned int>::vector2d(unsigned int v1, unsigned int v2);
    template vector2d<int>::vector2d(int v1, int v2);
    template vector2d<float>::vector2d(float v1, float v2);

    template vector3d<unsigned int>::vector3d(unsigned int v1, unsigned int v2, unsigned int v3);
    template vector3d<int>::vector3d(int v1, int v2, int v3);
    template vector3d<float>::vector3d(float v1, float v2, float v3);

}
