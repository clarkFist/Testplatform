::Generate image without CRC
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make clean
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make all_o CPU=CPU_A
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make all_o CPU=CPU_B
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make EMPTY_CRC CPU=CPU_A
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make EMPTY_CRC CPU=CPU_B
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make FIRST_LINK CPU=CPU_A
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make FIRST_LINK CPU=CPU_B
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make all_b CPU=CPU_A
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make all_b CPU=CPU_B
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make all_s CPU=CPU_A
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make all_s CPU=CPU_B
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make stop CPU=CPU_A
@C:\Freescale\CW_MCU_v10.4\Cross_Tools\CodeSourcery_Linux\bin\cs-make stop CPU=CPU_B
@pause

