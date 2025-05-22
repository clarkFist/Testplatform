#coding=utf-8    
import os,sys    
import io,re,time
import subprocess
import traceback
from colorama import Fore,Back,Style,init
import ctypes
init(autoreset=True)
def print_f(arg):
    print(arg)
class Msys_print:
    print_temp_max = 15
    print_temp=False
    str_list =[]
    output = io.open('output.msys','w',encoding='utf-8')
    msg = {'err_times':0,'warning_times':0,'print_lines':0}
    err = ''
    err_mode = 0
    toprint =set()
    @classmethod
    def err1_s(cls):
        rt = []
        temp = cls.err
        temp = re.sub(r'\\[rn]','',temp)
        temp = re.sub(r'\s+',' ',temp)
        print(temp)
        res = re.search(r'[\\/](\w+\.\w)\|(\d+)\|[^\)]*\)\s*=(.*)',temp)
        for i in range(1,4):
            if res.group(i) is not None:
                rt.append(res.group(i))
            else:
                raise Exception()
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
    @classmethod
    def end(cls):
        cls.output.close()
        f = open('temp.log','w+')
        f.write('\r\n'.join(cls.toprint))
        f.close()
        #print('\r\n'.join(cls.toprint))
 
if __name__ == "__main__":
    try:
        start = time.time()
        stdin = open('input.txt','r')
        cd = os.path.abspath('.')
        os.system('title '+cd)
        print_f('start %s'%cd)
        #cd = cd.replace(':','')
        cd = cd.replace('\\','/')
        p = subprocess.Popen(r'C:\dev_tools\MSYS-1.0.10\1.0\bin\sh --login -i',\
            shell=False,stdin = stdin,stdout = subprocess.PIPE,stderr = subprocess.STDOUT)#,creationflags =subprocess.CREATE_NEW_CONSOLE
        #os.system("title "+cd)
        while p.poll() is None:    
            in_str = p.stdout.readline()
            in_str = in_str.decode('gbk')
            Msys_print.default(in_str)
        stdin.close()
        print_f('\r\n'.join(Msys_print.str_list))
        print_f('time:%d'%(int(time.time()-start)))
        Msys_print.toprint = list(Msys_print.toprint)
        Msys_print.toprint.insert(0,'err %d warning %d time %d s '%(Msys_print.msg['err_times'],Msys_print.msg['warning_times'],int(time.time()-start)))
    except Exception as e:
        traceback.print_exc()
    Msys_print.end()
    ctypes.windll.user32.FlashWindow(
    ctypes.windll.kernel32.GetConsoleWindow(),True)
    os.system('pause')