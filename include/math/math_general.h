#ifndef TRILLEK_MATH_GENERAL_H
#define TRILLEK_MATH_GENERAL_H

namespace trillek
{
namespace math
{
    int find_next_pow2(int num)
    {
        if(num<0)
            return 0;
        num--;
        num |= num << 1;
        num |= num << 2;
        num |= num << 4;
        num |= num << 8;
        num |= num << 16;
        num++;
        return num;
    }
}
}

#endif //TRILLEK_MATH_GENERAL_H
