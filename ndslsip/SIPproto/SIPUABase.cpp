/*************************************************************************
	> File Name: SIPUABase.cpp
	> Author: cooperz
	> Mail: zbzcsn@qq.com
	> Created Time: Thu 23 Jul 2015 02:36:36 PM CST
 ************************************************************************/
#include "SIPUABase.h"
#include "SIPStatuCode.h"

#include<iostream>
using namespace std;

namespace svss
{
    namespace SIP
    {       
        SIPUABase::SIPUABase( string local_dev_name,
                string local_ip,
                string local_port,
                string passwd
                ):_manager_( local_dev_name,
                    local_ip,
                    local_port,
                    passwd)
        {
            _contact_id_ = -1;
            _ua_task_id_ = 1;
            _status_code_= SIP_LOGIN_NONE;
        }
        SIPUABase::~SIPUABase()
        {

        }

        int SIPUABase::Init()
        {
            bool init = _manager_.Init();
            if( init)
            {
                return SIP_SUCCESS;
            }else
            {
                return SIP_MANAGER_INIT_ERR;
            }
        }

        int SIPUABase::Register( char**rtmsg, 
                size_t* rtlen,
                string remote_name,
                string remote_ip,
                string remote_port,
                string passwd)
        {
            int state = 0;
            int tid = _ua_task_id_;
            _manager_.Register( tid, rtmsg, rtlen, &state, &_contact_id_, 
                    remote_name,remote_ip, remote_port, passwd);
            if( 0 == state)
            {
                _status_code_ = SIP_LOGIN_ING;
                return SIP_SUCCESS;
            }else{
                return SIP_REGISTER_ERR;
            }
        }

        int SIPUABase::Unregister()
        {
            return 1;
        }
    }
}
