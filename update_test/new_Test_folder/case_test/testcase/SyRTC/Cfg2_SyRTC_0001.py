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
#endregion
import caseFunc
from caseFunc import Log, Config, FrameLib, Conf, Tag, Simulator, sleep, Public, gevent, funclog
import threading
import time
form = Config.form

def LogPath_set(path):
    dir_path = os.path.dirname(__file__) + '/Log/%s/%s' % (os.path.splitext(os.path.basename(__file__))[0], path)
    Log.path_set(dir_path)
LogPath_set('default')
###########################################
def step2(count):
    LogPath_set('%s/' % count + '02')
    if count % 2 == 1:
        funcMode = form.get_funcMode(2)
        if funcMode != 'MVCUA':
            form.tst_set('16', Conf.toStandby())
    else:
        funcMode = form.get_funcMode(16)
        if funcMode != 'MVCUA':
            form.tst_set('2', Conf.toStandby())

    form.tst_set(dst_slot, Tag.tag_send('00010111'))
    sims = Simulator('cfg2')
    for i in range(1, 7):
        sims.openwith(i, 'E')
    time.sleep(100)

def step3(count):
    LogPath_set('%s/' % count + '03')
    sims = Simulator('cfg2')
    for i in range(1, 7):
        sims.openwith(i, '2')
    time.sleep(100)

def step4(count):
    LogPath_set('%s/' % count + '04')
    form.tst_set('2', Conf.TriggerFaultFifter())
    time.sleep(30)

def step5(count):
    LogPath_set('%s/' % count + '05')
    time.sleep(150)
def step6(count):
    LogPath_set('%s/' % count + '06')
    form.tst_set('16', Conf.TriggerFaultFifter())
    time.sleep(30)

def step7(count):
    LogPath_set('%s/' % count + '07')
    time.sleep(150)

def step8(count):
    LogPath_set('%s/' % count + '08')
    form.tst_set('16', Conf.TriggerFaultFifter())
    time.sleep(30)

def step9(count):
    LogPath_set('%s/' % count + '09')
    form.tst_set('2', Conf.TriggerFaultFifter())

dst_slot = '8'
def main():
    form.Serial_Flag = True
    count = 1
    #caseFunc.vm_build_update('2')
    step2(count)
    step3(count)
    step4(count)
    step5(count)
    step6(count)
    step7(count)
    step8(count)
    step9(count)

if __name__ == "__main__":
    '''使用例可以作为单独的模块执行'''
    
    from udp_loglib import TestPlatForm, gevent_join
    form = TestPlatForm(18125, 18125)
    Config.form = form
    
    def confSet():
        FrameLib.respondCheck()  # 会检查之前所有的需回复消息，检查完毕后结束
        t = threading.Thread(target=main)
        t.start()
        t.join()
        Public.thread_end()
        Public.mPrint('exit')
    st = time.time()
    
    gevent_join(confSet)
    print(time.time() - st)

    