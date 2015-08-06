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
            SIP_HEART_BEAT_WAIT_200,
            SIP_GET_CAMERAINFO_MESSAGE,
            SIP_BYE_WAIT_ACK
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
            std::string remote_dev_name;
        };
        struct DialogInfo
        {
            /*dailog_id = call_id_num + from_tag_num*/
            std::string dailog_id;
            std::string call_id_num;
            std::string from_tag_num;
            std::string to_tag_num;
        };

        class SIPManager
        {
            public:
                explicit SIPManager( std::string dev_name,
                        std::string local_ip,
                        std::string local_port,
                        std::string _local_passwd_
                        );
                bool Init();
                void Register( SIP_IN uint32_t tid, 
                        SIP_OUT char** meg,
                        SIP_OUT size_t *len, 
                        SIP_OUT int* state,
                        SIP_OUT int* contactid,
                        SIP_IN std::string remote_dev_name = REMOTE_DEV_NAME,
                        SIP_IN std::string uas_ip = UAS_IP,
                        SIP_IN std::string uas_listen_port_str = UAS_LISTEN_PORT_STR
                        );
                void InviteLivePlay( SIP_IN uint32_t tid,
                        SIP_IN int contactid,
                        SIP_OUT char** meg, 
                        SIP_OUT size_t *len, 
                        SIP_OUT int* state,
                        SIP_IN std::string recv_port,
                        SIP_IN std::string sender_vedio_serial_num = SENDER_VEDIO_SERIAL_NUM_STR,
                        SIP_IN std::string recver_vedio_serial_num = RECVER_VEDIO_SERIAL_NUM_STR
                        );
                void HeartBeat( SIP_IN int tid, SIP_IN int contactid, 
                        SIP_OUT char** rtmsg, 
                        SIP_OUT size_t* rtlen, 
                        SIP_OUT int* state);
                void GetCameraInfo( SIP_IN int tid, SIP_IN int contactid, 
                        SIP_OUT char** rtmsg, 
                        SIP_OUT size_t* rtlen, 
                        SIP_OUT int* state);
                /*存在被邀请的可能，所以这里可能产生新的会话+新的事物，但不需要
                 *预留tid，因为是单次事物，接收一次立马回复，没有后续，但是要记录
                 *其会话id，因为会接受到bye
                 *port 预留是因为 作为被邀请方，可能需要新开一个端口接收视频流
                 * */
                void DealSIPMeg( SIP_IN char* meg,
                        SIP_IN size_t len,
                        /*SIP_IN uint32_t tid, 不需要预留的tid*/
                        SIP_IN std::string &port,/*预留port*/
                        SIP_OUT char** rtmeg,
                        SIP_OUT size_t* rtlen,
                        SIP_OUT int* state,
                        SIP_OUT uint32_t* rttid);
                void Bye( SIP_IN uint32_t tid,
                        SIP_IN int contactid,
                        SIP_OUT char** remeg,
                        SIP_OUT size_t* len,
                        SIP_OUT int* state);
                void BeenInvited( SIP_IN osip_message_t* osip_msg,
                        SIP_IN std::string port,
                        SIP_OUT char** rtmsg,
                        SIP_OUT size_t* rtlen,
                        SIP_OUT int* state);
                bool CleanTid( uint32_t tid);
                void GetContentBody( SIP_IN char* msg, SIP_IN size_t len,
                        SIP_OUT std::string &camera_xml);
                bool IsPlayBackRequest( char* msg, size_t len, 
                        std::string &remote_ip,
                        std::string &remote_port,
                        std::string &playback_start_time,
                        std::string &playback_end_time);
                void DestoryMsg( ::osip_message_t* msg);
                void AddToTag( osip_message_t* msg);
                virtual ~SIPManager();
            public:
                SIPParser* _sip_parser_;
            private:
                SIPManager( const SIPManager& manager);
                SIPManager& operator=(const SIPManager&);
            private:
                SIPBuilder* _sip_builder_;
                int _registerid_;
                /*contact id, 注册时的对端信息以及名称密码*/
                std::map< int , struct ReAuthInfo> _rid_usinfo_;
                /*branch 代表一个事物，一次会话可能有多个事物*/
                std::map< std::string, struct TidState > _affairs_tid_;
                /*callid的数字部分，由此找到contact id，为了区别这里使用rid*/
                std::map< std::string, int> _cid_rid_;
                /*会话id，由fromtag+callid确定，invite register都会产生*/
                std::map< std::string, struct DialogInfo> _did_dialog_info_;
                /*invite bye的时候使用*/
                std::map< uint32_t, std::string> _tid_did_;
                std::string _local_dev_name_;
                std::string _local_ip_str_;
                std::string _local_listen_port_str_;
                std::string _local_passwd_;
        };
    }
}

#endif
