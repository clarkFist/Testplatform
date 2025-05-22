pause
rem #镜像的格式，区分大小写
rem #操作指令 0文件上传并更新 1文件上传  2恢复更新 3镜像更新 4重启 5获取版本号
rem #镜像所在的CPU_X所在路径
rem #槽道号 可选参数 若未给出则手动输入 (rack-1)*16 + slot = moudle - 3
start python light_update.py S19 0 r32s32 13-14,18-30,32,34,35
start python light_update.py S19 0 r64s64 11,12
start python light_update.py S19 0 r128s32 3-6
start python light_update.py S19 0 r32s128 7-10
