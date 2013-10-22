#include "httpserver.h"

#include <string>

#include "tcpserver.h"
#include "httpconnection.h"
#include "log.h"

using namespace std;
using namespace std::tr1::placeholders;

namespace tpush
{
    const int DefaultMaxHeaderSize = 4 * 1024;
    const int DefaultMaxBodySize = 1024 * 1024;

    typedef std::tr1::shared_ptr<HttpConnection>  HttpConnectionPtr_t;
    typedef std::tr1::shared_ptr<Connection> ConnectionPtr_t;
    typedef std::tr1::shared_ptr<HttpRequest> HttpRequestPtr_t;
    
    void dummyRequestCallback(const ConnectionPtr_t&, const HttpRequestPtr_t&)
    {
        
    }

    HttpServer::HttpServer(TcpServer* server)
        : m_server(server)
        , m_maxHeaderSize(DefaultMaxHeaderSize)
        , m_maxBodySize(DefaultMaxBodySize)
    {
        m_func = std::tr1::bind(&dummyRequestCallback, _1, _2);
        HttpConnection::initParserSettings(&m_parserSettings);
    }
   
    HttpServer::~HttpServer()
    {
        
    }
     
    int HttpServer::listen(const Address& addr)
    {
        return m_server->listen(addr, std::tr1::bind(&HttpServer::onConnEvent, this, _1, _2, _3, _4));     
    }

    void HttpServer::onConnEvent(const ConnectionPtr_t& conn, Connection::Event event, const char* buffer, int count)
    {
        switch(event)
        {
            case Connection::ReadEvent:
                return onConnRead(conn, buffer, count);
            case Connection::WriteCompleteEvent:
                return onConnWriteComplete(conn);
            case Connection::CloseEvent:
                return onConnClose(conn);
            case Connection::ErrorEvent:
                return onConnError(conn);    
            default:
                break;
        }
    }

    void HttpServer::onConnRead(const ConnectionPtr_t& conn, const char* buffer, int count)
    {
        HttpConnectionPtr_t c = std::tr1::static_pointer_cast<HttpConnection>(conn->getUserData());
        if(!bool(c))
        {
            c = HttpConnectionPtr_t(new HttpConnection(conn, this));
            conn->setUserData(c);
        }

        struct http_parser* parser = c->getParser();

        int n = http_parser_execute(parser, &m_parserSettings, buffer, count);
      
        do
        {
            if(n != count)
            {
                LOG_ERROR("parser error %s:%s", 
                    http_errno_name((http_errno)parser->http_errno), \
                    http_errno_description((http_errno)parser->http_errno));
                break;
            } 
            else if(parser->upgrade)
            {
                LOG_ERROR("not support http upgrade protocol");
                break;
            }

            //ok here
            if(c->eof())
            {
                //read all http request
                conn->resetUserData();
            }

            return;

        }while(0);

        //error here, we will close this connection
        conn->resetUserData();
        
        conn->shutDown();
    }

    void HttpServer::onConnWriteComplete(const ConnectionPtr_t& conn)
    {
        
    }

    void HttpServer::onConnClose(const ConnectionPtr_t& conn)
    {
        
    }

    void HttpServer::onConnError(const ConnectionPtr_t& conn)
    {
        
    }
}
