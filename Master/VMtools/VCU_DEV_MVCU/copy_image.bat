cd %~dp0
set ttt=mvcu
python ../copyfromVM.py %ttt% && call ../copy-s19-RUT-VCU.bat %ttt%

echo %errorlevel%
PAUSE