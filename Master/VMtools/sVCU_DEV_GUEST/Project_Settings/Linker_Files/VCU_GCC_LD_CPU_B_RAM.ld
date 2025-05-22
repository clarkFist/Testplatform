 /*####################################################################################
 # Copyright � 2018 CASCO Signal. All rights reserved.                          #
 #                                                                                  #
 #                                                                                  #
 #    Date: 2018                                                                    #
 #                                                                                  #
 # DESCRIPTION                                                                      #
 #   Linker command file for ram-console target.                                    #
 #                                                                                  #
 #                                                                                  #
 #                                                                                  #
 # NOTE                                                                             #
 #                                                                                  #
 #																					#
 #                                                                                  #
 # HISTORY                                                                          #
 # 1. created by J.s.,20180103.                                                     #
 ####################################################################################*/

MEMORY
{
        /* No symbol should be put into this region. If it is used, it
        means a symmbol is not correctly declared */
        dummy      		(RWX) : ORIGIN = 0x00000000, LENGTH = 0x00000001
        vector_ram 		(RWX) : ORIGIN = 0x81000000, LENGTH = 0x00000500
        sram       		(RWX) : ORIGIN = 0x81000000, LENGTH = 0x00010000
        sram_user               (RWX) : ORIGIN = 0x81000500, LENGTH = 0x0000F700
        sram_stack_boot         (RWX) : ORIGIN = 0x8100FC00, LENGTH = 0x00000400
        ext_sdram_data 	(RWX) : ORIGIN = 0x44000000, LENGTH = 0x01000000 /*# 0x01000000 = 16M ,0x04000000 = 64MB*/ 
       /* ipsbar     		(RWX) : ORIGIN = 0x40000000, LENGTH = 0x00000001*/
		ext_flash   (RX)    : ORIGIN = 0x20000000, LENGTH = 0x00320000
		ext_flash_ES   (RX) : ORIGIN = 0x20320000, LENGTH = 0x00CE0000
        ext_sdram_code 	(RWX) : ORIGIN = 0x40000000, LENGTH = 0x00800000 /*# 0x00800000 = 8M ,0x04000000 = 64MB*/
}

/* MANDATORY TO ENSURE THAT BOOT VECTOR AND ENTRY POINT ARE FIND BY THE LINKER
 * Warning the GCC linker is not verbose and removing this line will result in
 * complexe behaviour
 */
ENTRY(_startup)

SECTIONS
{
        .vector_ram : {} > vector_ram
     /*   .ipsbar     : {} > ipsbar*/
        .sram       : {} > sram
        .ext_sdram_data   : {} > ext_sdram_data
        .ext_flash  : {} > ext_flash
        .sram_user  : {} > sram_user
        .ext_sdram_code : {} > ext_sdram_code
	.ext_flash_ES : {} > ext_flash_ES

        .boot :
        {
                __START_VECTOR_ROM = .;
                *(.textvectors)
                *(.rodatavectors)
                __END_VECTOR_ROM = .;

                __START_BOOT_ROM = .;
                *(.textboot)
                *(.rodataboot)
                *(.rodataboot.str1.1)
                . = ALIGN (0x4);
                __END_BOOT_ROM = .;
        } > ext_sdram_code

        .crc :
        {
                __CRC_FLASH_START = .;
                __CRC_ROM_START   = .;
                *(.CRC)
                . = ALIGN (0x4);
                __CRC_ROM_END   = .;
        } > ext_sdram_code

        .code :
        {
                __CODE_FLASH_START = .;
                __START_CODE_ROM   = .;
                *(EXCLUDE_FILE(*_boot.a) .text)
                *(EXCLUDE_FILE(*_boot.a) .code)
                *(.rodata.str1.1)
                . = ALIGN (0x4);
                __END_CODE_ROM = .;
        } > ext_sdram_code

        .sram_code :
        {
	        __CODE_SRAM_FLASH_START = .;
                __START_CODE_SRAM = .;
                *(.textsram)
                . = ALIGN (0x4);
                __END_CODE_SRAM = .;
        } > sram_user

        .initialized_data_sram :
        {
                __DATA_SRAM_FLASH_START = .;
                __DATA_RAM_START        = .;
                __DATA_SRAM_START       = .;
                *(.dataos)
                *(.sdataos)
                *(.idata_sram)
		*(.databoot)
                . = ALIGN (0x4);
		__NO_TEST_SRAM_INIT_SECTION_START = .;
        	*(.idata_no_memtest_sram)
        	. = ALIGN (0x4);
        	__NO_TEST_SRAM_INIT_SECTION_END = .;
                __DATA_SRAM_END = .;
        } > sram_user

        .initialized_constant_sram :
        {
                __CONST_SRAM_FLASH_START = .;
                __CONST_SRAM_START       = .;
                *(.rodataos)
                . = ALIGN (0x4);
                __CONST_SRAM_END = .;
                __DATA_END = .;
        } > sram_user

       .uninitialized_data_sram :
       {
                __BSS_START = .;
                *(.bssos)
                *(.sbssos)
                __BSS_MIDDLE = .;
       } > sram_user

       .uninitialized_data_sram_force (NOLOAD):
       {
		__BSS_FORCE_SRAM_START = .;
                *(.udata_sram)

                . = ALIGN (0x4);
		__NO_TEST_SRAM_UNINIT_SECTION_START = .;
        	*(.udata_no_memtest_sram)
        	. = ALIGN (0x4);
        	__NO_TEST_SRAM_UNINIT_SECTION_END = .;
                __BSS_END = .;

                ___HEAP_START = .;
                .             = . + 0x0000;
                ___HEAP_END     = .;

                . = ALIGN (0x4);
        } > sram_user
        .uninitialized_data_stack_boot :
        {
                ___SP_END = .;
                *(.bootstack)

                ___SP_INIT = .;
		            __BSS_FORCE_SRAM_END = .;
        } > sram_stack_boot

        /* Default is to EXT RAM */
        .initialized_data_extram :
        {
                __DATA_EXT_FLASH_START = .;
                __DATA_EXT_SDRAM_DATA_START      = .;
                __DATA_EXT_START = .;
                *(.data)
                *(.sdata)

                . = ALIGN (0x4);
		__DCA_INIT_SECTION_START = .;
        	*(.idata_dca)
        	. = ALIGN (0x4);
        	__DCA_INIT_SECTION_END = .;

                . = ALIGN (0x4);
		__NO_TEST_INIT_SECTION_START = .;
        	*(.idata_no_memtest)
        	. = ALIGN (0x4);
        	__NO_TEST_INIT_SECTION_END = .;

                __DATA_EXT_SDRAM_DATA_END = .;
        } > ext_sdram_data

        .initialized_constant_extram :
        {
                __CONST_EXT_FLASH_START = .;
                __CONST_EXT_SDRAM_CODE_START      = .;
                *(.rodata)
                . = ALIGN (0x4);
                __CONST_EXT_SDRAM_CODE_END = .;
                __DATA_EXT_END      = .;

        } > ext_sdram_code



        /* Default is to Ext RAM */
        .uninitialized_data_extram :
        {
                __BSS_EXT_SDRAM_DATA_START = .;
                *(.bss)
                *(COMMON)
                *(.sbss)
                *(SCOMMON)
                . = ALIGN (0x4);
		__BSS_EXT_SDRAM_DATA_MIDDLE = .;
        } > ext_sdram_data

        .uninitialized_data_extram_dca (NOLOAD) :
	{
		__DCA_UNINIT_SECTION_START = .;
       		*(.udata_dca)
       		. = ALIGN (0x4);
       		__DCA_UNINIT_SECTION_END = .;
        } > ext_sdram_data

        .uninitialized_data_extram_no_memtest (NOLOAD) :
	{
		__NO_TEST_UNINIT_SECTION_START = .;
       		*(.udata_no_memtest)
       		. = ALIGN (0x4);
       		__NO_TEST_UNINIT_SECTION_END = .;
                __BSS_EXT_SDRAM_DATA_END = .;
        } > ext_sdram_data

    /*    __IPSBAR            = ADDR(.ipsbar);
        ___IPSBAR           = __IPSBAR;*/

        __VECTOR_RAM        = ADDR(.vector_ram);
        __VECTOR_RAM_END    = __VECTOR_RAM + 0x400;

        __SDRAM             = 0;

        __SRAM              = ADDR(.sram);
        ___SRAM             = __SRAM;

        __EXT_SDRAM_DATA          = ADDR(.ext_sdram_data);

        __EXT_SDRAM_CODE     = ADDR(.ext_sdram_code);
        __SRAM_CODE		   = ADDR(.sram_code);

        __EXT_FLASH         = ADDR(.ext_flash);
        ___EXT_FLASH        = __EXT_FLASH;
	
	 /*add from ROM*/
	__EXT_FLASH_END_USE   = __CONST_EXT_FLASH_START + (__CONST_EXT_SDRAM_CODE_END - __CONST_EXT_SDRAM_CODE_START); 
        ___EXT_FLASH_END_USE  = __EXT_FLASH_END_USE;

        __SP_INIT           = ___SP_INIT;

	__EXT_FLASH_ES         = ADDR(.ext_flash_ES);
	__SRAM = ___SRAM;
	__SP_END              = ___SP_END;
}
