#include "connchecker.h"

#include <assert.h>

#include "ioloop.h"
#include "timer.h"
#include "tcpserver.h"
#include "connection.h"
#include "misc.h"
#include "iolooppooltimer.h"
#include "log.h"

using namespace std;
using namespace std::tr1::placeholders;

namespace tnet
{
    const int DefaultConnCheckRepeat = 10;
    const int DefaultConnCheckStep = 2000;
    const int DefaultConnTimeout = 60;
    const int DefaultConnectTimeout = 20; 

    ConnChecker::ConnChecker(const vector<IOLoop*>& connLoops, const vector<ConnectionPtr_t>& connections)
        : m_loops(connLoops)
        , m_connections(connections)
    {
        m_connCheckRepeat = DefaultConnCheckRepeat;
        m_connCheckStep = DefaultConnCheckStep;
        m_connTimeout = DefaultConnTimeout;
        m_connectTimeout = DefaultConnectTimeout;

        m_connChecker = new IOLoopPoolTimer(m_loops, std::tr1::bind(&ConnChecker::onConnCheck, this, _1), 
                DefaultConnCheckRepeat * 1000, DefaultConnTimeout * 1000);
    }

    ConnChecker::~ConnChecker()
    {
        delete m_connChecker;
    }
    
    void ConnChecker::start()
    {
        m_connChecker->start();
    }

    void ConnChecker::stop()
    {
        m_connChecker->stop();
    }


    void ConnChecker::setConnCheckRepeat(int seconds)
    {            
        m_connChecker->reset(seconds * 1000);
    }

    void ConnChecker::onConnCheck(const TimerPtr_t& timer)
    {
        IOLoop* loop = timer->getLoop();
        IntPtr_t num = std::tr1::static_pointer_cast<int>(timer->getContext()); 
        if(!num)
        {
            num = IntPtr_t(new int(0));
            timer->setContext(num);    
        }

        int lastIndex = *num;

        int index = lastIndex;

        ev_tstamp now = ev_now(loop->evloop()); 

        int connNum = int(m_connections.size());

        for(int i = 0; i < m_connCheckStep && i < connNum; ++i, ++index)
        {
            if(index == connNum)
            {
                index = 0;    
            }

            if(getHashLoop(index) == loop)
            {
                ConnectionPtr_t conn = m_connections[index];
                if(conn)
                {
                    ev_tstamp lastUpdate = conn->getLastUpdate();
                    int t = int(now - lastUpdate);
                    if(t > m_connTimeout || (conn->isConnecting() && t > m_connectTimeout))
                    {
                        conn->shutDown();    
                    }     
                }
            }
        }

        *num = index;
    }
}
