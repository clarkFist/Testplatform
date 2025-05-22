@echo off

set svcu=E:\V115Ô¤²âÊÔ\SVCU_CW_CPUA-V115-20211112\VCU_CW_CPUA
set mvcu=E:\V115Ô¤²âÊÔ\MVCU_CW_CPUA-V115-20211112¹æÔò¼ì²é\MVCU_CW_CPUA-V115-20211112\VCU_CW_CPUA
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