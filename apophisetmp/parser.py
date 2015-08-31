#!/usr/bin/env python
# coding=utf-8
# ************************************************************************/
#	> Author: cooperzhang
#	> Mail: zbzcsn@qq.com
#	> Created Time: Fri 10 Jul 2015 08:30:00 PM CST
# ************************************************************************/

import json

class Parser:
    def parserMeg(self,meg):
        #print meg
        info = json.loads( meg)
        return info
