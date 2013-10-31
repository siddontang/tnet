#include "httpserver.h"

#include <string>

#include "tcpserver.h"
#include "log.h"
#include "httpparser.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "wsconnection.h"

using namespace std;
using namespace std::tr1::placeholders;

namespace tnet
{
    const int DefaultMaxHeaderSize = 4 * 1024;
    const int DefaultMaxBodySize = 1024 * 1024;

    static string rootPath = "/";

    typedef std::tr1::shared_ptr<Connection> ConnectionPtr_t;

    void httpNotFoundCallback(const HttpRequest& request, const std::tr1::shared_ptr<Connection>& conn)
    {
        HttpResponse resp;
        resp.statusCode = 404;
        
        conn->send(resp.dump());      
    } 

    void wsNotFoundCallback(const ConnectionPtr_t& conn, WsEvent event, const string& message)
    {
        conn->shutDown();    
    }

    HttpServer::HttpServer(TcpServer* server)
        : m_server(server)
        , m_maxHeaderSize(DefaultMaxHeaderSize)
        , m_maxBodySize(DefaultMaxBodySize)
    {
        HttpParser::initSettings();
    
        m_httpFuncs[rootPath] = std::tr1::bind(&httpNotFoundCallback, _1, _2);
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

    class HttpContext
    {
    public:
        enum Type
        {
            HttpType,
            WsType,    
        };

        HttpContext(Type t, void* c)
        {
            type = t;
            context = c;
        }

        ~HttpContext()
        {
            if(type == HttpType)
            {
                delete static_cast<HttpParser*>(context);    
            }
            else
            {
                delete static_cast<WsConnection*>(context);    
            }
        }

        Type type;
        void* context;
    };

    typedef std::tr1::shared_ptr<HttpContext> HttpContextPtr_t;

    void HttpServer::handleRead(const ConnectionPtr_t& conn, const char* buf, size_t count)
    {
        HttpContextPtr_t c = std::tr1::static_pointer_cast<HttpContext>(conn->getContext());    
        if(!c)
        {
            HttpParser* parser = new HttpParser(this, conn);
            c = HttpContextPtr_t(new HttpContext(HttpContext::HttpType, parser));
            conn->setContext(c);
        }

        if(c->type == HttpContext::HttpType)
        {
            HttpParser* parser = static_cast<HttpParser*>(c->context);
            parser->onConnRead(conn, buf, count);
        }
        else
        { 
            WsConnection* ws = static_cast<WsConnection*>(c->context);
            ws->onRead(conn, buf, count);
        }

    }

    void HttpServer::setHttpCallback(const string& path, const HttpCallback_t& conn)
    {
        m_httpFuncs[path] = conn;    
    }

    void HttpServer::onRequest(const ConnectionPtr_t& conn, const HttpRequest& request)
    {
        map<string, HttpCallback_t>::iterator iter = m_httpFuncs.find(request.path);
        if(iter == m_httpFuncs.end())
        {
            m_httpFuncs[rootPath](request, conn);  
        }
        else
        {
            (iter->second)(request, conn);    
        }
    }

    void HttpServer::setWsCallback(const string& path, const WsCallback_t& func)
    {
        m_wsFuncs[path] = func;    
    }

    void HttpServer::onWebsocket(const ConnectionPtr_t& conn, const HttpRequest& request, const char* buffer, size_t count)
    {
        map<string, WsCallback_t>::iterator iter = m_wsFuncs.find(request.path);
        if(iter == m_wsFuncs.end())
        {
            conn->shutDown();
        }
        else
        {
            WsConnection* wsConn = new WsConnection(iter->second);
            HttpContextPtr_t c(new HttpContext(HttpContext::WsType, wsConn));

            if(wsConn->onHandshake(conn, request) == 0)
            { 
                conn->setContext(c);

                wsConn->onRead(conn, buffer, count);
            }
           
            return;
        }
    }
}
