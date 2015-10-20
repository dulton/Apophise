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

#include "SIPManager.h"
#include "SIPUtil.h"
#include "SIPSDP.h"


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
                        struct DialogInfo &dlg_info,
                        std::string &via_branch,
                        std::string uas_dev_name,
                        std::string uas_ip,
                        std::string uas_listen_port_str
                        );
                void InviteLivePlay( char** meg, size_t* len, int* state,
                        std::string dev_id_been_invited,
                        std::string recv_port,
                        struct DialogInfo &dlg_info,
                        std::string &via_branch,
                        std::string remote_dev_name = REMOTE_DEV_NAME,
                        std::string uas_ip = UAS_IP,
                        std::string uas_listen_port_str = UAS_LISTEN_PORT_STR,
                        std::string sender_vedio_serial_num = SENDER_VEDIO_SERIAL_NUM_STR,
                        std::string recver_vedio_serial_num = RECVER_VEDIO_SERIAL_NUM_STR
                        );
                void HeartBeat(char** rtmsg, size_t* rtlen, int* state,
                        std::string &via_branch,
                        std::string uas_dev_name,
                        std::string uas_ip,
                        std::string uas_listen_port_str);
                void GetCameraInfo(char** rtmsg, size_t* rtlen, int* state,
                        std::string &via_branch,
                        std::string uas_dev_name,
                        std::string uas_ip,
                        std::string uas_listen_port_str);
                void InviteACK( osip_message_t* msg, char** rtmeg , size_t *rtlen,
                        int* state);
                void AuRegister( osip_message_t* msg, char** rtmeg, size_t * rtlen,
                        std::string &via_branch_num,
                        struct DialogInfo dlg_info,
                        struct ReAuthInfo re_au
                        );
                void Bye( char** rtmeg, size_t *rtlen , int* state, 
                        std::string &via_branch_num, struct DialogInfo dlg_info, 
                        struct ReAuthInfo);
                void BeenInvited( osip_message_t* msg, std::string port,char** rtmsg,
                        size_t *rtlen, int*state, struct DialogInfo &dlg_info);
                void PlayEnd( char** rtmsg, size_t *rtlen, std::string call_id_num,
                        std::string call_host,
                        std::string to_tag_num,
                        std::string from_tag_numa,
                        std::string camera_dev_id,
                        std::string remote_dev_name,
                        std::string uas_ip,
                        std::string uas_port_str);
                void CameraInfoAck(osip_message_t* msg,
                        char** rtmsg, size_t* rtlen);
                virtual ~SIPBuilder();
            private:
                std::string _RandomNum();
                std::string _RegisterMd5( std::string username, std::string realm, 
                        std::string passwd, std::string uri, std::string nonce);
                
            private:
                std::string _dev_name_;
                std::string _local_ip_str_;
                std::string _local_port_str_;
                int _message_cseq_;
                SDP _sdp_builder_;
        };
    }
}
#endif
