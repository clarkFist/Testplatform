#!/usr/bin/python
#encoding:utf-8
import sys, os
import re
paths = os.path.abspath(__file__).split('\\')
form_path = '\\'.join(paths[:-2])
if form_path not in sys.path:
    sys.path.append( form_path )
from udp_loglib import Log, Conf, Tag, FrameLib, Public, VCU, Simulator, Wireshark, myAssert, funclog
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
        # 2, 2,16 16
        return '2'

    @staticmethod
    def active_svcu() -> str:
        return '3'

    @staticmethod
    def serial_config() -> dict:
        return {'A': '3', 'B': ''}
        
    @staticmethod
    def get_normal_image_path():
        '''获取当前正常镜像的路径'''
        return os.path.abspath('%s/normal_image' % (os.path.dirname(__file__)))

    @staticmethod
    def case_path() -> list:
        '''设定本次执行的用例目录，以及n 级子目录'''
        path1 = os.path.dirname(__file__) + '\\' + 'testcase/Sw_MVCU' 

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
        return ['.PY'], [] # 本次自动执行的脚本 格式（后缀）
    
    @staticmethod
    def get_prefix() -> list: 
        '''执行时会将用例名称按_分割，分割后的字符需满足返回的所有条件，忽略大小写'''
        return ['SwITC', 'cfg2'], [] # 本次自动执行的脚本的前缀

    #CONFIG_END
#初始化read_dict
read_dict = {}  #保存数据
class StubRequirement:
    def __init__(self, slots: str = None, svcu_count: int = None, serial: dict = None):
        '''
          args:
           serial: dict key: A:slots B:slots AB:slots M/SVCUA:count M/SVCUB:count M/SVCUAB:count
        '''
        self.slots = slots
        self.svcu_count = svcu_count
        
        if serial is not None:
            self.serial_requirement = {}
            serial_count = {}
            for v in ['mvcu', 'svcu']:
                serial_count[v] = {}
                for m in ['A', 'B', 'AB']:
                    serial_count[v][m] = 0
                
            serial_config = serial
            for m in ['A', 'B', 'AB']:
                self.serial_requirement.setdefault(m, [])
            for key, value in serial_config.items():
                if key in ['A', 'B', 'AB']:
                    vcus = udp_loglib.VCU.inputsplit(value)
                    self.serial_requirement[key] = vcus
                    for v in vcus:
                        if v == 2 or v == 16:
                            m_s_vcu = 'mvcu'
                        else:
                            m_s_vcu = 'svcu'
                        
                        serial_count[m_s_vcu][key] += 1
                else:
                    key = str(key).lower()
                    if key.startswith('svcu'):
                        m_s_vcu = 'svcu'
                    elif key.startswith('mvcu'):
                        m_s_vcu = 'mvcu'
                    mode = key.split('vcu')[1].upper()
                    assert(mode in ['A', 'B', 'AB'])
                    serial_count[m_s_vcu][mode] += value
                del m_s_vcu

            for m_s_vcu in ['mvcu', 'svcu']:
                serial_count[m_s_vcu]['A'] += serial_count[m_s_vcu]['AB']
                serial_count[m_s_vcu]['B'] += serial_count[m_s_vcu]['AB']
            self.serial_requirement['A'] += self.serial_requirement['AB']
            self.serial_requirement['B'] += self.serial_requirement['AB']
            self.serial_count = serial_count             
        else:
            self.serial_requirement = serial

class myQueue(Queue.Queue):
    Empty = Queue.Empty

    def put(self, item, block=True, timeout=None):
        if self.qsize() > 300:
            for i in range(200):
                try:
                    self.get(timeout=0)
                except Exception:
                    break
        super(myQueue, self).put(item, block, timeout)
    
def log_queue_get(arg, cpu = None, use_Type = 'moudle_3') -> myQueue:
    assert( use_Type in ['moudle_3', 'NodeID', 'lru', 'com'])
    if use_Type != 'com' and isinstance(arg, str) and arg.startswith('COM'):
        return log_queue_get(arg, cpu, use_Type = 'com') # TODO 判断类型放在别处更合理
    if use_Type == 'moudle_3':
        assert( int(arg) in list(range(2, 35)) )
        value_dict = {'A': 0, 'B': 1, 'CPUA': 0, 'CPUB': 1}
        assert(cpu in value_dict.keys())
        key = (int(arg) + 3) * 2 + value_dict[cpu]
    elif use_Type == 'lru':
        temp = re.sub('[PS]', '', arg)
        temp = temp.split('_')
        rack = int(temp[0])
        slot = int(temp[1])
        moudle_3 = (rack - 1) * 16 + slot
        return log_queue_get(moudle_3, cpu, use_Type = 'moudle_3')
    elif use_Type == 'NodeID':
        key = int(arg)
    elif use_Type == 'com':
        key = arg

    point = read_dict
    point[key] = point.get(key, myQueue(500))
    return point[key]

def power_restart():
    AutoPower.sendPowerOffCommand("1,2,3,4")
    time.sleep(2)
    AutoPower.sendPowerOnCommand("1,2,3,4")

def vm_build_update(slots, open_initTag = None, wait = None):
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
        time.sleep(2)
    vmtool.build(cmd_type)
    vmtool.copyImage_fromVM(cmd_type)
    form.update('auto', 0, slots)
    if wait is not None:
        time.sleep(70)
        assert(wait in ['NORMAL', 'LIMATION', 'BOOT_FAILED', 'REPROG', 'VIV_ERROR'])
        assert(check_allboard(slots, timeout=200, dstmode=wait))

def check_allboard(slots = None, timeout: int = 0, dstmode = 'NORMAL'):
    start_time = time.time()
    retry_slots = []
    rt = True
    assert(dstmode in ['NORMAL', 'LIMATION', 'BOOT_FAILED', 'REPROG', 'VIV_ERROR'])
    if slots is None:
        __ = []
        mode_dict = Config.form.get_funcMode(Config.active_mvcu())
        __ = [key for key, mode in mode_dict.items() if mode != dstmode ]
        if len(__) > 0:
            retry_slots = None
        else:
            mode_dict = Config.form.get_funcMode(Config.active_svcu())
            retry_slots = [key for key, mode in mode_dict.items() if mode != dstmode ]
    else:
        mode_dict = Config.form.get_funcMode(slots)
        retry_slots = [key for key, mode in mode_dict.items() if mode != dstmode ]
    
    use_time = time.time() - start_time
    rt = isinstance(retry_slots, list) and len(retry_slots) == 0
    if (timeout - use_time ) > 15 and rt is False:
        time.sleep(10)
        use_time += 10
        rt = check_allboard(retry_slots, timeout - use_time)
    return rt

def get_slots_withSerial(mode = None) -> tuple:
    svcus = {}
    mvcus = {}
    for m in ['A', 'B']:
        svcus[m] = []
        mvcus[m] = []
    for key, value in Config.serial_config().items():
        slots = VCU.inputsplit(value)
        for slot in slots:
            if slot in [16, 2]:
                mvcus[key].append(slot)
            else:
                svcus[key].append(slot)
    svcus['AB'] = [ slot for slot in svcus['A'] if slot in svcus['B']]
    mvcus['AB'] = [ slot for slot in mvcus['A'] if slot in mvcus['B']]
    mvcus['_A'] = [ slot for slot in mvcus['A'] if slot not in mvcus['AB']]
    mvcus['_B'] = [ slot for slot in mvcus['B'] if slot not in mvcus['AB']]
    svcus['_A'] = [ slot for slot in svcus['A'] if slot not in svcus['AB']]
    svcus['_B'] = [ slot for slot in svcus['B'] if slot not in svcus['AB']]
    if mode == 'svcu':
        return svcus
    elif mode == 'mvcu':
        return mvcus
    return mvcus, svcus

def wait_to_ack_ext(msg = "please enter key to continue", timeout = 120) -> str:
    '''给出一句提示， 并捕获输入'''
    udp_loglib.Public.wait_to_ack_ext(msg, timeout)

if __name__ == '__main__':
    Config.form = udp_loglib.TestPlatForm(18125, 18125, set_client=False)
    power_restart()
    check_allboard(slots=[2], timeout=200)