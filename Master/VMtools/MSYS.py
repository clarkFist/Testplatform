import subprocess 
import os,time
from datetime import datetime
def listdir(path):
    paths = list()
    for f in os.listdir(path):
        if os.path.isdir(f):
            paths =paths + listdir('%s/%s'%(path,f))
        else:
            paths.append(os.path.getctime('%s/%s'%(path,f)))
    return paths
p = subprocess.Popen('python %s/msyslog.py'%os.path.dirname(__file__),shell = False, creationflags =subprocess.CREATE_NEW_CONSOLE)
p.wait()
with open('temp.log','r') as f:
    r = f.read()
    print(r)
os.remove('temp.log')
