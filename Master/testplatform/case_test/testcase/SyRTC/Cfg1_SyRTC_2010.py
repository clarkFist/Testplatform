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

def func(slot):
    form.tst_set(slot, Conf.TriggerFaultFifter())
    time.sleep(100)
    caseFunc.check_allboard(slot, timeout=300)
    form.tst_set(slot, Tag.tag_send('00020111'))

def step2():
    LogPath_set('02')
    sims = Simulator('cfg1')
    for i in range(7):
        sims.openwith(1, 'C')
        sims.openwith(4, '0' * 32 + '6')
        time.sleep(30)
        sims.openwith(1, '0')
        sims.openwith(4, '0')
        time.sleep(30)
    sims.openwith(1, 'C')
    sims.openwith(4, '0' * 32 + '6')

def step3():
    LogPath_set('03')
    for i in range(5):
        time.sleep(30)
        func('3.14')

def step4():
    LogPath_set('04')
    for i in range(5):
        time.sleep(30)
        func('2')

def step5():
    LogPath_set('05')
    for i in range(5):
        time.sleep(30)
        func('16')

def main():
    form.tst_set('3.14', Conf.allFlagSet(1))
    form.tst_set('3', Conf.sendorWriteCount(32, 128))
    form.tst_set('14', Conf.sendorWriteCount(32, 32))
    form.tst_set('2.16.3.14', Tag.tag_send('00020111'))
    step2()
    step3()
    step4()
    step5()

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

    