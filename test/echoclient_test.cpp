#include "tcpclient.h"
#include "address.h"
#include "log.h"

#include <tr1/memory>
#include <tr1/functional>
#include <stdio.h>

using namespace tnet;
using namespace std;
using namespace std::tr1::placeholders;

typedef std::tr1::shared_ptr<Connection> ConnectionPtr_t;

void onConnEvent(const ConnectionPtr_t& conn, Connection::Event event, const char* buf, int count)
{
    switch(event)
    {
        case Connection::ConnectingEvent:
            LOG_INFO("connecting");
            break;
        case Connection::ConnectEvent:
            LOG_INFO("connect");
            conn->send("Hello World");
            break;
        case Connection::ReadEvent:
            conn->send(buf, count);
            break;
        default:
            return;    
    }
}

int main()
{
    TcpClient c(4, 1000);

    Address addr("127.0.0.1", 11181);

    LOG_INFO("start client");
    c.start();

    for(int i = 0; i < 500; i++)
    {
        c.connect(addr, std::tr1::bind(&onConnEvent, _1, _2, _3, _4));
    }

    LOG_INFO("press any key to exit!");
    getchar();

    return 0;    
}
