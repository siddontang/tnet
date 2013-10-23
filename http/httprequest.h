#ifndef _HTTPREQUEST_H_
#define _HTTPREQUEST_H_

#include <string>
#include <map>
#include <stdint.h>

namespace tnet
{
    class HttpRequest
    {
    public:
        HttpRequest();
        ~HttpRequest();

        void clear();

        std::string url;
        std::string body;

        std::map<std::string, std::string> headers;

        unsigned short majorVersion;
        unsigned short minorVersion;

        unsigned char method;
    };
        
}

#endif
