#ifndef VECTOR_H
#define VECTOR_H
#include <vector>
#include <math.h>

namespace trillek
{
template<typename T>
struct vector2d
{
    T x,y;
    vector2d(T x,T y){this->x=x;this->y=y;};
};

template<typename T>
struct vector3d
{
    T x,y,z;
    vector3d(){this->x=0;this->y=0;this->z=0;};
    vector3d(T x,T y,T z){this->x=x;this->y=y;this->z=z;};
    vector3d<T> operator+(vector3d<T> other)
    {
        return vector3d<T>(this->x+other.x,
                           this->y+other.y,
                           this->z+other.z);
    }
    vector3d<T> operator-(vector3d<T> other)
    {
        return vector3d<T>(this->x-other.x,
                           this->y-other.y,
                           this->z-other.z);
    }
    vector3d<T> operator/(float other)
    {
        return vector3d<T>(this->x/other,
                           this->y/other,
                           this->z/other);
    }
    vector3d<T> cross(vector3d<float> other)
    {
        return vector3d<T>( (this->y*other.z)-(this->z*other.y),
                            (this->z*other.x)-(this->x*other.z),
                            (this->x*other.y)-(this->y*other.x));
    }
    float length()
    {
        return sqrt(x * x + y * y + z * z);
    }
    vector3d<T> normalize()
    {
        float length=this->length();
        return vector3d<T>( this->x/length,
                            this->y/length,
                            this->z/length);
    }
    void operator/=(float other)
    {
        this->x/=other;
        this->y/=other;
        this->z/=other;
    }
};

template<typename T>
vector3d<T> interpolate(vector3d<T> a,vector3d<T> b)
{
    vector3d<T> retval=b-a;
    retval/=2;
    return retval+a;
}

template<typename T>
struct vector_comparer
{
    bool operator()(const vector3d<T> & Left, const vector3d<T> & Right) const
    {
        if(Left.x<Right.x)
            return true;
        if(Left.y<Right.y)
            return true;
        if(Left.z<Right.z)
            return true;
        return false;
    }
};

}

#endif // VECTOR_H
