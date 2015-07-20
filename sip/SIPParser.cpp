/*************************************************************************
	> File Name: SIPparser.cpp
	> Author: cooperz
	> Mail: zbzcsn@qq.com
	> Created Time: Mon 13 Jul 2015 09:48:02 AM CST
 ************************************************************************/
#include"SIPParser.h"

#include<iostream>

using namespace std;

namespace svss
{
    namespace SIP
    {
        SIPParser::SIPParser()
        {
            //attention:must init before used
            parser_init();
        }

        osip_message_t* SIPParser::parser( char* meg, size_t meglen)
        {
            osip_message_t* osipmeg = NULL;
            osip_message_init( &osipmeg);
            ::osip_message_parse( osipmeg, meg, meglen);
            return osipmeg;
        }
    }
}
