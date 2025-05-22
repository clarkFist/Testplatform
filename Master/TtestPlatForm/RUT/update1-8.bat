rem #LRU
rem #下位机IP地址
rem #CPUA和CPUB文件夹路径
rem #CPU_A文件名称
rem #CPU_B文件名称
rem #操作指令 0文件上传并更新 1文件上传  2恢复更新 3镜像更新 4重启 5获取版本号
rem #上层工具需要的参数（目前默认为0）
java -jar rut-2.0-SNAPSHOT.jar  1_8 20.1.1.10 . VCU_CW_CPU_A_ROM_GUEST_NOETH.S19 VCU_GCC_CPU_B_ROM_GUEST_NOETH.S19 0 0
pause