#include "timer.h"

#include "ioloop.h"

namespace tnet
{

    Timer::Timer(IOLoop* loop, const TimerCallback_t& func, int repeat, int after)
        : m_loop(loop)
    {
        m_func = func;
        ev_timer_init(&m_timer, Timer::onTimer, (ev_tstamp)(after / 1000.0), (ev_tstamp)(repeat / 1000.0));         
    }

    Timer::~Timer()
    {
        
    }

    void Timer::start()
    {
        if(!m_loop->inLoopThread())
        {
            m_loop->runTask(std::tr1::bind(&Timer::startInLoop, this));
        }   
        else
        {
            startInLoop();    
        }
    }
    
    void Timer::stop()
    {
        if(!m_loop->inLoopThread())
        {
            m_loop->runTask(std::tr1::bind(&Timer::stopInLoop, this));    
        }
        else
        {
            stopInLoop();    
        }
    }

    void Timer::reset(int milliseconds)
    {
        if(!m_loop->inLoopThread())
        {
            m_loop->runTask(std::tr1::bind(&Timer::resetInLoop, this, milliseconds));    
        }
        else
        {
            resetInLoop(milliseconds);    
        }
    }

    void Timer::startInLoop()
    {
        m_timer.data = this;
        ev_timer_start(m_loop->evloop(), &m_timer);    
    }

    void Timer::stopInLoop()
    {
        ev_timer_stop(m_loop->evloop(), &m_timer);    
    }

    void Timer::resetInLoop(int repeat)
    {
        m_timer.repeat = repeat / 1000.0;
        
        ev_timer_again(m_loop->evloop(), &m_timer); 
            
    }

    void Timer::onTimer(struct ev_loop* loop, struct ev_timer* w, int revents)
    {
        Timer* timer = (Timer*)w->data;
        
        (timer->m_func)(timer->shared_from_this());    
    }
}
