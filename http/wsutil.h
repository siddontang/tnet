#ifndef _WSUTIL_H_
#define _WSUTIL_H_

#include <tr1/memory>
#include <tr1/functional>
#include <string>

namespace tnet
{
    class Connection;
    class WsUtil
    {
    public:
        typedef std::tr1::shared_ptr<Connection> ConnectionPtr_t;
        static void ping(const ConnectionPtr_t& conn, const std::string& message);
        static void send(const ConnectionPtr_t& conn, const std::string& message, bool binary = false);
        static void close(const ConnectionPtr_t& conn);
    };
}

#endif
