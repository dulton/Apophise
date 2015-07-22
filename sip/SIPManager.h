/*************************************************************************
  > File Name: SIPManager.h
  > Author: cooperz
  > Mail: zbzcsn@qq.com
  > Created Time: Mon 13 Jul 2015 09:48:41 AM CST
 ************************************************************************/

#ifndef _SIPMANAGER_H
#define _SIPMANAGER_H

#include <iostream>
#include <string.h>
#include <map>
#include <stdint.h>
#include <osipparser2/sdp_message.h>
#include<eXosip2/eXosip.h>

#include "SIPUtil.h"


namespace svss
{
    namespace SIP
    {
        #ifdef GTEST

        #endif
        class SIPBuilder;
        class SIPParser;

        enum SIP_FAM_STATE
        {
            SIP_REGISTER_WAIT_401,
            SIP_REGISTER_WAIT_200,
            SIP_INVITE_WAIT_ACK,
        };
        struct TidState
        {
            uint32_t tid;
            uint32_t rid;
            SIP_FAM_STATE fam_state;
        };
        struct ReAuthInfo
        {
            std::string uas_ip;
            std::string uas_port_str;
            std::string passwd;
        };
        struct DialogInfo
        {
            /*dailog_id = call_id_num + from_tag_num*/
            std::string dailog_id;
            std::string call_id_num;
            std::string from_tag_num;
        };

        class SIPManager
        {
            public:
                explicit SIPManager( std::string dev_name,
                        std::string local_ip,
                        std::string local_port,
                        std::string _local_passwd_
                        );
                bool SIPManagerInit();
                void Register( SIP_IN uint32_t tid, 
                        SIP_OUT char** meg,
                        SIP_OUT size_t *len, 
                        SIP_OUT int* state,
                        SIP_IN std::string uas_ip = UAS_IP,
                        SIP_IN std::string uas_listen_port_str = UAS_LISTEN_PORT_STR,
                        SIP_IN std::string passwd = LOCAL_DEV_PASSWD_STR
                        );
                void InviteLivePlay( SIP_IN uint32_t tid,
                        SIP_OUT char** meg, 
                        SIP_OUT size_t *len, 
                        SIP_OUT int* state,
                        SIP_IN std::string remote_dev_name = REMOTE_DEV_NAME,
                        SIP_IN std::string uas_ip = UAS_IP,
                        SIP_IN std::string uas_listen_port_str = UAS_LISTEN_PORT_STR,
                        SIP_IN std::string sender_vedio_serial_num = SENDER_VEDIO_SERIAL_NUM_STR,
                        SIP_IN std::string recver_vedio_serial_num = RECVER_VEDIO_SERIAL_NUM_STR
                        );
                void DealSIPMeg( SIP_IN char* meg, 
                        SIP_IN size_t len,
                        SIP_OUT char** rtmeg, 
                        SIP_OUT size_t* rtlen,
                        SIP_OUT int* state,
                        SIP_OUT uint32_t* rttid);
                bool CleanTid( uint32_t tid);
                void DestoryMsg( ::osip_message_t* msg);
            private:
                SIPBuilder* _sip_builder_;
                SIPParser* _sip_parser_;                
                int _registerid_;
                std::map< int , struct ReAuthInfo> _rid_usinfo_;
                std::map< std::string, struct TidState > _affairs_tid_;
                std::map< std::string, int> _cid_rid_;
                std::map< std::string, struct DialogInfo> _did_dialog_info_;
                std::string _local_dev_name_;
                std::string _local_ip_str_;
                std::string _local_listen_port_str_;
                std::string _local_passwd_;
        };
    }
}

#endif
