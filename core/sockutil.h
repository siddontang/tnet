#ifndef _SOCKUTIL_H_
#define _SOCKUTIL_H_

namespace tnet
{
    class Address;
    class SockUtil
    {
    public:
        static int create();
        static int bindAndListen(const Address& addr);
        static int connect(int sockFd, const Address& addr);

        static int setNoDelay(int sockFd, bool on);
        static int setCloseOnExec(int sockFd, bool on);
        static int setNonBlocking(int sockFd, bool on);
        static int setNonBlockingAndCloseOnExec(int sockFd);
        static int createDummyFile();

        static int setReuseable(int sockFd, bool on);
        static int setKeepAlive(int sockFd, bool on);

        static int getLocalAddr(int sockFd, Address& addr);
        static int getRemoteAddr(int sockFd, Address& addr);

        static int getSockError(int sockFd);
    };

}

#endif
