# !/usr/bin/env python
# coding=utf-8
from __future__ import print_function
import sys, os
import subprocess
import time

if sys.version_info > (3, 0):
    raw_input = input

from datetime import datetime
import traceback
#region
import re
#CONFIG_START
slot_ip = {2: '20.2.1.11', 16: '20.1.1.11'}
#CONFIG_END
lru_ban = []
default_ip = ''

def gettime():
    '''fmt:%Y-%m-%d %H:%M:%S'''
    return datetime.now().strftime('%Y%m%d%H%M%S')
def isIPreachable(ip):
    '''通过ping命令判断ip是否可以通信'''
    p = subprocess.Popen(['ping', '-n', '1', ip], stdout=subprocess.PIPE)
    out = str(p.stdout.read())
    if 'TTL=' in out:
        return True
    else:
        return False
def inputsplit(slot, rack=1):
    global lru_ban
    slot = str(slot)
    slot = re.split(r'[,，.]', slot)
    moduleID_3 = []
    for s in slot:
        if len(s) == 0:
            continue
        s = s.split('-')
        s.append(s[0])
        moduleID_3 += range(int(s[0]), int(s[1]) + 1)
    for i in range(len(moduleID_3)):
        if moduleID_3[i] < 17:
            moduleID_3[i] += (int(rack) - 1) * 16
        if moduleID_3[i] in lru_ban:
            raise Exception('slot input mistake')
            moduleID_3[i] = 0
    return moduleID_3

def get_ip(slot=3):
    def get(todo=slot_ip):
        get_rt = []
        for i in slot_ip.values():
            if isIPreachable(i):
                get_rt.append(i)
        if len(get_rt) != 0:
            return get_rt
        else:
            raise ValueError
    if slot not in slot_ip:
        try:
            ip = get()
        except ValueError:
            raise ValueError('no ip is reachable')
    else:
        ip = slot_ip[slot]
        if not isIPreachable(ip):
            raise ValueError('%s is not reachable' % slot_ip.get(slot, 'null'))
        else:
            ip = [ip, ]
    return ip

def lruget(arg, sp={2: '1_2P', 16: '1_16S'}):
    arg = int(arg)
    if arg in lru_ban:
        raise Exception()
    x = int((arg - 1) / 16) + 1
    y = arg - 16 * (x - 1)
    return sp.get(arg, '%s_%s' % (str(int(x)), str(int(y))))
#endregion

def get_version(module_3, savepath=None, retry=2):
    if not isinstance(module_3, list):
        module_3 = inputsplit(module_3)  
    #module_3 = VCUs_on(module_3)
    default_ip = get_ip()[0]
    for i in slot_ip.keys():
        if i in module_3:
            get_ip(i)
    sub_p = {}
    output_dict = {}
    if savepath is None:
        savepath = os.path.abspath( os.path.dirname(__file__))
    else:
        if not os.path.isdir(savepath):
            os.mkdir(savepath)
        if not os.path.isdir(savepath + '/log/%s' % default_ip):
            os.makedirs(os.path.abspath(savepath) + '/log/%s' % default_ip)
        savepath = savepath

    cmd = 'java -jar %s/rut-2.0-SNAPSHOT.jar  %s %s . VCU_CW_CPU_A_ROM_GUEST_NOETH.S19 VCU_GCC_CPU_B_ROM_GUEST_NOETH.S19 5 0'
    for i in range(retry + 1):
        if len(module_3) == 0:
            break

        for i in module_3:
            ip = default_ip
            p = subprocess.Popen(cmd % (os.path.abspath( os.path.dirname(__file__) ), lruget(i), ip), shell = False, cwd = savepath, 
                                 stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            sub_p[i] = p 

        module_3 = []
        time.sleep(3)
        for k, p in sub_p.items():
            if p.poll() is None:
                module_3.append(k)
            else:
                if k not in output_dict.keys():
                    output_dict[k] = p.stdout.read().decode()
                    flag = re.search(r'Success', output_dict[k])
                    if flag is None:
                        output_dict.pop(k)
                        module_3.append(k)

    ver_dict = {}
    for k, p in sub_p.items():
        if k not in module_3:
            output = output_dict.get(k, p.stdout.read().decode())
            ver = re.search(r'soft_version:\s*V([\d\.]+)', output)
            ver_dict[k] = ver.group(1).replace('.', '')
            ver_dict[k] = int(ver_dict[k])
        else:
            ver_dict[k] = -1

    return ver_dict

code_dict = {'success': 0, 'running': 1, 'fail': 2, 'timeout': 3}
def update_check(sub_p, act, savepath, get_ip_func):
    fail_list = []
    running_list = []
    success_list = []
    for key, p in sub_p.items():
        if p.poll() is not None:
            cur_path = os.path.join(savepath, 'log', 
                                    get_ip_func(key)) #'IMAGE_LRU_%s.txt' % lruget(key)
            mtime_dict = {}
            for filename in os.listdir(cur_path):
                res = re.search(r'^IMAGE_LRU_%s.txt' % lruget(key), filename)
                if res is not None:
                    mtime = os.path.getmtime(os.path.join(cur_path, filename))
                    mtime_dict[mtime] = filename
            
            lastest = max(list(mtime_dict.keys()))
            assert(time.time() - lastest < 100)
            cur_name = os.path.join(cur_path, mtime_dict[lastest] )

            dst_rename = '%s_IMAGE_LRU_%s.txt' % (gettime(), lruget(key))
            dst_rename = os.path.join(cur_path, dst_rename )
            with open(cur_name, 'r') as f:
                log_content = f.read()
            if act == 0 or act == 6:
                pattren = ('Restore REPROG safety flag on CPU A.+Restore REPROG safety flag on CPU B' +
                           '.+Get command status for CPU A.+Get command status for CPU B.+Success')
            elif act == 1:
                pattren = r'Result[\s>》]+Success'
            elif act == 4:
                pattren = 'reboot to CPU.+reboot success'
            elif act == 5:
                pattren = 'Success'
            elif act == 'cfg':
                cur_name = os.path.join(cur_path, 'CONFIG_LRU_%s.txt' % lruget(key))
                dst_rename = '%s_CONFIG_LRU_%s.txt' % (gettime(), lruget(key))
                dst_rename = os.path.join(cur_path, dst_rename )
                pattren = 'Result          >>>   Success'                
            else:
                raise Exception(act)
            if re.search(pattren, log_content, re.S | re.DOTALL) is None or p.returncode != 0:
                state = False
            else:
                state = True
                      
            if state is False:
                fail_list.append(key)
                try:
                    os.rename(cur_name, dst_rename)
                except PermissionError:
                    os.system('@copy "%s" "%s"' % (cur_name, dst_rename) )
                    try:
                        os.unlink(cur_name)
                    except Exception:
                        pass
            else:
                success_list.append(key)
        else:
            running_list.append(key)
            
    if len(running_list) > 0:
        return code_dict['running'], running_list
    elif len(fail_list) > 0:
        return code_dict['fail'], fail_list
    else:
        return code_dict['success'], success_list

update_default_path = os.path.dirname(__file__)
def set_update_default_path(path):
    global update_default_path
    print('set update_default_path from %s to %s' % (update_default_path, path))
    update_default_path = path
    
def update(fileType, act, module_3, dirname = '', block = True, retry = 3, savepath=None):
    '''
    Descrption: 调用RUT工具
    args:
     fileType:str 镜像的类型如 S19,bin
     act:int|str  {'0': 'image update', '5': 'get version', "4": 'restart'}
     module_3:str|list '3-8,16' or [2,3,4,5,7]
     dirname:str 镜像所在文件的路径
     block:bool  是否等在更新完成
     retry:int   当阻塞时， 失败重传最大次数 为0 则不重传且抛出异常， 为负数则不重传并不抛出异常
    '''
    if len(dirname) == 0:
        dirname = update_default_path
    if not isinstance(module_3, list):
        module_3 = inputsplit(module_3)  
    #module_3 = VCUs_on(module_3)
    default_ip = get_ip()[0]
    for i in slot_ip.keys():
        if i in module_3:
            1#get_ip(i)

    print(fileType)
    if fileType == 'auto' and int(act) in [0, 1, 6]:
        type_list = ['lzw', 'bin', 'S19']
        type_dict = {}
        name_dict = {}
        for f in os.listdir(os.path.join(dirname, 'CPU_A')):
            file_path = f
            fname, t = os.path.splitext(file_path)
            type_dict[t[1:]] = file_path
            mtime = os.path.getmtime(os.path.join(dirname, 'CPU_A', f))
            name_dict.setdefault(fname, []).append(int(mtime))
            #ctime = os.path.getctime(os.path.join(dirname, 'CPU_B', f))
            #name_dict.setdefault(fname, []).append(ctime)

        for name, mtimes in name_dict.items():
            _time = min(mtimes)
            for m in mtimes:
                if (m - _time) > 30:
                    raise Exception('%s %s version may not eq' % (name, m - _time))
        for t in type_list:
            if t in type_dict.keys():
                fileType = t
                if t == 'lzw':
                    ver_dict = get_version(module_3)
                    for k, ver in ver_dict.items():
                        if (ver < 115 and k in [2.16]) or ( ver < 114 and k not in [2.16]):
                            fileType = 'S19'
                            break
                break
        else:
            raise Exception('not have image')
        print('file type %s' % fileType)
    
    if int(act) not in [0, 1, 6] and fileType == 'auto':
        fileType = 'S19'
    if int(act) in [0, 1, 6]:
        assert(fileType in ['lzw', 'bin', 'S19'])
    sub_p = {}

    if savepath is None:
        savepath = os.path.abspath( os.path.dirname(__file__))
    else:
        if not os.path.isdir(savepath):
            os.mkdir(savepath)
        if not os.path.isdir(savepath + '/log/%s' % default_ip):
            os.makedirs(os.path.abspath(savepath) + '/log/%s' % default_ip)
        savepath = savepath

    for i in module_3:
        arg = i
        #fileType = fileType_dict.get(arg, _fileType)
        ip = slot_ip.get(arg, default_ip)
        cmd = 'cd %s\n' % os.path.abspath(os.path.dirname(__file__))
        cmd += 'title %s\n' % lruget(arg)
        java_cmd1 = 'java -jar %s/rut-2.0-SNAPSHOT.jar %s %s %s ' % (os.path.abspath(os.path.dirname(__file__)), lruget(arg), ip, dirname)
        cmd += java_cmd1
       
        #temp = 'VCU_CW_CPU_A_ROM_GUEST_%s.%s VCU_GCC_CPU_B_ROM_GUEST_%s.%s %s 0\r\npause\r\nexit'
        if arg in [2, 16]:
            image_nameA = 'VCU_CW_CPU_A_ROM_GUEST_%s.%s' % ('INTA', fileType)
            image_nameB = 'VCU_GCC_CPU_B_ROM_GUEST_%s.%s' % ('INTA', fileType)
        else:
            image_nameA = 'VCU_CW_CPU_A_ROM_GUEST_%s.%s' % ('NOETH', fileType)
            image_nameB = 'VCU_GCC_CPU_B_ROM_GUEST_%s.%s' % ('NOETH', fileType)
        java_cmd2 = image_nameA + ' ' + image_nameB + ' %s 0' % act
        cmd += '%s\npause\nexit' % java_cmd2
        if block is False:
            fname = '%s/update/temp_update%s.bat' % (os.path.abspath(os.path.dirname(__file__)), lruget(arg))
            f = open(fname, 'w')
            f.write(cmd)
            f.close()
            os.system('start /high %s' % fname)
        else:
            java_cmd = (java_cmd1 + java_cmd2)#.join(' ')
            #stdout = subprocess.PIPE, stderr= subprocess.STDOUT
            sub_p[arg] = subprocess.Popen(java_cmd, shell = False, cwd = savepath, creationflags = subprocess.CREATE_NEW_CONSOLE)
            print(java_cmd)
        if arg in [2, 16]:
            time.sleep(2) # 避免同时更新mvcu 
        else:
            time.sleep(0.5)

    start_time = time.time()

    if block:
        while True:
            rt_code, rt_list = update_check(sub_p, act, savepath, lambda key: slot_ip.get(key, default_ip))
            if rt_code != code_dict['running']:
                break
            else:
                time.sleep(2)
            if time.time() - start_time > float(60 * 30):
                rt_code = code_dict['timeout']
                break

        while rt_code == code_dict['fail'] and retry > 0:
            retry -= 1
            rt_code, rt_list = update(fileType, act, rt_list, dirname, block, retry = -10) # 没有采用递归是为了检查超时 禁止retty
            if rt_code == code_dict['success']:
                break
            if time.time() - start_time > float(60 * 45):
                rt_code = code_dict['timeout'] 
                break
        if retry >= 0: # 当retry < 1 不会抛出异常
            if rt_code != code_dict['success']:
                raise Exception(rt_code, rt_list)

        return rt_code, rt_list
    else:
        return sub_p

def update_cfg(dst, cfgname, savepath = None, retry = 2):
    assert(retry >= 0)
    if not isinstance(dst, list):
        dst = inputsplit(dst)
    assert(len(dst) > 0)
    __ = [slot for slot in dst if slot not in slot_ip.keys()]
    if len(__) > 0:
        raise Exception(__)
    cmd = 'java -jar %s/rut-2.0-SNAPSHOT.jar  config ' % os.path.abspath(os.path.dirname(__file__))
    if len(dst) == 2:
        assert(dst[0] != dst[1])

        if isIPreachable(slot_ip[dst[0]]):
            key = dst[0]
            cmd += '%s %s %s/input/%s 00000001 ' % (lruget(dst[0]), slot_ip[dst[0]], os.path.abspath(os.path.dirname(__file__)), cfgname)
            cmd += '%s 0 0' % (lruget(dst[1]))
            
        elif isIPreachable(slot_ip[dst[1]]):
            key = dst[1]
            cmd += '%s %s %s/input/%s 00000001 ' % (lruget(dst[1]), slot_ip[dst[1]], os.path.abspath(os.path.dirname(__file__)), cfgname)
            cmd += '%s 0 0' % (lruget(dst[0]))
        else:
            raise Exception(dst)
    else:
        if isIPreachable(slot_ip[dst[0]]):
            key = dst[0]
            cmd += '%s %s %s/input/%s 00000001 ' % (lruget(dst[0]), slot_ip[dst[0]], os.path.abspath(os.path.dirname(__file__)), cfgname)
            cmd += '0 0 0'
        else:
            raise Exception(dst)

    if savepath is None:
        savepath = os.path.abspath( os.path.dirname(__file__))
    else:
        if not os.path.isdir(savepath):
            os.mkdir(savepath)
        if not os.path.isdir(savepath + '/log/%s' % slot_ip[key]):
            os.makedirs(os.path.abspath(savepath) + '/log/%s' % slot_ip[key])
        savepath = savepath
    sub_p = {}
    sub_p[key] = subprocess.Popen(cmd, shell = False, cwd = savepath, creationflags = subprocess.CREATE_NEW_CONSOLE)
    sub_p[key].wait()

    if sub_p[key].returncode != 0:
        update_cfg(dst, cfgname, retry - 1)

def update_dump(module_3, mode = 'AB', savepath = os.path.dirname(__file__) + '/dump'):
    global default_ip
    if not isinstance(module_3, list):
        module_3 = inputsplit(module_3)  
    #module_3 = VCUs_on(module_3)
    default_ip = get_ip()[0]
    for i in slot_ip.keys():
        if i in module_3:
            get_ip(i)
        
    sub_p = {}
    for i in module_3:
        arg = i
        ip = slot_ip.get(arg, default_ip)
        java_cmd = 'java -jar %s/RTD-1.0-SNAPSHOT.jar %s %s ' % (os.path.abspath(os.path.dirname(__file__) + '/dump'), lruget(arg), ip)
        if 'A' in mode:
            cmd = java_cmd + 'A'
            key = str(arg) + '_A' 
            sub_p[key] = subprocess.Popen(cmd, shell = False, cwd = savepath, creationflags =subprocess.CREATE_NEW_CONSOLE)
        if 'B' in mode:
            cmd = java_cmd + 'B'
            key = str(arg) + '_B'
            sub_p[key] = subprocess.Popen(cmd, shell = False, cwd = savepath, creationflags =subprocess.CREATE_NEW_CONSOLE)

    for key, value in sub_p.items():
        value.wait()

    fail_list = []
    for key, value in sub_p.items():
        if value.returncode != 0:
            fail_list.append( (key, value.returncode) )
    if len(fail_list) > 0:
        raise Exception(fail_list)
        
if __name__ == '__main__':
    #update('auto', 0, '3', '', block = False)
    if sys.argv[1] == 'image':
        slot = ''
        argv = sys.argv[2:]
        dirname = argv[2]
        act = argv[1]
        fileType = str(argv[0]).replace(' ', '')
        if len(argv) > 3:
            slot = argv[3]

        if len(slot) == 0:
            slot = raw_input('please input')

        def run_func():
            update(fileType, act, slot, dirname, block = False)
    elif sys.argv[1] == 'cfg':
        cfgname = sys.argv[2]
        if len(sys.argv) > 3:
            dst = sys.argv[3]
        else:
            dst = raw_input('please input')

        def run_func():
            update_cfg(dst, cfgname)

    elif sys.argv[1] == 'dump':
        mode = sys.argv[2]
        if len(sys.argv) > 3:
            module_3 = sys.argv[3]
        else:
            module_3 = raw_input('please input')

        def run_func():
            update_dump(module_3, mode)

    else:
        raise Exception(sys.argv)
    try:
        run_func()
    except Exception:
        traceback.print_exc()
        print('except end')
        while True:
            pass
    # os.remove('temp_update.bat')
