#include "httprequest.h"

#include <string.h>
#include <stdlib.h>

#include "log.h"

using namespace std;

namespace tnet
{

    HttpRequest::HttpRequest()
    {
        majorVersion = 1;
        minorVersion = 1;
    }
   
    HttpRequest::~HttpRequest()
    {
    } 

    void HttpRequest::clear()
    {
        url.clear();
        body.clear();
        headers.clear();
        
        majorVersion = 1;
        minorVersion = 1;
        method = 0;        
    }
}
