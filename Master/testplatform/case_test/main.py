import os
import sys
import importlib.util
import importlib.machinery
import re
from datetime import datetime
import shutil

paths = os.path.abspath(__file__).split('\\')
form_path = '\\'.join(paths[:-2])
if form_path not in sys.path:
    sys.path.append( form_path )
import udp_loglib
from udp_loglib import Log, Tag, Conf, FrameLib, Public, VCU_VIV, VMTool
from udp_loglib import gevent, threading, time
from autopower import AutoPower
from simulator import Simulator
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
    caseFunc.power_restart()
    time.sleep(50)
    caseFunc.check_allboard(timeout=100)

    form._set_client(mode=active_mvcu, dst_svcu='')
    form.update('S19', 0, active_mvcu, dirname=normal_path)
    for sims in Simulator.ins_list:
        for i in range(1, 7):
            sims.close(i)
    gevent.sleep(40) # 等待mvcu 进入normal
    for i in range(20):
        a_mvcu = form.get_active_mvcu()
        if a_mvcu is None:
            gevent.sleep(10)
        else:
            break

    form.tst_set(active_mvcu, Conf.allFlagSet(1))  
    FrameLib.respondCheck() # 默认打开mvcu的log, 同时确保mvcu 处于normal模式
    #module_3s = form.mib.VCUs_all() # 获取目前配置所有的板卡槽道
    module_3s = udp_loglib.VCU.inputsplit(active_svcu)
    if len(module_3s) > 0:
        form.update('S19', 0, active_svcu, dirname=normal_path)
        gevent.sleep(50)
        for i in range(20):
            mode_svcu = form.get_funcMode(module_3s[0])
            if mode_svcu == 'VIV_ERROR':
                gevent.sleep(10)
            else:
                break
        to_check = module_3s
        flag_dict = {} # 上电完成允许一次异常状态
        start_time = time.time()
        while len(to_check) > 0:
            func_mode_dict = form.get_funcMode(to_check)
            for slot, funcmode in func_mode_dict.items():
                if funcmode == 'VIV_ERROR': # 无法获取
                    if time.time() - start_time > 230:
                        raise TimeoutError(slot, funcmode)
                    else:
                        continue
                elif funcmode != 'NORMAL': # 更新后仍是异常模式
                    if flag_dict.get(slot, False):
                        raise Exception(slot, funcmode)
                    else:
                        flag_dict[slot] = True

                else: # 正常模式
                    to_check.remove(slot)
            if len(to_check) > 0:
                gevent.sleep(10)


def pre_check(cases):
    global case_dict
    global active_mvcu
    global active_svcu
    mvcus = udp_loglib.VCU.inputsplit(active_mvcu)
    svcus = udp_loglib.VCU.inputsplit(active_svcu)

    mod_dict = {} # 动态加载moudle
    svcu_count = len( svcus )
    serial_count = {}  # 检查串口的配置 是否满足用例要求
    for v in ['mvcu', 'svcu']:
        serial_count[v] = {}
        for m in ['A', 'B']:
            serial_count[v][m] = 0
    serial_config = {}
    for m in ['A', 'B']:
        serial_config.setdefault(m, []) 

    for key, value in caseFunc.Config.serial_config().items():
        vcus = udp_loglib.VCU.inputsplit(value)
        serial_config[key] = vcus
        for v in vcus:
            if v == 2 or v == 16:
                m_s_vcu = 'mvcu'
            else:
                m_s_vcu = 'svcu'
            
            serial_count[m_s_vcu][key] += 1
        #exec('%s_serial_count_%s[0] += 1' % (m_s_vcu, value), locals()) 
    serial_config['AB'] = [ slot for slot in serial_config['A'] if slot in serial_config['B']] # 双通道同时配置了串口的板卡
    __ = [slot for slot in serial_config['AB'] if slot == 16 or slot == 2]
    serial_count['mvcu']['AB'] = len(__) # mvcu中双通道同时配置了串口的板卡
    __ = [slot for slot in serial_config['AB'] if slot != 16 and slot != 2]
    serial_count['svcu']['AB'] = len(__)
    _not_satified = []
    for case in cases:
        mod_name = os.path.splitext(case)[0] # 获取用例名称
        loader = importlib.machinery.SourceFileLoader(mod_name, '%s/%s' % (case_dict[case], case)) # 导入moudle
        spec = importlib.util.spec_from_loader(loader.name, loader)
        mod = importlib.util.module_from_spec(spec)
        loader.exec_module(mod)
        mod_dict[case] = mod

        if mod.requirement.slots is not None:
            re_slots = udp_loglib.VCU.inputsplit(mod.requirement.slots)
            __ = [slot for slot in re_slots if slot not in mvcus]
            __ = [slot for slot in __ if slot not in svcus]
            if len(__) > 0:
                _not_satified.append((case, 'slot', __))
                
        if mod.requirement.svcu_count is not None:
            if svcu_count < mod.requirement.svcu_count:
                _not_satified.append((case, 'count', svcu_count, mod.requirement.svcu_count)) 

        if mod.requirement.serial_requirement is not None:
            for v in ['mvcu', 'svcu']:
                for m in ['A', 'B', 'AB']:
                    if serial_count[v][m] < mod.requirement.serial_count[v][m]:
                        _not_satified.append((case, 'serial', v, m))

            for m in ['A', 'B', 'AB']:
                __ = [slot for slot in mod.requirement.serial_requirement[m] if slot not in serial_config[m]]
                if len(__) > 0:
                    _not_satified.append((case, 'serial', __))
    if len(_not_satified) > 0:
        raise Exception(_not_satified)
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
    temp = caseFunc.Config.get_suffix()
    
    if isinstance(temp, tuple) and len(temp) == 2:
        case_suffixs, ban_suffixs = temp
    else:
        case_suffixs = temp
        ban_suffixs = []

    temp = caseFunc.Config.get_prefix()
    if isinstance(temp, tuple) and len(temp) == 2:
        case_prefixs, ban_prefixs = temp
    else:
        case_prefixs = temp
        ban_prefixs = []

    cases = [] # 只保存用例名称
    
    for path in case_paths:
        _lists = os.listdir(path)
        for case in _lists:
            if os.path.isfile('%s/%s' % (path, case) ):
                cases.append(case) 

    for case in cases[:]:
        case_lower = case.lower()
        prefixs = case_lower.split('_')

        __ = [prefix for prefix in case_prefixs if prefix.lower() not in prefixs] # 是否有不满足条件的前缀
        if len(__) > 0:
            cases.remove(case)
            continue

        __ = [prefix for prefix in ban_prefixs if prefix.lower() in prefixs] # 是否有禁止的前缀
        if len(__) > 0:
            cases.remove(case)
            continue

        __ = [suffix for suffix in case_suffixs if case_lower.endswith(suffix.lower()) ] # 是否有满足的后缀
        if len(__) == 0:
            cases.remove(case)
            continue
        
        __ = [suffix for suffix in ban_suffixs if case_lower.endswith(suffix.lower()) ] # 是否有禁止的后缀
        if len(__) > 0:
            cases.remove(case)
            continue

    if (len(cases) != len(set_cases := set(cases))): # 避免重复的用例
        for k in set_cases:
            cases.remove(k)
        raise Exception(cases)

    case_dict = {} # 获取用例的完整路径
    for path in case_paths:
        _lists = os.listdir(path)
        for case in _lists:
            if os.path.isfile('%s/%s' % (path, case) ):
                if case in cases:
                    case_dict[case] = path

    form = udp_loglib.TestPlatForm(18125, 18125, set_client=False)
    form.Serial_Flag = True
    caseFunc.Config.set_form(form) # 指定用例的form
    Log._toRun_list = [
        (lambda _: caseFunc.log_queue_get(_.src_lru, _.src_cpu, use_Type='lru').put((_.info, _.recvVal)), ('_instance_',)), 
        (lambda _: _._store(), ('_instance_',))] # 自定义log 的torun
    #Log._toRun_list.append( (lambda _: caseFunc.log_queue_get(_.src_lru, _.src_cpu, use_Type='lru').put((_.info, _.recvVal)), ('_instance_',)) )
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

    def testcase_log(msg, show=True):
        '''记录log'''
        if show:
            Public.mPrint(msg)
        else:
            udp_loglib.funclog.wirte(msg)
        with open('%s/log/testcase.log' % os.path.dirname(__file__), 'a') as f:
            f.write(msg + '\r\n' )

    def main():
        global testcase_log
        Log.path_set(os.path.dirname(__file__) + '/Log/default')
        testcase_log('%s to run cases: [%s]' % (gettime(), ','.join(cases)), show=False)

        for case in cases:  
            testcase_log('start %s' % case)
            mod = mod_dict[case]
            dir_path = case_dict[case] + '/Log/%s' % (os.path.splitext(os.path.basename(case))[0])
            Public.mPrint('delete %s' % dir_path)
            if os.path.isdir(dir_path):
                shutil.rmtree(dir_path) # 执行用例前清空旧log
            mod.before_main()
            case_start(mod) # 执行用例前的预操作
            try:
                mod.main() # 执行用例的 main 方法
            except Exception as e:
                'execption'
                case_log = '%s execption at testcase %s/%s %s' % (gettime(), case_dict[case], case, e)
                testcase_log(case_log)
                Public.program_end(e) # 异常时直接结束
            else: 
                'success'
                case_log = '%s normal end, testcase %s/%s' % (gettime(), case_dict[case], case)
                testcase_log(case_log)

            case_switch()

        time.sleep(10)
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