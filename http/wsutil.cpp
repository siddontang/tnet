#include "wsutil.h"

#include "wsconnection.h"

using namespace std;

namespace tnet
{
    void WsUtil::ping(const ConnectionPtr_t& conn, const string& message)
    {
        WsConnection::ping(conn, message);    
    }

    void WsUtil::send(const ConnectionPtr_t& conn, const string& message, bool binary)
    {
        WsConnection::send(conn, message, binary);    
    }

    void WsUtil::close(const ConnectionPtr_t& conn)
    {
        WsConnection::close(conn);    
    } 
}
