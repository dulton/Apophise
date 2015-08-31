#!/usr/bin/env python
# coding=utf-8
# ************************************************************************/
#	> Author: cooperzhang
#	> Mail: zbzcsn@qq.com
#	> Created Time: Thu 09 Jul 2015 08:12:29 PM CST
# ************************************************************************/

#todo: to analyze the access log

from threading import Timer
import os
import time

g_one_time_in_24 = 0

def do_analyze_log( filepath):
    #超过两百次的ip数量
    total200 = 0
    #当天总共的ip数量
    totalip = 0
    #当天总共的访问量
    totalaccess = 0
    #当天每个小时区间的访问量
    total24hour = {}
    requesttimeover200 = {}
    result = {}
    fd = open( filepath, 'r')
    line = fd.readline()
    #print line
    while line:
        #print line
        div = line.split(' ')
        #print div[3]
        div24 = str(div[3]).split(':')
        #print div24
        if result.has_key( div[0]):
            result[div[0]] = result[div[0]] + 1
            if result[div[0]] > 199:
                if not requesttimeover200.has_key(div[0]):
                    total200 = total200 + 1 
                    requesttimeover200[div[0]] = 1
        else:
            result[div[0]] = 1
            totalip = totalip + 1
        totalaccess = totalaccess + 1
        #print div24[1]
        if total24hour.has_key(div24[1]):
            total24hour[div24[1]]= total24hour[div24[1]] + 1
        else:
            total24hour[div24[1]]= 1
        line =  fd.readline()
    fd.close
    return totalip, total200 ,totalaccess, total24hour

def analyzelog( filepath):
    totalip =  0
    total200 =  0
    totalaccess =  0
    total24hour =   {}
    global g_one_time_in_24
    stamp = time.localtime()
    style_time = str(time.strftime("%Y-%m-%d:%H:%M:%S", stamp))
    div = style_time.split(':')
    hour_in_24 = str(div[1])
    if (cmp( hour_in_24, '18') == 0):
        if g_one_time_in_24 == 0:
            g_one_time_in_24 = 1
            return do_analyze_log( filepath)
    else:
        if cmp( hour_in_24, '00'):
            g_one_time_in_24 = 0
            return totalip, total200, totalaccess, total24hour
    return totalip, total200, totalaccess, total24hour
    
if __name__ == '__main__':
    totalip, total200 ,totalaccess, total24hour =  analyzelog( "./logs/access.log")
    print "totalip %d total200 %d totalacc %d"%( totalip, total200, totalaccess,)
    print str(total24hour)

