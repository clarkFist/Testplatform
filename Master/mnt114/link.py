#coding=utf-8
from __future__ import print_function 
import os,sys
import tempfile
import traceback 
import io,re 
def cfgSS(cfgname):
    path = os.path.dirname(cfgname)
    temp = os.path.abspath(path) +'/build/out'
    if not os.path.exists(temp):
        os.system('python cfgSS.py %s'%os.path.abspath(cfgname))
        return
    with io.open(cfgname,'r',encoding ='utf-8') as f:
        lines = f.readlines()
    for i in range(len(lines)):
        a = str(lines[i])
        pattern = r'^SS\s(.*)'
        res = re.search(pattern,a)
        if  res is not None :
            if 'conf_A' in cfgname[-20:]:
                dst = temp+'/CPU_A/' + re.split(r'[/\\]',res.group(1))[-1]
            elif 'conf_B' in cfgname[-20:]:
                dst = temp+'/CPU_B/' + re.split(r'[/\\]',res.group(1))[-1]
            else:
                print('%s may not build,please check %s is ok'%(path,cfgname))
                return
            lines[i] = u"SS %s\r\n"%dst
            break
    with io.open(cfgname,'w',encoding ='utf-8') as f:
        for line in lines:
            if len(re.sub(r'[\s\r\n]','',line)) >0:
                f.write(line)
def lnk(path):
    global tmpname
    path = os.path.abspath(path)
    cmd = 'cscript -nologo -e:vbscript "%s" "%s" "%s"'%(tmpname,os.path.abspath(path+'/MSYS.lnk'),os.path.abspath(path))
    os.system(cmd)
    with open('%s/input.txt'%path,'w') as f:
        res = re.search(r'([C-Z]):',path)
        t = res.group(1)
        path = re.sub(r'%s:'%t,'/%s'%t,path)
        path = re.sub(r'\\','/',path)
        f.write('cd %s/build/\r\nmake.sh'%path)
end =[]
lnkcmd = '''Set fso=CreateObject("Scripting.FileSystemObject")
    Set ws=CreateObject("WScript.Shell")
    Set lnkfile=fso.GetFile(WSH.Arguments(0))
    Set lnkobj=ws.CreateShortcut(lnkfile.Path)
    lnkobj.WorkingDirectory = WSH.Arguments(1)
    lnkobj.TargetPath = "%s"
    lnkobj.Save'''%os.path.abspath(os.path.dirname(__file__)+'\MSYS.bat')
paths = ['VCU_DEV_MVCU','VCU_DEV','sVCU_DEV_GUEST','mVCU_DEV_GUEST','Restore_fuse_new','Flash_Es']
for i,p in enumerate(paths[:]):
    if not os.path.isdir(os.path.dirname(__file__)+'/'+p):
        print(p+' is not exist')
        paths.pop(paths.index(p))
#paths = ['VCU_DEV_TEST']
print(os.path.abspath('.')) 
tmpfd,tmpname = tempfile.mkstemp()
os.close(tmpfd)
end.append((lambda _:os.unlink(_),(tmpname,)))
if __name__=='__main__':
    with open(tmpname,'w') as tmpfd:
        tmpfd.write(lnkcmd)
    try:
        for path in paths:
            dir = os.listdir(path)
            print(path+' start')
            if 'build' in dir and 'MSYS.lnk' in dir:
                lnk(path)
            for file in dir:
                if '.cfg' in file:
                    cfgSS(path+'/'+file)

    except Exception as e:
        traceback.print_exc(e)
    for todo in end:
        todo[0](*todo[1])