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

        string SIPParser::getBranchNum( osip_message_t* meg)
        {
            string via_branch_num;
            osip_via_t *via;
            char* via_c = NULL; 
            if( !osip_list_eol (&meg->vias, 0))
            {
                via = (osip_via_t *) osip_list_get (&meg->vias, 0);
                osip_via_to_str( via, &via_c);
            }else{
                return via_branch_num;
            }
            string via_header(via_c);
            int pos = via_header.find( "branch", 0 );
            if(pos<=0)
            {
                return via_branch_num;
            }
            int length = via_header.length();
            via_branch_num = via_header.substr( (pos+14), length);
            return via_branch_num;
        }

        string SIPParser::getDialogId( osip_message_t* msg)
        {
            char* from_tag_c;
            osip_from_to_str( msg->from, &from_tag_c );
            string from_header(from_tag_c);
            int pos = from_header.find( "tag=", 0 ); 
            int length = from_header.length();
            string from_tag_num = from_header.substr( (pos+4), length);
            string call_id_num( msg->call_id->number);
            return from_tag_num + call_id_num;
        }
    }
}
