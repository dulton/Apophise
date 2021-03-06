/*************************************************************************
  > File Name: SIPparser.h
  > Author: cooperz
  > Mail: zbzcsn@qq.com
  > Created Time: Mon 13 Jul 2015 09:47:43 AM CST
 ************************************************************************/

#ifndef _SIPPARSER_H
#define _SIPPARSER_H

#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <osipparser2/osip_message.h>
#include <osipparser2/osip_parser.h>

#include "SIPUtil.h"

namespace svss
{
    namespace SIP
    {
        #ifdef GTEST
        #endif
        class SIPParser
        {
            public:
                SIPParser();
                osip_message_t* parser( char* meg, size_t meglen);
                std::string getBranchNum( osip_message_t* meg,
                        std::string branch_ip);
                std::string getDialogId( osip_message_t* meg);
                std::string getFromTag( osip_message_t* meg);
                std::string getCallId( osip_message_t* meg);
                std::string getCallHost( osip_message_t* msg);
                std::string getToTag( osip_message_t* meg);
                std::string getFromUri( char* msg, size_t len);
                std::string getXMLFromMsg( char* msg, size_t len);
                bool GetPlayBackIPPORT( char* msg, size_t len,
                        std::string &camera_dev_id,
                        std::string &remote_ip, std::string &remote_port,
                        std::string &playback_start_time,
                        std::string &playback_end_time,
                        std::string &dialog_id);
                virtual ~SIPParser();
                bool MayCameraInfo(SIP_IN char* msg, SIP_IN size_t len,
                        SIP_OUT int* state, 
                        SIP_OUT std::string& camera_id);
        };
    }
}

#endif
