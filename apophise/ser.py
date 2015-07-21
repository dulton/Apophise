#!/usr/bin/env python
# coding=utf-8

from socket import *
from netconn import *
from parser import *
from threadpool import ThreadPool
import threading
import os

from util import diskWriter


def threadjob( arglist, kwargs):
    filewt = diskWriter()                                                       
    info = arglist[0]
    print "in the thread is:" + str(info)
    filename = (info[1].split(" ")[0]) + "-"+ info[0]                           
    filepath = "/home/apophise/metadata/"+ info[0] + "/"                        
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
    if info[0] not in METAclient:
        METAclient.__add__( tuple(info[0]))
    tdpool.add_job( threadjob, info, info)
    print "out the thread is:" + str(info)

