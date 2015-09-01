#!/usr/bin/env python
# coding=utf-8

from myemail import *
import time
import os
from util import diskWriter
from config import *

class WebServer:
    dev_name = ''
    cpu_max = 90.0
    mem_max = 90.0
    disk_max = 90.0
    disk_busy = 0
    cpu_busy = 0
    mem_busy = 0
    last_mail_time = 0.0
    def SetDevname(self,name):
        self.dev_name = name

    def JobDespatcher(self,info):
        job_type = info["msg_type"]
        if job_type == "heartbeat":
            print "heartbeat"
            self.DoServerInfo(info)
        elif job_type == "dailyaccess":
            print "dailyaccess"
            self.DoAccessLog(info)
    def DoServerInfo(self,info):
        filewt = diskWriter()
        filename = (info["timestamp"].split(" ")[0]) + "-"+ info["dev_name"]
        filepath = "data/"+ info["dev_name"] + "/"
        targetpath = os.path.dirname(filepath)
        if not os.path.isdir(targetpath):
            os.makedirs(targetpath)
        filename = filepath + filename
        filewt.write( filename, str(info)+'\n')
        cpu = float(info["cpu_percent"])
        if cpu > self.cpu_max:
            self.cpu_busy = 1
        mem = float(info["mem_percent"])
        if mem > self.mem_busy:
            self.mem_busy = 1
        disk_info_list = str(info["mem_percent"])
        disk_list = disk_info_list.split(',')
        for pama in disk_list:
            if 'mountpath' not in pama:
                disk = float(pama)
                if disk > self.disk_max:
                    self.disk_busy = 1
        if self.cpu_busy == 1 | self.mem_busy == 1 | self.disk_busy == 1:
            new_time = time.time()
            if (new_time - self.last_mail_time) > 3600:
                print "#################### info user ######################"
                print "now_time="+str(new_time)+"last_mail_time"+str(self.last_mail_time)
                EmergencyReport(self.dev_name,info["cpu_percent"],info["mem_percent"],info["disk_percent"])
                self.last_mail_time = time.time()
            self.cpu_busy = 0
            self.disk_busy = 0
            self.mem_busy = 0

    def DoAccessLog(self,info):
        print "do log"
        global g_lock
        global g_client_list
        global g_client_access_list
        content = str(':\t')+str("总接入IP数量:")+str(info["totalip"])+ str('\t')+str("访问超过200次IP数量:") + str(info["total200"]) + \
                str('\t') + str("总访问次数:")+ str(info["totalaccess"]) + str('\r\n') + \
                str(info["total24hour"])
        g_lock.acquire()
        g_client_access_list[str(info["dev_name"])]=content
        client_len = len(g_client_list)
        access_len = len(g_client_access_list)
        if client_len == access_len:
            print "send access mail"
            ReportDaily(g_client_access_list)
        g_lock.release()
