#include "connlooppool.h"

#include <assert.h>

#include "ioloopthreadpool.h"
#include "ioloop.h"
#include "timer.h"
#include "tcpserver.h"
#include "connection.h"
#include "misc.h"

using namespace std;
using namespace std::tr1::placeholders;

namespace tnet
{
    const int DefaultConnCheckRepeat = 10;
    const int DefaultConnCheckStep = 2000;
    const int DefaultConnTimeout = 60;


    ConnLoopPool::ConnLoopPool(int loopNum, TcpServer* server)
        : m_server(server)
    {
        m_connCheckRepeat = DefaultConnCheckRepeat;
        m_connCheckStep = DefaultConnCheckStep;
        m_connTimeout = DefaultConnTimeout;

        m_pool = new IOLoopThreadPool(loopNum, "connloop");
        
        m_loops = m_pool->getLoops();
    
        m_connChecker.resize(m_loops.size());
        for(size_t i = 0; i < m_loops.size(); ++i)
        {
            m_connChecker[i] = new Timer(std::tr1::bind(&ConnLoopPool::onConnCheck, 
                this, m_loops[i], std::tr1::shared_ptr<int>(new int(0))),
                DefaultConnCheckRepeat * 1000, DefaultConnTimeout * 1000);
        }
    }

    ConnLoopPool::~ConnLoopPool()
    {
        for_each_all_delete(m_connChecker);

        delete m_pool;
    }
    
    void ConnLoopPool::start()
    {
        m_pool->start();
        
        for_each_all(m_connChecker, std::tr1::bind(&Timer::start, _1));     
    }

    void ConnLoopPool::stop()
    {
        for_each_all(m_connChecker, std::tr1::bind(&Timer::stop, _1));     
     
        m_pool->stop();    
    }

    void ConnLoopPool::setIOInterval(int milliseconds)
    {
        assert(milliseconds > 0);
        for_each_all(m_loops, std::tr1::bind(&IOLoop::setIOInterval, _1, milliseconds));     
    }

    void ConnLoopPool::setConnCheckRepeat(int seconds)
    {            
        assert(seconds > 0);
        for_each_all(m_connChecker, std::tr1::bind(&Timer::reset, _1, seconds * 1000));     
    }

    void ConnLoopPool::onConnCheck(IOLoop* loop, const std::tr1::shared_ptr<void>& content)
    {
        std::tr1::shared_ptr<int> num = std::tr1::static_pointer_cast<int>(content); 
        
        int lastIndex = *num;

        int index = lastIndex;

        ev_tstamp now = ev_now(loop->evloop()); 

        vector<ConnectionPtr_t>& connections = m_server->getConnections();

        for(int i = 0; i < m_connCheckStep; ++i, ++index)
        {
            int fd = index % connections.size();
            if(fd  == lastIndex && fd != index)
            {
                //travel for a loop
                break;    
            }

            if(getHashLoop(fd) == loop)
            {
                ConnectionPtr_t conn = connections[fd];
                if(conn && int(now - conn->getLastUpdate()) > m_connTimeout) 
                {
                    conn->shutDown();    
                }   
            }
        }

        *num = index % m_connections.size();
    }
}
