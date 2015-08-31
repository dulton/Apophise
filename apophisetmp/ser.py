#!/usr/bin/env python
# coding=utf-8

from socket import *
from netconn import *
from parser import *
from threadpool import ThreadPool
from WebServer import WebServer
import thread
import threading
import os
from config import *

from util import diskWriter

print 'test ser start'
udp_server = UDPser( '127.0.0.1', 9876)
tdpool = ThreadPool(10)

def deskWriteJob( arglist, kwargs):
    info = arglist[0]
    #print "in the thread is:" + str(info)
    webservername = str(info["dev_name"])
    if info["dev_name"] not in g_client_list:
        g_lock.acquire()
        print "new webserver"
        print info["dev_name"]
        g_client_list[info["dev_name"]] = info["dev_name"]
        g_lock_dict[info["dev_name"]] = thread.allocate_lock()
        newwebserver = WebServer()
        newwebserver.SetDevname(info["dev_name"])
        g_websever[info["dev_name"]] = newwebserver
        g_lock.release()
    temp_lock = g_lock_dict[info["dev_name"]]
    temp_lock.acquire()
    temp_webserver = g_websever[info["dev_name"]]
    temp_webserver.JobDespatcher(info)
    temp_lock.release()


while True:
    data,addr = udp_server.readmeg()
    parsertest = Parser()
    info = parsertest.parserMeg(data)
    #print "info json = "
    #print info
    tdpool.add_job( deskWriteJob, info, info)
    #print "out the thread is:" + str(info)

