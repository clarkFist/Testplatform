## 一. SNTP

 sntp : ntp 协议的简化

### **时间表示：**

* NTP time: NTP 时间有两个32 位的时间表示 秒，与毫秒。第一个为 1900年1月1日 到当前时间的秒数，第二个为当前毫秒 * 0x418937 

* VCU can time:  用两个 INT32U  表示， 第一个为1984年1月1日到当前时间的天数，第二位为当前天数的第n 秒，当ntp 服务器启动且可信时， cantime 会与 ntp 同步

* UNIX 时间戳：从1970 年1 月1 日到当前的秒数 

## 二.  SNMP

  TCP/IP 网络协议管理框架分为三部分

1. 第一部分为网络管理协议（**SNMP**），主要涉及同信息通信相关的关系和消息流，定义了管理系统上运行的管理站软件如何与管理代理通信，包括两者之间交换的消息分组的格式、含义及名字与值的表示等，此外也定义了被管设备间的管理关系，即提供了管理系统的授权管理。

2. 第二部分为管理信息结构（**SMI**，Structure of Management Information）是描述管理信息的标准符号，说明了定义和构造MIB的总体框架，以及数据类型的表示和命名方法。

3. 第三部分为管理信息库（**MIB**，Management Information Base）。MIB定义了受管设备必须保存的数据项、允许对每个数据项进行的操作及其含义，即管理系统可访问的受管设备的控制和状态信息等数据变量都保存在MIB中。MIB定义的通用化格式支持对每一个新的被管理设备定义其特定的MIB组，因此厂家可以采用标准的方法定义其专用的管理对象，从而可以管理许多新协议和设备，可扩展性很好。

### 发送**trap**的触发条件

1. 如果N-1周期rMonGauge组中一个变量的值rmgValue低于阈值rmgThresholdAlarm，N周期该变量的rmgValue达到或超过rmgThresholdAlarm，则VCU发送code为1xxx的trap，其中xxx为rmgTable中对应的RmgEntry的rmgIndex。
2. 如果N-1周期rMonGauge组中一个变量的值rmgValue超过阈值rmgThresholdAlarm，N周期该变量的rmgValue低于rmgThresholdAlarm，则VCU发送code为2xxx的trap，其中xxx为rmgTable中对应的RmgEntry的rmgIndex。
3. 如果LRU组中出现一个故障码（lruDefaultCode不等于0），则VCU发送code为3 yyy xxx的trap，其中yyy为该故障码，xxx为lruTable中对应的lruEntry的lruIndex。
4. 如果LRU组中一个故障码消失（lruDefaultCode恢复为0），则VCU发送code为4 yyy xxx的trap，其中yyy为该故障码，xxx为lruTable中对应的lruEntry的lruIndex。

VCU 使用的 SNMP 版本为 SNMPv1, 端口 161 Community 为 URBALIC

**测试平台继承了VCU 的MIB 的获取模块， 可以通过 属性名获取,**

## 三. WireShark

  WireShark 的根目录会存在多个 .html 的参考文档

### 1.  过滤：

* 协议名称：udp , snmp

* 字段：
  
  * udp.port == 50001 || udp.port in {40001, 40000}
  
  * udp.port >= 50000

* 切片：
  
  * udp and data[0] == 01
  
  * udp and data[2:2] == 01:02
  
  * udp and data[2-4] == 01:02
  
  * udp and data[-3:] == 03:04:05

* 字节： # 任何一位满足均为true
  
  * udp.port & 0x0011 // 相当于udp.port & 0x0011 != 0
  
  * udp and data[-2:] & 01:00
  
  参考[wireshark-filter.html](./wireshark-filter.html)

### 2. 命令行抓包

  测试平台的wireShark 抓包是对dumpcap.exe 的封装

  调用方法参考[dumpcap.html](./dumpcap.html)

### 3. 自定义协议

  测试平台提供了三个自定义协议， 可以解析FSFB2, RSSP1 和 测试平台的报文

  编写方法参考官方文档

## 四. 模拟器

**对EDICSim 和 RSSP1 进行了一些修改，可以通过修改initValue.ini 设置模拟器的初值**

### 1. FSFB2:

  修改了FSFB2_SimDataGen_New 的两个默认设置

* 修改了触发的默认本地IP

* 修改了触发式的PollingMode(from 1 to 0)  

### 2. RSSP1

  当log文件为只读时不会存储log

### initValue.ini

```ini
RSSP1-1:E000
RSSP1-2:0000
RSSP1-3:0000
RSSP1-4:0000
RSSP1-5:E000
RSSP1-6:E000
```

格式为name:value, 每一行的空格会被忽略

name 与模拟器的version.ini 的 VERSION 一致。

value 是16进制的值， E000 表示模拟器前三个码位设置为一，其他码位设置为0

value 最长为56位，对应模拟器224个码位。超过的部分会被忽略

## 五. 离线dump 工具

### 1. offline_tool

 配置好烧录器， 打开offline_tool.bat

 在弹出的窗口 键入 cd xx/offline_tool ,xx 是offline_tool 的目录

 offline_tool 文件夹的名称不可更改

 如果烧录 A 通道， 则需将烧录器插在A 通道， 同时将B 通道复位

 随后执行 extract_dumpinfo.sh A （如果是B 则为B ）

 若烧录器不可用（包括未插好，没有驱动）

>  m68k-elf-sprite:error:Cannot find a matching debug device

 正常运行 会出现两次 BreakPoint,  若只出现一次就卡死，则大概率为对通道未复位

 dump 过程中 会将 结果先存放在offline_tool/bin_out 文件夹

 完成后后会移动到 offline_tool/bin_out/vcu_dump 文件夹

 参考 offline_tool/offline下载工具使用手册.docx

### 2. analyse_dump_tool

   将dump tool (包括在线dump 或 离线 dump) 生成的双通道的 Dump_Sram、   Dump_Sdram 文件复制到analyse_dump_tool\dumpinfo 文件夹, 重命名为    sram_a.bin, sram_b.bin sdram\_a.bin, sdram_b.bin

   将宕机板卡的代码的编译生成的 双通道的 elf 文件 复制到analyse_dump_tool\dumpinfo 文件夹, 重命名为 a.bin, b.bin

  打开analyse_dump_tool\MSYS-1.0.10\1.0\路径下,点击msys.bat,打开命令窗口
  输入命令跳到analyse_dump_tool\sh路径下
  输入dumpA_vcu.sh或dumpB_vcu.sh回车即可,出现CMD后工具分析完毕

  参考analyse_dump_tool/DumpToolUM.docx

## 六. RUT

### 1. 在线更新镜像

```batch
rem #LRU
rem #下位机IP地址
rem #CPUA和CPUB文件夹路径
rem #CPU_A文件名称
rem #CPU_B文件名称
rem #操作指令 0文件上传并更新 1文件上传  2恢复更新 3镜像更新 4重启 5获取版本号
rem #上层工具需要的参数（目前默认为0）
java -jar rut-2.0-SNAPSHOT.jar  1_2P 20.1.1.10 . VCU_CW_CPU_A_ROM_GUEST_NOETH.S19 VCU_GCC_CPU_B_ROM_GUEST_NOETH.S19 0 0
pause
```

### 2.  在线更新配置

```batch
rem # 配置更新 config
rem # 连着网线的LRU
rem # IP
rem # 文件路径及名称
rem # safetyClient：默认值为1，可由用户改写，格式为16进制
rem # LRU to be transfered : 需要转发就填写待转发的LRU，不需要转发就填写0
rem # 操作指令  0-配置数据上传并更新 1-配置数据上传  2-配置数据更新
rem # 上层工具需要的参数（目前默认为0）
java -jar rut-2.0-SNAPSHOT.jar config 1_2P 20.2.1.10 ./input/%1% 00000001 0 0 0
pause
```

### 3. 在线dump

```batch
rem #LRU
rem #下位机IP地址
rem #CPUA/CPUB
cd "./dump"
set ip=20.2.1.10
set slot=1_2P
start java -jar RTD-1.0-SNAPSHOT.jar %slot% %ip% A
start java -jar RTD-1.0-SNAPSHOT.jar %slot% %ip% B
pause
```

### 4.  测试平台的在线更新镜像命令

```batch
rem #镜像的格式，区分大小写
rem #操作指令 0文件上传并更新 1文件上传  2恢复更新 3镜像更新 4重启 5获取版本号
rem #镜像所在的CPU_X所在路径
rem #槽道号 可选参数 若未给出则手动输入 (rack-1)*16 + slot = moudle - 3
start python3 light_update.py image bin 0 .
```

## 七. VS code

  测试平台 对VS code 版本无要求， 安装Python 插件即可

  测试平台提供了一个集成Python 的VScode

  VScode 的常用快捷键参考文件：[ VS Code常用快捷键总结.md](./VSCode常用快捷键总结.md)

## 八.  MD2HTML

  Mark Tesk:一款简单好用、开源的markdown 编辑器

  官网： [http://marktext.app](http://marktext.app)

   i5ting_toc:  将markdown 文件转换为html

  GitHub: [https://github.com/chjj/marked](https://github.com/chjj/marked)

   可通过 md2html.py 脚本将当前文件夹下所有.md 文件转为html

```batch
rem arg1:若给出则只转化单独一个文件，未赋值则转换当前文件夹
rem 脚本需要安装node.js 和 i5ting_toc 模块
python md2html.py %*
pause
```



