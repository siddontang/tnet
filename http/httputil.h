#ifndef _HTTPUTIL_H_
#define _HTTPUTIL_H_

namespace tnet
{
    const char* httpStatusCodeReason(int statusCode);    

    const char* httpMethodStr(unsigned char method);
}

#endif
