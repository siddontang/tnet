#ifndef _HTTPDEFS_H_
#define _HTTPDEFS_H_

#include <tr1/memory>
#include <tr1/functional>
#include <string>

#include "coredefs.h"
#include "wsevent.h"

namespace tnet
{
    class WsConnection;
    class HttpConnection;
    class HttpRequest;
    class HttpResponse;
    class HttpParser;

    typedef std::tr1::shared_ptr<WsConnection> WsConnectionPtr_t;
    typedef std::tr1::weak_ptr<WsConnection> WeakWsConnectionPtr_t;

    typedef std::tr1::shared_ptr<HttpConnection> HttpConnectionPtr_t;
    typedef std::tr1::weak_ptr<HttpConnection> WeakHttpConnectionPtr_t;

    typedef std::tr1::function<int (const HttpRequest&)> AuthCallback_t;
    typedef std::tr1::function<void (const HttpConnectionPtr_t&, const HttpRequest&)> HttpCallback_t;
    typedef std::tr1::function<void (const WsConnectionPtr_t&, WsEvent, const std::string&)> WsCallback_t;
}

#endif
