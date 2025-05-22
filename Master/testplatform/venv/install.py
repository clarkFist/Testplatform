import subprocess
import io
import os
import re
import whl_python.install as install
import shutil

py_version = '3.10'
p = subprocess.Popen('where pip%s' % py_version, stdout = subprocess.PIPE, stderr = subprocess.STDOUT, shell = False)
p.wait()
py_path = p.stdout.read().decode()
py_path = os.path.dirname(os.path.dirname(py_path))
file_dir = os.path.abspath(os.path.dirname(__file__))
print(py_path)
os.system('setx VCU_TEST "%s"' % os.path.dirname(file_dir))
encoding = 'utf8'
requirements = ['gevent', 'pysnmp', 'pyserial', 'pywin32', 'flake8', 'yapf']
to_deletes = ['include', 'Scripts', 'Lib/site-packages', 'pyvenv.cfg']
for to_d in to_deletes:
    to_d = '%s/%s' % (file_dir, to_d)
    if os.path.isfile(to_d):
        os.remove(to_d)
    elif os.path.exists(to_d):
        shutil.rmtree(to_d)

os.system('%s/python -m venv %s' % (py_path, file_dir))
dirname = '%s/whl_python/%s/' % (file_dir, py_version)
pip = '%s/Scripts/pip%s.exe' % (file_dir, py_version)
for require_lib in requirements:
    install.install('%s/%s' % (dirname, require_lib), pip)