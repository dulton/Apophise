/*************************************************************************
> File Name: main.cpp
> Author: 
> Mail: 
> Created Time: Mon 29 Jun 2015 02:08:12 PM CST
************************************************************************/

#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include<sys/socket.h>                                                          
#include <netinet/in.h>                                                         
#include <arpa/inet.h>  
#include<stdlib.h>
//#include <boost/algorithm/string.hpp>
#include <vector>
#include<iostream>
#include <string.h>

#ifdef ENABLE_MPATROL
#include <mpatrol.h>
#endif

//#include <osipparser2/internal.h>
#include <osipparser2/osip_port.h>
#include <osipparser2/osip_message.h>
//#include <osip_authorization.h>

#include"Agent.h"
#include"mediarecver.h"
#include"md5.h"

using namespace std;
char* nonce = NULL;
osip_www_authenticate_t *wwwauthenticate;

vector< char*>* strsplit( char* sipchar)
{
    vector<char*>* re = new vector<char*>();
    char* ptr = sipchar;
    int cur_char_len = 0;
    bool newheader = true;
    char buf[200];
    while(1)
    {
        if( newheader)
        {
            cur_char_len = 0;
            newheader = false;
        }

        if( '\n' != (*ptr))
        {
            buf[cur_char_len] = (*ptr);
            cur_char_len++;
            ptr++;
        }else
        {
            if( 1 == cur_char_len)
            {// header == '\r\n'
                break;
            }
            buf[cur_char_len] = '\0';
            char* header = ( char*)malloc( cur_char_len + 1);
            memcpy( header, buf, (cur_char_len + 1));
            re->push_back( header);
            if( 'w' == (*header))
            {
                nonce = header;
                cout<< " nonce got "<<endl;
		cout<< nonce <<endl;
            }
        }
    }
    return re;
}

string regsiterMD5( string username, string realm, string passwd, string uri)
{
    string str;
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
    str = md5_3.toString();
    return str;
}

void changeResponse( char* buf, char* destresponse)
{
    string src(buf);
    int offset = src.find("response");
    offset = offset+10;
    int count = 0;
    char* ptr = destresponse;
    for( count = 0; count < 32; count++)
    {
        buf[offset + count] = destresponse[count]; 
        count++;
    }
}

int main( int argn, char** argv)
{
    struct sockaddr_in addr;
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
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5060);
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    vss::net::ConnectAgent connAgent;
    //vss::net::Agent* registeragent = connAgent.to_connect( argv[1], 5060, 0);

    int file_fd = open( "./res/sip1", O_RDWR);
    if( file_fd < 0)
    {
        cout<<" open file failed"<<endl;
    }
    long fsize = lseek( file_fd, 0L, SEEK_END);
    lseek( file_fd, 0L, SEEK_SET);
    int rnum = read( file_fd, buf, fsize);
    if( rnum <= 0)
    {
        cout<<" read failed "<<endl;
        return -1;
    }
    int wnum = sendto( udpfd, buf, (fsize), 0, (struct sockaddr *)&addr, sizeof(addr));
    if( wnum <=0)
    {
        cout<<" write to server failed "<<endl;
        return -1;
    }
    sleep(1);

    int rnetnum = read( udpfd, rbuf , 500);
    if( rnetnum < 0)
    {
        cout<<" read to server failed "<<endl;
        return -1;
    }
    cout<< rbuf <<endl;
    strsplit( rbuf);
    osip_www_authenticate_init (&wwwauthenticate);
    osip_www_authenticate_parse ( wwwauthenticate, nonce);
    nonce = wwwauthenticate->nonce;
    string username("34020000001320000001");
    string passwd("111111");
    string uri("sip:34020000002000000001@192.168.1.136");
    string realm("3402000000");
    string response = regsiterMD5( username, realm, passwd, uri);

    {//second
        char buf[500];
        char rbuf[500];
        int file_fd = open( "./res/sip2", O_RDWR);
        if( file_fd < 0)
        {
            cout<<" open file failed"<<endl;
        }
        long fsize = lseek( file_fd, 0L, SEEK_END);
        lseek( file_fd, 0L, SEEK_SET);
        int rnum = read( file_fd, buf, fsize);
        if( rnum <= 0)
        {
            cout<<" read failed "<<endl;
            return -1;
        }
        changeResponse( buf, response);	
        int wnum = sendto( udpfd, buf, (fsize), 0, (struct sockaddr *)&addr, sizeof(addr));
        if( wnum <=0)
        {
            cout<<" write to server failed "<<endl;
            return -1;
        }
        sleep(1);

        memset( rbuf, 0, 500);
        int rnetnum = read( udpfd, rbuf , 500);
        if( rnetnum < 0)
        {
            cout<<" read to server failed "<<endl;
            return -1;
        }
        cout<< rbuf <<endl;
    }
    return 0;
}