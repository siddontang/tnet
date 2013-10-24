#include "connchecker.h"

#include <assert.h>

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
    const int DefaultConnectTimeout = 20; 

    ConnChecker::ConnChecker(const vector<IOLoop*>& connLoops, const vector<ConnectionPtr_t>& connections)
        : m_loops(connLoops)
        , m_connections(connections)
    {
        m_connCheckRepeat = DefaultConnCheckRepeat;
        m_connCheckStep = DefaultConnCheckStep;
        m_connTimeout = DefaultConnTimeout;
        m_connectTimeout = DefaultConnectTimeout;

        m_connChecker.resize(m_loops.size());
        for(size_t i = 0; i < m_loops.size(); ++i)
        {
            m_connChecker[i] = new Timer(m_loops[i], std::tr1::bind(&ConnChecker::onConnCheck, 
                this, m_loops[i], std::tr1::shared_ptr<int>(new int(0))),
                DefaultConnCheckRepeat * 1000, DefaultConnTimeout * 1000);
        }
    }

    ConnChecker::~ConnChecker()
    {
        for_each_all_delete(m_connChecker);
    }
    
    void ConnChecker::start()
    {
        for_each_all(m_connChecker, std::tr1::bind(&Timer::start, _1));     
    }

    void ConnChecker::stop()
    {
        for_each_all(m_connChecker, std::tr1::bind(&Timer::stop, _1));     
    }


    void ConnChecker::setConnCheckRepeat(int seconds)
    {            
        assert(seconds > 0);
        for_each_all(m_connChecker, std::tr1::bind(&Timer::reset, _1, seconds * 1000));     
    }

    void ConnChecker::onConnCheck(IOLoop* loop, const std::tr1::shared_ptr<void>& content)
    {
        std::tr1::shared_ptr<int> num = std::tr1::static_pointer_cast<int>(content); 
        
        int lastIndex = *num;

        int index = lastIndex;

        ev_tstamp now = ev_now(loop->evloop()); 

        for(int i = 0; i < m_connCheckStep; ++i, ++index)
        {
            int fd = index % m_connections.size();
            if(fd  == lastIndex && fd != index)
            {
                //travel a round
                break;    
            }

            if(getHashLoop(fd) == loop)
            {
                ConnectionPtr_t conn = m_connections[fd];
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

        *num = index % m_connections.size();
    }
}
