rem # 配置更新 config
rem # 连着网线的LRU
rem # IP
rem # 文件路径及名称
rem # safetyClient：默认值为1，可由用户改写，格式为16进制
rem # LRU to be transfered : 需要转发就填写待转发的LRU，不需要转发就填写0
rem # 操作指令  0-配置数据上传并更新 1-配置数据上传  2-配置数据更新
rem # 上层工具需要的参数（目前默认为0）
timeout /t 3
cd ..
java -jar rut-2.0-SNAPSHOT.jar  config 1_16S 20.1.1.10  ./input/%1% 00000001 0 0 0
pause
exit