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
    dir_path = os.path.dirname(__file__) + '/Log/%s/%s' % (os.path.splitext(os.path.basename(__file__))[0], path)
    Log.path_set(dir_path)
def before_main():
    LogPath_set('default')
'''''''''''' # 以上为固定头
requirement = caseFunc.StubRequirement(slots=None) #slots='2,3-8'  # 当前用例的环境需求 ，该变量必须赋值

def check_dump():
    for i in range(0, 350):
        time.sleep(10)
        funcMode = form.get_funcMode(dst_slot)[dst_slot]
        if funcMode == 'VIV_ERROR':
            break
    else:
        raise Exception()
    for i in range(0, 350):
        time.sleep(10)
        funcMode = form.get_funcMode(dst_slot)[dst_slot]
        if funcMode == 'NORMAL':
            break
    else:
        raise Exception()

def step2():
    #mvcus = caseFunc.Config.active_mvcu()
    #mvcus = caseFunc.VCU.inputsplit(mvcus)
    #time.sleep(20) 
    LogPath_set('02')
    form.tst_set(dst_slot, Conf.AllowFaultFifter())
    form.tst_set(dst_slot, Tag.tag_send('00680211'))
    check_dump()
    form.get_eventlog(dst_slot)
   
def main():
    global dst_slot
    dst_slot = form.get_active_mvcu()
    form.Serial_Flag = True
    step2()


'''使用例可以作为单独的模块执行''' # 格式固定
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
        Public.normal_end()
        Public.mPrint('exit')
    st = time.time()
    
    gevent_join(confSet)
    print(time.time() - st)

    