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
#include<osipparser2/headers/osip_via.h>

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
            stream_to_header << "To: <sip:" << local_dev_name << "@" << uas_ip << ":" << uas_listen_port_str<<">\r\n";
            to_header = stream_to_header.str();

            string from_header;
            string from_tag = string("tag=1008610086");
            stringstream stream_from_header;
            stream_from_header<< "From: <sip:"<< local_dev_name <<"@"<<uas_ip << ":" << uas_listen_port_str<<">;"<< from_tag<<"\r\n";
            from_header = stream_from_header.str();

            string call_id = "61070442";
            string call_id_header = string("Call-ID: ")+call_id+"\r\n";
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
            callid = call_id;
            *state = 1;
            return;
        }

        void SIPBuilder::InviteLivePlay( char** rtmeg, size_t* rtlen, 
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
            string call_id_num = "163324055";
            call_id = call_id_num;
            stringstream ss;
            ss<<strMsg.length();
            string request_line = "INVITE sip:"+ local_dev_name +"@"+ uas_ip +":" +uas_listen_port_str+" SIP/2.0" + "\r\n";
            string via_header = "Via: SIP/2.0/UDP "+ uac_ip +":"+_local_port_str_+";rport;branch=z9hG4bK162557620"+"\r\n";
            string from_heaer = "From: <sip:"+ local_dev_name + "@" + uac_ip + ":" + uac_listen_port_str +">;tag=1217605478"+"\r\n";
            string to_header = "To: <sip:"+ remote_dev_name + "@" + uas_ip + ":" + uas_listen_port_str+">\r\n";
            string call_header = "Call-ID: "+call_id_num+"\r\n";
            string cseq_header ="CSeq: 20 INVITE\r\n";
            string contact_heaer = "Contact: <sip:"+ local_dev_name + "@" + uac_ip + ":" + uac_listen_port_str +">"+"\r\n";
            string content_type_header = "Content-Type: APPLICATION/SDP\r\n";
            string content_lenth = "Content-Length: "+ ss.str()+"\r\n";
            string forwords = string("Max-Forwards: 70\r\n");
            string useragent = string("User-Agent: eXosip/4.1.0\r\n");
            string expires = string("Expires: 3000\r\n");
            string cflr = string("\r\n");
            string subject;
            subject = remote_dev_name + ":" + sender_vedio_serial_num 
                + local_dev_name + ":" + recver_vedio_serial_num + "\r\n";

            string sip_msg_str = request_line + via_header+from_heaer
                +to_header +call_header+ cseq_header+
                contact_heaer +content_type_header +content_lenth +
                forwords + useragent + expires +cflr+ strMsg;

            size_t sip_len = sip_msg_str.length();
            char* sip_msg_c = (char*)malloc(sizeof(char)* sip_len);
            memcpy( sip_msg_c, sip_msg_str.c_str(), sip_len);
            *rtmeg = sip_msg_c;
            *rtlen = sip_len; 
            *state = 0 ;
            return;
        }
        void SIPBuilder::InviteACK( osip_message_t* msg, char** rtmeg , size_t *rtlen,
                int* state)
        {//todo:小心内存泄漏
            int pos = 0;

            string request_line = "ACK sip:"+ _dev_name_ + "@" + _local_ip_str_ + ":" + _local_port_str_ + " SIP/2.0" +"\r\n";
            char* via_header_c;
            int ret = ::osip_via_to_str( (osip_via_t *) osip_list_get (&msg->vias, pos), &via_header_c);
            if( ret != 0 )
            {
                cout<<"via to str error"<<endl;
            }
            string via_header("Via: ");
            via_header = via_header + via_header_c + string("\r\n");

            char* from_header_c;
            ret = ::osip_from_to_str( msg->from, &from_header_c);
            if( ret != 0 )
            {
                cout<<"from to str error"<<endl;
            }
            string from_header("From: ");
            from_header = from_header + from_header_c+ string("\r\n");


            char* to_header_c;
            ret = ::osip_to_to_str( msg->to, &to_header_c);
            if( ret != 0 )
            {
                cout<<"to to str error"<<endl;
            }
            string to_header("To: ");
            to_header = to_header + to_header_c + string("\r\n");

            
            char* callid_header_c;
            ret = ::osip_call_id_to_str( msg->call_id, &callid_header_c);
            if( ret != 0 )
            {
                cout<<"to to str error"<<endl;
            }
            string callid_header("Call-ID: ");
            callid_header = callid_header + callid_header_c + string("\r\n");

            string contact_heaer = "Contact: <sip:"+ _dev_name_ + "@" + _local_ip_str_ + ":" + _local_port_str_ +">"+"\r\n";
            string cseq_header ="CSeq: 20 ACK\r\n";
            string forwords = string("Max-Forwards: 70\r\n");
            string useragent = string("User-Agent: eXosip/4.1.0\r\n");
            string expires = string("Expires: 3000\r\n");
            string cflr = string("\r\n");
            string sip_msg_str = request_line + via_header + from_header + to_header + callid_header
                + contact_heaer + cseq_header + forwords + expires + cflr;

            size_t sip_len = sip_msg_str.length();
            char* sip_msg_c = (char*)malloc(sizeof(char)* sip_len);
            memcpy( sip_msg_c, sip_msg_str.c_str(), sip_len);
            *rtmeg = sip_msg_c;
            *rtlen = sip_len; 
            *state = 0 ;



#ifdef DEBUG
            cout<<"invite ack meg'string :"<<*rtmeg << endl;
#endif
            *state = 1;
            return;
        }
        void SIPBuilder::AuRegister( osip_message_t* msg, char** rtmeg, size_t* rtlen,
                string uas_ip, string uas_listen_port_str, string local_dev_passwd_str)
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
            string quato("\"");

            string uac_ip = _local_ip_str_;
            string uac_listen_port_str = _local_port_str_;
            string local_dev_name = _dev_name_;
            string protocol = "UDP";

            string request_line;
            stringstream request_stream;
            request_stream<<"REGISTER sip:"<< local_dev_name<<"@"<< uas_ip<<":"<< uas_listen_port_str<<" SIP/2.0\r\n";
            request_line = request_stream.str();

            string via_header;
            string randnum = "1008718888";
            via_header = "Via: SIP/2.0/"+protocol+" "+uac_ip+":"+uac_listen_port_str+";rport;branch=z9hG4bK"+randnum+"\r\n";

            string to_header;
            stringstream stream_to_header;
            stream_to_header << "To: <sip:" << local_dev_name << "@" << uas_ip << ":" << uas_listen_port_str<<">\r\n";
            to_header = stream_to_header.str();

            string from_header;
            string from_tag = string("tag=1008610086");
            stringstream stream_from_header;
            stream_from_header<< "From: <sip:"<< local_dev_name <<"@"<<uas_ip << ":" << uas_listen_port_str<<">;"<< from_tag<<"\r\n";
            from_header = stream_from_header.str();

            string call_id_header = string("Call-ID: 61070442\r\n");
            string cseq_header = string("CSeq: 2 REGISTER\r\n");

            string contact_header;
            stringstream stream_contact_header;
            stream_contact_header<<"Contact: <sip:" << local_dev_name << "@" << uac_ip << ":"<< uac_listen_port_str<<">\r\n";
            contact_header = stream_contact_header.str();

            string uri = quato +"sip:"  + local_dev_name+"@"+uas_ip+":"+uas_listen_port_str + quato;
            string response = _RegisterMd5( local_dev_name, realm, local_dev_passwd_str, 
                    uri, nonce);

            string au_header;
            au_header = "Authorization: Digest username="+ quato + local_dev_name 
                + quato + ", realm="+ realm +", nonce=" +  nonce  
                + ", uri=" + uri + ", response="+ quato + 
                response + quato+", algorithm=MD5\r\n";
            string forwords = string("Max-Forwards: 70\r\n");
            string useragent = string("User-Agent: eXosip/4.1.0\r\n");
            string expires = string("Expires: 3000\r\n");
            string contentlenth = string("Content-Length: 0\r\n");
            string cflr = string("\r\n");

            string sip_msg_str = request_line + via_header + to_header + from_header
                + call_id_header + cseq_header  + contact_header +au_header +  forwords + 
                useragent + expires + contentlenth + cflr;
#ifdef DEBUG
            cout<<"check AuRe"<<endl;
            cout<<sip_msg_str<<endl;
#endif
            size_t sip_len = sip_msg_str.length();
            char* sip_msg_c = (char*)malloc(sizeof(char)* sip_len);
            memcpy( sip_msg_c, sip_msg_str.c_str(), sip_len);
            *rtmeg = sip_msg_c;
            *rtlen = sip_len; 

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
