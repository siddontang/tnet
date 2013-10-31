#ifndef _CONNEVENT_H_
#define _CONNEVENT_H_

namespace tnet
{
    enum ConnEvent
    {
        Conn_ConnectEvent = 0,
        Conn_ConnectingEvent,
        Conn_ReadEvent,
        Conn_WriteCompleteEvent,
        Conn_ErrorEvent,
        Conn_CloseEvent,
    };
 
}

#endif
