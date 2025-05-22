# !/usr/bin/python
# _*_ coding: utf-8 _*_
#from gevent import monkey
#monkey.patch_all()
import re, sys
import os
import gevent
import serial
import serial.tools.list_ports
from sub_module_base import SubMoudleBase
if sys.version_info < (3, 0):
    import Queue
else:
    import queue as Queue
import time
from FuncLog import FuncLog
funclog = FuncLog()  #设定funclog保存路径
port_line_dict = {}
def getline(data, port=''):
    global port_line_dict

    '''将字符串按'\n'分割,并将最后一行与下一次的参数合并'''
    port_line_dict.setdefault(port, str(''))
    data = str(data)#将bytes 编码为utf-8字符串
    data = re.sub('\x1b[^mABCDHJKsuIh]*[mABCDHJKsuIh]', '', port_line_dict[port] + data)#去除终端控制符
    data = re.sub('\r', '', data)
    data = re.sub('\n\n', '\n', data)
    lines = data.split('\n')
    port_line_dict[port] = lines.pop()#将可能未处理完成的最后一行与下一次读取的数据合并处理
    return lines

class myQueue(Queue.Queue):
    def put(self, *args, **kwargs):
        if self.qsize() > 1000:
            for i in range(500):
                self.get()
        super(myQueue, self).put(*args, **kwargs)

class SerialRead(SubMoudleBase):
    '''description:用于处理串口通信
    
    Attributes:
    
    Arguments:
      :
    '''
    def __init__(self, **kwargs):
        self.conn_dict = {}
        self.queue_dict = {}
        self.greenlet_list = []
    
    def open_all(self, outdirname = 'serial_log', reOpen = False):
        if outdirname is not None:
            outdirname = self.get_path(outdirname)
            if not os.path.exists(outdirname):
                os.mkdir(outdirname)

        serial_list = serial.tools.list_ports.comports()
        for port in serial.tools.list_ports.comports():
            if 'serial' not in port.description.lower():
                serial_list.remove(port)
        if len(serial_list) == 0:
            self.print('no port is avaliable')
        for port in serial_list:
            port = port.device
            if port in self.conn_dict.keys():
                if reOpen is False:
                    raise Exception('port %s already open' % port )
                else:
                    self.port_close(port)
            self.queue_dict[port] = myQueue()
            if outdirname is None:
                outfile = None
            else:
                outfile = outdirname + '/%s.log' % port
            
            funclog.wirte(port, outfile)
            g = gevent.spawn(self.port_open, port, outfile)
            self.greenlet_list.append(g)

    def joinall(self, timeout):
        start = time.time()
        if timeout is None:
            timeout = 0xFFFFFFFF
        while time.time() - start < timeout:
            for g in self.greenlet_list[:]:
                if g.dead:
                    try:
                        g.get()
                    except Exception as e:
                        raise e
                    self.greenlet_list.remove(g)
                gevent.sleep(1)

    def close_all(self):
        for port, conn in self.conn_dict.items():
            conn.close()
        self.conn_dict = {}
        self.greenlet_list.clear()

    def port_open(self, port, outfile, buand = 115200, timeout = 0):
        try:
            conn = serial.Serial(port, buand)
        except Exception:
            self.print('port %s cannot open' % port )
            return
        # if conn.isOpen():
        #     raise Exception('port %s already open' % port )
        #conn.open()
        self.conn_dict[port] = conn
        conn.timeout = timeout
        buf = conn.read(size=2048)
        buf = buf.decode()
        buf = re.sub(r'^[^\n]*\n', '', buf)
        while conn.isOpen():
            try:
                buf = conn.read(size=2048)
            except serial.PortNotOpenError:
                continue
            except serial.SerialException as e:
                conn.close()
                self.print('exception %s at %s' % (port, repr(e)))
                continue
            buf = buf.decode()
            gevent.sleep(0.2)
            for line in getline(buf, port):
                if len(line) < 2:
                    continue
                self.queue_dict[port].put(line)
            if outfile is not None:
                with open(outfile, 'w') as f:
                    f.write( '\r\n'.join(getline(buf)) )

    def port_close(self, port):
        self.conn_dict[port].close()
        self.conn_dict.pop(port)
if __name__ == '__main__':
    pass