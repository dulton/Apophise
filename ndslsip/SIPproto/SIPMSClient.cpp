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

        SIP_STATE_CODE SIPMSClient::FSMDrive(uint32_t reserved_task_id,
                char* msg, size_t len, uint32_t* rt_task_id,
                char** rtmsg, size_t* rtlen,
                struct SIPContent &sip_content)
        {
            int state;
            uint32_t sip_tid = -1;
            /*MS作为点播服务器，在回复200ok的信息中，
             * sdp内容中的port是不会被使用的
             * */
            string port("8888");
            _manager_.DealSIPMeg( msg, len, port, rtmsg, rtlen, 
                    &state, &sip_tid);
            //*rt_task_state = state;
            auto ite_siptid_taskid = _siptid_taskid_.find( sip_tid);
            if( ite_siptid_taskid == _siptid_taskid_.end())
            {
                bool is_play_back = MayPlayBackRuqeust( msg, len, 
                        sip_content.camera_dev_id_,
                        sip_content.remote_ip_, 
                        sip_content.remote_recv_port_,
                        sip_content.playback_start_time_,
                        sip_content.playback_end_time_);
                if( is_play_back)
                {
                    return SIP_PLAYBACK_RECVED;
                }
                else
                {
                    return SIP_MSG_BEEN_DROP;
                }
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
                                /*跳转状态机的下一个状态*/
                                ite_task_state->second.fsm_state = MS_CLIENT_FSM_END;
                            }
                            break;
                        }
                    case MS_CLIENT_FSM_HEARTBEAT:
                        {
                            if( 0 == state)
                            {
                                return SIP_CONTINUE;
                            }
                            else if( 1 == state)
                            {
                                /*跳转状态机的下一个状态*/
                                ite_task_state->second.fsm_state = MS_CLIENT_FSM_END;
                            }
                            break;
                        }
                    case MS_CLIENT_FSM_GET_CAMERA_INFO:
                        {
                            if( 0 == state)
                            {
                                return SIP_CONTINUE;
                            }
                            else if( 1==state)
                            {
                                /*获得XML内容，既摄像头列表*/
                                _manager_.GetContentBody( msg, len, sip_content.camera_xml_);
                                /*跳转状态机的下一个状态*/
                                ite_task_state->second.fsm_state = MS_CLIENT_FSM_END;
                                _siptid_taskid_.erase(ite_siptid_taskid);
                                _task_state_machine_.erase(ite_task_state);
                                return SIP_CAMERA_INFO;
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

        SIP_STATE_CODE SIPMSClient::RegisterMSClient(uint32_t task_id, char** rtmsg, 
                size_t *rtlen,
                string remote_name,
                string remote_ip,
                string remote_port)
        {
            int rt = Register( rtmsg, rtlen, remote_name, remote_ip, 
                    remote_port);
            if( rt == SIP_SUCCESS)
            {
                struct MSClientState cli_state;
                cli_state.sip_tid = _ua_task_id_;
                cli_state.fsm_state = MS_CLIENT_FSM_REGISTER;
                _task_state_machine_.insert( make_pair( task_id, cli_state));
                _siptid_taskid_.insert( make_pair( _ua_task_id_, task_id));
                _ua_task_id_++;
                return SIP_CONTINUE;
            }
            else 
                return SIP_REGISTER_ERR;
        }

        SIP_STATE_CODE SIPMSClient::HeartBeat(uint32_t task_id, char** rtmsg,
                size_t *rtlen)
        {
            int sip_id = _ua_task_id_;
            int state;
            _manager_.HeartBeat( sip_id, _contact_id_, rtmsg, rtlen, &state);
            {
                struct MSClientState cli_state;
                cli_state.sip_tid = _ua_task_id_;
                cli_state.fsm_state = MS_CLIENT_FSM_HEARTBEAT;
                _task_state_machine_.insert( make_pair( task_id, cli_state));
                _siptid_taskid_.insert( make_pair( _ua_task_id_, task_id));
                _ua_task_id_++;
            }
            return SIP_CONTINUE;
        }

        SIP_STATE_CODE SIPMSClient::GetCameraInfo(uint32_t task_id, char** rtmsg,
                size_t *rtlen)
        {
            int sip_id = _ua_task_id_;
            int state;
            _manager_.GetCameraInfo( sip_id, _contact_id_, rtmsg, rtlen, &state);
            {
                struct MSClientState cli_state;
                cli_state.sip_tid = _ua_task_id_;
                cli_state.fsm_state = MS_CLIENT_FSM_GET_CAMERA_INFO;
                _task_state_machine_.insert( make_pair( task_id, cli_state));
                _siptid_taskid_.insert( make_pair( _ua_task_id_, task_id));
                _ua_task_id_++;
            }
            return SIP_CONTINUE;
        }

        bool SIPMSClient::MayPlayBackRuqeust( char* msg, size_t len, 
                string &camera_dev_id,
                string &remote_ip, string &remote_port, 
                string &playback_start_time, string &playback_end_time)
        {
            bool rt =  _manager_.IsPlayBackRequest( msg, len, camera_dev_id, remote_ip, 
                    remote_port, playback_start_time, playback_end_time);
            return rt;
        }
        int SIPMSClient::UnRegisterMSClient()
        {
            return Unregister();
        }
    }
}
