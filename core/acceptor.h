#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_

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
    class IOLoop;
    class IOLoopThreadPool;
    class Address;
    class AcceptLoop;

    class Acceptor : public nocopyable
    {
    public:
        Acceptor(int maxLoopNum);    
        ~Acceptor();

        int listen(const Address& addr, const NewConnCallback_t& func);

        void start();
        void stop();

    private:
        IOLoopThreadPool* m_loopPool;

        std::vector<AcceptLoop*> m_loops;

        std::vector<int> m_sockFds;
    };
    
}

#endif

