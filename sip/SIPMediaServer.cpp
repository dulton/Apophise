/*************************************************************************
	> File Name: SIPMediaServer.cpp
	> Author: cooperz
	> Mail: zbzcsn@qq.com
	> Created Time: Thu 23 Jul 2015 02:38:06 PM CST
 ************************************************************************/

#include<iostream>
using namespace std;

#include "SIPMediaServer.h"

#include<iostream>
#include<sstream>

#include "SIPStatuCode.h"
#include "SIPParser.h"

using namespace std;

namespace svss
{
    namespace SIP
    {
        SIPMediaServer::SIPMediaServer( string local_dev_name,
                string local_ip,
                string local_port,
                string passwd):SIPUABase( local_dev_name,
                    local_ip,
                    local_port,
                    passwd)
        {
            _fsm_status_ = FSM_START;
            _recver_vedio_serial_num_ = 1;
            _task_id_ = 1;
        }

        SIPMediaServer::~SIPMediaServer()
        {

        }

        int SIPMediaServer::MediaServerInit()
        {
            bool rtinit = Init();
            if( rtinit)
                return SIP_SUCCESS;
            else
                return SIP_MANAGER_INIT_ERR;
        }

        uint32_t SIPMediaServer::FSMDrive(uint32_t task_id, char* msg, size_t len,  
                string &port, char** rtmsg, size_t* rtlen)
        {
            int state;
            uint32_t sip_tid = 0;
            _manager_.DealSIPMeg( msg, len, port, rtmsg, rtlen, 
                    &state, &sip_tid);
            /*task_id是为新产生的事务预留的ID,
             *若state为 -1 ，则此条消息不属于任何已知处理范畴,亦或者错误
             *若state为0，则注册，或者被邀请使用
             *若state为1，表示此次事务已经结束,改变状态机
             * */
            string from_uri = (_manager_._sip_parser_)->getFromUri( msg, len);
            auto ite_uri_fsm = _task_state_machine_.find( from_uri);    
            if( -1 == state)
            {
                return SIP_OUT_RANGE;
            }
            if( 0 == state)
            {
                if( sip_tid != 0)
                {
                    /*register*/
                    return SIP_CONTINUE;
                }
                if( ite_uri_fsm == _task_state_machine_.end())
                {
                    /*第一次接收到这个uri发出邀请，
                     *产生了媒体服务器层面的事务.
                     * */
                    struct MediaServerState media_state;
                    media_state.sip_tid = 0;
                    media_state.fsm_state = FSM_1_ACK;
                    _task_state_machine_.insert( make_pair(from_uri, media_state));
                    return SIP_CONTINUE;
                }
                else
                {
                    /*这次事务已经有记录了*/
                    if( FSM_2_ACK == (ite_uri_fsm->second).fsm_state)
                    {
                        _task_state_machine_.erase( ite_uri_fsm);
                        return SIP_SUCCESS;
                    }else
                    {
                        (ite_uri_fsm->second).fsm_state = FSM_2_ACK;
                        return SIP_CONTINUE;
                    }
                }
            }
            if( 1 == state)
            {

            }
            return SIP_CORE_ERR;
        }

        int SIPMediaServer::RegisterMediaServer(uint32_t task_id, char** rtmsg, 
                size_t *rtlen,
                string remote_ip,
                string remote_port)
        {
            /*父类已经将_ua_task_ 作为sip tid传入下层*/
            int rt = Register( rtmsg, rtlen, remote_ip, remote_port);
            if( rt == SIP_SUCCESS)
            {
                struct MediaServerState mds_state;
                mds_state.sip_tid = _ua_task_id_;
                mds_state.fsm_state = FSM_REGISTER;
                _task_state_machine_.insert( make_pair( task_id, mds_state));
                _siptid_taskid_.insert( make_pair( _ua_task_id_, task_id));
                _ua_task_id_++;
                return SIP_SUCCESS;
            }
            else 
                return SIP_REGISTER_ERR;
        }

        int SIPMediaServer::UnRegisterClient()
        {
            return Unregister();
        }

        int SIPMediaServer::InviteStore( uint32_t task_id, string recv_port,
                char** rtmsg, size_t *rtlen)
        {
            return 1;
        }
    }
}
