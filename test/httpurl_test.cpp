#include "httpurl.h"
#include <iostream>

using namespace std;
using namespace tnet;

void test1()
{
    HttpUrl url("http://localhost:11181/abc/de?a=1&b=2");
    
    cout << url.schema << endl;
    cout << url.host << endl;
    cout << url.port << endl;
    cout << url.path << endl;
    
    for(map<string, string>::iterator iter = url.params.begin(); iter != url.params.end(); ++iter)
    {
        cout << iter->first << "\t" << iter->second << endl;    
    }    
}

int main()
{
    test1();
}
