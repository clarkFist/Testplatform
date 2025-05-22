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
	.long 1155408385
_p_CPU_A_Crc_vector:
	.long 1549863526
_p_CPU_A_Crc_code:
	.long 2097471418
_p_CPU_A_Crc_code_sram:
	.long 2017614982
_p_CPU_A_Crc_sram:
	.long 2123865749
_p_CPU_A_Crc_const_sram:
	.long 2921652381
_p_CPU_A_Crc_ext_sdram:
	.long 4031623085
_p_CPU_A_Crc_const_ext_sdram:
	.long 499198272
_p_CPU_B_Crc_boot:
	.long 1212517120
_p_CPU_B_Crc_vector:
	.long 879687224
_p_CPU_B_Crc_code:
	.long 2745817242
_p_CPU_B_Crc_code_sram:
	.long 1065226387
_p_CPU_B_Crc_sram:
	.long 925330434
_p_CPU_B_Crc_const_sram:
	.long 2921652381
_p_CPU_B_Crc_ext_sdram:
	.long 1547091109
_p_CPU_B_Crc_const_ext_sdram:
	.long 1164686648
