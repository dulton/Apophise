/*************************************************************************
	> File Name: SIPStutaCode.h
	> Author: cooperz
	> Mail: zbzcsn@qq.com
	> Created Time: Thu 23 Jul 2015 03:43:16 PM CST
 ************************************************************************/

#ifndef _SIPSTATUCODE_H
#define _SIPSTATUCODE_H

typedef int SIP_STATE_CODE;

#define SIP_SUCCESS 1
#define SIP_CONTINUE 0

#define SIP_MANAGER_INIT_ERR -1
#define SIP_REGISTER_ERR -2
#define SIP_INVITE_STORE_ERR -3

#define SIP_LOGIN_NONE 11
#define SIP_LOGIN_ING 12
#define SIP_LOGIN_OK 13
#define SIP_PLAYBACK_RECVED 14
#define SIP_CAMERA_INFO 15

#define SIP_CORE_ERR -9
#define SIP_OUT_RANGE -10
#define SIP_MSG_BEEN_DROP -11

#endif
