#ifndef _IOLOOPTHREADPOOL_H_
#define _IOLOOPTHREADPOOL_H_

#include <vector>
#include <string>

namespace tnet
{
    class IOLoop;
    class IOLoopThread;
    class IOLoopThreadPool
    {
    public:
        IOLoopThreadPool(int threadNum, const std::string& threadName = "");
        ~IOLoopThreadPool();
        
        void start();
        void stop();

        int getPoolNum() { return (int)m_loops.size(); }
        IOLoop* getHashLoop(int hashKey) { return m_loops[hashKey % getPoolNum()]; }
        IOLoop* getLoop(int index) { return m_loops[index]; }
        const std::vector<IOLoop*>& getLoops() { return m_loops; }
    
    private:
        std::vector<IOLoopThread*> m_threads;
        std::vector<IOLoop*> m_loops;
    };
    
}

#endif
