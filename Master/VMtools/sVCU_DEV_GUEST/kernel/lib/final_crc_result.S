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
	.long 1397826929
_p_CPU_A_Crc_vector:
	.long 179611555
_p_CPU_A_Crc_code:
	.long 494053460
_p_CPU_A_Crc_code_sram:
	.long 1486179579
_p_CPU_A_Crc_sram:
	.long 604132696
_p_CPU_A_Crc_const_sram:
	.long 2921652381
_p_CPU_A_Crc_ext_sdram:
	.long 2783329295
_p_CPU_A_Crc_const_ext_sdram:
	.long 4082967434
_p_CPU_B_Crc_boot:
	.long 4206503441
_p_CPU_B_Crc_vector:
	.long 1482961555
_p_CPU_B_Crc_code:
	.long 876208149
_p_CPU_B_Crc_code_sram:
	.long 3810912520
_p_CPU_B_Crc_sram:
	.long 2304339160
_p_CPU_B_Crc_const_sram:
	.long 2921652381
_p_CPU_B_Crc_ext_sdram:
	.long 3330012728
_p_CPU_B_Crc_const_ext_sdram:
	.long 2985024426
