/*************************************************************************
	> File Name: SIPMSClient.h
	> Author: cooperz
	> Mail: zbzcsn@qq.com
	> Created Time: Thu 30 Jul 2015 04:28:16 PM CST
 ************************************************************************/

#ifndef _SIPMSCLIENT_H
#define _SIPMSCLIENT_H
#include "SIPUABase.h"

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
                uint32_t FSMDrive(uint32_t task_id, char* msg, size_t len, 
                        std::string &port, char** rtmsg, size_t* rtlen);
                int MSClientInit();
                int RegisterMSClient( uint32_t task_id, char** rtmsg, size_t *len,
                        std::string remote_ip, 
                        std::string remote_port);
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
