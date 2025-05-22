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
	.long 1225826720
_p_CPU_A_Crc_vector:
	.long 3171593553
_p_CPU_A_Crc_code:
	.long 3707155209
_p_CPU_A_Crc_code_sram:
	.long 2723598350
_p_CPU_A_Crc_sram:
	.long 2158951211
_p_CPU_A_Crc_const_sram:
	.long 2921652381
_p_CPU_A_Crc_ext_sdram:
	.long 1272958394
_p_CPU_A_Crc_const_ext_sdram:
	.long 1237497198
_p_CPU_B_Crc_boot:
	.long 1045726710
_p_CPU_B_Crc_vector:
	.long 3053931861
_p_CPU_B_Crc_code:
	.long 1394535591
_p_CPU_B_Crc_code_sram:
	.long 3328317267
_p_CPU_B_Crc_sram:
	.long 2729670445
_p_CPU_B_Crc_const_sram:
	.long 2921652381
_p_CPU_B_Crc_ext_sdram:
	.long 2297401606
_p_CPU_B_Crc_const_ext_sdram:
	.long 3547578540
