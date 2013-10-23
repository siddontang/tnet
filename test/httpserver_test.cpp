#include <stdio.h>
#include <string>

#include <signal.h>

#include "log.h"

#include "address.h"
#include "tcpserver.h"

#include "connection.h"

#include "httpserver.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "misc.h"

using namespace std;
using namespace tnet;
using namespace std::tr1::placeholders;

void sigAction(TcpServer* pServer, int signum)
{
    printf("signum %d\n", signum);

    pServer->stop();
}

typedef std::tr1::shared_ptr<Connection> ConnectionPtr_t;

void onHandler(const HttpRequest& request, const ConnectionPtr_t& conn)
{
    HttpResponse resp;
    resp.statusCode = 200;
    resp.setContentType("text/html");
    resp.setKeepAlive(true);
    resp.enableDate();
    
    resp.body.resize(1600);

    conn->send(resp.dump());
}

int main()
{
    //Log::rootLog().setLevel(Log::ERROR);
     
    TcpServer s(2, 4, 10000);

    HttpServer httpd(&s);

    httpd.setRequestCallback(std::tr1::bind(&onHandler, _1, _2));

    httpd.listen(Address(11181));

    s.addSignal(SIGINT, std::tr1::bind(sigAction, &s, std::tr1::placeholders::_1));

    LOG_INFO("start tcp server");

    s.start();

    LOG_INFO("stop server");

    return 0;
} 


