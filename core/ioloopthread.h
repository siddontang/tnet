#ifndef _IOLOOPTHREAD_H_
#define _IOLOOPTHREAD_H_

#include <string>

#include "nocopyable.h"

namespace tnet
{
    class IOLoop;
    class Thread;

    class IOLoopThread : public nocopyable
    {
    public:
        IOLoopThread(const std::string& threadName = "");
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
