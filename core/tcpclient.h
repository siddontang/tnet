#ifndef _TCPCLIENT_H_
#define _TCPCLIENT_H_

#include <tr1/functional>
#include <tr1/memory>

#include <vector>

#include "connevent.h"
#include "coredefs.h"

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
        TcpClient(int loopNum, int connNum);        
        ~TcpClient();

        void start();
        void stop();

        int connect(const Address& addr, const ConnEventCallback_t& func);

        const std::vector<IOLoop*>& getLoops() { return m_loops; }

    private:
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
