#ifndef COUNT_UNIQUE_PARAMS_H
#define COUNT_UNIQUE_PARAMS_H
#include "are_same.h"

namespace trillek
{
    template<class T, std::size_t s>
    void check_uniqueness(std::size_t& num,
                          std::array<T,s>& unique_array,
                          T& p)
    {
        for(unsigned char i=0; i<num;++i) {
            if(unique_array[i]==p) {
                return;
            }else if(i==num-1) {
                unique_array[num]=p;
                num++;
            }
        }
    }
    template <typename H,typename... T,std::size_t s>
    void check_uniqueness(std::size_t& num,
                          std::array<H,s>& unique_array,
                          H& p0, T&... pn) {
        check_uniqueness(num,unique_array,p0);
        check_uniqueness(num,unique_array,pn...);
    }

    template <typename H,typename... T>
    unsigned char count_unique_params(H p0,T&... pn) {
        static_assert(are_same<T...>::value,
            "count_unique only accepts parameters with matching types");

        std::size_t num=1;
        std::array<H,sizeof...(pn)+1> unique_array;
        unique_array[0]=p0;
        check_uniqueness(num,unique_array,pn...);
        return num;
    }

}

#endif // COUNT_UNIQUE_PARAMS_H
