#include "httpserver.h"

#include <string>

#include "tcpserver.h"
#include "log.h"
#include "httpparser.h"

using namespace std;
using namespace std::tr1::placeholders;

namespace tnet
{
    const int DefaultMaxHeaderSize = 4 * 1024;
    const int DefaultMaxBodySize = 1024 * 1024;

    void dummyRequest(const HttpRequest& request, const std::tr1::shared_ptr<void>& conn)
    {
        
    }

    void dummyCallback(const std::tr1::shared_ptr<Connection>& conn)
    {
            
    }

    HttpServer::HttpServer(TcpServer* server)
        : m_server(server)
        , m_maxHeaderSize(DefaultMaxHeaderSize)
        , m_maxBodySize(DefaultMaxBodySize)
    {
        HttpParser::initSettings();
    
        m_requestCallback = std::tr1::bind(&dummyRequest, _1, _2);    
        m_closeCallback = std::tr1::bind(&dummyCallback, _1);
        m_errorCallback = std::tr1::bind(&dummyCallback, _1);
    }
   
    HttpServer::~HttpServer()
    {
        
    }
     
    int HttpServer::listen(const Address& addr)
    {
        return m_server->listen(addr, std::tr1::bind(&HttpServer::onConnectionEvent, this, _1, _2, _3, _4));     
    }

    void HttpServer::onConnectionEvent(const ConnectionPtr_t& conn, Connection::Event event, const char* buf, int count)
    {
        switch(event)
        {
            case Connection::ReadEvent:
                handleRead(conn, buf, count);
                return;
            default:
                return;
        }
    }

    void HttpServer::handleRead(const ConnectionPtr_t& conn, const char* buf, int count)
    {
        HttpParserPtr_t parser = std::tr1::static_pointer_cast<HttpParser>(conn->getContext());    
        if(!parser)
        {
            parser = HttpParserPtr_t(new HttpParser(this, conn));
            conn->setContext(parser);    
        }

        parser->onConnRead(conn, buf, count);
    }
}
