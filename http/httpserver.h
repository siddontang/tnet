#ifndef _HTTPSERVER_H_
#define _HTTPSERVER_H_

#include <tr1/functional>

#include <map>

#include "nocopyable.h"
#include "connection.h"
#include "wsevent.h"

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
    class HttpResponse;

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
        typedef std::tr1::shared_ptr<WsConnection> WsConnectionPtr_t;
        typedef std::tr1::shared_ptr<HttpParser> HttpParserPtr_t;

        typedef std::tr1::function<void (const HttpRequest&, const ConnectionPtr_t& conn)> HttpCallback_t;
        void setHttpCallback(const std::string& path, const HttpCallback_t& func);

        typedef std::tr1::function<void (const ConnectionPtr_t&, WsEvent, const std::string)> WsCallback_t;
        void setWsCallback(const std::string& path, const WsCallback_t& func);

    private:
        void onConnectionEvent(const ConnectionPtr_t&, Connection::Event, const char*, size_t);
        void handleRead(const ConnectionPtr_t& conn, const char* buf, size_t count);
    
        void onRequest(const ConnectionPtr_t& conn, const HttpRequest& request);
        void onWebsocket(const ConnectionPtr_t& conn, const HttpRequest& request, const char* buffer, size_t count);

    private:
        TcpServer* m_server;
    
        int m_maxHeaderSize;
        int m_maxBodySize;
    
        std::map<std::string, HttpCallback_t> m_httpFuncs;        

        std::map<std::string, WsCallback_t> m_wsFuncs;
    };
    
}

#endif
