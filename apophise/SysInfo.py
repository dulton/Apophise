#!/usr/bin/env python
# coding=utf-8

##########
#  File Namesysinfo.py
#  Author: cooperz
#  Mail: zbzcsn@qq.com
#  Created Time: Mon 06 Jul 2015 03:50:26 PM CST
##########

import psutil,time
import re
import sys
import socket
import fcntl
import struct
from util import *

class DiskUsage:
    path = ""
    sdusage = ""
    def __init__( self, n, x):
        self.path = n
        self.sdusage = x

def getMemoryState():
    phymem = psutil.virtual_memory()
    json_mem_percent = '\"mem_percent\":\"'+str(phymem[2])+'\",'
    return json_mem_percent

def getCPUState():
    json_cpustate = '\"cpu_percent\":\"'+ str(psutil.cpu_percent(2)) + '\",'
    return json_cpustate

def getCPUCount():
    cpucount = str(psutil.cpu_count( logical=False)) #只返回物理核心数
    return cpucount

def getDiskInfo():
    x = []
    info = psutil.disk_partitions()
    #print info
    for parttion in info:
        x.append( parttion[1])
    re = []
    for y in x:
        w = ''
        w = "mountpath="+str( y )+","
        #w = w + str(psutil.disk_usage(y))
        w = w + str(psutil.disk_usage(y)[3])
        re.append(w)
    disk_percent = '\"disk_percent\":\"'+ str(re) + '\",'
    return disk_percent

def getDiskIOInfo():
    disks_before = psutil.disk_io_counters()
    time.sleep(1)
    disks_after = psutil.disk_io_counters()
    disks_read_per_sec = disks_after.read_bytes - disks_before.read_bytes
    disks_write_per_sec = disks_after.write_bytes - disks_before.write_bytes
    wio = str( disks_write_per_sec)
    rio = str( disks_read_per_sec)
    w = '\"diskwrite\":\"' + wio + '\",\"' +"diskread\":\""+rio +'\",'
    return w

def getProcessInfo(p): 
    #取出指定进程占用的进程名，进程ID，进程实际内存, 虚拟内存,CPU使用率
    try:
        cpu = int(p.get_cpu_percent(interval=0)) 
        rss, vms = p.get_memory_info() 
        name = p.name 
        pid = p.pid 
    except psutil.error.NoSuchProcess, e:
        name = "Closed_Process"
        pid = 0
        rss = 0
        vms = 0
        cpu = 0
    return [name, pid, rss, vms, cpu]

def getAllProcessInfo():
    #取出全部进程的进程名，进程ID，进程实际内存, 虚拟内存,CPU使用率
    instances = []
    all_processes = list(psutil.process_iter()) 
    for proc in all_processes: 
        proc.get_cpu_percent(interval=0) 
        #此处sleep1秒是取正确取出CPU使用率的重点
        time.sleep(1) 
        for proc in all_processes: 
            instances.append(getProcessInfo(proc))
        return instances

def processinfo(x):
    p = psutil,get_process_list()
    for r in p :
        aa = str(r);
        f = re.compile( x, re, I)
        if f.search(aa):
            print aa.split('pid')

def getNetIO():
    w = psutil.net_io_counters(pernic=False)
    net_before = psutil.net_io_counters()
    #print str(net_before)
    time.sleep(1)
    net_after = psutil.net_io_counters()
    #print str(net_after)
    net_read_per_sec = net_after.bytes_recv - net_before.bytes_recv
    net_write_per_sec = net_after.bytes_sent - net_before.bytes_sent
    wio = str( net_write_per_sec)
    rio = str( net_read_per_sec)
    w = '\"netwrite\":\"' + wio + '\",\"' +"netread\":\""+rio +'\"'
    return w

def getIPAddress(ifname):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    return socket.inet_ntoa(fcntl.ioctl(
        s.fileno(),
        0x8915,  # SIOCGIFADDR
        struct.pack('256s', ifname[:15])
    )[20:24])

if __name__ == '__main__':
    print getCPUState()
    print getMemoryState()
    print getDiskInfo()
    print getDiskIOInfo()
    print getNetIO()
    print getIPAddress('eth0')

print 'magic python'
#allpinfo = getAllProcessInfo()
#for x in allpinfo:
#    print x
#print getCPUState()
#print getMemoryState()
#print getDiskInfo()
#print getDiskIOInfo()
#print getNetIO()

