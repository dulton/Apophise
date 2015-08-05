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

        string SIPParser::getBranchNum( osip_message_t* meg,
                std::string branch_ip)
        {
            string via_branch_num;
            osip_via_t *via;
            char* via_c = NULL;
            int pos = 0;
            while (!osip_list_eol ( &(meg->vias), pos))
            {
                via = (osip_via_t *) osip_list_get (&meg->vias, pos);
                osip_via_to_str( via, &via_c);
                string via_header(via_c);
                int ip_pos = via_header.find(branch_ip);
                if(ip_pos <= 0)
                {
                    continue;
                }else
                {
                    int branch_pos = via_header.find( string("branch") );
                    int string_len = via_header.length();
                    char temp = via_header.at(branch_pos+14);
                    int skep_len = 14;
                    while(';' !=  temp)
                    {
                        skep_len++;
                        if((branch_pos+skep_len)>=string_len)
                        {
                            break;
                        }
                        temp = via_header.at(branch_pos+skep_len);
                    }
                    if((branch_pos+skep_len)==string_len)
                    {
                        return via_branch_num = via_header.substr((branch_pos+14),
                                string_len);
                    }
                    if( ';'==temp)
                    {
                        return via_branch_num = via_header.substr((branch_pos+14),
                                (skep_len - 14));
                    }
                }
            }
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

        string SIPParser::getFromTag( osip_message_t* msg)
        {
            char* from_tag_c;
            osip_from_to_str( msg->from, &from_tag_c );
            string from_header(from_tag_c);
            int pos = from_header.find( "tag=", 0 ); 
            int length = from_header.length();
            string from_tag_num = from_header.substr( (pos+4), length);
            return from_tag_num;
        }

        string SIPParser::getCallId( osip_message_t* msg)
        {
            string call_id_num( msg->call_id->number);
            return call_id_num;
        }

        string SIPParser::getToTag( osip_message_t* msg)
        {
            char* to_tag_c;
            osip_to_to_str( msg->to, &to_tag_c );
            string to_header(to_tag_c);
            int pos = to_header.find( "tag=", 0 ); 
            int length = to_header.length();
            string to_tag_num = to_header.substr( (pos+4), length);
            return to_tag_num;
        }

        string SIPParser::getFromUri( char* msg, size_t len)
        {
            osip_message_t* osip_msg = NULL;
            osip_message_init( &osip_msg);
            ::osip_message_parse( osip_msg, msg, len);
            char* from_tag_c;
            osip_from_to_str( osip_msg->to, &from_tag_c );
            string from_header( from_tag_c);
            int pos = from_header.find( "tag=", 0 ); 
            string from_Uri_num = from_header.substr( 0, pos);
            return from_Uri_num;
        }

        SIPParser::~SIPParser()
        {

        }
    }
}
