#include "iolooppooltimer.h"

#include <assert.h>

#include "timer.h"
#include "misc.h"

using namespace std;
using namespace std::tr1::placeholders;

namespace tnet
{
    IOLoopPoolTimer::IOLoopPoolTimer(const vector<IOLoop*>& loops, const TimerCallback_t& func, int repeat, int after)
        : m_loops(loops)
    {
        m_timers.resize(loops.size());
        for(size_t i = 0; i < loops.size(); ++i)
        {
            m_timers[i] = TimerPtr_t(new Timer(m_loops[i], func, repeat, after));
        } 
    }

    IOLoopPoolTimer::~IOLoopPoolTimer()
    {
    }

    void IOLoopPoolTimer::start()
    {
        for_each_all(m_timers, std::tr1::bind(&Timer::start, _1)); 
    }

    void IOLoopPoolTimer::stop()
    {
        for_each_all(m_timers, std::tr1::bind(&Timer::stop, _1)); 
    }

    void IOLoopPoolTimer::reset(int repeat)
    {
        assert(repeat > 0);
        for_each_all(m_timers, std::tr1::bind(&Timer::reset, _1, repeat)); 
    }
}
