#!/usr/bin/python
# _*_ coding: utf-8 _*_
'''
'''
from __future__ import print_function
from functools import wraps
import sys, os
from inspect import getframeinfo, stack
import traceback
import re, json
import struct
import binascii
import copy
from datetime import datetime

default_print = print
'''添加系统路径'''
#region
sysPath_temp = []
paths = os.path.abspath(os.path.dirname(__file__)).split('\\')
for i in range(-1, -len(paths), -1):
    path = '\\'.join(paths[:i] + [paths[i]])
    if 'py_lib' in os.listdir(path):
        sysPath_temp.append(path + '\\py_lib')
        sysPath_temp.append(path)
    #if 'venv' in os.listdir(path):
        path += '\\venv\\Lib'
        if sys.version_info > (3, 0):
            sysPath_temp.append(path + '\\site-packages')
        else:
            sysPath_temp.append(path + '\\python2')
        break

sys.path += [path for path in sysPath_temp if path not in sys.path]
#endregion
'''第三方库'''
from gevent import greenlet, Greenlet, sleep as g_sleep, monkey, time
from gevent import threading, subprocess, socket, threadpool
from gevent.lock import BoundedSemaphore
'''test'''
from singletontype import Singletontype
from mib import VCU_Mib as Mib
from logdatabase import DataBase, dbcommit
from FuncLog import FuncLog
from wireShark import Wireshark
from simulator import Simulator
from vmtcp import Client as VMTool
from serialread import SerialRead
from tftpclient import TftpClient, TftpException
import RUT.light_update as rut

funclog = FuncLog()  #设定funclog保存路径
'''基础类 和 部分变量初始化'''

class Config:
    #CONFIG_START
    slot_ip = {2: '20.2.1.10', 16: '20.1.1.10'} # MVCU 槽道，ip
    local_ip = {2: '20.2.1.100', 16: '20.1.1.100'} # 本地与MVCU 同一网段的ip
    wireshark_path = r"C:\Program Files (x86)\Wireshark" # wireshark 安装路径
    default_ethdesc = 'Realtek PCIe GBE Family Controller' # 本地网卡描述
    version = '116'
    #CONFIG_END
#region

class Public(object):
    thread_flag = {}
    _end_process = []
    run_flag = True

    @classmethod
    @funclog
    def normal_end(cls):
        '''正常结束程序'''
        for func in cls._end_process:
            cls.myAssert(len(func) < 4 and len(func) > 0)
            try:
                kwargs = {}
                args = ()
                for i in func[1:]:
                    if isinstance(i, tuple):
                        args = i
                    elif isinstance(i, dict):
                        kwargs = i
                        
                func[0](*args, **kwargs)
            except Exception as e:
                Public.mPrint(repr(e))
        cls.mPrint('normal end')

    @classmethod
    @funclog
    def program_end(cls, exc: Exception, *args, **kwargs):
        '''程序异常结束执行'''
        def wdg(flag, t):
            '''该方法调用后t秒内未完成则退出'''
            start = time.time()
            while flag[0] is True:
                if (time.time() - start > t):
                    Public.mPrint("program not end in %ds" % t)
                    exit(1)
                g_sleep(0.1)
            Public.mPrint("program end")
        dstname = re.sub(r'[^\w\d]', '', repr(exc))[:30] 
        dstfile = os.path.dirname(funclog.filepath) + '/%s' % dstname + '_%s.log'
        dstname_count = 0
        Public.run_flag = False
        while os.path.exists(dstfile % dstname_count):
            dstname_count += 1

        dstfile = dstfile % dstname_count
        flag = [True]
        for line in Public.decodeException():
            mPrint(line, mode=2)
        t = threading.Thread(target=wdg, args=(flag, 3.0))
        t.setDaemon(True)
        t.start()
        os.rename(funclog.filepath, dstfile)
        for func in cls._end_process:
            try:
                kwargs = {}
                args = ()
                for i in func[1:]:
                    if isinstance(i, tuple):
                        args = i
                    elif isinstance(i, dict):
                        kwargs = i

                func[0](*args, **kwargs)
                
            except Exception as e:
                Public.mPrint(repr(e))
        flag[0] = False
        raise exc

    @staticmethod
    def modify(string: str, indexs: int, value: str) -> str:
        '''修改字符串某几位，Public.modify(str,range(1,2),'123')
        Public.modify(str,'1-2','123')
        不包括2，和range 用法类似'''
        if isinstance(indexs, str):
            indexs = indexs.replace('-', ',', 1)
            indexs = eval('range(%s)' % indexs)
        Public.myAssert(isinstance(string, str))
        Public.myAssert(isinstance(indexs, range))

        indexs = [i for i in indexs]
        temp = list(string)
        for i in indexs:
            temp.pop(indexs[0])
        pos = indexs[0]
        for v in value:
            temp.insert(pos, v)
            pos += 1
        rt = ''.join(temp)
        return rt

    @staticmethod
    def decodeException():
        import tempfile
        tmpfd, tmpname = tempfile.mkstemp()
        with open(tmpname, 'w+') as f:
            traceback.print_exc(file=f)
            f.seek(0)
            lines = f.readlines()
        os.close(tmpfd)
        os.unlink(tmpname)
        #value = sys.exc_info()
        return lines

    @staticmethod
    def int2hex(val: int, width: int) -> str:
        '''将整形转换为一个字节码字符串'''
        #if isinstance(val,str):
        #    val = int(val,16)
        Public.myAssert(val <= int('F' * width, 16))
        temp = hex(val)[2:]  #去掉前缀0x
        temp = re.sub(r'[Ll]', '', temp)  #去掉后缀
        temp = temp.rjust(width, '0')  #高位补零
        return temp.upper()

    @staticmethod
    def isIPreachable(ip, timeout = 1.5):
        '''通过ping命令判断ip是否可以通信，ping中文默认编码方式是GBK，
            out 内中文为乱码，字母可正常显示   
        '''
        p = subprocess.Popen(['ping', '-n', '1', '-w', str( int(timeout * 1000)), ip], stdout=subprocess.PIPE)
        p.wait()
        out = str(p.stdout.read())
        if 'TTL=' in out:
            return True
        else:
            return False

    @staticmethod
    def isDebugMode():
        gettrace = getattr(sys, 'gettrace', None)
        if gettrace is None:
            return False
        elif gettrace():
            return True
        else:
            return False

    @classmethod
    @funclog
    def none_func(cls, *args, **kwargs):
        '''global print print = Public.none_func'''
        pass

    @staticmethod
    @funclog
    def mPrint(*args, mode=0, **kwargs):
        '''测试平台的打印
         args: 
           mode:int [0,1,2,3] 0时两条分割线均打印. 1 仅打印第一条 2： 均不打印 3： 仅打印第三条
        '''
        # if 'mode' in kwargs.keys():
        #     mode = kwargs.pop('mode')
        # else:
        #     mode = 0

        if mode != 3 and mode != 2:
            default_print('\n---------------------------------------')
        caller = getframeinfo(stack()[2][0])
        filename = caller.filename.split('\\')[-1]
        msg = "%s %s msg:" % (filename, caller.lineno)
        default_print(msg, end='')
        kwargs['end'] = '\n'
        default_print(*args, **kwargs)
        if mode != 1 and mode != 2:
            default_print('---------------------------------------\n')

    @staticmethod
    def myAssert(ass: bool, *args, **kwargs):
        '''断言函数'''
        global print
        if not ass:
            #print = Public.none_func
            caller = getframeinfo(stack()[2][0])
            filename = caller.filename.split('\\')[-1]
            Public.mPrint(filename, caller.lineno, caller.function,
                          caller.code_context,
                          mode = 1)
            caller = getframeinfo(stack()[1][0])

            filename = caller.filename.split('\\')[-1]
            msg = ('assert failed at',
                   filename,
                   caller.lineno,
                   caller.function,
                   caller.code_context)
            Public.mPrint(msg, mode = 3)
            if len(args) > 0 or len(kwargs) > 0:
                Public.mPrint(*args, **kwargs)
            raise AssertionError(msg)

    @staticmethod
    def tuple_len(arg: tuple):
        '''计算一个元组里的字符数'''
        Public.myAssert(isinstance(arg, tuple) or isinstance(arg, list))
        length = 0
        for v in arg:
            if isinstance(v, tuple) or isinstance(v, list):
                length += Public.tuple_len(v)
            else:
                length += len(str(v))
        return length

    @staticmethod
    def crc16(data: str, leng):
        '''CCITT-FALSE'''
        seed = 0xFFFF
        poly = 0x1021
        #data = bin(int(data,16))[2:]
        for i in range(0, leng * 8):
            if i % 8 == 0:
                index = int(i / 4)
                temp = int(data[index:index + 2], 16) << 8
            if (seed ^ temp) & 0x8000 != 0:
                seed <<= 1
                seed ^= poly
            else:
                seed <<= 1
            temp <<= 1
            seed &= 0xFFFF
            temp &= 0xFFFF
        crc = hex(seed).upper()
        crc = crc[2:6].rjust(4, '0')[-4:]
        return crc

    @staticmethod
    def wait_to_ack_ext(msg = "please enter key to continue", timeout = 120) -> str:
        '''给出一句提示， 并捕获输入'''
        import tempfile
        tmpfd, tmpname = tempfile.mkstemp()
        os.close(tmpfd)
        with open(tmpname, 'w+') as f:
            f.write(f'''
        @echo off
        echo {msg}:
        set /p ack=
        echo #%ack%#>>%0
        exit /b 0
        :end
        ''')
        try:
            os.rename(tmpname, tmpname + '.bat')
            tmpname += '.bat'
            p = subprocess.Popen(tmpname,
                                 #stdin = subprocess.PIPE,
                                 #stdout = subprocess.PIPE,
                                 #stderr = subprocess.PIPE,
                                 creationflags =subprocess.CREATE_NEW_CONSOLE,
                                 ) # 开始执行
            p.wait(timeout)
        except Exception as e:
            p.kill()
            raise Public.myException(repr(e))
        else:
            if p.returncode == 0:
                with open(tmpname, 'r') as f:
                    content = f.read()
                    res = re.search(r'\s:end\s+#([^\r\n#]*)#', content, re.S | re.DOTALL)
                ret = res.group(1)
            else:
                ret = ''
            
        finally:
            os.unlink(tmpname)
        
        return ret
    
    class myException(Exception):
        pass

class myGevent(greenlet.Greenlet):
    '''原协程会在单个协程异常后继续执行，此协程异常会使进程退出'''
    monkey = monkey
    gevent_dict = {}

    @classmethod
    @funclog
    def spawn(cls, func = None, *args, **kwargs) -> greenlet.Greenlet:
        '''开启一个协程'''
        caller = stack()
        t = greenlet.Greenlet(cls.run, func, caller, *args, **kwargs)
        if Public.run_flag is True:
            t.start()
        else:
            return None
        return t

    @classmethod
    @funclog
    def joinall(cls, greenlets, timeout=None, raise_error=False, count=None):
        '''等待协程 
        greenlets: myGevent.spawn 的返回值构成的一个列表
        If ``count`` is ``None`` (the default), wait for all ``greenletscts``
        to become ready.

        If ``count`` is a number, wait for (up to) ``count`` objects to become
        ready. (For example, if count is ``1`` then the function exits
        when any object in the list is ready).

        If ``timeout`` is provided, it specifies the maximum number of
        seconds ``wait()`` will block.'''
        #if DEBUG_MODE is True:
        raise_error = True
        greenlet.joinall(greenlets,
                         timeout=timeout,
                         raise_error=raise_error,
                         count=count)

    @classmethod
    def run(cls, func, caller, *args, **kwargs):
        try:
            rt_val = func(*args, **kwargs)
        except Exception as e:
            Public.program_end(e)
        return rt_val

    @classmethod
    def sleep(cls, sec):
        '''主动切换协程'''
        g_sleep(sec)

    @staticmethod
    def empty():
        '''计算空闲时间，count 数值越大空闲时间越多，不能检查出某个线程完全阻塞'''
        # start = time.time()
        count = 100000
        temp = time.time()

        @funclog
        def buzy(count, ttime):
            Public.mPrint('block', count, round(ttime, 2))

        while Public.thread_flag.setdefault('empty', True):
            g_sleep(0.1)
            count += 1
            cur_time = time.time()

            if cur_time - temp >= 1:
                if count < 5 or cur_time - temp > 1.5:
                    buzy(count, (cur_time - temp))
                temp = cur_time
                count = 0

DEBUG_MODE = Public.isDebugMode()  # 
if DEBUG_MODE:
    threadpool.sys.settrace = lambda _: None
#Public.mPrint("Run in debug:%s" % DEBUG_MODE)
myAssert = Public.myAssert
mPrint = Public.mPrint
print = default_print  # print = Public.no_prnt
gevent = myGevent  # 自定义协程

Public._end_process.append(( exec, ('for k in Public.thread_flag.keys():\n\tPublic.thread_flag[k]=False',) ))

class VCU:
    ''''''
    lru_ban = [1, 15, 17, 31, 33, 47]  #非法slot ID

    @classmethod
    def moudle_3toNode(cls, moudle_3):
        '''将moudleID减三（即槽道号+（机笼号-1）*16）转为nodeID'''
        moudle_3 = int(moudle_3)
        Public.myAssert(moudle_3 <= 16 * 3 and moudle_3 > 0)
        if moudle_3 == 2:
            nodeId1 = '02'
            nodeId2 = '03'
        elif moudle_3 == 16:
            nodeId1 = '04'
            nodeId2 = '05'
        else:
            nodeId1 = hex((moudle_3 + 3) * 2)[2:]
            nodeId1 = nodeId1.rjust(2, '0') 
            nodeId2 = hex((moudle_3 + 3) * 2 + 1)[2:]
            nodeId2 = nodeId2.rjust(2, '0')
        return nodeId1, nodeId2  # 对应槽道的cpua、cpub的nodeid

    @staticmethod
    def inputsplit(slot: str, rack=1):
        '''将输入的moudle — 3 分割成数组
            exmple：
            VCU.inputsplit('2.3,4，5，6-20') -> [2,3,4,5,6,7,...,20]'''
        global lru_ban
        slot = str(slot)
        slot = re.split(r'[,，.]', slot)
        moduleID_3 = []
        for s in slot:
            if len(s) == 0:
                continue
            s = s.split('-')
            s.append(s[0])
            moduleID_3 += list(range(int(s[0]), int(s[1]) + 1))
        for i in range(len(moduleID_3)):
            if moduleID_3[i] < 17:
                moduleID_3[i] += (int(rack) - 1) * 16
            Public.myAssert(moduleID_3[i] <= 0xFF and moduleID_3[i] > 0, '格式错误')
            # if moduleID_3[i] in lru_ban:
            #     raise Exception('slot input mistake')
            #     moduleID_3[i] = 0
        return [m for m in moduleID_3 if m not in VCU.lru_ban]

    @staticmethod
    def node2lru(arg, sp={-2: '1_2P', -1: '1_16S', 2: '1_2P', 16: '1_16S'}):
        '''将nodeid 转换为lru格式,sp:特殊值
           arg: int|str
        '''
        if isinstance(arg, str):
            arg = int(arg, 16)
        myAssert(arg < 0xFF and arg >= 0)
        cpu = {0: 'CPUA', 1: 'CPUB'}.get(arg % 2)

        arg = arg // 2 - 3  #先转为moudleid - 3 ，sp->key

        x = (arg - 1) // 16 + 1  #机笼号
        y = arg - 16 * (x - 1)  # 槽道号
        return sp.get(arg, '%s_%s' % (str(x), str(y))), cpu

    @staticmethod
    def gettime():
        '''fmt:%Y-%m-%d %H:%M:%S'''
        return datetime.now().strftime('%Y-%m-%d %H:%M:%S')


#endregion

'''udp 消息发送、接收解析和分类'''

#region
class HexType:
    '''UdpRecvFrame 里解析struct 用到'''
    def __init__(self, val):
        self.val = val

    def __repr__(self):
        return str(self.val)

class UdpSendFrame(object):
    @funclog
    def __init__(self, string=''):
        if len(string) > 0:
            pass

    @property
    def data(self) -> str:
        temp = self.dstNodeID + self.srcNodeID + self.mtype._mtype + self.len + self.command + self.crc
        return temp

    @property
    def len(self) -> str:
        #该位为fram长度，单位为字节。两个字符的字符串为一个字节。
        #将frame长度 转为16进制字符串
        _len = Public.int2hex(len(self.command + self.crc) // 2 + 4, 2)
        return _len

    @property
    def mtype(self):
        return self.__mtype #type: Default

    @property
    def srcNodeID(self):
        return self.__srcNodeID

    @property
    def dstNodeID(self):
        return self.__dstNodeID

    @property
    def command(self):
        return self.__command

    @property
    def crc(self):
        if hasattr(self, '_crc'):
            return self._crc
        else:
            return ''

    @srcNodeID.setter
    def srcNodeID(self, val: str):
        myAssert(len(val) == 2)
        self.__srcNodeID = val

    @dstNodeID.setter
    def dstNodeID(self, val: str):
        myAssert(len(val) == 2 and int(val, 16) < 104)
        self.__dstNodeID = val

    @mtype.setter
    def mtype(self, val):
        myAssert(issubclass(val, Default))
        self.__mtype = val

    @command.setter
    def command(self, val: str):
        myAssert(len(val) % 2 == 0 and len(val) <= (1 + 4 * 2 + 2) * 2,
                 val)  # 和VCU配置有关
        self.__command = val

    @crc.setter
    def crc(self, val: str):
        '''添加crc 需要两步，第一步先设置一个固定长度的常量，使后面添加crc后不会更改data（添加crc后长度会增加）
           第二步添加crc'''
        if hasattr(self, '_crc'):
            myAssert(len(self._crc) == len(val))
        self._crc = val

    def copy(self):
        return copy.copy(self)

    def setSendCallBack(self, func):
        self._frameSendCallBack = func
        
    def frameSendCallBack(self, slots):
        if hasattr(self, '_frameSendCallBack'):
            return self._frameSendCallBack(slots)
        else:
            return None
    
    @property
    def isRecord(self):
        if hasattr(self, '_isRecord'):
            if time.time() - self._isRecord[1] < 60:
                return self._isRecord[0]
            else:
                return True
        else:
            return True

    @isRecord.setter
    def isRecord(self, value):
        self._isRecord = (value, time.time())


class FrameLib(object):
    type_dict_1 = {}
    type_dict_2 = {
        '0': 'Tag',
        '2': 'Conf',
        'E': 'Log'
    }  #消息接收时按照此类型分类 需与对应类的_mtype 的第二位对应

    respond_list = []  # 确认回复时用到
    request_dict = {}  #

    @classmethod
    @funclog
    def resigeter(cls, frame, timeout=8.0):
        '''若某条消息需要目的地址回复确认包，需在此注册，respondCheck 若在timeout内未收到回复消息则重发。'''
        Public.myAssert(timeout < 11)
        if frame in cls.request_dict.keys():
            cls.request_dict[frame] = ((frame.dstNodeID + frame.mtype._mtype[1] + frame.command + frame.crc).upper(),
                                       time.time(), 
                                       timeout, 
                                       cls.request_dict[frame][-1]) # 发送时间保持不变
        else:
            cls.request_dict[frame] = ((frame.dstNodeID + frame.mtype._mtype[1] + frame.command + frame.crc).upper(),
                                       time.time(),  # 本次发送时间
                                       timeout, 
                                       time.time()) #第一次发送时间

    @classmethod
    @funclog
    def respondCheck(cls):
        '''检查是否收到回复报文，当目前注册消息全部检查完毕后结束，后来的消息需再次调用'''
        Public.thread_flag['respondCheck'] = Public.thread_flag.get(
            'respondCheck', True)
        while Public.thread_flag['respondCheck']:
            deleteTemp = set()  #避免重复

            for res, restime in cls.respond_list[:]:  #清除旧消息
                if time.time() - restime > 30:
                    cls.respond_list.remove((res, restime))

            for key, val in cls.request_dict.items():
                for res, restime in cls.respond_list:  #restime- respond time
                    if restime > val[1]:  #回复消息是来自发送消息之后的消息
                        if res == val[0]:
                            if restime - val[1] > val[2]:
                                Public.mPrint('%s timeout' %
                                              val[0])  #在上一次检查时未超时，此次超时，只打印
                            else:
                                pass
                            deleteTemp.add(key)  #此处删除会引起遍历错误

            for key, val in cls.request_dict.items():
                if time.time() - val[3] > val[2] * 8:
                    raise Exception('the msg to %s, %s not recv respond, msg %s' %
                                    (VCU.node2lru(key.dstNodeID)[0],
                                     VCU.node2lru(key.dstNodeID)[1], key.data))
                if time.time() - val[1] > val[2] and key not in deleteTemp:
                    #deleteTemp.add(key) # 消息会重发会重新注册
                    Public.mPrint('the msg to %s,%s not recv respond, msg %s' %
                                  (VCU.node2lru(key.dstNodeID)[0],
                                   VCU.node2lru(key.dstNodeID)[1], key.data))
                    TestPlatForm.form._reSend(
                        key)  # 重发 ,会修改cls.event_dict的val，不会引起遍历错误

            for key in deleteTemp:
                cls.request_dict.pop(key)

            if len(cls.request_dict) == 0:
                Public.mPrint('set success TP Ver-3.0.5')
                break
            g_sleep(1)


class UdpRecvFrame:
    udpFrame_struct = [('destNodeID', 1), ('srcNodeID', 1), ('dataType', 1),
                       ('dataLen', 1),
                       ('tstData', 1, '')]  # 参考UdpRecFrame.parse_data 方法

    def __init__(self, data, dst=None):
        '''若dst不为None 则忽略data的类型位，将data解析为指定类型'''
        if isinstance(data, str):
            self.frame = data.upper()
        else:
            Public.myAssert(False, 'type err')
        self.transform(dst)

    @property
    def data_type(self):
        # 所处理数据类型的class
        return self.__mtype # type: Log

    # @funclog
    def _parse(self, _struct: dict, dst, frame: str):
        '''description:解析结构体,可以用struct 代替
        
        Attributes: self, struct, dst, frame
        
        Arguments:
          struct:dict 结构体格式
          dst:instance 解析内容保存的目标
          frame:str  待解析的消息帧
        Returns: Type:None
          无返回值
        '''
        pos = 0
        for data in _struct:
            name, typeLen, saveType, endflag = self.parse_data(*data)
            prePos = pos
            if isinstance(saveType, int):
                setattr(dst, name, frame[pos: pos + typeLen])
                #exec('dst.' + name + '= frame[pos:pos+typeLen]')
                setattr(dst, name, int(getattr(dst, name), 16))
                #exec('dst.%s = int(dst.%s,16)' % (name, name))
                pos += typeLen
            elif isinstance(saveType, HexType):
                setattr(dst, name, frame[pos: pos + typeLen])
                #exec('dst.' + name + '= frame[pos:pos+typeLen]')
                pos += typeLen
            elif isinstance(saveType, str):
                #tempPos = pos
                if not isinstance(saveType, int):
                    if len(saveType) > 0:
                        max_len = int(saveType[0], 16)
                    else:
                        max_len = 999
                else:
                    max_len = saveType
                #exec('dst.' + name + '= ""')
                setattr(dst, name, "")
                for i in range(pos, len(frame), typeLen):
                    if endflag == frame[pos:pos + typeLen]:  # 结束标志
                        pos += typeLen
                        break
                    else:
                        #temp = getattr(dst, name)
                        #getattr(dst, name) += frame[pos: pos + typeLen]
                        exec('dst.' + name + '+= frame[pos:pos+typeLen]')
                        pos += typeLen

                    if pos + typeLen > len(frame):  # frame末尾
                        if pos < len(frame):
                            mPrint('err %d,%d' % (pos, len(frame)))
                        break
                    if pos - prePos >= max_len * typeLen:  # 固定长度
                        if pos - prePos > max_len * typeLen:
                            mPrint('err %d,%d' %
                                   (pos - prePos, max_len * typeLen))
                        break
                #exec('dst.%s = binascii.unhexlify(dst.%s).decode()'%(name,name))
            elif isinstance(saveType, list):
                try:
                    max_len = int(saveType[0])
                except Exception:
                    max_len = 9999
                #tempPos = pos
                setattr(dst, name, [])
                for i in range(pos, len(frame), typeLen):
                    if endflag == frame[pos:pos + typeLen]:
                        pos += typeLen
                        break
                    else:
                        
                        exec('dst.' + name + '.append(frame[pos:pos+typeLen])')
                        pos += typeLen

                    if pos + typeLen > len(frame):  # frame末尾
                        if pos < len(frame):
                            mPrint('err %d,%d' % (pos, len(frame)))
                        break
                    if pos - prePos >= max_len * typeLen:  # 固定长度
                        if pos - prePos > max_len * typeLen:
                            mPrint('err %d,%d' % (pos - prePos, max_len * typeLen))
                        break

    def transform(self, dst_2):
        ''''''
        _parse = self._parse
        _struct = UdpRecvFrame.udpFrame_struct  #先解析头文件
        dst = self
        frame = self.frame
        _parse(_struct, dst, frame)
        frame = self.tstData  #开始解析类型和内容
        if dst_2 is None:
            self.dataType = self.dataType.upper()
            if self.dataType[0] in FrameLib.type_dict_1.keys():
                dst = eval(FrameLib.type_dict_1[self.dataType[0]])
            else:
                dst = eval(FrameLib.type_dict_2[self.dataType[1]])

        else:
            dst = dst_2

        self.__mtype = dst() # type: Log
        _struct = self.__mtype._struct
        _parse(_struct, self.__mtype, frame)
        self.__mtype._default_run(self)
        return self
        
    def parse_data(self, name, typeLen, saveType=HexType('1'), endflag=''):
        '''name是保存的名称，会将对应的值储存在对应类的以name为名的变量内。
            typeLen 是保存类型的基本长度（若是数组以元素为准）。
            saveType 是保存的类型及该位长度。若未给出则默认16进制字符串保存。若显式赋值为整数 则以十进制保存。这两个值的大小无意义。
            若为字符串类型，则将其以utf—8解析为字符串。若为数组类型，则将其以固定长的的16进制字符串保存在数组里。这两个的值如'2',[3]等为
            此两项最长大小。若为'',[]，则不限制。
            endflag 为数组或字符串的结束标志。若为空则不限'''
        return name, typeLen * 2, saveType, endflag  #长度为字节长度，一个字节两个字符


class UdpClient:
    def __init__(self, localAddr, dstAddr):
        '''本地或远端ip的值均为(ip,port)的元组类型'''
        Public.myAssert(isinstance(localAddr, tuple))
        Public.myAssert(isinstance(dstAddr, tuple))
        Public.myAssert(len(localAddr) == 2 and len(dstAddr) == 2)
        try:
            client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  #创建udp连接
            client.bind(localAddr)  # 绑定本地ip
            client.setblocking(0)
            client.settimeout(0.01)  #设置超时时间
            client.setsockopt(socket.SOL_SOCKET, socket.SO_RCVTIMEO, 1)
        except socket.error as e:
            mPrint("may not have the local ip %s" % localAddr[0])
            mPrint("Error creating socket: %s" % e)
            raise socket.error(repr(e))
        self.remote = dstAddr
        self.client = client
        self.send_time = time.time()

    def recv(self):
        '''接收消息，正常返回值为data,异常时为None'''
        try:
            data, server_addr = self.client.recvfrom(4096)
        except socket.timeout:
            return None
        string = binascii.b2a_hex(data).decode()
        if sys.version_info < (3, 0):
            string = str(string)
        data = string
        if string[4] == '3':  #若为3 则是回复报文
            #mPrint('recv ack %s' % string)
            Reply._reply_Recv(string)
            return None
        else:
            if string[4] == '1':  #若为1 则需要回复
                data = Reply._reply_Send(string)
                self.client.send(string)

        return data

    def send(self, frame):
        while time.time() - self.send_time < 0.5:
            g_sleep(0.3)  #避免频繁发送
        self.send_time = time.time()
        data = binascii.unhexlify(frame.data)  #将16进制字符串解析为字节码
        self.client.sendto(data, self.remote)

class VCU_VIV:
    def __init__(self, slot):
        if not isinstance(slot, int):
            slot = int(slot)

        self.moduleid = slot + 3
        self.slot = slot
        if slot == 2:
            self.nodeA = 2
            self.nodeB = 3
        elif slot == 16:
            self.nodeA = 4
            self.nodeB = 5
        else:
            self.nodeA = self.moduleid * 2
            self.nodeB = self.nodeA + 1
        self.port = (1024 + self.nodeA * 20, 1024 + self.nodeB * 20)
        for ip in Config.slot_ip.values():
            if Public.isIPreachable(ip):
                self.remoteIP = ip
                break
        else:
            raise Public.myException('no ip is reachable!')
        self.client = TftpClient(self.remoteIP)
        #from threading import Event
        #self.event = Event()

    def _SessionOpen(self, cpu):
        if not hasattr(self, '_o_data'):
            self._o_data = {}
        elif cpu in self._o_data.keys():
            raise Exception('Session %s not close' % self._o_data[cpu])

        _o_cmd = {'A': ('viv_v001_oHDGB45G6', self.port[0]), 'B': ('viv_v001_oG5OSJ5SL', self.port[1]) }

        data = self.client.read_request(*_o_cmd[cpu])
        data = binascii.b2a_hex(data).decode()
        
        self._o_data[cpu] = data
        return data

    def _SessionClose(self, cpu):
        if hasattr(self, '_o_data') and cpu in self._o_data.keys():
            #_c_cmd = {'A': ('viv_v001_c%s' % self._o_data[cpu], self.port[0]), 'B': ('viv_v001_c%s' % self._o_data[cpu], self.port[1]) }
            #self.client.read_request(*_c_cmd[cpu])
            self._o_data.pop(cpu)

    def _cop_open(self, cpu):
        if not hasattr(self, '_copopen_dict'):
            self._copopen_dict = {}
        elif cpu in self._copopen_dict.keys():
            return #raise Exception('cop%s not close' % self._o_data[cpu])
        node_dict = {'A': self.nodeA, 'B': self.nodeB}
        data = Public.int2hex(node_dict[cpu], 2)
        self.client.write_request('cop_v001_open', data=data)
        self._copopen_dict[cpu] = data

    def _cop_close(self, cpu):
        if cpu in self._copopen_dict.keys():
            node_dict = {'A': self.nodeA, 'B': self.nodeB}
            data = Public.int2hex(node_dict[cpu], 2)
            try:
                self.client.write_request('cop_v001_close', data=data)
            except Exception:
                try:
                    self.client.write_request('cop_v001_close', data=data)
                except Exception as e:
                    e
            self._copopen_dict.pop(cpu)

    def _CommandInfo(self, cpu):
        if not hasattr(self, '_i_data'):
            self._i_data = {}

        _i_cmd = {'A': ('viv_v001_i%s' % self._o_data[cpu], self.port[0]), 'B': ('viv_v001_i%s' % self._o_data[cpu], self.port[1]) }
        data = self.client.read_request(*_i_cmd[cpu])
        data = binascii.b2a_hex(data).decode()
        
        self._i_data[cpu] = data
        mouduleId, funcMode, fuseState, reprogAutho = re.findall('.{2}', data)
        return mouduleId, funcMode, fuseState, reprogAutho

    def _MemReadInit(self, cpu, data, dst = 'SPI'):
        self._VIV_write_base('m', cpu, data, _file_tail = dst)
        # _type = 'm'
        # _file = 'viv_v001_%s%s%s' % (_type, self._o_data[cpu], dst)
        # _cmd = {'A': (_file, self.port[0]), 'B': (_file, self.port[1]) }
        # self.client.write_request(*_cmd[cpu], data = data)

    def _VIV_request_base(self, cmdtype, cpu, _file_tail = ''):
        _type = cmdtype
        _file = 'viv_v001_%s%s%s' % (_type, self._o_data[cpu], _file_tail)
        _cmd = {'A': (_file, self.port[0]), 'B': (_file, self.port[1]) }
        if not hasattr(self, '_%s_data' % _type ):
            setattr(self, '_%s_data' % _type, {})
        _dict = getattr(self, '_%s_data' % _type)
        data = self.client.read_request(*_cmd[cpu])
        data = binascii.b2a_hex(data).decode()  
        _dict[cpu] = data
        return data

    def _VIV_write_base(self, cmdtype, cpu, data, _file_tail = ''):
        _type = 'm'
        _file = 'viv_v001_%s%s%s' % (_type, self._o_data[cpu], _file_tail)
        _cmd = {'A': (_file, self.port[0]), 'B': (_file, self.port[1]) }
        self.client.write_request(*_cmd[cpu], data = data)

    def _CommandStatus(self, cpu):
        return self._VIV_request_base('s', cpu)
        
    def _CommandMemRead(self, cpu, dstfile = None):
        _type = 'u'
        if not hasattr(self, '_%s_data' % _type ):
            setattr(self, '_%s_data' % _type, {})
        _dict = getattr(self, '_%s_data' % _type)
        _file = 'viv_v001_%s%s' % (_type, self._o_data[cpu])
        _cmd = {'A': (_file, self.port[0]), 'B': (_file, self.port[1]) }

        data = self.client.read_request(*_cmd[cpu], srcfilename=dstfile)
        data = binascii.b2a_hex(data).decode()  
        _dict[cpu] = data
        return data

    def __get_function(self, retry, cpu, function, *args, **kwargs):
        '''
         从VCU 获取信息
        '''
        while retry >= 0:
            try:
                self._cop_open(cpu)
                self._SessionOpen(cpu)
                data = function(*args, **kwargs)
            except (TimeoutError, socket.timeout, TftpException) as e:
                e
                retry -= 1
                gevent.sleep(1)
                self._SessionClose(cpu)
                self._cop_close(cpu)
                continue
            else:
                self._SessionClose(cpu)
                self._cop_close(cpu)
                break                
        else:
            raise TimeoutError('get fail')

        return data

    @funclog
    def get_functionmode(self, retry = 2):

        mouduleId, funcMode, fuseState, reprogAutho = self.__get_function(retry, 'A', self._CommandInfo, 'A')
 
        return int(funcMode, 16)

    @funclog
    def get_eventlog(self, cpu, seq = 0, retry = 3):
        def func(cpu, data):
            self._MemReadInit(cpu, data=data, dst = 'SPI')
            state = 0
            while state != 1:
                state = self._CommandStatus(cpu)
                state = int(state)
                gevent.sleep(0.06)
            data = self._CommandMemRead(cpu)
            return data
            
        data = '0001%s00004000' % Public().int2hex(0x4000 * seq, 4)
        data = self.__get_function(retry, cpu, func, cpu, data)

        infos = re.findall(r'(\w{40})f{216}', data)
        rt_list = []
        endflag = False
        for info in infos:
            if info == 'f' * 40:
                endflag = True
                break
            msg = struct.unpack('!HBBIIII', binascii.unhexlify(info))
            msg = 'line:%d taskprio:%d id:%d file_crc:0x%x context:%d can_days:%d can_ms:%d' % msg
            rt_list.append(msg)
        if endflag is False:
            rt_list += self.get_eventlog(cpu, seq = seq + 1)
        return rt_list

class TcpServer:
    '''转发VCU消息'''
    def __init__(self, host, port):
        try:
            #server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR,1 )#允许重复地址和端口
            server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server.setblocking(0)
            server.settimeout(0.02)
            self.server = server
        except socket.error as e:
            print("Error creating socket: %s" % e)
            sys.exit()
        try:
            server.bind((host, port))
        except socket.error as e:
            print("Bind failed! " + str(e))
            sys.exit()
        print("Socket bind complete")
        server.listen(10)
        gevent.spawn(self.listen)

    def listen(self):
        print("Socket now listening")
        Public.thread_flag['TcpListen'] = Public.thread_flag.get(
            'TcpListen', True)
        while Public.thread_flag['TcpListen']:
            g_sleep(1)
            if not hasattr(self, 'conn'):
                try:
                    conn, addr = self.server.accept()
                    self.conn = conn
                except socket.timeout:
                    continue
                else:
                    mPrint('conn')

    def send(self, lru, data: str):
        if hasattr(self, 'conn'):
            msg = json.dumps({
                'name': lru,
                'path': os.path.dirname(__file__) + '\\' + Log.path_get() + '%s.log' % lru,
                'value': data
            }) + '//r//r'
            if sys.version_info > (3, 0):
                msg = bytes(msg, 'utf8')
            else:
                msg = bytes(msg)
            try:
                self.conn.send(msg)
            except socket.error as e:
                mPrint(repr(e))
                self.conn.shutdown(2)
                self.conn.close()
                del self.conn
#endregion
'''处理 udp 发送或接收的消息的类型'''

#region
class Default(object):

    '''接受或发送的udp消息的基类
    处理接受的udp消息时，若Tst_DataType的类型无消息类型则进入，正常模式不会进入'''
    _struct = [('tstData', 1, '')
               ]  # 参考UdpFrame.parse_data 方法，第一个str 会作为类变量动态添加至当前类
    _mtype = 'XY'  #发送消息时此变量必须有，第一位是0：无需回复，1：需要回复，3:是回复报文。
    #第二位是报文类型 发送时类型取决于第二位，接收时以FrameLib的字典为准
    '''接收消息时此变量必须有，表示收到此类型消息后会执行的方法
    以列表的形式会更加灵活，可以在其他地方随时增删
    因为在此时cls or 其他需执行的方法大概率未完成初始化，故需以字符串的形式使用'''
    def __init__(self):
        Default._toRun_list = [(self._print, ()), (self._store, ())]

    def _print(self):
        Public.myAssert(False)
    
    def _store(self):
        Public.myAssert(False)

    def _default_run(self, frame):
        '''接收消息时此方法必须有,UdpRecvFrame 在完成结构体解析时会执行该方法，同时UDPFrame会作为参数传递给该函数'''
        Public.myAssert(False)

    def _run_list(self):
        for func in self._toRun_list:
            if callable(func):
                func = [func]
            myAssert(len(func) < 4 and len(func) > 0)
            kwargs = {}
            args = ()
            for i in func[1:]:
                if isinstance(i, tuple):
                    if '_instance_' in i:
                        i = list(i)
                        i[i.index('_instance_')] = self
                        i = tuple(i)
                    args = i
                elif isinstance(i, dict):
                    kwargs = i  
            func[0](*args, **kwargs)

    @classmethod
    def print(cls, *args, **kwargs):
        print(*args, **kwargs)

    @classmethod
    def modeSet(cls, val, mode = 0, default='FFFF') -> str:
        ''' mode = 0 将 val 转换为 长度为8 的字符串，高位补零
            mode = 1:将高四位 置为default,低四位为val
            mode = 2:将低四位 置为default，高四位为val
            mode = 3:高四位、低四位均置为 val
        '''
        my_len = len(default)
        if isinstance(val, int):
            val = hex(val)[2:]
            val = re.sub(r'[Ll]', '', val)
            myAssert(len(val) <= my_len * 2)

        if mode != 0:

            myAssert( my_len % 2 == 0 or my_len == 1)
            myAssert(len(val) <= my_len )
            val = val.rjust( my_len, '0')
            modeB = (mode >> 1)
            modeA = (mode & 0x01)
            if modeA == 1:
                valA = val
            else:
                valA = default
            if modeB == 1:
                valB = val
            else:
                valB = default
            val = valB + valA
        else:
            val = val.rjust(my_len * 2, '0')

        return val
#region
# class Maint(Default):
#     __get_flag = None
#     _mtype = '14'
#     _strcut = [('maint_type', 1),
#                ('req_ack', 1), # 1 req, 3 ack
#                ('info', 1, '')]

#     type_dict = {'state': '00'}

#     @classmethod
#     def _default_run(cls, frame):
#         '''接收消息时此方法必须有,UdpRecvFrame 在完成结构体解析时会执行该方法，同时UDPFrame会作为参数传递给该函数'''
#         Public.myAssert(cls.__get_flag is not None)
#         cls.msg = ''
#         for item in cls._struct[2:]:
#             cls.msg += str(getattr(cls, item[0]))
#         cls.__get_flag = None

#     @classmethod
#     def _print(cls):
#         cls.print(cls.msg)
    
#     @classmethod
#     def _store(cls):
#         Public.myAssert(False)

#     @classmethod
#     def get_state(cls):
#         _strcut = [('info', 1, '')]
#         return cls.__send_request(_strcut, 'state', 1)

#     @classmethod
#     def __send_request(cls, _struct, func, flag):
#         assert(cls.__get_flag is None or cls._strcut == _struct) 
#         cls.__get_flag = func  
#         cls._strcut = cls._strcut[:2] + _struct
#         mode = cls.modeSet(1, mode = 3, default = 'F')
#         msg = cls.type_dict[func] + mode + Public.int2hex(int(flag), 2)
#         frame = UdpSendFrame()
#         frame.mtype = cls
#         frame.command = msg
#         return frame
#endregion

class Conf(Default):
    '''控制消息的类'''
    _struct = [  # 参考UdpFrame.parse_data 方法
        ('dstFunc', 1),
        ('arg1', 1),
        ('arg2', 1),
    ]
    _mtype = '12'
    ###
    __cycle = '01'
    __flag = '00'
    __control = '02'
    __writeValue = 13
    __sendOrRecvCount = 14
    ####
    __flag_dict = {'serial': 0, 'udp': 1, 'all': 3, 'tsk': 4} 
    __cycle_dict = {'all': 3} 
    __control_dict = {'Shutdown': 0, 'FaultFifter': 1, 'TriggerFaultFifter': 2, 'ReprogflagOnDump': 3, 'toStandby': 4, 'NotBurnFuse': 5}
    ###

    @classmethod
    def sendOrRecvCount(cls, sendcount, recvcount):
        return cls._set(cls.__sendOrRecvCount, sendcount, recvcount, auto=True)

    @classmethod
    def udpFlagSet(cls, val, mode=3):
        arg = (cls.__flag, cls.__flag_dict['udp'])
        val = cls.modeSet(val, mode)
        return cls._set(arg[0], arg[1], val, auto=True)

    @classmethod
    def serialFlagSet(cls, val, mode=3):
        arg = (cls.__flag, cls.__flag_dict['serial'])
        val = cls.modeSet(val, mode)
        frame = cls._set(arg[0], arg[1], val, auto=True)
        frame.isRecord = False # before_dump 借用 
        return frame

    @classmethod
    def notReprogOnDump(cls):
        arg = (cls.__control, cls.__control_dict['ReprogflagOnDump'])
        val = cls.modeSet(0, 3)
        return cls._set(arg[0], arg[1], val, auto=True)

    @classmethod
    def notBurnFuse(cls):
        arg = (cls.__control, cls.__control_dict['NotBurnFuse'])
        val = cls.modeSet(1, 3)
        return cls._set(arg[0], arg[1], val, auto=True)

    @classmethod
    def allFlagSet(cls, val, mode=3):
        arg = (cls.__flag, cls.__flag_dict['all'])
        val = cls.modeSet(val, mode)
        return cls._set(arg[0], arg[1], val, auto=True)

    @classmethod 
    def Shutdown(cls):

        arg = (cls.__control, cls.__control_dict['Shutdown'])
        val = cls.modeSet(1, 3)
        frame = cls._set(arg[0], arg[1], val, auto=True)
        frame._frameSendCallBack = TestPlatForm.form.before_dump
        frame.isRecord = False
        return frame
    
    @classmethod 
    def TriggerFaultFifter(cls):
        cls.on_get_slots = TestPlatForm.form.before_dump
        arg = (cls.__control, cls.__control_dict['TriggerFaultFifter'])
        val = cls.modeSet(1, 3)
        frame = cls._set(arg[0], arg[1], val, auto=True)
        frame._frameSendCallBack = TestPlatForm.form.before_dump
        frame.isRecord = False
        return frame
    
    @classmethod
    def toStandby(cls):
        arg = (cls.__control, cls.__control_dict['toStandby'])
        val = cls.modeSet(1, 3)
        return cls._set(arg[0], arg[1], val, auto=True)

    @classmethod
    def AllowFaultFifter(cls, value = 1):
        cls.on_get_slots = TestPlatForm.form.before_dump
        arg = (cls.__control, cls.__control_dict['FaultFifter'])
        val = cls.modeSet(value, 3)
        frame = cls._set(arg[0], arg[1], val, auto=True)
        frame._frameSendCallBack = TestPlatForm.form.before_dump
        frame.isRecord = False
        return frame

    @classmethod
    def bothCycleSet(cls, val, mode=3):
        arg = (cls.__cycle, cls.__cycle_dict['all'])
        val = cls.modeSet(val, mode)
        return cls._set(arg[0], arg[1], val, auto=True)

    @classmethod
    def _set(cls, arg1, arg2, arg3, auto=False):
        '''description:获得所需的指令str
        
        Attributes: args, auto
        
        Arguments:
          args:tuple 内容为整形，以十进制赋值并自动对齐，若为字符串则只有auto 为True 时才会自动对齐
          auto:bool 如果为真且该 func 被允许，将自动打开allFlag，默认为TRUE，无需显式赋值
        Returns: Type:instance of UdpSendFrame
          发送给VCU的指令'''
        def rjust(args, indexs=[0, 1, 2]):
            size_dict = {0: 2, 1: 8, 2: 8}
            for i in indexs:
                args[i] = args[i].rjust(size_dict[i], '0')

        args = [arg1, arg2, arg3]
        #myAssert(len(args) == 3)
        #args = list(args)
        for i, arg in enumerate(args):
            if isinstance(arg, int):
                val = hex(arg)[2:]
                val = re.sub(r'[Ll]', '', val)
                myAssert(len(val) <= 8)
                args[i] = val
                rjust(args, (i, ))
        if auto:
            rjust(args)
        myAssert(len(args[0]) == 2)
        myAssert(len(args[1]) == 8)
        myAssert(len(args[2]) == 8)
        command = ''.join(args)
        frame = UdpSendFrame()
        frame.mtype = cls
        frame.command = command
        return frame

class Tag(Default):
    _struct = [  # 参考UdpFrame.parse_data 方法
        ('Tag', 4),
    ]
    _mtype = '10'

    # SwRTC = '00'
    # SwITC = '02'
    # SyRTC = '04'
    # SyITC = '06'

    @classmethod
    def _print(cls):
        cls.print(cls.Tag)

    @classmethod
    def tag_send(cls, tag: str, tagType='00', cycle=0):
        '''description:生成向VCU打开桩指令
        
        Attributes: cls, tag, tagType, cycle
        
        Arguments:
          cls:class 调用该函数的实例，无需手动赋值
          tag:str 格式为 [0-9A-Fa-f]{4}[01]{2}
          tagType:str 暂未用到
          cycle:bool 该桩开始生效的测试周期
        Returns: Type:UdpSendFrame
          生成的指令
        '''
        Public.myAssert(isinstance(tag, str), tag)
        if len(tag) == 6:
            tag += '11'

        Public.myAssert(len(tag) == 8, tag)
        try:
            int(tag, 16)
        except Exception:
            Public.myAssert(False, 'int(tag,16) %s' % tag)
        Public.myAssert(int(tag, 16) & 0xEE == 0, tag)  # 末两位只能为0||1
        cls.activeTag = tag[:4]
        frame = UdpSendFrame()
        frame.mtype = cls
        frame.command = tagType + tag + Public.int2hex(cycle, 8)
        return frame

class Log(Default):
    _struct = [  # 参考UdpFrame.parse_data 方法
        ('number', 2, 1),
        ('info', 1, '', '00'),
        ('recvVal', 4, [6], ''),
    ]
    _root = 'Log_Log/default/'  # 消息保存的路径
    _mtype = '0E'

    # Log.tcp   #若定义 tcp 为 instance of TcpServer

    line_dict = {}  # 保存不同槽道的消息序号。序号大小和打印顺序无必然关系
    _toRun_list = [(lambda self:self._print(), ('_instance_',)), (lambda self:self._store(), ('_instance_',))]
    show_flag = True
    num_dict = {}  #保存不同cpu的消息序号，检查消息丢失

    def __init__(self):
        pass
        
    @classmethod
    def db_get(cls, lru: str) -> DataBase:
        if not hasattr(cls, '%s_db' % lru):
            path = os.path.dirname(__file__) + '/Log_Log/database/%s.db' % lru
            db = DataBase(path, print_func = mPrint, file_root_get = Log.path_get)
            db.createTable1(db.logform)  #创建表
            db.createTable1(db.pathform)
            setattr(cls, '%s_db' % lru, db)
        return getattr(cls, '%s_db' % lru)

    @classmethod
    @property
    def src(cls):
        '''src nodeID'''
        return cls.__src

    def _default_run(self, frame):
        self._src_node = frame.srcNodeID
        #cls.__src = int(time.time() * 1000) % 10 + 10
        self.src_lru, self.src_cpu = VCU.node2lru(self._src_node)
        self.info = binascii.unhexlify(self.info).decode()
        self.info = re.sub('\x1b[^mABCDHJKsuIh]*[mABCDHJKsuIh]', '',
                           self.info).replace('\n', '\\n').replace('\r', '')  #过滤终端控制符

        self.line_dict[self.src_lru + self.src_cpu] = self.number + 1

        self.line_dict[self.src_lru] = str(
            self.line_dict.get(self.src_lru + 'CPUA', 0) + self.line_dict.get(self.src_lru + 'CPUB', 0))
        self.msg = self.line_dict[self.src_lru] + ' ' + self.src_cpu + ' ' + self.info + ' ' + ','.join(self.recvVal)

        if (self.src_lru + self.src_cpu) in self.num_dict.keys():  #检查消息丢失
            if self.number - self.num_dict[self.src_lru + self.src_cpu] != 1 and self.number != 0:
                tempMsg = '%s %s log between %s and %s lost!\n' % (
                    self.src_lru, self.src_cpu,
                    self.num_dict[self.src_lru + self.src_cpu], self.number)
                tempMsg += 'line before %s' % self.line_dict[self.src_lru]
                Public.mPrint(tempMsg)
                self._store(tempMsg)

        self.num_dict[self.src_lru + self.src_cpu] = self.number

    def _print(self):
        if self.show_flag:
            self.print(self.src_lru, self.msg)

        if hasattr(Log, 'tcp'):
            Log.tcp.send(self.src_lru, self.msg)  # 转发消息
    
    @classmethod
    @funclog
    def path_set(cls, path):
        '''更改保存路径'''
        with threading.Lock():
            cls._root = os.path.abspath(path) + '/'
            if not os.path.exists(cls._root):
                try:
                    os.makedirs(cls._root)
                except Exception:
                    pass
        mPrint('Log path set to %s' % cls._root)
        funclog.sub_filepath = cls._root + 'funclog.log'
        
    @classmethod
    def path_get(cls):
        return cls._root 

    def _store(self, msg=''):
        dst = self.path_get() + self.src_lru + '.log'
        if len(msg) == 0:
            msg = self.msg
            dbcommit(self.db_get(self.src_lru), self.msg)
        if not os.path.exists(os.path.dirname(dst)):
            os.makedirs(os.path.dirname(dst))  #批量创建目录
        with open(dst, 'a+') as f:
            f.write(VCU.gettime() + ' ' + self.src_lru + ' ' + msg + '\r\n')

class Reply(Default):
    '''#TODO Reply 目前未作为常规消息， 在UDP 接受处特殊处理'''
    _struct = [('tstData', 1, '')]  # 参考UdpFrame.parse_data 方法
    _mtype = '3X'  #不会用到
    _toRun_list = []  #不会用到
    
    @classmethod
    @funclog
    def _reply_Send(cls, string):
        '''回复消息'''
        frame = UdpRecvFrame(string, cls)  #解析源消息
        srcNodeID = 'FF'  #本机节点
        destNodeID = frame.srcNodeID
        dataType = list(frame.dataType)  ## 此三行修改字符串的值
        dataType[0] = '3'  ## str为不可变类不能直接修改
        dataType = ''.join(dataType)  ##
        dataLen = frame.dataLen  #回复消息长度和源相同
        tstData = cls.tstData  #消息内容不变，即校验码不重新计算
        data = destNodeID + srcNodeID + dataType + dataLen + tstData
        return data

    @classmethod
    @funclog
    def _reply_Recv(cls, string):
        frame = UdpRecvFrame(string, cls)
        flag = frame.srcNodeID + frame.dataType[
            1] + cls.tstData  #保存消息源 nodeid 和消息类型的第二位 和消息内容

        FrameLib.respond_list.append((flag.upper(), time.time()))  # 添加
        if time.time() - FrameLib.respond_list[0][1] > 50.0:
            FrameLib.respond_list.pop(0)
            mPrint('_reply_Recv', time.time() - FrameLib.respond_list[0][1])

    def _default_run(self, frame):
        Reply.tstData = self.tstData
        # _ty = frame.dataType
        # if _ty[0] == '3':  #若为3 则是回复报文
        #     #mPrint('recv ack %s' % string)
        #     Reply._reply_Recv(frame.frame)
        # elif _ty[0] == '1':  #若为1 则需要回复
        #     Reply._reply_Send(frame)

#endregion

class TestPlatForm(object):
    @funclog
    def __init__(self, srcPort: int, dstPort: int, set_client = True):
        '''description: 发送测试命令的基本类
        Attributes: 
          lru_auto: bool 若为 True 发送l指令时会自动过滤不可用槽道
          Serial_Flag: bool 若为True 则会自动保存可用的串口消息
          mib: Mib  可通过此属性获取mib 信息 rt = self.mib.get_value('LruDefaultCode'， outfile = None)
          wireShark: Wireshark self.wireshark.start(...) 开始抓包
          update: func 在线更新镜像的函数
          update_cfg: func 在线更新配置的函数
          vmtool: VMTool 与虚拟机交互的实例
        Arguments:
          self:instance 调用该函数的实例，无需手动赋值
          srcPort:int 本地端口号
          dstPort:int VCU 端口号
          set_client： 自动设置udp client 默认为[]
        Returns: Type:None
          无返回值
        '''
        myAssert(not hasattr(TestPlatForm, 'form') )
        gevent.spawn(gevent.empty)  #空协程，统计空闲时间
        self.__srcPort = srcPort
        self.__dstPort = dstPort
        self.clientS = UdpClient((Config.local_ip[16], srcPort),
                                 (Config.slot_ip[16], dstPort))  #备系非安全网
        self.clientP = UdpClient((Config.local_ip[2], srcPort),
                                 (Config.slot_ip[2], dstPort))  #主系非安全网
        if set_client:
            self._set_client(mode = [], retry=0)
        TestPlatForm.form = self

        self._lru_auto = False
        self.Serial_Flag = False
        self.mib = Mib('ICMTC', Config.slot_ip, print_func = mPrint, file_root_get = Log.path_get)
        self.wireShark = Wireshark(Config.wireshark_path, Config.default_ethdesc, print_func = mPrint, file_root_get = Log.path_get)
        self._SerialRead = SerialRead(print_func = mPrint, file_root_get = Log.path_get)
        rut.slot_ip = Config.slot_ip
        #self.update = rut.update
        self.update_cfg = rut.update_cfg
        self.vmtool = VMTool()
        self.vmtool.rut = rut
        self.__viv_dict = {}
        self.__svcu_set = set()
        self.__dump_flag = (False, time.time())

    def update(self, fileType, act, module_3, dirname = '', block = True, retry = 3, savepath=None):
        if Config.version.startswith('102') and act == 0:
            act = 6
        elif act == 6:
            act = 0
        return rut.update(fileType, act, module_3, dirname, block, retry, savepath)

    @funclog
    def get_dump(self, moudle_3, CPU = 'AB', outfile_dir = None):
        if outfile_dir is None:
            outfile_dir = Log.path_get()
        assert(os.path.isdir(outfile_dir))
        rut.update_dump(moudle_3, CPU, dirname=outfile_dir)

    @funclog
    def get_eventlog(self, moudle_3):
        '''获取 vcu eventlog 结果自动保存在‘EventLog’ 文件夹
            moudle_3: str|list 目标槽道
        '''
        if not isinstance(moudle_3, list):
            moudle_3 = VCU.inputsplit(moudle_3)
        dirname = Log.path_get() + '\\EventLog'
        if not os.path.isdir(dirname):
            os.mkdir(dirname)

        for slot in moudle_3:
            viv = VCU_VIV(slot)
            for cpu in ['A', 'B']:
                event = viv.get_eventlog(cpu)
                dstpath = dirname + '\\%s_%s.log' % (VCU.node2lru((slot + 3) * 2)[0], cpu)
                if len(event) > 20:
                    mode = 'w'
                else:
                    mode = 'a'
                    if os.path.isfile(dstpath):
                        event.insert(0, '\r\nSome items followed may repeat')
                with open(dstpath, mode) as f:
                    f.write('\r\n'.join(event))

    #@staticmethod
    def __get_funcMode(self, module_3: int):
        try:
            viv = VCU_VIV(module_3)
        except Public.myException:
            funcMode = -1
        else:
            try:
                funcMode = viv.get_functionmode()
            except TimeoutError:
                funcMode = -1 #获取失败则返回
        return module_3, funcMode

    @funclog       
    def get_funcMode(self, slots) -> dict:
        '''description: 获取功能模式
        Arguments:
          self:instance 调用该函数的实例，无需手动赋值
          slots: str|list 所需获取的槽道号
        Returns: Type:dict
          dict[slot] = funcmode
        '''
        from trys import Trys

        '''通过viv 获取功能模式， vcu boot -> limation ->normal'''
        funcMode_dict = {0: 'NORMAL', 1: 'NORMAL', 2: 'NORMAL', 3: 'LIMATION', 4: 'LIMATION', 5: 'LIMATION', 6: 'BOOT_FAILED', 7: 'REPROG', 8: 'BOOT', -1: 'VIV_ERROR'}

        if not isinstance(slots, list):
            slots = VCU.inputsplit(slots)
        #trys = Trys(todo=slots, target='module_3', rt_type='all')
        func_modes = []
        #func_modes = trys(self.__get_funcMode)()
        for module_3 in slots:
            func_modes.append( self.__get_funcMode(module_3))

        rt_dict = {}
        for module_3, funcMode in func_modes:
            rt_dict[int(module_3)] = funcMode_dict[funcMode]
        
        return rt_dict

    @funclog
    def get_active_mvcu(self) -> int:
        '''获取主mvcu 的槽道号'''
        from trys import Trys
        funcMode_dict = {0: 'MVCUA', 3: 'MVCUA', 1: 'MVCUS', 4: 'MVCUS'}

        rt = []
        trys = Trys(todo=[2, 16], target='module_3', rt_type='all')
        func_modes = trys(self.__get_funcMode)()
        for module_3, funcMode in func_modes:
            if funcMode_dict.get(funcMode, 'MVCUSORERROR') == 'MVCUA':
                rt.append(module_3)

        if len(rt) > 1:
            raise Exception(rt)
        elif len(rt) == 0:
            return None
        else:
            return rt[0]

    @funclog
    def get_standby_mvcu(self) -> int:
        '''获取备mvcu 的槽道号'''
        from trys import Trys
        funcMode_dict = {0: 'MVCUA', 3: 'MVCUA', 1: 'MVCUS', 4: 'MVCUS'}

        rt = []
        trys = Trys(todo=[2, 16], target='module_3', rt_type='all')
        func_modes = trys(self.__get_funcMode)()
        for module_3, funcMode in func_modes:
            if funcMode_dict.get(funcMode, 'MVCUSORERROR') == 'MVCUS':
                rt.append(module_3)

        if len(rt) > 1:
            raise Exception(rt)
        elif len(rt) == 0:
            return None
        else:
            return rt[0]

    @property
    def lru_auto(self):
        '''为True 后 会自动过滤掉不可用的槽道，但因mib 信息存在延迟，可能会不准，耗时根据配置，最大额外耗时为7秒左右'''
        return self._lru_auto

    @lru_auto.setter
    def lru_auto(self, val: bool):
        '''为True 后 会自动过滤掉不可用的槽道，但因mib 信息存在延迟，可能会不准，耗时根据配置，最大额外耗时为7秒左右'''
        self._lru_auto = val

    @funclog
    def _VCUs_available(self, moudle_3s: list) -> list:
        '''description:获取可用的槽道号        
        Arguments:
          self:instance 调用该函数的实例，无需手动赋值
          moudle_3s：list 原始list 
        Returns: Type:list
          返回原始list 中可用的 槽道号 
        '''
        self._avais = self.mib.VCUs_available(moudle_3s)

        moudle_3s_temp = [m for m in moudle_3s if m not in self._avais]
        if len(moudle_3s_temp) > 0:
            mPrint('these lrus not available', moudle_3s_temp)
        return self._avais

    @funclog
    def tst_set(self, slots: str, frame: UdpSendFrame, openflag=True, autowait = True) -> None:
        '''description:向VCU发送指令
        
        Attributes: self, slot, func, frame, openflag, autowait
        
        Arguments:
          self:instance 调用该函数的实例，无需手动赋值
          slot:str 格式为'3-8,2.16，9'，-表示连续，[,，.]表示离散，数字为moudleID - 3
          frame:UdpSendFrame 
          openflag:bool 如果为真且该 func 被允许，将自动打开allFlag，默认为TRUE，无需显式赋值
          autowait:nool 如果为真则命令发送后会自动调用FrameLib.respondCheck() （检查命令是否发送成功）
        Returns: Type:None
          无返回值
        '''

        if frame.mtype in [
                Tag
        ] and openflag is True:  #若flag 为True 会默认为允许的类型打开 udp 和 all 标志
            self.tst_set(slots, Conf.allFlagSet(1), False, False)

        if isinstance(slots, list):
            moudle_3s = slots
        else:
            moudle_3s = VCU.inputsplit(slots)

        if self.lru_auto:  # 将此标志位置为true 时 将会自动根据mib的信息过滤槽道号，但mib信息可能延迟
            moudle_3s = self._VCUs_available(moudle_3s)

        frame.frameSendCallBack(moudle_3s)

        svcus = [slot for slot in moudle_3s if slot != 16 and slot != 2]
        if self.__dump_flag[0] and time.time() - self.__dump_flag[1] < 60:
            self.__dump_flag = (False, time.time())
            frame.isRecord = False
        elif frame.isRecord:
            for i in svcus:
                self.__svcu_set.add(i) # self.__svcu_set
        
        for moudle_3 in set(moudle_3s):
            node1, node2 = VCU.moudle_3toNode(moudle_3)  #cpuA,B
            frame.srcNodeID = 'FF'
            frame.dstNodeID = node1
            if frame.mtype._mtype[0] == '1':  #若需要回复，一段时间未收到回复则重发
                frame.crc = '0000'  # 表示crc 使用crc16
                frame.crc = Public.crc16(frame.data,
                                         int(frame.len, 16) - len(frame.crc) // 2)  # crc 不参与crc计算
                FrameLib.resigeter(frame.copy())
            if (int(frame.dstNodeID, 16) // 2) % 2 == 0: # moudleid 为偶数，则槽道号为奇数
                self._client_even.send(frame)  #通过1——16
            else:
                self._client_odd.send(frame) #通过1——2
        if autowait:
            FrameLib.respondCheck()

    @funclog
    def before_dump(self, slots):
        '''在板卡宕机前调用，'''
        self.tst_set(slots, Conf.notReprogOnDump(), openflag = False, autowait=False) # 临时关闭dump 破坏reporg
        self.tst_set(slots, Conf.notBurnFuse(), openflag = False, autowait=False) # 临时关闭dump 破坏reporg
        if isinstance(slots, list):
            moudle_3s = slots
        else:
            moudle_3s = VCU.inputsplit(slots)
        moudle_3s = set(moudle_3s) # 避免两次重复的mvcu  
        mvcus = [ slot for slot in moudle_3s if slot == 2 or slot == 16]

        if len(mvcus) == 1: # 长度为0表示双系均不宕机，无需切换， 长度为2 表示双系均会宕机， 无需切换 
            if 2 in moudle_3s: # dst = moudle_3s[0]
                dst = 16
            else:
                dst = 2
            try:
                self._set_client(dst, dst_svcu=list(self.__svcu_set), retry = 1) # 切换主mvcu
            except Public.myException:
                mPrint('the other mvcu is not usable!')
        
        svcus = moudle_3s ^ set(mvcus)
        self.__svcu_set ^= svcus & self.__svcu_set # 删除之前的消息的记录
        self.__dump_flag = (True, time.time())
        FrameLib.respondCheck()
   
    @funclog
    def _log_recv(self, client: UdpClient):
        '''接收client的 消息'''
        mPrint('recv start %s,%s' % client.remote)
        Public.thread_flag['recv'] = Public.thread_flag.get('recv', True)
        while Public.thread_flag['recv']:
            data = client.recv()
            if data is not None:
                frame = UdpRecvFrame(data)
                #默认vcu只会主动往外发log 消息，即frame.data_type始终指向Log
                frame.data_type._run_list()
                gevent.sleep(0)
            else:
                gevent.sleep(0.01)    
                 
    @funclog
    def _reSend(self, frame: UdpSendFrame):
        '''发送失败时再次发送'''
        dst = frame.dstNodeID
        frame.isRecord = frame.isRecord # 重注册
        FrameLib.resigeter(frame)  # 重注册
        dst = int(dst, 16)
        if (int(frame.dstNodeID, 16) // 2) % 2 == 0: 
            self._client_even.send(frame)  #发送给CPUA
        else:
            self._client_odd.send(frame)

    def _serial_recv(self):
        '''若self.Serial_Flag 为 true 则接收所有串口log'''
        Public.thread_flag['serial_recv'] = Public.thread_flag.get('serial_recv', True)
        flag = True
        while Public.thread_flag['serial_recv']:
            if self.Serial_Flag:
                if flag:
                    self._SerialRead.open_all(outdirname = None)
                    flag = False
                    mPrint('start listen', self._SerialRead.queue_dict.keys())
                for port, que in self._SerialRead.queue_dict.items():
                    try:
                        l_msg = que.get(block = False)
                    except Exception:
                        g_sleep(0.2)
                        continue
                    
                    serial_log = Log()
                    serial_log.src_lru = port + '_' #TODO 不加_ 特殊情况下会出现异常
                    serial_log.msg = l_msg
                    res = re.search(r'(\d+)\s+([CPUABchanel12]+)\s+\w+\s+(\S+):(\d+)>(.*)', l_msg)
                    if res is not None:
                        serial_log.info = res.group(5)
                        src_cpu = res.group(2)
                        __dict = {'chanel1': 'CPUA', 'chanel2': 'CPUB'}
                        serial_log.src_cpu = __dict.get(src_cpu, src_cpu)
                        serial_log.recvVal = []
                    else:
                        serial_log.info = l_msg
                        serial_log.src_cpu = 'CPUA'
                        serial_log.recvVal = [] #TODO 该功能只有udp_report 函数打印出来有用， 待改进 暂时未用到 
                    
                    serial_log._run_list()

                for g in self._SerialRead.greenlet_list:
                    if g.dead:
                        try:
                            g.get()
                        except Exception as e:
                            raise e
                gevent.sleep(0)
            else:
                if flag is False:
                    self._SerialRead.close_all()
                    flag = True
                gevent.sleep(1)
        self._SerialRead.close_all()

    def _wireshark_recv(self):
        Public.thread_flag['_wireshark_recv'] = Public.thread_flag.get('_wireshark_recv', True)
        temp_path = ''
        p_new = None

        def taskkill_dumpcap():
            p = subprocess.Popen('taskkill /t /f /fi "WINDOWTITLE eq %s\\test_dumpcap.exe"' % Config.wireshark_path, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            out = p.stdout.read()
            out = out.decode('gbk')

            # if '没有运行的带有指定标准的任务' not in out:
            #     mPrint(out)

        def kill_p_old(p_old):
            g_sleep(5)
            p_old.kill()

        taskkill_dumpcap()
        Public._end_process.append((taskkill_dumpcap, ()))
        while Public.thread_flag['_wireshark_recv']:
            if temp_path != Log.path_get():
                temp_path = Log.path_get()
                p_old = p_new
                p_new = self.wireShark.start(outfile='wireshark_recv.pcapng', block=False)
                if p_old is not None:
                    gevent.spawn(kill_p_old, p_old)
            else:
                g_sleep(1)

    def _mib_recv(self):
        Public.thread_flag['_mib_recv'] = Public.thread_flag.get('_mib_recv', True)
        temp_path = ''
        count = 0
        while Public.thread_flag['_mib_recv']:
            if temp_path != Log.path_get():
                temp_path = Log.path_get()
                count = 100
            else:
                count += 1
                g_sleep(1)   
            if count > 60: # 每分钟获取一次
                self.mib.get_value('lruDefaultCode', outfile= 'auto_getmib.log', raise_error=False)
                count = 0

    def _set_client(self, mode = [], dst_svcu = None, retry = 3):
        '''description: 指定发送消息的MVCU，当使用两个mvcu 时 发送给奇数槽道的svcu的消息会通过 mvcuS 转发， 发送给偶数svcu 的槽道的消息会通过mvcuP 转发
        
        Attributes: None
        
        Arguments:
          self:instance 调用该函数的实例，无需手动赋值
          mode:str|list 格式为 '2' '16' '2,16' [2,16] [] 若为空则自动选择
          retry:int 重新连接次数， 若无需重连则设为0
        Returns: Type:None
          无返回值
        '''
        if hasattr(self, '_active_svcu') and dst_svcu is None:
            dst_svcu = self._active_svcu

        if not isinstance(mode, list):
            mode = VCU.inputsplit(mode)

        for i in range(len(mode)):
            mode[i] = int(mode[i])
        if retry < 0:
            raise Public.myException( mode, 'ip is not reachable')

        __client = None
        _client_odd = None
        _client_even = None
        if Public.isIPreachable(Config.slot_ip[2]):  #判断可用的mvcu
            __client = self.clientP
            _client_odd = __client
            
        if Public.isIPreachable(Config.slot_ip[16]):
            __client = self.clientS
            _client_even = __client

        if __client is None:
            return self._set_client(mode, retry=retry - 1)

        if (2 in mode and 16 in mode): # 指定两个mvcu 同时工作
            if _client_odd is None or _client_even is None:
                gevent.sleep(5)
                return self._set_client(mode, retry=retry - 1)
        elif 16 not in mode and 2 not in mode: # 没有指定任何mvcu
            if _client_odd is None:
                _client_odd = _client_even
            elif _client_even is None:
                _client_even = _client_odd
        elif 16 in mode: # 指定16
            if _client_even is None:
                gevent.sleep(5)
                return self._set_client(mode, retry=retry - 1)
            else:
                _client_odd = _client_even
        else:# 指定2
            if _client_odd is None:
                gevent.sleep(5)
                return self._set_client(mode, retry=retry - 1)
            else:
                _client_even = _client_odd
                
        if _client_odd is _client_even:
            if _client_odd is self.clientP:
                mPrint('use mvcu p')
            else:
                mPrint('use mvcu S')
        else:
            mPrint('use 2 mvcu')
        self._client_odd = _client_odd
        self._client_even = _client_even 
        if dst_svcu is not None and len(dst_svcu) > 0:
            self.tst_set(dst_svcu, Conf.serialFlagSet(1))           

def gevent_join(greenlet_list):
    '''启动协程'''
    form = TestPlatForm.form

    if not isinstance(greenlet_list, list):
        greenlet_list = [gevent.spawn(greenlet_list)]
    elif len(greenlet_list) > 0:
        if not isinstance(greenlet_list[0], greenlet.Greenlet):
            for i in range(len(greenlet_list)):
                greenlet_list[i] = gevent.spawn(greenlet_list[i])

    default_list = [
        gevent.spawn(form._log_recv, *(form.clientP, )),  # 接收1_2P
        gevent.spawn(form._log_recv, *(form.clientS, )),  # 接收1_16S
        gevent.spawn(form._serial_recv),
        gevent.spawn(form._mib_recv),
        gevent.spawn(form._wireshark_recv)]

    default_list += greenlet_list
    gevent.joinall(default_list)

if __name__ == "__main__":
    viv = VCU_VIV(8)
    viv._cop_open('A')
    viv._SessionOpen('A')
    viv._MemReadInit('A', data='0001000000004000', dst = 'SPI')
    viv_list = []
    for i in range(50):
        temp = VCU_VIV(8)
        temp._o_data = viv._o_data
        temp._copopen_dict = viv._copopen_dict
        viv_list.append(temp)
    g_list = []
    for i in range(8):
        g = threading.Thread(targert = viv_list[i]._cop_open, args=('A',))
        g_list.append(g)
    time.sleep(10)
    viv._cop_close('A')