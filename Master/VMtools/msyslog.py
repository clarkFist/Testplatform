#coding=utf-8    
import os,sys    
import io,re,time
import subprocess
import traceback
from datetime import datetime
from colorama import Fore,Back,Style,init
import ctypes
init(autoreset=True)
is_print = False
def removeCCppComment( text ):

    def blotOutNonNewlines( strIn ):  # Return a string containing only the newline chars contained in strIn
        return "" + ("\n" * strIn.count('\n'))

    def replacer( match ):
        s = match.group(0)
        if s.startswith('/'):  # Matched string is //...EOL or /*...*/  ==> Blot out all non-newline chars
            return blotOutNonNewlines(s)
        else:                  # Matched string is '...' or "..."  ==> Keep unchanged
            return s

    pattern = re.compile(
        r'//.*?$|/\*.*?\*/|\'(?:\\.|[^\\\'])*\'|"(?:\\.|[^\\"])*"',
        re.DOTALL | re.MULTILINE
    )

    return re.sub(pattern, replacer, text)
def gettime():
    '''fmt:%Y-%m-%d %H:%M:%S'''
    return datetime.now().strftime('%Y-%m-%d %H:%M:%S')
def print_f(arg):
    global is_print
    if is_print:
        print(arg)
class Msys_print:
    print_temp_max = 15
    print_temp=False
    str_list =[]

    msg = {'err_times':0,'warning_times':0,'print_lines':0}
    err = ''
    err_mode = 0
    toprint = set()
    
    @classmethod
    def init(cls):
        for k,v in cls.msg.items():
            cls.msg[k] = 0
        cls.toprint = set()
        cls.print_temp = False
        cls.str_list = []
    @classmethod
    def err1_s(cls):
        rt = []
        temp = cls.err
        temp = re.sub(r'\\[rn]','',temp)
        temp = re.sub(r'\s+',' ',temp)
        print_f(temp)
        res = re.search(r'[\\/](\w+\.\w)\|(\d+)\|[^\)]*\)\s*=(.*)',temp)
        if res is None:
            raise Exception(temp)
        for i in range(1,4):
            if res.group(i) is not None:
                rt.append(res.group(i))
            else:
                raise Exception(temp)
        cls.err =''
        return rt
    @classmethod
    def err_print(cls,in_str):
        msg =[]
        if cls.err_mode == 1:
            if '>' not in in_str:
                cls.err += in_str
                if len(cls.err) > 1000:
                    raise Exception(cls.err)
            else:
                cls.err += in_str
                cls.print_temp = False
                msg = cls.err1_s()
        elif cls.err_mode == 2:
            cls.print_temp = False
            res = re.search(r'/(\w+\.\w):(\d+):[^:]+:(.*)',in_str)
            for i in range(1,4):
                if res.group(i) is not None:
                    msg.append(res.group(i))
                else:
                    raise Exception()
        if cls.print_temp == False:
            cls.toprint.add(':'.join(msg))
            print_f(str(Fore.RED+Back.RESET)+':'.join(msg)+str(Fore.RESET+Back.RESET))
    @classmethod
    def judge(cls,in_str):
        in_str = in_str.lower()
        rt = 0
        if '|error' in in_str or ': error:' in in_str:
            rt = 2
        elif '|warning' in in_str or ': warning:' in in_str:
            rt = 1
        return rt
    @classmethod
    def default(cls,in_str):
        if len(in_str)<3:
            return ''
        #in_str = re.sub('|Error',' |Error ',in_str,re.I)
        #in_str = re.sub('|Warning',' |Warning ',in_str,re.I) 
        if len(cls.str_list)>cls.print_temp_max:
            cls.str_list.pop(0)
        if cls.msg['print_lines']%90 == 0:
            print_f(str(Fore.GREEN+Back.RESET)+'err_times: %d warning_times %d print_lines %d '%(cls.msg['err_times'],cls.msg['warning_times'],cls.msg['print_lines'])+str(Fore.RESET+Back.RESET))
        #in_str = cls.str_color(in_str)
        jd = cls.judge(in_str)
        if jd == 2:
            cls.msg['err_times'] += 1
            cls.print_temp = True
            res = re.search(r'^[^\s]*\.\w:\d+:\d+: error:.*',in_str)
            if res is None:
                res = re.search('|Error',in_str)
                if res is not None:
                    cls.err_mode = 1
                else:
                    raise Exception('judge() or res err')
            else:
                cls.err_mode = 2
        elif jd ==1:
            cls.msg['warning_times'] += 1
        if cls.print_temp == True:
            cls.err_print(in_str)
            
        cls.str_list.append(in_str)
        in_str = in_str.replace('Warning',' |Warning ')
        in_str = in_str.replace('Error',' |Error ')
        cls.msg['print_lines'] += 1
        cls.output.write(in_str)
    @classmethod
    def str_color(cls,in_str):
        #in_str = re.sub(r'Error',Fore.RED+Back.RESET+'Error'+Fore.RESET+Back.RESET,in_str,re.I)
        #in_str = re.sub(r'Warning',Fore.GREEN+Back.RESET+'Warning'+Fore.RESET+Back.RESET,in_str,re.I)  
        jd = cls.judge(in_str)
        if jd == 2:
            in_str = str(Fore.RED+Back.RESET)+in_str+str(Fore.RESET+Back.RESET)
        elif jd ==1:
            in_str = str(Fore.YELLOW+Back.RESET)+in_str+str(Fore.RESET+Back.RESET)
        return in_str

def gettime():
    '''fmt:%Y-%m-%d %H:%M:%S'''
    return datetime.now().strftime('%Y-%m-%d %H:%M:%S')

def stubCheck2(name,rithtValue,flag):
    liftValue = re.search(r'_(\d{4})0(\d{2})',name)
    if liftValue == None:
        liftValue = re.search(r'_(\d{4})_(\d+)',name)
        if liftValue == None:
            flag.append((name,'format Err'))
            return
    value1 = liftValue.group(1)
    value2 = liftValue.group(2)
    liftValue = int(value1 + '00',16) + int(value2,16)
    rithtValue = rithtValue&0x00FFFFFF
    if rithtValue != liftValue:
        flag.append((name,'value Err'))
def stubCheck(cmdpath):
    #path = r'C:\Users\user\Desktop\VCU114\SVCU\VCU_CW_CPUA\tst/tst_vcu_stubs.h'
    path = '%s/src/tst/tst_vcu_stubs.h'%os.path.abspath(cmdpath)
    if not os.path.exists(path):
        path = '%s/guest/demo/src/tst/tst_vcu_stubs.h'%os.path.abspath(cmdpath)
        if not os.path.exists(path):
            return True
    with open(path ,'r') as f:
        content = f.read()
    content = removeCCppComment(content)
    content = re.sub(r'[\n\s\r]','',content)
    content = re.search(r'typedefenum{([^}]*)}TAG_NAME;',content).group(1)

    stubs = content.split(',')
    temp_list = []
    stubCheck2_flag = []

    temp_set = set()
    for stub in stubs:
        if len(stub) == 0:
            continue
        temp = stub.split('=')
        if '0x' in temp[1]:
            rightValue = int(temp[1],16)
        else:
            print_f(' a value is not hex: %s'%stub)
            rightValue = int(temp[1])
        stubCheck2(temp[0],rightValue,stubCheck2_flag)
        rightValue = hex(rightValue)
        temp_list.append(rightValue)
        temp_set.add(rightValue)
    for temp in temp_set:
        temp_list.remove(temp)
    if len(temp_list) > 0 or len(stubCheck2_flag) > 0:
        execMsg = ''
        if len(temp_list) > 0:
            print_f("stub repeat:"+ ','.join(temp_list))
            execMsg += ' '.join(temp_list)
        if len(stubCheck2_flag) > 0:
            print("stub format err:",stubCheck2_flag)
            for name, flag in stubCheck2_flag:
                execMsg += ' %s %s ' % (name, flag)
        raise Exception(execMsg)
    return True

def build(cmdpath):
    if os.path.exists('%s/temp.log' % cmdpath):
        os.remove('%s/temp.log' % cmdpath)
    Msys_print.init()
    start = time.time()
    stubCheck(cmdpath)
    stdin = open('%s/input.txt'%cmdpath,'r')
    cd = os.path.abspath(cmdpath)
    os.system('title '+cd)
    print_f('start %s'%cd)
    p = subprocess.Popen(r'C:\dev_tools\MSYS-1.0.10\1.0\bin\sh --login -i',\
        shell=False,cwd = cmdpath, stdin = stdin,stdout = subprocess.PIPE,stderr = subprocess.STDOUT)#,creationflags =subprocess.CREATE_NEW_CONSOLE
    with io.open('output.msys','w',encoding='utf-8') as f:
        Msys_print.output = f
        while p.poll() is None:    
            in_str = p.stdout.readline()
            try:
                in_str = in_str.decode('gbk')
            except:
                in_str = in_str.decode('utf-8')
            Msys_print.default(in_str)        
    stdin.close()
    print_f('\r\n'.join(Msys_print.str_list))
    print_f('time:%d'%(int(time.time()-start)))
    print_f('current time:%s'%(gettime()))
    Msys_print.toprint = list(Msys_print.toprint)
    Msys_print.toprint.insert(0,'err %d warning %d time %d s '%(Msys_print.msg['err_times'],Msys_print.msg['warning_times'],int(time.time()-start)))
    f = open('%s/temp.log'% cmdpath,'w+')
    f.write('\r\n'.join(Msys_print.toprint))
    f.close()
    print(p.returncode)
    return p.returncode, '\r\n'.join(Msys_print.toprint)
if __name__ == "__main__":
    _rt = 0
    is_print = True
    try:
        stdin = open('input.txt','r')
        oldpath = stdin.read()
        oldpath = '/'.join( oldpath.split('/')[:-3] )
        oldpath = re.search(r'cd\s/*(\w):*/(.*)',oldpath)
        oldpath = oldpath.group(1) + ':/' + oldpath.group(2)
        if os.path.normcase(oldpath).lower() != os.path.normcase( os.path.abspath(os.path.dirname(__file__)) ).lower():
            raise Exception('not link,exec %s,%s'%( oldpath,os.path.abspath(os.path.dirname(__file__))))
        if os.path.exists('src'):
            path = 'src'
            if os.path.exists(path + '/msys.log'):
                with open(path + '/msys.log') as f:
                    print_f("maybe build already at %s, continue?"%f.read())
                a = raw_input()
        elif os.path.exists('guest'):
            path = 'guest'
        
        _rt, errmsg = build('.')
    except Exception as e:
        _rt = 3
        traceback.print_exc()
    ctypes.windll.user32.FlashWindow(
    ctypes.windll.kernel32.GetConsoleWindow(),True)
    if _rt == 0:
        with open(path + '/msys.log','w') as f:
            f.write(gettime())
    print_f(_rt)
    exit(_rt)