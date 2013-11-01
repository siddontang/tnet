#ifndef _TIMER_H_
#define _TIMER_H_

#include <tr1/functional>
#include <tr1/memory>

extern "C"
{
#include <ev.h>    
}

#include "nocopyable.h"
#include "coredefs.h"

namespace tnet
{
    class IOLoop;

    class Timer : public nocopyable
                , public std::tr1::enable_shared_from_this<Timer> 
    {
    public:
        //repeat and after are milliseconds
        Timer(IOLoop* loop, const TimerCallback_t& func, int repeat, int after = 0);
        ~Timer();
        
        void start();
        void stop();

        void reset(int repeat);

        void setContext(const ContextPtr_t& context) { m_context = context; }
        ContextPtr_t getContext() { return m_context; }
        void resetContext() { m_context.reset(); }

        IOLoop* getLoop() { return m_loop; }

    private:
        void startInLoop();
        void stopInLoop();
        void resetInLoop(int);

        static void onTimer(struct ev_loop*, struct ev_timer*, int);

    private:
        IOLoop* m_loop;
        struct ev_timer m_timer;
        TimerCallback_t m_func;

        ContextPtr_t m_context;
    };
    
}

#endif
