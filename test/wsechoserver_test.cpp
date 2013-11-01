#include "httpserver.h"
#include "tcpserver.h"

#include "log.h"
#include "address.h"
#include "connection.h"
#include "wsconnection.h"

#include "wsevent.h"

using namespace std;
using namespace tnet;
using namespace std::tr1::placeholders;
using namespace std::tr1;

void onWsCallback(const WsConnectionPtr_t& conn, WsEvent event, const string& message)
{
    switch(event)
    {
        case Ws_OpenEvent:
            LOG_INFO("websocket open");
            break;    
        case Ws_CloseEvent:
            LOG_INFO("websocket close");
            break;
        case Ws_MessageEvent:
            LOG_INFO("websocket message %s", message.c_str());
            conn->send("hello " + message);
            break;
        case Ws_PongEvent:
            LOG_INFO("websocket pong");
            break;
        case Ws_ErrorEvent:
            LOG_INFO("websocket error");
            break;
    }
}

void sigHandler(TcpServer* p, int sig)
{
    p->stop();    
}

int main()
{
    TcpServer s(1, 1, 100);

    s.addSignal(SIGINT, std::tr1::bind(&sigHandler, &s, _1));
    
    HttpServer httpd(&s);
    
    httpd.setWsCallback("/", std::tr1::bind(&onWsCallback, _1, _2, _3));    

    httpd.listen(Address(11181));

    s.start();
    
    return 0; 
}
