#ifndef _TIMER_H_
#define _TIMER_H_

#include <tr1/functional>
#include <tr1/memory>

extern "C"
{
#include <ev.h>    
}

#include "nocopyable.h"

namespace tnet
{
    class IOLoop;

    class Timer : public nocopyable
    {
    public:
        typedef std::tr1::function<void (Timer*)> TimerFunc_t;

        //repeat and after are milliseconds
        Timer(IOLoop* loop, const TimerFunc_t& func, int repeat, int after = 0);
        ~Timer();
        
        void start();
        void stop();

        void reset(int repeat);

        void setContext(const std::tr1::shared_ptr<void>& context) { m_context = context; }
        std::tr1::shared_ptr<void> getContext() { return m_context; }
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
        TimerFunc_t m_func;

        std::tr1::shared_ptr<void> m_context;
    };
    
}

#endif
