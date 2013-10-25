#include "stringutil.h"

using namespace std;

namespace tnet
{

    vector<string> StringUtil::split(const string& src, const string& delim)
    {
        size_t lastPos = 0;
        size_t pos = 0;
        size_t size = src.size();

        vector<string> tokens;

        while(pos < size)
        {
            pos = lastPos;
            while(pos < size && delim.find_first_of(src[pos]) == string::npos)
            {
                ++pos;    
            }    

            if(pos - lastPos > 0)
            {
                tokens.push_back(src.substr(lastPos, pos - lastPos));    
            }

            lastPos = pos + 1;
        }

        return tokens;
    }
    
}
