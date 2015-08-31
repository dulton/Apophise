#!/usr/bin/env python
# coding=utf-8

from netconn import udp_send
import sys

print 'cli start'
print 'please input some string'
while True:
    readline = sys.stdin.readline()
    udp_send( readline, '127.0.0.1', 45678 )
    print 'send OK'
