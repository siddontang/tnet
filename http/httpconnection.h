#ifndef _HTTPCONNECTION_H_
#define _HTTPCONNECTION_H_

#include <string>
#include <map>
#include <tr1/memory>

#include "nocopyable.h"

namespace tnet
{
    class Connection;
    class HttpServer;
    class HttpRequest;

    //inner class work with httpserver
    class HttpConnection : public nocopyable
    {
    public:
        typedef std::tr1::weak_ptr<Connection> WeakConnectionPtr_t;
        HttpConnection(const WeakConnectionPtr_t& conn);
        ~HttpConnection();

        void send(const std::string& data);

    private:
        WeakConnectionPtr_t m_conn;
    };
    
}

#endif
