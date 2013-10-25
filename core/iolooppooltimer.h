#ifndef _CONNPOOLTIMER_H_
#define _CONNPOOLTIMER_H_

#include <tr1/memory>
#include <tr1/functional>
#include <vector>

namespace tnet
{
    class IOLoop;
    class Timer;

    class IOLoopPoolTimer
    {
    public:
        typedef std::tr1::function<void (Timer*)> TimerFunc_t;
         
        //repeat and after are milliseconds
        IOLoopPoolTimer(const std::vector<IOLoop*>& loops, const TimerFunc_t& func, int repeat, int after);
        ~IOLoopPoolTimer();    

        void start();
        void stop();

        void reset(int repeat);

    private:
        const std::vector<IOLoop*>& m_loops;
        std::vector<Timer*> m_timers;
    };    
}

#endif
