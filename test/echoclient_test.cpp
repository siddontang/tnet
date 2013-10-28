#include "tcpclient.h"
#include "address.h"
#include "log.h"
#include "ioloop.h"

#include <tr1/memory>
#include <tr1/functional>
#include <stdio.h>
#include <string>

using namespace tnet;
using namespace std;
using namespace std::tr1::placeholders;

typedef std::tr1::shared_ptr<Connection> ConnectionPtr_t;

void onConnEvent(const ConnectionPtr_t& conn, Connection::Event event, const char* buf, int count)
{
    std::tr1::shared_ptr<int> num = std::tr1::static_pointer_cast<int>(conn->getContext());
    if(!num)
    {
        num = std::tr1::shared_ptr<int>(new int(0));
        conn->setContext(num);    
    }

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
            {
                if((*num) > 10)
                {
                    LOG_INFO("shutdown");
                    conn->shutDown();
                    return;    
                }

                (*num)++;

            conn->send(buf, count);
            }
            break;
        default:
            return;    
    }
}

int main()
{
    TcpClient c(1, 1000);

    Address addr("127.0.0.1", 11181);

    LOG_INFO("start client");
    c.start();

    for(int i = 0; i < 100; i++)
    {
        c.connect(addr, std::tr1::bind(&onConnEvent, _1, _2, _3, _4));
    }

    IOLoop* mainLoop = new IOLoop();
    mainLoop->start();

    return 0;    
}
