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
set eth_name=blue
netsh interface IP add address name="%eth_name%" addr=20.3.1.1 mask=255.0.0.0
netsh interface IP add address name="%eth_name%" addr=20.2.1.1 mask=255.0.0.0
netsh interface IP add address name="%eth_name%" addr=20.1.1.1 mask=255.0.0.0
netsh interface IP add address name="%eth_name%" addr=20.5.1.1 mask=255.0.0.0
netsh interface IP add address name="%eth_name%" addr=20.3.1.100 mask=255.255.0.0
netsh interface IP add address name="%eth_name%" addr=20.5.1.100 mask=255.255.0.0
netsh interface IP add address name="%eth_name%" addr=20.2.1.100 mask=255.255.0.0
netsh interface IP add address name="%eth_name%" addr=20.2.1.101 mask=255.255.0.0
netsh interface IP add address name="%eth_name%" addr=20.1.1.100 mask=255.255.0.0
netsh interface IP add address name="%eth_name%" addr=10.6.3.102 mask=255.255.0.0
netsh interface IP add address name="%eth_name%" addr=10.7.4.102 mask=255.255.0.0
netsh interface IP add address name="%eth_name%" addr=20.3.1.101 mask=255.255.0.0
netsh interface IP add address name="%eth_name%" addr=20.5.1.101 mask=255.255.0.0
netsh interface IP add address name="%eth_name%" addr=20.3.1.102 mask=255.255.0.0
netsh interface IP add address name="%eth_name%" addr=20.5.1.102 mask=255.255.0.0
netsh interface IP add address name="%eth_name%" addr=20.3.1.103 mask=255.255.0.0
netsh interface IP add address name="%eth_name%" addr=20.5.1.103 mask=255.255.0.0
netsh interface IP add address name="%eth_name%" addr=20.3.1.104 mask=255.255.0.0
netsh interface IP add address name="%eth_name%" addr=20.5.1.104 mask=255.255.0.0
netsh interface IP add address name="%eth_name%" addr=20.3.1.105 mask=255.255.0.0
netsh interface IP add address name="%eth_name%" addr=20.5.1.105 mask=255.255.0.0
netsh interface IP add address name="%eth_name%" addr=172.16.0.100 mask=255.255.0.0
pause