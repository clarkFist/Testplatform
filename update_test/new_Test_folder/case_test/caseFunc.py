#!/usr/bin/python
#encoding:utf-8
import sys, os
import re
paths = os.path.abspath(__file__).split('\\')
form_path = '\\'.join(paths[:-2])
if form_path not in sys.path:
    sys.path.append( form_path )
from udp_loglib import Log, Conf, Tag, input_ui, FrameLib, Public, VCU, Simulator, Wireshark, myAssert, funclog
from udp_loglib import g_sleep as sleep, threading, subprocess, time, gevent, socket
import udp_loglib
gevent.monkey.patch_all()
from autopower import AutoPower
if sys.version_info < (3, 0):
    import Queue
else:
    import queue as Queue
class Config:
    form = None #type: udp_loglib.TestPlatForm
    slot_ip = udp_loglib.Config.slot_ip #type: dict
    
    #CONFIG_START
    @staticmethod
    def active_mvcu() -> str:
        '2, 2,16 16'
        return '16'

    @staticmethod
    def active_svcu() -> str:
        return '8'

    @staticmethod
    def get_normal_image_path():
        '''获取当前正常镜像的路径'''
        return os.path.abspath('%s/normal_image' % (os.path.dirname(__file__)))

    @staticmethod
    def case_path() -> list:
        '''设定本次执行的用例目录，以及n 级子目录'''
        path1 = os.path.dirname(__file__) + '\\' + 'testcase' 

        deep = 4 # 包含n级子目录
        rt_path = []
        rt_path.append(path1)

        def find_dir(path, deep):
            rt_path = []
            if deep > 0:
                for p in os.listdir(path):
                    p = path + '\\' + p
                    if os.path.isdir(p):
                        rt_path.append(p)
                        rt_path += find_dir(p, deep - 1)
            return rt_path
        rt_path += find_dir(path1, deep)
        return rt_path # 本次自动执行的脚本路径

    @classmethod
    def set_form(cls, form: udp_loglib.TestPlatForm):
        cls.form = form

    @staticmethod
    def get_suffix() -> list:
        '''执行时，用例名称的结尾需满足返回的列表的条件之一，忽略大小写'''
        return ['.PY'] # 本次自动执行的脚本 格式（后缀）
    
    @staticmethod
    def get_prefix() -> list: 
        '''执行时会将用例名称按_分割，分割后的字符需满足返回的所有条件，忽略大小写'''
        return ['SwRTC', 'cfg2'] # 本次自动执行的脚本的前缀

    #CONFIG_END
#初始化read_dict
read_dict = {}  #保存数据
class StubRequirement:
    def __init__(self, slots = None, ):
        self.slots = slots

class myQueue(Queue.Queue):
    def put(self, *args, **kwargs):
        if self.qsize() > 300:
            for i in range(200):
                try:
                    self.get(timeout=0)
                except Exception:
                    break
        super(myQueue, self).put(*args, **kwargs)

def read_dict_get(key) -> myQueue:
    if isinstance(key, tuple):
        point = read_dict
        for k in key:
            point[k] = point.get(k, {'queue': myQueue(500)})
            point = point[k]
        return point['queue']
    else:
        return read_dict_get((key,))

def power_restart():
    AutoPower.sendPowerOffCommand("1,2,3,4")
    time.sleep(2)
    AutoPower.sendPowerOnCommand("1,2,3,4")
    time.sleep(100)

def vm_build_update(slots, open_initTag = None):
    if not isinstance(slots, list):
        slot_list = VCU.inputsplit(slots)
    else:
        slot_list = slots
    temp_len = len(slot_list)
    
    for i in slot_list[:]:
        assert(isinstance(i, int))
        if 2 == i or 16 == i:
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

    form = Config.form
    vmtool = form.vmtool
    vmtool.copySrc_toVM(cmd_type)
    if open_initTag is not None:
        vmtool.openTagInit(cmd_type, open_initTag)
    vmtool.build(cmd_type)
    vmtool.copyImage_fromVM(cmd_type)
    form.update('S19', 0, slots)


def nonennn():
    '''
    ip_opera_dict ={}
    def unplug_IP(arg):
        global Config.eth_name
        global ip_opera_dict
        #os.system('netsh interface IP add address name="%s" addr=%s mask=255.255.0.0')
        strCmd = 'netsh interface IP delete address name="%s" addr=%s'%(Config.eth_name,arg)
        p = subprocess.Popen(strCmd,shell = True,stdout = subprocess.PIPE,stderr = subprocess.STDOUT)
        p.wait()
        curlines = str(p.stdout.read(),'gbk')
        print(curlines)
        time.sleep(des_dict.get('delay',1))
        ip_opera_dict['ip_opera'] = True
        return "success unplug_IP:"+arg
    def plug_IP(arg):
        global Config.eth_name
        global ip_opera_dict
        strCmd = 'netsh interface IP add address name="%s" addr=%s mask=255.255.0.0'%(Config.eth_name,arg)
        p = subprocess.Popen(strCmd,shell = True,stdout = subprocess.PIPE,stderr = subprocess.STDOUT)
        p.wait()
        curlines = str(p.stdout.read(),'gbk')
        print(curlines)
        if '对象已存在' not in curlines:
            time.sleep(des_dict.get('delay',6))
        ip_opera_dict['ip_opera'] = True
        return "success plug_IP:"+arg

    def normal_ip():
        print('normal_ip')
    '''

if __name__ == '__main__':
    pass