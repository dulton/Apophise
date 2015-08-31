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
import config
import accessanalyze


#g_pcnamehead = '\"dev_name\":\"'+ sysinfo.getIPAddress('eth0') +'\",'
#g_interval = 2
#g_serip = '127.0.0.1'
#g_serport = 9876

def TryDoAccessLog(udptranser):
    totalip, total200 ,totalaccess, total24hour = \
            accessanalyze.analyzelog(config.g_accesslog)
    type_header = '{\"msg_type\":\"dailyaccess\",'
    totalip_json = '\"totalip\":\"'+str(totalip)+'\",'
    total200_json = '\"total200\":\"'+str(total200)+'\",'
    totalaccess_json = '\"totalaccess\":\"'+str(totalaccess)+'\",'
    total24hour_json = '\"total24hour\":\"'+str(total24hour)+'\"}'
    msg_access = type_header+ config.g_pcnamehead + totalip_json + total200_json + totalaccess_json \
            + total24hour_json
    if totalip > 0:
        print msg_access
        udptranser.sendmeg(msg_access)


while True:
    print "detect"
    stamp = time.localtime()
    type_header = '\"msg_type\":\"heartbeat\",'
    otherStyleTime = '\"timestamp\":\"' + str(time.strftime("%Y-%m-%d %H:%M:%S", stamp)) + '\",'
    sysCPU = str(sysinfo.getCPUState())
    sysMEM = str(sysinfo.getMemoryState()) 
    sysDisk = str(sysinfo.getDiskInfo()) 
    sysDiskIO = str(sysinfo.getDiskIOInfo()) 
    sysNetIO = str(sysinfo.getNetIO())
    udptranser = UDPAgent( config.g_serip, config.g_serport) 
    meg = "{"+ type_header + config.g_pcnamehead +  otherStyleTime + sysCPU + sysMEM + sysDisk  + sysDiskIO + sysNetIO + "}"
    udptranser.sendmeg( str(meg))
    TryDoAccessLog(udptranser)
    print meg
    time.sleep( config.g_interval)
