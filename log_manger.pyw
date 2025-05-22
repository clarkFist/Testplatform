# !/usr/bin/python
# _*_coding:utf-8_*_
from __future__ import print_function
import os, sys
# from gevent import monkey, socket, threading, Greenlet
import socket
import subprocess
import time
import json
import traceback
print(os.path.abspath('.'))

from py_lib import logprint
from py_lib.logprint import print
from udp_loglib import gevent
if sys.version_info < (3, 0):
    import Tkinter as tk
    import ttk
    import tkFileDialog as filedialog
else:
    import tkinter as tk
    from tkinter import ttk
    from tkinter import filedialog
listbox_temp = []
listbox_tail = ''
def box_print(msg,end='\n'):
    global listbox_temp
    global listbox_tail
    listbox_temp.append(listbox_tail + str(msg) + end)
    default_print(listbox_tail + str(msg),end=end)
    if hasattr(select_ui,'ui'):
        msg = ''.join(listbox_temp)
        lines = msg.split('\n')
        listbox_tail = lines[-1]
        for val in lines[:-1]:
            select_ui.ui.print_list_append(val)
        listbox_temp = []
default_print = logprint.default_print
logprint.default_print = box_print
class public(object):
    '''管理全局变量
    '''
    cur_open = ''
    cur_openTime = 0
    cur_close = ''
    BUF_SIZE = 4096
    client_dict = {}
    is_exit = False
    end_list = []
    path_dict = {}

    @classmethod
    def end_exe(cls):
        Log_manager.manager.server.shutdown(2)
        Log_manager.manager.server.close()
        Log_manager.manager.client.shutdown(2)
        Log_manager.manager.client.close()
        while True:
            to_close =  public.client_dict.keys()
            if len(to_close) == 0:
                break
            for lru in list(to_close)[:]:
                cls.cur_close = lru
                Log_manager.manager._sub_close(lru)
        for func in cls.end_list:
            func[0](*func(1))
        print('end')
    @classmethod
    def exit(cls,val = 1):
        try:
            cls.end_exe()
        except:
            pass
        exit(val)
class select_ui(object):

    def __init__(self):
        self.win = tk.Tk()
        #self.win.bind('<Destroy>',self._destroy)
        self.win.title('')
        self.row = 1
        self.column = 1
        self.return_val = {}
        self.frame1=tk.Frame(self.win,pady=10,padx=15)
        self.frame1.grid(row=0,column=0)
        self.win.geometry('320x320')
        self.value_dict = {}
        select_ui.value_dict = self.value_dict
        self.list1 = set()
        self.cycle = 0
    def _destroy(self):
        print('destroy')
        #self.win.destroy()
    def run(self):
        ttk.Button(self.frame1,text='全选',command=self._select_all).grid(row=0,column=0,sticky=tk.W)
        ttk.Button(self.frame1,text='反选',command=self._unselect_all).grid(row=0,column=1,sticky='w')
        ckv = tk.IntVar()
        ckv.set(1)
        self.win_top(ckv)
        ck = ttk.Checkbutton(self.frame1,text='置顶',variable=ckv,onvalue=1 ,offvalue=0,command=lambda  ckv=ckv:self.win_top(ckv))
        ck.grid(row=0,column=2,sticky='w')
        self.print_box()
        self.win.after(50,self._updata)
        select_ui.ui = self
        print('start')
        self.win.mainloop()
    def print_box(self):
        #for val in self.print_list:
        #   val.set('')
        self.boxVar = tk.StringVar()
        self.listbox = tk.Listbox(self.win,listvariable = self.boxVar)
        self.boxVar.set(self.print_list)
        self.listbox.config(height=20,width=100)

            #self.listbox.insert(0,val.get())
    @property
    def print_list(self):
        if not hasattr(self,'__print_list'):
            self.__print_list = ['']*10
        return self.__print_list
    def print_list_append(self,msg):
        for i,val in enumerate(self.__print_list[:-1]):
            #val.set(self.__print_list[i+1].get())
            self.__print_list[i] = self.__print_list[i+1]
        self.__print_list[-1] = msg
        index = 0
        for val in self.__print_list:
            if len(val) >0:
                break
            else:
                index += 1
        self.boxVar.set(self.__print_list[index:])
    def win_top(self,ckv):
        val = ckv.get()
        self.win.wm_attributes('-topmost',val)
    def list_add(self,val):
        self.list1.add(val)
    #反选
    def _unselect_all(self):
        for index,item in self.value_dict.items():
            self.value_dict[index].set('')
    #全选
    def _select_all(self):
        for index,item in self.value_dict.items():
            self.value_dict[index].set(index)
            a = self.value_dict[index].get()
    def select_get(self):
        self.return_val['select'] = [v.get() for v in self.value_dict.values() if len(v.get())>0]
        self.return_val['unselect'] = [v for v in self.value_dict.keys() if v not in self.return_val['select']]
        
        if public.cur_open == '':
            self.toOpen_list = [v for v in self.return_val['select'] if v not in public.client_dict.keys()]
            if len(self.toOpen_list)>0:
                public.cur_open = self.toOpen_list[0]
                python_path = '%s/python' % (os.path.dirname(__file__) + '/' + 'venv/Scripts')
                subprocess.Popen(python_path + ' lru_print.py %s %s'%(public.path_dict[public.cur_open],public.cur_open)
                                ,shell=False,cwd='.', )#creationflags =subprocess.CREATE_NEW_CONSOLE)
                public.cur_openTime = time.time()
        self.toClose_list = [v for v in public.client_dict.keys() if v in self.return_val['unselect']]
        if len(self.toClose_list)>0:
            public.cur_close = self.toClose_list[0]
            Log_manager.manager._sub_close(public.cur_close)

    def select_select(self,name,val):
        print(val)
        pass
    def _updata(self):
        s = time.time()
        self.todo()
        if time.time() - s > 0.2:
            print(time.time() - s)
        self.cycle += 1
        temp_list = list(self.list1)
        temp_list.sort()
        for index,item in enumerate(temp_list):
            if item not in self.value_dict.keys():
                self.value_dict[item] = tk.StringVar()
                #self.value_dict[item].set(item)
                self.value_dict[item].set('')
            ck = ttk.Checkbutton(self.frame1,text=item,variable=self.value_dict[item],onvalue=item ,offvalue='')
            ck.grid(row=index//3+1,column=index%3,sticky='w')
        listbox_row = ( len(temp_list) - 1 )//3 + 1 + 1
        self.listbox.grid(row =listbox_row, column=0,columnspan=15,sticky='w')
        to_remove = [v for v in self.value_dict.keys() if v not in self.list1]
        for v in to_remove:
            self.value_dict.pop(v)
        self.select_get()
        self.win.after(500, self._updata)

class Log_manager:
    def __init__(self):
        self.todo_list = [(self.listen, ()), (self.log_recv, ())]
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)#创建tcp连接
        client.setblocking(0)
        client.settimeout(0.01)
        client.setsockopt(socket.SOL_SOCKET, socket.SO_RCVTIMEO, 10)#设置接受超时
        self.client = client
        while not public.is_exit:
            try:
                client.connect(('127.0.0.1', 18999))
            except socket.error as e:
                print(e)
                gevent.sleep(5)
                continue
            else:
                print('connect')
                break
        try:
            #server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR,1 )#允许重复地址和端口
            server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server.setblocking(0)
            server.settimeout(0.01)
            server.setsockopt(socket.SOL_SOCKET, socket.SO_RCVTIMEO, 50)#设置接受超时
            self.server = server
        except socket.error as e:
            print("Error creating socket: %s" % e)
            public.exit()
        host = '127.0.0.1'
        port = 19000
        try:
            server.bind((host, port))
        except socket.error as e:
            print("Bind failed! " + str(e))
            public.exit()
        print("Socket bind complete")
        server.listen(15)
        Log_manager.manager = self

        self.__log_recv_temp = ''

    def log_recv(self):
        try:# 如果发生socket.error异常结束线程
            datas = self.client.recv(public.BUF_SIZE).decode()
            if len(datas) == 0:#长度为零代表客户端断开连接
                # print("clint close")
                exit(1)
        except socket.timeout:
            return
        except socket.error as e:
            print(e)
            public.exit(1)

        datas = self.__log_recv_temp + datas
        data_list = datas.split('//r//r')
        self.__log_recv_temp = data_list.pop()
        for data in data_list:#每个json数据的分隔符
            data_loaded = json.loads(data)
            lru = str(data_loaded['name'])
            msg = data_loaded['value']
            self.ui.list_add(lru)
            public.path_dict[lru] = data_loaded['path']
            self._log_transmit(lru, msg)

    def _log_transmit(self, lru, msg):
        global server
        if lru in public.client_dict.keys():
            msg = json.dumps({'name': lru, 'type':'log','value':msg}) + '//r//r'
            if sys.version_info > (3, 0):
                msg = bytes(msg, 'utf8')
            else:
                msg = bytes(msg)
            try:
                public.client_dict[lru].send(msg)
            except socket.error as e:
                print(e)
                self.on_ConnClose(lru)

    def _sub_close(self, lru):
        msg = json.dumps({'name': lru, 'type':'close','value':''}) + '//r//r'
        if sys.version_info > (3,0):
            msg = bytes(msg,'utf8')
        else:
            msg = bytes(msg)
        try:
            public.client_dict[lru].send(msg)
        except socket.error as e:
            print(e)
            # if public.cur_close == '':
            #     exit(1)
            # else:
            #     self.on_ConnClose(lru)

    def on_ConnClose(self, name, msg = ''):
        select_ui.value_dict[name].set('')
        conn = public.client_dict[name]
        conn.shutdown(2)
        conn.close()
        self.todo_list.remove((self.clients, (conn, name)))
        public.client_dict.pop(name)
    
    def clients(self, conn, name):
        try:# 如果发生socket.error异常结束线程
            datas = conn.recv(public.BUF_SIZE).decode()
            if len(datas) == 0:#长度为零代表客户端断开连接
                self.on_ConnClose(name, 'clients')
                return
        except socket.timeout:
            return
        except socket.error as e:
            print(e)
            self.on_ConnClose(name)
            return

        for data in datas.split('//r//r')[:-1]:#每个json数据的分隔符
            data_loaded = json.loads(data)
            recv_name = data_loaded['name']
            if name != recv_name:
                public.exit(1)

    def listen(self):
        try:
            if public.cur_open != '' and (time.time() - public.cur_openTime > 5):
                select_ui.value_dict[public.cur_open].set('')
                public.cur_open = ''
            conn, addr = self.server.accept()
        except socket.timeout:
            return
        if public.cur_open == '':
            public.exit(1)
        conn.settimeout(0.02)
        public.client_dict[public.cur_open] = conn
        self.todo_list.append((self.clients, (conn, public.cur_open)))
        public.cur_open = ''
if __name__ == "__main__":
    print(os.path.abspath('.'))
    manager = Log_manager()
    ui = select_ui()
    manager.ui = ui
    def todo(self=manager):
        g = []
        for func in self.todo_list:
            g.append(gevent.spawn(func[0],*func[1]))
        gevent.joinall(g)
    ui.todo = todo
    ui.run()
    public.end_exe()