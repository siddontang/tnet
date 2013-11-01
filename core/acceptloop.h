#ifndef _ACCEPTLOOP_H_
#define _ACCEPTLOOP_H_

#include <tr1/functional>
#include <vector>

extern "C"
{
#include <ev.h>    
}

#include "coredefs.h"

namespace tnet
{
    class IOLoop;
    class Address;

    class AcceptLoop
    {
    public:
        AcceptLoop(IOLoop* loop);
        ~AcceptLoop();

        void start();
        void stop();

        void listen(int sockFd, const NewConnCallback_t& func);
    
    private:
        void stopInLoop();
        void listenInLoop(int sockFd, const NewConnCallback_t& func);

        static void onAccept(struct ev_loop*, struct ev_io*, int);
     
    private:
        IOLoop*  m_loop;
        
        class Watcher
        {
        public:
            struct ev_io io;
            NewConnCallback_t func;    
        };
        
        std::vector<Watcher*> m_watchers;
    
        int m_dummyFd;
    };


}

#endif
