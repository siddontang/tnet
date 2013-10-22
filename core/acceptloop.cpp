#include "acceptloop.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <stdio.h>

#include "ioloop.h"
#include "address.h"
#include "misc.h"
#include "sockutil.h"
#include "log.h"

using namespace std;

namespace tnet
{

    AcceptLoop::AcceptLoop(IOLoop* loop)
        : m_loop(loop)
    {
        m_dummyFd = SockUtil::createDummyFile();
    }
    
    AcceptLoop::~AcceptLoop()
    {
        clearContainer(m_watchers);
        close(m_dummyFd);
    }

    void AcceptLoop::start()
    {
     
    }

    void AcceptLoop::stop()
    {
        IOLoop::Callback_t func = std::tr1::bind(&AcceptLoop::stopInLoop, this);
        m_loop->runTask(func);
    }

    void AcceptLoop::stopInLoop()
    {
        for(size_t i = 0; i < m_watchers.size(); i++)
        {
            ev_io_stop(m_loop->evloop(), &(m_watchers[i]->io));    
        }
    }

    void AcceptLoop::listen(int sockFd, const NewConnectionFunc_t& func)
    {
        IOLoop::Callback_t taskFunc = std::tr1::bind(&AcceptLoop::listenInLoop, this, sockFd, func);
        m_loop->runTask(taskFunc);    
    } 

    void AcceptLoop::listenInLoop(int sockFd, const NewConnectionFunc_t& func)
    {
        Watcher* watcher = NULL;
        for(size_t i = 0; i < m_watchers.size(); i++)
        {
            if(m_watchers[i] && m_watchers[i]->io.fd == sockFd)
            {
                watcher = m_watchers[i];
                break;    
            }    
        }

        if(!watcher)
        {
            watcher = new Watcher;
            ev_io_init(&(watcher->io), &AcceptLoop::onAccept, sockFd, EV_READ);     
            
            m_watchers.push_back(watcher);
        }

        watcher->io.data = this;
        watcher->func = func;
        ev_io_start(m_loop->evloop(), &(watcher->io));
    }

    void AcceptLoop::onAccept(struct ev_loop* loop, struct ev_io* w, int revents)
    {
        if(!(revents & EV_READ))
        {
            LOG_ERROR("onAccept ev error");
            //some error may occur, maybe call ev_io_stop ?
            ev_io_stop(loop, w);
            return;    
        }

        Watcher* watcher = (Watcher*)w;

        AcceptLoop* acceptLoop = (AcceptLoop*)w->data; 

        int sockFd = accept(w->fd, NULL, NULL);
        if(sockFd < 0)
        {
            int err = errno;
            if(err == EMFILE || err == ENFILE)
            {
                //we may do later   
                close(acceptLoop->m_dummyFd);
                sockFd = accept(w->fd, NULL, NULL);
                if(sockFd > 0)
                    close(sockFd);
                acceptLoop->m_dummyFd = SockUtil::createDummyFile();
            }
            return;
        }    

        SockUtil::setNonBlockingAndCloseOnExec(sockFd);
        SockUtil::setKeepAlive(sockFd, true);
        
        (watcher->func)(sockFd);
    }
}
