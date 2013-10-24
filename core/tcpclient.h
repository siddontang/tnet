#ifndef _TCPCLIENT_H_
#define _TCPCLIENT_H_

#include <tr1/functional>
#include <tr1/memory>

namespace tnet
{
    class IOLoop;
    class Address;
    class Connection;

    class TcpClient
    {
    public:
        typedef std::tr1::shared_ptr<Connection> ConnectionPtr_t;

        TcpClient(IOLoop* loop);        
    
        void connect(const Address& addr);

    private:
        
    };
    
}

#endif
