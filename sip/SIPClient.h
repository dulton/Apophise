/*************************************************************************
  > File Name: SIPClient.h
  > Author: cooperz
  > Mail: zbzcsn@qq.com
  > Created Time: Thu 23 Jul 2015 02:37:18 PM CST
 ************************************************************************/

#ifndef _SIPCLIENT_H
#define _SIPCLIENT_H
#include <iostream>

#include "SIPUABase.h"

namespace svss
{
    namespace SIP
    {
        enum CLIENT_FSM
        {
            FSM_START,
            FSM_INVITE_STORE,
            FSM_REGISTER,
            FSM_END
        };

        struct ClientState
        {
            CLIENT_FSM fsm_state;
            uint32_t sip_tid;
        };

        class SIPClient : public SIPUABase
        {
            public:
                explicit SIPClient( std::string local_dev_name,
                        std::string local_ip,
                        std::string local_port,
                        std::string passwd
                        );
                virtual ~SIPClient();
                uint32_t FSMDrive(uint32_t task_id, char* msg, size_t len, 
                        std::string &port, char** rtmsg, size_t* rtlen);
                int ClientInit();
                int RegisterClient( uint32_t task_id, char** rtmsg, size_t *len,
                        std::string remote_ip, 
                        std::string remote_port);
                int UnRegisterClient();
                int InviteStore( uint32_t task_id, std::string recv_port,
                        char** rtmsg, size_t *rtlen);
            private:
                CLIENT_FSM _fsm_status_;
                std::map< uint32_t, struct ClientState> _task_state_machine_;
                std::map< uint32_t, uint32_t> _siptid_taskid_;
                int _recver_vedio_serial_num_;
        };
    }
}

#endif
