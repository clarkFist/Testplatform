::Generate Lib only

@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make clean
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make all_o CPU=CPU_A
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make all_o CPU=CPU_A COMP=GCC
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make all_o CPU=CPU_B
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make all_o CPU=CPU_B COMP=CW
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make local_post_build CPU=CPU_A
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make local_post_build CPU=CPU_A COMP=GCC
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make local_post_build CPU=CPU_B
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make local_post_build CPU=CPU_B COMP=CW
@pause

