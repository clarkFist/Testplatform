# _*_ coding: utf-8 _*_
from __future__ import print_function
import os, sys
import io
import re
import pysnmp.hlapi as snmp
from trys import Trys
import gevent
from gevent.lock import BoundedSemaphore
import subprocess
from copy import deepcopy
from datetime import datetime
from sub_module_base import SubMoudleBase
from FuncLog import FuncLog

funclog = FuncLog()
#CONFIG_START
slot_ip = {2: '20.2.1.10', 16: '20.1.1.10'}
#CONFIG_END
def isIPreachable(ip):
    '''通过ping命令判断ip是否可以通信'''
    p = subprocess.Popen(['ping', '-n', '1', ip], stdout=subprocess.PIPE)
    p.wait()
    out = str(p.stdout.read())
    if 'TTL=' in out:
        return True
    else:
        return False

def gettime():
    '''fmt:%Y-%m-%d %H:%M:%S'''
    return datetime.now().strftime('%Y-%m-%d %H:%M:%S')
# endregion


class Mib(SubMoudleBase):

    def __init__(self, path, ips = slot_ip, **kwargs):
        self.__generation('%s\\' % os.path.dirname(__file__) + path)
        if isinstance(ips, dict):
            self._mib_ip = list(ips.values())
        elif isinstance(ips, list):
            self._mib_ip = deepcopy(ips)
        elif isinstance(ips, tuple):
            self._mib_ip = deepcopy( list(ips) )
        elif isinstance(ips, str):
            self._mib_ip = [ips]
        self._default_ip = deepcopy( self._mib_ip )
        self._trys = Trys(todo=self._mib_ip, target='snmpip', rt_type='first')
        self._sem_dict = {}
        for value in self._default_ip:
            self._sem_dict[value] = BoundedSemaphore(1)

    def __generation(self, path):
        self._read_dict = {}
        self._read_dict['iso'] = {'__value': '1'}
        self._name_dict = {'iso': ['iso']}
        with io.open(path, 'r', encoding='utf8') as f:
            mib_content = f.read()
        mib_content = re.sub(r'  +', ' ', mib_content)
        mib_content = re.search(r'\n\s*([\w\-_]+)\s+\w+\s*::=\s*BEGIN(.*)END', mib_content, re.S)
        #mib_name = mib_content.group(1)
        mib_content = mib_content.group(2)
        mib_content = re.sub(r'IMPORTS[^;]+;', '', mib_content)
        #mib_content = re.sub(r'\s\w+[^:]*::=\s*SEQUENCE\s*{[^}]*}', '', mib_content)
        mib_sub = re.findall(r'\s(\w+)\s+OBJECT[^:]+::=\s*{\s*(\w+)\s+(\d+)\s*}', mib_content, re.S)
        
        for name, sup, value in mib_sub:
            self._name_dict[name] = self._name_dict[sup] + [name]
            self._read_dict_get(sup)[name] = {}

            self._read_dict_get(name)['__value'] = self._read_dict_get(sup)['__value'] + '.' + value
            
            if 'number' in name.lower():
                self._read_dict_get(sup)['number'] = self._read_dict_get(name)['__value']
            elif 'table' in name.lower():
                self._read_dict_get(name)['number'] = self._read_dict_get(sup)['number']

            if 'table' in sup.lower():
                self._read_dict_get(name)['number'] = self._read_dict_get(sup)['number']
            
            if 'entry' in sup.lower():
                self._read_dict_get(name)['mode'] = 'entry'
                self._read_dict_get(name)['number'] = self._read_dict_get(sup)['number']
            else:
                self._read_dict_get(name)['mode'] = 'element'

            self._read_dict_get(sup)['mode'] = 'sup'
        
    @funclog                
    def _get(self, objID, snmpip = '', add='.0'):
        '''使用trys装饰器必须拥有todo形参，调用函数时无需对todo赋值。
        该函数代码会通过todo 遍历装饰器参数的列表，直至遍历完成或未return Trys.todoException'''
        if not isIPreachable(snmpip):
            if snmpip in self._mib_ip:
                self._mib_ip.remove(snmpip)
            return Trys.todoException('ip is not reachable')
        
        objID += add
        self._sem_dict[snmpip].acquire(timeout=30)
        res = snmp.getCmd(snmp.SnmpEngine(), snmp.CommunityData('URBALIS', mpModel=0),
                          snmp.UdpTransportTarget((snmpip, 161)), snmp.ContextData(), snmp.ObjectType(snmp.ObjectIdentity(objID)))
        try:
            rt = next(res)[3][0][1]._value
        except Exception:
            if snmpip in self._mib_ip:
                self._mib_ip.remove(snmpip)
            return Trys.todoException('cant get snmp')
        self._sem_dict[snmpip].release()
        if isinstance(rt, bytes):
            rt = rt.decode()
        return rt
    
    @funclog
    def _walk(self, objID, end_idx, snmpip):
        if not isIPreachable(snmpip):
            if snmpip in self._mib_ip:
                self._mib_ip.remove(snmpip)
            return Trys.todoException('ip is not reachable')
        res = []

        assert(end_idx > 0)
        
        objID += '.0'
        self._sem_dict[snmpip].acquire(timeout=30)
        for e1, e2, e3, tabble in snmp.nextCmd(snmp.SnmpEngine(), snmp.CommunityData('URBALIS', mpModel=0),
                                               snmp.UdpTransportTarget((snmpip, 161)), snmp.ContextData(), 
                                               snmp.ObjectType(snmp.ObjectIdentity(objID)), maxRows=end_idx):
            temp = tabble[0][1]._value
            if isinstance(temp, bytes):
                temp = temp.decode()
            res.append(temp)
        if len(res) == 0:
            if snmpip in self._mib_ip:
                self._mib_ip.remove(snmpip)
            return Trys.todoException('cant get snmp')
        self._sem_dict[snmpip].release()
        return res

    def _read_dict_get(self, key):
        if isinstance(key, list):
            point = self._read_dict
            for k in key:
                point = point[k]
            # if not isinstance(key, dict):
            #    point = {}
            return point
        else:
            return self._read_dict_get(self._name_dict[key])

    def _get_objid(self, name):
        return self._read_dict_get(name)['__value']

    @funclog
    def get_value(self, name, lru = 0, outfile = None, raise_error = True):
        '''description:获取MIB 消息
        
        Attributes: self, name, lru, outfile, raise_error
        
        Arguments:
          self:instance 调用该函数的实例，无需手动赋值
          name:str 目标属性名称
          lru:int  仅可当 name 是 Entry 有效，使mib 仅获取该lru的信息
          outfile:str  当不为 None 时，会将结果以追加的方式保存，无需指定目录 
          raise_error:bool 如果为真则获取mib 失败时会抛出异常
        Returns: Type:str or list
          当获取的 mib 为 entry 且 lru 为 0 时 返回 list，否则为str
        '''
        objID = self._get_objid(name)
        mode = self._read_dict_get(name)['mode']
        
        def get():
            if mode == 'entry':
                if lru != 0:
                    tmp = self.lruIndex_get(lru)
                    return self._trys(self._get)(objID + '.%s' % tmp, add='')
                else:
                    end_index = self._trys(self._get)(self._read_dict_get(name)['number'])
                    return self._trys(self._walk)(objID, end_index)
            elif mode == 'element':
                return self._trys(self._get)(objID)
            else:
                raise Exception(mode)
        rt = get() # int or str or list
        if rt is None:
            self._mib_ip.clear()
            for ip in self._default_ip:
                self._mib_ip.append(ip)
            rt = get()
            if rt is None:
                if raise_error:
                    raise Exception(name)
                else:
                    rt = None
        if outfile is not None:
            if isinstance(rt, list):
                pass#rt = ' '.join(rt)
            outfile = self.get_path(outfile)
            with open(outfile, 'a') as f:
                f.write('%s %s: %s \r\n' % (gettime(), name, rt ) )
        return rt


class VCU_Mib(Mib):

    def lruIndex_get(self, lru, sp = {'1_2P': '1_2', '1_16S': '1_16'}):
        if not hasattr(self, 'lruIndex'):
            self.lruIndex = {}
            lruDescr_value = self.get_value('lruDescr')
            for i in range(len(lruDescr_value)):
                self.lruIndex[lruDescr_value[i].split(' ')[0]] = i + 1
        if isinstance(lru, int):
            lru = '%s_%s' % ((lru - 1) // 16 + 1, (lru - 1) % 16 + 1 )

        return self.lruIndex[sp.get(lru, lru)]

    def VCUs_available(self, moudle_3s):
        moudle_3s.sort()
        code = self.get_value('lruDefaultCode')
        rt = []
        for i, v in enumerate(moudle_3s):
            try:
                val = self.lruIndex_get(v) - 1
            except KeyError:
                continue
            if code[val] != 15:
                rt.append(v)
        rt = list(set(rt) & set(moudle_3s))
        return rt

    def VCUs_all(self):
        code = self.get_value('lruDescr')
        moudle_3s = []
        for i, v in enumerate(code):
            lru = v.split(' ')[0]
            rack, slot = lru.split('_')
            moduleID_3 = (int(rack) - 1) * 16 + int(slot)
            moudle_3s.append(moduleID_3)
        return moudle_3s
if __name__ == "__main__":
    import time
    print(os.getcwd())
    mib = VCU_Mib('ICMTC')
    print(mib.VCUs_all())
    mib.get_value('name')