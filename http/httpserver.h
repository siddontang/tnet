#ifndef _HTTPSERVER_H_
#define _HTTPSERVER_H_

#include <tr1/functional>
#include <tr1/memory>

#include <map>

#include "nocopyable.h"
#include "connevent.h"
#include "wsevent.h"
#include "httpdefs.h"

extern "C"
{
#include "http_parser.h"    
}

namespace tnet
{
    class TcpServer;
    class Address;
    class WsConnection;
    class HttpRequest;
    class HttpResponse;
    class Connection;
    class HttpConnection;

    class HttpServer : public nocopyable
    {
    public:
        friend class HttpConnection;

        HttpServer(TcpServer* server);
        ~HttpServer();
        
        void setMaxHeaderSize(int headerSize) { m_maxHeaderSize = headerSize; }
        void setMaxBodySize(int bodySize) { m_maxBodySize = bodySize; }

        int getMaxHeaderSize() { return m_maxHeaderSize; }
        int getMaxBodySize() { return m_maxBodySize; }

        int listen(const Address& addr);

        //when server receive full headers(include websocket)
        //you may check this request's validity
        void setAuthCallback(const AuthCallback_t& func) { m_authFunc = func; }
    
        void setHttpCallback(const std::string& path, const HttpCallback_t& func);
        void setWsCallback(const std::string& path, const WsCallback_t& func);

    private:
        void onConnEvent(const ConnectionPtr_t&, ConnEvent, const char*, size_t);
    
        int onAuth(const HttpRequest& request);
        void onRequest(const HttpConnectionPtr_t& conn, const HttpRequest& request);
        void onWebsocket(const ConnectionPtr_t& conn, const HttpRequest& request, const char* buffer, size_t count);

        void onHttpConnEvent(const HttpConnectionPtr_t&, const ConnectionPtr_t&, ConnEvent, const char*, size_t);
        void onWsConnEvent(const WsConnectionPtr_t&, const ConnectionPtr_t&, ConnEvent, const char*, size_t);

    private:
        TcpServer* m_server;
    
        int m_maxHeaderSize;
        int m_maxBodySize;
    
        std::map<std::string, HttpCallback_t> m_httpFuncs;        

        std::map<std::string, WsCallback_t> m_wsFuncs;

        AuthCallback_t m_authFunc;
    };
    
}

#endif
