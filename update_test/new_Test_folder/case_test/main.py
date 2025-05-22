import os
import sys
import importlib.util
import importlib.machinery
import re
from datetime import datetime

paths = os.path.abspath(__file__).split('\\')
form_path = '\\'.join(paths[:-2])
if form_path not in sys.path:
    sys.path.append( form_path )
import udp_loglib
from udp_loglib import Log, Tag, Conf, FrameLib, Public, VCU_VIV, VMTool
from udp_loglib import gevent, threading, time
from autopower import AutoPower
import caseFunc

def gettime():
    '''fmt:%Y-%m-%d %H:%M:%S'''
    return datetime.now().strftime('%Y-%m-%d %H:%M:%S')

def case_switch():
    '''用例切换
       重启电源， 随后检查板卡的功能模式，若为reprog 则对其进行在线更新，若为限制态则抛出异常
       在线更新后若不为正常模式则抛出异常
    '''
    global form
    global active_mvcu
    global active_svcu
    global normal_path
    Public.mPrint('switch')
    AutoPower.sendPowerOffCommand("1,2,3,4") # 重启
    gevent.sleep(2)
    AutoPower.sendPowerOnCommand("1,2,3,4")
    gevent.sleep(90)

    form._set_client(mode=active_mvcu, dst_svcu='')
    form.update('S19', 0, active_mvcu, dirname=normal_path)
    gevent.sleep(130) # 等待mvcu 进入normal
    form.tst_set(active_mvcu, Conf.allFlagSet(1))  
    FrameLib.respondCheck() # 默认打开mvcu的log, 同时确保mvcu 处于normal模式
    #module_3s = form.mib.VCUs_all() # 获取目前配置所有的板卡槽道
    module_3s = udp_loglib.VCU.inputsplit(active_svcu)

    form.update('S19', 0, active_svcu, dirname=normal_path)
    gevent.sleep(100)
    to_check = module_3s
    flag_dict = {} # 上电完成允许一次异常状态
    start_time = time.time()
    while len(to_check) > 0:
        gevent.sleep(10)
        func_mode_dict = form.get_funcMode(to_check)
        for slot, funcmode in func_mode_dict.items():
            if funcmode == 'VIV_ERROR': # 无法获取
                if time.time() - start_time > 230:
                    raise Exception(slot, funcmode)
                else:
                    continue
            elif funcmode != 'NORMAL': # 更新后仍是异常模式
                if flag_dict.get(slot, False):
                    raise Exception(slot, funcmode)
                else:
                    flag_dict[slot] = True

            else: # 正常模式
                to_check.remove(slot)

def pre_check(cases):
    global case_dict
    global active_mvcu
    global active_svcu
    mvcus = udp_loglib.VCU.inputsplit(active_mvcu)
    svcus = udp_loglib.VCU.inputsplit(active_svcu)

    mod_dict = {}
    for case in cases:
        mod_name = os.path.splitext(case)[0]
        loader = importlib.machinery.SourceFileLoader(mod_name, '%s/%s' % (case_dict[case], case))
        spec = importlib.util.spec_from_loader(loader.name, loader)
        mod = importlib.util.module_from_spec(spec)
        loader.exec_module(mod)
        mod_dict[case] = mod
        if mod.requirement.slots is not None:
            re_slots = udp_loglib.VCU.inputsplit(mod.requirement.slots)
            __ = [slot for slot in re_slots if slot not in mvcus]
            __ = [slot for slot in __ if slot not in svcus]
            if len(__) > 0:
                raise Exception('stub requirements', case, __)
    return mod_dict

def case_start(mod):
    global form
    global active_mvcu
    global active_svcu
    global normal_path
    form._set_client(mode=active_mvcu)
    form.tst_set(active_mvcu, Conf.allFlagSet(1))# 默认打开mvcu的log, 同时确保mvcu 处于normal模式
    form.get_eventlog(active_mvcu)
    form.get_eventlog(active_svcu)

if __name__ == "__main__":

    setattr(sys.modules['udp_loglib'], 'caseFunc', caseFunc) # 使udp_loglib 可以执行caseFunc的函数
    case_paths = caseFunc.Config.case_path() # 获取本次的用例范围
    case_suffixs = caseFunc.Config.get_suffix() 
    case_prefixs = caseFunc.Config.get_prefix()
    cases = []
    case_dict = {}
    for path in case_paths:
        _lists = os.listdir(path)
        for case in _lists:
            if os.path.isfile('%s/%s' % (path, case) ):
                cases.append(case)
                case_dict[case] = path

    for case in cases[:]:
        case_lower = case.lower()
        prefixs = case_lower.split('_')
        __ = [prefix for prefix in case_prefixs if prefix.lower() not in prefixs] # 是否有不满足条件的前缀
        if len(__) > 0:
            cases.remove(case)
        else:
            __ = [suffix for suffix in case_suffixs if case_lower.endswith(suffix.lower()) ] # 是否有满足的后缀
            if len(__) == 0:
                cases.remove(case)

    form = udp_loglib.TestPlatForm(18125, 18125, set_client=False)
    form.Serial_Flag = True
    caseFunc.Config.set_form(form) # 指定用例的form
    Log._toRun_list = [
        (lambda _:caseFunc.read_dict_get(_.src_lru).put((_.src_cpu, _.info, _.recvVal)), ('_instance_',)), 
        (lambda _: _._store(), ('_instance_',))] # 自定义log 的torun
    #Log._toRun_list.append( (lambda _: caseFunc.read_dict_get(_.src_lru).put((_.src_cpu, _.info, _.recvVal)), ('_instance_',)))
    active_mvcu = caseFunc.Config.active_mvcu()
    active_svcu = caseFunc.Config.active_svcu()
    form._active_svcu = active_svcu
    mod_dict = pre_check(cases) 

    normal_path = caseFunc.Config.get_normal_image_path()
    cmd_type = 'mvcu'
    form.vmtool.copyImage_fromVM(cmd_type, dstpath=normal_path)
    cmd_type = 'svcu'
    form.vmtool.copyImage_fromVM(cmd_type, dstpath=normal_path)
    form.Serial_Flag = True

    def main():

        udp_loglib.funclog.wirte(cases)
        Log.path_set(os.path.dirname(__file__) + '/Log/default')

        for case in cases:
            Public.mPrint('start %s' % case)
            mod = mod_dict[case]
            mod.before_main()
            case_start(mod)
            try:
                ret = mod.main() # 执行用例的 main 方法
                if ret is None:
                    ret = ''
            except Exception as e:
                'execption'
                testcase_log = '%s execption at testcase %s/%s %s' % (gettime(), case_dict[case], case, e)
                Public.normal_end()
                Public.mPrint(testcase_log)
                with open('%s/log/testcase.log' % os.path.dirname(__file__), 'w+') as f:
                    f.write(testcase_log)
                raise e
            else:
                'success'
                testcase_log = '%s normal end, testcase %s/%s' % (gettime(), case_dict[case], case)
                Public.mPrint(testcase_log)
                with open('%s/log/testcase.log' % os.path.dirname(__file__), 'w+') as f:
                    f.write(testcase_log)
            case_switch()

        Public.normal_end()

    def thead_start():
        try:
            main()
        except Exception as e:
            Public.program_end(e)

    t = threading.Thread(target=thead_start)
    t.setDaemon = True
    t.start()
    udp_loglib.gevent_join([])