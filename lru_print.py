from __future__ import print_function
import sys, os
import socket
import time
import traceback
import json
from udp_loglib import gevent
from gevent import monkey 
monkey.patch_all()
def resovleArgv(argv):
    if len(argv) > 0:
        path = argv[0]
        title = argv[1]
    return path,title
# 接受消息
def recv():
    global name
    global client
    global Lru
    global is_exit
    left = ''
    while not is_exit:
        time.sleep(0.1)
        try:
            datas = client.recv(1024)
        except socket.timeout:
            continue
        except socket.error as e:
            raise e
        datas = left + datas.decode()
        json_s = datas.split('//r//r')[:-1]
        left = datas.split('//r//r')[-1]
        for data in json_s:#每个json数据的分隔符
            try:
                data_loaded = json.loads(data)
            except Exception as e:
                print(e,data)
                while True:
                    pass
            lru = str(data_loaded['name'])
            msg = data_loaded['value']
            ty = str(data_loaded['type'])
        #来自server的消息为close时结束程序
            if lru != name:
                raise Exception()
            elif ty == 'close':
                client.shutdown(socket.SHUT_RDWR)
                client.close()
                is_exit = True
                break
            else:
                print(data_loaded['value'])
    is_exit = True #线程退出
    
def heartbeat():
    global client
    global name
    global is_exit
    while not is_exit:
        msg = json.dumps({'name': name,'type':'heartbeat', 'value':''}) + '//r//r'
        if sys.version_info > (3,0):
            msg = bytes(msg,'utf8')
        else:
            msg = bytes(msg)
        try:
            client.send(msg)
        except socket.error as e:
            print(e)
            client.conn.shutdown(2)
            client.conn.close()
            break
        time.sleep(3)
    is_exit = True
            
if __name__ == "__main__":
    normal_flag = None
    try:
        port = 19000
        path, title = resovleArgv(sys.argv[1:])
        #title = '1_3'
        name = title
        os.system('title %s' % title)
        is_exit = False
        retry_num = 0
        if os.path.exists(path):
            with open(path, 'r') as f:
                lines = f.readlines()
                if len(lines) > 200:
                    lines = lines[-200:]
                for line in lines:
                    if len(line) > 5:
                        print(name, line)
    except Exception as e:
        normal_flag = e
        is_exit = True
        
    while not is_exit:
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)#创建tcp连接
        try:
            remote_ip = '127.0.0.1'#远端ip 127.0.0.1
            client.connect((remote_ip, port))
            print("Socket connected to ip %s:%s" % (remote_ip, port))
        except socket.error as e:  #address related error
            time.sleep(1)#连接失败尝试重连
            retry_num += 1
            if retry_num > 9:
                is_exit = True
            normal_flag = e
            continue
        else:
            client.setblocking(0)   
            client.settimeout(2)
            normal_flag = True
            break
    if normal_flag is True:
        gevent.joinall([gevent.spawn(recv),
                       gevent.spawn(heartbeat)
                       ])
    else:
        dst = 'Log_Log/subProcess'
        if not os.path.exists(dst):
            os.makedirs(dst)
        with open(dst + '/%s.log' % name, 'w') as f:
            traceback.print_exception(normal_flag, file=f)
