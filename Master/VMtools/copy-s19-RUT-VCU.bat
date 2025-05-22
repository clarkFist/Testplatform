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