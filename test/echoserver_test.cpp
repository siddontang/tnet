#include "tcpserver.h"
#include "address.h"
#include "connection.h"
#include "log.h"

#include <tr1/memory>
#include <tr1/functional>

using namespace tnet;
using namespace std;
using namespace std::tr1::placeholders;

typedef std::tr1::shared_ptr<Connection> ConnectionPtr_t;

void onConnEvent(const ConnectionPtr_t& conn, Connection::Event event, const char* buf, int count)
{
    switch(event)
    {
        case Connection::ReadEvent:
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
