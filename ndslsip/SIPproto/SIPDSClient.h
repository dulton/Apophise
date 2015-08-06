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

namespace svss
{
    namespace SIP
    {
        enum DS_CLIENT_FSM
        {
            DS_CLIENT_FSM_START,
            DS_CLIENT_FSM_INVITE_STORE,
            DS_CLIENT_FSM_REGISTER,
            DS_CLIENT_FSM_END
        };

        struct ClientState
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
                uint32_t FSMDrive(uint32_t task_id, char* msg, size_t len, 
                        std::string &port, char** rtmsg, size_t* rtlen);
                int ClientInit();
                int RegisterDSClient( uint32_t task_id, char** rtmsg, size_t *len,
                        std::string remote_name,
                        std::string remote_ip, 
                        std::string remote_port);
                int UnRegisterClient();
                int InviteStore( uint32_t task_id, std::string recv_port,
                        char** rtmsg, size_t *rtlen);
            private:
                DS_CLIENT_FSM _fsm_status_;
                std::map< uint32_t, struct ClientState> _task_state_machine_;
                std::map< uint32_t, uint32_t> _siptid_taskid_;
                int _recver_vedio_serial_num_;
        };
    }
}

#endif
