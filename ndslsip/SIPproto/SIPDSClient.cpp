/*************************************************************************
  > File Name: SIPDSClient.cpp
  > Author: cooperz
  > Mail: zbzcsn@qq.com
  > Created Time: Thu 23 Jul 2015 02:37:41 PM CST
 ************************************************************************/
#include "SIPDSClient.h"

#include<iostream>
#include<sstream>

#include "SIPStatuCode.h"

using namespace std;

namespace svss
{
    namespace SIP
    {
        SIPDSClient::SIPDSClient( string local_dev_name,
                string local_ip,
                string local_port,
                string passwd):SIPUABase( local_dev_name,
                    local_ip,
                    local_port,
                    passwd)
        {
            _fsm_status_ = DS_CLIENT_FSM_START;
            _recver_vedio_serial_num_ = 1;
        }

        SIPDSClient::~SIPDSClient()
        {

        }

        int SIPDSClient::ClientInit()
        {
            bool rtinit = Init();
            if( rtinit )
                return SIP_SUCCESS;
            else
                return SIP_MANAGER_INIT_ERR;
        }

        SIP_STATE_CODE SIPDSClient::FSMDrive(char* msg, size_t len, 
                uint32_t* rt_task_id ,char** rtmsg, size_t* rtlen)
        {
            int state;
            uint32_t sip_tid;
            string port("DS never used");
            _manager_.DealSIPMeg( msg, len, port, rtmsg, rtlen, 
                    &state, &sip_tid);
            auto ite_siptid_taskid = _siptid_taskid_.find( sip_tid);
            if( ite_siptid_taskid == _siptid_taskid_.end())
            {
                return SIP_MSG_BEEN_DROP;
            }
            *rt_task_id = ite_siptid_taskid->second;
            auto ite_task_state = _task_state_machine_.find( ite_siptid_taskid->second);
            if( ite_task_state == _task_state_machine_.end())
            {
                return SIP_CORE_ERR;
            }
            if( -1 != state){
                switch (ite_task_state->second.fsm_state)
                {
                    case DS_CLIENT_FSM_REGISTER:
                        {
                            if( 0 == state)
                            {
                                _status_code_ = SIP_LOGIN_ING;
                                return SIP_CONTINUE;
                            }
                            else if( 1 == state)
                            {
                                _status_code_ = SIP_LOGIN_OK;
                                /*跳转状态机的下一个状态*/
                                ite_task_state->second.fsm_state = DS_CLIENT_FSM_END;
                            }
                            break;
                        }
                    case DS_CLIENT_FSM_HEARTBEAT:
                        {
                            if( 0 == state)
                            {
                                return SIP_CONTINUE;
                            }
                            else if( 1 == state)
                            {
                                /*跳转状态机的下一个状态*/
                                ite_task_state->second.fsm_state = DS_CLIENT_FSM_END;
                            }
                            break;
                        }
                    case DS_CLIENT_FSM_INVITE_STORE:
                        {
                            if( 0 == state)
                            {
                                return SIP_CONTINUE;
                            }
                            else if( 1==state)
                            {
                                /*跳转状态机的下一个状态*/
                                ite_task_state->second.fsm_state = DS_CLIENT_FSM_END;
                                _siptid_taskid_.erase(ite_siptid_taskid);
                                _task_state_machine_.erase(ite_task_state);
                                return SIP_INVITE_STROE_ACK;
                            }
                            break;
                        }
                    default:
                        break;
                };
                if( DS_CLIENT_FSM_END == ite_task_state->second.fsm_state)
                {
                    /*客户端级别的任务状态机走完
                     *返回上层 之前发起这次客户端任务的ID
                     * */
                    _siptid_taskid_.erase(ite_siptid_taskid);
                    _task_state_machine_.erase(ite_task_state);
                    return SIP_SUCCESS;
                }
                else
                {
                    return SIP_CONTINUE;
                }
            }
            return SIP_CORE_ERR;
        }

        SIP_STATE_CODE SIPDSClient::RegisterDSClient(uint32_t task_id, char** rtmsg,
                size_t *rtlen,
                string remote_name,
                string remote_ip,
                string remote_port)
        {
            int rt = Register( rtmsg, rtlen, remote_name ,remote_ip, remote_port);
            if( rt == SIP_CONTINUE)
            {
                struct DSClientState cli_state;
                cli_state.sip_tid = _ua_task_id_;
                cli_state.fsm_state = DS_CLIENT_FSM_REGISTER;
                _task_state_machine_.insert( make_pair( task_id, cli_state));
                _siptid_taskid_.insert( make_pair( _ua_task_id_, task_id));
                _ua_task_id_++;
                return SIP_CONTINUE;
            }
            else 
                return SIP_REGISTER_ERR;
        }

        SIP_STATE_CODE SIPDSClient::UnRegisterClient()
        {
            return Unregister();
        }

        SIP_STATE_CODE SIPDSClient::HeartBeat(uint32_t task_id, char** rtmsg,
                size_t *rtlen)
        {
            int sip_id = _ua_task_id_;
            int state;
            _manager_.HeartBeat( sip_id, _contact_id_, rtmsg, rtlen, &state);
            {
                struct DSClientState cli_state;
                cli_state.sip_tid = _ua_task_id_;
                cli_state.fsm_state = DS_CLIENT_FSM_HEARTBEAT;
                _task_state_machine_.insert( make_pair( task_id, cli_state));
                _siptid_taskid_.insert( make_pair( _ua_task_id_, task_id));
                _ua_task_id_++;
            }
            return SIP_CONTINUE;
        }

        SIP_STATE_CODE SIPDSClient::InviteStore( uint32_t task_id, string recv_port,
                string dev_id_been_invited, char** rtmsg, size_t *rtlen)
        {
            if( _status_code_ != SIP_LOGIN_OK)
                return _status_code_;
            _fsm_status_ = DS_CLIENT_FSM_INVITE_STORE;
            int state;
            int tid = _ua_task_id_;
            stringstream recver_vedio_serial_num;
            recver_vedio_serial_num << _recver_vedio_serial_num_;
            _recver_vedio_serial_num_++;
            _manager_.InviteLivePlay( tid, _contact_id_, rtmsg ,
                    rtlen, &state, dev_id_been_invited, recv_port, string("1"), 
                    recver_vedio_serial_num.str());
            if( 0 == state)
            {
                struct DSClientState cli_state;
                cli_state.sip_tid = tid;
                cli_state.fsm_state = DS_CLIENT_FSM_INVITE_STORE;
                _task_state_machine_.insert( make_pair( task_id, cli_state));
                _siptid_taskid_.insert( make_pair( _ua_task_id_, task_id));
                _ua_task_id_++;
                return SIP_CONTINUE;
            }
            else
            {
                /*废除这个错误的事务ID*/
                _ua_task_id_++;
                return SIP_INVITE_STORE_ERR;
            }
        }
    }
}
