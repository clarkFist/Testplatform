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
	.long 450366837
_p_CPU_A_Crc_vector:
	.long 3963518749
_p_CPU_A_Crc_code:
	.long 930813538
_p_CPU_A_Crc_code_sram:
	.long 2875466304
_p_CPU_A_Crc_sram:
	.long 4126272587
_p_CPU_A_Crc_const_sram:
	.long 2921652381
_p_CPU_A_Crc_ext_sdram:
	.long 3679054365
_p_CPU_A_Crc_const_ext_sdram:
	.long 3966451614
_p_CPU_B_Crc_boot:
	.long 2737195938
_p_CPU_B_Crc_vector:
	.long 2597580755
_p_CPU_B_Crc_code:
	.long 2242782829
_p_CPU_B_Crc_code_sram:
	.long 2233063559
_p_CPU_B_Crc_sram:
	.long 2948870029
_p_CPU_B_Crc_const_sram:
	.long 2921652381
_p_CPU_B_Crc_ext_sdram:
	.long 2632850679
_p_CPU_B_Crc_const_ext_sdram:
	.long 145449558
