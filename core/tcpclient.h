#ifndef _TCPCLIENT_H_
#define _TCPCLIENT_H_

#include <tr1/functional>
#include <tr1/memory>

#include <vector>

#include "connection.h"

namespace tnet
{
    class IOLoop;
    class Address;
    class Connection;
    class IOLoopThreadPool;
    class ConnChecker;

    class TcpClient
    {
    public:
        typedef std::tr1::shared_ptr<Connection> ConnectionPtr_t;

        TcpClient(int loopNum, int connNum);        
        ~TcpClient();

        void start();
        void stop();

        typedef std::tr1::function<void (const ConnectionPtr_t&, Connection::Event, const char*, int)> ConnectionFunc_t;
        int connect(const Address& addr, const ConnectionFunc_t& func);

        const std::vector<IOLoop*>& getLoops() { return m_loops; }

    private:
        void onConnEvent(const ConnectionFunc_t& func, const ConnectionPtr_t& conn, Connection::Event event, const char* buf, int count);
        void deleteConnection(const ConnectionPtr_t& conn);
        void deleteConnectionInLoop(const ConnectionPtr_t& conn);

    private:
        IOLoopThreadPool* m_pool;
        ConnChecker* m_checker;

        std::vector<IOLoop*> m_loops;
        std::vector<ConnectionPtr_t> m_connections;
    
        int m_maxConnections;
        volatile int m_curConnections;
    };
    
}

#endif
