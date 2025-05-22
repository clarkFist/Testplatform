cd /d "%~dp0"
set target=E:\testplatform\py_lib\vmtcp.py
python %target% server
@IF %ERRORLEVEL% NEQ 50 (goto end)

@echo off
if exist "%SystemRoot%\SysWOW64" path %path%;%windir%\SysNative;%SystemRoot%\SysWOW64;%~dp0
bcdedit >nul
if '%errorlevel%' NEQ '0' (goto UACPrompt) else (goto UACAdmin)
:UACPrompt
%1 start "" mshta vbscript:createobject("shell.application").shellexecute("""%~0""","::",,"runas",1)(window.close)&exit
exit /B
:UACAdmin
cd /d "%~dp0"
echo 当前运行路径是：%CD%
echo 已获取管理员权限
set eth_name=本地连接
netsh interface IP add address name="%eth_name%" addr=20.3.1.50 mask=255.255.0.0
:end
pause