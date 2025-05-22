set dirname=%~dp0310
set pip=C:\Users\LiuXintian\Desktop\testplatform\venv\Scripts\pip3.exe
python3 install.py %dirname%\pyserial  %pip%
pause
python3 install.py %dirname%\flake8  %pip%
python3 install.py %dirname%\gevent  %pip%
python3 install.py %dirname%\pysnmp  %pip%
python3 install.py %dirname%\yapf  %pip%
pause