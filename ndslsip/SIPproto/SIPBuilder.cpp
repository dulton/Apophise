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


#include"SIPMD5.h"

using namespace std;

namespace svss
{
    namespace SIP
    {
#ifdef GTEST

#endif
        SIPBuilder::SIPBuilder( string dev_name , string local_ip, string local_port)
            :_sdp_builder_( dev_name, local_ip)
        {
            srand((unsigned int)time(NULL));
            _dev_name_ = dev_name;
            _local_ip_str_ = local_ip;
            _local_port_str_ = local_port;
        }
        void SIPBuilder::Register( char** meg, size_t* len, int* state,
                struct DialogInfo &dlg_info,
                string &via_branch,
                string uas_dev_name,
                string uas_ip ,
                string uas_listen_port_str
                )
        {
            string uac_ip = _local_ip_str_;
            string uac_listen_port_str = _local_port_str_;
            string local_dev_name = _dev_name_;
            string protocol = "UDP";

            string request_line;
            stringstream request_stream;
            request_stream<<"REGISTER sip:"<< uas_dev_name<<"@"<< uas_ip<<":"<< uas_listen_port_str<<" SIP/2.0\r\n";
            request_line = request_stream.str();

            string via_header;
            via_branch = _RandomNum();
            via_header = "Via: SIP/2.0/"+protocol+" "+uac_ip+":"+uac_listen_port_str+";rport;branch=z9hG4bK"+ via_branch+"\r\n";

            string to_header;
            stringstream stream_to_header;
            stream_to_header << "To: <sip:" << local_dev_name << "@" << uac_ip << ":" << uac_listen_port_str<<">\r\n";
            to_header = stream_to_header.str();

            string from_header;
            dlg_info.from_tag_num = _RandomNum();
            string from_tag = string("tag=")+ dlg_info.from_tag_num;
            stringstream stream_from_header;
            stream_from_header<< "From: <sip:"<< local_dev_name <<"@"<<uac_ip << ":" << uac_listen_port_str<<">;"<< from_tag<<"\r\n";
            from_header = stream_from_header.str();

            dlg_info.call_id_num = _RandomNum();
            string call_id_header = string("Call-ID: ")+ dlg_info.call_id_num + "\r\n";
            string cseq_header = string("CSeq: 1 REGISTER\r\n");

            dlg_info.dailog_id = dlg_info.from_tag_num + dlg_info.call_id_num;

            string contact_header;
            stringstream stream_contact_header;
            stream_contact_header<<"Contact: <sip:" << local_dev_name << "@" << uac_ip << ":"<< uac_listen_port_str<<">\r\n";
            contact_header = stream_contact_header.str();

            string forwords = string("Max-Forwards: 70\r\n");
            string expires = string("Expires: 3000\r\n");
            string contentlenth = string("Content-Length: 0\r\n");
            string cflr = string("\r\n");
            string sip_msg_str = request_line + via_header + to_header + from_header
                + call_id_header + cseq_header  + contact_header + forwords + 
                 expires + contentlenth + cflr;
#ifdef DEBUG
cout<<"Register msg :\n"<<sip_msg_str<<endl;
#endif
            size_t sip_len = sip_msg_str.length();
            char* sip_msg_c = (char*)malloc(sizeof(char)* sip_len);
            memcpy( sip_msg_c, sip_msg_str.c_str(), sip_len);
            *meg = sip_msg_c;
            *len = sip_len; 
            *state = 1;
            return;
        }

        void SIPBuilder::InviteLivePlay( char** rtmeg, size_t* rtlen, 
                int* state,
                string recv_port,
                struct DialogInfo &dlg_info,
                string &via_branch,
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

            string sdp_msg = _sdp_builder_.toString( string("Play"), recver_vedio_serial_num, recv_port);
            /*
            string strMsg = string("v=0\r\n") 
                + "o=" + local_dev_name  + " 0 0 IN IP4 " + uac_ip +"\r\n"         
                + "s=Play\r\n"                                                         
                + "u=" + local_dev_name + ":" + recver_vedio_serial_num + "\r\n"
                + "c=IN IP4 " + uac_ip + "\r\n"                                          
                + "m=video "+ recv_port +" RTP/AVP 96 98 97\r\n"                         
                + "a=recvonly\r\n"                                                         
                + "a=rtpmap:96 H264/90000\r\n"                                             
                + "a=rtpmap:98 H264/90000\r\n"                                             
                + "y=0999999999\r\n"                                                       
                + "f=\r\n"; 
            */
            string call_id_num = _RandomNum();
            dlg_info.call_id_num = call_id_num;
            string from_tag_num = _RandomNum();
            dlg_info.from_tag_num = from_tag_num;
            dlg_info.dailog_id = from_tag_num + call_id_num;
            via_branch = _RandomNum();

            stringstream sdp_length;
            sdp_length<<sdp_msg.length();
            string request_line = "INVITE sip:"+ local_dev_name +"@"+ uas_ip +":" +uas_listen_port_str+" SIP/2.0" + "\r\n";
            string via_header = "Via: SIP/2.0/UDP "+ uac_ip +":"+_local_port_str_+";rport;branch=z9hG4bK"+ via_branch +"\r\n";
            string from_header = "From: <sip:"+ local_dev_name + "@" + uac_ip + ":" + uac_listen_port_str +">;tag="+ from_tag_num + "\r\n";
            string to_header = "To: <sip:"+ remote_dev_name + "@" + uas_ip + ":" + uas_listen_port_str+">\r\n";
            string call_header = "Call-ID: "+call_id_num+"\r\n";
            string cseq_header ="CSeq: 20 INVITE\r\n";
            string contact_heaer = "Contact: <sip:"+ local_dev_name + "@" + uac_ip + ":" + uac_listen_port_str +">"+"\r\n";
            string content_type_header = "Content-Type: APPLICATION/SDP\r\n";
            string content_lenth = "Content-Length: "+ sdp_length.str()+"\r\n";
            string forwords = string("Max-Forwards: 70\r\n");
            string expires = string("Expires: 3000\r\n");
            string cflr = string("\r\n");
            string subject;
            subject = remote_dev_name + ":" + sender_vedio_serial_num 
                + local_dev_name + ":" + recver_vedio_serial_num + "\r\n";

            string sip_msg_str = request_line + via_header+from_header
                +to_header +call_header+ cseq_header+
                contact_heaer + content_type_header +content_lenth +
                forwords + expires +cflr+ sdp_msg;

            size_t sip_len = sip_msg_str.length();
            char* sip_msg_c = (char*)malloc(sizeof(char)* sip_len);
            memcpy( sip_msg_c, sip_msg_str.c_str(), sip_len);
            *rtmeg = sip_msg_c;
            *rtlen = sip_len; 
            *state = 0 ;
#ifdef DEBUG
            cout<<"Invite play  msg :\n"<<sip_msg_str<<endl;
#endif
            return;
        }
        void SIPBuilder::InviteACK( osip_message_t* msg, char** rtmeg , size_t *rtlen,
                int* state)
        {//todo:小心内存泄漏
            int pos = 0;

            string request_line = "ACK sip:"+ _dev_name_ + "@" + _local_ip_str_ 
                + ":" + _local_port_str_ + " SIP/2.0" +"\r\n";
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
            cout<<"invite ack meg'string :\n"<<*rtmeg << endl;
#endif
            *state = 1;
            return;
        }

        void SIPBuilder::AuRegister( osip_message_t* msg, char** rtmeg, size_t* rtlen,
               std::string &via_branch_num, struct DialogInfo dlg_info,
               struct ReAuthInfo re_au)
        {
            string uas_ip = re_au.uas_ip;
            string uas_listen_port_str = re_au.uas_port_str;
            string local_dev_passwd_str = re_au.passwd;
            string remote_dev_name = re_au.remote_dev_name;
            string from_tag_num = dlg_info.from_tag_num;
            string call_id_num = dlg_info.call_id_num;
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
            string randnum = _RandomNum();
            via_branch_num = randnum;
            via_header = "Via: SIP/2.0/"+protocol+" "+uac_ip+":"+uac_listen_port_str+";rport;branch=z9hG4bK"+randnum+"\r\n";

            string to_header;
            stringstream stream_to_header;
            stream_to_header << "To: <sip:" << local_dev_name << "@" << uas_ip << ":" << uas_listen_port_str<<">\r\n";
            to_header = stream_to_header.str();

            string from_header;
            string from_tag = string("tag=") + from_tag_num;
            stringstream stream_from_header;
            stream_from_header<< "From: <sip:"<< local_dev_name <<"@"<<uas_ip << ":" << uas_listen_port_str<<">;"<< from_tag<<"\r\n";
            from_header = stream_from_header.str();

            string call_id_header = string("Call-ID: " + call_id_num + "\r\n");
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
            string expires = string("Expires: 3000\r\n");
            string contentlenth = string("Content-Length: 0\r\n");
            string cflr = string("\r\n");

            string sip_msg_str = request_line + via_header + to_header + from_header
                + call_id_header + cseq_header  + contact_header +au_header +  forwords 
                + expires + contentlenth + cflr;
#ifdef DEBUG
            cout<<"check Re Au:"<<endl;
            cout<<sip_msg_str<<endl;
#endif
            size_t sip_len = sip_msg_str.length();
            char* sip_msg_c = (char*)malloc(sizeof(char)* sip_len);
            memcpy( sip_msg_c, sip_msg_str.c_str(), sip_len);
            *rtmeg = sip_msg_c;
            *rtlen = sip_len;

            return;
        }
        
        void SIPBuilder::Bye( char** rtmeg, size_t *rtlen, int*state , 
                string &via_branch_num, struct DialogInfo dig_info, 
                struct ReAuthInfo re_info)
        {
            string uac_ip = _local_ip_str_;
            string uac_listen_port_str = _local_port_str_;
            string local_dev_name = _dev_name_;
            string uas_ip = re_info.uas_ip;
            string uas_listen_port_str = re_info.uas_port_str;
            string remote_dev_name = re_info.remote_dev_name;
            string from_tag_num = dig_info.from_tag_num;
            string to_tag_num = dig_info.to_tag_num;
            string call_id_num = dig_info.call_id_num;

            via_branch_num = _RandomNum();

            string request_line = "BYE sip:"+ local_dev_name +"@"+ uas_ip +":" +uas_listen_port_str+" SIP/2.0" + "\r\n";
            string via_header = "Via: SIP/2.0/UDP "+ uac_ip +":"+ uac_listen_port_str + ";rport;branch=z9hG4bK" + via_branch_num +"\r\n";
            string from_heaer = "From: <sip:"+ local_dev_name + "@" + uac_ip + ":" + uac_listen_port_str +">;tag="+ from_tag_num + "\r\n";
            string to_header = "To: <sip:"+ remote_dev_name + "@" + uas_ip + ":" + uas_listen_port_str+">;tag="+ to_tag_num +"\r\n";
            string call_header = "Call-ID: "+call_id_num+"\r\n";
            string cseq_header ="CSeq: 21 BYE\r\n";
            string contact_heaer = "Contact: <sip:"+ local_dev_name + "@" + uac_ip + ":" + uac_listen_port_str +">"+"\r\n";
            string content_lenth = string("Content-Length: 0")+"\r\n";
            string forwords = string("Max-Forwards: 70\r\n");
            string expires = string("Expires: 3000\r\n");
            string cflr = string("\r\n");

            string sip_msg_str = request_line + via_header + from_heaer
                + to_header + call_header + cseq_header +
                contact_heaer + content_lenth +
                forwords + expires +cflr;
            size_t sip_len = sip_msg_str.length();
            char* sip_msg_c = (char*)malloc(sizeof(char)* sip_len);
            memcpy( sip_msg_c, sip_msg_str.c_str(), sip_len);
            *rtmeg = sip_msg_c;
            *rtlen = sip_len; 
            *state = 0 ;
#ifdef DEBUG
            cout<<"check Bye:"<<endl;
            cout<<sip_msg_str<<endl;
#endif
            return;
        }
        void SIPBuilder::BeenInvited( osip_message_t* msg, string port,char** rtmsg,
                size_t *rtlen, int*state, struct DialogInfo &dlg_info)
        {
            string uac_ip = _local_ip_str_;
            string uac_listen_port_str = _local_port_str_;
            string local_dev_name = _dev_name_;

            string head_line("SIP/2.0 200 OK\r\n");

            osip_via_t *via;
            char* via_c = NULL; 
            if( !osip_list_eol (&msg->vias, 0))
            {
                via = (osip_via_t *) osip_list_get (&msg->vias, 0);
                osip_via_to_str( via, &via_c);
            }else{
                *state = -1;
                return;
            }
            string via_header(via_c);
            via_header = string("Via: ")+via_header+string("\r\n");
            
            char* from_tag_c;
            osip_from_to_str( msg->from, &from_tag_c );
            string from_header(from_tag_c);
            from_header = string("From: ")+from_header+string("\r\n");

            char* to_tag_c;
            osip_to_to_str( msg->to, &to_tag_c );
            string to_header(to_tag_c);
            string to_tag_num = _RandomNum();
            to_header = to_header + ";tag="+to_tag_num;
            to_header = string("To: ")+to_header+string("\r\n");
            dlg_info.to_tag_num = to_tag_num;

            string contact_header;
            stringstream stream_contact_header;
            stream_contact_header<<"Contact: <sip:" << local_dev_name << "@" << uac_ip << ":"<< uac_listen_port_str<<">\r\n";
            contact_header = stream_contact_header.str();

            string call_id_num = string(msg->call_id->number);
            string call_header = string("Call-ID: ")+call_id_num+("\r\n");

            string cseq_num = string(msg->cseq->number);
            string cseq_header = string("Cseq: ")+cseq_num+string(" INVITE\r\n");
            string content_type_header = "Content-Type: APPLICATION/SDP\r\n";

            string forwords = string("Max-Forwards: 70\r\n");
            string expires = string("Expires: 3000\r\n");
            string sdp_msg = _sdp_builder_.toString( string("##2015"), port);
            stringstream sdp_msg_length; 
            sdp_msg_length<< sdp_msg.length();
            string contentlenth = string("Content-Length: ")+sdp_msg_length.str()+string("\r\n");
            string cflr = string("\r\n");

            string sip_msg_str = head_line + via_header + to_header + from_header
                + call_header + cseq_header  + contact_header  +  content_type_header + 
                forwords + expires + contentlenth + cflr + sdp_msg;
#ifdef DEBUG
            cout<<"check Been Invite:"<<endl;
            cout<<sip_msg_str<<endl;
#endif
            size_t sip_len = sip_msg_str.length();
            char* sip_msg_c = (char*)malloc(sizeof(char)* sip_len);
            memcpy( sip_msg_c, sip_msg_str.c_str(), sip_len);
            *rtmsg = sip_msg_c;
            *rtlen = sip_len;
            /*send 200ok, wait ack*/
            *state = 0;
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

        string SIPBuilder::_RandomNum()
        {
            int randnum = 0;
            stringstream rand_num_sstr;
            randnum = rand();
            rand_num_sstr<<randnum;
            return rand_num_sstr.str();
        }
        SIPBuilder::~SIPBuilder()
        {

        }
    }
}
