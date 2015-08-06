/*************************************************************************
  > File Name: SIPDSClient.h
  > Author: cooperz
  > Mail: zbzcsn@qq.com
  > Created Time: Thu 23 Jul 2015 02:37:18 PM CST
 ************************************************************************/

#ifndef _SIPCLIENT_H
#define _SIPCLIENT_H
#include <iostream>

#include "SIPUABase.h"
#include "SIPStatuCode.h"

namespace svss
{
    namespace SIP
    {
        enum DS_CLIENT_FSM
        {
            DS_CLIENT_FSM_START,
            DS_CLIENT_FSM_INVITE_STORE,
            DS_CLIENT_FSM_REGISTER,
            DS_CLIENT_FSM_HEARTBEAT,
            DS_CLIENT_FSM_END
        };

        struct DSClientState
        {
            DS_CLIENT_FSM fsm_state;
            uint32_t sip_tid;
        };

        class SIPDSClient : public SIPUABase
        {
            public:
                explicit SIPDSClient( std::string local_dev_name,
                        std::string local_ip,
                        std::string local_port,
                        std::string passwd
                        );
                virtual ~SIPDSClient();
                /*
                 * fuction: FSMDrive
                 * input:
                 *     msg:收到的sip协议信息
                 *     len：收到的suo协议信息长度
                 * output:
                 *     rt_task_id：返回该sip协议对应的上层任务id
                 *     rtmsg：需要发送的sip消息体
                 *     rtlen：需要发送的sip消息体长度
                 * return:
                 *     SIP_SUCCESS：心跳或者注册成功时返回
                 *     SIP_CONTINUE: 证明任务正常进行，需要把remsg发送出去
                 *     SIP_INVITE_STROE_ACK: 证明发出的邀请某个摄像头数据的
                 *     请求成功了
                 * */
                SIP_STATE_CODE FSMDrive( char* msg, size_t len, 
                        uint32_t* rt_task_id, char** rtmsg, 
                        size_t* rtlen);
                int ClientInit();
                /*
                 * fuction: RegisterDSClient
                 * input :
                 *     task_id: 你发起注册任务的任务id
                 *     remote_name: sip 服务器的dev名字
                 *     remote_ip：sip服务器的ip
                 *     remote_port： sip服务器的端口
                 * output ：
                 *     rtmsg：需要发送的sip消息体
                 *     len： rtmsg的大小
                 * return:
                 *     SIP_CONTINUE:表示构造sip消息成功
                 *     其他返回值就是错误
                 * */
                SIP_STATE_CODE RegisterDSClient( uint32_t task_id, char** rtmsg, 
                        size_t *len,
                        std::string remote_name,
                        std::string remote_ip, 
                        std::string remote_port);
                SIP_STATE_CODE UnRegisterClient();
                /*
                 * fuction: InviteStore
                 * input :
                 *     task_id: 你发起邀请摄像头任务的任务id
                 *     recv_port：要求摄像头发数据发向本机的哪个端口
                 *     dev_id_been_invited: 摄像头的dev_name
                 * output ：
                 *     rtmsg：需要发送的sip消息体
                 *     rtlen： rtmsg的大小
                 * return :
                 *     SIP_CONTINUE:表示构造sip消息成功
                 *     其他返回值就是错误
                 * */
                SIP_STATE_CODE InviteStore( uint32_t task_id, std::string recv_port, 
                        std::string dev_id_been_invited,
                        char** rtmsg, size_t *rtlen);
                /*
                 * fuction: HeartBeat
                 * input :
                 *     task_id: 你发起心跳任务的任务id
                 * output ：
                 *     rtmsg：需要发送的sip消息体
                 *     rtlen：rtmsg的大小
                 * return :
                 *     SIP_CONTINUE:表示构造sip消息成功
                 *     其他返回值就是错误
                 * */
                SIP_STATE_CODE HeartBeat( uint32_t task_id, char** rtmsg,
                        size_t *rtlen);
            private:
                DS_CLIENT_FSM _fsm_status_;
                std::map< uint32_t, struct DSClientState> _task_state_machine_;
                std::map< uint32_t, uint32_t> _siptid_taskid_;
                int _recver_vedio_serial_num_;
        };
    }
}

#endif
