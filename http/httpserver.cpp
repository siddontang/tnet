#include "httpserver.h"

#include <string>

#include "tcpserver.h"
#include "log.h"
#include "httpparser.h"
#include "httprequest.h"
#include "httpresponse.h"

using namespace std;
using namespace std::tr1::placeholders;

namespace tnet
{
    const int DefaultMaxHeaderSize = 4 * 1024;
    const int DefaultMaxBodySize = 1024 * 1024;

    static string rootPath = "/";

    void notFoundCallback(const HttpRequest& request, const std::tr1::shared_ptr<Connection>& conn)
    {
        HttpResponse resp;
        resp.statusCode = 404;
        
        conn->send(resp.dump());      
    } 

    HttpServer::HttpServer(TcpServer* server)
        : m_server(server)
        , m_maxHeaderSize(DefaultMaxHeaderSize)
        , m_maxBodySize(DefaultMaxBodySize)
    {
        HttpParser::initSettings();
    
        m_funcs[rootPath] = std::tr1::bind(&notFoundCallback, _1, _2);
    }
   
    HttpServer::~HttpServer()
    {
        
    }
     
    int HttpServer::listen(const Address& addr)
    {
        return m_server->listen(addr, std::tr1::bind(&HttpServer::onConnectionEvent, this, _1, _2, _3, _4));     
    }

    void HttpServer::onConnectionEvent(const ConnectionPtr_t& conn, Connection::Event event, const char* buf, size_t count)
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

    void HttpServer::setHttpCallback(const string& path, const HttpCallback_t& conn)
    {
        m_funcs[path] = conn;    
    }

    void HttpServer::onRequest(const HttpRequest& request, const ConnectionPtr_t& conn)
    {
        map<string, HttpCallback_t>::iterator iter = m_funcs.find(request.path);
        if(iter == m_funcs.end())
        {
            m_funcs[rootPath](request, conn);  
        }
        else
        {
            (iter->second)(request, conn);    
        }
    }
}
