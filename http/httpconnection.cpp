#include "httpconnection.h"

#include <string>
#include <stdlib.h>
#include <string.h>

#include "httpserver.h"
#include "connection.h"
#include "httprequest.h"

#include "log.h"

using namespace std;

namespace tnet
{

    HttpConnection::HttpConnection(const ConnectionPtr_t& conn, HttpServer* server)
        : m_conn(conn)
        , m_server(server)
        , m_request(new HttpRequest())
        , m_lastWasValue(true)
        , m_eof(false)
    {
        resetParser();
    }

    HttpConnection::~HttpConnection()
    {
    }

    void HttpConnection::resetParser()
    {
        memset(&m_parser, 0, sizeof(m_parser));
        http_parser_init(&m_parser, HTTP_REQUEST); 
        
        m_parser.data = this;
    }

    void HttpConnection::initParserSettings(struct http_parser_settings* settings)
    {
        settings->on_message_begin = &HttpConnection::onMessageBegin;
        settings->on_url = &HttpConnection::onUrl;
        settings->on_status_complete = &HttpConnection::onStatusComplete;
        settings->on_header_field = &HttpConnection::onHeaderField;
        settings->on_header_value = &HttpConnection::onHeaderValue;
        settings->on_headers_complete = &HttpConnection::onHeadersComplete;
        settings->on_body = &HttpConnection::onBody;
        settings->on_message_complete = &HttpConnection::onMessageComplete;    
    }    

    int HttpConnection::onMessageBegin(struct http_parser* parser)
    {
        HttpConnection* conn = (HttpConnection*)parser->data;
        
        return conn->handleMessageBegin();    
    }

    int HttpConnection::onUrl(struct http_parser* parser, const char* at, size_t length)
    {
        HttpConnection* conn = (HttpConnection*)parser->data;
        return conn->handleUrl(at, length);
    }

    int HttpConnection::onStatusComplete(struct http_parser* parser)
    {
        HttpConnection* conn = (HttpConnection*)parser->data;
        return conn->handleStatusComplete();
    }

    int HttpConnection::onHeaderField(struct http_parser* parser, const char* at, size_t length)
    {
        HttpConnection* conn = (HttpConnection*)parser->data;
        return conn->handleHeaderField(at, length);
    }

    int HttpConnection::onHeaderValue(struct http_parser* parser, const char* at, size_t length)
    {
        HttpConnection* conn = (HttpConnection*)parser->data;
        return conn->handleHeaderValue(at, length);
    }

    int HttpConnection::onHeadersComplete(struct http_parser* parser)
    {
        HttpConnection* conn = (HttpConnection*)parser->data;
        return conn->handleHeadersComplete();
    }

    int HttpConnection::onBody(struct http_parser* parser, const char* at, size_t length)
    {
        HttpConnection* conn = (HttpConnection*)parser->data;
        return conn->handleBody(at, length);
    }

    int HttpConnection::onMessageComplete(struct http_parser* parser)
    {
        HttpConnection* conn = (HttpConnection*)parser->data;
        return conn->handleMessageComplete();
    }

     
    int HttpConnection::handleMessageBegin()
    {
        m_eof = false;
        m_request->clear();
        return 0;    
    }
        
    int HttpConnection::handleUrl(const char* at, size_t length)
    {
        if(!validHeaderSize())
        {
            return -1;    
        }

        m_request->url.append(at, length);
        return 0;
    }
     
    int HttpConnection::handleStatusComplete()
    {
        return 0;
    }
        
    int HttpConnection::handleHeaderField(const char* at, size_t length)
    {
        if(!validHeaderSize())
        {
            return -1;    
        }

        if(m_lastWasValue)
        {
            m_curField.clear();    
        }

        m_curField.append(at, length);

        m_lastWasValue = 0;

        return 0;
    }
        
    int HttpConnection::handleHeaderValue(const char* at, size_t length)
    {
        if(!validHeaderSize())
        {
            return -1;    
        }

        m_request->headers[m_curField].append(at, length);
        m_lastWasValue = 1;

        return 0;
    }
        
    int HttpConnection::handleHeadersComplete()
    {
        return 0;
    }
        
    int HttpConnection::handleBody(const char* at, size_t length)
    {
        if(m_request->body.size() > (uint64_t)m_server->getMaxBodySize())
        {
            return -1;    
        }


        m_request->body.append(at, length);
        return 0;
    }
        
    int HttpConnection::handleMessageComplete()
    {
        m_request->majorVersion = m_parser.http_major;
        m_request->minorVersion = m_parser.http_minor;
        m_request->method = m_parser.method;

        m_eof = true;
        
        ConnectionPtr_t conn = m_conn.lock();
        if(!conn)
        {
            return -1;
        }

        (m_server->getRequestCallback())(conn, m_request);
        return 0;
    }

    bool HttpConnection::validHeaderSize()
    {
        return (m_parser.nread <= (uint32_t)m_server->getMaxHeaderSize());
    }
}
