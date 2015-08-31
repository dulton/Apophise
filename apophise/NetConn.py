#!/usr/bin/env python
# coding=utf-8

##########
#  File Namenetconn.py
#  Author: cooperz
#  Mail: zbzcsn@qq.com
#  Created Time: Mon 06 Jul 2015 03:51:46 PM CST
##########

from socket import *

class UDPAgent:
    __ip = ''
    __port = 0
    __addr = ( '', '')
    udpclient = socket( AF_INET, SOCK_DGRAM)
    def __init__( self, ip, port):
        self.__ip = ip
        self.__port = port
        self.__addr = ( ip, port)
    def sendmeg(self , meg):
        self.udpclient.sendto( meg, self.__addr)

class UDPser:
    __ip = ''
    __port = 0
    __addr = ( '', '')
    udpclient = socket( AF_INET, SOCK_DGRAM)
    def __init__( self, ip, port):
        self.__ip = ip
        self.__port = port
        self.__addr = ( ip, port)
        self.udpclient.bind( self.__addr)
    def readmeg( self):
        return self.udpclient.recvfrom(1024)
    


def udp_send( meg, addr_ip, port):
    ADDR = ( addr_ip, port)
    udpclient = socket( AF_INET, SOCK_DGRAM)
    udpclient.sendto( meg, ADDR)

def udp_server( addr_ip, port):
    ADDR = ( addr_ip, port)
    udpSerSock = socket(AF_INET, SOCK_DGRAM)
    udpSerSock.bind( ADDR)

    while True:
        print 'waiting for meg'
        data, addr = udpSerSock.recvfrom(1024)
        print data
