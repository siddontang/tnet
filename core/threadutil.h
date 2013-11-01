#ifndef _THREADUTIL_H_
#define _THREADUTIL_H_

#include <string>
#include <pthread.h>
#include <tr1/functional>

#include "nocopyable.h"
#include "coredefs.h"

namespace tnet
{
    class Thread : public nocopyable
    {
    public:
        Thread(const ThreadFunc_t& func, const std::string& name = "");
        ~Thread();
        
        void start();
        void stop();
    
    private: 
        pthread_t m_thread;   
        ThreadFunc_t m_func;
    
        std::string m_name;
    };

    class Mutex : public nocopyable
    {
    public:
        Mutex() 
        {
            pthread_mutex_init(&m_mutex, NULL);   
        }
        ~Mutex()
        {
            pthread_mutex_destroy(&m_mutex);    
        }
        
        pthread_mutex_t* get() {
            return &m_mutex;
        }

        void lock()
        {
            pthread_mutex_lock(&m_mutex);
        }

        void unlock()
        {
            pthread_mutex_unlock(&m_mutex);
        }
        
    private:
        pthread_mutex_t m_mutex;    
    };

    class MutexGuard : public nocopyable
    {
    public:
        MutexGuard(Mutex& mutex)
            : m_mutex(mutex)
        {
            m_mutex.lock();    
        }

        ~MutexGuard()
        {
            m_mutex.unlock();    
        }

    private:
        Mutex& m_mutex;
    };
   
   
    class Condition : public nocopyable
    {
    public:
        Condition(Mutex& mutex)
            : m_mutex(mutex)
        {
            pthread_cond_init(&m_cond, NULL);    
        }

        ~Condition()
        {
            pthread_cond_destroy(&m_cond);
        }

        void wait()
        {
            MutexGuard mg(m_mutex);
            pthread_cond_wait(&m_cond, m_mutex.get());    
        }

        void wait(int timeout);

        void notify()
        {
            pthread_cond_signal(&m_cond);    
        }

        void notifyAll()
        {
            pthread_cond_broadcast(&m_cond);    
        }

    private:
        Mutex& m_mutex;
        pthread_cond_t m_cond;
    };
    
    class SpinLock : public nocopyable
    {
    public:
        SpinLock() {
            m_lock = 0;    
        }
        ~SpinLock() {} 

        void lock()
        {
            while(__sync_lock_test_and_set(&m_lock, 1))
            {
            }    
        }

        void unlock()
        {
            __sync_lock_release(&m_lock);    
        }
        
    private:
        volatile int m_lock;   
    }; 

    class SpinLockGuard : public nocopyable
    {
    public:
        SpinLockGuard(SpinLock& lock) 
            : m_lock(lock)
        {
            m_lock.lock();   
        }

        ~SpinLockGuard()
        {
            m_lock.unlock();
        }
    private:
        SpinLock& m_lock;
    };

    pid_t gettid();

    bool isMainThread();
}

#endif
