#!/usr/bin/env python
# coding=utf-8
# ************************************************************************/
#	> Author: cooperzhang
#	> Mail: zbzcsn@qq.com
#	> Created Time: Thu 09 Jul 2015 08:13:46 PM CST
# ************************************************************************/

#@brif: the client for apophise

from NetConn import *
import time
import Logger
import SysInfo

g_pcnamehead = '\"dev_name\":\"'+ SysInfo.getIPAddress('eth0') +'\",'
g_interval = 2
g_serip = '127.0.0.1'
g_serport = 9876

while True:
    print "detect"
    stamp = time.localtime() 
    otherStyleTime = '\"timestamp\":\"' + str(time.strftime("%Y-%m-%d %H:%M:%S", stamp)) + '\",'
    sysCPU = str(SysInfo.getCPUState())
    sysMEM = str(SysInfo.getMemoryState()) 
    sysDisk = str(SysInfo.getDiskInfo()) 
    sysDiskIO = str(SysInfo.getDiskIOInfo()) 
    sysNetIO = str(SysInfo.getNetIO())
    udptranser = UDPAgent( g_serip, g_serport) 
    meg = "{"+ g_pcnamehead +  otherStyleTime + sysCPU + sysMEM + sysDisk  + sysDiskIO + sysNetIO + "}"
    udptranser.sendmeg( str(meg))
    print meg
    time.sleep( g_interval)
