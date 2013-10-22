#ifndef _URICODEC_H_
#define _URICODEC_H_

#include <string>

namespace tnet
{
    std::string uriEncode(const std::string& src);
    std::string uriDecode(const std::string& src);    
}

#endif
