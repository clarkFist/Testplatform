##############################################################################################
# Copyright 2018 - CASCO SIGNAL, MAKEFILE For GUEST
##############################################################################################
#
#                 ii.                                         ;9ABH,          
#                SA391,                                    .r9GG35&G          
#                &#ii13Gh;                               i3X31i;:,rB1         
#                iMs,:,i5895,                         .5G91:,:;:s1:8A         
#                 33::::,,;5G5,                     ,58Si,,:::,sHX;iH1        
#                  Sr.,:;rs13BBX35hh11511h5Shhh5S3GAXS:.,,::,,1AG3i,GG        
#                  .G51S511sr;;iiiishS8G89Shsrrsh59S;.,,,,,..5A85Si,h8        
#                 :SB9s:,............................,,,.,,,SASh53h,1G.       
#              .r18S;..,,,,,,,,,,,,,,,,,,,,,,,,,,,,,....,,.1H315199,rX,       
#            ;S89s,..,,,,,,,,,,,,,,,,,,,,,,,....,,.......,,,;r1ShS8,;Xi       
#          i55s:.........,,,,,,,,,,,,,,,,.,,,......,.....,,....r9&5.:X1       
#         59;.....,.     .,,,,,,,,,,,...        .............,..:1;.:&s       
#        s8,..;53S5S3s.   .,,,,,,,.,..      i15S5h1:.........,,,..,,:99       
#        93.:39s:rSGB@A;  ..,,,,.....    .SG3hhh9G&BGi..,,,,,,,,,,,,.,83      
#        G5.G8  9#@@@@@X. .,,,,,,.....  iA9,.S&B###@@Mr...,,,,,,,,..,.;Xh     
#        Gs.X8 S@@@@@@@B:..,,,,,,,,,,. rA1 ,A@@@@@@@@@H:........,,,,,,.iX:    
#       ;9. ,8A#@@@@@@#5,.,,,,,,,,,... 9A. 8@@@@@@@@@@M;    ....,,,,,,,,S8    
#       X3    iS8XAHH8s.,,,,,,,,,,...,..58hH@@@@@@@@@Hs       ...,,,,,,,:Gs   
#      r8,        ,,,...,,,,,,,,,,.....  ,h8XABMMHX3r.          .,,,,,,,.rX:  
#     :9, .    .:,..,:;;;::,.,,,,,..          .,,.               ..,,,,,,.59  
#    .Si      ,:.i8HBMMMMMB&5,....                    .            .,,,,,.sMr
#    SS       :: h@@@@@@@@@@#; .                     ...  .         ..,,,,iM5
#    91  .    ;:.,1&@@@@@@MXs.                            .          .,,:,:&S
#    hS ....  .:;,,,i3MMS1;..,..... .  .     ...                     ..,:,.99
#    ,8; ..... .,:,..,8Ms:;,,,...                                     .,::.83
#     s&: ....  .sS553B@@HX3s;,.    .,;13h.                            .:::&1
#      SXr  .  ...;s3G99XA&X88Shss11155hi.                             ,;:h&,
#       iH8:  . ..   ,;iiii;,::,,,,,.                                 .;irHA  
#        ,8X5;   .     .......                                       ,;iih
#           1831,                                                 .,;irr
#             ;5A8r.                                            .:;iii
#               :X@H3s.......                                .,:;iii
#                r#h:;,...,,.. .,,:;;;;;:::,...              .:;;;
#               ,M8 ..,....,.....,,::::::,,...         .     .,;;;
#               8B;.,,,,,,,.,.....          .           ..   .:;;;;
#              i@5,:::,,,,,,,,.... .                   . .:::;;;;;i;
#              9Bi,:,,,,......                        ..r91;;;;;iir;;
#  
##############################################################################################
### Could be modified if path have changed by application###
#NOTE: The folder name cannot use the space character !

# path to Codewarrior compiler
CW_PATH = C:/Freescale/CW_MCU_v10.4/MCU/ColdFire_Tools/Command_Line_Tools

# path to GCC compiler
GCC_PATH = C:/gcc/bin

# Used application name by folder
APP_MODULENAME = demo/src/voob demo/src/tst

#List Module Name
MODULE_NAME = GUEST_NOETH
### End of allowed modificatin ###

# Path Definition for Compiler
CW_CC   = $(CW_PATH)/mwccmcf
GCC_CC   = $(GCC_PATH)/m68k-elf-gcc

CW_AS   = $(CW_PATH)/mwasmmcf
GCC_AS   = $(GCC_PATH)/m68k-elf-gcc

# Path Definition for linkers
CW_LK   = $(CW_PATH)/mwldmcf
GCC_LK   = $(GCC_PATH)/m68k-elf-ld

# Path Definition for Objcopy
CW_OBJCP   = $(CW_PATH)/mwldmcf
GCC_OBJCP   = $(GCC_PATH)/m68k-elf-objcopy

#Path Definition for AR library maker
CW_AR  = $(CW_PATH)/mwldmcf
GCC_AR = $(GCC_PATH)/m68k-elf-ar

# Path Definition for CRC maker
CRC_FUNC = ./crc_make/crc.exe

# Default to Flash target
ifndef TYPE
TYPE = ROM
endif

#Target Identification
TARGET = VCU_$(PREFIX)$(CPU)_$(TYPE)_$(MODULE_NAME)

#Linker Scripts
CW_LINKER_SCRIPT = ../Project_Settings/Linker_Files/VCU_CW_LD_AB_$(TYPE).lcf
GCC_LINKER_SCRIPT = ../Project_Settings/Linker_Files/VCU_GCC_LD_$(CPU)_$(TYPE).ld

# Module List

COMMONMODULE = $(addsuffix /include, $(LIBRARIES))

APP_MODULE = $(addprefix ../guest/,$(APP_MODULENAME))

ALLMODULE = $(COMMONMODULE) $(APP_MODULE)

# OBJ List
OBJ_DIR = ./out/$(CPU)/

CRC_SOURCE = $(OBJ_DIR)$(PREFIX)crc_result.S
FINAL_CRC_SOURCE = ./out/final_crc_result.S
CRC_OBJ = $(OBJ_DIR)$(PREFIX)crc_result.o

CW_ELFOBJ = $(OBJ_DIR)$(TARGET).elf
CW_S19OBJ = $(OBJ_DIR)$(TARGET).S19
CW_MAPOBJ = $(ELFOBJ).xMAP
CW_BINOBJ = $(OBJ_DIR)$(TARGET).rbin

GCC_ELFOBJ = $(OBJ_DIR)$(TARGET).elf
GCC_S19OBJ = $(OBJ_DIR)$(TARGET).S19
GCC_MAPOBJ = $(OBJ_DIR)$(TARGET).map
GCC_BINOBJ = $(OBJ_DIR)$(TARGET).bin

#LIB List
LIBRARIES = ../kernel

# LIBRARIES AND PATH used for link
LIBS       = $(addprefix -l$(PREFIX), $(basename $(notdir $(LIBRARIES))))
LIBS_PATHS = $(addprefix -L, $(addsuffix /lib/$(CPU), $(LIBRARIES)))

#Library
LIB_NAME = $(addprefix $(OBJ_DIR)lib$(PREFIX), kernel.a)

# Sort libs in different orders for each CPU
# Only for test sort -r
ifeq ($(CPU), CPU_B)
LIBS_SORT  = $(shell for lib in $(LIBS); do echo $$lib; done | sort -r)
else
LIBS_SORT  = $(shell for lib in $(LIBS); do echo $$lib; done | sort)
endif

##############################################################################################
# File List include Kernel and All

CFILES = $(foreach n,$(APP_MODULE),$(wildcard ./$(n)/*.c))

SFILES = $(foreach n,$(APP_MODULE),$(wildcard ./$(n)/*.s))

OPTI_SFILES = $(foreach n,$(APP_MODULE),$(wildcard ./$(n)/*.S))

ALLFILES = $(SFILES) $(OPTI_SFILES) $(CFILES)

OBJECTS     = $(join $(dir $(ALLFILES)), $(addprefix $(PREFIX), $(addsuffix .o,$(basename $(notdir $(ALLFILES))))))


# Include DIR
INCDIR = $(addprefix $(inc_opt) ,$(ALLMODULE))

#OBJS    = $(OBJECTS)
KERN_OBJS    = $(sort $(KERN_OBJECTS))

TARGET_OBJS = $(addprefix $(OBJ_DIR), $(notdir $(OBJECTS)))
#TARGET_OBJS = $(sort $(addprefix $(OBJ_DIR), $(notdir $(OBJECTS))))

##############################################################################################
# Calc .Elf size

SIZE_BOOT           = `$(GCC_PATH)/m68k-elf-objdump -h $(ELFOBJ) | grep -F  ".boot" |tr -s ' ' |cut -d " " -f4`
OFFSET_BOOT         = `$(GCC_PATH)/m68k-elf-objdump -h $(ELFOBJ) | grep -F  ".boot" |tr -s ' ' |cut -d " " -f7`

# Vector table offset is not an entire sectire. It is only a table located in .boot section. The table
# start address is BOOT_VECTOR_TO_RAM. The four following variable are used to extract this table address in .ELF files:
EXT_FLASH_START_VMA = \`$(GCC_PATH)/m68k-elf-objdump -h $(ELFOBJ) | grep -F  ".boot" |tr -s ' ' |cut -d " " -f6\`
VECTOR_START_VMA    = \`$(GCC_PATH)/m68k-elf-objdump -t $(ELFOBJ) | grep "BOOT_VECTOR_TO_RAM" | tr -s ' ' | cut -d " " -f1\`
EXT_FLASH_OFFSET    = \`$(GCC_PATH)/m68k-elf-objdump -h $(ELFOBJ) | grep -F  ".boot" |tr -s ' ' |cut -d " " -f7\`
OFFSET_VECTOR       = `VAL=$$((0x$(EXT_FLASH_OFFSET)+0x$(VECTOR_START_VMA)-0x$(EXT_FLASH_START_VMA))) && printf "%x" $$VAL`

SIZE_CODE           = `$(GCC_PATH)/m68k-elf-objdump -h $(ELFOBJ) | grep -F  ".code" |tr -s ' ' |cut -d " " -f4`
OFFSET_CODE         = `$(GCC_PATH)/m68k-elf-objdump -h $(ELFOBJ) | grep -F  ".code" |tr -s ' ' |cut -d " " -f7`

SIZE_CODE_SRAM      = `$(GCC_PATH)/m68k-elf-objdump -h $(ELFOBJ) | grep -F  ".sram_code" |tr -s ' ' |cut -d " " -f4`
OFFSET_CODE_SRAM    = `$(GCC_PATH)/m68k-elf-objdump -h $(ELFOBJ) | grep -F  ".sram_code" |tr -s ' ' |cut -d " " -f7`

SIZE_DATA_SRAM      = `$(GCC_PATH)/m68k-elf-objdump -h $(ELFOBJ) | grep -F  ".initialized_data_sram" |tr -s ' ' |cut -d " " -f4`
OFFSET_DATA_SRAM    = `$(GCC_PATH)/m68k-elf-objdump -h $(ELFOBJ) | grep -F  ".initialized_data_sram" |tr -s ' ' |cut -d " " -f7`

SIZE_CONST_SRAM     = `$(GCC_PATH)/m68k-elf-objdump -h $(ELFOBJ) | grep -F  ".initialized_constant_sram" |tr -s ' ' |cut -d " " -f4`
OFFSET_CONST_SRAM   = `$(GCC_PATH)/m68k-elf-objdump -h $(ELFOBJ) | grep -F  ".initialized_constant_sram" |tr -s ' ' |cut -d " " -f7`

SIZE_DATA_EXTRAM    = `$(GCC_PATH)/m68k-elf-objdump -h $(ELFOBJ) | grep -F  ".initialized_data_extram" |tr -s ' ' |cut -d " " -f4`
OFFSET_DATA_EXTRAM  = `$(GCC_PATH)/m68k-elf-objdump -h $(ELFOBJ) | grep -F  ".initialized_data_extram" |tr -s ' ' |cut -d " " -f7`

SIZE_CONST_EXTRAM   = `$(GCC_PATH)/m68k-elf-objdump -h $(ELFOBJ) | grep -F  ".initialized_constant_extram" |tr -s ' ' |cut -d " " -f4`
OFFSET_CONST_EXTRAM = `$(GCC_PATH)/m68k-elf-objdump -h $(ELFOBJ) | grep -F  ".initialized_constant_extram" |tr -s ' ' |cut -d " " -f7`

##############################################################################################

# C compiler options
CW_CFLAGS = -sym full -msgstyle parseable -proc 54418 -c -nostdinc -nosyspath -enc ascii $(INCDIR)\
-DALSTOM_CONFIG_$(CPU) -warn unwanted -warn extended -warn hidevirtual -warn implicitconv -warn impl_int2float \
-warn unusedarg -warn unusedvar -warn extracomma -warn emptydecl -warn structclass -opt level=0 -opt speed -ipa file \
-inline on -align coldfire -model farCode -model farData -fp soft -a6 -nocoloring -noscheduling -nopeephole \
-requireprotos -bool off -Cpp_Exceptions off -RTTI off -enum int -char unsigned -str nopool -str reuse -abi standard

GCC_CFLAGS = -g -mcpu=54418 -Wall -fno-builtin $(INCDIR) -nostdinc -fno-common -fno-guess-branch-probability
GCC_CFLAGS += -Wswitch-default -Wswitch-enum -Wunused-parameter -Wshadow -Wpointer-arith -Wbad-function-cast -Wnested-externs
GCC_CFLAGS += -Wredundant-decls -Wpointer-arith -Wshadow -mstrict-align -malign-int
GCC_CFLAGS += -Wextra -pipe -Winline -DALSTOM_CONFIG_$(CPU)
GCC_CFLAGS += -O0 -Wno-unknown-pragmas
 
#asm compiler options
CW_ASFLAGS = -sym full -msgstyle parseable -proc 54418 -nosyspath $(INCDIR) -g

GCC_ASFLAGS = $(GCC_CFLAGS)

#linker options
CW_LKFLAGS = $(TARGET_OBJS) -sym full -msgstyle parseable -proc 54418 -nostdlib $(LINKER_SCRIPT) \
-m __startup -force_active __BOOT_VECTOR_ROM -application -map -map keep -w on -o $(ELFOBJ) $(LIBS_PATHS) $(LIBS_SORT) $(CRC_OBJ)

#GCC_LKFLAGS = $(TARGET_OBJS) -nodefaultlibs -nostartfiles -nostdlib --script=$(LINKER_SCRIPT) \
#-Map $(MAPOBJ) -o $(ELFOBJ) $(LIBS_PATHS) --start-group $(LIBS_SORT) --end-group $(CRC_OBJ) -u start

GCC_LKFLAGS = $(TARGET_OBJS) $(CRC_OBJ) -nodefaultlibs -nostartfiles -nostdlib --no-accept-unknown-input-arch --fatal-warnings --no-gc-sections
GCC_LKFLAGS += -Map $(OBJ_DIR)$(TARGET).map --script=$(LINKER_SCRIPT)
GCC_LKFLAGS += -o $(ELFOBJ) $(LIBS_PATHS) --start-group $(LIBS_SORT) --end-group -u start

# objcopy flags
CW_OBFLAGS = $(TARGET_OBJS) $(CRC_OBJ) -sym full -msgstyle parseable -proc 54418 -nostdlib $(LINKER_SCRIPT) \
-m __startup -force_active __BOOT_VECTOR_ROM -application -map -map keep -srec -sreclength 252 \
-sreceol dos -w off -o $(ELFOBJ) $(LIBS_PATHS) $(LIBS_SORT) && mv $(ELFOBJ).S19 $(S19OBJ)

GCC_OBFLAGS  = --output-target srec $(ELFOBJ) $(S19OBJ)

CW_BIFLAGS = $(TARGET_OBJS) $(CRC_OBJ) -sym full -msgstyle parseable -proc 54418 -nostdlib $(LINKER_SCRIPT) \
-m __startup -force_active __BOOT_VECTOR_ROM -application -map -map keep -rbin -rbingap 0x10000 \
-w off -o $(ELFOBJ) $(LIBS_PATHS) $(LIBS_SORT) && mv $(ELFOBJ).rbin $(BINOBJ)

GCC_BIFLAGS  = --output-target binary $(ELFOBJ) $(BINOBJ)

#AR Flags listing
CW_ARFLAGS  = -nostdlib -library -o
GCC_ARFLAGS = -rcs

##############################################################################################

#Specification of compilation option with user choice

ifeq ($(COMP), GCC)
	COMPILER		= GCC
	CC			= $(GCC_CC)
	AS			= $(GCC_AS)
	AR			= $(GCC_AR)
	LK			= $(GCC_LK)
	OBJCP			= $(GCC_OBJCP)
	CFLAGS			= $(GCC_CFLAGS)
	ASFLAGS			= $(GCC_ASFLAGS)
	ARFLAGS			= $(GCC_ARFLAGS)
	LKFLAGS			= $(GCC_LKFLAGS)
	OBFLAGS			= $(GCC_OBFLAGS)
	BIFLAGS			= $(GCC_BIFLAGS)
	LINKER_SCRIPT		= $(GCC_LINKER_SCRIPT)
	PREFIX			= GCC_
	inc_opt			= -I	
	ELFOBJ			= $(GCC_ELFOBJ)
	S19OBJ			= $(GCC_S19OBJ)
	MAPOBJ			= $(GCC_MAPOBJ)
	BINOBJ			= $(GCC_BINOBJ)	
endif

ifeq ($(COMP), CW)
	COMPILER		= CW
	CC			= $(CW_CC)
	AS			= $(CW_AS)
	AR			= $(CW_AR)
	LK			= $(CW_LK)
	OBJCP			= $(CW_OBJCP)
	CFLAGS			= $(CW_CFLAGS)
	ASFLAGS			= $(CW_ASFLAGS)
	ARFLAGS			= $(CW_ARFLAGS)
	LKFLAGS			= $(CW_LKFLAGS)
	OBFLAGS			= $(CW_OBFLAGS)
	BIFLAGS			= $(CW_BIFLAGS)
	LINKER_SCRIPT		= $(CW_LINKER_SCRIPT)
	PREFIX			= CW_
	inc_opt			= -i
	ELFOBJ			= $(CW_ELFOBJ)
	S19OBJ			= $(CW_S19OBJ)
	MAPOBJ			= $(CW_MAPOBJ)
	BINOBJ			= $(CW_BINOBJ)
endif

#Nominal compilation options
ifeq ($(COMP), )
	ifeq ($(CPU), CPU_A)
		COMPILER		= CW
		CC			= $(CW_CC)
		AS			= $(CW_AS)
		AR			= $(CW_AR)
		LK			= $(CW_LK)
		OBJCP			= $(CW_OBJCP)
		CFLAGS			= $(CW_CFLAGS)
		ASFLAGS			= $(CW_ASFLAGS)
		ARFLAGS			= $(CW_ARFLAGS)
		LKFLAGS			= $(CW_LKFLAGS)
		OBFLAGS			= $(CW_OBFLAGS)
		BIFLAGS			= $(CW_BIFLAGS)
		LINKER_SCRIPT		= $(CW_LINKER_SCRIPT)
		PREFIX			= CW_
		inc_opt			= -i
		ELFOBJ			= $(CW_ELFOBJ)
		S19OBJ			= $(CW_S19OBJ)
		MAPOBJ			= $(CW_MAPOBJ)
		BINOBJ			= $(CW_BINOBJ)
	endif
	ifeq ($(CPU), CPU_B)
		COMPILER		= GCC
		CC			= $(GCC_CC)
		AS			= $(GCC_AS)
		AR			= $(GCC_AR)
		LK			= $(GCC_LK)
		OBJCP			= $(GCC_OBJCP)
		CFLAGS			= $(GCC_CFLAGS)
		ASFLAGS			= $(GCC_ASFLAGS)
		ARFLAGS			= $(GCC_ARFLAGS)
		LKFLAGS			= $(GCC_LKFLAGS)
		OBFLAGS			= $(GCC_OBFLAGS)
		BIFLAGS			= $(GCC_BIFLAGS)
		LINKER_SCRIPT		= $(GCC_LINKER_SCRIPT)
		PREFIX			= GCC_
		inc_opt			= -I	
		ELFOBJ			= $(GCC_ELFOBJ)
		S19OBJ			= $(GCC_S19OBJ)
		MAPOBJ			= $(GCC_MAPOBJ)
		BINOBJ			= $(GCC_BINOBJ)	
	endif
endif

MAKE_FLAGS := --silent
##############################################################################################
.PHONY: all_o all_e all_b all_s all

all : 
	@$(MAKE) ${MAKE_FLAGS} clean
	@$(MAKE) ${MAKE_FLAGS} -C . all_o CPU=CPU_A
	@$(MAKE) ${MAKE_FLAGS} -C . all_o CPU=CPU_B
	@$(MAKE) ${MAKE_FLAGS} -C . EMPTY_CRC CPU=CPU_A
	@$(MAKE) ${MAKE_FLAGS} -C . EMPTY_CRC CPU=CPU_B
	@$(MAKE) ${MAKE_FLAGS} -C . FIRST_LINK CPU=CPU_A
	@$(MAKE) ${MAKE_FLAGS} -C . FIRST_LINK CPU=CPU_B
	@$(MAKE) ${MAKE_FLAGS} -C . RIGHT_CRC CPU=CPU_A
	@$(MAKE) ${MAKE_FLAGS} -C . RIGHT_CRC CPU=CPU_B
	@$(MAKE) ${MAKE_FLAGS} -C . FINAL_LINK CPU=CPU_A
	@$(MAKE) ${MAKE_FLAGS} -C . FINAL_LINK CPU=CPU_B
	@$(MAKE) ${MAKE_FLAGS} done CPU=CPU_A
	@$(MAKE) ${MAKE_FLAGS} done CPU=CPU_B

	
all_o : $(OBJECTS)
	@echo "---------- $(CPU) $(MODULE_NAME) Done "
	@mv $(OBJECTS) $(OBJ_DIR)
	
all_e : 
	@echo "Linking... " $<
	@$(LK) $(LKFLAGS)

all_b : 
	@echo "Generate Bin... " $<
	@$(OBJCP) $(BIFLAGS)
	
all_s : 
	@echo "Generate S19... " $<
	@$(OBJCP) $(OBFLAGS)
##############################################################################################

#General rules for compiling/assembling source code
$(PREFIX)%.o : %.s
	@echo "Compile " $<
	@$(AS) -c $(ASFLAGS) -o $@ $< 

$(PREFIX)%.o : %.S
	@echo "Compile " $<
	@$(AS) -c $(ASFLAGS) -o $@ $< 
	
$(PREFIX)%.o : %.c
	@echo "Compile " $<
	@$(CC) -c $(CFLAGS) -o $@ $< 

	
	
##############################################################################################	
#Generic lib
.PHONY: local_post_build

local_post_build: $(LIB_NAME)
	@echo "Build $(CPU) library $(COMPILER)..."
$(LIB_NAME): $(KERN_OBJS)
	@$(AR) $(ARFLAGS) $(LIB_NAME) $^

##############################################################################################	
.PHONY: EMPTY_CRC RIGHT_CRC FIRST_LINK FINAL_LINK
	
FIRST_LINK: 
	@$(AS) -c $(ASFLAGS) -o $(CRC_OBJ) $(CRC_SOURCE)
	@echo "Linking for $(CPU)... " $<
	@$(LK) $(LKFLAGS)

FINAL_LINK:
#	@echo "$(CPU) Update... " $<
	@rm -rf $(ELFOBJ)
	@rm -rf $(MAPOBJ)
	@rm -f $(CRC_OBJ)
	@$(AS) -c $(ASMFLAGS) -o $(CRC_OBJ) $(FINAL_CRC_SOURCE)

	@echo "Linking... " $<
	@$(LK) $(LKFLAGS)
	
ifeq ($(TYPE), ROM)	
	@echo "Generate Bin... " $<
	@$(OBJCP) $(BIFLAGS)

	@echo "Generate S19 for $(CPU)... " $<
	@$(OBJCP) $(OBFLAGS)
else
	@echo " RAM file only, no bin/S19 Object generated for $(CPU)"
endif
	
EMPTY_CRC:
	@ mkdir -p $(dir $(CRC_SOURCE))
	@cat ./crc_make/basic_crc_result.S > $(CRC_SOURCE)
	@echo "_p_CPU_A_Crc_boot:" >> $(CRC_SOURCE)
	@echo "	.long 00000000" >> $(CRC_SOURCE)
	@echo "_p_CPU_A_Crc_vector:" >> $(CRC_SOURCE)
	@echo "	.long 00000000" >> $(CRC_SOURCE)
	@echo "_p_CPU_A_Crc_code:" >> $(CRC_SOURCE)
	@echo "	.long 00000000" >> $(CRC_SOURCE)
	@echo "_p_CPU_A_Crc_code_sram:" >> $(CRC_SOURCE)
	@echo "	.long 00000000" >> $(CRC_SOURCE)
	@echo "_p_CPU_A_Crc_sram:" >> $(CRC_SOURCE)
	@echo "	.long 00000000" >> $(CRC_SOURCE)
	@echo "_p_CPU_A_Crc_const_sram:" >> $(CRC_SOURCE)
	@echo "	.long 00000000" >> $(CRC_SOURCE)
	@echo "_p_CPU_A_Crc_ext_sdram:" >> $(CRC_SOURCE)
	@echo "	.long 00000000" >> $(CRC_SOURCE)
	@echo "_p_CPU_A_Crc_const_ext_sdram:" >> $(CRC_SOURCE)
	@echo "	.long 00000000" >> $(CRC_SOURCE)

	@echo "_p_CPU_B_Crc_boot:" >> $(CRC_SOURCE)
	@echo "	.long 00000000" >> $(CRC_SOURCE)
	@echo "_p_CPU_B_Crc_vector:" >> $(CRC_SOURCE)
	@echo "	.long 00000000" >> $(CRC_SOURCE)
	@echo "_p_CPU_B_Crc_code:" >> $(CRC_SOURCE)
	@echo "	.long 00000000" >> $(CRC_SOURCE)
	@echo "_p_CPU_B_Crc_code_sram:" >> $(CRC_SOURCE)
	@echo "	.long 00000000" >> $(CRC_SOURCE)
	@echo "_p_CPU_B_Crc_sram:" >> $(CRC_SOURCE)
	@echo "	.long 00000000" >> $(CRC_SOURCE)
	@echo "_p_CPU_B_Crc_const_sram:" >> $(CRC_SOURCE)
	@echo "	.long 00000000" >> $(CRC_SOURCE)
	@echo "_p_CPU_B_Crc_ext_sdram:" >> $(CRC_SOURCE)
	@echo "	.long 00000000" >> $(CRC_SOURCE)
	@echo "_p_CPU_B_Crc_const_ext_sdram:" >> $(CRC_SOURCE)
	@echo "	.long 00000000" >> $(CRC_SOURCE)	
		
RIGHT_CRC:

ifeq ($(CPU), CPU_A)
	@ mkdir -p $(dir $(FINAL_CRC_SOURCE))
	@cat ./crc_make/basic_crc_result.S > $(FINAL_CRC_SOURCE)
endif
	@echo "_p_$(CPU)_Crc_boot:" >> $(FINAL_CRC_SOURCE)
	@$(CRC_FUNC) $(ELFOBJ) $(SIZE_BOOT) $(OFFSET_BOOT) >> $(FINAL_CRC_SOURCE)

	@echo "_p_$(CPU)_Crc_vector:" >> $(FINAL_CRC_SOURCE)
	@$(CRC_FUNC) $(ELFOBJ) 400 $(OFFSET_VECTOR) >> $(FINAL_CRC_SOURCE)

	@echo "_p_$(CPU)_Crc_code:" >> $(FINAL_CRC_SOURCE)
	@$(CRC_FUNC) $(ELFOBJ) $(SIZE_CODE) $(OFFSET_CODE) >> $(FINAL_CRC_SOURCE)
	@echo "_p_$(CPU)_Crc_code_sram:" >> $(FINAL_CRC_SOURCE)
	@$(CRC_FUNC) $(ELFOBJ) $(SIZE_CODE_SRAM) $(OFFSET_CODE_SRAM) >> $(FINAL_CRC_SOURCE)
	@echo "_p_$(CPU)_Crc_sram:" >> $(FINAL_CRC_SOURCE)
	@$(CRC_FUNC) $(ELFOBJ) $(SIZE_DATA_SRAM) $(OFFSET_DATA_SRAM) >> $(FINAL_CRC_SOURCE)
	@echo "_p_$(CPU)_Crc_const_sram:" >> $(FINAL_CRC_SOURCE)
	@$(CRC_FUNC) $(ELFOBJ) $(SIZE_CONST_SRAM) $(OFFSET_CONST_SRAM) >> $(FINAL_CRC_SOURCE)
	@echo "_p_$(CPU)_Crc_ext_sdram:" >> $(FINAL_CRC_SOURCE)
	@$(CRC_FUNC) $(ELFOBJ) $(SIZE_DATA_EXTRAM) $(OFFSET_DATA_EXTRAM) >> $(FINAL_CRC_SOURCE)
	@echo "_p_$(CPU)_Crc_const_ext_sdram:" >> $(FINAL_CRC_SOURCE)
	@$(CRC_FUNC) $(ELFOBJ) $(SIZE_CONST_EXTRAM) $(OFFSET_CONST_EXTRAM) >> $(FINAL_CRC_SOURCE)
	
	@echo "Software data identification for "$(CPU)": 0x"$(shell printf "%x" $$((\
		$(shell $(CRC_FUNC) $(ELFOBJ) $(SIZE_BOOT) $(OFFSET_BOOT) | sed -e "s/.\.long \([0-9]*\).*/\1/") ^\
		$(shell $(CRC_FUNC) $(ELFOBJ) $(SIZE_CODE) $(OFFSET_CODE) | sed -e "s/.\.long \([0-9]*\).*/\1/") ^\
		$(shell $(CRC_FUNC) $(ELFOBJ) $(SIZE_CODE_SRAM) $(OFFSET_CODE_SRAM) | sed -e "s/.\.long \([0-9]*\).*/\1/") ^\
		$(shell $(CRC_FUNC) $(ELFOBJ) $(SIZE_DATA_SRAM) $(OFFSET_DATA_SRAM) | sed -e "s/.\.long \([0-9]*\).*/\1/") ^\
		$(shell $(CRC_FUNC) $(ELFOBJ) $(SIZE_CONST_SRAM) $(OFFSET_CONST_SRAM) | sed -e "s/.\.long \([0-9]*\).*/\1/") ^\
		$(shell $(CRC_FUNC) $(ELFOBJ) $(SIZE_DATA_EXTRAM) $(OFFSET_DATA_EXTRAM) | sed -e "s/.\.long \([0-9]*\).*/\1/") ^\
		$(shell $(CRC_FUNC) $(ELFOBJ) $(SIZE_CONST_EXTRAM) $(OFFSET_CONST_EXTRAM) | sed -e "s/.\.long \([0-9]*\).*/\1/")))) | tee ./out/$(CPU)/software_data_identification_$(CPU).txt	
		
##############################################################################################		
.PHONY: display
display:
	@echo "------------------------------------------------------------------"
	@echo -e "\033[1;34mCPU\033[0m        	: $(CPU)"
	@echo -e "\033[1;34mTYPE\033[0m       	: $(TYPE)"
	@echo -e "\033[1;34mTARGET_DIR\033[0m 	: $(OBJ_DIR)"
	@echo -e "\033[1;34mMODULE_NAME\033[0m	: $(MODULE_NAME)"
	@echo -e "\033[1;34mTARGET_OBJS\033[0m	: $(TARGET_OBJS)"
	@echo -e "\033[1;34mTARGET_SOURCE\033[0m	: $(ALLMODULE)"
	@echo "------------------------------------------------------------------"
	@echo -e "\033[1;34mELFOBJ\033[0m 		: $(ELFOBJ)"
	@echo -e "\033[1;34mS19OBJ\033[0m 		: $(S19OBJ)"
	@echo -e "\033[1;34mMAPOBJ\033[0m   	: $(MAPOBJ)"
	@echo -e "\033[1;34mBINOBJ\033[0m 		: $(BINOBJ)"
	@echo -e "\033[1;34mLINKER_SCRIPT\033[0m 	: $(LINKER_SCRIPT)"
	@echo "------------------------------------------------------------------"
	@echo -e "\033[1;34mLIBRARIES\033[0m 	: $(LIBRARIES)"	
	@echo -e "\033[1;34mLIBS\033[0m 		: $(LIBS)"	
	@echo -e "\033[1;34mLIB_PATHS\033[0m 	: $(LIBS_PATHS)"
	@echo -e "\033[1;34mLIB_NAME\033[0m 	: $(LIB_NAME)"
	@echo -e "\033[1;34mLIBS_SORT\033[0m 	: $(LIBS_SORT)"
	@echo "------------------------------------------------------------------"

##############################################################################################			
.PHONY: done
done:

	@echo "*** $(MODULE_NAME) $(CPU) Done ***"
	@echo "*** Target is in $(OBJ_DIR) ***"

##############################################################################################			
.PHONY:	clean 
clean:
	@mkdir -p ./out/CPU_A
	@mkdir -p ./out/CPU_B
	@echo "cleanall..."
	@rm -rf out/CPU_A/*
	@ls ./out/CPU_A
	@rm -rf out/CPU_B/*
	@ls ./out/CPU_B
	@rm -rf ./out/final_crc_result.S	
	
help:
	@echo "------------------------------------------------------------------"
	@echo -e "\033[1;34mThe following Rules are available:\033[0m"
	@echo "all	:Build all objects and link ROM for CPU_A in CW & CPU_B in GCC"
	@echo "clean	:Remove all objects and generated binaries for all CPU/TYPES"
	@echo "display	:Display internal variable of the makefile (debug make purpose)"	
	@echo -e "\033[1;34mThis are the options used in guest make.sh:\033[0m"
	@echo "TYPE=RAM or ROM: Define target build type for forced type."
	@echo "	Default : target build type is ROM"
	@echo "COMP=GCC or CW : Force the selection of one compiler."
	@echo "	Default : CPU_A compiled with CW & CPU_B with GCC"
	@echo "------------------------------------------------------------------"
	@echo -e "\033[1;34mExamples make all_CW:\033[0m"
	@echo "	  make all_o CPU=CPU_A "
	@echo "	  make all_o CPU=CPU_B COMP=CW "
	@echo "	  make EMPTY_CRC CPU=CPU_A "
	@echo "	  make EMPTY_CRC CPU=CPU_B COMP=CW "
	@echo "	  make FIRST_LINK CPU=CPU_A "
	@echo "	  make FIRST_LINK CPU=CPU_B COMP=CW"
	@echo "	  make RIGHT_CRC CPU=CPU_A"
	@echo "	  make RIGHT_CRC CPU=CPU_B COMP=CW"
	@echo "	  make FINAL_LINK CPU=CPU_A"
	@echo "	  make FINAL_LINK CPU=CPU_B COMP=CW"
	@echo "	  make done CPU=CPU_A"
	@echo "	  make done CPU=CPU_B COMP=CW"
	@echo "------------------------------------------------------------------"
	@echo -e "\033[1;34mExamples make all_GCC:\033[0m"
	@echo "	  make all_o CPU=CPU_A COMP=GCC"
	@echo "	  make all_o CPU=CPU_B "
	@echo "	  make EMPTY_CRC CPU=CPU_A COMP=GCC "
	@echo "	  make EMPTY_CRC CPU=CPU_B "
	@echo "	  make FIRST_LINK CPU=CPU_A COMP=GCC "
	@echo "	  make FIRST_LINK CPU=CPU_B"
	@echo "	  make RIGHT_CRC CPU=CPU_A COMP=GCC"
	@echo "	  make RIGHT_CRC CPU=CPU_B"
	@echo "	  make FINAL_LINK CPU=CPU_A COMP=GCC"
	@echo "	  make FINAL_LINK CPU=CPU_B"
	@echo "	  make done CPU=CPU_A COMP=GCC"
	@echo "	  make done CPU=CPU_B"
	@echo "------------------------------------------------------------------"
# 
# Include the dependency files, should be the last of the makefile
#
#-include $(shell mkdir .dep 2> nul) $(wildcard .dep/*)


# *** EOF ***