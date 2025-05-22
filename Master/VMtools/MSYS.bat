echo %cd%
python %~dp0msyslog.py
pause
if %ERRORLEVEL% NEQ 0 (pause)