#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <tr1/functional>
#include <tr1/memory>
#include <string>

extern "C"
{
#include <ev.h>    
}

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

        typedef std::tr1::shared_ptr<Connection> ConnectionPtr_t;
        typedef std::tr1::function<void (const ConnectionPtr_t&, ConnEvent, const char*, size_t)> EventCallback_t;
        typedef std::tr1::function<void (const ConnectionPtr_t&)> ReleaseFunc_t;

        Connection(IOLoop* loop, int sockFd);
        ~Connection();    

        static void setReleaseFunc(const ReleaseFunc_t& func) { ms_releaseFunc = func; }

        void setEventCallback(const EventCallback_t& func) { m_func = func; }

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

        void setContext(const std::tr1::shared_ptr<void>& context) { m_context = context; }
        std::tr1::shared_ptr<void> getContext() { return m_context; }
        void resetContext() { m_context.reset(); }

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
        
        EventCallback_t m_func;
    
        std::string m_sendBuffer;
    
        ev_tstamp m_lastUpdate;
    
        std::tr1::shared_ptr<void> m_context;
    
        static ReleaseFunc_t ms_releaseFunc;
    };    
}

#endif
