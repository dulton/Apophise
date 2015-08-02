/*************************************************************************
  > File Name: SIPMSClient.cpp
  > Author: cooperz
  > Mail: zbzcsn@qq.com
  > Created Time: Thu 30 Jul 2015 04:27:15 PM CST
 ************************************************************************/

#include "SIPMSClient.h"

#include<iostream>
#include<sstream>

#include "SIPStatuCode.h"

using namespace std;

namespace svss
{
    namespace SIP
    {
        SIPMSClient::SIPMSClient( string local_dev_name,
                string local_ip,
                string local_port,
                string passwd):SIPUABase( local_dev_name,
                    local_ip,
                    local_port,
                    passwd)
        {
            _fsm_status_ = MS_CLIENT_FSM_START;
            _recver_vedio_serial_num_ = 1;
        }

        SIPMSClient::~SIPMSClient()
        {

        }

        int SIPMSClient::MSClientInit()
        {
            bool rtinit = Init();
            if( rtinit )
                return SIP_SUCCESS;
            else
                return SIP_MANAGER_INIT_ERR;
        }

        uint32_t SIPMSClient::FSMDrive(uint32_t task_id, char* msg, size_t len, 
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
                    case MS_CLIENT_FSM_REGISTER:
                        {
                            if( 0 == state)
                            {
                                _status_code_ = SIP_LOGIN_ING;
                                return SIP_CONTINUE;
                            }
                            else if( 1 == state)
                            {
                                _status_code_ = SIP_LOGIN_OK;
                                _siptid_taskid_.erase( ite_siptid_taskid);
                                /*跳转状态机的下一个状态*/
                                ite_task_state->second.fsm_state = MS_CLIENT_FSM_END;
                            }
                            break;
                        }
                    case MS_CLIENT_FSM_UNREGISTER:
                        {
                            if( 0 == state)
                            {
                                return SIP_CONTINUE;
                            }
                            else if( 1==state)
                            {
                                _siptid_taskid_.erase( ite_siptid_taskid);
                                /*跳转状态机的下一个状态*/
                                ite_task_state->second.fsm_state = MS_CLIENT_FSM_END;
                            }
                            break;
                        }
                    default:
                        break;
                };
                if( MS_CLIENT_FSM_END == ite_task_state->second.fsm_state)
                {
                    /*客户端级别的任务状态机走完
                     *返回上层 之前发起这次客户端任务的ID
                     * */
                    uint32_t rttask_id = ite_siptid_taskid->second;
                    _siptid_taskid_.erase(ite_siptid_taskid);
                    return rttask_id;
                }
                else
                {
                    return SIP_CONTINUE;
                }
            }
            return SIP_CORE_ERR;
        }

        int SIPMSClient::RegisterMSClient(uint32_t task_id, char** rtmsg, 
                size_t *rtlen,
                string remote_name,
                string remote_ip,
                string remote_port,
                string passwd)
        {
            int rt = Register( rtmsg, rtlen, remote_name, remote_ip, 
                    remote_port, passwd);
            if( rt == SIP_SUCCESS)
            {
                struct MSClientState cli_state;
                cli_state.sip_tid = _ua_task_id_;
                cli_state.fsm_state = MS_CLIENT_FSM_REGISTER;
                _task_state_machine_.insert( make_pair( task_id, cli_state));
                _siptid_taskid_.insert( make_pair( _ua_task_id_, task_id));
                _ua_task_id_++;
                return SIP_SUCCESS;
            }
            else 
                return SIP_REGISTER_ERR;
        }

        int SIPMSClient::UnRegisterMSClient()
        {
            return Unregister();
        }
    }
}
