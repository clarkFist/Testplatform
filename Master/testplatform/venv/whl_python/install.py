import sys,os
import re
import shutil
import subprocess
def install(dir_name, pip):
    files = os.listdir(dir_name)
    files.sort()
    print(files)
    temp = []
    for f in files:
        if f.endswith('.whl') or f.endswith('.gz'):
            newname = None
            res =  re.search(r"^\d+(.*$)",f)
            f = os.path.abspath(dir_name) + '/%s' % f
            if res is not None:
                newname = os.path.abspath(dir_name) + '/%s' % res.group(1)
                shutil.copy(f,newname)
                f = newname
            cmd = r"%s install %s"%(pip, f)
            rt_code = subprocess.call(cmd)
            if rt_code != 0:
                raise Exception(cmd)
            temp.append(f)
            if newname != None:
                os.remove(newname)
if __name__ == '__main__':
    dirname = sys.argv[1]
    if len(sys.argv ) > 1:
        pip = sys.argv[2]
    else:
        pip = 'pip'
    install(dirname, pip)