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

# def plugout(slot):
#     Log.show_flag = False
#     form.before_dump(slot)
#     #Public.mPrint("plug out %s" % slot)
#     for i in range(25):
#         Public.mPrint("plug out %s" % slot)
#         mode = form.get_funcMode(slot)[int(slot)]
#         if mode != 'NORMAL' and mode != 'LIMATION':
#             break
#         time.sleep(10)
#     else:
#         raise Exception()
#     Log.show_flag = True
#     time.sleep(10)

# def plugon(slot):
#     Log.show_flag = False
    
#     #Public.mPrint("plug out %s" % slot)
#     for i in range(40):
#         Public.mPrint("plug on %s" % slot)
#         mode = form.get_funcMode(slot)[int(slot)]
#         if mode == 'NORMAL':
#             break
#         time.sleep(10)
#     else:
#         raise Exception()
#     Log.show_flag = True
#     form.tst_set(slot, Conf.allFlagSet(1))
#     time.sleep(20)
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
    
def step2(count):
    LogPath_set('%s/' % count + '02')
    form.tst_set(dst_slot, Tag.tag_send('00610111'))
    sims = Simulator('cfg4')
    for i in range(1, 7):
        sims.openwith(i, 'E')
    time.sleep(60)

def step3(count):
    LogPath_set('%s/' % count + '03')
    sims = Simulator('cfg4')
    for i in range(1, 7):
        sims.openwith(i, '2')
    time.sleep(60)

def step4(count):
    LogPath_set('%s/' % count + '04')
    plugout(2)
    time.sleep(5)

def step5(count):
    LogPath_set('%s/' % count + '05')
    time.sleep(40)
    plugon(2)
    

def step6(count):
    LogPath_set('%s/' % count + '06')
    plugout(16)
    time.sleep(5)

def step7(count):
    LogPath_set('%s/' % count + '07')
    time.sleep(40)
    plugon(16)

def step8(count):
    LogPath_set('%s/' % count + '08')
    plugout(16)
    time.sleep(10)

def step9(count):
    LogPath_set('%s/' % count + '09')
    plugout(2)
    time.sleep(15)

dst_slot = ''
def main():
    global dst_slot
    dst_slot = '3-8'
    form.Serial_Flag = True
    start = (1, 2)
    last_step = False 
    if start[1] > 2:
        form.tst_set(dst_slot, Tag.tag_send('00610111'))
        sims = Simulator('cfg4')
        for i in range(1, 7):
            sims.openwith(i, '2')

    for count in range(1, 7):
        if count == 6:
            last_step = True
            break
        if count < start[0]:
            continue
        if count % 2 == 1:
            active_req = 2
        else:
            active_req = 16
        active = form.get_active_mvcu()
        while active != active_req:
            form.tst_set(active, Conf.toStandby())
            time.sleep(5)
            active = form.get_active_mvcu()
        Public.mPrint('mvcu-A is %s' % active)
        #caseFunc.vm_build_update('2')
        form.tst_set('2.16', Conf.allFlagSet(1))
        if 2 >= start[1]:
            step2(count)
        if 3 >= start[1]:
            step3(count)
        if 4 >= start[1]:
            step4(count)
        if 5 >= start[1]:
            step5(count)
        if 6 >= start[1]:
            step6(count)
        if 7 >= start[1]:
            step7(count)
        if 8 >= start[1]:
            step8(count)
        if 9 >= start[1]:
            step9(count)
        start = (0, 0)
        caseFunc.power_restart()
        time.sleep(60)
        caseFunc.check_allboard(timeout=50)
    
    if last_step:
        LogPath_set('12')
        form.tst_set(dst_slot, Tag.tag_send('00610111'))
        sims = Simulator('cfg4')
        for i in range(1, 7):
            sims.openwith(i, 'E')
        time.sleep(50)
        return

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
    