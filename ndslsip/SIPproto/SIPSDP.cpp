/*************************************************************************
	> File Name: SIPSDP.cpp
	> Author: cooperz
	> Mail: zbzcsn@qq.com
	> Created Time: Fri 24 Jul 2015 09:19:24 AM CST
 ************************************************************************/
#include "SIPSDP.h"

#include<iostream>

using namespace std;

namespace svss
{
    namespace SIP
    {
        SDP::SDP( string local_dev_name, string uac_ip)
        {
            _local_dev_name_ = local_dev_name;
            _uac_ip_ = uac_ip;
        } 

        string SDP::toString( string s_type, string dev_id_been_invited, string recver_vedio_serial_num,
                string recv_port)
        {
            string strMsg = string("v=0\r\n") 
                + "o=" + dev_id_been_invited + " 0 0 IN IP4 " + _uac_ip_ +"\r\n"         
                + "s="+ s_type +"\r\n"                                              
                + "u=" + dev_id_been_invited + ":" + recver_vedio_serial_num + "\r\n"
                + "c=IN IP4 " + _uac_ip_ + "\r\n"    
                + "m=video "+ recv_port +" RTP/AVP 96 98 97\r\n"                         
                + "a=recvonly\r\n"                                                     
                + "a=rtpmap:96 H264/90000\r\n"                                  
                + "a=rtpmap:98 H264/90000\r\n"                                      
                + "y=0999999999\r\n"                                                  
                + "f=\r\n"; 
            return strMsg;
        }

        string SDP::toString( string s_type, string recv_port)
        {
            string strMsg = string("v=0\r\n") 
                + "o=" + _local_dev_name_  + " 0 0 IN IP4 " + _uac_ip_ +"\r\n"         
                + "s="+ s_type +"\r\n"                                              
                + "c=IN IP4 " + _uac_ip_ + "\r\n"    
                + "m=video "+ recv_port +" RTP/AVP 96 98 97\r\n"                         
                + "a=recvonly\r\n"                                                     
                + "a=rtpmap:96 H264/90000\r\n"                                  
                + "a=rtpmap:98 H264/90000\r\n"                                      
                + "y=0999999999\r\n"                                                  
                + "f=\r\n"; 
            return strMsg;
        }

        SDP::~SDP()
        {

        }
    }
}

