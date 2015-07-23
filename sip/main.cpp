/*************************************************************************
  > File Name: main.cpp
  > Author: cooperz
  > Mail: zbzcsn@qq.com
  > Created Time: Thu 16 Jul 2015 09:26:25 AM CST
  > this cpp is for test register and invite in the sip
 ************************************************************************/

#include <fcntl.h>                                                              
#include <unistd.h>                                                             
#include <iostream>                                                             
#include <sys/socket.h>                                                          
#include <netinet/in.h>                                                         
#include <arpa/inet.h>                                                          
#include <stdlib.h>   
#include <string.h>
#include <errno.h>

#include "SIPManager.h"
#include "SIPUtil.h"
using namespace std;

using namespace svss::SIP;

int main( int argn, char** argv)
{
    FILE* logfile = fopen( "logfileman.txt", "w");
    osip_trace_initialize( (_trace_level)8, logfile );
    struct sockaddr_in seraddr;                                                    
    struct sockaddr_in cliaddr;                                                    
    char buf[500];                                                              
    char rbuf[500];                                                             
    int udpfd;                                                               
    if( argn < 2)                                                               
    {                                                                           
        cout<<" Usage : ./test IP defualt PORT 5060\n"<<endl;                   
        return -1;                                                              
    }                                                                           
    if ( ( udpfd=socket(AF_INET, SOCK_DGRAM, 0)) <0)                            
    {                                                                           
        return -1;                                                              
    }                                                                           
    seraddr.sin_family = AF_INET;                                                  
    seraddr.sin_port = htons(5060);                                                
    seraddr.sin_addr.s_addr = inet_addr(argv[1]);      

    bzero(&cliaddr, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(5059);
    cliaddr.sin_addr.s_addr = INADDR_ANY;//inet_addr("192.168.14.101");

    if( (udpfd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
    {
        fprintf(stderr, "socket TCP: %s \n", strerror(errno));
        return -1;
    }
    int ret = (bind(udpfd,(struct sockaddr*)&cliaddr,sizeof(cliaddr)));
    cout << "ret"<<ret<<endl;
    if(-1 == ret) 
    { 
        perror("Server Bind Failed:"); 
        exit(1); 
    } 
    uint32_t tid = 1;
    char* meg;
    size_t len;
    int state;
    int contactid;
    SIPManager* sipma = new SIPManager( LOCAL_DEV_NAME, UAC_IP, 
            UAC_LISTEN_PORT_STR, LOCAL_DEV_PASSWD_STR);
    sipma->SIPManagerInit();
    sipma->Register( tid, &meg, &len, &state, &contactid, UAS_IP, UAS_LISTEN_PORT_STR);
    cout<<"state is "<< state <<endl;
    cout<<"the length "<<len<<endl;
    int wnum = sendto( udpfd, meg, (len), 0, (struct sockaddr *)&seraddr, sizeof(seraddr));
    if( wnum <=0)
    {
        fprintf(stderr, "socket TCP: %s \n", strerror(errno));
        cout<<" write to server failed "<<endl;
        return -1;
    }
    {
        int rnetnum = read( udpfd, rbuf , 500);
        if( rnetnum < 0)
        {
            cout<<" read to server failed "<<endl;
            return -1; 
        }
        string rt_str = string( rbuf, rnetnum);
        cout<<"check there is 401:\n"<<rt_str <<endl;
        char* rtmeg;
        size_t rtlen;
        int restate;
        uint32_t rttid;
        sipma->DealSIPMeg( rbuf, rnetnum, &rtmeg, &rtlen, &restate, &rttid);
        sleep(1);
        int wnum = sendto( udpfd, rtmeg, (rtlen), 0, (struct sockaddr *)&seraddr, sizeof(seraddr));
        if( wnum <=0)
        {
            fprintf(stderr, "socket TCP: %s \n", strerror(errno));
            return -1;
        }
    }
    {
        memset(rbuf,0,500);
        int rnetnum = read( udpfd, rbuf , 500);
        if( rnetnum < 0)
        {
            cout<<" read to server failed "<<endl;
            return -1; 
        }
        string rt_str = string( rbuf, rnetnum);
        cout<<"check there is 200ok\n"<<rt_str <<endl;
    }
    {
        char* meg;
        size_t len;
        int state=0;
        sipma->InviteLivePlay( tid, contactid, &meg, &len, &state);
        int wnum = sendto( udpfd, meg, (len), 0, (struct sockaddr *)&seraddr, sizeof(seraddr));
        if( wnum <=0)
        {
            fprintf(stderr, "socket UDP: %s \n", strerror(errno));
            return -1;
        }
        memset(rbuf,0,500);
        int rnetnum = read( udpfd, rbuf , 500);
        if( rnetnum < 0)
        {
            cout<<" read to server failed "<<endl;
            return -1; 
        }
        sipma->DealSIPMeg( rbuf, rnetnum, &meg, &len, &state, &tid);//todo: 100try
        memset(rbuf,0,500);
        rnetnum = read( udpfd, rbuf , 500);
        if( rnetnum < 0)
        {
            cout<<" read to server failed "<<endl;
            return -1; 
        }
        sipma->DealSIPMeg( rbuf, rnetnum, &meg, &len, &state, &tid);//todo: 200ok
        wnum = sendto( udpfd, meg, (len), 0, (struct sockaddr *)&seraddr, sizeof(seraddr));
        if( wnum <=0)
        {
            fprintf(stderr, "socket UDP: %s \n", strerror(errno));
            return -1;
        }
        sipma->Bye( 1, 0, &meg, &len, &state);
        wnum = sendto( udpfd, meg, (len), 0, (struct sockaddr *)&seraddr, sizeof(seraddr));
        if( wnum <=0)
        {
            fprintf(stderr, "socket UDP: %s \n", strerror(errno));
            return -1;
        }
    }
}
