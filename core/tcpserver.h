#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include <string>
#include <vector>
#include <tr1/functional>
#include <tr1/memory>

#include "nocopyable.h"
#include "address.h"
#include "threadutil.h"
#include "connection.h"

namespace tnet 
{   
    class IOLoop;
    class IOLoopThreadPool;
    
    class Signaler;
    class Acceptor;
    class Connection;

    class Timer;

    class TcpServer : public nocopyable
    {
    public:
        TcpServer(int acceptLoopNum, int connLoopNum, int maxConnections);
        ~TcpServer();
      
        typedef std::tr1::shared_ptr<Connection> ConnectionPtr_t;
        typedef std::tr1::function<void (const ConnectionPtr_t&, Connection::Event, const char*, int)> ConnEventCallback_t;
      
        int listen(const Address& addr, const ConnEventCallback_t& func);
       
        void setConnLoopIOInterval(int milliseconds);
        void setConnCheckInterval(int milliseconds);
        void setConnCheckNumOneLoop(int checkNum);
        void setMaxConnTimeout(int seconds) { m_maxConnTimeout = seconds; }

        typedef std::tr1::function<void (int)> SignalFunc_t;
        void addSignal(int signum, const SignalFunc_t& func);

        void start();
        void stop();

    private:
        void onNewConnection(int sockFd, const ConnEventCallback_t& func);

        void newConnectionInLoop(IOLoop* loop, int sockFd, const ConnEventCallback_t& func);

        void deleteConnection(ConnectionPtr_t conn);
        void deleteConnectionInLoop(ConnectionPtr_t conn);

        void onConnEvent(const ConnEventCallback_t& func, 
                         const ConnectionPtr_t& conn, 
                         Connection::Event event, const char* buffer, int count);

        void initConnCheck();
        void startConnCheck();
        void stopConnCheck();
        void onConnCheck(IOLoop* loop, const std::tr1::shared_ptr<void>& content);

    private:
        Acceptor* m_acceptor;

        //threads for handle connections
        IOLoopThreadPool* m_connLoops;
    
        std::vector<Timer*> m_connChecker;
    
        IOLoop* m_mainLoop;

        Signaler* m_signaler;

        std::vector<ConnectionPtr_t> m_connections;
    
        int m_maxConnections;
        volatile int m_curConnections;
    
        int m_maxConnTimeout;
        int m_connCheckNum;

        SpinLock m_lock;
    };
        
}

#endif

