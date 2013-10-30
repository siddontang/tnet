#include "sockutil.h"

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <assert.h>

#ifdef LINUX
#include <endian.h>
#else
#endif

#include "address.h"
#include "misc.h"
#include "log.h"

namespace tnet
{
    int SockUtil::create()
    {
#ifdef LINUX
        int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);    
#else
        int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(fd < 0)
        {
            return fd;    
        }

        SockUtil::setNonBlockingAndCloseOnExec(fd);
#endif
        return fd;
    }

    int SockUtil::bindAndListen(const Address& addr)
    {
        int err = 0;
        
        int fd = create();
        if(fd < 0)
        {
            err = errno;
            LOG_ERROR("create socket error %s", errorMsg(err));
            return fd;    
        }
            
        SockUtil::setReuseable(fd, true);
        
        do
        {
            struct sockaddr_in sockAddr = addr.sockAddr();
            
            if(bind(fd, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) < 0)
            {
                err = errno;
                LOG_ERROR("bind address %s:%d error: %s", addr.ipstr().c_str(), addr.port(), errorMsg(err));
                break;    
            }

            if(listen(fd, SOMAXCONN) < 0)
            {
                err = errno;
                LOG_ERROR("listen address %s:%d error: %s", addr.ipstr().c_str(), addr.port(), errorMsg(err));
                break;     
            }


            return fd;

        }while(0);

        close(fd);
        return err;
    }

    int SockUtil::connect(int sockFd, const Address& addr)
    {
        struct sockaddr_in sockAddr = addr.sockAddr();
    
        int ret = ::connect(sockFd, (struct sockaddr*)&sockAddr, sizeof(sockAddr));
        if(ret < 0)
        {
            int err = errno;
            return err;    
        }
        else
        {
            return ret;
        }
    }

    int SockUtil::setNoDelay(int sockFd, bool on)
    {
        int opt = on ? 1 : 0;
        
        return setsockopt(sockFd, IPPROTO_TCP, 
                        TCP_NODELAY, &opt, 
                        static_cast<socklen_t>(sizeof(opt)));    
    }
   
    static int setFdFlag(int fd, int flag, bool on)
    {
        int flags = fcntl(fd, F_GETFL, 0);
        if(flags < 0)
            return flags;

        flags = on ? (flags | flag) : (flags & ~flag);
        return fcntl(fd, F_SETFL, flags);
    }
    
    int SockUtil::setNonBlocking(int sockFd, bool on)
    {
        return setFdFlag(sockFd, O_NONBLOCK, on);
    }

    int SockUtil::setCloseOnExec(int sockFd, bool on)
    {
        return setFdFlag(sockFd, O_CLOEXEC, on);    
    }

    int SockUtil::setNonBlockingAndCloseOnExec(int sockFd)
    {
        return setFdFlag(sockFd, O_NONBLOCK | O_CLOEXEC, true);    
    }

    int SockUtil::createDummyFile()
    {
        return open("/dev/null", O_RDONLY | O_CLOEXEC);    
    }

    int SockUtil::setReuseable(int sockFd, bool on)
    {
        int opt = on ? 1 : 0;
        return setsockopt(sockFd, SOL_SOCKET, 
                        SO_REUSEADDR, &opt,
                        static_cast<socklen_t>(sizeof(opt)));    
    }

    int SockUtil::setKeepAlive(int sockFd, bool on)
    {
        int opt = on ? 1 : 0;
        return setsockopt(sockFd, SOL_SOCKET, 
                        SO_KEEPALIVE, &opt,
                        static_cast<socklen_t>(sizeof(opt)));    
    }

    int SockUtil::getLocalAddr(int sockFd, Address& addr)
    {
        struct sockaddr_in sockAddr;
        socklen_t sockLen = sizeof(sockAddr);
        if(getsockname(sockFd, (struct sockaddr*)&sockAddr, &sockLen) != 0)
        {
            int err = errno;
            return err;
        }

        addr = Address(sockAddr);
        return 0;
    }

    int SockUtil::getRemoteAddr(int sockFd, Address& addr)
    {
        struct sockaddr_in sockAddr;
        socklen_t sockLen = sizeof(sockAddr);
        if(getpeername(sockFd, (struct sockaddr*)&sockAddr, &sockLen) != 0)
        {
            int err = errno;
            return err;
        }

        addr = Address(sockAddr);
        return 0;
    }

    int SockUtil::getSockError(int sockFd)
    {
        int opt;
        socklen_t optLen = static_cast<socklen_t>(sizeof(opt));
        
        if(getsockopt(sockFd, SOL_SOCKET, SO_ERROR, &opt, &optLen) < 0)
        {
            int err = errno;
            return err;    
        }   
        else
        {
            return opt;    
        }
    }

    static inline uint64_t swap64(uint64_t x)
    {
        return ((((x) & 0xff00000000000000LL) >> 56) | 
                (((x) & 0x00ff000000000000LL) >> 40) | 
                (((x) & 0x0000ff0000000000LL) >> 24) | 
                (((x) & 0x000000ff00000000LL) >> 8) | 
                (((x) & 0x00000000ff000000LL) << 8) | 
                (((x) & 0x0000000000ff0000LL) << 24) | 
                (((x) & 0x000000000000ff00LL) << 40) | 
                (((x) & 0x00000000000000ffLL) << 56));
    }

    uint64_t SockUtil::ntohll(uint64_t net)
    {
#if __BYTE_ORDER==__LITTLE_ENDIAN
        return swap64(net);
#else
        return net;
#endif    
    }

    uint64_t SockUtil::htonll(uint64_t host)
    {
#if __BYTE_ORDER==__LITTLE_ENDIAN
        return swap64(host);
#else
        return host;
#endif
    }
}
