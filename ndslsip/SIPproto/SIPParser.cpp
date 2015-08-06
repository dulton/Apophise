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
            osip_from_to_str( osip_msg->to, &from_tag_c);
            string from_header( from_tag_c);
            int pos = from_header.find( "tag=", 0 ); 
            string from_Uri_num = from_header.substr( 0, pos);
            osip_message_free( osip_msg);
            return from_Uri_num;
        }

        string SIPParser::getXMLFromMsg( char* msg, size_t len)
        {
            string content_xml;
            osip_message_t* osip_msg = NULL;
            osip_message_init( &osip_msg);
            osip_message_parse( osip_msg, msg, len);
            osip_body_t* xml_body;
            osip_body_init( &xml_body);
            osip_message_get_body( osip_msg, 0, &xml_body);
            content_xml = xml_body->body;
            osip_message_free(osip_msg);
            return content_xml;
        }

        bool SIPParser::GetPlayBackIPPORT( char* msg, size_t len,
                string &remote_ip, string &remote_port, 
                string &playback_start_time, string &playback_end_time)
        {
            string content_sdp;
            osip_message_t* osip_msg = NULL;
            osip_message_init( &osip_msg);
            osip_message_parse( osip_msg, msg, len);
            osip_body_t* sdp_body;
            osip_body_init( &sdp_body);
            osip_message_get_body( osip_msg, 0, &sdp_body);
            content_sdp = sdp_body->body;
            osip_message_free(osip_msg);
            size_t sdp_len = content_sdp.length();
            size_t playback_pos = content_sdp.find("Playback");
            if(playback_pos == std::string::npos)
            {
                return false;
            }
            size_t ip_pos = content_sdp.find("c=IN");
            if(ip_pos == std::string::npos)
            {
                return false;
            }
            size_t i = 0;
            for( ; (ip_pos + i) < sdp_len ;i++)
            {
                if( '.' == content_sdp.at( ip_pos + i))
                {
                    break;   
                }
            }
            if( i == (sdp_len -1 - ip_pos))
                return false;
            size_t ip_start_pos = ip_pos + i;
            while( ' ' != content_sdp.at(ip_start_pos))
            {
                ip_start_pos--;
            }
            if( ip_start_pos < ip_pos)
                return false;
            ip_start_pos++;
            size_t ip_end_pos = ip_pos + i;
            /*\n换行和\r\n换行都要被支持解析*/
            while( ('\r'==content_sdp.at(ip_end_pos) || 
                        '\n'==content_sdp.at(ip_end_pos)))
            {
                ip_end_pos++;
            }
            if(ip_end_pos >= sdp_len)
                return false;
            ip_end_pos--;
            remote_ip = content_sdp.substr( ip_start_pos, ip_end_pos); 

            size_t port_pos = content_sdp.find("m=video");
            size_t j = port_pos + 6;
            for(; j < sdp_len - 1; j++)
            {
                if(' ' == content_sdp.at(j))
                    if( ' '!=content_sdp.at(j+1))
                        break;
            }
            if( j == (sdp_len - 2))
                return false;
            size_t port_start_pos = j+1;
            j++;
            while( ' '!= content_sdp.at(j))
            {
                j++;
                if( j >= sdp_len)
                    return false;
            }
            size_t port_end_pos = j;
            remote_port = content_sdp.substr( port_start_pos, port_end_pos);
            
            size_t time_pos = content_sdp.find("t=");
            char temp = content_sdp.at((time_pos+2));
            if( temp < 48 || 57 < temp)
                return false;
            size_t timestart_start_pos = time_pos+2;
            size_t n = time_pos+2;
            while( ' ' != content_sdp.at(n))
            {
                n++;
                if( n >= sdp_len)
                    return false;
            }
            size_t timestart_end_pos = n-1;
            playback_start_time = content_sdp.substr( timestart_start_pos, 
                    timestart_end_pos);
            n = n + 1;
            temp = content_sdp.at((n));
            if( temp < 48 || 57 < temp)
                return false;
            size_t timeend_start_pos = n-1;
            while( '\r' == content_sdp.at(n) || '\n' == content_sdp.at(n))
            {
                n++;
                if( n >= sdp_len)
                    return false;
            }
            size_t timeend_end_pos = n-1;
            playback_end_time = content_sdp.substr( timeend_start_pos,
                    timeend_end_pos);
            return true;
        }
        SIPParser::~SIPParser()
        {

        }
    }
}
