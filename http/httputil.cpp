#include "httputil.h"
#include <vector>
#include <string>

extern "C"
{
#include "http_parser.h"    
}

using namespace std;

namespace tnet
{
    static vector<string> initReasons()
    {
        vector<string> r;
        r.resize(600);  //now http status code is less than 600
        
        r[100] = "Continue";
        r[101] = "Switching Protocols";
        r[200] = "OK";
        r[201] = "Created";
        r[202] = "Accepted";
        r[203] = "Non-Authoritative Information";
        r[204] = "No Content";
        r[205] = "Reset Content";
        r[206] = "Partial Content";
        r[300] = "Multiple Choices";
        r[301] = "Moved Permanently";
        r[302] = "Found";
        r[303] = "See Other";
        r[304] = "Not Modified";
        r[305] = "Use Proxy";
        r[307] = "Temporary Redirect";
        r[400] = "Bad Request";
        r[401] = "Unauthorized";
        r[402] = "Payment Required";
        r[403] = "Forbidden";
        r[404] = "Not Found";
        r[405] = "Method Not Allowed";
        r[406] = "Not Acceptable";
        r[407] = "Proxy Authentication Required";
        r[408] = "Request Time-out";
        r[409] = "Conflict";
        r[410] = "Gone";
        r[411] = "Length Required";
        r[412] = "Precondition Failed";
        r[413] = "Request Entity Too Large";
        r[414] = "Request-URI Too Large";
        r[415] = "Unsupported Media Type";
        r[416] = "Requested range not satisfiable";
        r[417] = "Expectation Failed";
        r[500] = "Internal Server Error";
        r[501] = "Not Implemented";
        r[502] = "Bad Gateway";
        r[503] = "Service Unavailable";
        r[504] = "Gateway Time-out";
        r[505] = "HTTP Version not supported";            

        return r;
    }

    static vector<string> reasons = initReasons();
    static const char* unknown = "Unknown Error";
    
    const char* httpStatusCodeReason(int statusCode)
    {
        if((size_t)statusCode > reasons.size())
        {
            return unknown;
        }  

        string r = reasons[statusCode];
        return r.empty() ? unknown : r.c_str(); 
    }

    const char* httpMethodStr(unsigned char method)
    {
        return http_method_str((http_method)method);    
    }        
}
