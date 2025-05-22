rem 槽道分隔符仅支持 . ,
rem arg1:等价于 copySrc_toVM openTagInit build copyImage_fromVM update
rem arg2:将目标槽道识别为svcu 或 mvcu 并进行编译，随后自动更新, 不支持mvcu, svcu 同时输入
rem arg3:若未输入则不打开初始化桩
rem 若无arg, 则需手动输入槽道号，不能打开初始化桩
venv\Scripts\python.exe py_lib/vmtcp.py
pause