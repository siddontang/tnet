#ifndef _HTTPPARSER_H_
#define _HTTPPARSER_H_

#include <tr1/memory>
#include <tr1/functional>

extern "C"
{
#include "http_parser.h"
}

#include "connection.h"
#include "httprequest.h"
#include "nocopyable.h"

namespace tnet
{
    class HttpServer;

    //for http inner use
    class HttpParser : public nocopyable
    {
    public:
        friend class HttpServer;

        HttpParser(HttpServer* server);
        ~HttpParser();

        static void initSettings();

        typedef std::tr1::shared_ptr<Connection> ConnectionPtr_t;

        void onConnEvent(const ConnectionPtr_t& conn, Connection::Event event, const char* buffer, int count);

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

        void onConnRead(const ConnectionPtr_t& conn, const char* buffer, int count);

    private:    
        static struct http_parser_settings ms_settings;
         
        HttpServer* m_server;

        struct http_parser m_parser;

        HttpRequest m_request;

        //for parse http header
        std::string m_curField;
        bool m_lastWasValue;
    };    
}

#endif
