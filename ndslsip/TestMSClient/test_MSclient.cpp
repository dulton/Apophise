/*************************************************************************
  > File Name: main.cpp
  > Author: cooperz
  > Mail: zbzcsn@qq.com
  > Created Time: Mon 27 Jul 2015 05:02:26 PM CST
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

#include "SIPUtil.h"
#include "SIPStatuCode.h"
#include "SIPClient.h"
#include "SIPMediaServer.h"

using namespace svss::SIP;
using namespace std;

uint32_t g_task_id = 1;

struct sockaddr_in seraddr;                                                    
struct sockaddr_in cliaddr;                                                    

int init_udp_fd(char* ip)
{

    FILE* logfile = fopen( "logfileman.txt", "w");
    osip_trace_initialize( (_trace_level)8, logfile );
    int udpfd;                                                               
    if ( ( udpfd=socket(AF_INET, SOCK_DGRAM, 0)) <0)                            
    {                                                                           
        return -1;                                                              
    }                                                                           
    seraddr.sin_family = AF_INET;                                                  
    seraddr.sin_port = htons(5060);                                                
    seraddr.sin_addr.s_addr = inet_addr(ip);      

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
    return udpfd;
}

int test_client_invite(int udpfd, SIPClient* client);
int test_client_register(int udpfd)
{
    SIPClient client(LOCAL_DEV_NAME, UAC_IP, UAC_LISTEN_PORT_STR,
            LOCAL_DEV_PASSWD_STR);
    /*初始化，主要是初始化SIP解析器*/
    int rt = client.Init();
    if(rt != SIP_SUCCESS)
    {
#ifdef DEBUG
        cout<<"client init"<<endl;
#endif
        return -1;
    }
    char* send_msg;
    size_t send_len;
    /*每个client只能注册向一个SIP sever*/
    uint32_t registerid = g_task_id;
    g_task_id++;
    client.RegisterClient(registerid, &send_msg, &send_len, REMOTE_DEV_NAME ,UAS_IP,
            UAS_LISTEN_PORT_STR, LOCAL_DEV_PASSWD_STR);
    int wnum = sendto( udpfd, send_msg, (send_len), 0, (struct sockaddr *)&seraddr, 
            sizeof(seraddr));
    if( wnum <=0)
    {
        fprintf(stderr, "socket UDP: %s \n", strerror(errno));
        cout<<" write to server failed "<<endl;
        return -1;
    }
    char rbuf[1024];
    int rnetnum = read( udpfd, rbuf , 500);
    if( rnetnum < 0)
    {
        cout<<" read to server failed "<<endl;
        return -1; 
    }
    string rt_str = string( rbuf, rnetnum);
    cout<<"check there is 401:\n"<<rt_str <<endl;
    /*接收sip协议可能产生新的task_id,同时新的任务会使用一个port；
     * 对于client来说，是不会使用到这两个参数的。
     * 当时没设计好，FSMDrive是从父类的纯虚函数
     * */
    char* new_send_msg = NULL;
    size_t new_send_len = 0;
    uint32_t retain_task_id = g_task_id;
    string retain_port = string("87878");
    rt = client.FSMDrive( retain_task_id, rbuf, rnetnum, retain_port, 
            &new_send_msg, &new_send_len);
    if(rt != SIP_CONTINUE)
    {
        cout<<"401 recved, but build new register failed "<<endl;
        return -1; 
    }
    wnum = sendto( udpfd, new_send_msg, (new_send_len), 0, 
            (struct sockaddr *)&seraddr, 
            sizeof(seraddr));
    if(wnum<=0)
    {
        fprintf(stderr, "socket UDP: %s \n", strerror(errno));
        return -1;
    }

    memset(rbuf,0,500);
    rnetnum = read( udpfd, rbuf , 500);
    if( rnetnum < 0)
    {
        cout<<" read to server failed "<<endl;
        return -1; 
    }
    rt_str = string( rbuf, rnetnum);
    cout<<"check there is 200ok\n"<<rt_str <<endl;
    rt = client.FSMDrive( retain_task_id, rbuf, rnetnum, retain_port, 
            &new_send_msg, &new_send_len);
    if(rt != (int)registerid)
    {
        cout<<"200 recved, but register finish failed"<<endl;
        return -1; 
    }else
    {
        /*有的任务结束了，但还有最后一次信息必须发出去
         *有的任务结束了，却是没有信息需要发出去了，所以对rtlen要做判断
         * */
        if( 0==new_send_len)
        {
            cout<<"task_id = "<<registerid<<" finished"<<endl;
        }
    }
    cout<<"Type Anything To Start Client InviteStore"<<endl;                                                                                                                         
    //getchar();  
    test_client_invite(udpfd,&client);
    return 1;
}

int test_client_invite(int udpfd, SIPClient* client)
{
    int invite_id = g_task_id++;
    char* send_msg;
    size_t send_len;
    string recv_port("76677");
    client->InviteStore(invite_id,recv_port,&send_msg,&send_len);
    int wnum = sendto( udpfd, send_msg, (send_len), 0, (struct sockaddr *)&seraddr, 
            sizeof(seraddr));
    if( wnum <=0)
    {
        fprintf(stderr, "socket UDP: %s \n", strerror(errno));
        cout<<" write to server failed "<<endl;
        return -1;
    }
    char rbuf[1024];
    memset(rbuf,0,500);
    int rnetnum = read( udpfd, rbuf , 500);
    if( rnetnum < 0)
    {
        cout<<" read to server failed "<<endl;
        return -1; 
    }
    char* new_send_msg = NULL;
    size_t new_send_len = 0;
    uint32_t retain_task_id = g_task_id;
    string retain_port = string("87878");
    int rt = client->FSMDrive( retain_task_id, rbuf, rnetnum, retain_port, 
            &new_send_msg, &new_send_len);
    if(rt != SIP_CONTINUE)
    {
        cout<<"100 recved, but error happend "<<endl;
        return -1; 
    }
    rnetnum = read( udpfd, rbuf , 500);
    if( rnetnum < 0)
    {
        cout<<" read to server failed "<<endl;
        return -1; 
    }
    rt = client->FSMDrive( retain_task_id, rbuf, rnetnum, retain_port, 
            &new_send_msg, &new_send_len);
    if(rt != invite_id)
    {
        cout<<"200ok recved, invite end, but error happend "<<endl;
        return -1;
    }
    else
    {
        cout<<"task_id = "<<invite_id<<" finished"<<endl;
    }
    wnum = sendto( udpfd, new_send_msg, (new_send_len), 0, 
            (struct sockaddr *)&seraddr, 
            sizeof(seraddr));
    if(wnum<=0)
    {
        fprintf(stderr, "socket UDP: %s \n", strerror(errno));
        return -1;
    }
    return 1;
}

int test_media_invite(int udpfd, SIPMediaServer* media_server);
int test_media_register(int udpfd)
{
    SIPMediaServer media_server(LOCAL_DEV_NAME, UA_ME_IP, UA_ME_PORT_STR,
            LOCAL_DEV_PASSWD_STR);
    /*初始化，主要是初始化SIP解析器*/
    int rt = media_server.Init();
    if(rt != SIP_SUCCESS)
    {
#ifdef DEBUG
        cout<<"client init"<<endl;
#endif
        return -1;
    }
    char* send_msg;
    size_t send_len;
    /*每个client只能注册向一个SIP sever*/
    uint32_t registerid = g_task_id;
    g_task_id++;
    media_server.RegisterMediaServer(registerid, &send_msg, &send_len, 
            REMOTE_DEV_NAME , UAS_IP, UAS_LISTEN_PORT_STR,
            LOCAL_DEV_PASSWD_STR);
    int wnum = sendto( udpfd, send_msg, (send_len), 0, (struct sockaddr *)&seraddr, 
            sizeof(seraddr));
    if( wnum <=0)
    {
        fprintf(stderr, "socket UDP: %s \n", strerror(errno));
        cout<<" write to server failed "<<endl;
        return -1;
    }
    char rbuf[1024];
    int rnetnum = read( udpfd, rbuf , 500);
    if( rnetnum < 0)
    {
        cout<<" read to server failed "<<endl;
        return -1; 
    }
    string rt_str = string( rbuf, rnetnum);
    cout<<"check there is 401:\n"<<rt_str <<endl;
    /*接收sip协议可能产生新的task_id,同时新的任务会使用一个port；
     * 对于client来说，是不会使用到这两个参数的。
     * 当时没设计好，FSMDrive是从父类的纯虚函数
     * */
    char* new_send_msg = NULL;
    size_t new_send_len = 0;
    uint32_t retain_task_id = g_task_id;
    string retain_port = string("87878");
    rt = media_server.FSMDrive( retain_task_id, rbuf, rnetnum, retain_port, 
            &new_send_msg, &new_send_len);
    if(rt != SIP_CONTINUE)
    {
        cout<<"401 recved, but build new register failed "<<endl;
        return -1; 
    }
    wnum = sendto( udpfd, new_send_msg, (new_send_len), 0, 
            (struct sockaddr *)&seraddr, 
            sizeof(seraddr));
    if(wnum<=0)
    {
        fprintf(stderr, "socket UDP: %s \n", strerror(errno));
        return -1;
    }

    memset(rbuf,0,500);
    rnetnum = read( udpfd, rbuf , 500);
    if( rnetnum < 0)
    {
        cout<<" read to server failed "<<endl;
        return -1; 
    }
    rt_str = string( rbuf, rnetnum);
    cout<<"check there is 200ok\n"<<rt_str <<endl;
    rt = media_server.FSMDrive( retain_task_id, rbuf, rnetnum, retain_port, 
            &new_send_msg, &new_send_len);
    if(rt != (int)registerid)
    {
        cout<<"200 recved, but register finish failed"<<endl;
        return -1; 
    }else
    {
        /*有的任务结束了，但还有最后一次信息必须发出去
         *有的任务结束了，却是没有信息需要发出去了，所以对rtlen要做判断
         * */
        if( 0==new_send_len)
        {
            cout<<"task_id = "<<registerid<<" finished"<<endl;
        }
    }
    cout<<"Type Anything To Continue Media_Server Invite"<<endl;
    //getchar();
    test_media_invite(udpfd, &media_server);
    return 1;
}

int test_media_invite(int udpfd, SIPMediaServer *media_server)
{
    char rbuf[1024];
    memset(rbuf,0,500);
    int rnetnum = read( udpfd, rbuf , 500);
    if( rnetnum < 0)
    {
        cout<<" read to server failed "<<endl;
        return -1; 
    }
    char* send_msg = NULL;
    size_t send_len = 0;
    uint32_t retain_task_id = g_task_id;
    string retain_port = string("9876");
    int rt = media_server->FSMDrive( retain_task_id, rbuf, rnetnum, retain_port, 
            &send_msg, &send_len);
    if(rt != SIP_CONTINUE)
    {
        cout<<"401 recved, but build new register failed "<<endl;
        return -1; 
    }
    int wnum = sendto( udpfd, send_msg, (send_len), 0, (struct sockaddr *)&seraddr, 
            sizeof(seraddr));
    if( wnum <=0)
    {
        fprintf(stderr, "socket UDP: %s \n", strerror(errno));
        cout<<" write to server failed "<<endl;
        return -1;
    }
    rt = media_server->FSMDrive( retain_task_id, rbuf, rnetnum, retain_port, 
            &send_msg, &send_len);
    if(rt != (int)retain_task_id)
    {
        cout<<"ack recved, but mediserver play  failed "<<endl;
        return -1; 
    }
    return 1;
}


int main(int argn, char** argv)
{
    if( argn < 2)                                                               
    {                                                                           
        cout<<" Usage : ./test IP defualt PORT 5060\n"<<endl;                   
        return -1;                                                              
    }                                                                           
    int udpfd = init_udp_fd(argv[1]);
    test_client_register(udpfd);
    //test_media_register(udpfd);
    return 1;
}
