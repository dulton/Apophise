/*************************************************************************
  > File Name: md5test.cpp
  > Author: 
  > Mail: 
  > Created Time: Sun 19 Jul 2015 09:26:36 PM CST
 ************************************************************************/

#include<iostream>
#include"md5.h"
using namespace std;

string RegisterMd5( string username, string realm, string passwd, string uri, string nonce)
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

/*
int main()
{
    string local_dev_name("34020000002020000001");
    string realm("192.168.171.130");
    string local_dev_passwd_str("12345678");
    string uri("sip:34020000002020000001@192.168.171.130:5060");
    string nonce("69c65675ce797945");
    string response = RegisterMd5( local_dev_name, realm, local_dev_passwd_str, uri, nonce);
    cout<< response << endl;
    return 0;
}

*/
