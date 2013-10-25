#ifndef _STRINGUTIL_H
#define _STRINGUTIL_H

#include <string>
#include <vector>

namespace tnet
{
    class StringUtil
    {
    public:
        static std::vector<std::string> split(const std::string& src, const std::string& delim);
    };    
}

#endif
