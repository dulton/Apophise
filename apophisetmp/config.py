#!/usr/bin/env python
# coding=utf-8

import sysinfo
import thread

g_pcnamehead = '\"dev_name\":\"'+ sysinfo.getIPAddress('eth0') +'\",'
g_interval = 2
g_serip = '222.197.183.107'
g_serport = 9876
g_accesslog = '/usr/local/nginx/logs/access'


g_client_list ={}
g_client_access_list ={}
g_lock_dict = {}
g_websever = {}
g_lock = thread.allocate_lock()
