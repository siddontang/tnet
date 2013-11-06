#include "httpserver.h"

#include <string>

#include "tcpserver.h"
#include "log.h"
#include "httpconnection.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "wsconnection.h"
#include "connection.h"

using namespace std;
using namespace std::tr1::placeholders;

namespace tnet
{
    const int DefaultMaxHeaderSize = 4 * 1024;
    const int DefaultMaxBodySize = 1024 * 1024;

    static string rootPath = "/";

    void httpNotFoundCallback(const HttpConnectionPtr_t& conn, const HttpRequest& request)
    {
        HttpResponse resp;
        resp.statusCode = 404;
        
        conn->send(resp);      
    } 

    int dummyAuthCallback(const HttpRequest&)
    {
        return 0;    
    }

    HttpServer::HttpServer(TcpServer* server)
        : m_server(server)
        , m_maxHeaderSize(DefaultMaxHeaderSize)
        , m_maxBodySize(DefaultMaxBodySize)
    {
        HttpConnection::initSettings();
    
        m_httpFuncs[rootPath] = std::tr1::bind(&httpNotFoundCallback, _1, _2);
    
        m_authFunc = std::tr1::bind(&dummyAuthCallback, _1);
    }
   
    HttpServer::~HttpServer()
    {
        
    }
     
    int HttpServer::listen(const Address& addr)
    {
        return m_server->listen(addr, std::tr1::bind(&HttpServer::onConnEvent, this, _1, _2, _3, _4));     
    }

    void HttpServer::onConnEvent(const ConnectionPtr_t& conn, ConnEvent event, const char* buf, size_t count)
    {
        switch(event)
        {
            case Conn_EstablishEvent:
                {
                    HttpConnectionPtr_t httpConn(new HttpConnection(this, conn));
                    conn->setEventCallback(std::tr1::bind(&HttpServer::onHttpConnEvent, this, httpConn, _1, _2, _3, _4));
                }
                break;
            default:
                LOG_INFO("error when enter this");
                return;
        }
    }

    void HttpServer::onHttpConnEvent(const HttpConnectionPtr_t& httpConn, const ConnectionPtr_t& conn, ConnEvent event, const char* buf, size_t count)
    {
        switch(event)
        {
            case Conn_ReadEvent:
                httpConn->onRead(conn, buf, count);
                break;
            default:
                break;    
        }    
    }

    void HttpServer::onWsConnEvent(const WsConnectionPtr_t& wsConn, const ConnectionPtr_t& conn, ConnEvent event, const char* buf, size_t count)
    {
        switch(event)
        {
            case Conn_ReadEvent:
                wsConn->onRead(conn, buf, count);
                break;
            default:
                break;    
        }
    }

    void HttpServer::setHttpCallback(const string& path, const HttpCallback_t& conn)
    {
        m_httpFuncs[path] = conn;    
    }

    void HttpServer::onRequest(const HttpConnectionPtr_t& conn, const HttpRequest& request)
    {
        map<string, HttpCallback_t>::iterator iter = m_httpFuncs.find(request.path);
        if(iter == m_httpFuncs.end())
        {
            m_httpFuncs[rootPath](conn, request);  
        }
        else
        {
            (iter->second)(conn, request);    
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
            WsConnectionPtr_t wsConn(new WsConnection(conn, iter->second));

            if(wsConn->onHandshake(conn, request) == 0)
            { 
                wsConn->onRead(conn, buffer, count);
                conn->setEventCallback(std::tr1::bind(&HttpServer::onWsConnEvent, this, wsConn, _1, _2, _3, _4));
            }
            else
            {
                conn->shutDown();    
            }
           
            return;
        }
    }

    int HttpServer::onAuth(const HttpRequest& request)
    {
        if(m_authFunc(request) != 0)
        {
            return -1;         
        }    

        return 0;
    }
}
