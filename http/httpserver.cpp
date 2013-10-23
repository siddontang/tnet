#include "httpserver.h"

#include <string>

#include "tcpserver.h"
#include "httpconnection.h"
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

    HttpServer::HttpServer(TcpServer* server)
        : m_server(server)
        , m_maxHeaderSize(DefaultMaxHeaderSize)
        , m_maxBodySize(DefaultMaxBodySize)
    {
        HttpParser::initSettings();
    
        m_requestCallback = std::tr1::bind(&dummyRequest, _1, _2);    
    }
   
    HttpServer::~HttpServer()
    {
        
    }
     
    int HttpServer::listen(const Address& addr)
    {
        return m_server->listen(addr, std::tr1::bind(&HttpServer::onNewConnection, this, _1));     
    }

    void HttpServer::onNewConnection(const ConnectionPtr_t& conn)
    {
        HttpParserPtr_t parser(new HttpParser(this, conn));
        
        conn->setCallback(std::tr1::bind(&HttpParser::onConnEvent, parser, _1, _2, _3, _4)); 
    }

}
