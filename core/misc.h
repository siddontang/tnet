#ifndef _MISC_H_
#define _MISC_H_

#include <vector>

namespace tnet
{

    template<typename T>
    void clearContainer(T& c)
    {
        typename T::iterator iter = c.begin();
        while(iter != c.end())
        {
            delete *iter;
            ++iter;    
        }

        c.clear();
    }
   
    const char* errorMsg(int errorNum); 
}

#endif
