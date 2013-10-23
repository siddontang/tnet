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
    class HttpConnection;
    class HttpRequest;
    class HttpParser;

    class HttpServer : public nocopyable
    {
    public:
        HttpServer(TcpServer* server);
        ~HttpServer();
        
        void setMaxHeaderSize(int headerSize) { m_maxHeaderSize = headerSize; }
        void setMaxBodySize(int bodySize) { m_maxBodySize = bodySize; }

        int getMaxHeaderSize() { return m_maxHeaderSize; }
        int getMaxBodySize() { return m_maxBodySize; }

        int listen(const Address& addr);
    
        typedef std::tr1::shared_ptr<Connection> ConnectionPtr_t;
        typedef std::tr1::shared_ptr<HttpConnection> HttpConnectionPtr_t;
        typedef std::tr1::shared_ptr<HttpRequest> HttpRequestPtr_t;
        typedef std::tr1::shared_ptr<HttpParser> HttpParserPtr_t;

        typedef std::tr1::function<void (const ConnectionPtr_t&, const HttpRequestPtr_t&)> HttpRequestCallback_t;

        void setRequestCallback(const HttpRequestCallback_t& func) { m_func = func; }
        HttpRequestCallback_t& getRequestCallback() { return m_func; }
           
    private:

        void onNewConnection(const ConnectionPtr_t&);

    private:
        TcpServer* m_server;
    
        int m_maxHeaderSize;
        int m_maxBodySize;
    
        HttpRequestCallback_t  m_func;
    };
    
}

#endif
