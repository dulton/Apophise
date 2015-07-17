/*************************************************************************
  > File Name: SIPBuilder.cpp
  > Author: cooperz
  > Mail: zbzcsn@qq.com
  > Created Time: Mon 13 Jul 2015 09:48:29 AM CST
 ************************************************************************/
#include"SIPBuilder.h"
#include<iostream>
#include<sstream>
#include<stdlib.h>
#include<cstring>
#include<stdlib.h>
#include<string.h>

#include"md5.h"


using namespace std;

namespace svss
{
    namespace SIP
    {
#ifdef GTEST

#endif
        SIPBuilder::SIPBuilder( string dev_name , string local_ip, string local_port)
        {
            _dev_name_ = dev_name;
            _local_ip_str_ = local_ip;
            _local_port_str_ = local_port;
        }
        void SIPBuilder::Register( char** meg, size_t* len, int* state,
                string &callid,
                int *rid,
                string uas_ip ,
                string uas_listen_port_str ,
                string local_dev_passwd_str
                )
        {
            string uac_ip = _local_ip_str_;
            string uac_listen_port_str = _local_port_str_;
            string local_dev_name = _dev_name_;
            string protocol = "UDP";

            string request_line;
            stringstream request_stream;
            request_stream<<"REGISTER sip:"<<local_dev_name<<"@"<< uas_ip<<":"<< uas_listen_port_str<<" SIP/2.0\r\n";
            request_line = request_stream.str();

            string via_header;
            string randnum = "1008710087";
            via_header = "Via: SIP/2.0/"+protocol+" "+uac_ip+":"+uac_listen_port_str+";rport;branch=z9hG4bK"+randnum+"\r\n";

            string to_header;
            stringstream stream_to_header;
            stream_to_header << "From: <sip:" << local_dev_name << "@" << uas_ip << ":" << uas_listen_port_str<<">\r\n";
            to_header = stream_to_header.str();

            string from_header;
            string from_tag = string("tag=1008610086");
            stringstream stream_from_header;
            stream_from_header<< "To: <sip:"<< local_dev_name <<"@"<<uas_ip << ":" << uas_listen_port_str<<">;"<< from_tag<<"\r\n";
            from_header = stream_from_header.str();

            string call_id_header = string("Call-ID: 61070442\r\n");
            string cseq_header = string("CSeq: 1 REGISTER\r\n");

            string contact_header;
            stringstream stream_contact_header;
            stream_contact_header<<"Contact: <sip:" << local_dev_name << "@" << uac_ip << ":"<< uac_listen_port_str<<">\r\n";
            contact_header = stream_contact_header.str();

            string forwords = string("Max-Forwards: 70\r\n");
            string useragent = string("User-Agent: eXosip/4.1.0\r\n");
            string expires = string("Expires: 3000\r\n");
            string contentlenth = string("Content-Length: 0\r\n");
            string cflr = string("\r\n");
#ifdef DEBUG
            cout<< request_line <<endl;
            cout<< via_header <<endl;
            cout<< to_header <<endl;
            cout<< from_header <<endl;
            cout<<contact_header<<endl;
            cout<<forwords<<endl;
            cout<<useragent<<endl;
            cout<<expires<<endl;
            cout<<contentlenth<<endl;
            cout<<cflr<<endl;
#endif

            string sip_msg_str = request_line + via_header + to_header + from_header
                + call_id_header + cseq_header  + contact_header + forwords + 
                useragent + expires + contentlenth + cflr;
            size_t sip_len = sip_msg_str.length();
            char* sip_msg_c = (char*)malloc(sizeof(char)* sip_len);
            memcpy( sip_msg_c, sip_msg_str.c_str(), sip_len);
            *meg = sip_msg_c;
            *len = sip_len; 
            callid = call_id_header;
            *rid = 1;
            *state = 1;
            return;
        }

        void SIPBuilder::InviteLivePlay( char** meg, size_t* len, 
                int* state,
                string &call_id,
                string remote_dev_name,
                string uas_ip,
                string uas_listen_port_str,
                string sender_vedio_serial_num,
                string recver_vedio_serial_num
                )
        {
            string uac_ip = _local_ip_str_;
            string uac_listen_port_str = _local_port_str_;
            string local_dev_name = _dev_name_;

            string to_header;
            stringstream stream_to_header;
            stream_to_header << "sip:" << remote_dev_name << "@" << uas_ip << ":" << uas_listen_port_str;
            to_header = stream_to_header.str();

            string from_header;
            stringstream stream_from_header;
            stream_from_header<<"sip:" << local_dev_name << "@" << uac_ip << ":" << uac_listen_port_str;
            from_header = stream_from_header.str();

            string subject;
            subject = remote_dev_name + ":" + sender_vedio_serial_num 
                + local_dev_name + ":" + recver_vedio_serial_num + "\r\n";

            osip_message_t *invite = NULL;
            int ret = 0;
            if( ret < 0 )
            {
                *state = -1;
                return;
            }

            string strMsg = string("v=0\r\n") 
                //+ "o=34020000001140000001 0 0 IN IP4 127.0.0.1\r\n"                        
                + "o=" + local_dev_name  + " 0 0 IN IP4 " + uac_ip +"\r\n"                        
                + "s=Play\r\n"                                                         
                //+ "u=34020000002020000001:1\r\n"                                           
                "u=" + local_dev_name + ":" + recver_vedio_serial_num + "\r\n"
                //+ "c=IN IP4 127.0.0.1\r\n"                                                 
                + "c=IN IP4 " + uac_ip + "\r\n"                                                 
                //+ "t=1288625085 1288625871\r\n"                                            
                + "m=video 6000 RTP/AVP 96 98 97\r\n"                                      
                + "a=recvonly\r\n"                                                         
                + "a=rtpmap:96 H264/90000\r\n"                                             
                + "a=rtpmap:98 H264/90000\r\n"                                             
                + "y=0999999999\r\n"                                                       
                + "f=\r\n"; 
            ret = ::osip_message_set_body( invite, strMsg.c_str(), strMsg.length());
            if( ret < 0)
            {
                *state = -1;
                return;
            }
            ret = ::osip_message_set_content_type (invite, "APPLICATION/SDP");
            if( ret < 0)
            {
                *state = -1;
                return;
            }
            call_id = string( invite->call_id->number);
            ret = ::osip_message_to_str( invite, meg, len);
            if( ret < 0)
            {
                *state = - 1;
                return ;
            }
            *state = 1;
            return;
        }
        void SIPBuilder::InviteACK( osip_message_t* msg, char** rtmeg , size_t *rtlen,
                int* state)
        {//todo:小心内存泄漏
            stringstream request_line;
            osip_message_t* re_osip;
            osip_message_clone( msg, &re_osip);
            osip_message_free( msg);
            char* ack = new char[4];
            ack[0]='A';
            ack[1]='C';
            ack[2]='K';
            ack[3]='\0';
            if(re_osip->sip_method != NULL)
            {
                free(re_osip->sip_method);
            }
            osip_message_set_method( re_osip, ack);
            char* cseq_value = new char[4];
            cseq_value[0]='A';
            cseq_value[1]='C';
            cseq_value[2]='K';
            cseq_value[3]='\0';
            if(re_osip->cseq->method != NULL)
            {
                free(re_osip->cseq->method);
            }
            osip_cseq_set_method( re_osip->cseq, cseq_value );

            if( re_osip->reason_phrase != NULL)
            {
                cout<<"reason_phrase"<<re_osip->reason_phrase<<endl;
                free( re_osip->reason_phrase);
            }
            stringstream reason_phrase;
            reason_phrase<<"sip:"<<_dev_name_<<"@"<<_local_ip_str_<<":"
                <<_local_port_str_;
            string reason = reason_phrase.str();
            char* reason_phrase_pchar = (char*)malloc( reason.length() + 1);
            memcpy( reason_phrase_pchar, reason.c_str(), reason.length()+1);
            osip_message_set_reason_phrase( re_osip, reason_phrase_pchar); 

            if( re_osip->status_code !=0 )
            {
                re_osip->status_code = 0;
            }

            osip_message_to_str( re_osip, rtmeg, rtlen);
#ifdef DEBUG
            cout<<"invite ack meg'string :"<<*rtmeg << endl;
#endif
            *state = 1;
            return;
        }
        void SIPBuilder::AuRegister( osip_message_t* msg, char** rtmeg, size_t* rtlen)
        {
            int pos = 0;
            string realm;
            string nonce;
            if( NULL != msg)
            {
                while (!osip_list_eol (&msg->www_authenticates, pos))
                {
                    osip_www_authenticate_t* auth;
                    auth = (osip_www_authenticate_t *) osip_list_get (&msg->www_authenticates, pos);
                    realm = string(auth->realm);
                    nonce = string(auth->nonce);
                    break;
                }
            }
            
            return;
        }
        string SIPBuilder::_RegisterMd5( string username, string realm, string passwd,
                string uri, string nonce)
        {
            string rtresp_md5;
            MD5 md5;                                                                       
            md5.update(username);                                                          
            md5.update(":");                                                               
            md5.update(realm);                                                             
            md5.update(":");                                                               
            md5.update(passwd);                                                            
            string hash1 = md5.toString();                                                 
            MD5 md5_2;                                                                     
            md5_2.update("REGISTER");                                                      
            md5_2.update(":");                                                             
            md5_2.update(uri);                                                             
            string hash3 = md5_2.toString();                                               
            MD5 md5_3;                                                                     
            md5_3.update(hash1);                                                           
            md5_3.update(":");                                                             
            string non( nonce);                                                            
            md5_3.update(non);                                                             
            md5_3.update(":");                                                             
            md5_3.update(hash3);                                                           
            rtresp_md5 = md5_3.toString();                                                        
            return rtresp_md5;
        }
    }
}
