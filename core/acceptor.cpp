#include "acceptor.h"

#include <tr1/functional>

#include "ioloop.h"
#include "ioloopthreadpool.h"
#include "sockutil.h"
#include "address.h"
#include "acceptloop.h"
#include "misc.h"

using namespace std;
using namespace std::tr1::placeholders;

namespace tnet
{
    Acceptor::Acceptor(int maxPoolNum)
    {
        m_loopPool = new IOLoopThreadPool(maxPoolNum, "accept");
    
        for(int i = 0; i < maxPoolNum; i++)
        {
            AcceptLoop* loop = new AcceptLoop(m_loopPool->getLoop(i));
            m_loops.push_back(loop);    
        }
    }

    Acceptor::~Acceptor()
    {
        for_each_all_delete(m_loops);
        delete m_loopPool;
    }

    void Acceptor::start()
    {
        m_loopPool->start();
        for_each_all(m_loops, std::tr1::bind(&AcceptLoop::start, _1));
    }

    void Acceptor::stop()
    {
        for_each_all(m_loops, std::tr1::bind(&AcceptLoop::stop, _1));
        
        m_loopPool->stop();

        for(size_t i = 0; i < m_sockFds.size(); i++)
        {
            close(m_sockFds[i]);    
        }

        m_sockFds.clear();
    }

    int Acceptor::listen(const Address& addr, const NewConnectionFunc_t& func)
    { 
        int sockFd = SockUtil::bindAndListen(addr);
        if(sockFd < 0)
        {
            return sockFd;
        }

        m_sockFds.push_back(sockFd);

        for_each_all(m_loops, std::tr1::bind(&AcceptLoop::listen, _1, sockFd, func));

        //for(size_t i = 0; i < m_loops.size(); i++)
        //{
        //   m_loops[i]->listen(sockFd, func);
        //}

        return sockFd;
    }
    
}
