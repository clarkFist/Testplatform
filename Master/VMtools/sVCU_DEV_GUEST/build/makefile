################################################################################
# Copyright 2019 - CASCO - Shanghai
################################################################################
#
# Variables used in a guest makefile:
#
# $(MODULE_NAME) is used for print name during the makefile execution. 
#	-> MANDATORY
#
# $(APP_MODULENAME) indicates the relative path to all *.c/*.h of folder name to compile, separated by space (for instance, 'VOOB VOOB/src')
# NOTE: The folder path cannot use the space character !!!
#
# $(LIBRARIES) indicates pre-compiled libraries (absolute PATH) used by the module (for instance, '$(MSTEP_DEV_ROOT)/external_lib').
#	-> MANDATORY, default with '../kernel'
#
# $(CW_PATH) is used for path to Codewarrior compiler
#	-> MANDATORY
# $(GCC_PATH) is used for path to GCC compiler
#	-> MANDATORY
#
# $(COMP_FUNC) is Path Definition for compress tool
#	-> MANDATORY if 'COMPRESS' = 1
################################################################################

# path to Codewarrior compiler
CW_PATH = C:/Freescale/CW_MCU_v10.4/MCU/ColdFire_Tools/Command_Line_Tools
#CW_PATH = E:/C_backup/dev_compiler/CW/ColdFire_Tools/Command_Line_Tools
# path to GCC compiler
GCC_PATH = C:/GCC/bin
#GCC_PATH = E:/C_backup/dev_compiler/GCC/bin

# Used application name by folder
APP_MODULENAME = demo/src/com demo/src/tst demo/src/voob

#List Module Name
MODULE_NAME = GUEST_NOETH

#LIB List
LIBRARIES = ../kernel

# Definition for compress feature
COMP_FUNC = ./crc_make/sample.exe

COMPRESS = 1
#Makefile included
include ./gen_def.mk
#include ./gen_def_opt.mk
