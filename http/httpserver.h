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
        const RequestCallback_t& getRequestCallback() { return m_requestCallback; }

    private:
        void onNewConnection(const ConnectionPtr_t&);

        void onRequest(const HttpRequest& request, const ConnectionPtr_t& conn);

    private:
        TcpServer* m_server;
    
        int m_maxHeaderSize;
        int m_maxBodySize;
    
        RequestCallback_t m_requestCallback;
    };
    
}

#endif
