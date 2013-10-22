#ifndef _HTTPCONNECTION_H_
#define _HTTPCONNECTION_H_

#include <string>
#include <map>
#include <tr1/memory>

extern "C"
{
#include "http_parser.h"
}

#include "nocopyable.h"

namespace tpush
{
    class Connection;
    class HttpServer;
    class HttpRequest;

    //inner class work with httpserver
    class HttpConnection : public nocopyable
    {
    public:
        typedef std::tr1::shared_ptr<Connection> ConnectionPtr_t; 
        typedef std::tr1::shared_ptr<HttpRequest> HttpRequestPtr_t;

        HttpConnection(const ConnectionPtr_t& conn, HttpServer* server);
        ~HttpConnection();

        struct http_parser* getParser() { return &m_parser; }
        void resetParser();

        bool eof() { return m_eof; }

        const HttpRequestPtr_t& getRequest() { return m_request; }

        static void initParserSettings(struct http_parser_settings* settings);

    private:
        static int onMessageBegin(struct http_parser*);
        static int onUrl(struct http_parser*, const char*, size_t);
        static int onStatusComplete(struct http_parser*);
        static int onHeaderField(struct http_parser*, const char*, size_t);
        static int onHeaderValue(struct http_parser*, const char*, size_t);
        static int onHeadersComplete(struct http_parser*);
        static int onBody(struct http_parser*, const char*, size_t);
        static int onMessageComplete(struct http_parser*);
    
        int handleMessageBegin();
        int handleUrl(const char*, size_t);
        int handleStatusComplete();
        int handleHeaderField(const char*, size_t);
        int handleHeaderValue(const char*, size_t);
        int handleHeadersComplete();
        int handleBody(const char*, size_t);
        int handleMessageComplete();

        bool validHeaderSize();

    private:
        std::tr1::weak_ptr<Connection> m_conn;
        HttpServer* m_server;
        struct http_parser m_parser;
        
        HttpRequestPtr_t m_request;

        //for parse http header
        std::string m_curField;
        bool m_lastWasValue;

        bool m_eof;
    };
    
}

#endif
