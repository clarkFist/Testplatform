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
from caseFunc import Log, Config, FrameLib, Conf, Tag, , sleep, Public, gevent, funclog
from autopower import AutoPower
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
requirement = caseFunc.StubRequirement(slots='2,16') #slots='2,3-8'  # 当前用例的环境需求 ，该变量必须赋值

def check_dump():
    for i in range(0, 350):
        time.sleep(10)
        funcMode = form.get_funcMode(2)[2]
        if funcMode == 'VIV_ERROR':
            break
    else:
        raise Exception()
    for i in range(0, 350):
        time.sleep(10)
        funcMode = form.get_funcMode(2)[2]
        if funcMode == 'NORMAL':
            break
    else:
        raise Exception()

def step2():
    #time.sleep(20) 
    LogPath_set('02')
    form.tst_set('16', Tag.tag_send('52430111'))
    time.sleep(30)

def step3():
    LogPath_set('03')
    form.tst_set('16', Tag.tag_send('52430210'))
    time.sleep(20)


   
def main():
    form.Serial_Flag = True
    for i in range(3, 4):
        eval('step%s' % i)()


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

    