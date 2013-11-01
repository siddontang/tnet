#include "tcpclient.h"
#include "address.h"
#include "log.h"
#include "ioloop.h"

#include <tr1/memory>
#include <tr1/functional>
#include <stdio.h>
#include <string>

#include "connection.h"

using namespace tnet;
using namespace std;
using namespace std::tr1::placeholders;

void onNumConnEvent(IntPtr_t num, const ConnectionPtr_t& conn, ConnEvent event, const char* buf, int count)
{
    switch(event)
    {
        case Conn_ReadEvent:
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

void onConnEvent(const ConnectionPtr_t& conn, ConnEvent event, const char* buf, int count)
{
    switch(event)
    {
        case Conn_ConnectingEvent:
            LOG_INFO("connecting");
            break;
        case Conn_ConnectEvent:
            LOG_INFO("connect");
            conn->setEventCallback(std::tr1::bind(&onNumConnEvent, IntPtr_t(new int(0)), _1, _2, _3, _4));
            conn->send("Hello World");
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
