#ifndef _IOLOOP_H_
#define _IOLOOP_H_ 

#include <tr1/functional>
#include <vector>
#include <map>

extern "C" 
{
#include <ev.h>
}

#include "threadutil.h"
#include "coredefs.h"

namespace tnet
{

    class IOLoop
    {
    public:
        IOLoop();
        ~IOLoop();
           
        void start();
        void stop();
   
        void addTask(const Callback_t& func);

        void runTask(const Callback_t& func);

        bool isMainLoop() { return m_mainLoop; }

        struct ev_loop* evloop() { return m_loop; }
        void setIOInterval(int milliseconds) { ev_set_io_collect_interval(m_loop, double(milliseconds) / 1000); }
        void setTimerInterval(int milliseconds) { ev_set_timeout_collect_interval(m_loop, double(milliseconds) / 1000); }

        bool inLoopThread();

    private:
        void wakeUp();
        void handleWaker();
        static void onWaked(struct ev_loop*, struct ev_async*, int);
       
        void check();
        static void onChecked(struct ev_loop*, struct ev_check*, int); 
        
        void runTasks();

    private:
        struct ev_loop* m_loop;
        struct ev_async m_asyncWatcher; 
        struct ev_check m_checkWatcher;

        bool m_stop;
        bool m_runTasks;
       
        pthread_t m_threadId;
        
        std::vector<Callback_t> m_tasks;
            
        SpinLock m_taskLock;

        bool m_mainLoop;
    };
    
}

#endif
