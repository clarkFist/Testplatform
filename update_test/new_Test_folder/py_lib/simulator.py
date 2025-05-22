# !/usr/bin/python
# _*_coding:utf-8_*_ 
from __future__ import print_function
import sys, os, time
import win32gui
import win32con
import win32api
import io, re
from FuncLog import FuncLog
funclog = FuncLog() 
#CONFIG_START
class FSFB2:

    @classmethod
    def dirname(cls, num):
        return 'CC%s' % num

    @classmethod
    def sim_exe(cls, num):
        return 'ECIDSim.exe'

    @classmethod
    def get_name(cls, num):
        return 'SIMU%s' % num

class RSSP1:

    @classmethod
    def dirname(cls, num):
        return 'RSSP1_%s' % num

    @classmethod
    def sim_exe(cls, num):
        return 'RSSP1Sim.exe'  
    
    @classmethod
    def get_name(cls, num):
        return 'RSSP1-%s' % num
#CONFIG_END

class Config:
    #CONFIG_START
    root = r'C:\Users\LiuXintian\Desktop\data_new'
    cfg1 = ('data1-FSFB2-每个节点配置224', FSFB2, FSFB2) # 协议混用怕配置多个类型，最多支持6 个会话
    cfg2 = ('data2-FSFB2-发送码位不同', FSFB2)
    cfg3 = ('data3-RSSP-I-1344 para-6auto-拷机', RSSP1)
    cfg4 = ('data4-RSSP-I-6auto发送码位不同', RSSP1)
    #CONFIG_END
 
    @classmethod
    def get_simpath(cls, cfg: tuple, num: int):
        assert(num > 0 and num < 7)
        assert(len(cfg) > 1)

        dirname = cfg[0]
        if len(cfg) < num + 1:
            _cfg = cfg[-1] #type:FSFB2
        else:
            _cfg = cfg[num]
        path = os.path.join(cls.root, dirname, _cfg.dirname(num), _cfg.sim_exe(num))
        return path

    @classmethod
    def get_sim_name(cls, cfg: tuple, num: int):
        assert(num > 0)
        if len(cfg) < num + 1:
            _cfg = cfg[-1] #type:FSFB2
        else:
            _cfg = cfg[num]
        name = _cfg.get_name(num)
        return name


class Simulator:
    @funclog
    def __init__(self, config: str, print_func = print):
        config = getattr(Config, config)
        self.config = config
                
        self.sims = [self.get_simpath(n) for n in range(1, 7)]
        self.print = print_func
    
    @funclog
    def openwith(self, num: int, value='', hide = True):

        hwnd = self.get_Sim_hwnd(num)
        if hwnd is not None:
            self.close(num)
            self.print('sim %s already opened is closed' % num)
            time.sleep(0.5)
            return self.openwith(num, value, hide)
    
        sim_path = self.get_simpath(num)
        sim_dir = os.path.dirname(sim_path)
        win32api.ShellExecute(0, 'open', sim_path, '', sim_dir, 1 if hide is False else 0)

        #os.system('start %s' % sim_path)
        if len(value) > 0:
            with open('%s/../initValue.ini' % sim_dir, 'r') as f:
                lines = f.readlines()

            temp_name = Config.get_sim_name(self.config, num) # 模拟器version 的version字段
            for i, line in enumerate(lines):
                if re.search(r'^%s:.*' % temp_name, line) is not None:
                    temp = re.sub(r'^%s:.*' % temp_name, '%s:%s' % (temp_name, value), line)
                    lines[i] = temp
                    break
            with open('%s/../initValue.ini' % sim_dir, 'w') as f:
                f.writelines(lines)
        time.sleep(0.5)
        hwnd = self.get_Sim_hwnd(num)
        if hwnd is None:
            self.print('open%s failed' % num)
            return False
        time.sleep(1)
        return True

    def __iter__(self):
        return self.__next__()

    def __next__(self):
        for i in range(1, len(self.sims) + 1):
            yield i
        #raise StopIteration

    @funclog
    def close(self, num):
        hwnd = self.get_Sim_hwnd(num)
        if hwnd is None:
            self.print('%s not open' % num)
            return
        win32gui.PostMessage(hwnd, win32con.WM_CLOSE, 0, 0)

    def get_simpath(self, num):
        return Config.get_simpath(self.config, num)

    def get_Sim_hwnd(self, name):
        rt = None
        temp = {}
        if isinstance(name, int):
            name = Config.get_sim_name(self.config, name)

        def enumHandler(hwnd, name):
            if win32gui.IsWindowVisible(hwnd):
                temp[win32gui.GetWindowText(hwnd)] = temp.get(win32gui.GetWindowText(hwnd), [])  
                temp[win32gui.GetWindowText(hwnd)].append(hwnd)    
        win32gui.EnumWindows(enumHandler, name)
        if name in temp.keys():
            if len(temp[name]) > 1:
                if name != 'FSFB2Sim':
                    self.print('exits more than one named:%s' % name)
            rt = temp[name][0]
        return rt


if __name__ == "__main__":
    sim = Simulator('cfg2')
    for i in sim:
        print(i)
        sim.openwith(i, 'ABCD')
    print('end')
    while True:
        pass