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

def ReportDaily(client_access_list):
    sender='handmore@sina.com'
    receiver1 = 'zbzcsn@qq.com'
    subject = 'O&M.daily.report'
    smtpserver = 'smtp.sina.com'
    username = 'handmore'
    userpasswd = 'klcf116'
    
    msg=MIMEMultipart()
    msg['From']=sender
    msg['To']=receiver1
    msg['Subject']=Header( subject,'utf-8')
    body_header=\
    str('uestc.yzb.O&M:\r\n') #+ \
    #str('                 Disk       Net      Cpu       Access        Peak      Time-Quantum   \r\n') 
    body_content = '' 
    print str(client_access_list)
    for meg in client_access_list:
        print "meg in cli access" + str(meg)
        body_content = body_content + str(meg) + client_access_list[meg] + str('\r\n')
    body_tail = \
    str('                                                report by uestc yzb O&M\r\n')
    body = body_header+body_content+body_tail
    con = MIMEText( body, _subtype='plain', _charset='utf-8')
    msg.attach(con)


    smtp = smtplib.SMTP()
    smtp.connect( smtpserver)
    smtp.login( username, userpasswd)
    smtp.sendmail( sender, receiver1, msg.as_string())
    smtp.quit()

def EmergencyReport(dev_name,cpu,mem,disk):
    sender='handmore@sina.com'
    receiver1 = 'zbzcsn@qq.com'
    receiver2 = 'zb'
    subject = 'O&M.emergency.report'
    smtpserver = 'smtp.sina.com'
    username = 'handmore'
    userpasswd = 'klcf116'
    
    msg=MIMEMultipart()
    msg['From']=sender
    msg['To']=receiver1
    msg['Subject']=Header( subject,'utf-8')
    body= \
    "uestc.yzb.O&M:\r\n\r\n" + \
    str(dev_name)+str(":  CPU ")+ str(cpu) + str(",  MEM ")+ str(mem) + str(",  DISK ")+ str(disk) + str('\r\n\r\n') + \
    str("                                             report by uestc yzb O&M\r\n")
    con = MIMEText( body, _subtype='plain', _charset='utf-8')
    msg.attach(con)

    smtp = smtplib.SMTP()
    smtp.connect( smtpserver)
    smtp.login( username, userpasswd)
    smtp.sendmail( sender, receiver1, msg.as_string())
    smtp.quit()

