#coding=utf-8
from __future__ import print_function
import sys, os
import subprocess
import time
import re
from sub_module_base import SubMoudleBase
from FuncLog import FuncLog
import shutil
funclog = FuncLog()  #设定funclog保存路径
class Wireshark(SubMoudleBase):
    '''网络抓包'''

    def __init__(self,
                 wireshark_path,
                 ethdesc,
                 count=None,
                 timeout=None,
                 outfile=None,
                 fifter=None,
                 **kwargs):
        '''
            wireshark_path：str 本机wireshark 的路径
            count:int 捕捉到 count 条报文后停止
            timeout:int 一次start 最大持续 timeout s
            outfile:str 报文保存路径, 非指定路径不支持':',普通路径将会保存在Log 指定的路径
            fifter:str 过滤，和wireshark 格式不同 比如：“udp src port not 50000 and udp src port not 50001”
            ethdesc:str 目标网卡的描述，非名称
            **kwargs 为实现一些自定义功能的参数，只在父类使用
        '''
        self.cmd_dict = {}  # 默认值
        #self.cmd_dict['-p'] = ''
        self.wireshark_path = wireshark_path
        if count is not None:
            self.cmd_dict['-c'] = count
        if timeout is not None:
            self.cmd_dict['-a'] = 'duration:%s' % timeout
        if outfile is not None:
            self.cmd_dict['-w'] = '"%s"' % self.get_path(outfile)
        if fifter is not None:
            self.cmd_dict['-f'] = '"%s"' % fifter

        self.ethdesc = ethdesc
        self.start_dict = {}
        
    def __find_Conn(self):
        '''根据self.ethdesc 找到目的网卡'''

        if not hasattr(self, '_Conn'):
            # p = subprocess.Popen('cmd.exe',
            #                      stdout = subprocess.PIPE,
            #                      stderr = subprocess.STDOUT,
            #                      stdin = subprocess.PIPE,
            #                      ) # 开始执行
            # p.stdin.write(b'ipconfig /all\n')
            # p.stdin.write(b'exit')
            # p.wait(2.5)

            # out = p.stdout.read()
            # out = out.decode('gbk')
            out = subprocess.getoutput('ipconfig /all')
            res = re.findall(r'以太网适配器\s*([^:：]+)[:：].*?描述\..*?[:：]\s*([^\r\n]+)', out, re.S)
            temp_dict = {}
            for patch in res:
                k = patch[1]
                v = patch[0]
                temp_dict[k] = v
                if k.startswith(self.ethdesc):
                    if not hasattr(self, '_Conn'):
                        self._Conn = v
                    else:
                        raise Exception('find two %s' % self.ethdesc)
            self.print('以太网适配器: %s' % self._Conn)
            #for k, v
        return self._Conn

    @funclog
    def start(self,
              count=None,
              timeout=None,
              outfile=None,
              fifter=None,
              block=True):
        '''
            count:int 捕捉到 count 条报文后停止
            timeout:int 一次start 最大持续 timeout s
            outfile:str 报文保存路径
            fifter:str 过滤，和wireshark 格式不同 比如：“udp src port not 50000 and udp src port not 50001”
            block:boolean 是否阻塞
        '''
        assert( block is False or isinstance(timeout, int)) # 若为阻塞必须设置超时

        cmd_dict = self.cmd_dict.copy()  # 实际值

        #self.cmd_dict['-q'] = ''
        if count is not None: # 命令参数可参考dumpcap
            cmd_dict['-c'] = count
        if timeout is not None:
            cmd_dict['-a'] = 'duration:%s' % timeout #支持两种停止方式，超时，文档size
        if outfile is not None:
            cmd_dict['-w'] = '%s' % self.get_path(outfile)
            #if os.path.exists(outfile) == False:
            self.print('%s' % self.get_path(outfile))
        if fifter is not None:
            cmd_dict['-f'] = '%s' % fifter
        cmd_dict['-i'] = self.__find_Conn()
        cmd_list = []
        for k, v in cmd_dict.items():
            cmd_list.append('%s%s' % (k, v))
        if os.path.exists("%s\\test_dumpcap.exe" % self.wireshark_path) is False:
            shutil.copyfile("%s\\dumpcap.exe" % self.wireshark_path, "%s\\test_dumpcap.exe" % self.wireshark_path)

        startupinfo = subprocess.STARTUPINFO(wShowWindow=subprocess.SW_HIDE)
        startupinfo.dwFlags = subprocess.CREATE_NEW_CONSOLE | subprocess.STARTF_USESHOWWINDOW
        p = subprocess.Popen(["%s\\test_dumpcap.exe" % self.wireshark_path] + cmd_list,
                             stdout = subprocess.PIPE,
                             stderr = subprocess.PIPE,
                             startupinfo = startupinfo,
                             creationflags =subprocess.CREATE_NEW_CONSOLE,
                             ) # 开始执行
        if ( count is None or count > 300) and \
           ( timeout is None or timeout > 30):
            time.sleep(1.5)
            if p.poll() is not None:
                if p.returncode != 0:
                    raise Exception(p.returncode) 
        if block is True:
            p.wait(timeout + 2)
            #out = p.stderr.read()
            #print(out)
            #out = out.decode('gbk')
            if p.returncode != 0:
                raise Exception(p.returncode) 
        return p  # 若非阻塞，可通过 p.poll() 判断是否停止，p.wait 
if __name__ == "__main__":
    def func_test():
        return '123'
    wire = Wireshark('', '', outfile='file', file_root_get = func_test)
    wire.start(timeout=5)