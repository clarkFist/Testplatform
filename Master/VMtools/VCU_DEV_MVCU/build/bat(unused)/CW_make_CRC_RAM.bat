::Generate image with CRC
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make clean
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make all_o CPU=CPU_A TYPE=RAM
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make EMPTY_CRC CPU=CPU_A TYPE=RAM
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make FIRST_LINK CPU=CPU_A TYPE=RAM
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make RIGHT_CRC CPU=CPU_A TYPE=RAM
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make FINAL_LINK CPU=CPU_A TYPE=RAM
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make done CPU=CPU_A TYPE=RAM
@pause

