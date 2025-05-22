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

def get_svcu() -> list:
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
requirement = caseFunc.StubRequirement(slots=None, svcu_count=None) #slots='2,3-8'  # 当前用例的环境需求 ，该变量必须赋值

svcu = get_svcu()
def step2():
    global mvcu
    LogPath_set('02')
    sims = Simulator('cfg2')
    for i in range(1, 7):
      sims.openwith(i, 'E')
    sleep(120)

def main():
    global dst_slot
    dst_slot = '3-8'
    form.Serial_Flag = True
    form.tst_set(dst_slot, Tag.tag_send('00010111'))
    step2()

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
