#include "ioloopthreadpool.h"

#include <tr1/functional>
#include <assert.h>

#include "ioloopthread.h"
#include "misc.h"

using namespace std;
using namespace std::tr1::placeholders;

namespace tnet
{

    IOLoopThreadPool::IOLoopThreadPool(int num, const string& threadName)
    {
        assert(num > 0);
        m_threads.resize(num);
        m_loops.resize(num);

        for(int i = 0; i < num; i++)
        {
            m_threads[i] = new IOLoopThread(threadName);
            m_loops[i] = m_threads[i]->getLoop();        
        }
    }

    IOLoopThreadPool::~IOLoopThreadPool()
    {
        m_loops.clear();
        for_each_all_delete(m_threads);
    }
    
    void IOLoopThreadPool::start()
    {
        for_each_all(m_threads, std::tr1::bind(&IOLoopThread::start, _1));
    }    

    void IOLoopThreadPool::stop()
    {
        for_each_all(m_threads, std::tr1::bind(&IOLoopThread::stop, _1));
    }
}
