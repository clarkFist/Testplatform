# !/usr/bin/python
# _*_ coding: utf-8 _*_
import sys, os
'''添加系统路径'''
#region
sysPath_temp = []
paths = os.path.abspath(os.path.dirname(__file__)).split('\\')
for i in range(-1, -len(paths), -1):
    #path = '\\'.join(paths[:i] + [paths[i]])
    if 'case_test' in paths[i]:
        path = '\\'.join(paths[:i] + [paths[i]])
        break
sysPath_temp.append(path)
sys.path += [path for path in sysPath_temp if path not in sys.path]
import caseFunc
from caseFunc import Log, Config, FrameLib, Conf, Tag, Simulator, sleep, Public, gevent, funclog
import threading
import time
form = Config.form
#endregion

def LogPath_set(path):
    '''设置log 保存路径， 以脚本当前目录所造的log文件夹为根目录'''
    dir_path = os.path.dirname(__file__) + '/Log/%s/%s' % (os.path.splitext(os.path.basename(__file__))[0], path)
    Log.path_set(dir_path)

def get_svcu(count: int) -> list:
    '''获取当前caseFunc.Config.active_svcu 所设置的svcu'''
    svcus = caseFunc.Config.active_svcu()
    svcus = caseFunc.VCU.inputsplit(svcus)
    return svcus

def before_main():
    import shutil
    dir_path = os.path.dirname(__file__) + '/Log/%s/%s' % (os.path.splitext(os.path.basename(__file__))[0], '')
    if os.path.exists(dir_path):
        shutil.rmtree(dir_path)    
    LogPath_set('default')
'''''''''''' # 以上为固定头
requirement = caseFunc.StubRequirement(slots='2.16.3.14') #slots='2,3-8'  # 当前用例的环境需求 ，该变量必须赋值
assert(__name__ == "__main__") # 只能手动


def plugout(slot):
    Log.show_flag = False
    #Public.mPrint("plug out %s" % slot)
    for i in range(25):
        Public.mPrint("plug out %s" % slot)
        mode = form.get_funcMode(slot)[int(slot)]
        if mode != 'NORMAL' and mode != 'LIMATION':
            break
        time.sleep(10)
    else:
        raise Exception()
    Log.show_flag = True
    time.sleep(10)

def plugon(slot):
    Log.show_flag = False
    
    #Public.mPrint("plug out %s" % slot)
    for i in range(40):
        Public.mPrint("plug on %s" % slot)
        mode = form.get_funcMode(slot)[int(slot)]
        if mode == 'NORMAL':
            break
        time.sleep(10)
    else:
        raise Exception()
    Log.show_flag = True
    form.tst_set(slot, Conf.allFlagSet(1))
    time.sleep(20)
    
def step1():
    LogPath_set('01')
    caseFunc.power_restart()
    time.sleep(10)
    caseFunc.power_restart()
    time.sleep(30)

def step2():
    LogPath_set('02')
    for i in range(3, 3 + 5):
        caseFunc.power_restart()
        time.sleep(i * 3)
    caseFunc.power_restart()

def step3():
    LogPath_set('03')
    time.sleep(80)

def step4():
    LogPath_set('04')
    plugout(2)
    

def step5():
    LogPath_set('05')
    plugon(2)
    plugout(16)
    time.sleep(100)

def step6():
    for i in range(6, 6 + 21):
        caseFunc.power_restart()
        time.sleep(i * 2)


def main():
    for i in range(1, 7):
        eval('step%s' % i)()

'''使用例可以作为单独的模块执行''' # 格式固定
if __name__ == "__main__":
    '''使用例可以作为单独的模块执行'''
    
    from udp_loglib import TestPlatForm, gevent_join
    form = TestPlatForm(18125, 18125)
    Config.form = form
    
    def confSet():
        before_main()
        form._active_svcu = caseFunc.Config.active_svcu()
        FrameLib.respondCheck()  # 会检查之前所有的需回复消息，检查完毕后结束
        t = threading.Thread(target=main)
        t.start()
        t.join()
        Public.normal_end()
        Public.mPrint('exit')
    st = time.time()
    
    gevent_join(confSet)
    print(time.time() - st)

    