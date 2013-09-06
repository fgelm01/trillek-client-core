#ifndef TRILLEK_ARE_SAME_H
#define TRILLEK_ARE_SAME_H

namespace trillek
{
    template <class ...T>
    struct are_same;

    template <class A, class B, class ...T>
    struct are_same<A, B, T...>
    {
        static const bool value = std::is_same<A, B>::value && are_same<B, T...>::value;
    };

    template <class A>
    struct are_same<A>
    {
        static const bool value = true;
    };
}
#endif // TRILLEK_ARE_SAME_H
