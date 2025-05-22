::Generate image
@echo off
color f2
:top
echo "SELECT TYPE:"
echo 1.make All ROM
echo 2.make All RAM
echo 3.make All CW
echo 4.make All GCC
echo 5.help
echo.
set /p a=Select 

if %a% equ 1 goto ALL_ROM
if %a% equ 2 goto ALL_RAM
if %a% equ 3 goto ALL_CW
if %a% equ 4 goto All_GCC
if %a% equ 5 goto HELP
cls
echo "Select Error!"
echo.
goto HELP

:ALL_ROM
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make clean
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make all_o CPU=CPU_A
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make all_o CPU=CPU_B
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make EMPTY_CRC CPU=CPU_A
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make EMPTY_CRC CPU=CPU_B
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make FIRST_LINK CPU=CPU_A
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make FIRST_LINK CPU=CPU_B
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make RIGHT_CRC CPU=CPU_A
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make RIGHT_CRC CPU=CPU_B
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make FINAL_LINK CPU=CPU_A
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make FINAL_LINK CPU=CPU_B
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make done CPU=CPU_A
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make done CPU=CPU_B
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make local_post_build CPU=CPU_A
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make local_post_build CPU=CPU_B
@pause
@exit

:ALL_RAM
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make clean
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make all_o CPU=CPU_A TYPE=RAM
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make all_o CPU=CPU_B TYPE=RAM
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make EMPTY_CRC CPU=CPU_A TYPE=RAM
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make EMPTY_CRC CPU=CPU_B TYPE=RAM
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make FIRST_LINK CPU=CPU_A TYPE=RAM
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make FIRST_LINK CPU=CPU_B TYPE=RAM
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make RIGHT_CRC CPU=CPU_A TYPE=RAM
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make RIGHT_CRC CPU=CPU_B TYPE=RAM
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make FINAL_LINK CPU=CPU_A TYPE=RAM
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make FINAL_LINK CPU=CPU_B TYPE=RAM
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make done CPU=CPU_A TYPE=RAM
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make done CPU=CPU_B TYPE=RAM
@pause
@exit

:ALL_CW
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make clean
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make all_o CPU=CPU_A
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make all_o CPU=CPU_B COMP=CW
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make EMPTY_CRC CPU=CPU_A
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make EMPTY_CRC CPU=CPU_B COMP=CW
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make FIRST_LINK CPU=CPU_A
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make FIRST_LINK CPU=CPU_B COMP=CW
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make RIGHT_CRC CPU=CPU_A
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make RIGHT_CRC CPU=CPU_B COMP=CW
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make FINAL_LINK CPU=CPU_A
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make FINAL_LINK CPU=CPU_B COMP=CW
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make done CPU=CPU_A
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make done CPU=CPU_B COMP=CW
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make local_post_build CPU=CPU_A
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make local_post_build CPU=CPU_B COMP=CW
@pause
@exit

:ALL_GCC
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make clean
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make all_o CPU=CPU_A COMP=GCC
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make all_o CPU=CPU_B
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make EMPTY_CRC CPU=CPU_A COMP=GCC
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make EMPTY_CRC CPU=CPU_B
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make FIRST_LINK CPU=CPU_A COMP=GCC
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make FIRST_LINK CPU=CPU_B
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make RIGHT_CRC CPU=CPU_A COMP=GCC
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make RIGHT_CRC CPU=CPU_B
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make FINAL_LINK CPU=CPU_A COMP=GCC
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make FINAL_LINK CPU=CPU_B
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make done CPU=CPU_A COMP=GCC
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make done CPU=CPU_B
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make local_post_build CPU=CPU_A COMP=GCC
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make local_post_build CPU=CPU_B
@pause
@exit

:HELP
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make HELP
goto top