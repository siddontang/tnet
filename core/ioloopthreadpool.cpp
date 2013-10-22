#include "ioloopthreadpool.h"

#include <assert.h>

#include "ioloopthread.h"

using namespace std;

namespace tnet
{

    IOLoopThreadPool::IOLoopThreadPool(int num)
    {
        assert(num > 0);
        m_threads.resize(num);
        m_loops.resize(num);

        for(int i = 0; i < num; i++)
        {
            m_threads[i] = new IOLoopThread();
            m_loops[i] = m_threads[i]->getLoop();        
        }
    }

    IOLoopThreadPool::~IOLoopThreadPool()
    {
        m_loops.clear();
        for(size_t i = 0; i < m_threads.size(); ++i)
        {
            delete m_threads[i];
        }
    }
    
    void IOLoopThreadPool::start()
    {
        for(size_t i = 0; i < m_threads.size(); i++)
        {
            m_threads[i]->start();    
        }
    }    

    void IOLoopThreadPool::stop()
    {
        for(size_t i = 0; i < m_threads.size(); i++)
        {
            m_threads[i]->stop();    
        }
    }
}
