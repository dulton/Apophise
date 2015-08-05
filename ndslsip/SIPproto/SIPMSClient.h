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
        enum MS_CLIENT_FSM
        {
            MS_CLIENT_FSM_START,
            MS_CLIENT_FSM_INVITE_STORE,
            MS_CLIENT_FSM_REGISTER,
            MS_CLIENT_FSM_UNREGISTER,
            MS_CLIENT_FSM_HEARTBEAT,
            MS_CLIENT_FSM_GET_CAMERA_INFO,
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
                SIP_STATE_CODE FSMDrive(uint32_t reserved_task_id, 
                        char* msg, size_t len, uint32_t* rt_task_id, 
                        int* rt_task_state, char** rtmsg, size_t* rtlen, 
                        std::string& camera_xml);
                int MSClientInit();
                SIP_STATE_CODE RegisterMSClient( uint32_t task_id, char** rtmsg, 
                        size_t *rtlen,
                        std::string remote_name,
                        std::string remote_ip,
                        std::string remote_port,
                        std::string passwd);
                SIP_STATE_CODE HeartBeat( uint32_t task_id, char** rtmsg,
                        size_t *rtlen);
                SIP_STATE_CODE GetCameraInfo( uint32_t task_id, char** rtmsg,
                        size_t *rtlen);
                int UnRegisterMSClient();
            private:
                MS_CLIENT_FSM _fsm_status_;
                std::map< uint32_t, struct MSClientState> _task_state_machine_;
                std::map< uint32_t, uint32_t> _siptid_taskid_;
                int _recver_vedio_serial_num_;
        };
    }
}

#endif
