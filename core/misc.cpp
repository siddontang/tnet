#include "misc.h"

#include <string.h>
#include <signal.h>

namespace tnet
{
#ifdef LINUX
    __thread char errorBuf[64];
#endif

    const char* errorMsg(int errorNum)
    {
#ifdef LINUX
        return strerror_r(errorNum, errorBuf, sizeof(errorBuf));
#else
        return strerror(errorNum);
#endif
    }

    class IgnoreSigPipe
    {
    public:
        IgnoreSigPipe()
        {
            signal(SIGPIPE, SIG_IGN);    
        
            sigset_t signal_mask;
            sigemptyset (&signal_mask);
            sigaddset (&signal_mask, SIGPIPE);
            pthread_sigmask(SIG_BLOCK, &signal_mask, NULL);
        }    
    };
    
    static IgnoreSigPipe ignoreSigPipe;
}
