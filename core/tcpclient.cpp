#include "tcpclient.h"

#include <assert.h> 

#include "ioloop.h"
#include "sockutil.h"
#include "ioloopthreadpool.h"
#include "connchecker.h"
#include "log.h"
#include "connection.h"

using namespace std;
using namespace std::tr1::placeholders;

namespace tnet
{

    TcpClient::TcpClient(int num, int connNum)
        : m_maxConnections(connNum)
        , m_curConnections(0)
    {
        m_pool = new IOLoopThreadPool(num, "connpool");
        m_loops = m_pool->getLoops();

        m_connections.resize(int(connNum * 1.5));

        Connection::setReleaseFunc(std::tr1::bind(&TcpClient::deleteConnection, this, _1)); 

        m_checker = new ConnChecker(m_pool->getLoops(), m_connections);
    }

    TcpClient::~TcpClient()
    {
        delete m_checker;
        delete m_pool;
    }

    void TcpClient::start()
    {
        m_pool->start();
        m_checker->start();    
    }

    void TcpClient::stop()
    {
        m_checker->stop();
        m_pool->stop();    
    }

    int TcpClient::connect(const Address& addr, const ConnectionFunc_t& func)
    {
        int fd = SockUtil::create();
        if(fd < 0)
        {
            return fd;    
        }

        int connNum = __sync_add_and_fetch(&m_curConnections, 1);
        if(connNum > m_maxConnections)
        {
            LOG_ERROR("exceed max connections");
            close(fd);
            __sync_sub_and_fetch(&m_curConnections, 1);
            return -1;    
        }

        IOLoop* loop = m_pool->getHashLoop(fd);

        ConnectionPtr_t conn(new Connection(loop, fd));
        
        conn->setEventCallback(func);
        m_connections[fd] = conn;

        conn->connect(addr);
        return fd;
    }

    void TcpClient::deleteConnection(const ConnectionPtr_t& conn)
    {
        IOLoop* loop = conn->getLoop();

        if(!loop->inLoopThread())
        {
            loop->addTask(std::tr1::bind(&TcpClient::deleteConnectionInLoop, this, conn));    
        }
        else
        {
            deleteConnectionInLoop(conn);    
        }
    }

    void TcpClient::deleteConnectionInLoop(const ConnectionPtr_t& conn)
    {
        int fd = conn->getSockFd();
        
        m_connections[fd].reset();    

        __sync_sub_and_fetch(&m_curConnections, 1);
    }
}
