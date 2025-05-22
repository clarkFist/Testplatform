# !/usr/bin/python
# _*_ coding: utf-8 _*_
from __future__ import print_function
import sys, os
from udp_loglib import Log, Conf, Tag, TestPlatForm, TcpServer, gevent, input_ui,FrameLib
from udp_loglib import g_sleep as sleep,myAssert
import udp_loglib
import time
def LogPath_set():
    Log.path_set(input_ui().value_get())
    #root = r'%s\Log_Log'%os.dirname(__file__)
    #Log.path_set(root + '/' + 'default')
    pass
def init():
    LogPath_set()
    tcp = TcpServer('127.0.0.1', 18999)
    Log.tcp = tcp
    if sys.version_info > (3, 0):
        # os.system('start pythonw3 log_manger.pyw')
        pass
def tag_send(slot,arg):
    root = r'%s\Log_Log'%os.dirname(__file__)
    p1 = arg[:4]
    p2 = 'step' + arg[4:6]
    Log.path_set(root + '/' + p1 +'/' + p2)
    TestPlatForm.form.tst_set(slot, Tag.tag_send(arg))


if __name__ == "__main__":
    '''
        main：confSet 与 log_recv 是两个几乎独立的协程，log_recv 会存储回复消息 ，该消息会在confSet使用

        confSet : 在此函数内进行桩操作或变量配置，运行结束后该协程结束
    '''
    init()
    form = TestPlatForm(18125, 18125)  #18125 为默认端口号s
    #form.lru_auto = True
    def confSet():
        cmd = Conf.allFlagSet(1)
        #form.tst_set('2',cmds)
        #tag_send('5','51650211')      #会改路径s
        form.tst_set('2',Tag.tag_send('50760111') )
        # form.tst_set('2',Tag.tag_send('52840210') )
        # form.tst_set('16',Tag.tag_send('52570111') )
        # form.tst_set('2',Tag.tag_send('01420200') )
        # form.tst_set('16',Tag.tag_send('01420300') )
        # form.tst_set('16',Tag.tag_send('01420400') )
        # form.tst_set('16',Tag.tag_send('01420510') )
        #form.tst_set('2',Conf._set('03','80000101',0) )     
        FrameLib.respondCheck()  # 会检查之前所有的需回复消息，检查完毕后结束
        
    gevent.joinall([  #开启三个协程 
        gevent.spawn(form.log_recv, *(form.clientP, )),  # 接收1_2P
        gevent.spawn(form.log_recv, *(form.clientS, )),  # 接收1_16S
        gevent.spawn(confSet),
    ])
