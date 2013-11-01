#include "signaler.h"

#include "ioloop.h"
#include "misc.h"

namespace tnet
{
    Signaler::Signaler(IOLoop* loop)
        : m_loop(loop)
        , m_watchers(NSIG, NULL)
    {
         
    }   
    
    Signaler::~Signaler()
    {
        for_each_all_delete(m_watchers);
    } 

    void Signaler::add(int signum, const SignalCallback_t& func)
    {
        if(signum >= int(m_watchers.size()))
        {
            return;
        }

        Callback_t taskFunc = std::tr1::bind(&Signaler::addInLoop, this, signum, func);
        m_loop->runTask(taskFunc);          
    }

    void Signaler::remove(int signum)
    {
        if(signum >= int(m_watchers.size()))
        {
            return;
        }

        Callback_t func = std::tr1::bind(&Signaler::removeInLoop, this, signum);
        m_loop->runTask(func);
    }

    void Signaler::clear()
    {
        Callback_t func = std::tr1::bind(&Signaler::clearInLoop, this);
        m_loop->runTask(func);    
    }

    void Signaler::addInLoop(int signum, const SignalCallback_t& func)
    {
        if(!m_loop->isMainLoop())
        {
            return;    
        }

        Watcher* w = m_watchers[signum];
        if(!w)
        {
            w = new Watcher;
            ev_signal_init(&w->signal, Signaler::onSignal, signum);
            m_watchers[signum] = w;
        }
        
        ev_signal_start(m_loop->evloop(), &w->signal);
        w->func = func;    
    }

    void Signaler::removeInLoop(int signum)
    {
        if(!m_loop->isMainLoop())
        {
            return;    
        }

        Watcher* w = m_watchers[signum];
        if(!w)
        {
            return;    
        }    

        ev_signal_stop(m_loop->evloop(), &w->signal);
    }

    void Signaler::clearInLoop()
    {
        if(!m_loop->isMainLoop())
        {
            return;    
        }

        for(size_t i = 0; i < m_watchers.size(); i++)
        {
            removeInLoop(int(i));
        }
    }

    void Signaler::onSignal(struct ev_loop* loop, struct ev_signal* w, int revents)
    {
        Watcher* watcher = (Watcher*)w;
        
        (watcher->func)(w->signum);    
    }
}
