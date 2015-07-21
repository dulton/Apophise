#!/usr/bin/env python
# coding=utf-8
# ************************************************************************/
#	> Author: cooperzhang
#	> Mail: zbzcsn@qq.com
#	> Created Time: Thu 09 Jul 2015 08:13:46 PM CST
# ************************************************************************/

#@brif: the client for apophise

from netconn import *
import time
import logger
import sysinfo

g_pcnamehead = "testcli::"
g_interval = 2
g_serip = '127.0.0.1'
g_serport = 9876

while True:
    print "detect"
    stamp = time.localtime() 
    otherStyleTime = str(time.strftime("%Y-%m-%d %H:%M:%S", stamp))
    sysCPU = str(sysinfo.getCPUCount()) + "::"
    sysMEM = str(sysinfo.getMemoryState()) + "::"
    sysDisk = str(sysinfo.getDiskInfo()) + "::"
    sysDiskIO = str(sysinfo.getDiskIOInfo()) + "::"
    sysNetIO = str(sysinfo.getNetIO())
    udptranser = UDPAgent( g_serip, g_serport) 
    meg = g_pcnamehead +  otherStyleTime + sysCPU + sysMEM + sysDisk + sysDiskIO + sysNetIO 
    udptranser.sendmeg( meg)
    time.sleep( g_interval)
