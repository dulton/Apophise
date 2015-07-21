#!/usr/bin/env python
# coding=utf-8
# ************************************************************************/
#	> Author: cooperzhang
#	> Mail: zbzcsn@qq.com
#	> Created Time: Tue 14 Jul 2015 08:25:49 PM CST
# ************************************************************************/

import smtplib
from email.mime.text import *
from email.mime.multipart import *
from email.header import Header

sender='handmore@sina.com'
receiver1 = 'zbzcsn@qq.com'
receiver2 = 'zb'
subject = 'O&M.daily.report'
smtpserver = 'smtp.sina.com'
username = 'handmore'
userpasswd = 'klcf116'

msg=MIMEMultipart()
msg['From']=sender
msg['To']=receiver1
msg['Subject']=Header( subject,'utf-8')
body="""
uestc.yzb.O&M:
                     Disk       Net      Cpu       Access        Peak      Time-Quantum   
    back up: 
    gms:
    oa:
    msheas:
    gr:
                                                    report by apophise
                                                    designed by uestc yzb O&M
"""
con = MIMEText( body, _subtype='plain', _charset='utf-8')
msg.attach(con)


smtp = smtplib.SMTP()
smtp.connect( smtpserver)
smtp.login( username, userpasswd)
smtp.sendmail( sender, receiver1, msg.as_string())
smtp.quit()
