/*************************************************************************
	> File Name: SIPUABase.h
	> Author: cooperz
	> Mail: zbzcsn@qq.com
	> Created Time: Thu 23 Jul 2015 02:36:25 PM CST
 ************************************************************************/

#ifndef _SIPUABASE_H
#define _SIPUABASE_H
#include <iostream>

#include "SIPManager.h"

namespace svss
{
    namespace SIP
    {
        class SIPUABase
        {
            public:
               explicit SIPUABase( std::string local_dev_name, 
                       std::string local_ip,
                       std::string local_port,
                       std::string passwd
                       );
               virtual ~SIPUABase();
               int Init();
               int Register( char** rtmsg, size_t* len,
                       std::string remote_ip, std::string remote_port);
               int Unregister();
               virtual uint32_t FSMDrive( uint32_t taks_id, char* msg, 
                       size_t len, std::string &port,
                       char** rtmsg, size_t* rtlen) = 0;
            protected:
               SIPManager _manager_;
            protected:
               int _contact_id_;
               int _ua_task_id_;
               int _status_code_;
        };
    }
}

#endif
