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

        bool SIPManager::SIPManagerInit()
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

        void SIPManager::Register( SIP_IN uint32_t tid , 
                SIP_OUT char** meg,
                SIP_OUT size_t* len,
                SIP_OUT int* state,
                SIP_IN string uas_ip ,
                SIP_IN string uas_listen_port_str, 
                SIP_IN string passwd
                )
        {

            string call_id_num;
            string from_tag_num;
            string via_branch_num;
            _sip_builder_->Register( meg, len , state, call_id_num, 
                    from_tag_num, via_branch_num,
                    uas_ip, uas_listen_port_str);
#ifdef DEBUG
            string ret_meg = string((*meg));
            cout<<"Register meg'string :"<< ret_meg << endl;
            cout<<"call_id"<<call_id_num<<endl;
#endif
            if( *state != -1)
            {
                struct TidState tid_state;
                tid_state.tid = tid;
                tid_state.rid = _registerid_;
                tid_state.fam_state = SIP_REGISTER_WAIT_401;
                _affairs_tid_.insert( make_pair( via_branch_num, tid_state));    
                struct ReAuthinfo reau;
                reau.passwd = passwd;
                reau.uas_ip = uas_ip;
                reau.uas_port_str = uas_listen_port_str;
                _rid_usinfo_.insert( make_pair( _registerid_, reau));
                _registerid_++;
                *state = 0;
                return;
            }
            return;
        }

        void SIPManager::InviteLivePlay( SIP_IN uint32_t tid,
                SIP_OUT char** meg,
                SIP_OUT size_t *len,
                SIP_OUT int* state,
                SIP_IN string remote_dev_name,
                SIP_IN string uas_ip,
                SIP_IN string uas_listen_port_str,
                SIP_IN string sender_vedio_serial_num,
                SIP_IN string recver_vedio_serial_num
                )
        {
            /*
            std::pair< std::map< uint32_t, SIP_FAM_STATE >::iterator,bool > ret;
            ret = _sip_state_.insert( make_pair( tid, SIP_REGISTER_WAIT_401));
            if( !ret.second )
            {
                *state = -1;
                return;
            }*/
            string call_id;
            _sip_builder_->InviteLivePlay( meg, len, state, call_id,
                    remote_dev_name, uas_ip, uas_listen_port_str, 
                    sender_vedio_serial_num, recver_vedio_serial_num);
#ifdef DEBUG
                cout<<"invite call id:"<< call_id<<endl;
#endif
            if( *state != -1)
            {
                struct TidState tid_state;
                tid_state.tid = tid;
                tid_state.fam_state = SIP_INVITE_WAIT_ACK;
                _cid_tid_.insert( make_pair( call_id, tid_state));
            }
            return;
        }

        void SIPManager::DealSIPMeg( SIP_IN char* meg, 
                SIP_IN size_t len,
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
            osip_msg = _sip_parser_->parser(  meg, len);
            string call_id = string(osip_msg->call_id->number); 
            map<string, struct TidState>::iterator ite_cid_tid;
            ite_cid_tid = _cid_tid_.find(call_id);
            if( ite_cid_tid == _cid_tid_.end())
            {//todo:no such call id
                //todo free the osipmeg
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
            }else if( (osip_msg)->status_code == 401)
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
                map< int, struct ReAuthinfo>::iterator ite_usinfo;
                ite_usinfo = _rid_usinfo_.find( rid);
                if( ite == _cid_rid_.end())
                {
                    *state = -1;
                    return;
                }
                _sip_builder_->AuRegister( osip_msg, rtmeg, rtlen, 
                        (ite_usinfo->second).uas_ip,
                        (ite_usinfo->second).uas_port_str,
                        (ite_usinfo->second).passwd);
                //osip_message_to_str( osip_msg, rtmeg, rtlen);
                *state = 0;//等待 注册 认证信息的 200 ok
                return;
            }else if( ( osip_msg)->status_code == 200)
            {
                switch( (*ite_cid_tid).second.fam_state)
                {
                    case SIP_REGISTER_WAIT_200:
                        {
                            //todo: nothing to do
                            //register ok;
                            *state = 1;
                            return;
                            break;
                        }
                    case SIP_INVITE_WAIT_ACK:
                        { 
                            _sip_builder_->InviteACK( osip_msg, 
                                    rtmeg, 
                                    rtlen,
                                    state
                                    );
                            if( *state == 1)
                            {
                                return;
                            }
                            break;
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
        bool SIPManager::CleanTid( uint32_t tid)
        {
            std::map< string, struct TidState >::iterator ite = _cid_tid_.begin();
            for( ; ite != _cid_tid_.end() ; ++ite)
            {
                if( ite->second.tid == tid)
                {
                    _cid_tid_.erase( ite);
                    break;
                }
            }
            return true;
        }

        void DestoryMsg( osip_message_t* msg)
        {
            osip_message_free( msg);
        }
    }
}
