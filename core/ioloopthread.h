#ifndef _IOLOOPTHREAD_H_
#define _IOLOOPTHREAD_H_

#include "nocopyable.h"

namespace tnet
{
    class IOLoop;
    class Thread;

    class IOLoopThread : public nocopyable
    {
    public:
        IOLoopThread();
        ~IOLoopThread();

        IOLoop* getLoop() { return m_ioloop; }

        void start();
        void stop();

    private:
        IOLoop* m_ioloop;
        Thread* m_thread;
    };    
}

#endif
