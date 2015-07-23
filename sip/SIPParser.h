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
#include<eXosip2/eXosip.h>
#include<osipparser2/osip_message.h>

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
                std::string getBranchNum( osip_message_t* meg);
                std::string getDialogId( osip_message_t* meg);
                std::string getToTag( osip_message_t* meg);
        };
    }
}

#endif
