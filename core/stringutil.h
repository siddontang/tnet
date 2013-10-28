#ifndef _STRINGUTIL_H
#define _STRINGUTIL_H

#include <string>
#include <vector>
#include <stdint.h>

namespace tnet
{
    class StringUtil
    {
    public:
        static std::vector<std::string> split(const std::string& src, const std::string& delim);
        static uint32_t hash(const std::string& str);
    };    
}

#endif
