#ifndef VECTOR_H
#define VECTOR_H
#include <vector>

namespace trillek
{

template<typename T>
class _vector
{
    public:
        _vector(){};
        T operator[](unsigned int index)
        {
            if(v.size()>index)
                return v[index];
            else
                return 0;
        }
    protected:
        std::vector<T> v;
    private:
};

template<typename T>
class vector2d
    : public _vector<T>
{
    public:
        vector2d(T v1, T v2);
    protected:
    private:
};

template<typename T>
class vector3d
    : public _vector<T>
{
    public:
        vector3d(T v1, T v2, T v3);
    protected:
    private:
};

}

#endif // VECTOR_H
