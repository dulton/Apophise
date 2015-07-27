/*************************************************************************
  > File Name: SIPClient.cpp
  > Author: cooperz
  > Mail: zbzcsn@qq.com
  > Created Time: Thu 23 Jul 2015 02:37:41 PM CST
 ************************************************************************/
#include "SIPClient.h"

#include<iostream>
#include<sstream>

#include "SIPStatuCode.h"

using namespace std;

namespace svss
{
    namespace SIP
    {
        SIPClient::SIPClient( string local_dev_name,
                string local_ip,
                string local_port,
                string passwd):SIPUABase( local_dev_name,
                    local_ip,
                    local_port,
                    passwd)
        {
            _fsm_status_ = FSM_START;
            _recver_vedio_serial_num_ = 1;
        }

        SIPClient::~SIPClient()
        {

        }

        int SIPClient::ClientInit()
        {
            bool rtinit = Init();
            if( rtinit )
                return SIP_SUCCESS;
            else
                return SIP_MANAGER_INIT_ERR;
        }

        uint32_t SIPClient::FSMDrive(uint32_t task_id, char* msg, size_t len, 
                string &port, char** rtmsg, size_t* rtlen)
        {
            int state;
            uint32_t sip_tid;
            _manager_.DealSIPMeg( msg, len, port, rtmsg, rtlen, 
                    &state, &sip_tid);
            auto ite_siptid_taskid = _siptid_taskid_.find( sip_tid);
            if( ite_siptid_taskid == _siptid_taskid_.end())
            {
                return SIP_CORE_ERR;
            }
            auto ite_task_state = _task_state_machine_.find( ite_siptid_taskid->second);
            if( ite_task_state == _task_state_machine_.end())
            {
                return SIP_CORE_ERR;
            }

            if( -1 != state){
                switch (ite_task_state->second.fsm_state)
                {
                    case FSM_REGISTER:
                        {
                            if( 0 == state)
                            {
                                return SIP_CONTINUE;
                            }
                            else if( 1 == state)
                            {
                                _siptid_taskid_.erase( ite_siptid_taskid);
                                /*跳转状态机的下一个状态*/
                                ite_task_state->second.fsm_state = FSM_END;
                            }
                            break;
                        }
                    case FSM_INVITE_STORE:
                        {
                            if( 0 == state)
                            {
                                return SIP_CONTINUE;
                            }
                            else if( 1==state)
                            {
                                _siptid_taskid_.erase( ite_siptid_taskid);
                                /*跳转状态机的下一个状态*/
                                ite_task_state->second.fsm_state = FSM_END;
                            }
                            break;
                        }
                    default:
                        break;
                };
                if( FSM_END == ite_task_state->second.fsm_state)
                {
                    /*客户端级别的任务状态机走完
                     *返回上层 之前发起这次客户端任务的ID
                     * */
                    return ite_siptid_taskid->second;
                }
                else
                {
                    return SIP_CONTINUE;
                }

            }
            return SIP_CORE_ERR;
        }

        int SIPClient::RegisterClient(uint32_t task_id, char** rtmsg, 
                size_t *rtlen,
                string remote_ip,
                string remote_port)
        {
            int rt = Register( rtmsg, rtlen, remote_ip, remote_port);
            if( rt == SIP_SUCCESS)
            {
                struct ClientState cli_state;
                cli_state.sip_tid = _ua_task_id_;
                cli_state.fsm_state = FSM_REGISTER;
                _task_state_machine_.insert( make_pair( task_id, cli_state));
                _siptid_taskid_.insert( make_pair( _ua_task_id_, task_id));
                return SIP_SUCCESS;
            }
            else 
                return SIP_REGISTER_ERR;
        }

        int SIPClient::UnRegisterClient()
        {
            return Unregister();
        }

        int SIPClient::InviteStore( uint32_t task_id, string recv_port,
                char** rtmsg, size_t *rtlen)
        {
            if( _status_code_ != SIP_LOGIN_OK)
                return _status_code_;
            _fsm_status_ = FSM_INVITE_STORE;
            int state;
            int tid = _ua_task_id_;
            _ua_task_id_++;
            stringstream recver_vedio_serial_num;
            recver_vedio_serial_num << _recver_vedio_serial_num_;
            _recver_vedio_serial_num_++;
            _manager_.InviteLivePlay( tid, _contact_id_, rtmsg ,
                    rtlen, &state, recv_port, string("0"), 
                    recver_vedio_serial_num.str());
            if( 0 == state)
            {
                struct ClientState cli_state;
                cli_state.sip_tid = tid;
                cli_state.fsm_state = FSM_INVITE_STORE;
                _task_state_machine_.insert( make_pair( task_id, cli_state));
                return SIP_CONTINUE;
            }
            else
            {
                return SIP_INVITE_STORE_ERR;
            }
        }
    }
}
