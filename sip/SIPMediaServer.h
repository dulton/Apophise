/*************************************************************************
	> File Name: SIPMediaServer.h
	> Author: cooperz
	> Mail: zbzcsn@qq.com
	> Created Time: Thu 23 Jul 2015 02:37:58 PM CST
 ************************************************************************/

#ifndef _SIPMEDIASERVER_H
#define _SIPMEDIASERVER_H

#include <iostream>

#include "SIPUABase.h"

namespace svss
{
    namespace SIP
    {
        enum MEDIA_SERVER_FSM
        {
            FSM_START,
            FSM_REGISTER,
            FSM_1_ACK,
            FSM_2_ACK,
            FSM_END
        };

        struct MediaServerState
        {
            MEDIA_SERVER_FSM fsm_state;
            uint32_t sip_tid;
        };

        class SIPMediaServer : public SIPUABase
        {
            public:
                explicit SIPMediaServer( std::string local_dev_name,
                        std::string local_ip,
                        std::string local_port,
                        std::string passwd
                        );
                virtual ~SIPMediaServer();
                uint32_t FSMDrive( uint32_t task_id ,char* msg, size_t len, 
                        std::string &port, char** rtmsg, size_t* rtlen);
                int MediaServerInit();
                int RegisterMediaServer( uint32_t task_id, char** rtmsg, size_t *len,
                        std::string remote_ip, 
                        std::string remote_port);
                int UnRegisterMediaServer();
                int InviteStore( uint32_t task_id, std::string recv_port,
                        char** rtmsg, size_t *rtlen);
            private:
                MEDIA_SERVER_FSM _fsm_status_;
                std::map< std::string, struct MediaServerState> _task_state_machine_;
                std::map< uint32_t, uint32_t> _siptid_taskid_;
                int _recver_vedio_serial_num_;
        };
    }
}

#endif
