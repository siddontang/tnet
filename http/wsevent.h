#ifndef _WSEVENT_H_
#define _WSEVENT_H_

namespace tnet
{    
    enum WsEvent
    {
        Ws_OpenEvent,
        Ws_CloseEvent, 
        Ws_MessageEvent,
        Ws_PongEvent,    
        Ws_ErrorEvent,
    };    
}
#endif
