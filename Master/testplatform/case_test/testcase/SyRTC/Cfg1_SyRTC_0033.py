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
        1#shutil.rmtree(dir_path)
        
    #LogPath_set('default')
'''''''''''' # 以上为固定头
requirement = caseFunc.StubRequirement(slots='2.16.3-8') #slots='2,3-8'  # 当前用例的环境需求 ，该变量必须赋值
assert(__name__ == "__main__") # 只能手动

###########################################
def plugout(slot):
    Log.show_flag = False
    #Public.mPrint("plug out %s" % slot)
    form.before_dump(slot)
    caseFunc.wait_to_ack_ext("plug out %s" % slot, timeout=200)
    Log.show_flag = True
    time.sleep(10)

def plugon(slot):
    Log.show_flag = False
    
    #Public.mPrint("plug out %s" % slot)
    caseFunc.wait_to_ack_ext("plug on %s" % slot, timeout=200)
    Log.show_flag = True
    form.tst_set(slot, Conf.allFlagSet(1))
    time.sleep(20)


def UnplugNetworkLine():
    Log.show_flag = False
    #Public.mPrint("plug out %s" % slot)
    # form.before_dump(slot)
    caseFunc.wait_to_ack_ext("plug out 2rd and 4th network line  %s" % 16, timeout=200)
    Log.show_flag = True
    time.sleep(100)
    caseFunc.wait_to_ack_ext("plug on 2rd and 4th network line  %s" % 16, timeout=200)
    

def step2(slot):
    LogPath_set('02')
    form.tst_set(slot, Tag.tag_send('00330111'))
    form.tst_set(dst_slot, Tag.tag_send('00330311'))
    time.sleep(60)

def step3():
    Log.show_flag = False
    form.tst_set(dst_slot, Tag.tag_send('00330311'))
    caseFunc.wait_to_ack_ext("plug out 2rd and 4th network line  %s" % 16, timeout=200) 
    Log.show_flag = True
    LogPath_set('03')
    time.sleep(100)


def step4():
    LogPath_set('04')
    caseFunc.wait_to_ack_ext("plug on 2rd and 4th network line  %s" % 16, timeout=200)
    time.sleep(60)
    # 003302 trigger safe state
    form.tst_set(16, Tag.tag_send('00330211')) 
    time.sleep(60)

def step5():
    # 重启
    caseFunc.power_restart()
    time.sleep(60)
    caseFunc.check_allboard(timeout=50)

def step6():   
    LogPath_set('06')
    # 设置主系
    active = form.get_active_mvcu()
    while active != 2:
        form.tst_set(active, Conf.toStandby())
        time.sleep(5)
        active = form.get_active_mvcu()
    Public.mPrint('mvcu-A is %s' % active)
    form.Serial_Flag = True
    form.tst_set(dst_SVU_slot, Tag.tag_send('00330111'))# submi 11 to MVCU
    form.tst_set(dst_slot, Tag.tag_send('00330311'))
    time.sleep(60)


def step7():   
    LogPath_set('07')
    # 拔掉主系
    plugout(2)
    time.sleep(60)

dst_slot = ''
def main():
    global dst_slot
    global dst_SVU_slot
    dst_slot = '2,16'
    dst_SVU_slot = '3-8'
    # 设置主系
    active = form.get_active_mvcu()
    while active != 2:
        form.tst_set(active, Conf.toStandby())
        time.sleep(5)
        active = form.get_active_mvcu()
    Public.mPrint('mvcu-A is %s' % active)
    form.Serial_Flag = True
    # 模拟器发送111
    sims = Simulator('cfg1')
    sims.openwith(1, 'E')
    # submi 11 to MVCU
    step2(dst_SVU_slot)

    # 打印收发ip

    step3()

    step4()

    step5()

    step6()

    step7()

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
    