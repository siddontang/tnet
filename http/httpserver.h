#ifndef _HTTPSERVER_H_
#define _HTTPSERVER_H_

#include <tr1/functional>

#include "nocopyable.h"
#include "connection.h"

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
    class HttpParser;

    class HttpServer : public nocopyable
    {
    public:
        friend class HttpParser;

        HttpServer(TcpServer* server);
        ~HttpServer();
        
        void setMaxHeaderSize(int headerSize) { m_maxHeaderSize = headerSize; }
        void setMaxBodySize(int bodySize) { m_maxBodySize = bodySize; }

        int getMaxHeaderSize() { return m_maxHeaderSize; }
        int getMaxBodySize() { return m_maxBodySize; }

        int listen(const Address& addr);
    
        typedef std::tr1::shared_ptr<Connection> ConnectionPtr_t;
        typedef std::tr1::shared_ptr<HttpRequest> HttpRequestPtr_t;
        typedef std::tr1::shared_ptr<HttpParser> HttpParserPtr_t;
        typedef std::tr1::shared_ptr<WsConnection> WsConnectionPtr_t;

        typedef std::tr1::function<void (const HttpRequest&, const ConnectionPtr_t&)> RequestCallback_t;
        void setRequestCallback(const RequestCallback_t& func) { m_requestCallback = func; }

        typedef std::tr1::function<void (const ConnectionPtr_t&)> ConnCallback_t;
        void setConnCloseCallback(const ConnCallback_t& func) { m_closeCallback = func; }
        void setConnErrorCallback(const ConnCallback_t& func) { m_errorCallback = func; }

    private:
        void onConnectionEvent(const ConnectionPtr_t&, Connection::Event, const char*, int);
        void handleRead(const ConnectionPtr_t& conn, const char* buf, int count);
    
        void onRequest(const HttpRequest& request, const ConnectionPtr_t& conn) { m_requestCallback(request, conn); }

    private:
        TcpServer* m_server;
    
        int m_maxHeaderSize;
        int m_maxBodySize;
    
        RequestCallback_t m_requestCallback;

        ConnCallback_t m_closeCallback;
        ConnCallback_t m_errorCallback;
    };
    
}

#endif
