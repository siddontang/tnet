#include "stringutil.h"
#include <stdint.h>

using namespace std;

namespace tnet
{

    vector<string> StringUtil::split(const string& src, const string& delim)
    {
        size_t lastPos = 0;
        size_t pos = 0;
        size_t size = src.size();

        vector<string> tokens;

        while(pos < size)
        {
            pos = lastPos;
            while(pos < size && delim.find_first_of(src[pos]) == string::npos)
            {
                ++pos;    
            }    

            if(pos - lastPos > 0)
            {
                tokens.push_back(src.substr(lastPos, pos - lastPos));    
            }

            lastPos = pos + 1;
        }

        return tokens;
    }
   
    uint32_t StringUtil::hash(const string& str)
    {
        //use elf hash
        uint32_t h = 0; 
        uint32_t x = 0;
        uint32_t i = 0;
        uint32_t len = (uint32_t)str.size();
        for(i = 0; i < len; ++i)
        {
            h = (h << 4) + str[i];
            if((x = h & 0xF0000000L) != 0)
            {
                h ^= (x >> 24);    
                h &= ~x;
            }    
        } 

        return (h & 0x7FFFFFFF);
    } 
}
