#ifndef _COREDEFS_H_
#define _COREDEFS_H_

#include <tr1/memory>
#include <tr1/functional>

#include "connevent.h"

namespace tnet
{
    class Connection;
    class Timer;

    typedef std::tr1::shared_ptr<Connection> ConnectionPtr_t;
    typedef std::tr1::weak_ptr<Connection> WeakConnectionPtr_t;

    typedef std::tr1::function<void (const ConnectionPtr_t&, ConnEvent, const char*, size_t)> ConnEventCallback_t;
    typedef std::tr1::function<void (const ConnectionPtr_t&)> ConnReleaseFunc_t;
    
    typedef std::tr1::function<void (int)> NewConnCallback_t;
    
    typedef std::tr1::shared_ptr<Timer> TimerPtr_t;
    typedef std::tr1::function<void (const TimerPtr_t&)> TimerCallback_t;

    typedef std::tr1::function<void ()> ThreadFunc_t;
    typedef std::tr1::function<void (int)> SignalCallback_t;

    typedef std::tr1::shared_ptr<void> ContextPtr_t;
    typedef std::tr1::weak_ptr<void> WeakContextPtr_t;
    typedef std::tr1::shared_ptr<int> IntPtr_t;

    typedef std::tr1::function<void ()> Callback_t;
}

#endif
