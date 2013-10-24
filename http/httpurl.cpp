#include "httpurl.h"

extern "C"
{
#include "http_parser.h"    
}

#include "httputil.h"
#include "log.h"

using namespace std;

namespace tnet
{
    HttpUrl::HttpUrl(const string& url)
    {
        parseUrl(url);    
    }
    
    void HttpUrl::parseUrl(const string& url)
    {
        struct http_parser_url u;
        if(http_parser_parse_url(url.c_str(), url.size(), 0, &u) != 0)
        {
            LOG_ERROR("parseurl error %s", url.c_str());
            return;    
        }

        if(u.field_set & (1 << UF_SCHEMA))
        {
            schema = url.substr(u.field_data[UF_SCHEMA].off, u.field_data[UF_SCHEMA].len);    
        }

        if(u.field_set & (1 << UF_HOST))
        {
            host = url.substr(u.field_data[UF_HOST].off, u.field_data[UF_HOST].len);    
        }

        if(u.field_set & (1 << UF_PORT))
        {
            port = u.port;    
        }
        else
        {
            port = 80;    
        }

        if(u.field_set & (1 << UF_PATH))
        {
            path = url.substr(u.field_data[UF_PATH].off, u.field_data[UF_PATH].len);    
        }

        if(u.field_set & (1 << UF_QUERY))
        {
            string query = url.substr(u.field_data[UF_QUERY].off, u.field_data[UF_QUERY].len);    
        
            parseQuery(query);            
        } 
    }

    void HttpUrl::parseQuery(const string& query)
    {
        char sep1 = '=';
        char sep2 = '&';

        size_t pos1 = 0;
        size_t pos2 = 0;
        size_t lastPos2 = 0;
        string key;
        string value;

        for(;pos2 <= query.size(); ++pos2)
        {
            if(query[pos2] == sep2 || pos2 == query.size())
            {
                for(pos1 = lastPos2; pos1 < pos2 && query[pos1] != sep1; ++pos1)
                {}

                key = query.substr(lastPos2, pos1 - lastPos2);
                if(query[pos1] == sep1)
                {
                    value = query.substr(pos1 + 1, pos2 - pos1 - 1);   
                }
                else
                {
                    value = "";    
                }

                if(!key.empty())
                {
                    key = HttpUtil::unescape(key); 
                    params[key] = HttpUtil::unescape(value);
                }

                lastPos2 = pos2 + 1;
            }    
        }
    
    }
}
