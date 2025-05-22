rem #更新对象 image 镜像 cfg  配置 dump 宕机日志

rem IMAGE:
rem #镜像的格式，区分大小写
rem #操作指令 0文件上传并更新 1文件上传  2恢复更新 3镜像更新 4重启 5获取版本号
rem #镜像所在的CPU_X所在路径
rem #槽道号 可选参数 若未给出则手动输入 (rack-1)*16 + slot = moudle - 3

rem CFG:
rem # 配置名称
rem # 槽道号 只能包含mvcu 可选参数 若未给出则手动输入 (rack-1)*16 + slot = moudle - 3

rem DUMP:
rem # 对应CPU 只能为 A B AB
rem # 槽道号 可选参数 若未给出则手动输入 (rack-1)*16 + slot = moudle - 3

::python3 light_update.py image S19 0 . 
::python3 light_update.py image S19 0 . 5
::python3 light_update.py cfg CSZ_ECID_VOOB_6diff
python3 light_update.py dump AB
pause