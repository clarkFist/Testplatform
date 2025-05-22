::Generate image with CRC
@echo off
color f2
::@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make clean
::@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make display CPU=CPU_A
::@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make display CPU=CPU_A COMP=GCC
::@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make display CPU=CPU_B
::@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make display CPU=CPU_B COMP=CW

@C:\MSTEP\Sourcery_CodeBench_Lite_for_ColdFire_ELF\bin\cs-make display CPU=CPU_A
@C:\MSTEP\Sourcery_CodeBench_Lite_for_ColdFire_ELF\bin\cs-make display CPU=CPU_B

@pause

