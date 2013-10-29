#ifndef _URL_H_
#define _URL_H_

#include <string>
#include <map>
#include <stdint.h>

namespace tnet
{

    class HttpUrl
    {
    public:
        HttpUrl(const std::string& url);    

        std::string schema;
        std::string host;
        std::string path;

        std::map<std::string, std::string> params;
        
        uint16_t port;
    
    private:
        void parseUrl(const std::string& url);
        void parseQuery(const std::string& url);
    };
    
}

#endif
