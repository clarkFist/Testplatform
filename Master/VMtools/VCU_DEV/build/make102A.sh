# Copyright CASCO SIGNAL - 2018
# XD - 01/01/2018
# This file call make to build the full binary to be flashed. 
# This file exist only to be called from the run.bat script for DOS 
# compatibility as this is no easy to call make directly.

# First clean before rebuilding all, will be change when dependency (.h) will 
# be managed at makefile level.

make clean
make all_o CPU=CPU_A
make all_o CPU=CPU_B
make EMPTY_CRC CPU=CPU_A
make EMPTY_CRC CPU=CPU_B
make FIRST_LINK CPU=CPU_A
make FIRST_LINK CPU=CPU_B
make RIGHT_CRC CPU=CPU_A
make RIGHT_CRC CPU=CPU_B
make FINAL_LINK CPU=CPU_A
make FINAL_LINK CPU=CPU_B
make done CPU=CPU_A
make done CPU=CPU_B
make local_post_build CPU=CPU_A
make local_post_build CPU=CPU_B

