#ifndef TRILLEK_ELDALAR_VECTOR_H
#define TRILLEK_ELDALAR_VECTOR_H
/*
 * You never know how many vector.h exist out there
 * so make your include guards somewhat unique
 */
#include <cmath>
#include <functional>
#include <type_traits>
#include <iostream>

namespace trillek{

template <typename T>
class vector3d;
template <typename T>
class vector2d;

template <typename T>
vector3d<typename std::decay<T>::type> make_vector3d(T&& x, T&& y, T&& z);
template <typename T>
vector2d<typename std::decay<T>::type> make_vector2d(T&& x, T&& y);

template<typename T>
class vector2d{
public:
    typedef T value_type;
    value_type x,y;
    vector2d() : x(0), y(0) {}
    template <typename U>
    vector2d(U&& x, U&& y) : x(std::forward<U>(x)), y(std::forward<U>(y)) {}
    template <typename U>
    vector2d(const vector2d<U>& other) : x(other.x), y(other.y) {}
    template <typename U>
    decltype(std::declval<value_type>() * std::declval<U>()) cross(
            const vector2d<U>& other) const {
        return x * other.y - y * other.x;
    }
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
                const vector3d<U>& other) const {
        return make_vector3d(y * other.z - z * other.y,
                             z * other.x - x * other.z,
                             x * other.y - y * other.x);
    }
    template <typename U>
    decltype(std::declval<T>() * std::declval<U>()) dot(
            const vector3d<U>& other) const {
        return (x * other.x) + (y * other.y) + (z * other.z);
    }
    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }
    vector3d<T> normalize() const {
        float length=this->length();
        return vector3d<T>( this->x/length,
                            this->y/length,
                            this->z/length);
    }
    vector2d<T> to_vector2d() const {
        return make_vector2d(x, y);
    }
};

template <typename T>
vector2d<typename std::decay<T>::type> make_vector2d(T&& x, T&& y) {
    return vector2d<typename std::decay<T>::type>(std::forward<T>(x),
                                                  std::forward<T>(y));
}

template <typename T>
vector3d<typename std::decay<T>::type> make_vector3d(T&& x, T&& y, T&& z) {
    return vector3d<typename std::decay<T>::type>(std::forward<T>(x),
                                                  std::forward<T>(y),
                                                  std::forward<T>(z));
}

template <typename T>
vector2d<typename std::decay<T>::type> min_vector(
        const vector2d<T>& lhs, const vector2d<T>& rhs) {
    return make_vector2d(std::min(lhs.x, rhs.x),
                         std::min(lhs.y, rhs.y));
}

template <typename T>
vector3d<typename std::decay<T>::type> min_vector(
        const vector3d<T>& lhs, const vector3d<T>& rhs) {
    return make_vector3d(std::min(lhs.x, rhs.x),
                         std::min(lhs.y, rhs.y),
                         std::min(lhs.z, rhs.z));
}

template <typename T>
vector2d<typename std::decay<T>::type> max_vector(
        const vector2d<T>& lhs, const vector2d<T>& rhs) {
    return make_vector2d(std::max(lhs.x, rhs.x),
                         std::max(lhs.y, rhs.y));
}

template <typename T>
vector3d<typename std::decay<T>::type> max_vector(
        const vector3d<T>& lhs, const vector3d<T>& rhs) {
    return make_vector3d(std::max(lhs.x, rhs.x),
                         std::max(lhs.y, rhs.y),
                         std::max(lhs.z, rhs.z));
}

template <typename T>
vector2d<T> operator -(const vector2d<T>& rhs) {
    return make_vector2d(-rhs.x, -rhs.y);
}
template <typename T, typename U>
vector2d<T>& operator +=(vector2d<T>& lhs, const vector2d<U>& rhs) {
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}
template <typename T, typename U>
vector2d<T>& operator -=(vector2d<T>& lhs, const vector2d<U>& rhs) {
    return lhs += (-rhs);
}
template <typename T, typename U>
vector2d<T>& operator *=(vector2d<T>& lhs, const U& rhs) {
    lhs.x *= rhs;
    lhs.y *= rhs;
    return lhs;
}
template <typename T, typename U>
vector2d<decltype(std::declval<T>() + std::declval<U>())> operator +(
        const vector2d<T>& lhs, const vector2d<U>& rhs) {
    return make_vector2d(lhs.x + rhs.x,
                         lhs.y + rhs.y);
}
template <typename T, typename U>
vector2d<decltype(std::declval<T>() + std::declval<U>())> operator -(
        const vector2d<T>& lhs, const vector2d<U>& rhs) {
    return lhs + (-rhs);
}
template <typename T, typename U>
vector2d<decltype(std::declval<T>() * std::declval<U>())> operator *(
        const vector2d<T>& lhs, const U& rhs) {
    return make_vector2d(lhs.x * rhs, lhs.y * rhs);
}
template <typename T, typename U>
vector2d<decltype(std::declval<T>() * std::declval<U>())> operator *(
        const T& lhs, const vector2d<U>& rhs) {
    return make_vector2d(lhs * rhs.x, lhs * rhs.y);
}
template <typename T, typename U>
vector2d<decltype(std::declval<T>() * std::declval<U>())> operator *(
        const vector2d<T>& lhs, const vector2d<U>& rhs) {
    return make_vector2d(lhs.x * rhs.x, lhs.y * rhs.y);
}
template <typename T, typename U>
vector2d<decltype(std::declval<T>() / std::declval<U>())> operator /(
        const vector2d<T>& lhs, const U& rhs) {
    return make_vector2d(lhs.x / rhs, lhs.y / rhs);
}
template <typename T, typename U>
vector2d<decltype(std::declval<T>() / std::declval<U>())> operator /(
        const vector2d<T>& lhs, const vector2d<U>& rhs) {
    return make_vector2d(lhs.x / rhs.x, lhs.y / rhs.y);
}
template <typename T>
vector3d<T> operator -(const vector3d<T>& rhs) {
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

template <typename T, typename U>
bool operator ==(const vector2d<T>& lhs, const vector2d<U>& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}
template <typename T, typename U>
bool operator !=(const vector2d<T>& lhs, const vector2d<U>& rhs) {
    return !(lhs == rhs);
}
/**
 * @brief Give vector2 objects a strict weak ordering
 */
template <typename T, typename U>
bool operator <(const vector2d<T>& lhs, const vector2d<U>& rhs) {
    return lhs.x == rhs.x ? lhs.y < rhs.y : lhs.x < rhs.x;
}

template <typename T, typename U>
bool operator ==(const vector3d<T>& lhs, const vector3d<U>& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}
template <typename T, typename U>
bool operator !=(const vector3d<T>& lhs, const vector3d<U>& rhs) {
    return !(lhs == rhs);
}
/**
 * @brief Give vector3 objects a strict weak ordering
 */
template <typename T, typename U>
bool operator <(const vector3d<T>& lhs, const vector3d<U>& rhs) {
    return lhs.x == rhs.x ?
            (lhs.y == rhs.y ? lhs.z < rhs.z : lhs.y < rhs.y)
            : lhs.x < rhs.x;
}


template<typename T>
vector3d<T> interpolate(vector3d<T> a,vector3d<T> b)
{
    vector3d<T> retval=b-a;
    retval/=2;
    return retval+a;
}

template <typename T>
std::ostream& operator <<(std::ostream& out, const vector2d<T>& rhs) {
    return out << "[" << rhs.x << ", " << rhs.y << "]";
}

template <typename T>
std::ostream& operator <<(std::ostream& out, const vector3d<T>& rhs) {
    return out << "[" << rhs.x << ", " << rhs.y << ", " << rhs.z << "]";
}

}

#endif // TRILLEK_ELDALAR_VECTOR_H
