# !/usr/bin/python
# _*_ coding: utf-8 _*_
from __future__ import print_function
import sys, os
from udp_loglib import Log, Conf, Tag, TestPlatForm, TcpServer, gevent, FrameLib
from udp_loglib import g_sleep as sleep, myAssert
import udp_loglib
from simulator import Simulator
import time

time.sleep = udp_loglib.gevent.sleep

def init():
    if sys.version_info < (3, 0):
        import tkFileDialog as filedialog
    else:
        from tkinter import filedialog
    #path = filedialog.askdirectory()
    #if len(path) > 0:
    #   Log.path_set(path)
    if False:  # 配置log管理界面
        tcp = TcpServer('127.0.0.1', 18999)
        Log.tcp = tcp
        os.system('start %s/pythonw log_manger.pyw' % (os.path.dirname(__file__) + '/' + 'venv/Scripts'))
        pass


def log_path(arg):
    p1 = arg[:4]
    p2 = arg[4:6]
    path = "%s/Log_Log/%s/%s" % (os.path.dirname(__file__), p1, p2)
    Log.path_set(path)


def tag_send(slot, arg):
    log_path(arg)
    TestPlatForm.form.tst_set(slot, Tag.tag_send(arg))


if __name__ == "__main__":
    '''
        confSet : 在此函数内进行桩操作或变量配置
    '''
    init()
    form = TestPlatForm(18125, 18125, set_client=False)  # 18125 为默认端口号

    form._set_client(mode=[2])

    def confSet():
form.Serial_Flag = True
        form.update('ver', 5, '', savepath=Log.path_get())
        form.tst_set(2, Tag.tag_send('000105'))
        #return
        while True:
            try:
                cmd = udp_loglib.Public.wait_to_ack_ext('请输入命令', timeout=None)
            except udp_loglib.Public.myException:
                pass
            else:
                if len(cmd) > 0:
                    if cmd == 'break':
                        break
                    elif cmd == 'exit':
                        break
                    else:
                        try:
                            eval(cmd)
                        except Exception as e:
                            udp_loglib.Public.mPrint(str(e))
            finally:
                gevent.sleep(5)

    udp_loglib.gevent_join(confSet)
