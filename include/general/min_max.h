#ifndef TRILLEK_MIN_MAX_H
#define TRILLEK_MIN_MAX_H
#include "general/are_same.h"

namespace trillek
{
    template<typename H>
    H min(const H&& arg)
    {
        return arg;
    }

    template<typename H,typename... T>
    H min(const H&& arg,const T&&... args)
    {
        static_assert(are_same<H,T...>::value,
            "min only accepts parameters of the same type");
        return std::min(std::forward<const H>(arg),
                        min(std::forward<const T>(args)...));
    }

    template<typename H>
    H max(const H&& arg)
    {
        return arg;
    }

    template<typename H,typename... T>
    H max(const H&& arg,const T&&... args)
    {
        static_assert(are_same<H,T...>::value,
            "max only accepts parameters of the same type");
        return std::max(std::forward<const H>(arg),
                        max(std::forward<const T>(args)...));
    }
}

#endif
