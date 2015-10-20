/*************************************************************************
	> File Name: SIPMSClient.h
	> Author: cooperz
	> Mail: zbzcsn@qq.com
	> Created Time: Thu 30 Jul 2015 04:28:16 PM CST
 ************************************************************************/

#ifndef _SIPMSCLIENT_H
#define _SIPMSCLIENT_H
#include "SIPUABase.h"
#include "SIPStatuCode.h"

namespace svss
{
    namespace SIP
    {
        struct SIPContent
        {
            std::string camera_xml_;
            std::string camera_dev_id_;
            std::string remote_ip_;
            std::string remote_recv_port_;
            std::string playback_start_time_;
            std::string playback_end_time_;
        };
        enum MS_CLIENT_FSM
        {
            MS_CLIENT_FSM_START,
            MS_CLIENT_FSM_INVITE_STORE,
            MS_CLIENT_FSM_REGISTER,
            MS_CLIENT_FSM_UNREGISTER,
            MS_CLIENT_FSM_HEARTBEAT,
            MS_CLIENT_FSM_GET_CAMERA_INFO,
            MS_CLIENT_FSM_PLAYBACK_WAIT_ACK,
            MS_CLIENT_FSM_END
        };

        struct MSClientState
        {
            MS_CLIENT_FSM fsm_state;
            uint32_t sip_tid;
        };

        class SIPMSClient : public SIPUABase
        {
            public:
                explicit SIPMSClient( std::string local_dev_name,
                        std::string local_ip,
                        std::string local_port,
                        std::string passwd
                        );
                virtual ~SIPMSClient();
                /*
                 * fuction: FSMDrive
                 * input: 
                 *     reserved_task_id: 保留任务id，如果是返回值是SIP_PLAYBACK_RECVED，
                 *     就说明这个任务id被使用了，MS产生了一次回放任务，之后这个值将被用
                 *     在视频数据发送完成是调用FileEnd()使用；
                 *     msg：收到的sip协议信息
                 *     len：sip协议内容长度
                 * output:
                 *     rt_task_id: 返回该消息对应的任务id
                 *     rtmsg: 接下来要发送的消息存储
                 *     rtlen：rtmsg的大小
                 *     sip_content：
                 *         如果事务是 请求摄像头列表，那么sip_content成员
                 *     camera_xml会填充内容，内容格式参考国标附录A
                 *         如果事务是被邀请playback，那么sip_content成员
                 *     camera_dev_id_ 被填充为查看摄像头的编号，
                 *     remote_ip_ 被填充为对方接收视频的ip，
                 *     remote_recv_port_被填充为对方接受视频的端口，
                 *     play_back_start_time_被填充为查看视频的时间起点，
                 *     play_bcak_end_time_ 被填充为查看视频的时间止点
                 * return:
                 *     SIP_PLAYBACK_RECVED:表示收到了playback请求，请使用
                 *     sip_content的内容向ds下发命令;
                 *     SIP_MSG_BEEN_DROP:表示该消息不属于任何本机的任何事务
                 *     这种消息直接忽略，不需要处理;
                 *     SIP_CAMERA_INFO:获得了camerainfo，sip_content的
                 *         camera_xml 被填充
				 *	   SIP_HREATBEAT_SUCCESS:心跳成功
				 *     SIP_REGISTER_SUCCESS:注册成功
				 *     SIP_CAMERA_INFO:获得了摄像头列表
				 *     SIP_CONTINUE:事务正常进行中，需要发送rtmsg;
                 *     SIP_MSG_BEEN_DROP:这个消息不用管;
				 *     SIP_CORE_ERR：失败
                 * */
                SIP_STATE_CODE FSMDrive(uint32_t reserved_task_id, 
                        char* msg, size_t len, uint32_t* rt_task_id, 
                        char** rtmsg, size_t* rtlen, 
                        struct SIPContent &sip_content);
                int MSClientInit();
                SIP_STATE_CODE RegisterMSClient( uint32_t task_id, char** rtmsg, 
                        size_t *rtlen,
                        std::string remote_name,
                        std::string remote_ip,
                        std::string remote_port);
                SIP_STATE_CODE HeartBeat( uint32_t task_id, char** rtmsg,
                        size_t *rtlen);
                SIP_STATE_CODE GetCameraInfo( uint32_t task_id, char** rtmsg,
                        size_t *rtlen);
                int UnRegisterMSClient();
                /*
                 * 视频传输完成之后，主动调用这个函数。task_id不是MS主动产生的，
                 * 而是在之前收到视频请求时候调用FSMDrive时的reserved_task_id。
                 * */
                SIP_STATE_CODE FileEnd( uint32_t task_id, char** rtmsg,
                        size_t *rtlen);
            private:
                bool MayPlayBackRuqeust( char* msg, size_t len,
                        std::string& camera_dev_id,
                        std::string& remote_ip, 
                        std::string& remote_port,
                        std::string& playback_start_time,
                        std::string& playback_end_time,
                        std::string& dialog_id);
                bool MayCameraInfo( char* msg,
                        size_t len,
                        char** rtmsg,
                        size_t* rtlen,
                        struct SIPContent& content);
            private:
                MS_CLIENT_FSM _fsm_status_;
                std::map< uint32_t, struct MSClientState> _task_state_machine_;
                std::map< uint32_t, uint32_t> _siptid_taskid_;
                std::map< uint32_t, std::string> _taskid_dialogid_;
                int _recver_vedio_serial_num_;
        };
    }
}

#endif
