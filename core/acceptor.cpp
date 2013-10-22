#include "acceptor.h"

#include "ioloop.h"
#include "ioloopthreadpool.h"
#include "sockutil.h"
#include "address.h"
#include "acceptloop.h"
#include "misc.h"

using namespace std;

namespace tnet
{
    Acceptor::Acceptor(int maxPoolNum)
    {
        m_loopPool = new IOLoopThreadPool(maxPoolNum);
    
        for(int i = 0; i < maxPoolNum; i++)
        {
            AcceptLoop* loop = new AcceptLoop(m_loopPool->getLoop(i));
            m_loops.push_back(loop);    
        }
    }

    Acceptor::~Acceptor()
    {
        clearContainer(m_loops);
        delete m_loopPool;
    }

    void Acceptor::start()
    {
        m_loopPool->start();
        for(size_t i = 0; i < m_loops.size(); i++)
        {
            m_loops[i]->start();    
        }
    }

    void Acceptor::stop()
    {
        for(size_t i = 0; i < m_loops.size(); i++)
        {
            m_loops[i]->stop();    
        }
        m_loopPool->stop();

        for(size_t i = 0; i < m_sockFds.size(); i++)
        {
            close(m_sockFds[i]);    
        }
    }

    int Acceptor::listen(const Address& addr, const NewConnectionFunc_t& func)
    { 
        int sockFd = SockUtil::bindAndListen(addr);
        if(sockFd < 0)
        {
            return sockFd;
        }

        m_sockFds.push_back(sockFd);

        for(size_t i = 0; i < m_loops.size(); i++)
        {
            m_loops[i]->listen(sockFd, func);
        }

        return sockFd;
    }
    
}
