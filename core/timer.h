#ifndef _TIMER_H_
#define _TIMER_H_

#include <tr1/functional>
#include <tr1/memory>

extern "C"
{
#include <ev.h>    
}

#include "nocopyable.h"

namespace tpush
{
    class IOLoop;

    class Timer : public nocopyable
    {
    public:
        typedef std::tr1::function<void ()> TimerFunc_t;

        //repeat and after are milliseconds
        Timer(IOLoop* loop, const TimerFunc_t& func, int repeat, int after = 0);
        ~Timer();
        
        void start();
        void stop();

        void reset(int repeat);

        typedef std::tr1::shared_ptr<void> UserData_t;
        void setUserData(const UserData_t& udata) { m_userData = udata; }
        UserData_t getUserData() { return m_userData; }
        void resetUserData() { m_userData.reset(); }

    private:
        void startInLoop();
        void stopInLoop();
        void resetInLoop(int);

        static void onTimer(struct ev_loop*, struct ev_timer*, int);

    private:
        IOLoop* m_loop;
        struct ev_timer m_timer;
        TimerFunc_t m_func;

        UserData_t m_userData;
    };
    
}

#endif
