#!/usr/bin/env python
# coding=utf-8

##########
#  File Name: util.py
#  Author: cooperz
#  Mail: zbzcsn@qq.com
#  Created Time: Mon 06 Jul 2015 03:53:31 PM CST
##########

def bytes2human(n):
    symbols = ('K','M','G','T','P','E','Z','Y')
    prefix = {}
    for i,s in enumerate(symbols):
        prefix[s] = 1 << (i+1)*10
    for s in reversed(symbols):
        if n >= prefix[s]:
            value = float(n) / prefix[s]
            return '%.2f %s' % (value,s)
    return '%.2f B' %(n)

class diskWriter:
    meg=''
    def __init__(self):
        meg=''
    def write(self,filename,meg):
        output = open(filename, 'a')
        try:
            output.write(meg)
        finally:
            output.close()

if __name__ == '__main__':
    print  bytes2human( 100000000 )
