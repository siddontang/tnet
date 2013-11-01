#ifndef _CONNCHECKER_H_
#define _CONNCHECKER_H_

#include <tr1/memory>
#include <tr1/functional>
#include <vector>

#include "coredefs.h"

namespace tnet
{
    class IOLoop;
    class Timer;
    class Connection;
    class IOLoopPoolTimer;

    class ConnChecker
    {
    public:
        ConnChecker(const std::vector<IOLoop*>& connLoops, const std::vector<ConnectionPtr_t>& connections);
        ~ConnChecker();

        void start();
        void stop();
    
        const std::vector<IOLoop*>& getLoops() { return m_loops; }

        IOLoop* getHashLoop(int fd) { return m_loops[ fd % m_loops.size() ]; }

        void setConnCheckRepeat(int seconds);

        void setConnCheckStep(int step) { m_connCheckStep = step; }
        void setConnTimeout(int seconds) { m_connTimeout = seconds; }
        void setConnectTimeout(int seconds) { m_connectTimeout = seconds; }

    private:
        void onConnCheck(const TimerPtr_t& timer);
        
    private:
        IOLoopPoolTimer* m_connChecker;

        const std::vector<IOLoop*>& m_loops;
        const std::vector<ConnectionPtr_t>& m_connections;

        int m_connCheckRepeat;
        int m_connCheckStep;
        int m_connTimeout;
        int m_connectTimeout;
    };
    
}

#endif
