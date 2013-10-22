#include "threadutil.h"

#include <time.h>

#include <unistd.h>
#include <sys/syscall.h>

namespace tnet
{
    Thread::Thread(const ThreadFunc_t& func)
        : m_thread(0)
        , m_func(func)
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
            
            (routine->func)();
            
            delete routine;
            
            return NULL;    
        }
    
        typedef std::tr1::function<void ()> Func_t;

        Func_t func;
    };

    void Thread::start()
    {
        ThreadRoutine* routine = new ThreadRoutine;
        routine->func = m_func;
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
