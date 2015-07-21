/*************************************************************************
	> File Name: SIPBuilder.h
	> Author: cooperz
	> Mail: zbzcsn@qq.com
	> Created Time: Mon 13 Jul 2015 09:48:18 AM CST
 ************************************************************************/

#ifndef _SIPBUILDER_H
#define _SIPBUILDER_H

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <osipparser2/osip_message.h>
#include <eXosip2/eXosip.h>
#include <eXosip2/eX_register.h> 

#include "SIPUtil.h"


namespace svss
{
    namespace SIP
    {
        class SIPBuilder
        {
            public:
                explicit SIPBuilder( std::string dev_name,
                        std::string local_ip,
                        std::string local_port
                        );
                void Register( char** meg, size_t* len, int* state,
                        std::string &callid,
                        std::string &from_tag,
                        std::string &via_branch,
                        std::string uas_ip = UAS_IP,
                        std::string uas_listen_port_str = UAS_LISTEN_PORT_STR
                        );
                void InviteLivePlay( char** meg, size_t* len, int* state,
                        std::string &call_id,
                        std::string remote_dev_name = REMOTE_DEV_NAME,
                        std::string uas_ip = UAS_IP,
                        std::string uas_listen_port_str = UAS_LISTEN_PORT_STR,
                        std::string sender_vedio_serial_num = SENDER_VEDIO_SERIAL_NUM_STR,
                        std::string recver_vedio_serial_num = RECVER_VEDIO_SERIAL_NUM_STR
                        );
                void InviteACK( osip_message_t* msg, char** rtmeg , size_t *rtlen,
                        int* state);
                void AuRegister( osip_message_t* msg, char** rtmeg, size_t * rtlen,
                        std::string uas_ip, std::string uas_listen_port_str,
                        std::string local_dev_passwd_str);
            private:
                std::string _RandomNum();
                std::string _RegisterMd5( std::string username, std::string realm, 
                        std::string passwd, std::string uri, std::string nonce);
            private:
                std::string _dev_name_;
                std::string _local_ip_str_;
                std::string _local_port_str_;
        };
    }
}
#endif
