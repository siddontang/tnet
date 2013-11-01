#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <tr1/functional>
#include <tr1/memory>
#include <string>

extern "C"
{
#include <ev.h>    
}

#include "coredefs.h"
#include "nocopyable.h"
#include "connevent.h"

namespace tnet
{
    class IOLoop;
    class Address;

    class Connection : public nocopyable
                     , public std::tr1::enable_shared_from_this<Connection> 
    {
    public:
        enum Status
        {
            None,
            Connecting,
            Connected,
            Disconnecting,
            Disconnected,
        };

        Connection(IOLoop* loop, int sockFd, const ConnReleaseFunc_t& func);
        ~Connection();    

        void setEventCallback(const ConnEventCallback_t& func) { m_func = func; }
        const ConnEventCallback_t& getEventCalback() { return m_func; }

        void shutDown();
        
        void send(const char* data, int dataLen);
        void send(const std::string& data);

        Status getStatus() { return m_status; }

        bool isConnected() { return m_status == Connected; }
        bool isConnecting() { return m_status == Connecting; }
        bool isDisconnecting() { return m_status == Disconnecting; }
        bool isDisconnected() { return m_status == Disconnected; }

        int getSockFd() { return m_io.fd; } 

        void connect(const Address& addr);
        void onEstablished();

        IOLoop* getLoop() { return m_loop; }
        
        ev_tstamp getLastUpdate() { return m_lastUpdate; } 

    private:
        static void onData(struct ev_loop*, struct ev_io*, int);

        void handleRead();
        void handleError();
        void handleWrite();
        void handleClose();
        void handleConnect();

        void onEstablishedInLoop();
        void connectInLoop(const Address& addr);
        void sendInLoop(const std::string& data);

        void resetIOEvent(int events);

        void updateTime();

    private:
        IOLoop* m_loop;

        struct ev_io m_io;

        Status m_status;
         
        ev_tstamp m_lastUpdate;
    
        std::string m_sendBuffer;
    
        ConnEventCallback_t m_func;
        ConnReleaseFunc_t m_releaseFunc;
    };    
}

#endif
