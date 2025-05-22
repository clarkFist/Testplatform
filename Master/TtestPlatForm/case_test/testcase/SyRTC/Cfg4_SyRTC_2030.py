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
requirement = caseFunc.StubRequirement(slots='2.16.8') #slots='2,3-8'  # 当前用例的环境需求 ，该变量必须赋值
assert(__name__ == "__main__") # 只能手动
    

def step2():
    LogPath_set('02')
    caseFunc.udp_loglib.rut.slot_ip = {2: '20.2.1.10'}
    form.update('lzw', 0, '2')
    time.sleep(120)
    caseFunc.check_allboard('2')
    form.update('lzw', 0, '16.8')
    time.sleep(120)
    caseFunc.check_allboard('8.16')

def step3():
    LogPath_set('03')
    caseFunc.udp_loglib.rut.slot_ip = {16: '20.1.1.10'}
    form.update('lzw', 0, '2')
    time.sleep(120)
    caseFunc.check_allboard('2')
    form.update('lzw', 0, '16.8')
    time.sleep(120)
    caseFunc.check_allboard('8.16')

def step4():
    LogPath_set('04')
    caseFunc.power_restart()
    time.sleep(60)
    caseFunc.check_allboard('8.2.16')

def step5():
    LogPath_set('05')
    time.sleep(30)
    form.tst_set('2', Conf.Shutdown())
    caseFunc.udp_loglib.rut.slot_ip = {16: '20.1.1.10'}
    form.update('lzw', 0, '16.8')
    time.sleep(120)
    caseFunc.check_allboard('8.16')

def step6():
    LogPath_set('06')
    time.sleep(30)

def step7():
    LogPath_set('07')
    form.update('lzw', 0, '16.8')
    time.sleep(120)
    caseFunc.check_allboard('8.16')

def step8():
    LogPath_set('08')
    caseFunc.power_restart()
    time.sleep(60)
    caseFunc.check_allboard('8.2.16')

def step9():
    LogPath_set('09')
    time.sleep(30)
    form.tst_set('16', Conf.Shutdown())
    caseFunc.udp_loglib.rut.slot_ip = {2: '20.2.1.10'}
    form.update('lzw', 0, '2.8')
    time.sleep(120)
    caseFunc.check_allboard('8.2')

def main():
    normal_ip_dict = caseFunc.udp_loglib.rut.slot_ip.copy()
    for i in range(2, 10):
        eval('step%s' % i)()
    caseFunc.udp_loglib.rut.slot_ip = normal_ip_dict

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

    