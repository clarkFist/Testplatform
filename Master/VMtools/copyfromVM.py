from __future__ import print_function
import os, re, sys
from inspect import getframeinfo, stack
import shutil
def m_system(cmd):
    caller = getframeinfo(stack()[1][0])
    pos = caller.filename.split('\\')[-1]
    msg = "%s %s" % (pos, caller.lineno)
    print('%s ' % msg, end='')
    print(cmd)
    os.system(cmd)

def copy(dirname, obj):
    global mode
    global archive_dict
    global to_archive
    path = dirname + '\\' + obj
    path = path.replace('/','\\')
    str_dict = {'svcu': 'NOETH', 'mvcu': 'INTA', 'svcu_lib': 'NOETH', 'mvcu_lib': 'INTA'}
    dst = '../VCU_s19/'
    try:
        os.makedirs(dst + 'CPU_A/')
        os.makedirs(dst + 'CPU_B/')
    except:
        pass
    if 'CPU_A' in obj:
        dst += 'CPU_A/VCU_CW_CPU_A_ROM_GUEST_%s' % str_dict[mode]
    elif 'CPU_B' in obj:
        dst += 'CPU_B/VCU_GCC_CPU_B_ROM_GUEST_%s' % str_dict[mode]
    else:
        dst = ''
    if len(dst) != 0:
        if '.S19' in path:
            dst += '.S19'
        elif '.elf' in path:
            dst += '.elf'
        elif '.bin' in path:
            dst += '.bin'
        elif '.rbin' in path:
            dst += '.bin'
        elif '.lzw' in path:
            dst += '.lzw'
        elif 'software_data_identification' in path:
            with open(path, 'r') as f:
                res = re.search(r'for (.*): (.*)$', f.read())
                archive_dict[res.group(1)] = res.group(2)
            dst += '.txt'
        else:
            dst = ''
        if len(dst) != 0:
            m_system('COPY "%s" "%s"' % (path, dst))
    to_archive.append((path, obj))
def archive():
    global archive_dict
    global mode
    global to_archive
    import time

    dst = '..\\VCU_s19\%s' % (mode + archive_dict['CPU_A'] + '_' + archive_dict['CPU_B'])
    delete_time = time.time() - 60*60*24 * 15
    for dirname in os.listdir('..\\VCU_s19'):
        if dirname == 'CPU_A' or dirname == 'CPU_B':
            continue
        dirname = os.path.join('..\\VCU_s19', dirname)
        if os.path.isdir(dirname):
            if os.path.getctime(dirname ) < delete_time:
                print('delete %s' % dirname)
                shutil.rmtree(dirname)
    if os.path.exists(dst):
        print("already archive")
        return 
    os.makedirs(dst + '/CPU_A/')
    os.makedirs(dst + '/CPU_B/')
    print(dst + '/CPU_A/')
    print(dst + '/CPU_B/')
    for obj in to_archive:
        print(obj[0])
        if os.path.isdir(obj[0]):
            m_system('xcopy %s\\*.* %s\\%s\\ /s' % (obj[0].replace('/','\\'), dst, obj[1]))
        elif os.path.isfile(obj[0]):
            if 'CPU_A' in obj[1]:
                subdir = 'CPU_A\\'
            elif 'CPU_B' in obj[1]:
                subdir = 'CPU_B\\'
            else:
                subdir = ''
            m_system('COPY "%s" "%s\\%s%s"' % (obj[0], dst, subdir,obj[1]))
        else:
            print('err')
def resovleArgv(argv):
    if argv[0].lower() == 'svcu':
        return 'svcu'
    elif argv[0].lower() == 'mvcu':
        return 'mvcu'
    if argv[0].lower() == 'svcu_lib':
        return 'svcu_lib'
    elif argv[0].lower() == 'mvcu_lib':
        return 'mvcu_lib'
    else:
        raise Exception(argv[0])
if __name__ == '__main__':
    print(os.getcwd())
    mode = resovleArgv(sys.argv[1:])
    archive_dict = {}
    to_archive = []
    tocopy_files = {'./build/out/CPU_A': [r'.*\.S19', r'.*\.elf$', r'.*\.txt',r'.*.\.rbin',r'.*\.lzw$'], './build/out/CPU_B': [r'.*\.S19', r'.*\.elf$', r'.*\.txt',r'.*\.bin',r'.*\.lzw$'],
                    './src': ['VOOB', 'tst', 'COM'], './guest/demo/src': ['.*']}
    for dirname, mfilters in tocopy_files.items():
        if not os.path.exists(dirname):
            continue
        for obj in os.listdir(dirname):
            for mfilter in mfilters:
                res = re.search(mfilter, obj)
                if res is not None:
                    copy(dirname, obj)
    archive()
    exit(0)