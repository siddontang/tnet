#ifndef _HTTPREQUEST_H_
#define _HTTPREQUEST_H_

#include <string>
#include <map>
#include <stdint.h>

namespace tpush
{
    class HttpRequest
    {
    public:
        HttpRequest();
        ~HttpRequest();

        void clear();
        void parseUrl();

        std::string url;
        std::string body;

        std::string schema;
        std::string host;
        uint16_t port;

        std::string path;

        std::map<std::string, std::string> params;
        std::map<std::string, std::string> headers;

        unsigned short majorVersion;
        unsigned short minorVersion;

        unsigned char method;

    private:
        void parseQuery(const std::string& query);
    };
        
}

#endif
