/*************************************************************************
  > File Name: SIPManager.cpp
  > Author: cooperz
  > Mail: zbzcsn@qq.com
  > Created Time: Mon 13 Jul 2015 09:48:56 AM CST
 ************************************************************************/
#include "SIPManager.h"

#include<iostream>
#include<sstream>
#include<netinet/in.h>
#include <eXosip2/eX_setup.h>
#include <eXosip2/eX_options.h>
#include <osipparser2/osip_parser.h>

#include "SIPBuilder.h"
#include "SIPParser.h"
#include "SIPUtil.h"

using namespace std;

namespace svss
{
    namespace SIP
    {
        SIPManager::SIPManager( string dev_name,
                string local_ip,
                string local_port,
                string local_passwd
                )
        {
            _local_dev_name_ = dev_name;
            _local_ip_str_ = local_ip;
            _local_listen_port_str_ = local_port;
            _local_passwd_ = local_passwd;
            _registerid_ = 0;
        }

        bool SIPManager::Init()
        {
            int result;
            _sip_builder_ = new SIPBuilder( _local_dev_name_,
                    _local_ip_str_,
                    _local_listen_port_str_
                    );
            _sip_parser_ = new SIPParser();
#ifdef DEBUG
            FILE* logfile = fopen( "logfile.txt", "w");
            osip_trace_initialize( (_trace_level)8, logfile );
#endif
            istringstream istr( _local_listen_port_str_);
            int port;
            istr>>port;
#ifdef DEBUG
            cout<<"strport to intport "<<port<<endl;
#endif
            /*
               if (OSIP_SUCCESS != ::eXosip_listen_addr( _ctx_ , IPPROTO_UDP, NULL, port,
               AF_INET, 0))
               {
               printf("eXosip_listen_addr failure.\n");
               return false;
               }
               if (OSIP_SUCCESS != ::eXosip_set_option( _ctx_,
               EXOSIP_OPT_SET_IPV4_FOR_GATEWAY,
               _local_ip_str_.c_str()))
               {
               return -1;
               }*/
            return true;
        }

        /*
         * tid:上层传入的任务id
         * meg:返回需要发送的内容
         * len:返回需呀发送内容长度
         * state:状态 -1 错误 0 任务继续 1任务完成
         * contactid:manager层产生的注册id，每向sip服务器注册一次，便加一
         * */
        void SIPManager::Register( SIP_IN uint32_t tid , 
                SIP_OUT char** meg,
                SIP_OUT size_t* len,
                SIP_OUT int* state,
                SIP_OUT int* contactid,
                SIP_IN string remote_dev_name,
                SIP_IN string uas_ip ,
                SIP_IN string uas_listen_port_str, 
                SIP_IN string passwd
                )
        {
            struct DialogInfo dlg_info;
            string call_id_num;
            string via_branch_num;
            _sip_builder_->Register( meg, len , state,
                    dlg_info, via_branch_num,
                    remote_dev_name, uas_ip, uas_listen_port_str);
            call_id_num = dlg_info.call_id_num;

            if( *state != -1)
            {
                struct TidState tid_state;
                tid_state.tid = tid;
                tid_state.rid = _registerid_;
                tid_state.fam_state = SIP_REGISTER_WAIT_401;
                cout<<"add branch:"<<via_branch_num<<endl;
                _affairs_tid_.insert( make_pair( via_branch_num, tid_state));    
                struct ReAuthInfo re_au;
                re_au.passwd = passwd;
                re_au.uas_ip = uas_ip;
                re_au.uas_port_str = uas_listen_port_str;
                re_au.remote_dev_name = remote_dev_name;
                _rid_usinfo_.insert( make_pair( _registerid_, re_au));
                _cid_rid_.insert( make_pair( call_id_num, _registerid_));
                *contactid = _registerid_;
                _registerid_++;
                _did_dialog_info_.insert( make_pair( dlg_info.dailog_id, dlg_info));
                *state = 0;
                return;
            }
            return;
        }
        /*
         * tid:上层派发的任务id
         * meg:需要发送的内容
         * len:需要发送的内容长度
         * state:状态码
         * recv_port:接收视频流的端口
         * */
        void SIPManager::InviteLivePlay( SIP_IN uint32_t tid,
                SIP_IN int contactid,
                SIP_OUT char** meg,
                SIP_OUT size_t *len,
                SIP_OUT int* state,
                SIP_IN string recv_port,
                SIP_IN string sender_vedio_serial_num,
                SIP_IN string recver_vedio_serial_num
                )
        {
            map< int, struct ReAuthInfo>::iterator ite_rid_reau;
            ite_rid_reau = _rid_usinfo_.find( contactid);
            if( ite_rid_reau == _rid_usinfo_.end())
            {
                *state=-1;
                return;
            }
            string remote_dev_name = ite_rid_reau->second.remote_dev_name;
            string uas_ip = ite_rid_reau->second.uas_ip;
            string uas_listen_port_str = ite_rid_reau->second.uas_port_str;
            string via_branch_num;
            struct DialogInfo dlg_info;
            _sip_builder_->InviteLivePlay( meg, len, state, recv_port,
                    dlg_info, via_branch_num, remote_dev_name, uas_ip, 
                    uas_listen_port_str, sender_vedio_serial_num, 
                    recver_vedio_serial_num);
            if( *state != -1)
            {
                struct TidState tid_state;
                tid_state.tid = tid;
                tid_state.fam_state = SIP_INVITE_WAIT_ACK;
                _affairs_tid_.insert( make_pair( via_branch_num, tid_state));
                _tid_did_.insert( make_pair( tid, dlg_info.dailog_id));
                _did_dialog_info_.insert( make_pair( dlg_info.dailog_id, dlg_info));
#ifdef DEBUG
                cout<<"invite dlg_id:"<<dlg_info.dailog_id<<endl;
#endif
            }
            return;
        }
        /*
         * meg:需要处理的SIP协议内容
         * len:SIP协议内容长度
         * port:预留端口
         * rtmeg:回复SIP协议内容
         * rtlen:回复SIP内容长度
         * state:状态码
         * rttid:如果该任务完成，返回改任务的上层的任务ID
         * ATTENTION:
         *    state为1的时候，表示人物已经完成了。但是，此时的rtlen不为0，
         *    意味着任然有数据需要发送出去！即表示，这次SIP消息回复出去，
         *    才是真正的完成。   
         * */
        void SIPManager::DealSIPMeg( SIP_IN char* meg, 
                SIP_IN size_t len,
                SIP_IN string &port,
                SIP_OUT char** rtmeg,
                SIP_OUT size_t* rtlen,
                SIP_OUT int* state,
                SIP_OUT uint32_t* rttid)
        {
            /*
             * 发出Register 会收到401
             * 发出auth + passwd，会收到 200OK 
             * 终止
             * 
             * 发出invite，会收到200 OK
             * 需要回复ACK 然后 终止
             *
             * 收到invite邀请，需要发出数据
             * 需要回复200OK 然后终止
             *
             * state == 0 : work continue
             * state == -1 : error
             * state == 1 : work over
             * */
            osip_message_t* osip_msg;
            osip_msg = _sip_parser_->parser(meg, len);
            string call_id = string(osip_msg->call_id->number);
            string via_branch_num = _sip_parser_->getBranchNum(osip_msg,_local_ip_str_);
                cout<<"get branch:"<<via_branch_num<<endl;
            if(via_branch_num.length()<=0)
            {
#ifdef DEBUG 
                cout<<"can not get branch num"<<endl;
#endif
                osip_message_free( osip_msg);
                *state = -1;
                return;
            }
            map<string, struct TidState>::iterator ite_cid_tid;
            ite_cid_tid = _affairs_tid_.find(via_branch_num);
            if( ite_cid_tid == _affairs_tid_.end())
            {//todo:no such call id
                //todo free the osipmeg
                /* may be a new affairs*/
                if(MSG_IS_INVITE( osip_msg))
                {
                    BeenInvited( osip_msg, port, rtmeg, rtlen, state);
                    *state = 0;
                    return;
                }
                if(MSG_IS_ACK( osip_msg))
                {
                    *state=1;
                    return;
                }
#ifdef DEBUG 
                cout<<"no such branch num"<<endl;
#endif
                osip_message_free( osip_msg);
                *state = -1;
                return;
            }
            *rttid = (*ite_cid_tid).second.tid;

            if( (osip_msg)->status_code == 100)
            {
                /* 100 tring , invite continue*/
                *state = 0;
                return;
            }
            else if( (osip_msg)->status_code == 401)
            {
                map< string, int>::iterator ite;
                ite = _cid_rid_.find( call_id);
                if( ite == _cid_rid_.end())
                {
                    /*todo: in register record no such call id*/
                    *state = -1;
                    return;
                }
                int rid = ite->second;
                map< int, struct ReAuthInfo>::iterator ite_usinfo;
                ite_usinfo = _rid_usinfo_.find( rid);
                if( ite == _cid_rid_.end())
                {
                    *state = -1;
                    return;
                }
                ite_usinfo = _rid_usinfo_.find( rid);
                map< string, struct DialogInfo>::iterator ite_dlginfo;
                string dlgid = _sip_parser_->getDialogId( osip_msg);
                ite_dlginfo = _did_dialog_info_.find( dlgid);
                if( ite_dlginfo == _did_dialog_info_.end())
                {
                    *state= -1;
                    return;
                }
                string via_branch_num;
                _sip_builder_->AuRegister( osip_msg, rtmeg, rtlen, 
                        via_branch_num,
                        ite_dlginfo->second,
                        ite_usinfo->second);
                *state = 0;//等待注册认证信息, 200 ok
                _affairs_tid_.insert( make_pair(via_branch_num, ite_cid_tid->second));
                _affairs_tid_.erase(ite_cid_tid);

                return;
            }else if( ( osip_msg)->status_code == 200)
            {
                switch( (*ite_cid_tid).second.fam_state)
                {
                    case SIP_REGISTER_WAIT_200:
                        {
                            /*todo: nothing to do
                             *register ok;
                             *删除相关事务记录
                             */
                            string branch_num;
                            branch_num = _sip_parser_->getBranchNum( osip_msg,_local_ip_str_);
                            auto ite_affairs_tid = _affairs_tid_.find( branch_num);
                            if( ite_affairs_tid == _affairs_tid_.end())
                            {
                                /*内部错误不影响外部事务*/
                                *state = 2;
                                return;
                            }
                            _affairs_tid_.erase( ite_affairs_tid);
                            rtlen = 0;
                            *state = 1;
                            return;
                            break;
                        }
                    case SIP_INVITE_WAIT_ACK:
                        {//todo: invite 事务结束，删除branch 
                            _sip_builder_->InviteACK( osip_msg, 
                                    rtmeg, 
                                    rtlen,
                                    state
                                    );
                            string to_tag_num = _sip_parser_->getToTag( osip_msg);
                            map< string, struct DialogInfo>::iterator ite_dlginfo;
                            string dialog_id = _sip_parser_->getDialogId( osip_msg);
                            ite_dlginfo = _did_dialog_info_.find( dialog_id);
                            if( ite_dlginfo == _did_dialog_info_.end())
                            {
#ifdef DEBUG
                                cout<<"no such dlg_info, dlgid:"<<dialog_id<<endl;
#endif
                                *state= -1;
                                return;
                            }
                            ite_dlginfo->second.to_tag_num = to_tag_num;
                            string branch_num;
                            branch_num = _sip_parser_->getBranchNum( osip_msg, _local_ip_str_);

                            map< string, struct TidState>::iterator ite_branch_tid;
                            ite_branch_tid = _affairs_tid_.find( branch_num);
                            if( ite_branch_tid == _affairs_tid_.end())
                            {
#ifdef DEBUG
                                cout<<"no such affairs id"<<endl;
#endif
                                return;
                            }
                            uint32_t invite_tid = ite_branch_tid->second.tid;
                            _affairs_tid_.erase( ite_branch_tid);
                            /*invite 事务结束，会话已建立*/
                            _tid_did_.insert(make_pair( invite_tid, dialog_id));
                            return;
                            break;
                        }
                    case SIP_BYE_WAIT_ACK:
                        {
                            /*must delete the dig info*/
                            map< string, struct DialogInfo>::iterator ite_dialog_info;
                            string dialog_id = _sip_parser_->getDialogId( osip_msg);
                            ite_dialog_info = _did_dialog_info_.find( dialog_id);
                            if( ite_dialog_info == _did_dialog_info_.end())
                            {
                                *state = -1;
                                return;
                            }
                            _did_dialog_info_.erase( ite_dialog_info);
                        }
                    default:
                        {
                            break;
                        }
                }

                *state = 1;
                return;
            }else if( MSG_IS_INVITE( osip_msg))
            {
                *state = 1;
                return;
            }
        }

        /*
         * 作为媒体服务器，是可以被邀请进行视频流的转发的
         * */
        void SIPManager::BeenInvited( osip_message_t* osip_msg, string port,
                 char** rtmsg, size_t* rtlen, int* state)
        {
            string dialog_id = _sip_parser_->getDialogId(osip_msg);
            struct DialogInfo dig_info;
            dig_info.call_id_num = _sip_parser_->getCallId(osip_msg);
            dig_info.call_id_num = _sip_parser_->getCallId(osip_msg);
            _sip_builder_->BeenInvited( osip_msg, port, rtmsg, rtlen, state,
                    dig_info);
            if( -1 != *state)
            {
                _did_dialog_info_.insert( make_pair( dialog_id, dig_info));
                return;
            }
            return;
        }

        /*
         * tid:invite时候上层传入的任务id.
         * contactid:register时候产生的contactid.
         * */
        void SIPManager::Bye( uint32_t tid, int contactid, 
                char** rtmeg, size_t *rtlen, int* state) 
        {
            map< uint32_t, string>::iterator ite_tid_did;
            ite_tid_did = _tid_did_.find( tid);
            if( ite_tid_did == _tid_did_.end())
            {
                *state = -1;
                return;
            }
            string dialog_id = ite_tid_did->second;

            map< string, struct DialogInfo>::iterator ite_did_info;
            ite_did_info = _did_dialog_info_.find( dialog_id);
            if( ite_did_info == _did_dialog_info_.end())
            {
                *state =-1;
                return;
            }
            struct DialogInfo dig_info = ite_did_info->second;

            map< int, struct ReAuthInfo>::iterator ite_rid_reau;
            ite_rid_reau = _rid_usinfo_.find( contactid);
            if( ite_rid_reau == _rid_usinfo_.end())
            {
                *state = -1;
                return;
            }
            struct ReAuthInfo re_info = ite_rid_reau->second;
            string via_branch_num;
            _sip_builder_->Bye( rtmeg, rtlen , state, via_branch_num,
                    dig_info, re_info);
            if( *state == -1)
            {
                return;
            }

            struct TidState tid_state;
            tid_state.tid = tid;
            tid_state.rid = contactid;
            tid_state.fam_state = SIP_BYE_WAIT_ACK;
            _affairs_tid_.insert( make_pair( via_branch_num, tid_state));
            /*wait for bye ack*/
            *state = 0;
            return;
        }

        bool SIPManager::CleanTid( uint32_t tid)
        {
            std::map< string, struct TidState >::iterator ite = _affairs_tid_.begin();
            for( ; ite != _affairs_tid_.end() ; ++ite)
            {
                if( ite->second.tid == tid)
                {
                    _affairs_tid_.erase( ite);
                    break;
                }
            }
            return true;
        }

        void SIPManager::DestoryMsg( osip_message_t* msg)
        {
            osip_message_free( msg);
        }

        void SIPManager::AddToTag( osip_message_t* msg)
        {

        }
        SIPManager::~SIPManager()
        {

        }
    }
}
