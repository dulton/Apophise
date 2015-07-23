/*************************************************************************
	> File Name: SIPClient.cpp
	> Author: cooperz
	> Mail: zbzcsn@qq.com
	> Created Time: Thu 23 Jul 2015 02:37:41 PM CST
 ************************************************************************/
#include "SIPClient.h"

#include<iostream>
using namespace std;

namespace svss
{
    namespace SIP
    {
        SIPClient::SIPClient( string local_dev_name,
                string local_ip,
                string local_port,
                string passwd):SIPUABase( local_dev_name,
                    local_ip,
                    local_port,
                    passwd)
        {
            
        }

        int SIPClient::ClientInit()
        {
            return Init();
        }
        
        int SIPClient::FSMDrive(char* msg, size_t len,
                char** rtmsg, size_t* rtlen)
        {
            return 1;
        }
    }
}
