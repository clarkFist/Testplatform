/**
* @file GM_Error.h
* @brief 定义通用模块的返回值错误号
* @author Xavier
* @date 2013-7-29
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: </b>xiawei@casco.com.cn</pre>
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/
#ifndef _GM_ERROR_H
#define _GM_ERROR_H

/*************************************************************************/
/*              GM_Utils错误号部分，定义为0xA0XXXXXX，共462个            */
/* 例如某个错误号为0xA0010203,其中A0为GM_Utils部分的错误主分类号，01用来 */
/* 定位GM_Utils中的模块，02用来定位对应的函数，03表示函数内部的错误类型  */
/*************************************************************************/

/******************FILE部分,错误号为0xA001XXXX****************/
/**GM_Bin_File_Load(),0xA00101XX**/
#define  GM_BIN_FILE_LOAD_PAR_ERR		0xA0010100U			/*GM_Bin_File_Load传入参数有误*/
#define  GM_BIN_FILE_LOAD_OPEN_ERR		0xA0010101U			/*GM_Bin_File_Load打开文件失败*/
#define  GM_BIN_FILE_LOAD_LEN_ERR		0xA0010102U			/*GM_Bin_File_Load文件长度错误*/
#define  GM_BIN_FILE_LOAD_READ_ERR		0xA0010103U			/*GM_Bin_File_Load读取内容失败*/
#define  GM_BIN_FILE_LOAD_CLOSE_ERR		0xA0010104U			/*GM_Bin_File_Load关闭文件失败*/

/**GM_Ini_File_Partly_Load(),0xA00102XX**/
#define  GM_INI_FILE_PARTLY_LOAD_PAR_ERR		0xA0010200U			/*GM_Ini_File_Partly_Load传入参数有误*/
#define  GM_INI_FILE_PARTLY_LOAD_OPEN_ERR		0xA0010201U			/*GM_Ini_File_Partly_Load打开文件失败*/
#define  GM_INI_FILE_PARTLY_LOAD_CONTENT_ERR	0xA0010202U			/*GM_Ini_File_Partly_Load读取{中内容过多*/
#define  GM_INI_FILE_PARTLY_LOAD_BUFFER_ERR		0xA0010203U			/*GM_Ini_File_Partly_Load出入的buffer不够大*/
#define  GM_INI_FILE_PARTLY_LOAD_CLOSE_ERR		0xA0010204U			/*GM_Ini_File_Partly_Load关闭文件失败*/
#define  GM_INI_FILE_PARTLY_LOAD_NOT_FIND_ERR	0xA0010205U			/*GM_Ini_File_Partly_Load未找到对应内容*/

/**GM_Ini_File_Load(),0xA00103XX**/
#define  GM_INI_FILE_LOAD_PAR_ERR			0xA0010300U			/*GM_Ini_File_Load传入参数有误*/
#define  GM_INI_FILE_LOAD_OPEN_ERR			0xA0010301U			/*GM_Ini_File_Load打开文件失败*/
#define  GM_INI_FILE_LOAD_CONTENT_ERR		0xA0010302U			/*GM_Ini_File_Load读取内容过多*/
#define  GM_INI_FILE_LOAD_CLOSE_ERR			0xA0010303U			/*GM_Ini_File_Load关闭文件失败*/

/**GM_RSSP1_Ini_File_Read_String()，0xA00104XX**/
#define  GM_INI_FILE_READ_STRING_PAR_ERR		0xA0010400U			/*GM_RSSP1_Ini_File_Read_String传入参数有误*/
#define  GM_INI_FILE_READ_STRING_NOT_FIND_ERR	0xA0010401U			/*GM_RSSP1_Ini_File_Read_String没有找到key*/
#define  GM_INI_FILE_READ_STRING_SIZE_ERR		0xA0010402U			/*GM_RSSP1_Ini_File_Read_String传入的size太小*/

/**GM_Ini_File_Read_UInt(),0xA00105XX**/
#define  GM_INI_FILE_READ_UINT_PAR_ERR			0xA0010500U			/*GM_Ini_File_Read_UInt传入参数有误*/
#define  GM_INI_FILE_READ_UINT_STRING_ERR		0xA0010501U			/*GM_RSSP1_Ini_File_Read_String返回失败*/
#define  GM_INI_FILE_READ_UINT_ATOUI_ERR		0xA0010502U			/*GM_atoui返回失败*/

/**GM_RSSP1_Ini_File_Read_Int(),0xA00106XX**/
#define  GM_INI_FILE_READ_INT_PAR_ERR			0xA0010600U			/*GM_RSSP1_Ini_File_Read_Int传入参数有误*/
#define  GM_INI_FILE_READ_INT_STRING_ERR		0xA0010601U			/*GM_RSSP1_Ini_File_Read_String返回失败*/
#define  GM_INI_FILE_READ_INT_ATOI_ERR			0xA0010602U			/*GM_atoi返回失败*/

/**GM_Ini_File_Read_BOOL(),0xA00107XX**/
#define  GM_INI_FILE_READ_BOOL_PAR_ERR			0xA0010700U			/*GM_Ini_File_Read_BOOL传入参数有误*/
#define  GM_INI_FILE_READ_BOOL_VALUE_ERR		0xA0010701U			/*读出的值不是BOOL类型*/
#define  GM_INI_FILE_READ_BOOL_READ_ERR			0xA0010702U			/*GM_RSSP1_Ini_File_Read_Int返回失败*/

/**GM_Ini_File_Read_Hex(),0xA00108XX**/
#define  GM_INI_FILE_READ_HEX_PAR_ERR			0xA0010800U			/*GM_Ini_File_Read_Hex传入参数有误*/
#define  GM_INI_FILE_READ_HEX_PARSE_ERR			0xA0010801U			/*GM_Ini_File_Parse返回失败*/
#define  GM_INI_FILE_READ_HEX_HEX_PARSE_ERR		0xA0010802U			/*GM_Ini_File_Read_Hex_Parse返回失败*/

/**GM_Ini_File_Read_Hex_32(),0xA00109XX**/
#define  GM_INI_FILE_READ_HEX_32_PAR_ERR		0xA0010900U			/*GM_Ini_File_Read_Hex_32传入参数有误*/
#define  GM_INI_FILE_READ_HEX_32_PARSE_ERR		0xA0010901U			/*GM_Ini_File_Parse返回失败*/
#define  GM_INI_FILE_READ_HEX_32_HEX_PARSE_ERR	0xA0010902U			/*GM_Ini_File_Read_Hex_32_Parse返回失败*/

/**GM_Ini_File_Read_IntORHex8(),0xA0010AXX**/
#define  GM_INI_FILE_READ_INTORHEX8_PAR_ERR		   0xA0010A00U			/*GM_Ini_File_Read_IntORHex8传入参数有误*/
#define  GM_INI_FILE_READ_INTORHEX8_PARSE_ERR	   0xA0010A01U			/*GM_Ini_File_Parse返回失败*/
#define  GM_INI_FILE_READ_INTORHEX8_CPCOUNT_ERR	   0xA0010A02U			/*cpcount的值有错误*/
#define  GM_INI_FILE_READ_INTORHEX8_32_PARSE_ERR   0xA0010A03U			/*GM_Ini_File_Read_Hex_32_Parse返回失败*/
#define  GM_INI_FILE_READ_INTORHEX8_CPCOUNT_2_ERR  0xA0010A04U			/*cpcount的值有错误*/
#define  GM_INI_FILE_READ_INTORHEX8_ATOUI_ERR	   0xA0010A05U			/*GM_atoui返回失败*/
#define  GM_INI_FILE_READ_INTORHEX8_VALUE_ERR	   0xA0010A06U			/*value超过GM_RSSP1_UINT8_MAX*/

/**GM_Ini_File_Read_IntORHex16(),0xA0010BXX**/
#define  GM_INI_FILE_READ_INTORHEX16_PAR_ERR		   0xA0010B00U			/*GM_Ini_File_Read_IntORHex16传入参数有误*/
#define  GM_INI_FILE_READ_INTORHEX16_PARSE_ERR		   0xA0010B01U			/*GM_Ini_File_Parse返回失败*/
#define  GM_INI_FILE_READ_INTORHEX16_CPCOUNT_ERR	   0xA0010B02U			/*cpcount的值有错误*/
#define  GM_INI_FILE_READ_INTORHEX16_32_PARSE_ERR      0xA0010B03U			/*GM_Ini_File_Read_Hex_32_Parse返回失败*/
#define  GM_INI_FILE_READ_INTORHEX16_CPCOUNT_2_ERR     0xA0010B04U			/*cpcount的值有错误*/
#define  GM_INI_FILE_READ_INTORHEX16_ATOUI_ERR	       0xA0010B05U			/*GM_atoui返回失败*/
#define  GM_INI_FILE_READ_INTORHEX16_VALUE_ERR	       0xA0010B06U			/*value超过GM_RSSP1_UINT8_MAX*/

/**GM_Ini_File_Read_IntORHex32(),0xA0010CXX**/
#define  GM_INI_FILE_READ_INTORHEX32_PAR_ERR	       0xA0010C00U			/*GM_Ini_File_Read_IntORHex32传入参数有误*/
#define  GM_INI_FILE_READ_INTORHEX32_PARSE_ERR	       0xA0010C01U			/*GM_Ini_File_Parse返回失败*/
#define  GM_INI_FILE_READ_INTORHEX32_CPCOUNT_ERR	   0xA0010C02U			/*cpcount的值有错误*/
#define  GM_INI_FILE_READ_INTORHEX32_32_PARSE_ERR	   0xA0010C03U			/*GM_Ini_File_Read_Hex_32_Parse返回失败*/
#define  GM_INI_FILE_READ_INTORHEX32_CPCOUNT_2_ERR	   0xA0010C04U			/*cpcount的值有错误*/
#define  GM_INI_FILE_READ_INTORHEX32_ATOUI_ERR		   0xA0010C05U			/*GM_atoui返回失败*/

/**GM_Get_File_Partly_Size(),0xA0010DXX**/
#define  GM_GET_FILE_PARTLY_SIZE_PAR_ERR			  0xA0010D00U			/*GM_Get_File_Partly_Size传入参数有误*/
#define  GM_GET_FILE_PARTLY_SIZE_OPEN_ERR	 	 	  0xA0010D01U			/*打开文件失败*/
#define  GM_GET_FILE_PARTLY_SIZE_FSEEK_ERR			  0xA0010D02U			/*fseek返回失败*/
#define  GM_INI_FILE_PARTLY_SIZE_CONTENT_ERR		  0xA0010D03U			/*GM_Get_File_Partly_Size读取{中内容过多*/
#define  GM_GET_FILE_PARTLY_SIZE_FTELL_ERR			  0xA0010D04U			/*ftell返回失败*/
#define  GM_GET_FILE_PARTLY_SIZE_CLOSE_ERR			  0xA0010D05U			/*关闭文件失败*/
#define  GM_GET_FILE_PARTLY_SIZE_NOT_FIND_ERR		  0xA0010D06U			/*未找到partName返回失败*/




/*******BIN MULTIHOST INITIALIZE部分,错误号为0xA002XXXX***********/
/**GM_Bin_Head_H2N_N2H(),0xA00201XX**/
#define  GM_BIN_HEAD_H2N_H2N_PAR_ERR	     0xA0020100U		/*GM_Bin_Head_H2N_N2H传入参数有误*/

/**GM_Multibin_GetHost(),0xA00202XX**/
#define  GM_MULTIBIN_GETHOST_PAR_ERR	     0xA0020200U		/*GM_Multibin_GetHost传入参数有误*/
#define  GM_MULTIBIN_GETHOST_CRC_ERR	     0xA0020201U		/*CRC检查不通过*/
#define  GM_MULTIBIN_GETHOST_SIZE_ERR	     0xA0020202U		/*判断文件大小有误*/
#define  GM_MULTIBIN_GETHOST_VERSION_ERR	 0xA0020203U		/*判断文件版本有误*/
#define  GM_MULTIBIN_GETHOST_LOAD_ERR		 0xA0020204U		/*装载出错*/


/******************MODULAR部分,错误号为0xA003XXXX****************/
/**暂无**/


/*********** 64BIT DATA CALCULATE,错误号为0xA004XXXX****************/
/**暂无**/



/******************CODE_SNV部分,错误号为0xA005XXXX****************/
/**GM_Code_SNV_Init(),0xA00501XX**/
#define  GM_CODE_SNV_INIT_PAR_ERR			 0xA0050100U		/*GM_Code_SNV_Init传入参数有误*/
#define  GM_CODE_SNV_INIT_SIZE_ERR			 0xA0050101U		/*读取的文件长度与传入不一致*/
#define  GM_CODE_SNV_INIT_CRC_ERR			 0xA0050102U		/*CRC校验不通过*/
#define  GM_CODE_SNV_INIT_VERSION_ERR		 0xA0050103U		/*版本号校验不通过*/
#define  GM_CODE_SNV_INIT_NISALVCP_ERR		 0xA0050104U		/*GM_Code_Read_NISALVCP返回失败*/
#define  GM_CODE_SNV_INIT_SINGLENISAL_ERR	 0xA0050105U		/*GM_Code_Read_SINGLENISAL返回失败*/
#define  GM_CODE_SNV_INIT_MODE_ERR			 0xA0050106U		/*MODE错误*/

/**GM_Code_GetTypeIndex(),0xA00502XX**/
#define  GM_CODE_GETTYPEINDEX_PAR_ERR		 0xA0050200U		/*GM_Code_GetTypeIndex传入参数有误*/
#define  GM_CODE_GETTYPEINDEX_MODE_ERR		 0xA0050201U		/*MODE错误*/

/**GM_Code_SINGLE2NISAL(),0xA00503XX**/
#define  GM_CODE_SINGLE2NISAL_PAR_ERR		 0xA0050300U		/*GM_Code_SINGLE2NISAL传入参数有误*/
#define  GM_CODE_SINGLE2NISAL_TYPEINDEX_ERR	 0xA0050301U		/*typeIndex有误*/

/**GM_Code_NISAL2SINGLE(),0xA00504XX**/
#define  GM_CODE_NISAL2SINGLE_PAR_ERR		 0xA0050400U		/*GM_Code_NISAL2SINGLE传入参数有误*/
#define  GM_CODE_NISAL2SINGLE_TYPEINDEX_ERR	 0xA0050401U		/*typeIndex有误*/


/******************MUDP部分,错误号为0xA006XXXX******************/
/**GM_MUDP_Get_UDP_Flag(),0xA00601XX**/
#define  GM_MUDP_GET_UDP_FLAG_PAR_ERR		0xA0060100U		/*GM_MUDP_Get_UDP_Flag传入参数有误*/
#define  GM_MUDP_GET_UDP_FLAG_NOT_FOUND		0xA0060101U		/*没有找到对应的msgType*/

/**GM_MUDP_Write(),0xA00602XX**/
#define GM_MUDP_WRITE_PAR_ERR				0xA0060200U		/*GM_MUDP_Write传入参数有误*/
#define GM_MUDP_WRITE_WRITE_PROCESS_ERR		0xA0060201U		/*GM_MUDP_Write_Process返回失败*/

/**GM_MUDP_Open(),0xA00603XX**/
#define GM_MUDP_OPEN_PAR_ERR				0xA0060300U		/*GM_MUDP_Open传入参数有误*/
#define GM_MUDP_OPEN_MSGTYPE_ERR			0xA0060301U		/*待新建的msgType已经存在*/
#define GM_MUDP_OPEN_CREATE_ERR				0xA0060302U		/*创建Socket失败*/
#define GM_MUDP_OPEN_WRITE_ERR				0xA0060303U		/*发送SN为0的数据包失败*/

/**GM_MUDP_Init(),0xA00604XX**/
#define GM_MUDP_INIT_PAR_ERR				0xA0060400U		/*GM_MUDP_Init传入参数有误*/
#define GM_MUDP_INIT_UDP_INIT_ERR			0xA0060401U		/*GM_MUDP_Init初始化UDP失败*/
#define GM_MUDP_INIT_THREAD_CREATE_ERR		0xA0060402U		/*GM_MUDP_Init创建线程失败*/

/**GM_MUDP_Msg_Encode(),0xA00605XX**/
#define GM_MUDP_MSG_ENCODE_PAR_ERR			0xA0060500U		/*GM_MUDP_Msg_Encode传入参数有误*/
#define GM_MUDP_MSG_ENCODE_CHANGE_ERR		0xA0060501U		/*转换成网络字节序返回失败*/
#define GM_MUDP_MSG_ENCODE_MODE_ERR			0xA0060502U		/*GM_MUDP_Msg_Encode传入CRCMode有误*/
#define GM_MUDP_MSG_ENCODE_CAL_CRC_ERR		0xA0060503U		/*GM_CRC_Calculate_CRC16返回失败*/

/**GM_MUDP_Msg_Decode(),0xA00606XX**/
#define GM_MUDP_MSG_DECODE_PAR_ERR			0xA0060600U		/*GM_MUDP_Msg_Decode传入参数有误*/
#define GM_MUDP_MSG_DECODE_SIZE_ERR			0xA0060601U		/*GM_MUDP_Msg_Decode传入的消息size有误*/
#define GM_MUDP_MSG_DECODE_MODE_ERR			0xA0060602U		/*GM_MUDP_Msg_Decode传入CRCMode有误*/
#define GM_MUDP_MSG_DECODE_CRC_ERR			0xA0060603U		/*CRC检验失败失败*/
#define GM_MUDP_MSG_DECODE_CHANGE_ERR		0xA0060604U		/*转换成网络字节序返回失败*/

/*********************MR部分,错误号为0xA007XXXX******************/
/**GM_Memory_Refresh_Init(),0xA00701XX**/
#define GM_MEMORY_REFRESH_INIT_LOAD_ERR				0xA0070100U		/*GM_Ini_File_Load返回失败*/
#define GM_MEMORY_REFRESH_INIT_READ_ERR				0xA0070101U		/*初始化读取参数失败*/
#define GM_MEMORY_REFRESH_INIT_CHANNEL_ERR			0xA0070102U		/*初始化读取合并生成最终校核字的通道信息CHANNEL有误*/
#define GM_MEMORY_REFRESH_INIT_CHANNEL_ERR_2		0xA0070103U		/*初始化读取所属的通道信息CHANNEL有误*/
#define GM_MEMORY_REFRESH_INIT_MEMADDR_ERR			0xA0070104U		/*初始化读取内存首尾地址有误*/
#define GM_MEMORY_REFRESH_INIT_DUMPCHKWPOS_ERR		0xA0070105U		/*生成第dumpChkwInex个校核字的第dumpChkwPos位置上已有值*/
#define GM_MEMORY_REFRESH_INIT_MEMSIZE_ERR			0xA0070106U		/*dump的内存地址大小不等于实际刷新的内存memSize大小*/
#define GM_MEMORY_REFRESH_INIT_MODE_ERR				0xA0070107U		/*初始化读取REFRESH_MODE有误*/
#define GM_MEMORY_REFRESH_INIT_ADD_ERR				0xA0070108U		/*GM_Memory_Refresh_Add返回失败*/
#define GM_MEMORY_REFRESH_INIT_CHKWPOS_ERR			0xA0070109U		/*生成第chkwInex个校核字的第chkwPos位置上已有值*/

/**GM_Memory_Refresh_Create_CHKW(),0xA00702XX**/
#define GM_MEMORY_REFRESH_CREATE_CHKW_INIT_ERR			0xA0070200U		/*GM_Memory_Refresh_Initialized不为TRUE*/
#define GM_MEMORY_REFRESH_CREATE_CHKW_SLOTINDEX_ERR		0xA0070201U		/*slotIndex超出限定范围*/
#define GM_MEMORY_REFRESH_CREATE_CHKW_EACH_ERR			0xA0070202U		/*GM_Memory_Refresh_Create_Each_CHKW返回失败*/

/**GM_Memory_Refresh_Init_From_Addr(),0xA00703XX**/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_PAR_ERR				0xA0070300U		/*GM_Memory_Refresh_Init_From_Addr入参有误*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_SIZE_ERR				0xA0070301U		/*传入size有误*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_CRC_ERR				0xA0070302U		/*CRC校验失败*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_VERSION_ERR			0xA0070303U		/*版本校验失败*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_CHANNEL_ERR			0xA0070304U		/*读取合并生成最终校核字的通道信息有误*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_CHKW_NUM_ERR			0xA0070305U		/*CKW数目超过最大值*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_CHKW_MEMORY_ERR		0xA0070306U		/*内存块数目超过最大值*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_SLOT_NUM_ERR			0xA0070307U		/*刷新时隙数超过最大值*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_EACH_CHANNEL_ERR		0xA0070308U		/*读取每块内存的通道信息有误*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_EACH_SLOT_NUM_ERR		0xA0070309U		/*读取每块内存的刷新时隙数超过最大值*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_MEMADDR_ERR			0xA007030AU		/*读取内存首尾地址有误*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_INDEX_ERR				0xA007030BU		/*读取刷新内存index有误*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_CHKWPOS_ERR			0xA007030CU		/*生成第chkwInex个校核字的第chkwPos位置上已有值*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_MEMSIZE_ERR			0xA007030DU		/*dump的内存地址大小不等于实际刷新的内存memSize大小*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_CONFIG_INDEX_ERR		0xA007030EU		/*获取配置的校核字信息index有误*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_DUMPCHKWPOS_ERR		0xA007030FU		/*生成第dumpChkwInex个校核字的第dumpChkwPos位置上已有值*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_VALUE_ERR				0xA0070310U		/*读取自保留变量信息错误*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_ADD_ERR				0xA0070311U		/*GM_Memory_Refresh_Add返回失败*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_FALSE_INDEX_ERR		0xA0070312U		/*读取位于其后各内存的配置中的假校核字index有误*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_FALSE_CHKW_NUM_ERR		0xA0070313U		/*假校核字CKW数目超过最大值*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_FALSE_CHKWPOS_ERR		0xA0070314U		/*假校核字生成第chkwInex个校核字的第chkwPos位置上已有值*/


/*********************CRC部分,错误号为0xA008XXXX******************/
/**GM_CRC_Calculate_CRC16_Standard(),0xA00801XX**/
#define GM_CRC_CALCULATE_CRC16_STANDARD_PAR_ERR					0xA0080100U		/*GM_CRC_Calculate_CRC16_Standard入参有误*/
#define GM_CRC_CALCULATE_CRC16_STANDARD_POLY_ERR				0xA0080101U		/*传入Poly有误*/
#define GM_CRC_CALCULATE_CRC16_STANDARD_POLY_REFLECT_ERR		0xA0080102U		/*传入Poly有误*/

/**GM_CRC_Calculate_CRC16_Standard_Seed(),0xA00802XX**/
#define GM_CRC_CALCULATE_CRC16_STANDARD_SEED_PAR_ERR				0xA0080200U		/*GM_CRC_Calculate_CRC16_Standard_Seed入参有误*/
#define GM_CRC_CALCULATE_CRC16_STANDARD_SEED_POLY_ERR				0xA0080201U		/*传入Poly有误*/
#define GM_CRC_CALCULATE_CRC16_STANDARD_SEED_POLY_REFLECT_ERR		0xA0080202U		/*传入Poly有误*/

/**GM_CRC_Calculate_CRC32_Standard(),0xA00803XX**/
#define GM_CRC_CALCULATE_CRC32_STANDARD_PAR_ERR					0xA0080300U		/*GM_CRC_Calculate_CRC32_Standard入参有误*/
#define GM_CRC_CALCULATE_CRC32_STANDARD_POLY_ERR				0xA0080301U		/*传入Poly有误*/

/**GM_NISAL18CODE_Check(),0xA00804XX**/
#define GM_NISAL18CODE_CHECK_PAR_ERR			0xA0080400U		/*GM_NISAL18CODE_Check入参NISAL18CODE有误*/
#define GM_NISAL18CODE_CHECK_FAIL				0xA0080401U		/*GM_NISAL18CODE_Check检查不通过*/

/**GM_NISALCODE_Check(),0xA00805XX**/
#define GM_NISALCODE_CHECK_PAR_ERR			0xA0080500U		/*GM_NISALCODE_Check入参NISALCode有误*/
#define GM_NISALCODE_CHECK_FAIL				0xA0080501U		/*GM_NISALCODE_Check检查不通过*/


/*********************VSN部分,错误号为0xA009XXXX******************/
/**GM_VSN_Init(),0xA00901XX**/
#define GM_VSN_INIT_LFSR_1_ERR			0xA0090100U		/*GM_LFSR_Init通道1返回失败*/
#define GM_VSN_INIT_LFSR_2_ERR			0xA0090101U		/*GM_LFSR_Init通道2返回失败*/

/**GM_VSN_Cal_Check_Word_VSN1(),0xA00902XX**/
#define GM_VSN_CAL_CHECK_WORD_VSN1_PAR_ERR			0xA0090200U		/*GM_VSN_Cal_Check_Word_VSN1入参pCHKW为NULL*/
#define GM_VSN_CAL_CHECK_WORD_VSN1_CAL_FAIL			0xA0090201U		/*GM_VSN_Cal_Check_Word_VSN1计算失败*/

/**GM_VSN_Cal_Check_Word_VSN2(),0xA00903XX**/
#define GM_VSN_CAL_CHECK_WORD_VSN2_PAR_ERR			0xA0090300U		/*GM_VSN_Cal_Check_Word_VSN2入参pCHKW为NULL*/
#define GM_VSN_CAL_CHECK_WORD_VSN2_CAL_FAIL			0xA0090301U		/*GM_VSN_Cal_Check_Word_VSN2计算失败*/

/**GM_VSN_Add(),0xA00904XX**/
#define GM_VSN_ADD_PAR_ERR				0xA0090400U			/*GM_VSN_Add入参VSNID为0*/
#define GM_VSN_ADD_VSNID_EXIST			0xA0090401U			/*入参VSNID已经存在*/
#define GM_VSN_ADD_NUM_ERR				0xA0090402U			/*VSN_Num已经达到最大值*/

/**GM_VSN_Get_S(),0xA00905XX**/
#define GM_VSN_GET_S_PAR_ERR			0xA0090500U			/*GM_VSN_Get_S入参有误*/
#define GM_VSN_GET_S_VSN_NOT_FIND		0xA0090501U			/*未找到对应的VSNID*/

/**GM_VSN_OTHER_Init(),0xA00906XX**/
#define GM_VSN_OTHER_INIT_LFSR_1_ERR			0xA0090600U		/*GM_LFSR_Init通道1返回失败*/
#define GM_VSN_OTHER_INIT_LFSR_2_ERR			0xA0090601U		/*GM_LFSR_Init通道2返回失败*/

/**GM_VSN_OTHER_Cal_Check_Word_VSN1(),0xA00907XX**/
#define GM_VSN_OTHER_CAL_CHECK_WORD_VSN1_PAR_ERR			0xA0090700U		/*GM_VSN_OTHER_Cal_Check_Word_VSN1入参pCHKW为NULL*/
#define GM_VSN_OTHER_CAL_CHECK_WORD_VSN1_CAL_FAIL			0xA0090701U		/*GM_VSN_OTHER_Cal_Check_Word_VSN1计算失败*/

/**GM_VSN_OTHER_Cal_Check_Word_VSN2(),0xA00908XX**/
#define GM_VSN_OTHER_CAL_CHECK_WORD_VSN2_PAR_ERR			0xA0090800U		/*GM_VSN_OTHER_Cal_Check_Word_VSN2入参pCHKW为NULL*/
#define GM_VSN_OTHER_CAL_CHECK_WORD_VSN2_CAL_FAIL			0xA0090801U		/*GM_VSN_OTHER_Cal_Check_Word_VSN2计算失败*/

/**GM_VSN_Set_S(),0xA00909XX**/
#define GM_VSN_SET_S_VSN_NOT_FIND		0xA0090900U			/*未找到对应的VSNID*/

/*********************SIOCOM部分,错误号为0xA00AXXXX******************/
/**GM_SIO_Com_Open(),0xA000A01XX**/
#define GM_SIO_COM_OPEN_PAR_ERR				0xA00A0100U		/*GM_SIO_Com_Open入参有误*/
#define GM_SIO_COM_OPEN_INIT_FAIL			0xA00A0101U		/*打开串口返回失败*/
#define GM_SIO_COM_OPEN_RXBUF_INIT_FAIL		0xA00A0102U		/*打开接收缓冲区返回失败*/

/**GM_SIO_Com_Read(),0xA00A02XX**/
#define GM_SIO_COM_READ_PAR_ERR				0xA00A0200U		/*GM_SIO_Com_Read入参有误*/
#define GM_SIO_COM_READ_THREAD_ERR			0xA00A0201U		/*GM_SIO_Com_Read创建接收线程失败*/

/**GM_SIO_Com_Write(),0xA00A03XX**/
#define GM_SIO_COM_WRITE_PAR_ERR			0xA00A0300U		/*GM_SIO_Com_Write入参有误*/
#define GM_SIO_COM_WRITE_MSG_ENCODE_ERR		0xA00A0301U		/*GM_SIO_Msg_Encode返回失败*/
#define GM_SIO_COM_WRITE_MSG_FAIL			0xA00A0302U		/*GM_SIO_Com_Write返回失败*/

/**GM_SIO_Msg_Encode(),0xA00A04XX**/
#define GM_SIO_MSG_ENCODE_PAR_ERR			0xA00A0400U		/*GM_SIO_Msg_Encode入参有误*/
#define GM_SIO_MSG_ENCODE_CRC_ERR			0xA00A0401U		/*计算16位CRC返回失败*/

/**GM_SIO_Msg_Decode(),0xA00A05XX**/
#define GM_SIO_MSG_DECODE_PAR_ERR			0xA00A0500U		/*GM_SIO_Msg_Decode入参有误*/
#define GM_SIO_MSG_DECODE_SIZE_ERR			0xA00A0501U		/*消息长度有误*/
#define GM_SIO_MSG_DECODE_CRC_ERR			0xA00A0502U		/*16位CRC验证失败*/

/**VXWORKS6_6，openPort(),0xA00A06XX**/
#define OPENPORT6_6_PAR_ERR					0xA00A0600U		/*openPort入参有误*/
#define OPENPORT6_6_OPEN_ERR				0xA00A0601U		/*openPort打开设备失败*/
#define OPENPORT6_6_SET_BAUDRATE_ERR		0xA00A0602U		/*openPort设置波特率失败*/

/**GM_SIO_Open_VXWORKS6_6(),0xA00A07XX**/
#define GM_SIO_OPEN_VXWORKS6_6_PAR_ERR				0xA00A0700U			/*GM_SIO_Open_VXWORKS6_6入参有误*/
#define GM_SIO_OPEN_VXWORKS6_6_OPENPORT_ERR			0xA00A0701U			/*GM_SIO_Open_VXWORKS6_6打开设备失败*/
#define GM_SIO_OPEN_VXWORKS6_6_SETPORTBAUD_ERR		0xA00A0702U			/*GM_SIO_Open_VXWORKS6_6设置波特率失败*/
#define GM_SIO_OPEN_VXWORKS6_6_DATABITS_ERR			0xA00A0703U			/*传入的databits有误*/
#define GM_SIO_OPEN_VXWORKS6_6_STOPBITS_ERR			0xA00A0704U			/*传入的stopbits有误*/
#define GM_SIO_OPEN_VXWORKS6_6_PARITY_ERR			0xA00A0705U			/*传入的parity有误*/
#define GM_SIO_OPEN_VXWORKS6_6_SETPORTHWOPT_ERR		0xA00A0706U			/*设置Hardware Options失败*/

/**VXWORKS5_5，openPort(),0xA00A08XX**/
#define OPENPORT5_5_PAR_ERR					0xA00A0800U		/*openPort入参有误*/
#define OPENPORT5_5_OPEN_ERR				0xA00A0801U		/*openPort打开设备失败*/
#define OPENPORT5_5_SET_BAUDRATE_ERR		0xA00A0802U		/*openPort设置波特率失败*/

/**GM_SIO_Open_VXWORKS5_5(),0xA00A09XX**/
#define GM_SIO_OPEN_VXWORKS5_5_PAR_ERR				0xA00A0900U			/*GM_SIO_Open_VXWORKS5_5入参有误*/
#define GM_SIO_OPEN_VXWORKS5_5_OPENPORT_ERR			0xA00A0901U			/*GM_SIO_Open_VXWORKS5_5打开设备失败*/
#define GM_SIO_OPEN_VXWORKS5_5_SETPORTBAUD_ERR		0xA00A0902U			/*GM_SIO_Open_VXWORKS5_5设置波特率失败*/
#define GM_SIO_OPEN_VXWORKS5_5_DATABITS_ERR			0xA00A0903U			/*传入的databits有误*/
#define GM_SIO_OPEN_VXWORKS5_5_STOPBITS_ERR			0xA00A0904U			/*传入的stopbits有误*/
#define GM_SIO_OPEN_VXWORKS5_5_PARITY_ERR			0xA00A0905U			/*传入的parity有误*/

/**GM_SIO_Open_Windows(),0xA00A0AXX**/
#define GM_SIO_OPEN_WINDOWS_PAR_ERR					0xA00A0A00U			/*GM_SIO_Open_Windows入参有误*/
#define GM_SIO_OPEN_WINDOWS_GET_NAME_ERR			0xA00A0A01U			/*GM_SIO_Get_Name_By_No返回失败*/
#define GM_SIO_OPEN_WINDOWS_STOPBITS_ERR			0xA00A0A02U			/*传入stopbits有误*/
#define GM_SIO_OPEN_WINDOWS_BUILDCOMMDCB_ERR		0xA00A0A03U			/*BuildCommDCB返回失败*/
#define GM_SIO_OPEN_WINDOWS_CREATEFILE_ERR			0xA00A0A04U			/*CreateFile()返回失败*/
#define GM_SIO_OPEN_WINDOWS_SETUPCOMM_ERR			0xA00A0A05U			/*设置输入输出buf,SetupComm()返回失败*/
#define GM_SIO_OPEN_WINDOWS_SETCOMMSTATE_ERR		0xA00A0A06U			/*关联DCB结构,SetCommState()返回失败*/
#define GM_SIO_OPEN_WINDOWS_SETCOMMTIMEOUTS_ERR		0xA00A0A07U			/*关联超时,SetCommTimeouts()返回失败*/
#define GM_SIO_OPEN_WINDOWS_PURGECOMM_ERR			0xA00A0A08U			/*清空buf信息,PurgeComm()返回失败*/

/**GM_SIO_Read_Windows(),0xA00A0BXX**/
#define GM_SIO_READ_WINDOWS_PAR_ERR						0xA00A0B00U			/*GM_SIO_Read_Windows入参有误*/
#define GM_SIO_READ_WINDOWS_CREATEEVENT_ERR				0xA00A0B01U			/*CreateEvent()返回失败*/
#define GM_SIO_READ_WINDOWS_CLEARCOMMERROR_FAIL			0xA00A0B02U			/*ClearCommError()返回失败*/
#define GM_SIO_READ_WINDOWS_STATCBINQUE_ERR				0xA00A0B03U			/*ClearCommError()返回stat.cbInQue有误*/
#define GM_SIO_READ_WINDOWS_GETOVERLAPPEDRESULT_ERR		0xA00A0B04U			/*GetOverlappedResult()返回失败--重叠操作的当前状态*/
#define GM_SIO_READ_WINDOWS_WAITFORSINGLEOBJECT_ERR		0xA00A0B05U			/*WaitForSingleObject()返回失败*/
#define GM_SIO_READ_WINDOWS_GETERRORNO_ERR				0xA00A0B06U			/*GM_Get_Error_No()返回失败*/

/**GM_SIO_Write_Windows(),0xA00A0CXX**/
#define GM_SIO_WRITE_WINDOWS_PAR_ERR						0xA00A0C00U			/*GM_SIO_Write_Windows入参有误*/
#define GM_SIO_WRITE_WINDOWS_CREATEEVENT_ERR				0xA00A0C01U			/*CreateEvent()返回失败*/
#define GM_SIO_WRITE_WINDOWS_GETLASTERROR_ERR				0xA00A0C02U			/*GetLastError()返回失败*/
#define GM_SIO_WRITE_WINDOWS_GETOVERLAPPEDRESULT_ERR		0xA00A0C03U			/*GetOverlappedResult()返回失败*/
#define GM_SIO_WRITE_WINDOWS_DATASIZE_ERR					0xA00A0C04U			/*GetOverlappedResult()获得w_len与datasize不等*/
#define GM_SIO_WRITE_WINDOWS_WAITFORSINGLEOBJECT_ERR		0xA00A0C05U			/*WaitForSingleObject()返回失败*/
#define GM_SIO_WRITE_WINDOWS_WRITE_WLEN_ERR					0xA00A0C06U			/*WriteFile()写的w_len与datasize不等*/

/**GM_SIO_Rx_Task(),0xA00A0DXX**/
#define GM_SIO_RX_TASK_WIN32_PAR_ERR				0xA00A0D00U			/*win32下，GM_SIO_Rx_Task入参有误*/
#define GM_SIO_RX_TASK_WIN32_PORT_ERR				0xA00A0D01U			/*win32下，GM_SIO_Rx_Task入参port有误*/
#define GM_SIO_RX_TASK_WIN32_ANALYSE_ERR			0xA00A0D02U			/*win32下，GM_SIO_COM_Rx_Analyse_Task返回失败*/
#define GM_SIO_RX_TASK_VXWORKS_PORT_ERR				0xA00A0D03U			/*VXWORKS下，GM_SIO_Rx_Task入参port有误*/
#define GM_SIO_RX_TASK_VXWORKS_ANALYSE_ERR			0xA00A0D04U			/*VXWORKS下，GM_SIO_COM_Rx_Analyse_Task返回失败*/
#define GM_SIO_RX_TASK_QNX_PAR_ERR					0xA00A0D05U			/*QNX下，GM_SIO_Rx_Task入参有误*/
#define GM_SIO_RX_TASK_QNX_PORT_ERR					0xA00A0D06U			/*QNX下，GM_SIO_Rx_Task入参port有误*/
#define GM_SIO_RX_TASK_QNX_ANALYSE_ERR				0xA00A0D07U			/*QNX下，GM_SIO_COM_Rx_Analyse_Task返回失败*/



/*********************TIME部分,错误号为0xA00BXXXX******************/
/**GM_Time_BIOS_Get(),0xA00B01XX**/
#define GM_TIME_BIOS_GET_6U_PAR_ERR						0xA00B0100U			/*6U下，GM_Time_BIOS_Get()传入参数有误*/
#define GM_TIME_BIOS_GET_6U_TIME_GET_ERR				0xA00B0101U			/*6U下，Drv_Rtc_Time_Get()返回失败*/
#define GM_TIME_BIOS_GET_VXWORKS6_6_PAR_ERR				0xA00B0102U			/*VXWORKS6_6下，GM_Time_BIOS_Get()传入参数有误*/
#define GM_TIME_BIOS_GET_VXWORKS6_6_SYSRTCGET_ERR		0xA00B0103U			/*VXWORKS6_6下，sysRtcGet返回失败*/
#define GM_TIME_BIOS_GET_QNX_PAR_ERR					0xA00B0104U			/*QNX下，GM_Time_BIOS_Get()传入参数有误*/
#define GM_TIME_BIOS_GET_QNX_RTCINIT_ERR				0xA00B0105U			/*QNX下，rtcInit()返回失败*/
#define GM_TIME_BIOS_GET_QNX_RTCREAD_ERR				0xA00B0106U			/*QNX下，rtcRead()返回失败*/
#define GM_TIME_BIOS_GET_DEFINE_ERR						0xA00B0107U			/*GM_Time_BIOS_Get模式宏定义错误*/

/**GM_Time_BIOS_Set(),0xA00B02XX**/
#define GM_TIME_BIOS_SET_6U_PAR_ERR						0xA00B0200U			/*6U下，GM_Time_BIOS_Set()传入参数有误*/
#define	GM_TIME_BIOS_SET_6U_DAYTIME_ERR					0xA00B0201U			/*6U下，GM_Time_BIOS_Set()传入的dayTime数据错误*/
#define GM_TIME_BIOS_SET_6U_TIME_SET_ERR				0xA00B0202U			/*6U下，Drv_Rtc_Time_Set()返回失败*/
#define GM_TIME_BIOS_SET_VXWORKS6_6_PAR_ERR				0xA00B0203U			/*VXWORKS6_6下，GM_Time_BIOS_Set()传入参数有误*/
#define	GM_TIME_BIOS_SET_VXWORKS6_6_DAYTIME_ERR			0xA00B0204U			/*VXWORKS6_6下，GM_Time_BIOS_Set()传入的dayTime数据错误*/
#define GM_TIME_BIOS_SET_QNX_PAR_ERR					0xA00B0205U			/*QNX下，GM_Time_BIOS_Set()传入参数有误*/
#define GM_TIME_BIOS_SET_QNX_RTCINIT_ERR				0xA00B0206U			/*QNX下，rtcInit()返回失败*/

/**GM_Time_OS_Set(),0xA00B03XX**/
#define GM_TIME_OS_SET_WIN32_PAR_ERR					0xA00B0300U			/*WIN32下，GM_Time_OS_Set传入参数有误*/
#define GM_TIME_OS_SET_WIN32_SETLOCALTIME_ERR			0xA00B0301U			/*WIN32下，SetLocalTime返回失败*/
#define GM_TIME_OS_SET_QNX_PAR_ERR						0xA00B0302U			/*QNX下，GM_Time_OS_Set传入参数有误*/
#define GM_TIME_OS_SET_QNX_MKTIME_ERR					0xA00B0303U			/*QNX下，mktime返回失败*/
#define GM_TIME_OS_SET_QNX_SETTIMEOFDAY_ERR				0xA00B0304U			/*QNX下，sttimeofday返回失败*/
#define GM_TIME_OS_SET_VXWORKS_PAR_ERR					0xA00B0305U			/*VXWORKS下，GM_Time_OS_Set传入参数有误*/
#define GM_TIME_OS_SET_VXWORKS_CLOCKSETTIME_ERR			0xA00B0306U			/*VXWORKS下，clock_settime返回失败*/

/**GM_Time_Get_UTC(),0xA00B04XX**/
#define GM_TIME_GET_UTC_WIN32_PAR_ERR						0xA00B0400U				/*WIN32下，GM_Time_Get_UTC传入参数有误*/
#define GM_TIME_GET_UTC_OTHER_PAR_ERR						0xA00B0401U				/*其他系统下，GM_Time_Get_UTC传入参数有误*/


/*********************TIMER部分,错误号为0xA00CXXXX******************/
/**GM_Timer_Init(),0xA00C01XX**/
#define GM_TIMER_INIT_PAR_ERR						0xA00C0100U				/*GM_Timer_Init传入参数有误*/
#define GM_TIMER_INIT_WIN32_THREAD_ERR				0xA00C0101U				/*WIN32下，创建GM_Timer_Thread线程失败*/
#define GM_TIMER_INIT_VXWORKS5_5_INIT_ERR			0xA00C0102U				/*VXWORKS5_5下，Drv_M8247_Timer_Init()返回失败*/
#define GM_TIMER_INIT_QNX_PAR_ERR					0xA00C0103U				/*QNX下，sysTickTmrInit()返回失败*/
#define GM_TIMER_INIT_QNX_SYSTTCR_ERR				0xA00C0104U				/*QNX下，sysTickTmrConnectRoutine()返回失败*/
#define GM_TIMER_INIT_QNX_SYSTTSTART_ERR			0xA00C0105U				/*QNX下，sysTickTmrStart()返回失败*/
#define GM_TIMER_INIT_VXWORKS6_6_SMTCR_ERR			0xA00C0106U				/*VXWORKS6_6下，sysMpc8572TmrConnectRoutine()返回失败*/
#define GM_TIMER_INIT_VXWORKS6_6_SMTS_ERR			0xA00C0107U				/*VXWORKS6_6下，sysMpc8572TmrStart()返回失败*/
#define GM_TIMER_INIT_VXWORKS6_6_TIMERNO_ERR		0xA00C0108U				/*VXWORKS6_6下，传入的timerNo有误*/

/**GM_Timer_Set(),0xA00C02XX**/
#define GM_TIMER_SET_WIN32_PAR_ERR						0xA00C0200U				/*WIN32下，GM_Timer_Set传入参数有误*/
#define GM_TIMER_SET_VXWORKS5_5_PAR_ERR					0xA00C0201U				/*VXWORKS5_5下，GM_Timer_Set传入参数有误*/
#define	GM_TIMER_SET_VXWORKS5_5_TIMER_SET_ERR			0xA00C0202U				/*VXWORKS5_5下，Drv_M8247_Timer_Rstint()返回失败*/
#define GM_TIMER_SET_QNX_PAR_ERR						0xA00C0203U				/*QNX下，GM_Timer_Set传入参数有误*/
#define	GM_TIMER_SET_QNX_SYSTICKTMRSTART_ERR			0xA00C0204U				/*QNX下，sysTickTmrStart()返回失败*/
#define GM_TIMER_SET_VXWORKS6_6_SYSTMRDISABLE_ERR		0xA00C0205U				/*VXWORKS6_6下，sysMpc8572TmrDisable()返回失败*/
#define GM_TIMER_SET_VXWORKS6_6_SYSTMRSTART_ERR			0xA00C0206U				/*VXWORKS6_6下，sysMpc8572TmrStart()返回失败有误*/
#define GM_TIMER_SET_VXWORKS6_6_TIMERNO_ERR				0xA00C0207U				/*VXWORKS6_6下，传入的timerNo有误*/


/***********TIME OF HIGH-PRECISION部分,错误号为0xA00DXXXX**********/
/**GM_TimestampInit(),0xA00D01XX**/
#define GM_TIMESTAMPINIT_VXWORKS6_6_FAIL				0xA00D0100U			/*VXWORKS6_6下，sysTimestampEnable()返回失败*/
#define GM_TIMESTAMPINIT_QNX_SYSTICKTMRINIT_FAIL		0xA00D0101U			/*QNX下，sysTickTmrInit()返回失败*/
#define GM_TIMESTAMPINIT_QNX_SYSTICKTMRSTART_FAIL		0xA00D0102U			/*QNX下，sysTickTmrStart()返回失败*/


/*********************OTHER部分,错误号为0xA00EXXXX******************/
/**GM_atoui(),0xA00E01XX**/
#define GM_ATOUI_PAR_ERR					0xA00E0100U				/*GM_atoui()传入参数有误*/
#define GM_ATOUI_BUFSIZE_ZERO_ERR			0xA00E0101U				/*GM_atoui()传入bufsize为0*/
#define GM_ATOUI_BUFSIZE_BITTOMAX_ERR		0xA00E0102U				/*GM_atoui()传入bufsize位数超出最大值*/
#define GM_ATOUI_BUFSIZE_CHAR_ERR			0xA00E0103U				/*当前字符不是数字*/
#define GM_ATOUI_BUFSIZE_RTTOMAX_ERR		0xA00E0104U				/*传入buf大于"4294967295"*/
#define GM_ATOUI_BUFSIZE_2OO3_ERR			0xA00E0105U				/*2OO3不支持使用*/

/**GM_atoi(),0xA00E02XX**/
#define GM_ATOI_PAR_ERR						0xA00E0200U				/*GM_atoi()传入参数有误*/
#define GM_ATOI_BUFSIZE_ZERO_ERR			0xA00E0201U				/*GM_atoi()传入bufsize为0*/
#define GM_ATOI_BUFSIZE_ERR					0xA00E0202U				/*GM_atoui()传入bufsize位数错误*/
#define GM_ATOI_BUFSIZE_CHAR_ERR			0xA00E0203U				/*当前字符不是数字*/
#define GM_ATOI_BUFSIZE_RTTOMAX_ERR			0xA00E0204U				/*传入buf大于最大值*/
#define GM_ATOI_BUFSIZE_2OO3_ERR			0xA00E0205U				/*2OO3不支持使用*/

/*********************DRAM部分(VLE),错误号为0xA00FXXXX******************/
/**GM_DRAM_Write32_A(),0xA00F01XX**/
#define GM_DRAM_WRITE32_A_ERR					0xA00F0100U				/*GM_DRAM_Write32_A()传入参数有误*/
#define GM_DRAM_WRITE32_A_WRITE16_ERR			0xA00F0101U				/*GM_DRAM_Write16_A()返回失败*/
#define GM_DRAM_WRITE32_A_WRITE16_ERR_2			0xA00F0102U				/*GM_DRAM_Write16_A()返回失败*/

/**GM_DRAM_Write64_A(),0xA00F02XX**/
#define GM_DRAM_WRITE64_A_ERR					0xA00F0200U				/*GM_DRAM_Write64_A()传入参数有误*/
#define GM_DRAM_WRITE64_A_WRITE32_ERR			0xA00F0201U				/*GM_DRAM_Write32_A()返回失败*/
#define GM_DRAM_WRITE64_A_WRITE32_ERR_2			0xA00F0202U				/*GM_DRAM_Write32_A()返回失败*/

/**GM_DRAM_Read32_A(),0xA00F03XX**/
#define GM_DRAM_READ32_A_ERR					0xA00F0300U				/*GM_DRAM_Read32_A()传入参数有误*/
#define GM_DRAM_READ32_A_READ16_ERR				0xA00F0301U				/*GM_DRAM_Read32_A()返回失败*/
#define GM_DRAM_READ32_A_READ16_ERR_2			0xA00F0302U				/*GM_DRAM_Read32_A()返回失败*/

/**GM_DRAM_READ64_A(),0xA00F04XX**/
#define GM_DRAM_READ64_A_ERR					0xA00F0400U				/*GM_DRAM_READ64_A()传入参数有误*/
#define GM_DRAM_READ64_A_READ32_ERR				0xA00F0401U				/*GM_DRAM_READ64_A()返回失败*/
#define GM_DRAM_READ64_A_READ32_ERR_2			0xA00F0402U				/*GM_DRAM_READ64_A()返回失败*/

/**GM_DRAM_Write32_B(),0xA00F05XX**/
#define GM_DRAM_WRITE32_B_ERR					0xA00F0500U				/*GM_DRAM_Write32_B()传入参数有误*/
#define GM_DRAM_WRITE32_B_WRITE16_ERR			0xA00F0501U				/*GM_DRAM_Write16_B()返回失败*/
#define GM_DRAM_WRITE32_B_WRITE16_ERR_2			0xA00F0502U				/*GM_DRAM_Write16_B()返回失败*/

/**GM_DRAM_Write64_B(),0xA00F06XX**/
#define GM_DRAM_WRITE64_B_ERR					0xA00F0600U				/*GM_DRAM_Write64_B()传入参数有误*/
#define GM_DRAM_WRITE64_B_WRITE32_ERR			0xA00F0601U				/*GM_DRAM_Write32_B()返回失败*/
#define GM_DRAM_WRITE64_B_WRITE32_ERR_2			0xA00F0602U				/*GM_DRAM_Write32_B()返回失败*/

/**GM_DRAM_Read32_B(),0xA00F07XX**/
#define GM_DRAM_READ32_B_ERR					0xA00F0700U				/*GM_DRAM_Read32_B()传入参数有误*/
#define GM_DRAM_READ32_B_READ16_ERR				0xA00F0701U				/*GM_DRAM_Read32_B()返回失败*/
#define GM_DRAM_READ32_B_READ16_ERR_2			0xA00F0702U				/*GM_DRAM_Read32_B()返回失败*/

/**GM_DRAM_READ64_B(),0xA00F08XX**/
#define GM_DRAM_READ64_B_ERR					0xA00F0800U				/*GM_DRAM_READ64_B()传入参数有误*/
#define GM_DRAM_READ64_B_READ32_ERR				0xA00F0801U				/*GM_DRAM_READ64_B()返回失败*/
#define GM_DRAM_READ64_B_READ32_ERR_2			0xA00F0802U				/*GM_DRAM_READ64_B()返回失败*/

/**GM_DRAM_Init_B(),0xA00F09XX**/
#define GM_DRAM_INIT_B_ERR						0xA00F0900U				/*GM_DRAM_Init_B()传入参数有误*/
#define GM_DRAM_INIT_B_DVCOM_INIT_FAIL			0xA00F0901U				/*Drv_DVCOM_Init()返回失败*/


/*******************DRAMCOM部分,错误号为0xA010XXXX****************/
/**GM_DRAM_Com_Write_A(),0xA01001XX**/
#define GM_DRAM_COM_WRITE_A_PAR_ERR						0xA0100100U				/*GM_DRAM_Com_Write_A()传入参数有误*/
#define GM_DRAM_COM_WRITE_A_DATASIZE_ERR				0xA0100101U				/*max写 - current < 消息内容 + crc16(1) + 消息size(1)*/
#define GM_DRAM_COM_WRITE_A_WRITEDATASIZE_ERR			0xA0100102U				/*当前这条消息的大小返回失败*/
#define GM_DRAM_COM_WRITE_A_CALCRC_ERR					0xA0100103U				/*CRC计算返回失败*/
#define GM_DRAM_COM_WRITE_A_WRITECRC_ERR				0xA0100104U				/*写CRC返回失败*/
#define GM_DRAM_COM_WRITE_A_WRITEDATA_ERR				0xA0100105U				/*写消息内容返回失败*/
#define GM_DRAM_COM_WRITE_A_WRITEMSGNUM_ERR				0xA0100106U				/*写消息个数返回失败*/

/**GM_DRAM_Com_Read_A(),0xA01002XX**/
#define GM_DRAM_COM_READ_A_PAR_ERR						0xA0100200U				/*GM_DRAM_Com_Read_A()初始化未准备好*/
#define GM_DRAM_COM_READ_A_READMSGNUM_ERR				0xA0100201U				/*读DRAM中存放的消息个数返回失败*/
#define GM_DRAM_COM_READ_A_MSGNUM_ERR					0xA0100202U				/*消息个数为0*/
#define GM_DRAM_COM_READ_A_INDEX_ERR					0xA0100203U				/*消息index超过最大值*/
#define GM_DRAM_COM_READ_A_READMSGSIZE_ERR				0xA0100204U				/*读取消息大小失败或大小有错误*/
#define GM_DRAM_COM_READ_A_READCRC_ERR					0xA0100205U				/*读取消息CRC失败*/
#define GM_DRAM_COM_READ_A_MSGSIZETOMAX_ERR				0xA0100206U				/*当前index + 消息内容不大于最大读取index*/
#define GM_DRAM_COM_READ_A_READDATE_ERR					0xA0100207U				/*读取消息内容失败*/
#define GM_DRAM_COM_READ_A_CHECKCRC_ERR					0xA0100208U				/*校验消息CRC失败*/
#define GM_DRAM_COM_READ_A_CALLBACK_ERR					0xA0100209U				/*GM_DRAM_COM_CALL_BACK_FUN_A返回失败*/

/**GM_DRAM_Com_Write_B(),0xA01003XX**/
#define GM_DRAM_COM_WRITE_B_PAR_ERR						0xA0100300U				/*GM_DRAM_Com_Write_B()传入参数有误*/
#define GM_DRAM_COM_WRITE_B_DATASIZE_ERR				0xA0100301U				/*max写 - current < 消息内容 + crc16(1) + 消息size(1)*/
#define GM_DRAM_COM_WRITE_B_WRITEDATASIZE_ERR			0xA0100302U				/*当前这条消息的大小返回失败*/
#define GM_DRAM_COM_WRITE_B_CALCRC_ERR					0xA0100303U				/*CRC计算返回失败*/
#define GM_DRAM_COM_WRITE_B_WRITECRC_ERR				0xA0100304U				/*写CRC返回失败*/
#define GM_DRAM_COM_WRITE_B_WRITEDATA_ERR				0xA0100305U				/*写消息内容返回失败*/
#define GM_DRAM_COM_WRITE_B_WRITEMSGNUM_ERR				0xA0100306U				/*写消息个数返回失败*/

/**GM_DRAM_Com_Read_B(),0xA01004XX**/
#define GM_DRAM_COM_READ_B_PAR_ERR						0xA0100400U				/*GM_DRAM_Com_Read_B()初始化未准备好*/
#define GM_DRAM_COM_READ_B_READMSGNUM_ERR				0xA0100401U				/*读DRAM中存放的消息个数返回失败*/
#define GM_DRAM_COM_READ_B_MSGNUM_ERR					0xA0100402U				/*消息个数为0*/
#define GM_DRAM_COM_READ_B_INDEX_ERR					0xA0100403U				/*消息index超过最大值*/
#define GM_DRAM_COM_READ_B_READMSGSIZE_ERR				0xA0100404U				/*读取消息大小失败或大小有错误*/
#define GM_DRAM_COM_READ_B_READCRC_ERR					0xA0100405U				/*读取消息CRC失败*/
#define GM_DRAM_COM_READ_B_MSGSIZETOMAX_ERR				0xA0100406U				/*当前index + 消息内容不大于最大读取index*/
#define GM_DRAM_COM_READ_B_READDATE_ERR					0xA0100407U				/*读取消息内容失败*/
#define GM_DRAM_COM_READ_B_CHECKCRC_ERR					0xA0100408U				/*校验消息CRC失败*/
#define GM_DRAM_COM_READ_B_CALLBACK_ERR					0xA0100409U				/*GM_DRAM_COM_CALL_BACK_FUN_B返回失败*/


/********************MEMORY部分,错误号为0xA011XXXX****************/
/**GM_Simple_Memory_Ini(),0xA01101XX**/
#define GM_SIMPLE_MEMORY_INI_PAR_ERR					0xA0110100U				/*GM_Simple_Memory_Ini()入参有误*/
#define GM_SIMPLE_MEMORY_INI_ALREADY_ERR				0xA0110101U				/*GM_Simple_Memory_Ini()已执行初始化*/

/********************VERSION部分,错误号为0xA012XXXX****************/
/**GM_Version_Check(),0xA01201XX**/
#define GM_VERSION_CHECK_PAR_ERR					0xA0120100U					/*GM_Version_Check()入参有误*/
#define GM_VERSION_CHECK_LENGTH_ERR					0xA0120101U					/*GM_Version_Check()传入的length有误*/
#define GM_VERSION_CHECK_ATOUI_ERR					0xA0120102U					/*字符串转换为数字出错*/
#define GM_VERSION_CHECK_STRING_ERR					0xA0120103U					/*字符串格式有误*/
#define GM_VERSION_CHECK_VERSIONVALUE_ERR			0xA0120104U					/*字符串版本号值超过最大值*/
#define GM_VERSION_CHECK_DATE_ERR					0xA0120105U					/*日期不合法*/
#define GM_VERSION_CHECK_PERIOD_ERR					0xA0120106U					/*发布阶段不合法*/

/**GM_Version_Date_Decode(),0xA01202XX**/
#define GM_VERSION_DATE_DECODE_PAR_ERR				0xA0120200U					/*GM_Version_Date_Decode()传入参数有误*/
#define GM_VERSION_DATE_DECODE_LENGTH_ERR			0xA0120201U					/*GM_Version_Date_Decode()传入的length不对*/
#define GM_VERSION_DATE_DECODE_CHECK_ERR			0xA0120202U					/*GM_Version_Check()检查不通过*/
#define GM_VERSION_DATE_DECODE_CHECK_STRING_ERR		0xA0120203U					/*字符串格式有误*/
#define GM_VERSION_DATE_DECODE_CHECK_PERIOD_ERR		0xA0120204U					/*发布阶段不合法*/


/**********************MD5部分,错误号为0xA013XXXX*****************/
/**GM_MD5_String(),0xA01301XX**/
#define GM_MD5_STRING_POUTPUTMD5BUF_ERR						0xA0130100U					/*GM_MD5_String()传入参数pOutputMD5Buf有误*/
#define GM_MD5_STRING_INPUTSTRINGLEN_ERR					0xA0130101U					/*GM_MD5_String()传入参数inputString,len有误*/
#define GM_MD5_STRING_INIT_ERR								0xA0130102U					/*GM_MD5_Init()返回失败*/
#define GM_MD5_STRING_UPDATE_ERR							0xA0130103U					/*GM_MD5_Update()返回失败*/
#define GM_MD5_STRING_FINAL_ERR								0xA0130104U					/*GM_MD5_Final()返回失败*/

/**GM_MD5_String_Compare(),0xA01302XX**/
#define GM_MD5_STRING_COMPARE_POUTPUTMD5BUF_ERR				0xA0130200U					/*GM_MD5_String_Compare()传入参数pInputMD5Buf有误*/
#define GM_MD5_STRING_COMPARE_NOT_EQUAL						0xA0130201U					/*输入的字符串与MD5码不匹配*/
#define GM_MD5_STRING_COMPARE_MD5_STRING_EQUAL				0xA0130202U					/*GM_MD5_String()返回失败*/

/**GM_MD5_File(),0xA01303XX**/
#define GM_MD5_FILE_PAR_ERR									0xA0130300U					/*GM_MD5_File()传入参数有误*/
#define GM_MD5_FILE_OPEN_ERR								0xA0130301U					/*fopen()打开文件失败*/
#define GM_MD5_FILE_INIT_ERR								0xA0130302U					/*GM_MD5_Init()返回失败*/
#define GM_MD5_FILE_UPDATE_ERR								0xA0130303U					/*GM_MD5_Update()返回失败*/
#define GM_MD5_FILE_FINAL_ERR								0xA0130304U					/*GM_MD5_Final()返回失败*/

/**GM_MD5_File_Compare(),0xA01304XX**/
#define GM_MD5_FILE_COMPARE_PAR_ERR							0xA0130400U					/*GM_MD5_File_Compare传入参数有误*/
#define GM_MD5_FILE_COMPARE_FILEA_ERR						0xA0130401U					/*GM_MD5_File()计算A的MD5返回失败*/
#define GM_MD5_FILE_COMPARE_FILEB_ERR						0xA0130402U					/*GM_MD5_File()计算B的MD5返回失败*/
#define GM_MD5_FILE_COMPARE_FAIL							0xA0130403U					/*比较返回失败*/

/**GM_MD5_Context_Compare(),0xA01305XX**/
#define GM_MD5_CONTEXT_COMPARE_PAR_ERR						0xA0130500U					/*GM_MD5_Context_Compare()传入参数有误*/
#define GM_MD5_CONTEXT_COMPARE_FAIL							0xA0130501U					/*比较返回失败*/
#define GM_MD5_CONTEXT_COMPARE_FINAL_ERR					0xA0130502U					/*GM_MD5_Final()返回失败*/


/**********************SHA1部分,错误号为0xA014XXXX*****************/
/**GM_SHA1_String(),0xA01401XX**/
#define GM_SHA1_STRING_PAR_ERR								0xA0140100U					/*GM_SHA1_String()传入参数有误*/
#define GM_SHA1_STRING_SHA1_INIT_ERR						0xA0140101U					/*GM_SHA1_Init()返回失败*/
#define GM_SHA1_STRING_SHA1_UPDATE_ERR						0xA0140102U					/*GM_SHA1_Update()返回失败*/
#define GM_SHA1_STRING_SHA1_FINAL_ERR						0xA0140103U					/*GM_SHA1_Final()返回失败*/

/**GM_SHA1_String_Compare(),0xA01402XX**/
#define GM_SHA1_STRING_COMPARE_FAIL							0xA0140200U					/*不相等，比较失败*/
#define GM_SHA1_STRING_COMPARE_STRING_FAIL					0xA0140201U					/*GM_SHA1_String()返回失败*/

/**GM_SHA1_File(),0xA01403XX**/
#define GM_SHA1_FILE_PAR_ERR								0xA0140300U					/*GM_SHA1_File()传入参数有误*/
#define GM_SHA1_FILE_OPEN_ERR								0xA0140301U					/*GM_SHA1_File()打开文件失败*/
#define GM_SHA1_FILE_INIT_ERR								0xA0140302U					/*GM_SHA1_Init()返回失败*/
#define GM_SHA1_FILE_UPDATE_ERR								0xA0140303U					/*GM_SHA1_Update()返回失败*/
#define GM_SHA1_FILE_FINAL_ERR								0xA0140304U					/*GM_SHA1_Final()返回失败*/

/**GM_SHA1_File_Compare(),0xA01404XX**/
#define GM_SHA1_FILE_COMPARE_PAR_ERR						0xA0140400U					/*GM_SHA1_File_Compare()传入参数有误*/
#define GM_SHA1_FILE_COMPARE_FAIL							0xA0140401U					/*GM_SHA1_File_Compare()传入参数有误*/
#define GM_SHA1_FILE_COMPARE_FILE_B_ERR						0xA0140402U					/*计算filePathB的SHA1值返回失败*/
#define GM_SHA1_FILE_COMPARE_FILE_A_ERR						0xA0140403U					/*计算filePathA的SHA1值返回失败*/


/**********************CODE部分,错误号为0xA015XXXX*****************/
/**GM_Code_HEX2INT(),0xA01501XX**/
#define GM_CODE_HEX2INT_PAR_ERR								0xA0150100U					/*GM_Code_HEX2INT()传入参数有误*/
#define GM_CODE_HEX2INT_HEX_ERR								0xA0150101U					/*pHEX不满足十六进制数的格式要求*/


/*******************MULTITHREAD部分,错误号为0xA016XXXX**************/
/**GM_Thread_Create(),0xA01601XX**/
#define GM_THREAD_CREATE_WIN32_STACKSIZE_ERR				0xA0160100U					/*WIN32下，传入stackSize小于0*/
#define GM_THREAD_CREATE_WIN32_PRIORITY_ERR					0xA0160101U					/*WIN32下，传入priority有误*/
#define GM_THREAD_CREATE_WIN32_SETPRIORITY_ERR				0xA0160102U					/*WIN32下，设置线程优先级返回失败*/
#define GM_THREAD_CREATE_WIN32_BEGINTHREADEX_ERR			0xA0160103U					/*WIN32下，创建线程返回失败*/

#define GM_THREAD_CREATE_QNX_PAR_ERR						0xA0160104U					/*QNX下，传入参数有误*/
#define GM_THREAD_CREATE_QNX_SETINHERITSCHED_ERR			0xA0160105U					/*QNX下，pthread_attr_setinheritsched返回失败*/
#define GM_THREAD_CREATE_QNX_SETSCHEDPARAM_ERR				0xA0160106U					/*QNX下，pthread_attr_setschedparam返回失败*/
#define GM_THREAD_CREATE_QNX_SETSTACKSIZE_ERR				0xA0160107U					/*QNX下，pthread_attr_setstacksize返回失败*/
#define GM_THREAD_CREATE_QNX_SETGUARDSIZE_ERR				0xA0160108U					/*QNX下，pthread_attr_setguardsize返回失败*/
#define GM_THREAD_CREATE_QNX_PTHREADCREATE_ERR				0xA0160109U					/*QNX下，pthread_create返回失败*/
#define GM_THREAD_CREATE_QNX_PAR_GUARDSIZE_ERR				0xA016010AU					/*QNX下，传入参数guardSize有误*/

#define GM_THREAD_CREATE_VXWORKS_PAR_ERR					0xA016010BU					/*VXWORKS下，传入参数有误*/
#define GM_THREAD_CREATE_VXWORKS_TASKSPAWN_ERR				0xA016010CU					/*VXWORKS下，taskSpawn返回失败*/


/********************NETWORK部分,错误号为0xA017XXXX***************/
/**GM_IP_Set(),0xA01701XX**/
#define GM_IP_SET_PAR_ERR								0xA0170100U						/*GM_IP_Set()传入参数有误*/
#define GM_IP_SET_VXWORKS_IPATTACH_ERR					0xA0170101U						/*Vxworks下，ipAttach()返回错误*/
#define GM_IP_SET_VXWORKS5_5_INETADDR_ERR				0xA0170102U						/*Vxworks5_5下，inet_addr()返回错误*/
#define GM_IP_SET_VXWORKS5_5_IFMASKSET_ERR				0xA0170103U						/*Vxworks5_5下，ifMaskSet()返回错误*/
#define GM_IP_SET_VXWORKS5_5_IFADDRSET_ERR				0xA0170104U						/*Vxworks5_5下，ifAddrSet()返回错误*/
#define GM_IP_SET_VXWORKS6_6_IFCONFIG_ERR				0xA0170105U						/*Vxworks6_6下，ifconfig()返回错误*/
#define GM_IP_SET_QNX_SYSTEM_ERR						0xA0170106U						/*QNX下，system()返回错误*/

/**GM_IP_GW_Set(),0xA01702XX**/
#define GM_IP_GW_SET_PAR_ERR							0xA0170200U						/*GM_IP_GW_Set()传入参数有误*/
#define GM_IP_GW_SET_VXWORKS5_5_MROUTEADD_ERR			0xA0170201U						/*VXWORKS5_5下，mRouteAdd()返回失败*/
#define GM_IP_GW_SET_VXWORKS6_6_GATEMASK_ERR			0xA0170202U						/*VXWORKS6_6下,pGateWayMask为空*/
#define GM_IP_GW_SET_VXWORKS6_6_ROUTEC_ERR				0xA0170203U						/*VXWORKS6_6下,routec()返回失败*/
#define GM_IP_GW_SET_QNX_GATEMASK_ERR					0xA0170204U						/*QNX下,pGateWayMask为空*/
#define GM_IP_GW_SET_QNX_SYSTEM_ERR						0xA0170205U						/*QNX下,system()返回错误*/

/**GM_Socket_Create(),0xA01703XX**/
#define GM_SOCKET_CREATE_PAR_ERR						0xA0170300U						/*GM_Socket_Create()传入参数有误*/
#define GM_SOCKET_CREATE_WIN32_WSAIOCTL_ERR				0xA0170301U						/*WIN32下，WSAIoctl()返回失败*/
#define GM_SOCKET_CREATE_SOCKET_ERR						0xA0170302U						/*socket()创建失败*/

/**GM_TCP_Create_Socket(),0xA01704XX**/
#define GM_TCP_CREATE_SOCKET_PAR_ERR					0xA0170400U						/*GM_TCP_Create_Socket()传入参数有误*/
#define GM_TCP_CREATE_SOCKET_FAIL						0xA0170401U						/*GM_TCP_Create_Socket()创建socket失败*/
#define GM_TCP_CREATE_SOCKET_IOCTL_ERR					0xA0170402U						/*ioctl()返回失败*/
#define GM_TCP_CREATE_SOCKET_BIND_ERR					0xA0170403U						/*bind()返回失败*/

/**GM_UDP_Is_Read_Ready(),0xA01705XX**/
#define	GM_UDP_IS_READ_READY_PAR_ERR					0xA0170500U						/*GM_UDP_Is_Read_Ready()传入参数有误*/
#define	GM_UDP_IS_READ_READY_SELECT_ERR					0xA0170501U						/*select()返回失败*/
#define	GM_UDP_IS_READ_READY_FD_ISSET_ERR				0xA0170502U						/*FD_ISSET()返回失败*/

/**GM_UDP_Is_Write_Ready(),0xA01706XX**/
#define GM_UDP_IS_WRITE_READY_PAR_ERR					0xA0170600U						/*GM_UDP_Is_Write_Ready()传入参数有误*/
#define	GM_UDP_IS_WRITE_READY_SELECT_ERR				0xA0170601U						/*select()返回失败*/
#define	GM_UDP_IS_WRITE_READY_FD_ISSET_ERR				0xA0170602U						/*FD_ISSET()返回失败*/

/**GM_UDP_Socket_Create(),0xA01707XX**/
#define GM_UDP_SOCKET_CREATE_PAR_ERR					0xA0170700U						/*GM_UDP_Socket_Create()传入参数有误*/
#define GM_UDP_SOCKET_CREATE_FAIL						0xA0170701U						/*GM_UDP_Socket_Create()创建socket失败*/
#define GM_UDP_SOCKET_CREATE_IOCTL_ERR					0xA0170702U						/*ioctl()返回失败*/
#define GM_UDP_SOCKET_CREATE_BIND_ERR					0xA0170703U						/*bind()返回失败*/

/**GM_UDP_Send_Msg(),0xA01708XX**/
#define GM_UDP_SEND_MSG_PAR_ERR							0xA0170800U						/*GM_UDP_Send_Msg()传入参数有误*/
#define GM_UDP_SEND_MSG_IS_WRITE_READY_ERR				0xA0170801U						/*GM_UDP_Is_Write_Ready()传入参数有误*/
#define GM_UDP_SEND_MSG_SENDTO_ERR						0xA0170802U						/*sendto()返回失败*/

/**GM_UDP_Recv_Msg(),0xA01709XX**/
#define GM_UDP_RECV_MSG_PAR_ERR							0xA0170900U						/*GM_UDP_Recv_Msg()传入参数有误*/
#define GM_UDP_RECV_MSG_RECVNUM_ERR						0xA0170901U						/*recvfrom返回错误*/


/*********************SNTP部分,错误号为0xA018XXXX****************/
/*GM_SNTP_Client_Update_Time(),0xA01801XX*/
#define GM_SNTP_CLIENT_UPDATE_TIME_H2N_ERR				0xA0180100U						/*GM_SNTP_Msg_H2N_N2H()返回失败*/
#define GM_SNTP_CLIENT_UPDATE_TIME_CLRBUF_ERR			0xA0180101U						/*GM_UDP_UnBlock_Clear_Buffer()返回失败*/
#define GM_SNTP_CLIENT_UPDATE_TIME_N2H_ERR				0xA0180102U						/*GM_SNTP_Msg_H2N_N2H()返回失败*/
#define GM_SNTP_CLIENT_UPDATE_TIME_SELECT_ERR			0xA0180103U						/*select()返回失败*/
#define GM_SNTP_CLIENT_UPDATE_TIME_RECV_ERR				0xA0180104U						/*未接收到正确数据*/
#define GM_SNTP_CLIENT_UPDATE_TIME_N2H_ERR_2			0xA0180105U						/*GM_SNTP_Msg_H2N_N2H()返回失败*/

/*GM_SNTP_Client_Init(),0xA01802XX*/
#define GM_SNTP_CLIENT_INIT_PAR_ERR						0xA0180200U						/*GM_SNTP_Client_Init()传入参数有误*/
#define GM_SNTP_CLIENT_INIT_UDPINIT_ERR					0xA0180201U						/*GM_UDP_Init()返回失败*/
#define GM_SNTP_CLIENT_INIT_SOCKETCREAT_ERR				0xA0180202U						/*GM_UDP_Socket_Create()返回失败*/
#define GM_SNTP_CLIENT_INIT_THREADCREAT_ERR				0xA0180203U						/*GM_Thread_Create()返回失败*/


/***********************PD部分,错误号为0xA019XXXX****************/
/*GM_PD_Operation(),0xA01901XX*/
#define GM_PD_OPERATION_PAR_ERR							0xA0190100U						/*GM_PD_Operation()传入参数有误*/
#define GM_PD_OPERATION_ISALGORITHMA_ERR				0xA0190101U						/*GM_PD_Operation()传入isAlgorithmA有误*/

/*GM_PD_Operation2(),0xA01902XX*/
#define GM_PD_OPERATION2_PAR_ERR						0xA0190200U						/*GM_PD_Operation2()传入参数有误*/
#define GM_PD_OPERATION2_ISALGORITHMA_ERR				0xA0190201U						/*GM_PD_Operation2()传入isAlgorithmA有误*/

/*GM_PD_Inverse(),0xA01903XX*/
#define GM_PD_INVERSE_PAR_ERR							0xA0190300U						/*GM_PD_Inverse()传入参数有误*/
#define GM_PD_INVERSE_ISALGORITHMA_ERR					0xA0190301U						/*GM_PD_Inverse()传入isAlgorithmA有误*/


/***********************LFSR部分,错误号为0xA01AXXXX****************/
/**暂无**/


/***********************WD部分,错误号为0xA01BXXXX****************/
/**GM_WD_Start(),0xA01B01XX**/
#define GM_WD_START_QNX_PAR_ERR						0xA01B0100U						/*QNX下,传入参数period有误*/
#define GM_WD_START_QNX_WDINIT_ERR					0xA01B0101U						/*QNX下,watchDogInit()返回失败*/
#define GM_WD_START_VXWORKS6_6_PAR_ERR				0xA01B0102U						/*VXWORKS6_6下,传入参数period有误*/
#define GM_WD_START_VXWORKS6_6_WDINIT_ERR			0xA01B0103U						/*VXWORKS6_6下,sysPldWdgInit()返回失败*/
#define GM_WD_START_VXWORKS6_6_WDRESSET_ERR			0xA01B0104U						/*VXWORKS6_6下,sysPldWdgResSet()返回失败*/
#define GM_WD_START_VXWORKS6_6_WDSTART_ERR			0xA01B0105U						/*VXWORKS6_6下,sysPldWdgResSet()返回失败*/


/***********************VCPTOCRC部分,错误号为0xA01CXXXX****************/
/**GM_VCP2CRC_Get_Cnf(),0xA01C01XX**/
#define GM_VCP2CRC_GET_CNF_INIT_ERR					0xA01C0100U						/*获取配置数据时未进行初始化*/

/**GM_VCP2CRC_Init(),0xA01C02XX**/
#define GM_VCP2CRC_INIT_PAR_ERR						0xA01C0200U						/*初始化时传入参数有误*/
#define GM_VCP2CRC_INIT_DATALENGTH_ERR				0xA01C0201U						/*传入配置文件数据长度与配置文件中记录的不一致*/
#define GM_VCP2CRC_INIT_MD5_CAL_ERR					0xA01C0202U						/*计算配置文件MD5值返回错误*/
#define GM_VCP2CRC_INIT_MD5_CMP_ERR					0xA01C0203U						/*比较MD5不一致*/
#define GM_VCP2CRC_INIT_MALLOC_ERR					0xA01C0204U						/*申请内存失败*/

/**GM_VCP2CRC_Encode_Msg(),0xA01C03XX**/
#define GM_VCP2CRC_ENCODE_MSG_INIT_ERR				0xA01C0300U						/*未进行正常的初始化操作*/
#define GM_VCP2CRC_ENCODE_MSG_PAR_ERR				0xA01C0301U						/*传入的参数有误*/
#define GM_VCP2CRC_ENCODE_MSG_CRCMODE_ERR			0xA01C0302U						/*传入的CRC_Mode有误*/
#define GM_VCP2CRC_ENCODE_MSG_MSGTYPE_ERR			0xA01C0303U						/*未找到对应的msg_type*/
#define GM_VCP2CRC_ENCODE_MSG_VARNUM_ERR			0xA01C0304U						/*传入的VarNum大于该类型消息变量最大数*/
#define GM_VCP2CRC_ENCODE_MSG_MALLOC_ERR			0xA01C0305U						/*申请内存失败*/
#define GM_VCP2CRC_ENCODE_MSG_GET_CDWLEN_ERR		0xA01C0306U						/*GM_VCP2CRC_Get_Cdw_Length()返回失败*/
#define GM_VCP2CRC_ENCODE_MSG_CAL_CRC32_ERR			0xA01C0307U						/*GM_VCP2CRC_Calculate_CRC32()返回失败*/

/**GM_VCP2CRC_Decode_Msg(),0xA01C04XX**/
#define GM_VCP2CRC_DECODE_MSG_INIT_ERR				0xA01C0400U						/*未进行正常的初始化操作*/
#define GM_VCP2CRC_DECODE_MSG_PAR_ERR				0xA01C0401U						/*传入的参数有误*/
#define GM_VCP2CRC_DECODE_MSG_CRCMODE_ERR			0xA01C0402U						/*传入的CRC_Mode有误*/
#define GM_VCP2CRC_DECODE_MSG_MSGTYPE_ERR			0xA01C0403U						/*未找到对应的msg_type*/
#define GM_VCP2CRC_DECODE_MSG_MALLOC_ERR			0xA01C0404U						/*申请内存失败*/
#define GM_VCP2CRC_DECODE_MSG_GET_CDWLEN_ERR		0xA01C0405U						/*GM_VCP2CRC_Get_Cdw_Length()返回失败*/
#define GM_VCP2CRC_DECODE_MSG_VARINDEX_ERR			0xA01C0406U						/*传入的VCP_VarIndex[i]大于该类型消息变量最大数*/
#define GM_VCP2CRC_DECODE_MSG_CAL_CRC32_ERR			0xA01C0407U						/*GM_VCP2CRC_Calculate_CRC32()返回失败*/
#define GM_VCP2CRC_DECODE_MSG_CHECKCRC_ERR			0xA01C0408U						/*CRC校验失败*/

/**GM_VCP2CRC_Calculate_CRC32(),0xA01C05XX**/
#define GM_VCP2CRC_CALCULATE_CRC32_PAR_ERR			0xA01C0500U						/*传入形参有误*/
#define GM_VCP2CRC_CALCULATE_CRC32_VARINDEX_ERR		0xA01C0501U						/*VCP_VarIndex有误*/
#define GM_VCP2CRC_CALCULATE_CRC32_VARNUM_ERR		0xA01C0502U						/*计算出的变量数大于VarNum*/

/**GM_VCP2CRC_Get_Cdw_Length(),0xA01C06XX**/
#define GM_VCP2CRC_GET_CDW_LENGTH_PAR_ERR			0xA01C0600U						/*传入形参有误*/
#define GM_VCP2CRC_GET_CDW_LENGTH_VARINDEX_ERR		0xA01C0601U						/*VCP_VarIndex数据错误导致变量长度大于4*/

/**VCP_check(),0xA01C07XX,仅在DEBUG模式下有效**/
#define GM_VCP2CRC_VCP_CHECK_FAIL					0xA01C0700U						/*VCP签名校验失败*/



/******GM_Utils中错误号作为返回值部分，定义错误号为0xA1XXXXXX，共108个**********/
/********消息队列MQ部分,错误号为0xA101XXXX********/

/*GM_MQ、GM_MQ_Light、GM_MQ_Fixed进行Init操作的返回值，错误号为0xA10101XX*/
#define MQ_INIT_OK              0xA10101FFU          /*Init操作成功*/
#define MQ_INIT_PAR_ERR         0xA1010100U          /*Init入参错误*/
#define MQ_INIT_MALLOC_ERR      0xA1010101U          /*Init申请内存失败*/
#define MQ_INIT_MUTEX_ERR       0xA1010102U          /*Init初始化MQ互斥器失败*/

/*GM_MQ、GM_MQ_Light、GM_MQ_Fixed进行Read操作的返回值，错误号为0xA10102XX*/
#define MQ_RD_OK                0xA10102FFU          /*Read操作成功*/
#define MQ_RD_PAR_NULL          0xA1010200U          /*Read入参指针为空*/
#define MQ_RD_INIT_ERR          0xA1010201U          /*Read操作的MQ未初始化*/
#define MQ_RD_LOCK_ERR          0xA1010202U          /*Read获得MQ互斥器资源失败*/
#define MQ_RD_UNLOCK_ERR        0xA1010203U          /*Read释放MQ互斥器资源失败*/
#define MQ_RD_MQ_EMPTY          0xA1010204U          /*Read操作MQ为空*/
#define MQ_RD_MSGSIZE_ERR       0xA1010205U          /*Read存放消息缓冲区太小*/

/*GM_MQ、GM_MQ_Light、GM_MQ_Fixed进行Get操作的返回值，错误号为0xA10103XX*/
#define MQ_GET_OK               0xA10103FFU          /*Get操作成功*/
#define MQ_GET_PAR_NULL         0xA1010300U          /*Get入参指针为空*/
#define MQ_GET_INIT_ERR         0xA1010301U          /*Get操作的MQ未初始化*/
#define MQ_GET_LOCK_ERR         0xA1010302U          /*Get获得MQ互斥器资源失败*/
#define MQ_GET_UNLOCK_ERR       0xA1010303U          /*Get释放MQ互斥器资源失败*/
#define MQ_GET_INDEX_ERR        0xA1010304U          /*Get操作参数index大于实际消息数*/
#define MQ_GET_MSGSIZE_ERR      0xA1010305U          /*Get存放消息缓冲区太小*/

/*GM_MQ、GM_MQ_Light、GM_MQ_Fixed进行Write操作的返回值，错误号为0xA10104XX*/
#define MQ_WR_OK                0xA10104FFU          /*Write操作成功*/
#define MQ_WR_PAR_NULL          0xA1010400U          /*Write入参指针为空*/
#define MQ_WR_INIT_ERR          0xA1010401U          /*Write操作的MQ未初始化*/
#define MQ_WR_MSGSIZE_ERR       0xA1010402U          /*Write操作的消息size为0或大于所允许的最大值*/
#define MQ_WR_LOCK_ERR          0xA1010403U          /*Write获得MQ互斥器资源失败*/
#define MQ_WR_UNLOCK_ERR        0xA1010404U          /*Write释放MQ互斥器资源失败*/
#define MQ_WR_MQ_FULL           0xA1010405U          /*Write操作MQ已满*/



/********GAPP/RMS/RMS_UDP部分,错误号为0xA102XXXX********/
/*GAPP层函数返回值，错误号为0xA10201XX*/
#define GAPP_ADD_OK                    0xA10201FFU          /*GAPP层新节点添加成功*/
#define GAPP_ADD_ALREADY               0xA10201EEU          /*GAPP层已经添加过该节点*/
#define GAPP_INTERWRITE_OK             0xA10201DDU          /*GM_GAPP_Inter_Write函数返回成功*/
#define GAPP_INITS_OK                  0xA10201CCU          /*GM_GAPP_Init_S初始化成功*/
#define GAPP_WRITE_OK                  0xA10201BBU          /*GM_GAPP_WRITE返回成功*/
#define GAPP_INIT_FROM_ADDR_OK         0xA10201AAU          /*GM_GAPP_Init_From_Addr初始化成功*/
#define GAPP_PAR_ERR                   0xA1020100U          /*传入GAPP的消息格式有错误或RMS层未完成初始化*/
#define GAPP_NODE_FULL                 0xA1020101U          /*GAPP层动态添加节点数已达到最大值*/
#define GAPP_CHECK_ERR                 0xA1020102U          /*GAPP层验证失败*/
#define GAPP_ADD_ERR                   0xA1020103U          /*GAPP层动态添加失败*/
#define GAPP_MSG_CHANGE_ERR            0xA1020104U          /*网络字节序和主机字节序转换失败*/
#define GAPP_INTERWRITE_PAR_ERR        0xA1020105U          /*GM_GAPP_Inter_Write传入的参数有误*/
#define GAPP_INTERWRITE_NOT_FOUND      0xA1020106U          /*GM_GAPP_Inter_Write未找到该节点相关信息*/
#define GAPP_INTERWRITE_CHECK_ERR      0xA1020107U          /*GM_GAPP_Inter_Write对收到的消息进行验证失败*/
#define GAPP_INTERWRITE_MSGID_ERR      0xA1020108U          /*GM_GAPP_Inter_Write收到FSFB2消息的MsgID不正确*/
#define GAPP_INTERWRITE_CALLBACK_ERR   0xA1020109U          /*GM_GAPP_Inter_Write中回调函数返回失败*/
#define GAPP_WRITE_PAR_ERR             0xA102010AU          /*GM_GAPP_Write_S未初始化或传入参数错误*/
#define GAPP_WRITE_NOT_FOUND           0xA102010BU          /*GM_GAPP_Write_S未找到该节点的配置内容*/
#define GAPP_WRITE_MSGID_ERR           0xA102010CU          /*GM_GAPP_Write_S待发送消息的MsgID不正确*/
#define GAPP_WRITE_CALLBACK_ERR        0xA102010DU          /*GM_GAPP_Write_S中GM_GAPP_Write_Callback_Fun返回失败*/
#define GAPP_INITS_PAR_ERR             0xA102010EU          /*GM_GAPP_Init_S传入参数指针为空*/
#define GAPP_INITS_FILESIZE_ERR        0xA102010FU          /*GM_GAPP_Init_S获得配置文件大小为0或负数*/
#define GAPP_INITS_MALLOC_ERR          0xA1020110U          /*GM_GAPP_Init_S申请内存失败*/
#define GAPP_INITS_LOAD_FILE_ERR       0xA1020111U          /*GM_GAPP_Init_S读取配置文件失败*/
#define GAPP_INITS_READ_FILE_ERR       0xA1020112U          /*GM_GAPP_Init_S从配置文件中读取数据失败*/
#define GAPP_INITS_UNIQUE_CHECK_ERR    0xA1020113U          /*GM_GAPP_Init_S唯一性检查失败*/     
#define GAPP_INITS_MUTEX_ERR           0xA1020114U          /*GM_GAPP_Init_S初始化mutex失败*/
#define GAPP_INITADDR_PAR_ERR          0xA1020115U          /*GM_GAPP_Init_From_Addr传入指针为空*/
#define GAPP_INITADDR_GETHOST_ERR      0xA1020116U          /*GM_GAPP_Init_From_Addr中GM_Multibin_GetHost失败*/
#define GAPP_INITADDR_MALLOC_ERR       0xA1020117U          /*GM_GAPP_Init_From_Addr申请内存失败*/
#define GAPP_INITADDR_MUTEX_ERR        0xA1020118U          /*GM_GAPP_Init_From_Addr初始化mutex失败*/

/*RMS层函数返回值，错误号为0xA10202XX*/
#define RMS_ADD_OK                    0xA10202FFU          /*RMS层新节点添加成功*/
#define RMS_ADD_ALREADY               0xA10202EEU          /*RMS层已经添加过该节点*/
#define RMS_INTERWRITE_OK             0xA10202DDU          /*GM_RMS_Inter_Write返回成功*/
#define RMS_INITS_OK                  0xA10202CCU          /*GM_RMS_Init_S初始化成功*/
#define RMS_INIT_FROM_ADDR_OK         0xA10202BBU          /*GM_RMS_Init_From_Addr初始化成功*/
#define RMS_PAR_ERR                   0xA1020200U          /*传入RMS的消息格式有错误或RMS层未完成初始化*/
#define RMS_NODE_FULL                 0xA1020201U          /*RMS层动态添加节点数已达到最大值*/
#define RMS_CHECK_ERR                 0xA1020202U          /*RMS层验证失败*/
#define RMS_ADD_ERR                   0xA1020203U          /*RMS层动态添加失败*/
#define RMS_MSG_CHANGE_ERR            0xA1020204U          /*网络字节序和主机字节序转换失败*/
#define RMS_INTERWRITE_PAR_ERR        0xA1020205U          /*GM_RMS_Inter_Write传入的参数有误*/
#define RMS_INTERWRITE_CHECK_ERR      0xA1020206U          /*GM_RMS_Inter_Write对收到的消息进行验证失败*/
#define RMS_INTERWRITE_DATASIZE_ERR   0xA1020207U          /*GM_RMS_Inter_Write消息头部头部大小不一致*/
#define RMS_INTERWRITE_RESET_TAB_ERR  0xA1020208U          /*GM_RMS_Inter_Write中GM_RMS_SNTable_Remote_Reset失败*/
#define RMS_INTERWRITE_RESET_PAR_ERR  0xA1020209U          /*GM_RMS_Inter_Write中SN=0,但消息长度有误*/
#define RMS_INITS_PAR_ERR             0xA102020AU          /*GM_RMS_Init_S传入参数指针为空*/
#define RMS_INITS_FILESIZE_ERR        0xA102020BU          /*GM_RMS_Init_S获得配置文件大小为0或负数*/
#define RMS_INITS_MALLOC_ERR          0xA102020CU          /*GM_RMS_Init_S申请内存失败*/
#define RMS_INITS_LOAD_FILE_ERR       0xA102020DU          /*GM_RMS_Init_S读取配置文件失败*/
#define RMS_INITS_READ_FILE_ERR       0xA102020EU          /*GM_RMS_Init_S从配置文件中读取数据失败*/
#define RMS_INITS_MUTEX_ERR           0xA102020FU          /*GM_RMS_Init_S初始化mutex失败*/
#define RMS_INITADDR_PAR_ERR          0xA1020210U          /*GM_RMS_Init_From_Addr传入指针为空*/
#define RMS_INITADDR_GETHOST_ERR      0xA1020211U          /*GM_RMS_Init_From_Addr中GM_Multibin_GetHost失败*/
#define RMS_INITADDR_MUTEX_ERR        0xA1020212U          /*GM_RMS_Init_From_Addr初始化mutex失败*/

/*RMS_UDP层函数返回值，错误号为0xA10203XX*/
#define RMS_UDP_ADD_OK                0xA10203FFU         /*GM_RMS_UDP层成功添加动态节点*/
#define RMS_UDP_CHECK_SRCADDR_OK      0xA10203EEU         /*GM_RMS_UDP_Check_SrcAddr验证通过*/
#define RMS_UDP_GETCURMSG_OK          0xA10203DDU         /*GM_RMS_UDP_GetCurMsg_Src获得消息来源index CON和红蓝网属性成功*/
#define RMS_UDP_INITS_OK              0xA10203CCU         /*GM_RMS_UDP_Init_S初始化成功*/
#define RMS_UDP_INIT_FROM_ADDR_OK     0xA10203BBU         /*GM_RMS_UDP_Init_From_Addr初始化成功*/
#define RMS_UDP_PAR_NULL              0xA1020300U         /*入参指针为空*/
#define RMS_UDP_MUTEX_LOCK_ERR        0xA1020301U         /*RMS_UDP层获得mutex失败*/
#define RMS_UDP_MUTEX_UNLOCK_ERR      0xA1020302U         /*RMS_UDP层释放mutex失败*/
#define RMS_UDP_CHECK_NEW_NODE        0xA1020303U         /*GM_RMS_UDP_Check_SrcAddr验证发现是新节点*/
#define RMS_UDP_NODE_FULL             0xA1020304U         /*RMS_UDP层动态添加节点数已达到最大值*/
#define RMS_UDP_ADD_ERR               0xA1020305U         /*RMS_UDP层动态添加失败*/
#define RMS_UDP_CHECK_ERR             0xA1020306U         /*GM_RMS_UDP_Check_SrcAddr验证该节点不在本地配置表*/
#define RMS_UDP_GETCURMSG_PAR_ERR     0xA1020307U         /*GM_RMS_UDP_GetCurMsg_Src传入的参数指针为空或者sCurMsgSrcIndex为0xFF*/ 
#define RMS_UDP_INITS_PAR_ERR         0xA1020308U         /*GM_RMS_UDP_Init_S传入参数指针为空*/
#define RMS_UDP_INITS_UDP_INIT_ERR    0xA1020309U         /*GM_RMS_UDP_Init_S初始化GM_UDP_Init失败*/
#define RMS_UDP_INITS_FILESIZE_ERR    0xA102030AU         /*GM_RMS_UDP_Init_S获得配置文件大小为0或负数*/
#define RMS_UDP_INITS_MALLOC_ERR      0xA102030BU         /*GM_RMS_UDP_Init_S申请内存失败*/
#define RMS_UDP_INITS_LOAD_FILE_ERR   0xA102030CU        /*GM_RMS_UDP_Init_S读取配置文件失败*/
#define RMS_UDP_INITS_READ_FILE_ERR   0xA102030DU        /*GM_RMS_UDP_Init_S从配置文件中读取数据失败*/
#define RMS_UDP_INITS_SOCKET_ERR      0xA102030EU        /*GM_RMS_UDP_Init_S创建UDP_SOCKET失败*/
#define RMS_UDP_INITS_THREAD_ERR      0xA102030FU        /*GM_RMS_UDP_Init_S创建接收数据监听任务失败*/
#define RMS_UDP_INITS_MUTEX_ERR       0xA1020310U        /*GM_RMS_UDP_Init_S初始化mutex失败*/
#define RMS_UDP_INITADDR_PAR_ERR      0xA1020311U        /*GM_RMS_UDP_Init_From_Addr传入指针为空*/
#define RMS_UDP_INITADDR_GETHOST_ERR  0xA1020312U        /*GM_RMS_UDP_Init_From_Addr中GM_Multibin_GetHost失败*/
#define RMS_UDP_INITADDR_UDP_INIT_ERR 0xA1020313U        /*GM_RMS_UDP_Init_From_Addr初始化GM_UDP_Init失败*/
#define RMS_UDP_INITADDR_SOCKET_ERR   0xA1020314U        /*GM_RMS_UDP_Init_From_Addr创建UDP_SOCKET失败*/
#define RMS_UDP_INITADDR_THREAD_ERR   0xA1020315U        /*GM_RMS_UDP_Init_From_Addr创建接收数据监听任务失败*/
#define RMS_UDP_INITADDR_MUTEX_ERR    0xA1020316U        /*GM_RMS_UDP_Init_From_Addr初始化mutex失败*/

/***************************************************************************/
/*                           END of GM_Utils                               */
/***************************************************************************/




#endif