#ifndef _CONNLOOPPOOL_H_
#define _CONNLOOPPOOL_H_

#include <tr1/memory>
#include <tr1/functional>
#include <vector>

namespace tnet
{
    class TcpServer;
    class IOLoopThreadPool;
    class IOLoop;
    class Timer;

    class ConnLoopPool
    {
    public:
        ConnLoopPool(int loopNum, TcpServer* server);
        ~ConnLoopPool();

        void start();
        void stop();
    
        const std::vector<IOLoop*>& getLoops() { return m_loops; }

        IOLoop* getHashLoop(int fd) { return m_loops[ fd % m_loops.size() ]; }

        void setIOInterval(int milliseconds);

        void setConnCheckRepeat(int seconds);

        void setConnCheckStep(int step) { m_connCheckStep = step; }
        void setConnTimeout(int seconds) { m_connTimeout = seconds; }

    private:
        void onConnCheck(IOLoop* loop, const std::tr1::shared_ptr<void>& content);
        
    private:
        TcpServer* m_server;
        IOLoopThreadPool* m_pool;
        std::vector<Timer*> m_connChecker;
        std::vector<IOLoop*> m_loops;

        int m_connCheckRepeat;
        int m_connCheckStep;
        int m_connTimeout;
    };
    
}

#endif
