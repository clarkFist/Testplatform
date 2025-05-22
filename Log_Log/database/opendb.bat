@rem  -s starttime #创建的视图数据的起始时间
@rem  -e endtime  #视图数据的终止时间，-1
@rem  这两个参数为整形时，正时为当前时间多少天前，为负时为多少天后
@rem  这两个参数为%Y-%m-%d %H:%M:%S 形式时，以该形式为准
@rem  -v savetime #路径、行号视图只包括savetime 分钟内发送过消息的数据源（路径行号），与starttime取最近的时间,r若未定义则取starttime
@rem   -l limit = 10000 #文件行号视图的数据量限制
::CONFIG_START
python %~dp0updateview.py -p %1 -s 50  -l 10000
::CONFIG_END
set err="%ERRORLEVEL%"
@if %err% == "333" (start %~dp0SQLiteSpy/SQLiteSpy.exe %1.db)
@if %err% == "0" (start %~dp0SQLiteSpy/SQLiteSpy.exe %1)
pause
