#include "httpparser.h"
#include "httpserver.h"
#include "log.h"

namespace tnet
{
    struct http_parser_settings HttpParser::ms_settings;

    HttpParser::HttpParser(HttpServer* server, const ConnectionPtr_t& conn)
        : m_server(server)
        , m_conn(conn)
    {
        http_parser_init(&m_parser, HTTP_REQUEST);
        m_parser.data = this;

        m_lastWasValue = true;
    }

    HttpParser::~HttpParser()
    {
    }

    void HttpParser::initSettings()
    {
        ms_settings.on_message_begin = &HttpParser::onMessageBegin;
        ms_settings.on_url = &HttpParser::onUrl;
        ms_settings.on_status_complete = &HttpParser::onStatusComplete;
        ms_settings.on_header_field = &HttpParser::onHeaderField;
        ms_settings.on_header_value = &HttpParser::onHeaderValue;
        ms_settings.on_headers_complete = &HttpParser::onHeadersComplete;
        ms_settings.on_body = &HttpParser::onBody;
        ms_settings.on_message_complete = &HttpParser::onMessageComplete;    
    }    

    int HttpParser::onMessageBegin(struct http_parser* parser)
    {
        HttpParser* p = (HttpParser*)parser->data;
        
        return p->handleMessageBegin();    
    }

    int HttpParser::onUrl(struct http_parser* parser, const char* at, size_t length)
    {
        HttpParser* p = (HttpParser*)parser->data;
        return p->handleUrl(at, length);
    }

    int HttpParser::onStatusComplete(struct http_parser* parser)
    {
        HttpParser* p = (HttpParser*)parser->data;
        return p->handleStatusComplete();
    }

    int HttpParser::onHeaderField(struct http_parser* parser, const char* at, size_t length)
    {
        HttpParser* p = (HttpParser*)parser->data;
        return p->handleHeaderField(at, length);
    }

    int HttpParser::onHeaderValue(struct http_parser* parser, const char* at, size_t length)
    {
        HttpParser* p = (HttpParser*)parser->data;
        return p->handleHeaderValue(at, length);
    }

    int HttpParser::onHeadersComplete(struct http_parser* parser)
    {
        HttpParser* p = (HttpParser*)parser->data;
        return p->handleHeadersComplete();
    }

    int HttpParser::onBody(struct http_parser* parser, const char* at, size_t length)
    {
        HttpParser* p = (HttpParser*)parser->data;
        return p->handleBody(at, length);
    }

    int HttpParser::onMessageComplete(struct http_parser* parser)
    {
        HttpParser* p = (HttpParser*)parser->data;
        return p->handleMessageComplete();
    }

     
    int HttpParser::handleMessageBegin()
    {
        m_request.clear();
        m_curField.clear();
        m_lastWasValue = true;
        return 0;    
    }
        
    int HttpParser::handleUrl(const char* at, size_t length)
    {
        if(!validHeaderSize())
        {
            return -1;    
        }

        m_request.url.append(at, length);
        return 0;
    }
     
    int HttpParser::handleStatusComplete()
    {
        return 0;
    }
        
    int HttpParser::handleHeaderField(const char* at, size_t length)
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
        
    int HttpParser::handleHeaderValue(const char* at, size_t length)
    {
        if(!validHeaderSize())
        {
            return -1;    
        }

        m_request.headers[m_curField].append(at, length);
        m_lastWasValue = 1;

        return 0;
    }
        
    int HttpParser::handleHeadersComplete()
    {
        return 0;
    }
        
    int HttpParser::handleBody(const char* at, size_t length)
    {
        if(m_request.body.size() > (uint64_t)m_server->getMaxBodySize())
        {
            return -1;    
        }


        m_request.body.append(at, length);
        return 0;
    }
        
    int HttpParser::handleMessageComplete()
    {
        m_request.majorVersion = m_parser.http_major;
        m_request.minorVersion = m_parser.http_minor;
        m_request.method = m_parser.method;
        
        if(!m_parser.upgrade)
        {
            ConnectionPtr_t conn = m_conn.lock();
            if(conn)
            {
                m_server->onRequest(m_request, conn);
            }
            else
            {
                return -1;    
            }
        }

        return 0;
    }

    bool HttpParser::validHeaderSize()
    {
        return (m_parser.nread <= (uint32_t)m_server->getMaxHeaderSize());
    }

    void HttpParser::onConnRead(const ConnectionPtr_t& conn, const char* buffer, int count)
    {
        int n = http_parser_execute(&m_parser, &ms_settings, buffer, count);
        if(m_parser.upgrade)
        {
            //websokcet here, we may later support it
        }
        else if(n != count)
        {
            //http parser error, shutdown
            conn->shutDown();
            return;    
        }
    }
}
