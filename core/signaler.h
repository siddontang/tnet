#ifndef _SIGNALER_H_
#define _SIGNALER_H_

#include <tr1/functional>
#include <vector>

extern "C"
{
#include <ev.h>    
}

#include "nocopyable.h"
#include "coredefs.h"

namespace tnet
{
    //wrapper for ev signal
    
    class IOLoop;

    class Signaler : public nocopyable
    {
    public:
        Signaler(IOLoop* loop);
        ~Signaler();   
        
        void add(int signum, const SignalCallback_t& func);
        void remove(int signum);

        void clear();

    private:
        void addInLoop(int signum, const SignalCallback_t& func);
        void removeInLoop(int signum);
        void clearInLoop();

        static void onSignal(struct ev_loop*, struct ev_signal*, int);

    private:
        IOLoop* m_loop;
        
        class Watcher
        {
        public:
            struct ev_signal signal;
            SignalCallback_t     func;    
        }; 

        std::vector<Watcher*> m_watchers;
    };
}

#endif
