/*fichier assembleur de stockage des CRC*/

#ifdef __GNUC__
#define _p_CPU_A_Crc_boot			p_CPU_A_Crc_boot
#define _p_CPU_A_Crc_vector			p_CPU_A_Crc_vector
#define _p_CPU_A_Crc_code			p_CPU_A_Crc_code
#define _p_CPU_A_Crc_code_sram			p_CPU_A_Crc_code_sram
#define _p_CPU_A_Crc_sram			p_CPU_A_Crc_sram
#define _p_CPU_A_Crc_const_sram		p_CPU_A_Crc_const_sram
#define _p_CPU_A_Crc_ext_sdram		p_CPU_A_Crc_ext_sdram
#define _p_CPU_A_Crc_const_ext_sdram	p_CPU_A_Crc_const_ext_sdram

#define _p_CPU_B_Crc_boot			p_CPU_B_Crc_boot
#define _p_CPU_B_Crc_vector			p_CPU_B_Crc_vector
#define _p_CPU_B_Crc_code			p_CPU_B_Crc_code
#define _p_CPU_B_Crc_code_sram			p_CPU_B_Crc_code_sram
#define _p_CPU_B_Crc_sram			p_CPU_B_Crc_sram
#define _p_CPU_B_Crc_const_sram		p_CPU_B_Crc_const_sram
#define _p_CPU_B_Crc_ext_sdram		p_CPU_B_Crc_ext_sdram
#define _p_CPU_B_Crc_const_ext_sdram	p_CPU_B_Crc_const_ext_sdram
#endif

	 .global _p_CPU_A_Crc_boot, _p_CPU_A_Crc_vector, _p_CPU_A_Crc_code, _p_CPU_A_Crc_code_sram, _p_CPU_A_Crc_sram, _p_CPU_A_Crc_const_sram, _p_CPU_A_Crc_ext_sdram, _p_CPU_A_Crc_const_ext_sdram
	 .global _p_CPU_B_Crc_boot, _p_CPU_B_Crc_vector, _p_CPU_B_Crc_code, _p_CPU_B_Crc_code_sram, _p_CPU_B_Crc_sram, _p_CPU_B_Crc_const_sram, _p_CPU_B_Crc_ext_sdram, _p_CPU_B_Crc_const_ext_sdram

	.section .CRC, "a"
	.align 1
_p_CPU_A_Crc_boot:
	.long 567038509
_p_CPU_A_Crc_vector:
	.long 3241499107
_p_CPU_A_Crc_code:
	.long 3091957800
_p_CPU_A_Crc_code_sram:
	.long 2127266118
_p_CPU_A_Crc_sram:
	.long 2606881150
_p_CPU_A_Crc_const_sram:
	.long 2921652381
_p_CPU_A_Crc_ext_sdram:
	.long 48579091
_p_CPU_A_Crc_const_ext_sdram:
	.long 1475094166
_p_CPU_B_Crc_boot:
	.long 3209820844
_p_CPU_B_Crc_vector:
	.long 82386522
_p_CPU_B_Crc_code:
	.long 534738423
_p_CPU_B_Crc_code_sram:
	.long 3673409000
_p_CPU_B_Crc_sram:
	.long 4031239211
_p_CPU_B_Crc_const_sram:
	.long 2921652381
_p_CPU_B_Crc_ext_sdram:
	.long 2640351662
_p_CPU_B_Crc_const_ext_sdram:
	.long 3696305955
