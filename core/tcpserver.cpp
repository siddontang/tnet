#include "tcpserver.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#include "ioloopthreadpool.h"
#include "ioloop.h"
#include "sockutil.h"
#include "misc.h"
#include "log.h"
#include "acceptor.h"
#include "signaler.h"
#include "connection.h"
#include "timer.h"
#include "connlooppool.h"

using namespace std;
using namespace std::tr1::placeholders;

namespace tnet
{
    TcpServer::TcpServer(int acceptLoopNum, int connLoopNum, int maxConnections)
    {
        assert(maxConnections > 0);
        m_mainLoop = new IOLoop();

        m_acceptor = new Acceptor(acceptLoopNum);

        m_connPool = new ConnLoopPool(connLoopNum, this);
        m_connLoops = m_connPool->getLoops();
   
        m_signaler = new Signaler(m_mainLoop);

        m_maxConnections = maxConnections;
        m_curConnections = 0;
    
        //int sysMaxFileNum = getdtablesize();
   
        //we prealloc connection vector and never change its size
        m_connections.resize(int(m_maxConnections * 1.5) + 1024);
    }
    
    TcpServer::~TcpServer()
    {
        delete m_connPool;
        delete m_acceptor;

        delete m_mainLoop;
    
        m_connections.clear();
    }

    void TcpServer::onNewConnection(int sockFd, const NewConnCallback_t& func)
    {
        int connNum = __sync_add_and_fetch(&m_curConnections, 1);

        if(connNum > m_maxConnections)
        {
            LOG_ERROR("exceed max connections");
            close(sockFd);
            __sync_sub_and_fetch(&m_curConnections, 1);
            return;    
        }

        IOLoop* loop = m_connPool->getHashLoop(sockFd);

        loop->runTask(std::tr1::bind(&TcpServer::newConnectionInLoop, this, loop, sockFd, func));
    }

    void TcpServer::newConnectionInLoop(IOLoop* loop, int sockFd, const NewConnCallback_t& func)
    {
        ConnectionPtr_t conn(new Connection(loop, sockFd, std::tr1::bind(&TcpServer::deleteConnection, this, _1)));
        m_connections[sockFd] = conn;    

        func(conn);

        conn->onEstablished();
    }

    void TcpServer::deleteConnection(const ConnectionPtr_t& conn)
    {
        IOLoop* loop = conn->getLoop();

        if(!loop->inLoopThread())
        {
            loop->addTask(std::tr1::bind(&TcpServer::deleteConnectionInLoop, this, conn));    
        }
        else
        {
            deleteConnectionInLoop(conn);   
        }
    }

    void TcpServer::deleteConnectionInLoop(const ConnectionPtr_t& conn)
    {
        IOLoop* loop = conn->getLoop();

        assert(loop->inLoopThread());
        (void)loop;

        int sockFd = conn->getSockFd();
        
        m_connections[sockFd].reset();

        __sync_sub_and_fetch(&m_curConnections, 1);
    }


    int TcpServer::listen(const Address& addr, const NewConnCallback_t& func)
    {
        LOG_INFO("listen %s:%d", addr.ipstr().c_str(), addr.port());
        return m_acceptor->listen(addr, std::tr1::bind(&TcpServer::onNewConnection, this, _1, func));
    }
   
    void TcpServer::setConnLoopIOInterval(int milliseconds)
    {
        m_connPool->setIOInterval(milliseconds);
    }
    
    void TcpServer::addSignal(int signum, const SignalFunc_t& func)
    {
        m_signaler->add(signum, func);   
    }

    void TcpServer::setConnCheckRepeat(int seconds)
    {
        m_connPool->setConnCheckRepeat(seconds);    
    }

    void TcpServer::setConnCheckStep(int step)
    {
        m_connPool->setConnCheckStep(step);
    }

    void TcpServer::setConnTimeout(int seconds)
    {
        m_connPool->setConnTimeout(seconds);
    }

    void TcpServer::start()
    {
        m_connPool->start();

        m_acceptor->start();

        m_mainLoop->start();
    }  

    void TcpServer::stop()
    {
        m_signaler->clear();
    
        m_acceptor->stop();
    
        m_connPool->stop();
    
        m_mainLoop->stop();
    }
}


