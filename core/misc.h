#ifndef _MISC_H_
#define _MISC_H_

#include <vector>

namespace tnet
{
    //some function name may like c++ std name
    struct default_delete
    {
        template<typename T>
        void operator()(T* ptr) const
        {
            delete ptr;    
        }    
    };

    template<typename T, typename Func>
    void for_each_all(T& c, const Func& func)
    {
        typename T::iterator iter = c.begin();
        while(iter != c.end())
        {
            func(*iter);    
            ++iter;
        }
    }
   
    template<typename T>
    void for_each_all_delete(T& c)
    {
        for_each_all(c, default_delete());    
    }


    template<typename T>
    void clear_capacity(T& c)
    {
        T tmp;
        c.swap(tmp);    
    }

    const char* errorMsg(int errorNum); 
}

#endif
