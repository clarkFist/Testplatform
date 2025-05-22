# VCU_VM

> 20211015

## 一.  功能

### 1. 编译

```python
# 通过调用 sh.exe 执行编译目标的make.sh
p = subprocess.Popen(r'C:\dev_tools\MSYS-1.0.10\1.0\bin\sh --login -i',\
        shell=False,cwd = cmdpath, stdin = stdin,stdout = subprocess.PIPE,stderr = subprocess.STDOUT)
```

在SVCU 或 MVCU 的目录内有MSYS.lnk 快捷方式，会自动调用当前文件夹的make.sh

编译结果的警告会被忽略， 错误会被特殊标记，并将完整的编译结果保存在 output.msys 内

### 2. 检查测试桩

  编译前会默认检查源代码 tst_vcu_stubs.h （若存在）内的枚举桩格式是否正确

包括

```textile
1. 是否存在值相同的枚举桩
如 VCU_SyRTC_0001_1= 0x000101,
   VCU_SVCU_SwRTC_0001_1= 0x000101,
2. 枚举桩的命名是否与值对应
如 VCU_SyRTC_0001_1= 0x000102,
若枚举桩值为  VCU_SyRTC_0001_1= 258, 则会警告不会报错
```

### 3.  虚拟机内外交互

  可通过 Copy_SrcToVM.bat 将指定目录的所有内容复制到目标目录

```batch
call ../Copy_SrcToVM.bat E:\VCU114\SWRTC_SVCU\SVCU\VCU_CW_CPUA %~dp0src

echo %ERRORLEVEL%
pause
```

可通过 copyfromVM.py.bat 将build/out的部分内容（*.S19, * .elf, *. bin ）复制到目标目录

```batch
cd %~dp0
set ttt=svcu
python ../copyfromVM.py %ttt% && call ../copy-s19-RUT-VCU.bat %ttt%

echo %errorlevel%
PAUSE
```

 打开vmtcp.bat 文件可在虚拟机配置一个tcp 服务端，可以实现以下功能

    1. 远程更改待编译文件
    2. 远程编译
    3. 远程发送虚拟机内文件

### 4. 集成烧录命令

  在SVCU , MVCU 目录内提供了烧录当前编译镜像的命令

```textile
1_Flash_basic_conf_A_CW.bat
3_Flash_basic_conf_B_GCC.bat
```

在VMTOOL 目录提供了烧录熔丝、电子戳的命令

```textile
Flash_Es #电子戳文件夹
Restore_fuse_new # 烧录第一组熔丝
Restore_fuse_second# 烧录第二组熔丝
```

## 二. 配置

### 1. LINK

  安装 Python 2.7.8（32位）可通过 python -v 查看版本

  将site-packages 内的文件复制至C:\Python27\Lib\site-packages 

  配置4. VM_TCP 打开vm_tcp.bat ，无报错则配置成功， 若报错可重装Python 2.7.8

  在VMTools（名称可更改） 文件夹执行python link.py 命令（shift + 右键 在此处打开命令窗口） 可配置VMTools 内工具的路径,  每次更改VMTools 的名称或路径均需运行link.py

### 2. copy-s19-RUT-VCU.bat

```batch
@echo off

cd "%~dp0"
set dst=E:\testplatform\RUT

set mvcu_local=%~dp0VCU_DEV_MVCU\build\out
set svcu_local=%~dp0VCU_DEV\build\out

set mvcu_lib_local=%~dp0mVCU_DEV_GUEST\build\out
set svcu_lib_local=%~dp0sVCU_DEV_GUEST\build\out
if "%1"=="" (call:func mvcu
call:func svcu) else (call:func %1)
goto END
:func
 echo %1
 setlocal enabledelayedexpansion
 set path_local=!%1_local!
 setlocal disabledelayedexpansion
 set src_tail=NOETH
 set dst_tail=NOETH
 if "%1"=="mvcu_lib" (set dst_tail=INTA)
 if "%1"=="mvcu" (set dst_tail=INTA
 set src_tail=INTA)
 
 set dstpath_a=%dst%\CPU_A\VCU_CW_CPU_A_ROM_GUEST_%dst_tail%
 set srcpath_a=%path_local%\CPU_A\VCU_CW_CPU_A_ROM_GUEST_%src_tail%
 set dstpath_b=%dst%\CPU_B\VCU_GCC_CPU_B_ROM_GUEST_%dst_tail%
 set srcpath_b=%path_local%\CPU_B\VCU_GCC_CPU_B_ROM_GUEST_%src_tail%
 @for %%t in (.lzw .S19 .bin .elf) do ( 
 del "%dstpath_a%%%t"
 del "%dstpath_b%%%t"
)
@for %%t in (.lzw .S19 .elf) do ( 

 echo %srcpath_a% %dstpath_a% 
 echo %srcpath_b% %dstpath_b%
COPY "%srcpath_a%%%t" "%dstpath_a%%%t"
COPY "%srcpath_b%%%t" "%dstpath_b%%%t"
)
COPY "%srcpath_a%.rbin" "%dstpath_a%.bin"
COPY "%srcpath_b%.bin" "%dstpath_b%.bin"
goto:eof %path_local%

:END
PAUSE
```

### 3. Copy_SrcToVM.bat

```batch
@echo off

set svcu=E:\V115预测试\SVCU_CW_CPUA-V115-20211112\VCU_CW_CPUA
set mvcu=E:\V115预测试\MVCU_CW_CPUA-V115-20211112规则检查\MVCU_CW_CPUA-V115-20211112\VCU_CW_CPUA
set mvcu_local=%~dp0VCU_DEV_MVCU\src
set svcu_local=%~dp0VCU_DEV\src
if "%1"=="" (
 set for_set[0]=mvcu 
 set for_set[1]=svcu 
 set oplen=2
) else ( 
  set oplen=0
  for %%d in (%*) do ( 
  echo %%d
  set /a oplen+=1)
  set for_set[0]=%1
  set for_set[1]=%2
  set for_set[2]=%3
  set for_set[3]=%4
  )
 echo copy num  %oplen% %for_set[0]% %for_set[1]% %for_set[2]% %for_set[3]%
 set index = 0

 call:func %for_set[0]%

 set /a index+=1
 if %oplen%==%index% (goto END)
 
 call:func %for_set[1]%
 set /a index+=1
 if %oplen%==%index% (goto END)
 
 call:func %for_set[2]%
 set /a index+=1
 if %oplen%==%index% (goto END)
 
 call:func %for_set[3]%
 set /a index+=1
 if %oplen%==%index% (goto END)
goto END
  
:func
 echo run func %1
 set dst=%1
 setlocal enabledelayedexpansion
 set path_local=!%dst%_local! 
 set path_remote=!%dst%!
 setlocal disabledelayedexpansion
 echo src:%path_remote% 
 echo dst:%path_local%
 @rd %path_local% /s /Q  

 @rd %path_remote%\RAM /s /Q  2>nul 

 md %path_local% 
@IF %ERRORLEVEL% NEQ 0 (
echo %ERRORLEVEL%
ERRORLEVEL=11
goto END %1)
 xcopy "%path_remote%" "%path_local%" /s /K /H >nul 
@IF %ERRORLEVEL% NEQ 0 (
echo %ERRORLEVEL%
ERRORLEVEL=12
goto END %1)
goto:eof

:END
if %ERRORLEVEL% == 0 ( echo copy success %for_set[0]% %for_set[1]% %for_set[2]% %for_set[3]%) else (echo fail %1 %ERRORLEVEL%)
pause
```

### 4. VM_TCP

vmtcp.py 文件在虚拟机外，需在vm_tcp.bat 内配置路径，或将文件放在VMTools 目录下

镜像与源代码路径取决于Copy_SrcToVM.bat，copy-s19-RUT-VCU.bat

```batch
::vm_tcp.bat
set target=E:\testplatform\py_lib\vmtcp.py
::set target=vmtcp.py
python %target% server   
```

### 5. NotePad++(可选)

  使用NotePad++ 打开output.msys(编译结果的完整日志) ，选择语言-> 用户语言格式->导入 选择VMTools 目录内的 msys.xml 文件，重启就可以对log的部分语法高亮。

### 6. Source Insight（可选）

 在source insight 选项/自定义命令内添加直接编译命令；

![](.\pictures\2021-10-15-10-27-24-image.png) 

命令： ^\([a-zA-Z]+\.[ch]\):\([0-9]+\):.*

点击’键’按钮设置快捷键

 可在source insight 定位错误位置



