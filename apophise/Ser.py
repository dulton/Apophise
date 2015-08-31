#!/usr/bin/env python
# coding=utf-8

from socket import *
from NetConn import *
from Parser import *
from ThreadPool import ThreadPool
import threading
import os
from Util import diskWriter

def deskWriteJob( arglist, kwargs):
    filewt = diskWriter()                                                       
    info = arglist[0]
    print "in the thread is:" + str(info)
    filename = (info["timestamp"].split(" ")[0]) + "-"+ info["dev_name"]                           
    filepath = "data/"+ info["dev_name"] + "/"                        
    targetpath = os.path.dirname(filepath)                                      
    if not os.path.isdir(targetpath):
        os.makedirs(targetpath)                                                 
    filename = filepath + filename                                              
    filewt.write( filename, str(info)+'\n')

print 'test ser start'
udp_server = UDPser( '127.0.0.1', 9876)
METAclient =()
tdpool = ThreadPool(10)
while True:
    data,addr = udp_server.readmeg()
    parsertest = Parser()
    info = parsertest.parserMeg(data)
    print "info json = "
    print info
    if info["dev_name"] not in METAclient:
        METAclient.__add__( tuple(info["dev_name"]))
    tdpool.add_job( deskWriteJob, info, info)
    print "out the thread is:" + str(info)

