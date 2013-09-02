#ifndef TRILLEK_ELDALAR_VECTOR_H
#define TRILLEK_ELDALAR_VECTOR_H
/*
 * You never know how many vector.h exist out there
 * so make your include guards somewhat unique
 */
#include <vector>
#include <cmath>
#include <functional>
#include <type_traits>

namespace trillek
{

template <typename T>
class vector3d;

template <typename T>
vector3d<typename std::decay<T>::type> make_vector3d(T&& x, T&& y, T&& z);

template<typename T>
struct vector2d
{
    T x,y;
    vector2d(T x,T y){this->x=x;this->y=y;};
};

template<typename T>
struct vector3d
{
    typedef T value_type;
    value_type x,y,z;
    vector3d() : x(0), y(0), z(0) {}
    template <typename U>
    vector3d(U x,U y,U z) : x(x), y(y), z(z) {}
    template <typename U>
    vector3d(const vector3d<U>& other) : x(other.x), y(other.y), z(other.z) {}
    template <typename U>
    vector3d<decltype(std::declval<T>() * std::declval<U>())> cross(
                const vector3d<U>& other) {
        return make_vector3d(y * other.z - z * other.y,
                             z * other.x - x * other.z,
                             x * other.y - y * other.x);
    }
    float length() {
        return std::sqrt(x * x + y * y + z * z);
    }
    vector3d<T> normalize() {
        float length=this->length();
        return vector3d<T>( this->x/length,
                            this->y/length,
                            this->z/length);
    }
};

template <typename T>
vector3d<typename std::decay<T>::type> make_vector3d(T&& x, T&& y, T&& z)
{
    return vector3d<typename std::decay<T>::type>(std::forward<T>(x),
                                                  std::forward<T>(y),
                                                  std::forward<T>(z));
}

template <typename T>
vector3d<T> operator -(const vector3d<T> rhs) {
    return make_vector3d(-rhs.x, -rhs.y, -rhs.z);
}

template <typename T, typename U>
vector3d<T>& operator +=(vector3d<T>& lhs, const vector3d<U>& rhs) {
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    return lhs;
}
template <typename T, typename U>
vector3d<T>& operator -=(vector3d<T>& lhs, const vector3d<U>& rhs) {
    return lhs += (-rhs);
}
template <typename T, typename U>
vector3d<T>& operator *=(vector3d<T>& lhs, const U& rhs) {
    lhs.x *= rhs;
    lhs.y *= rhs;
    lhs.z *= rhs;
    return lhs;
}
template <typename T, typename U>
vector3d<T>& operator /=(vector3d<T>& lhs, const U& rhs) {
    lhs.x /= rhs;
    lhs.y /= rhs;
    lhs.z /= rhs;
    return lhs;
}
template <typename T, typename U>
vector3d<decltype(std::declval<T>() + std::declval<U>())> operator +(
        const vector3d<T>& lhs, const vector3d<U>& rhs) {
    return make_vector3d(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}
template <typename T, typename U>
vector3d<decltype(std::declval<T>() + std::declval<U>())> operator -(
        const vector3d<T>& lhs, const vector3d<U>& rhs) {
    return lhs + (-rhs);
}
template <typename T, typename U>
vector3d<decltype(std::declval<T>() * std::declval<U>())> operator *(
        const vector3d<T>& lhs, const U& rhs) {
    return make_vector3d(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
}
template <typename T, typename U>
vector3d<decltype(std::declval<T>() * std::declval<U>())> operator *(
        const T& lhs, const vector3d<U>& rhs) {
    return make_vector3d(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
}
template <typename T, typename U>
vector3d<decltype(std::declval<T>() / std::declval<U>())> operator /(
        const vector3d<T>& lhs, const vector3d<U>& rhs) {
    return make_vector3d(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
}
template <typename T, typename U>
vector3d<decltype(std::declval<T>() / std::declval<U>())> operator /(
        const vector3d<T>& lhs, const U& rhs) {
    return lhs / make_vector3d(rhs, rhs, rhs);
}


template<typename T>
vector3d<T> interpolate(vector3d<T> a,vector3d<T> b)
{
    vector3d<T> retval=b-a;
    retval/=2;
    return retval+a;
}

}

#endif // TRILLEK_ELDALAR_VECTOR_H
