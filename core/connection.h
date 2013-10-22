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

namespace tnet
{
    class IOLoop;

    class Connection : public nocopyable
                     , public std::tr1::enable_shared_from_this<Connection> 
    {
    public:
        enum Event
        {
            EstablishedEvent,
            ReadEvent,
            WriteCompleteEvent,
            ErrorEvent,
            CloseEvent,
        };
        
        enum Status
        {
            Connecting,
            Connected,
            Disconnecting,
            Disconnected,
        };

        Connection(IOLoop* loop, int sockFd);
        ~Connection();    

        void onEstablished();
        void shutDown();

        typedef std::tr1::shared_ptr<Connection> ConnectionPtr_t;

        typedef std::tr1::function<void (const ConnectionPtr_t&, Connection::Event, const char*, int)> ConnectionFunc_t;
        void setCallback(const ConnectionFunc_t& func) { m_func = func; }

        void send(const char* data, int dataLen);
        void send(const std::string& data);

        Status getStatus() { return m_status; }

        int getSockFd() { return m_io.fd; }

        IOLoop* getLoop() { return m_loop; }
  
        ev_tstamp getLastUpdate() { return m_lastUpdate; }

        typedef std::tr1::shared_ptr<void> UserData_t; 
        void setUserData(const UserData_t& data) { m_userData = data; }
        UserData_t getUserData() { return m_userData; } 
        void resetUserData() { m_userData.reset(); }

    private:
        static void onData(struct ev_loop*, struct ev_io*, int);

        void handleRead();
        void handleError();
        void handleWrite();
        void handleClose();

        void sendInLoop(const std::string& data);

        void resetIOEvent(int events);

        void updateTime();

    private:
        IOLoop* m_loop;

        struct ev_io m_io;

        Status m_status;
        
        ConnectionFunc_t m_func;

        std::string m_sendBuffer;

        std::tr1::shared_ptr<void>  m_userData;
    
        ev_tstamp m_lastUpdate;
    };    
}

#endif
