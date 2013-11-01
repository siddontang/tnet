#include "tcpserver.h"
#include "address.h"
#include "connection.h"
#include "log.h"

#include <tr1/memory>
#include <tr1/functional>

using namespace tnet;
using namespace std;
using namespace std::tr1::placeholders;

void onConnEvent(const ConnectionPtr_t& conn, ConnEvent event, const char* buf, int count)
{
    switch(event)
    {
        case Conn_ReadEvent:
            conn->send(buf, count);
            return;
        default:
            return;    
    }    
}

int main()
{
    TcpServer s(1, 1, 1000);
    
    s.listen(Address("127.0.0.1", 11181), std::tr1::bind(&onConnEvent, _1, _2, _3, _4));    

    s.start();
}
