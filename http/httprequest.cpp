#include "httprequest.h"

#include <string.h>
#include <stdlib.h>

#include "log.h"

using namespace std;

namespace tnet
{

    HttpRequest::HttpRequest()
    {
    }
   
    HttpRequest::~HttpRequest()
    {
    } 

    void HttpRequest::clear()
    {
        url.clear();
        body.clear();
        headers.clear();
        
        majorVersion = 0;
        minorVersion = 0;
        method = 0;        
    }
}
