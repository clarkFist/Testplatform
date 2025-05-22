# !/usr/bin/python
# _*_ coding: utf-8 _*_
from __future__ import print_function
#import socket
import sys, os
if len(sys.argv) == 1 or 'server' != sys.argv[1]:
    from FuncLog import FuncLog
    funclog = FuncLog()  #设定funclog保存路径
    RUN_MODE = 'client'
else:
    from gevent import monkey
    monkey.patch_all()
    RUN_MODE = 'server'

    def funclog(func):
        return func
import gevent
from gevent import socket, subprocess
from gevent.lock import BoundedSemaphore

import time
import json
import re
import shutil
import hashlib
import io
from datetime import datetime
from inspect import getframeinfo, stack
from collections import OrderedDict
def removeCCppComment( text ):

    def blotOutNonNewlines( strIn ):  # Return a string containing only the newline chars contained in strIn
        return "" + ("\n" * strIn.count('\n'))

    def replacer( match ):
        s = match.group(0)
        if s.startswith('/'):  # Matched string is //...EOL or /*...*/  ==> Blot out all non-newline chars
            return blotOutNonNewlines(s)
        else:                  # Matched string is '...' or "..."  ==> Keep unchanged
            return s

    pattern = re.compile(
        r'//.*?$|/\*.*?\*/|\'(?:\\.|[^\\\'])*\'|"(?:\\.|[^\\"])*"',
        re.DOTALL | re.MULTILINE
    )

    return re.sub(pattern, replacer, text)

def bytes(msg, encoding):
    return msg.encode(encoding)

def get_desktop():
    return os.path.join(os.path.expanduser('~'), 'Desktop')

def getPath_TestPlatform():
    ''''''
    if sys.version_info < (3, 0):
        import _winreg as winreg
    else:
        import winreg
    _key = winreg.OpenKey(winreg.HKEY_CURRENT_USER, r'Environment')
    value, ty = winreg.QueryValueEx(_key, 'VCU_TEST')
    return value

class VMConfig:
    #CONFIG_START
    server_addr = ('20.3.1.50', 18125) # VM 端 server ip
    name_dict = {'svcu': 'VCU_DEV', 'mvcu': 'VCU_DEV_MVCU'} # 代码在VMtools 的 名称
    path_dict = {} # 客户端在启动时会共享path_dict 至服务端
    if RUN_MODE != 'server': 
        path_dict[get_desktop()] = r'F:' # 虚拟机的桌面共享文件夹
        path_dict[getPath_TestPlatform()] = r'F:\testplaatform1025\Master\testplatform' #虚拟机内测试平台的共享文件夹（Master/testplatform）
    #CONFIG_END

    @classmethod
    def path_transform(cls, path, RUN_MODE = RUN_MODE):
        def transform1(path):
            path = re.sub(r'\s', '', path)
            return re.sub(r'\\', '/', path)
        path = transform1(path)
        my_path_dict = {}
        for key, value in cls.path_dict.items():
            key = transform1(key)
            value = transform1(value)
            my_path_dict[key] = value

        if RUN_MODE == 'server': # 将虚拟机路径转为外部路径
            for key, value in my_path_dict.items():
                pattern = re.compile(r'^%s' % value, re.I)
                rt_path = re.sub(pattern, key, path)
                if rt_path != path:
                    break
        else: # 将外部路径转为虚拟机路径
            for key, value in my_path_dict.items():
                pattern = re.compile(r'^%s' % key, re.I)
                rt_path = re.sub(pattern, value, path)
                if rt_path != path:
                    break
        if rt_path == path:
            raise Exception()
        return rt_path

    @staticmethod
    def gettime():
        '''fmt:%Y-%m-%d %H:%M:%S'''
        return datetime.now().strftime('%Y-%m-%d %H:%M:%S')

    @classmethod
    @funclog
    def print_func(cls, *args, **kwargs):
        caller = getframeinfo(stack()[2][0])
        filename = caller.filename.split('\\')[-1]
        msg = cls.gettime() + " %s %s msg:" % (filename, caller.lineno)
        print(msg, end='')
        print(*args, **kwargs)

class MyTcp():

    def __init__(self, client):  
        if client:
            block = False
            timeout = 1
        else:
            block = True
            timeout = 5
        self.block = block
        self.timeout = timeout
        self.client = client
        self.hide = True

    def create(self, block = None, timeout = None):
        if block is None:
            block = self._block
        if timeout is None:
            timeout = self._timeout

        tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        tcp.setblocking(1 if block is True else 0)
        if timeout is not None:
            tcp.settimeout(timeout)
        return tcp
    
    def get_key(self, func, args):
        try:
            key = ','.join(args)
        except TypeError:
            temp = json.dumps(args)
            if len(temp) > 80:
                md5 = hashlib.md5()
                md5.update(temp.encode('utf8'))
                key = md5.hexdigest()
            else:
                key = temp
        return func + '_' + key

class Server(MyTcp):

    def __init__(self):
        self.VM_rootpath = os.path.abspath(os.getcwd())
        sys.path.append(self.VM_rootpath)
        self._block = True
        self._timeout = 5
        self.func_dict = {'cmd': self.on_cmd, 'request': self.on_requect}
        self.gevent_dict = {}
        self.state_dict = {}

    def listen(self):
        tcp = self.create()
        try:
            tcp.bind(VMConfig.server_addr)
        except Exception:
            exit(50)
        VMConfig.print_func("Socket bind complete")
        tcp.listen(10)
        while True:
            try:
                conn, addr = tcp.accept()
            except socket.timeout:
                continue
            buff = conn.recv(4096)
            
            buff = buff.decode()
            data_loaded = json.loads(buff)
            msg = self.func_dict[data_loaded['type']](data_loaded)
            conn.send(bytes(msg, 'utf8'))
            conn.shutdown(1)
        tcp.close()

    def on_cmd(self, data_loaded):
        func = data_loaded['func']
        args = data_loaded['args']
        key = self.get_key(func, args)

        def run(key, func, *args, **kwargs):
            rt = func( *args, **kwargs)
            return rt
        func = eval('self.%s' % func)

        if key in self.gevent_dict.keys() and (not self.gevent_dict[key].dead):
            self.gevent_dict[key].kill()

        self.gevent_dict[key] = gevent.spawn(run, key, func, *args)
        return key
     
    def on_requect(self, data_loaded):
        '''可能需要'''
        func = data_loaded['func']
        args = data_loaded['args']
        key = self.get_key(func, args)
        if key in self.gevent_dict.keys():
            g = self.gevent_dict[key]
            if g.dead:
                try:
                    rt = g.get()
                except Exception as e:
                    err_msg = repr(e)
                    msg = json.dumps({'state': 'err', 'msg': err_msg})
                else:
                    if rt is None:
                        msg = json.dumps({'state': 'err', 'msg': 'the func has no returncode'})
                    elif rt[0] == 0:
                        msg = json.dumps({'state': 'success', 'msg': rt[1]})
                    else:
                        msg = json.dumps({'state': 'fail', 'msg': rt[1]})  
            else:
                msg = json.dumps({'state': 'running', 'msg': ''})
        else:
            msg = json.dumps({'state': 'err', 'msg': 'cant get %s' % key})
        return msg

    def build(self, name):
        try:
            self.state_dict.get('copy_src', BoundedSemaphore(1)).wait(20)
        except TimeoutError:
            return 1, 'copy_src is not normal ending'

        assert(self.state_dict.setdefault('build', BoundedSemaphore(1)).wait(280) > 0)
        with self.state_dict['build']:
            name = name.lower()
            cmdname = os.path.join(self.VM_rootpath, 'msyslog.py')
            from msyslog import build
            cd_path = os.path.join(self.VM_rootpath, VMConfig.name_dict[name])
            VMConfig.print_func('start build', cmdname, cd_path)
            #p = subprocess.Popen('python %s' % cmdname, shell = False, cwd=cd_path, creationflags =subprocess.CREATE_NEW_CONSOLE)
            #
            try:
                rt, msg = build(cd_path)
            except Exception as e:
                rt = 2
                msg = repr(e)

            VMConfig.print_func('build', cmdname, cd_path, 'end with %s %s' % (rt, msg))
        return rt, msg

    def copy_image(self, name, dstpath = None):
        
        if dstpath is not None:
            pass
        else:
            with open('%s/copy-s19-RUT-VCU.bat' % (self.VM_rootpath), 'r') as f:
                content = f.read()
                temp = re.search(r'^\s*set\s+dst=(.*)$', content, re.M)
                dstpath = temp.group(1)

        out_path = os.path.join(self.VM_rootpath, VMConfig.name_dict[name], 'build/out')
        VMConfig.print_func('start copy_image from %s to %s' % (out_path, dstpath))

        rt_code = 0
        p1 = subprocess.Popen('python %s/copyfromVM.py %s' % (self.VM_rootpath, name), cwd='%s/%s' % (self.VM_rootpath, VMConfig.name_dict[name]), 
                              stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        p1.wait(15)
        rt_code = p1.returncode
        # if rt_code == 0:
        #     p2 = subprocess.Popen('python %s/copy-s19-RUT-VCU.bat %s' % (self.VM_rootpath, name), cwd='%s/%s' % (self.VM_rootpath, VMConfig.name_dict[name]), 
        #                           stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        #     p2.wait(15)
        #     rt_code += p2.returncode
        sub_paths = ['CPU_A', 'CPU_B']
        for sub_path in sub_paths:
            VMConfig.print_func(out_path + '/' + sub_path )
            try:
                dir_list = os.listdir( out_path + '/' + sub_path )
            except Exception as e:
                raise e.__class__('os.listdir %s/%s err: %s' % (out_path, sub_path, repr(e)))
            for filename in dir_list:
                if filename.endswith('.S19') or filename.endswith('.elf') or filename.endswith('.bin') or filename.endswith('.rbin') or filename.endswith('.lzw'):
                    if name.startswith('mvcu'):
                        dstfilename = filename.replace('NOETH.', 'INTA.')
                    else:
                        dstfilename = filename
                    dstfilename = dstfilename.replace('.rbin', '.bin')
                    VMConfig.print_func( os.path.join(out_path, sub_path, filename), os.path.join(dstpath, sub_path, dstfilename))
                    srcpath_temp = os.path.join(out_path, sub_path, filename)
                    dstpath_temp = os.path.join(dstpath, sub_path, dstfilename)
                    try:
                        shutil.copyfile(srcpath_temp, dstpath_temp  )
                    except IOError as e:
                        raise e.__class__('copyfile %s %s err: %s' % (srcpath_temp.replace( '\\', '/'), dstpath_temp.replace( '\\', '/'), repr(e)))
        if rt_code == 0 or True: # else 分支存在问题， 待优化
            VMConfig.print_func('copy_image from %s to %s success' % (out_path, dstpath))
            msg = '%s to %s' % (out_path, dstpath)
        else:
            msg = p1.stdout.read()
            msg = str(msg)[2:]
            VMConfig.print_func(msg)
            
        return 0, msg # rt_code 不为0 时无影响， 操作错误会出异常

    def copy_src(self, name):
        #if self.state_dict.get('build', False) is True:
        #    return 1, 'is building'
        
        p = subprocess.Popen('%s/Copy_SrcToVM.bat %s' % (self.VM_rootpath, name), cwd='%s/%s' % (self.VM_rootpath, VMConfig.name_dict[name]), 
                             stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        p.stdin.write('\n\n\n')
        p.wait(30)
        code_dict = {0: '', 11: 'md %s err' % name, 12: 'xcopy %s err' % (name)}
        content = p.stdout.read().decode('gbk')
        temp = re.search(r'\s*src:([^\r\n]*)\r*\ndst:([^\r\n]*)\r*\n', content, re.S)
        if temp is None:
            raise Exception('src:([^\r\n]*)\r*\ndst:([^\r\n]*)\r*\n fail')
        srcpath = temp.group(1)
        dstpath = temp.group(2)

        msg = code_dict.get(p.returncode, 'Copy_SrcToVM.bat err')

        msg += ' %s to %s' % (srcpath, dstpath)
        VMConfig.print_func(msg)

        VMConfig.print_func('copy_src %s end with %s %s' % (name, p.returncode, msg))
            
        return p.returncode, msg

    def openTagInit(self, name, Tag_list):

        temp_list = str(Tag_list)
        assert(self.state_dict.setdefault('openTagInit', BoundedSemaphore(1)).wait(30) > 0)
        with self.state_dict.setdefault('openTagInit', BoundedSemaphore(1)):
            assert(isinstance(Tag_list, list))
            temp = os.path.join(self.VM_rootpath, VMConfig.name_dict[name])
            dstpath = temp + '/' + 'src'
            if os.path.exists(temp + '/guest/demo/src' ):
                dstpath = temp + '/guest/demo/src'
            
            dstpath = '%s/tst/tst_vcu_stubs.h' % dstpath
            try:
                with io.open(dstpath, 'r', encoding='utf8') as f:
                    self.tst_vcu_stubs_h = f.read()
                my_encoding = 'utf8'
            except UnicodeDecodeError:
                with io.open(dstpath, 'r', encoding='gbk') as f:
                    self.tst_vcu_stubs_h = f.read()
                my_encoding = 'gbk'
            content = removeCCppComment(self.tst_vcu_stubs_h)
            pattern = re.compile(
                r'^\s*#define\s+(\w+_\w+_\w+_\w+)\s+([TRUEFALS]+)', re.M)
            if len(Tag_list) == 0:
                VMConfig.print_func('close all init tag ')

            def replacer( match ):   
                name = match.group(1)
                state = match.group(2)
                if name in Tag_list:
                    state = 'TRUE'
                    VMConfig.print_func('open %s' % name)
                    Tag_list.remove(name)
                else:
                    state = 'FALSE'
                dst_value = '#define\t%s\t%s' % (name, state)

                count = [0]

                def getcount():
                    return count

                def sub_replacer( match ):
                    getcount()[0] += 1
                    return dst_value
                self.tst_vcu_stubs_h = re.sub(r'#define\s+%s\s+[TRUEFALS]+' % name, sub_replacer, self.tst_vcu_stubs_h)
                if count[0] != 1:
                    VMConfig.print_func('openTagInit %s has %s times' % (name, count[0]))
                    raise Exception(count)
                return dst_value
            re.sub(pattern, replacer, content) 
            if len(Tag_list) > 0:
                VMConfig.print_func('openTagInit %s not found' % (str(Tag_list) ))
                raise Exception(Tag_list)
            with io.open(dstpath, 'w', encoding=my_encoding) as f:
                f.write(self.tst_vcu_stubs_h)
            VMConfig.print_func('openTagInit %s success' % (temp_list ))
            return 0, ''
    
    def set_VM_path(self, args):
        VMConfig.path_dict = {}
        for key, value in args:
            VMConfig.path_dict[key] = value
        VMConfig.print_func(VMConfig.path_dict)
        return 0, ''

class Client(MyTcp):
    send_time = 0

    def __init__(self):
        self._block = True
        self._timeout = 5
        self.set_VM_path_flag = False

    @funclog
    def send(self, msg):
        if self.set_VM_path_flag is False:
            self.set_VM_path_flag = True
            self.set_VM_path()
        while time.time() - Client.send_time < 1:
            gevent.sleep(0.3)
        tcp = self.create()
        try:
            tcp.connect(VMConfig.server_addr)
        except ConnectionRefusedError:
            raise ConnectionRefusedError('no vmtool server in VM')
        tcp.send(bytes(msg, 'utf8'))
        Client.send_time = time.time()
        gevent.sleep(0.01)
        buff = tcp.recv(4096)
        tcp.close()
        
        return buff.decode('utf8')
    
    @funclog
    def _send_cmd(self, cmd, args):
        assert( isinstance(args, tuple) )
        msg = json.dumps({'type': 'cmd', 'func': cmd, 'args': args})
        buff = self.send(msg)
        key = self.get_key(cmd, args)
        
        if buff != key:
            VMConfig.print_func(buff, key)
            raise Exception(buff, key)
        else:
            VMConfig.print_func(buff)
    
    def get_request(self, cmd, args):
        msg = json.dumps({'type': 'request', 'func': cmd, 'args': args})
        buff = self.send(msg)
        data_loaded = json.loads(buff)
        state = data_loaded['state']
        msg = data_loaded['msg']
        return state, msg
    
    @funclog
    def openTagInit(self, name, Tag_list):
        if sys.version_info > (3, 0):
            unicode = str

        if isinstance(Tag_list, str) or isinstance(Tag_list, unicode):
            if len(Tag_list) == 0:
                Tag_list = []
            else:
                Tag_list = Tag_list.split(',')

        assert(isinstance(Tag_list, list))
    
        func, args = 'openTagInit', (name, Tag_list)
        self._send_cmd(func, args)
        state, msg = self.get_request(func, args)
        for i in range(30):
            gevent.sleep(1)
            state, msg = self.get_request(func, args)
            if state == 'running':
                continue
            elif state == 'success':
                break
            else:
                raise Exception(state, msg)
        else:
            raise Exception(state, msg)
        VMConfig.print_func(state, Tag_list)
        return func, args

    @funclog
    def build(self, name, block = True):
        func, args = 'build', (name,)
        self._send_cmd(func, args)
        if block:
            for i in range(150):
                gevent.sleep(2)
                state, msg = self.get_request(func, args)
                if state == 'running':
                    continue
                elif state == 'success':
                    break
                else:
                    raise Exception(state, msg)
            else:
                raise Exception(state, msg)
        VMConfig.print_func(state, msg)
        return func, args

    @funclog
    def copyImage_fromVM(self, name, dstpath = ''):
        if dstpath == '':
            func, args = 'copy_image', (name,)
        else:
            dstpath = VMConfig.path_transform(dstpath)
            func, args = 'copy_image', (name, dstpath)
        self._send_cmd(func, args)
        state, msg = self.get_request(func, args)
        for i in range(30):
            gevent.sleep(1)
            state, msg = self.get_request(func, args)
            if state == 'running':
                continue
            elif state == 'success':
                break
            else:
                raise Exception(state, msg)
        else:
            raise Exception(state, msg)
        VMConfig.print_func(state, msg)
        res = re.search(r'\sto\s*(.*)\s*$', msg)
        dstpath = res.group(1)
        dstpath = VMConfig.path_transform(dstpath, RUN_MODE = 'server') 
        VMConfig.print_func('image_path: %s ' % dstpath)
        if hasattr(self, 'rut'):
            self.rut.set_update_default_path(dstpath)
        return func, args

    @funclog     
    def copySrc_toVM(self, name, srcpath = ''):
        if srcpath == '':
            func, args = 'copy_src', (name,)
        else:
            func, args = 'copy_src', (name, srcpath)
        self._send_cmd(func, args)
        for i in range(30):
            gevent.sleep(1)
            state, msg = self.get_request(func, args)
            if state == 'running':
                continue
            elif state == 'success':
                break
            else:
                raise Exception(state, msg)
        else:
            raise Exception(state, msg)
        VMConfig.print_func(state, msg)
        return func, args

    @funclog
    def set_VM_path(self):
        func = 'set_VM_path'
        args = list(VMConfig.path_dict.items())
        args = (args,)
        self._send_cmd(func, args)
        for i in range(30):
            gevent.sleep(1)
            state, msg = self.get_request(func, args)
            if state == 'running':
                continue
            elif state == 'success':
                break
            else:
                raise Exception(state, msg)
        else:
            raise Exception(state, msg)
        VMConfig.print_func(state, msg)
        return func, args
        
def sys_args(sysargs = sys.argv[1:]):
    title = sysargs[0]
    filepath = sysargs[1]
    cwd = sysargs[2]
    funcargs = sysargs[3]
    return title, filepath, cwd, funcargs
def server():
    server = Server()
    server.listen()
def vm_build_update(slots, open_initTag = None):
    sys.path.append( os.path.abspath(os.path.dirname(__file__)) + '/../RUT')
    import light_update
    slot_list = re.split(r'[., ]', str(slots))
    for slot in slot_list[:]:
        if '-' in slot:
            slot_list.remove(slot)
            slot = slot.split('-')
            slot_list += list(range(int(slot[0]), int(slot[1]) + 1))
            
    temp_len = len(slot_list)
    
    for i in slot_list[:]:
        int(i)
        if '2' == i or '16' == i:
            slot_list.remove(i)
    
    new_len = len(slot_list)
    if temp_len == 0:
        raise Exception(slots)
    elif new_len == 0:
        cmd_type = 'mvcu'
    elif new_len == temp_len:
        cmd_type = 'svcu'
    else:
        raise Exception(slots)

    vmtool = Client()
    vmtool.rut = light_update
    vmtool.copySrc_toVM(cmd_type)
    if open_initTag is not None:
        vmtool.openTagInit(cmd_type, open_initTag)
    vmtool.build(cmd_type)
    vmtool.copyImage_fromVM(cmd_type)
    light_update.update('auto', 6, slots)

if __name__ == "__main__":
    if len(sys.argv) > 1:
        if 'server' == sys.argv[1]:
            #os.chdir(cwd)
            server()
        if 'copy_build' == sys.argv[1]:
            if len(sys.argv) > 3:
                open_initTag = sys.argv[3]
            else:
                open_initTag = None
            vm_build_update(sys.argv[2], open_initTag)
    else:
        if sys.version_info > (3, 0):
            raw_input = input
        
        slots = raw_input('input slots:')
        vm_build_update(slots)