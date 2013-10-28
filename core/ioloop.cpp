#include "ioloop.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>
#include "log.h"

using namespace std;

namespace tnet
{
    //for *unix, new fd descriptors is always the smallest positive integer 
    //vector<IOLoop::SocketWatcher*> IOLoop::m_socketWatchers(getdtablesize(), NULL);

    IOLoop::IOLoop()
    {
        m_stop = false;
        m_runTasks = false;
        m_mainLoop = false;

        m_threadId = 0;

        m_loop = (struct ev_loop*)ev_loop_new(EVFLAG_AUTO);        
        ev_set_userdata(m_loop, this);
            
        ev_async_init(&m_asyncWatcher, &IOLoop::onWaked);
        ev_check_init(&m_checkWatcher, &IOLoop::onChecked);
        ev_set_priority(&m_checkWatcher, EV_MAXPRI);
    
        ev_async_start(m_loop, &m_asyncWatcher);
        ev_check_start(m_loop, &m_checkWatcher);
    }

    IOLoop::~IOLoop()
    {
        ev_async_stop(m_loop, &m_asyncWatcher);
        ev_check_stop(m_loop, &m_checkWatcher);
        
        ev_loop_destroy(m_loop);
    }

    void IOLoop::start()
    {
        m_threadId = pthread_self();
        m_stop = false;

        m_mainLoop = isMainThread(); 

        ev_run(m_loop, 0);
    }

    void IOLoop::stop()
    {
        m_stop = true;

        ev_break(m_loop, EVBREAK_ALL);    
        wakeUp();
    }

    bool IOLoop::inLoopThread()
    {
        return m_threadId == pthread_self();    
    }

    void IOLoop::wakeUp()
    {
        ev_async_send(m_loop, &m_asyncWatcher);    
    }

    void IOLoop::onWaked(struct ev_loop* loop, struct ev_async* w, int revent)
    {
        IOLoop* ioloop = (IOLoop*)ev_userdata(loop);
        ioloop->handleWaker();
    }

    void IOLoop::handleWaker()
    {
        if(m_stop)
        {
            ev_break(m_loop, EVBREAK_ALL);
            return;    
        }
    }

    void IOLoop::runTask(const Callback_t& func)
    {
        if(m_stop)
        {
            return;    
        }

        if(inLoopThread())
        {
            func();    
        }
        else
        {
            addTask(func);
        }
    }

    void IOLoop::addTask(const Callback_t& func)
    {
        if(m_stop)
        {
            return;    
        }

        {
            SpinLockGuard gu(m_taskLock);
            m_tasks.push_back(func);    
        }

        if(!inLoopThread() || m_runTasks)
        {
            wakeUp();   
        }
    }

    void IOLoop::runTasks()
    {
        m_runTasks = true;

        vector<Callback_t> tasks;

        {
            SpinLockGuard gu(m_taskLock);
            m_tasks.swap(tasks);    
        }

        for(size_t i = 0; i < tasks.size() && !m_stop; ++i)
        {
            tasks[i]();
        }

        m_runTasks = false;    
    }

    void IOLoop::onChecked(struct ev_loop* loop, struct ev_check* w, int revents)
    {
        IOLoop* ioloop = (IOLoop*)ev_userdata(loop);
        ioloop->check();    
    }

    void IOLoop::check()
    {
        if(m_stop)
        {
            ev_check_stop(m_loop, &m_checkWatcher);
        }

        runTasks();
    }
}
