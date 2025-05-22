#coding=utf-8
'''
自动修改cfgname 下 SS 开头的路径。原路径的目标文件需在当前路径或子文件夹
文件夹不要重名
'''
import io
import os,sys
import re
def cfgSS(cfgname):
    path = re.sub(r'\\','/',os.path.dirname(cfgname))
    #tmp = re.split(r'[/\\]',path)
    paths =[]
#    for i in range(len(tmp)):
#        paths.append('/'.join(tmp[:i+1])+'/')
    paths = [path+'/']
    with io.open(cfgname,'r',encoding ='utf-8') as f:
        lines = f.readlines()
    #lines =[r'SS C:\Users\LiuXintian\Desktop\pytools\tftp\tftp_err.log']
    for i in range(len(lines)):
        a = str(lines[i])
        pattern = r'^SS\s(.*)'
        res = re.search(pattern,a)
        if  res is not None :
            old_path = res.group(1)
            while len(old_path)>0:
                try:
                    old_path = re.split(r'[/\\]',old_path,1)[1]
                except:
                    break
                for tmp in paths:
                    if os.path.exists(tmp+old_path):
                        dst = tmp+old_path
                        break
            if  'dst' not in locals():
                raise Exception('path is not avaliable')
            lines[i] = u"SS %s\r\n"%dst
    with io.open(cfgname,'w',encoding ='utf-8') as f:
        for line in lines:
            if len(re.sub(r'[\s\r\n]','',line)) >0:
                f.write(line)
if __name__ == '__main__':
    cfgname = sys.argv[1]
    #cfgname = 'STEP_Restore_SW_FUSE_MSTEP_CPUA.cfg'
    cfgSS(cfgname)