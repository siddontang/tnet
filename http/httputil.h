#ifndef _HTTPUTIL_H_
#define _HTTPUTIL_H_

namespace tpush
{
    const char* httpStatusCodeReason(int statusCode);    

    const char* httpMethodStr(unsigned char method);
}

#endif
