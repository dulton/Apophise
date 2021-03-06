/*************************************************************************
	> File Name: SIPUtil.h
	> Author: cooperz
	> Mail: zbzcsn@qq.com
	> Created Time: Mon 13 Jul 2015 04:27:10 PM CST
 ************************************************************************/

#ifndef _SIPUTIL_H
#define _SIPUTIL_H

#include<iostream>

#define DEBUG 1
//#define GTEST 1
#define SIP_OUT 
#define SIP_IN 

#define LOCAL_DEV_NAME ("34020000001320000001") //400客户端
#define LOCAL_MED_NAME ("34020000002020000001") //202媒体服务器
#define SIP_SERVER_NAME ("34020000002000000001") //SIP服务器
#define LOCAL_DEV_PASSWD_STR ("111111") 
#define REMOTE_DEV_NAME ("34020000002000000001") //1号摄像机
#define SENDER_VEDIO_SERIAL_NUM_STR ("1")
#define RECVER_VEDIO_SERIAL_NUM_STR ("1")

#define UAC_IP ("192.168.14.101")
#define UAC_LISTEN_PORT_STR ("5059")
#define UAC_LISTEN_PORT_INT (5059)

#define UA_ME_IP ("192.168.14.101")
#define UA_ME_PORT_STR ("5065")
#define UA_ME_PORT_INT (5065)

#define UAS_IP ("192.168.14.101")
#define UAS_LISTEN_PORT_STR ("5060")
#define UAS_LISTEN_PORT_INT (5060)

#define DS_DEV_NAME ("34020000002780000001")
#define DS_DEV_PASSWD_STR ("111111")
#define DS_PORT_STR ("5059")

#define MS_DEV_NAME ("34020000002860000001")
#define MS_DEV_PASSWD_STR ("111111")
#define MS_PORT_STR ("5058")

#define SPMVN_DEV_NAME ("34020000001320000001")
#define SPMVN_DEV_PASSWD_STR ("111111")
#define SPMVN_PORT_STR ("5057")

#define IPC_DEV_NAME ("34020000001310000001") //400客户端
#endif
