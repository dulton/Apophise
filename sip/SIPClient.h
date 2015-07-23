/*************************************************************************
  > File Name: SIPClient.h
  > Author: cooperz
  > Mail: zbzcsn@qq.com
  > Created Time: Thu 23 Jul 2015 02:37:18 PM CST
 ************************************************************************/

#ifndef _SIPCLIENT_H
#define _SIPCLIENT_H
#include <iostream>

#include "SIPUABase.h"
namespace svss
{
    namespace SIP
    {
        class SIPClient : public SIPUABase
        {
            public:
                explicit SIPClient( std::string local_dev_name,
                        std::string local_ip,
                        std::string local_port,
                        std::string passwd
                        );
                virtual ~SIPClient();
                int FSMDrive(char* msg, size_t len, 
                        char** rtmsg, size_t* rtlen);
                int ClientInit();
                int RegisterClient( char** rtmsg, size_t *len,
                        std::string remote_ip, 
                        std::string remote_port);
        };
    }
}

#endif
