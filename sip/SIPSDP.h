/*************************************************************************
	> File Name: SIPSDP.h
	> Author: cooperz
	> Mail: zbzcsn@qq.com
	> Created Time: Fri 24 Jul 2015 09:19:17 AM CST
 ************************************************************************/

#ifndef _SIPSDP_H
#define _SIPSDP_H
#include <iostream>

namespace svss
{
    namespace SIP
    {
        class SDP
        {
            public:
                explicit SDP( std::string local_dev_name,
                        std::string uac_ip);
                virtual ~SDP();
                std::string toString( std::string s_type, 
                        std::string recver_vedio_serial_num,
                        std::string recv_port);
            private:
                std::string _local_dev_name_;
                std::string _uac_ip_;                
        };
    }
}

#endif
