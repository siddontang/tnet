#include "threadutil.h"

#include <time.h>

#include <unistd.h>
#include <sys/syscall.h>

#include "log.h"

using namespace std;

namespace tnet
{
    Thread::Thread(const ThreadFunc_t& func, const string& name)
        : m_thread(0)
        , m_func(func)
        , m_name(name)
    {
    }

    Thread::~Thread()
    {
        
    }

    class ThreadRoutine
    {
    public:    
        static void* handle(void* arg)
        {
            ThreadRoutine* routine = static_cast<ThreadRoutine*>(arg);
            if(!routine->name.empty())
            {    
#ifdef LINUX
                pthread_setname_np(pthread_self(), routine->name.c_str());
#else
                pthread_setname_np(routine->name.c_str());
#endif
            }
 

            (routine->func)();
            
            delete routine;
            
            return NULL;    
        }
    
        ThreadFunc_t func;
        string name;
    };

    void Thread::start()
    {
        ThreadRoutine* routine = new ThreadRoutine;
        routine->func = m_func;
        routine->name = m_name;
        pthread_create(&m_thread, NULL, &ThreadRoutine::handle, routine);       
   }

    void Thread::stop()
    {
        pthread_join(m_thread, NULL);    
    }

    void Condition::wait(int timeout)
    {
        if(timeout <= 0)
        {
            return wait();    
        }

        struct timespec ts;
#ifdef LINUX
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += timeout;
#else
        ts.tv_sec += time(NULL) + timeout;
#endif
        MutexGuard mg(m_mutex);
        pthread_cond_timedwait(&m_cond, m_mutex.get(), &ts);
    }

    pid_t gettid()
    {
#ifdef LINUX
        return syscall(SYS_gettid);    
#else
        return syscall(SYS_thread_selfid);
#endif
    } 

    bool isMainThread()
    {
#ifdef LINUX
        return gettid() == getpid();
#else
        return pthread_main_np();

#endif    
    }       
}
