/**
* @file GM_Error.h
* @brief ����ͨ��ģ��ķ���ֵ�����
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
/*              GM_Utils����Ų��֣�����Ϊ0xA0XXXXXX����462��            */
/* ����ĳ�������Ϊ0xA0010203,����A0ΪGM_Utils���ֵĴ���������ţ�01���� */
/* ��λGM_Utils�е�ģ�飬02������λ��Ӧ�ĺ�����03��ʾ�����ڲ��Ĵ�������  */
/*************************************************************************/

/******************FILE����,�����Ϊ0xA001XXXX****************/
/**GM_Bin_File_Load(),0xA00101XX**/
#define  GM_BIN_FILE_LOAD_PAR_ERR		0xA0010100U			/*GM_Bin_File_Load�����������*/
#define  GM_BIN_FILE_LOAD_OPEN_ERR		0xA0010101U			/*GM_Bin_File_Load���ļ�ʧ��*/
#define  GM_BIN_FILE_LOAD_LEN_ERR		0xA0010102U			/*GM_Bin_File_Load�ļ����ȴ���*/
#define  GM_BIN_FILE_LOAD_READ_ERR		0xA0010103U			/*GM_Bin_File_Load��ȡ����ʧ��*/
#define  GM_BIN_FILE_LOAD_CLOSE_ERR		0xA0010104U			/*GM_Bin_File_Load�ر��ļ�ʧ��*/

/**GM_Ini_File_Partly_Load(),0xA00102XX**/
#define  GM_INI_FILE_PARTLY_LOAD_PAR_ERR		0xA0010200U			/*GM_Ini_File_Partly_Load�����������*/
#define  GM_INI_FILE_PARTLY_LOAD_OPEN_ERR		0xA0010201U			/*GM_Ini_File_Partly_Load���ļ�ʧ��*/
#define  GM_INI_FILE_PARTLY_LOAD_CONTENT_ERR	0xA0010202U			/*GM_Ini_File_Partly_Load��ȡ{�����ݹ���*/
#define  GM_INI_FILE_PARTLY_LOAD_BUFFER_ERR		0xA0010203U			/*GM_Ini_File_Partly_Load�����buffer������*/
#define  GM_INI_FILE_PARTLY_LOAD_CLOSE_ERR		0xA0010204U			/*GM_Ini_File_Partly_Load�ر��ļ�ʧ��*/
#define  GM_INI_FILE_PARTLY_LOAD_NOT_FIND_ERR	0xA0010205U			/*GM_Ini_File_Partly_Loadδ�ҵ���Ӧ����*/

/**GM_Ini_File_Load(),0xA00103XX**/
#define  GM_INI_FILE_LOAD_PAR_ERR			0xA0010300U			/*GM_Ini_File_Load�����������*/
#define  GM_INI_FILE_LOAD_OPEN_ERR			0xA0010301U			/*GM_Ini_File_Load���ļ�ʧ��*/
#define  GM_INI_FILE_LOAD_CONTENT_ERR		0xA0010302U			/*GM_Ini_File_Load��ȡ���ݹ���*/
#define  GM_INI_FILE_LOAD_CLOSE_ERR			0xA0010303U			/*GM_Ini_File_Load�ر��ļ�ʧ��*/

/**GM_RSSP1_Ini_File_Read_String()��0xA00104XX**/
#define  GM_INI_FILE_READ_STRING_PAR_ERR		0xA0010400U			/*GM_RSSP1_Ini_File_Read_String�����������*/
#define  GM_INI_FILE_READ_STRING_NOT_FIND_ERR	0xA0010401U			/*GM_RSSP1_Ini_File_Read_Stringû���ҵ�key*/
#define  GM_INI_FILE_READ_STRING_SIZE_ERR		0xA0010402U			/*GM_RSSP1_Ini_File_Read_String�����size̫С*/

/**GM_Ini_File_Read_UInt(),0xA00105XX**/
#define  GM_INI_FILE_READ_UINT_PAR_ERR			0xA0010500U			/*GM_Ini_File_Read_UInt�����������*/
#define  GM_INI_FILE_READ_UINT_STRING_ERR		0xA0010501U			/*GM_RSSP1_Ini_File_Read_String����ʧ��*/
#define  GM_INI_FILE_READ_UINT_ATOUI_ERR		0xA0010502U			/*GM_atoui����ʧ��*/

/**GM_RSSP1_Ini_File_Read_Int(),0xA00106XX**/
#define  GM_INI_FILE_READ_INT_PAR_ERR			0xA0010600U			/*GM_RSSP1_Ini_File_Read_Int�����������*/
#define  GM_INI_FILE_READ_INT_STRING_ERR		0xA0010601U			/*GM_RSSP1_Ini_File_Read_String����ʧ��*/
#define  GM_INI_FILE_READ_INT_ATOI_ERR			0xA0010602U			/*GM_atoi����ʧ��*/

/**GM_Ini_File_Read_BOOL(),0xA00107XX**/
#define  GM_INI_FILE_READ_BOOL_PAR_ERR			0xA0010700U			/*GM_Ini_File_Read_BOOL�����������*/
#define  GM_INI_FILE_READ_BOOL_VALUE_ERR		0xA0010701U			/*������ֵ����BOOL����*/
#define  GM_INI_FILE_READ_BOOL_READ_ERR			0xA0010702U			/*GM_RSSP1_Ini_File_Read_Int����ʧ��*/

/**GM_Ini_File_Read_Hex(),0xA00108XX**/
#define  GM_INI_FILE_READ_HEX_PAR_ERR			0xA0010800U			/*GM_Ini_File_Read_Hex�����������*/
#define  GM_INI_FILE_READ_HEX_PARSE_ERR			0xA0010801U			/*GM_Ini_File_Parse����ʧ��*/
#define  GM_INI_FILE_READ_HEX_HEX_PARSE_ERR		0xA0010802U			/*GM_Ini_File_Read_Hex_Parse����ʧ��*/

/**GM_Ini_File_Read_Hex_32(),0xA00109XX**/
#define  GM_INI_FILE_READ_HEX_32_PAR_ERR		0xA0010900U			/*GM_Ini_File_Read_Hex_32�����������*/
#define  GM_INI_FILE_READ_HEX_32_PARSE_ERR		0xA0010901U			/*GM_Ini_File_Parse����ʧ��*/
#define  GM_INI_FILE_READ_HEX_32_HEX_PARSE_ERR	0xA0010902U			/*GM_Ini_File_Read_Hex_32_Parse����ʧ��*/

/**GM_Ini_File_Read_IntORHex8(),0xA0010AXX**/
#define  GM_INI_FILE_READ_INTORHEX8_PAR_ERR		   0xA0010A00U			/*GM_Ini_File_Read_IntORHex8�����������*/
#define  GM_INI_FILE_READ_INTORHEX8_PARSE_ERR	   0xA0010A01U			/*GM_Ini_File_Parse����ʧ��*/
#define  GM_INI_FILE_READ_INTORHEX8_CPCOUNT_ERR	   0xA0010A02U			/*cpcount��ֵ�д���*/
#define  GM_INI_FILE_READ_INTORHEX8_32_PARSE_ERR   0xA0010A03U			/*GM_Ini_File_Read_Hex_32_Parse����ʧ��*/
#define  GM_INI_FILE_READ_INTORHEX8_CPCOUNT_2_ERR  0xA0010A04U			/*cpcount��ֵ�д���*/
#define  GM_INI_FILE_READ_INTORHEX8_ATOUI_ERR	   0xA0010A05U			/*GM_atoui����ʧ��*/
#define  GM_INI_FILE_READ_INTORHEX8_VALUE_ERR	   0xA0010A06U			/*value����GM_RSSP1_UINT8_MAX*/

/**GM_Ini_File_Read_IntORHex16(),0xA0010BXX**/
#define  GM_INI_FILE_READ_INTORHEX16_PAR_ERR		   0xA0010B00U			/*GM_Ini_File_Read_IntORHex16�����������*/
#define  GM_INI_FILE_READ_INTORHEX16_PARSE_ERR		   0xA0010B01U			/*GM_Ini_File_Parse����ʧ��*/
#define  GM_INI_FILE_READ_INTORHEX16_CPCOUNT_ERR	   0xA0010B02U			/*cpcount��ֵ�д���*/
#define  GM_INI_FILE_READ_INTORHEX16_32_PARSE_ERR      0xA0010B03U			/*GM_Ini_File_Read_Hex_32_Parse����ʧ��*/
#define  GM_INI_FILE_READ_INTORHEX16_CPCOUNT_2_ERR     0xA0010B04U			/*cpcount��ֵ�д���*/
#define  GM_INI_FILE_READ_INTORHEX16_ATOUI_ERR	       0xA0010B05U			/*GM_atoui����ʧ��*/
#define  GM_INI_FILE_READ_INTORHEX16_VALUE_ERR	       0xA0010B06U			/*value����GM_RSSP1_UINT8_MAX*/

/**GM_Ini_File_Read_IntORHex32(),0xA0010CXX**/
#define  GM_INI_FILE_READ_INTORHEX32_PAR_ERR	       0xA0010C00U			/*GM_Ini_File_Read_IntORHex32�����������*/
#define  GM_INI_FILE_READ_INTORHEX32_PARSE_ERR	       0xA0010C01U			/*GM_Ini_File_Parse����ʧ��*/
#define  GM_INI_FILE_READ_INTORHEX32_CPCOUNT_ERR	   0xA0010C02U			/*cpcount��ֵ�д���*/
#define  GM_INI_FILE_READ_INTORHEX32_32_PARSE_ERR	   0xA0010C03U			/*GM_Ini_File_Read_Hex_32_Parse����ʧ��*/
#define  GM_INI_FILE_READ_INTORHEX32_CPCOUNT_2_ERR	   0xA0010C04U			/*cpcount��ֵ�д���*/
#define  GM_INI_FILE_READ_INTORHEX32_ATOUI_ERR		   0xA0010C05U			/*GM_atoui����ʧ��*/

/**GM_Get_File_Partly_Size(),0xA0010DXX**/
#define  GM_GET_FILE_PARTLY_SIZE_PAR_ERR			  0xA0010D00U			/*GM_Get_File_Partly_Size�����������*/
#define  GM_GET_FILE_PARTLY_SIZE_OPEN_ERR	 	 	  0xA0010D01U			/*���ļ�ʧ��*/
#define  GM_GET_FILE_PARTLY_SIZE_FSEEK_ERR			  0xA0010D02U			/*fseek����ʧ��*/
#define  GM_INI_FILE_PARTLY_SIZE_CONTENT_ERR		  0xA0010D03U			/*GM_Get_File_Partly_Size��ȡ{�����ݹ���*/
#define  GM_GET_FILE_PARTLY_SIZE_FTELL_ERR			  0xA0010D04U			/*ftell����ʧ��*/
#define  GM_GET_FILE_PARTLY_SIZE_CLOSE_ERR			  0xA0010D05U			/*�ر��ļ�ʧ��*/
#define  GM_GET_FILE_PARTLY_SIZE_NOT_FIND_ERR		  0xA0010D06U			/*δ�ҵ�partName����ʧ��*/




/*******BIN MULTIHOST INITIALIZE����,�����Ϊ0xA002XXXX***********/
/**GM_Bin_Head_H2N_N2H(),0xA00201XX**/
#define  GM_BIN_HEAD_H2N_H2N_PAR_ERR	     0xA0020100U		/*GM_Bin_Head_H2N_N2H�����������*/

/**GM_Multibin_GetHost(),0xA00202XX**/
#define  GM_MULTIBIN_GETHOST_PAR_ERR	     0xA0020200U		/*GM_Multibin_GetHost�����������*/
#define  GM_MULTIBIN_GETHOST_CRC_ERR	     0xA0020201U		/*CRC��鲻ͨ��*/
#define  GM_MULTIBIN_GETHOST_SIZE_ERR	     0xA0020202U		/*�ж��ļ���С����*/
#define  GM_MULTIBIN_GETHOST_VERSION_ERR	 0xA0020203U		/*�ж��ļ��汾����*/
#define  GM_MULTIBIN_GETHOST_LOAD_ERR		 0xA0020204U		/*װ�س���*/


/******************MODULAR����,�����Ϊ0xA003XXXX****************/
/**����**/


/*********** 64BIT DATA CALCULATE,�����Ϊ0xA004XXXX****************/
/**����**/



/******************CODE_SNV����,�����Ϊ0xA005XXXX****************/
/**GM_Code_SNV_Init(),0xA00501XX**/
#define  GM_CODE_SNV_INIT_PAR_ERR			 0xA0050100U		/*GM_Code_SNV_Init�����������*/
#define  GM_CODE_SNV_INIT_SIZE_ERR			 0xA0050101U		/*��ȡ���ļ������봫�벻һ��*/
#define  GM_CODE_SNV_INIT_CRC_ERR			 0xA0050102U		/*CRCУ�鲻ͨ��*/
#define  GM_CODE_SNV_INIT_VERSION_ERR		 0xA0050103U		/*�汾��У�鲻ͨ��*/
#define  GM_CODE_SNV_INIT_NISALVCP_ERR		 0xA0050104U		/*GM_Code_Read_NISALVCP����ʧ��*/
#define  GM_CODE_SNV_INIT_SINGLENISAL_ERR	 0xA0050105U		/*GM_Code_Read_SINGLENISAL����ʧ��*/
#define  GM_CODE_SNV_INIT_MODE_ERR			 0xA0050106U		/*MODE����*/

/**GM_Code_GetTypeIndex(),0xA00502XX**/
#define  GM_CODE_GETTYPEINDEX_PAR_ERR		 0xA0050200U		/*GM_Code_GetTypeIndex�����������*/
#define  GM_CODE_GETTYPEINDEX_MODE_ERR		 0xA0050201U		/*MODE����*/

/**GM_Code_SINGLE2NISAL(),0xA00503XX**/
#define  GM_CODE_SINGLE2NISAL_PAR_ERR		 0xA0050300U		/*GM_Code_SINGLE2NISAL�����������*/
#define  GM_CODE_SINGLE2NISAL_TYPEINDEX_ERR	 0xA0050301U		/*typeIndex����*/

/**GM_Code_NISAL2SINGLE(),0xA00504XX**/
#define  GM_CODE_NISAL2SINGLE_PAR_ERR		 0xA0050400U		/*GM_Code_NISAL2SINGLE�����������*/
#define  GM_CODE_NISAL2SINGLE_TYPEINDEX_ERR	 0xA0050401U		/*typeIndex����*/


/******************MUDP����,�����Ϊ0xA006XXXX******************/
/**GM_MUDP_Get_UDP_Flag(),0xA00601XX**/
#define  GM_MUDP_GET_UDP_FLAG_PAR_ERR		0xA0060100U		/*GM_MUDP_Get_UDP_Flag�����������*/
#define  GM_MUDP_GET_UDP_FLAG_NOT_FOUND		0xA0060101U		/*û���ҵ���Ӧ��msgType*/

/**GM_MUDP_Write(),0xA00602XX**/
#define GM_MUDP_WRITE_PAR_ERR				0xA0060200U		/*GM_MUDP_Write�����������*/
#define GM_MUDP_WRITE_WRITE_PROCESS_ERR		0xA0060201U		/*GM_MUDP_Write_Process����ʧ��*/

/**GM_MUDP_Open(),0xA00603XX**/
#define GM_MUDP_OPEN_PAR_ERR				0xA0060300U		/*GM_MUDP_Open�����������*/
#define GM_MUDP_OPEN_MSGTYPE_ERR			0xA0060301U		/*���½���msgType�Ѿ�����*/
#define GM_MUDP_OPEN_CREATE_ERR				0xA0060302U		/*����Socketʧ��*/
#define GM_MUDP_OPEN_WRITE_ERR				0xA0060303U		/*����SNΪ0�����ݰ�ʧ��*/

/**GM_MUDP_Init(),0xA00604XX**/
#define GM_MUDP_INIT_PAR_ERR				0xA0060400U		/*GM_MUDP_Init�����������*/
#define GM_MUDP_INIT_UDP_INIT_ERR			0xA0060401U		/*GM_MUDP_Init��ʼ��UDPʧ��*/
#define GM_MUDP_INIT_THREAD_CREATE_ERR		0xA0060402U		/*GM_MUDP_Init�����߳�ʧ��*/

/**GM_MUDP_Msg_Encode(),0xA00605XX**/
#define GM_MUDP_MSG_ENCODE_PAR_ERR			0xA0060500U		/*GM_MUDP_Msg_Encode�����������*/
#define GM_MUDP_MSG_ENCODE_CHANGE_ERR		0xA0060501U		/*ת���������ֽ��򷵻�ʧ��*/
#define GM_MUDP_MSG_ENCODE_MODE_ERR			0xA0060502U		/*GM_MUDP_Msg_Encode����CRCMode����*/
#define GM_MUDP_MSG_ENCODE_CAL_CRC_ERR		0xA0060503U		/*GM_CRC_Calculate_CRC16����ʧ��*/

/**GM_MUDP_Msg_Decode(),0xA00606XX**/
#define GM_MUDP_MSG_DECODE_PAR_ERR			0xA0060600U		/*GM_MUDP_Msg_Decode�����������*/
#define GM_MUDP_MSG_DECODE_SIZE_ERR			0xA0060601U		/*GM_MUDP_Msg_Decode�������Ϣsize����*/
#define GM_MUDP_MSG_DECODE_MODE_ERR			0xA0060602U		/*GM_MUDP_Msg_Decode����CRCMode����*/
#define GM_MUDP_MSG_DECODE_CRC_ERR			0xA0060603U		/*CRC����ʧ��ʧ��*/
#define GM_MUDP_MSG_DECODE_CHANGE_ERR		0xA0060604U		/*ת���������ֽ��򷵻�ʧ��*/

/*********************MR����,�����Ϊ0xA007XXXX******************/
/**GM_Memory_Refresh_Init(),0xA00701XX**/
#define GM_MEMORY_REFRESH_INIT_LOAD_ERR				0xA0070100U		/*GM_Ini_File_Load����ʧ��*/
#define GM_MEMORY_REFRESH_INIT_READ_ERR				0xA0070101U		/*��ʼ����ȡ����ʧ��*/
#define GM_MEMORY_REFRESH_INIT_CHANNEL_ERR			0xA0070102U		/*��ʼ����ȡ�ϲ���������У���ֵ�ͨ����ϢCHANNEL����*/
#define GM_MEMORY_REFRESH_INIT_CHANNEL_ERR_2		0xA0070103U		/*��ʼ����ȡ������ͨ����ϢCHANNEL����*/
#define GM_MEMORY_REFRESH_INIT_MEMADDR_ERR			0xA0070104U		/*��ʼ����ȡ�ڴ���β��ַ����*/
#define GM_MEMORY_REFRESH_INIT_DUMPCHKWPOS_ERR		0xA0070105U		/*���ɵ�dumpChkwInex��У���ֵĵ�dumpChkwPosλ��������ֵ*/
#define GM_MEMORY_REFRESH_INIT_MEMSIZE_ERR			0xA0070106U		/*dump���ڴ��ַ��С������ʵ��ˢ�µ��ڴ�memSize��С*/
#define GM_MEMORY_REFRESH_INIT_MODE_ERR				0xA0070107U		/*��ʼ����ȡREFRESH_MODE����*/
#define GM_MEMORY_REFRESH_INIT_ADD_ERR				0xA0070108U		/*GM_Memory_Refresh_Add����ʧ��*/
#define GM_MEMORY_REFRESH_INIT_CHKWPOS_ERR			0xA0070109U		/*���ɵ�chkwInex��У���ֵĵ�chkwPosλ��������ֵ*/

/**GM_Memory_Refresh_Create_CHKW(),0xA00702XX**/
#define GM_MEMORY_REFRESH_CREATE_CHKW_INIT_ERR			0xA0070200U		/*GM_Memory_Refresh_Initialized��ΪTRUE*/
#define GM_MEMORY_REFRESH_CREATE_CHKW_SLOTINDEX_ERR		0xA0070201U		/*slotIndex�����޶���Χ*/
#define GM_MEMORY_REFRESH_CREATE_CHKW_EACH_ERR			0xA0070202U		/*GM_Memory_Refresh_Create_Each_CHKW����ʧ��*/

/**GM_Memory_Refresh_Init_From_Addr(),0xA00703XX**/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_PAR_ERR				0xA0070300U		/*GM_Memory_Refresh_Init_From_Addr�������*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_SIZE_ERR				0xA0070301U		/*����size����*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_CRC_ERR				0xA0070302U		/*CRCУ��ʧ��*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_VERSION_ERR			0xA0070303U		/*�汾У��ʧ��*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_CHANNEL_ERR			0xA0070304U		/*��ȡ�ϲ���������У���ֵ�ͨ����Ϣ����*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_CHKW_NUM_ERR			0xA0070305U		/*CKW��Ŀ�������ֵ*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_CHKW_MEMORY_ERR		0xA0070306U		/*�ڴ����Ŀ�������ֵ*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_SLOT_NUM_ERR			0xA0070307U		/*ˢ��ʱ϶���������ֵ*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_EACH_CHANNEL_ERR		0xA0070308U		/*��ȡÿ���ڴ��ͨ����Ϣ����*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_EACH_SLOT_NUM_ERR		0xA0070309U		/*��ȡÿ���ڴ��ˢ��ʱ϶���������ֵ*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_MEMADDR_ERR			0xA007030AU		/*��ȡ�ڴ���β��ַ����*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_INDEX_ERR				0xA007030BU		/*��ȡˢ���ڴ�index����*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_CHKWPOS_ERR			0xA007030CU		/*���ɵ�chkwInex��У���ֵĵ�chkwPosλ��������ֵ*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_MEMSIZE_ERR			0xA007030DU		/*dump���ڴ��ַ��С������ʵ��ˢ�µ��ڴ�memSize��С*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_CONFIG_INDEX_ERR		0xA007030EU		/*��ȡ���õ�У������Ϣindex����*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_DUMPCHKWPOS_ERR		0xA007030FU		/*���ɵ�dumpChkwInex��У���ֵĵ�dumpChkwPosλ��������ֵ*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_VALUE_ERR				0xA0070310U		/*��ȡ�Ա���������Ϣ����*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_ADD_ERR				0xA0070311U		/*GM_Memory_Refresh_Add����ʧ��*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_FALSE_INDEX_ERR		0xA0070312U		/*��ȡλ�������ڴ�������еļ�У����index����*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_FALSE_CHKW_NUM_ERR		0xA0070313U		/*��У����CKW��Ŀ�������ֵ*/
#define GM_MEMORY_REFRESH_INIT_FROM_ADDR_FALSE_CHKWPOS_ERR		0xA0070314U		/*��У�������ɵ�chkwInex��У���ֵĵ�chkwPosλ��������ֵ*/


/*********************CRC����,�����Ϊ0xA008XXXX******************/
/**GM_CRC_Calculate_CRC16_Standard(),0xA00801XX**/
#define GM_CRC_CALCULATE_CRC16_STANDARD_PAR_ERR					0xA0080100U		/*GM_CRC_Calculate_CRC16_Standard�������*/
#define GM_CRC_CALCULATE_CRC16_STANDARD_POLY_ERR				0xA0080101U		/*����Poly����*/
#define GM_CRC_CALCULATE_CRC16_STANDARD_POLY_REFLECT_ERR		0xA0080102U		/*����Poly����*/

/**GM_CRC_Calculate_CRC16_Standard_Seed(),0xA00802XX**/
#define GM_CRC_CALCULATE_CRC16_STANDARD_SEED_PAR_ERR				0xA0080200U		/*GM_CRC_Calculate_CRC16_Standard_Seed�������*/
#define GM_CRC_CALCULATE_CRC16_STANDARD_SEED_POLY_ERR				0xA0080201U		/*����Poly����*/
#define GM_CRC_CALCULATE_CRC16_STANDARD_SEED_POLY_REFLECT_ERR		0xA0080202U		/*����Poly����*/

/**GM_CRC_Calculate_CRC32_Standard(),0xA00803XX**/
#define GM_CRC_CALCULATE_CRC32_STANDARD_PAR_ERR					0xA0080300U		/*GM_CRC_Calculate_CRC32_Standard�������*/
#define GM_CRC_CALCULATE_CRC32_STANDARD_POLY_ERR				0xA0080301U		/*����Poly����*/

/**GM_NISAL18CODE_Check(),0xA00804XX**/
#define GM_NISAL18CODE_CHECK_PAR_ERR			0xA0080400U		/*GM_NISAL18CODE_Check���NISAL18CODE����*/
#define GM_NISAL18CODE_CHECK_FAIL				0xA0080401U		/*GM_NISAL18CODE_Check��鲻ͨ��*/

/**GM_NISALCODE_Check(),0xA00805XX**/
#define GM_NISALCODE_CHECK_PAR_ERR			0xA0080500U		/*GM_NISALCODE_Check���NISALCode����*/
#define GM_NISALCODE_CHECK_FAIL				0xA0080501U		/*GM_NISALCODE_Check��鲻ͨ��*/


/*********************VSN����,�����Ϊ0xA009XXXX******************/
/**GM_VSN_Init(),0xA00901XX**/
#define GM_VSN_INIT_LFSR_1_ERR			0xA0090100U		/*GM_LFSR_Initͨ��1����ʧ��*/
#define GM_VSN_INIT_LFSR_2_ERR			0xA0090101U		/*GM_LFSR_Initͨ��2����ʧ��*/

/**GM_VSN_Cal_Check_Word_VSN1(),0xA00902XX**/
#define GM_VSN_CAL_CHECK_WORD_VSN1_PAR_ERR			0xA0090200U		/*GM_VSN_Cal_Check_Word_VSN1���pCHKWΪNULL*/
#define GM_VSN_CAL_CHECK_WORD_VSN1_CAL_FAIL			0xA0090201U		/*GM_VSN_Cal_Check_Word_VSN1����ʧ��*/

/**GM_VSN_Cal_Check_Word_VSN2(),0xA00903XX**/
#define GM_VSN_CAL_CHECK_WORD_VSN2_PAR_ERR			0xA0090300U		/*GM_VSN_Cal_Check_Word_VSN2���pCHKWΪNULL*/
#define GM_VSN_CAL_CHECK_WORD_VSN2_CAL_FAIL			0xA0090301U		/*GM_VSN_Cal_Check_Word_VSN2����ʧ��*/

/**GM_VSN_Add(),0xA00904XX**/
#define GM_VSN_ADD_PAR_ERR				0xA0090400U			/*GM_VSN_Add���VSNIDΪ0*/
#define GM_VSN_ADD_VSNID_EXIST			0xA0090401U			/*���VSNID�Ѿ�����*/
#define GM_VSN_ADD_NUM_ERR				0xA0090402U			/*VSN_Num�Ѿ��ﵽ���ֵ*/

/**GM_VSN_Get_S(),0xA00905XX**/
#define GM_VSN_GET_S_PAR_ERR			0xA0090500U			/*GM_VSN_Get_S�������*/
#define GM_VSN_GET_S_VSN_NOT_FIND		0xA0090501U			/*δ�ҵ���Ӧ��VSNID*/

/**GM_VSN_OTHER_Init(),0xA00906XX**/
#define GM_VSN_OTHER_INIT_LFSR_1_ERR			0xA0090600U		/*GM_LFSR_Initͨ��1����ʧ��*/
#define GM_VSN_OTHER_INIT_LFSR_2_ERR			0xA0090601U		/*GM_LFSR_Initͨ��2����ʧ��*/

/**GM_VSN_OTHER_Cal_Check_Word_VSN1(),0xA00907XX**/
#define GM_VSN_OTHER_CAL_CHECK_WORD_VSN1_PAR_ERR			0xA0090700U		/*GM_VSN_OTHER_Cal_Check_Word_VSN1���pCHKWΪNULL*/
#define GM_VSN_OTHER_CAL_CHECK_WORD_VSN1_CAL_FAIL			0xA0090701U		/*GM_VSN_OTHER_Cal_Check_Word_VSN1����ʧ��*/

/**GM_VSN_OTHER_Cal_Check_Word_VSN2(),0xA00908XX**/
#define GM_VSN_OTHER_CAL_CHECK_WORD_VSN2_PAR_ERR			0xA0090800U		/*GM_VSN_OTHER_Cal_Check_Word_VSN2���pCHKWΪNULL*/
#define GM_VSN_OTHER_CAL_CHECK_WORD_VSN2_CAL_FAIL			0xA0090801U		/*GM_VSN_OTHER_Cal_Check_Word_VSN2����ʧ��*/

/**GM_VSN_Set_S(),0xA00909XX**/
#define GM_VSN_SET_S_VSN_NOT_FIND		0xA0090900U			/*δ�ҵ���Ӧ��VSNID*/

/*********************SIOCOM����,�����Ϊ0xA00AXXXX******************/
/**GM_SIO_Com_Open(),0xA000A01XX**/
#define GM_SIO_COM_OPEN_PAR_ERR				0xA00A0100U		/*GM_SIO_Com_Open�������*/
#define GM_SIO_COM_OPEN_INIT_FAIL			0xA00A0101U		/*�򿪴��ڷ���ʧ��*/
#define GM_SIO_COM_OPEN_RXBUF_INIT_FAIL		0xA00A0102U		/*�򿪽��ջ���������ʧ��*/

/**GM_SIO_Com_Read(),0xA00A02XX**/
#define GM_SIO_COM_READ_PAR_ERR				0xA00A0200U		/*GM_SIO_Com_Read�������*/
#define GM_SIO_COM_READ_THREAD_ERR			0xA00A0201U		/*GM_SIO_Com_Read���������߳�ʧ��*/

/**GM_SIO_Com_Write(),0xA00A03XX**/
#define GM_SIO_COM_WRITE_PAR_ERR			0xA00A0300U		/*GM_SIO_Com_Write�������*/
#define GM_SIO_COM_WRITE_MSG_ENCODE_ERR		0xA00A0301U		/*GM_SIO_Msg_Encode����ʧ��*/
#define GM_SIO_COM_WRITE_MSG_FAIL			0xA00A0302U		/*GM_SIO_Com_Write����ʧ��*/

/**GM_SIO_Msg_Encode(),0xA00A04XX**/
#define GM_SIO_MSG_ENCODE_PAR_ERR			0xA00A0400U		/*GM_SIO_Msg_Encode�������*/
#define GM_SIO_MSG_ENCODE_CRC_ERR			0xA00A0401U		/*����16λCRC����ʧ��*/

/**GM_SIO_Msg_Decode(),0xA00A05XX**/
#define GM_SIO_MSG_DECODE_PAR_ERR			0xA00A0500U		/*GM_SIO_Msg_Decode�������*/
#define GM_SIO_MSG_DECODE_SIZE_ERR			0xA00A0501U		/*��Ϣ��������*/
#define GM_SIO_MSG_DECODE_CRC_ERR			0xA00A0502U		/*16λCRC��֤ʧ��*/

/**VXWORKS6_6��openPort(),0xA00A06XX**/
#define OPENPORT6_6_PAR_ERR					0xA00A0600U		/*openPort�������*/
#define OPENPORT6_6_OPEN_ERR				0xA00A0601U		/*openPort���豸ʧ��*/
#define OPENPORT6_6_SET_BAUDRATE_ERR		0xA00A0602U		/*openPort���ò�����ʧ��*/

/**GM_SIO_Open_VXWORKS6_6(),0xA00A07XX**/
#define GM_SIO_OPEN_VXWORKS6_6_PAR_ERR				0xA00A0700U			/*GM_SIO_Open_VXWORKS6_6�������*/
#define GM_SIO_OPEN_VXWORKS6_6_OPENPORT_ERR			0xA00A0701U			/*GM_SIO_Open_VXWORKS6_6���豸ʧ��*/
#define GM_SIO_OPEN_VXWORKS6_6_SETPORTBAUD_ERR		0xA00A0702U			/*GM_SIO_Open_VXWORKS6_6���ò�����ʧ��*/
#define GM_SIO_OPEN_VXWORKS6_6_DATABITS_ERR			0xA00A0703U			/*�����databits����*/
#define GM_SIO_OPEN_VXWORKS6_6_STOPBITS_ERR			0xA00A0704U			/*�����stopbits����*/
#define GM_SIO_OPEN_VXWORKS6_6_PARITY_ERR			0xA00A0705U			/*�����parity����*/
#define GM_SIO_OPEN_VXWORKS6_6_SETPORTHWOPT_ERR		0xA00A0706U			/*����Hardware Optionsʧ��*/

/**VXWORKS5_5��openPort(),0xA00A08XX**/
#define OPENPORT5_5_PAR_ERR					0xA00A0800U		/*openPort�������*/
#define OPENPORT5_5_OPEN_ERR				0xA00A0801U		/*openPort���豸ʧ��*/
#define OPENPORT5_5_SET_BAUDRATE_ERR		0xA00A0802U		/*openPort���ò�����ʧ��*/

/**GM_SIO_Open_VXWORKS5_5(),0xA00A09XX**/
#define GM_SIO_OPEN_VXWORKS5_5_PAR_ERR				0xA00A0900U			/*GM_SIO_Open_VXWORKS5_5�������*/
#define GM_SIO_OPEN_VXWORKS5_5_OPENPORT_ERR			0xA00A0901U			/*GM_SIO_Open_VXWORKS5_5���豸ʧ��*/
#define GM_SIO_OPEN_VXWORKS5_5_SETPORTBAUD_ERR		0xA00A0902U			/*GM_SIO_Open_VXWORKS5_5���ò�����ʧ��*/
#define GM_SIO_OPEN_VXWORKS5_5_DATABITS_ERR			0xA00A0903U			/*�����databits����*/
#define GM_SIO_OPEN_VXWORKS5_5_STOPBITS_ERR			0xA00A0904U			/*�����stopbits����*/
#define GM_SIO_OPEN_VXWORKS5_5_PARITY_ERR			0xA00A0905U			/*�����parity����*/

/**GM_SIO_Open_Windows(),0xA00A0AXX**/
#define GM_SIO_OPEN_WINDOWS_PAR_ERR					0xA00A0A00U			/*GM_SIO_Open_Windows�������*/
#define GM_SIO_OPEN_WINDOWS_GET_NAME_ERR			0xA00A0A01U			/*GM_SIO_Get_Name_By_No����ʧ��*/
#define GM_SIO_OPEN_WINDOWS_STOPBITS_ERR			0xA00A0A02U			/*����stopbits����*/
#define GM_SIO_OPEN_WINDOWS_BUILDCOMMDCB_ERR		0xA00A0A03U			/*BuildCommDCB����ʧ��*/
#define GM_SIO_OPEN_WINDOWS_CREATEFILE_ERR			0xA00A0A04U			/*CreateFile()����ʧ��*/
#define GM_SIO_OPEN_WINDOWS_SETUPCOMM_ERR			0xA00A0A05U			/*�����������buf,SetupComm()����ʧ��*/
#define GM_SIO_OPEN_WINDOWS_SETCOMMSTATE_ERR		0xA00A0A06U			/*����DCB�ṹ,SetCommState()����ʧ��*/
#define GM_SIO_OPEN_WINDOWS_SETCOMMTIMEOUTS_ERR		0xA00A0A07U			/*������ʱ,SetCommTimeouts()����ʧ��*/
#define GM_SIO_OPEN_WINDOWS_PURGECOMM_ERR			0xA00A0A08U			/*���buf��Ϣ,PurgeComm()����ʧ��*/

/**GM_SIO_Read_Windows(),0xA00A0BXX**/
#define GM_SIO_READ_WINDOWS_PAR_ERR						0xA00A0B00U			/*GM_SIO_Read_Windows�������*/
#define GM_SIO_READ_WINDOWS_CREATEEVENT_ERR				0xA00A0B01U			/*CreateEvent()����ʧ��*/
#define GM_SIO_READ_WINDOWS_CLEARCOMMERROR_FAIL			0xA00A0B02U			/*ClearCommError()����ʧ��*/
#define GM_SIO_READ_WINDOWS_STATCBINQUE_ERR				0xA00A0B03U			/*ClearCommError()����stat.cbInQue����*/
#define GM_SIO_READ_WINDOWS_GETOVERLAPPEDRESULT_ERR		0xA00A0B04U			/*GetOverlappedResult()����ʧ��--�ص������ĵ�ǰ״̬*/
#define GM_SIO_READ_WINDOWS_WAITFORSINGLEOBJECT_ERR		0xA00A0B05U			/*WaitForSingleObject()����ʧ��*/
#define GM_SIO_READ_WINDOWS_GETERRORNO_ERR				0xA00A0B06U			/*GM_Get_Error_No()����ʧ��*/

/**GM_SIO_Write_Windows(),0xA00A0CXX**/
#define GM_SIO_WRITE_WINDOWS_PAR_ERR						0xA00A0C00U			/*GM_SIO_Write_Windows�������*/
#define GM_SIO_WRITE_WINDOWS_CREATEEVENT_ERR				0xA00A0C01U			/*CreateEvent()����ʧ��*/
#define GM_SIO_WRITE_WINDOWS_GETLASTERROR_ERR				0xA00A0C02U			/*GetLastError()����ʧ��*/
#define GM_SIO_WRITE_WINDOWS_GETOVERLAPPEDRESULT_ERR		0xA00A0C03U			/*GetOverlappedResult()����ʧ��*/
#define GM_SIO_WRITE_WINDOWS_DATASIZE_ERR					0xA00A0C04U			/*GetOverlappedResult()���w_len��datasize����*/
#define GM_SIO_WRITE_WINDOWS_WAITFORSINGLEOBJECT_ERR		0xA00A0C05U			/*WaitForSingleObject()����ʧ��*/
#define GM_SIO_WRITE_WINDOWS_WRITE_WLEN_ERR					0xA00A0C06U			/*WriteFile()д��w_len��datasize����*/

/**GM_SIO_Rx_Task(),0xA00A0DXX**/
#define GM_SIO_RX_TASK_WIN32_PAR_ERR				0xA00A0D00U			/*win32�£�GM_SIO_Rx_Task�������*/
#define GM_SIO_RX_TASK_WIN32_PORT_ERR				0xA00A0D01U			/*win32�£�GM_SIO_Rx_Task���port����*/
#define GM_SIO_RX_TASK_WIN32_ANALYSE_ERR			0xA00A0D02U			/*win32�£�GM_SIO_COM_Rx_Analyse_Task����ʧ��*/
#define GM_SIO_RX_TASK_VXWORKS_PORT_ERR				0xA00A0D03U			/*VXWORKS�£�GM_SIO_Rx_Task���port����*/
#define GM_SIO_RX_TASK_VXWORKS_ANALYSE_ERR			0xA00A0D04U			/*VXWORKS�£�GM_SIO_COM_Rx_Analyse_Task����ʧ��*/
#define GM_SIO_RX_TASK_QNX_PAR_ERR					0xA00A0D05U			/*QNX�£�GM_SIO_Rx_Task�������*/
#define GM_SIO_RX_TASK_QNX_PORT_ERR					0xA00A0D06U			/*QNX�£�GM_SIO_Rx_Task���port����*/
#define GM_SIO_RX_TASK_QNX_ANALYSE_ERR				0xA00A0D07U			/*QNX�£�GM_SIO_COM_Rx_Analyse_Task����ʧ��*/



/*********************TIME����,�����Ϊ0xA00BXXXX******************/
/**GM_Time_BIOS_Get(),0xA00B01XX**/
#define GM_TIME_BIOS_GET_6U_PAR_ERR						0xA00B0100U			/*6U�£�GM_Time_BIOS_Get()�����������*/
#define GM_TIME_BIOS_GET_6U_TIME_GET_ERR				0xA00B0101U			/*6U�£�Drv_Rtc_Time_Get()����ʧ��*/
#define GM_TIME_BIOS_GET_VXWORKS6_6_PAR_ERR				0xA00B0102U			/*VXWORKS6_6�£�GM_Time_BIOS_Get()�����������*/
#define GM_TIME_BIOS_GET_VXWORKS6_6_SYSRTCGET_ERR		0xA00B0103U			/*VXWORKS6_6�£�sysRtcGet����ʧ��*/
#define GM_TIME_BIOS_GET_QNX_PAR_ERR					0xA00B0104U			/*QNX�£�GM_Time_BIOS_Get()�����������*/
#define GM_TIME_BIOS_GET_QNX_RTCINIT_ERR				0xA00B0105U			/*QNX�£�rtcInit()����ʧ��*/
#define GM_TIME_BIOS_GET_QNX_RTCREAD_ERR				0xA00B0106U			/*QNX�£�rtcRead()����ʧ��*/
#define GM_TIME_BIOS_GET_DEFINE_ERR						0xA00B0107U			/*GM_Time_BIOS_Getģʽ�궨�����*/

/**GM_Time_BIOS_Set(),0xA00B02XX**/
#define GM_TIME_BIOS_SET_6U_PAR_ERR						0xA00B0200U			/*6U�£�GM_Time_BIOS_Set()�����������*/
#define	GM_TIME_BIOS_SET_6U_DAYTIME_ERR					0xA00B0201U			/*6U�£�GM_Time_BIOS_Set()�����dayTime���ݴ���*/
#define GM_TIME_BIOS_SET_6U_TIME_SET_ERR				0xA00B0202U			/*6U�£�Drv_Rtc_Time_Set()����ʧ��*/
#define GM_TIME_BIOS_SET_VXWORKS6_6_PAR_ERR				0xA00B0203U			/*VXWORKS6_6�£�GM_Time_BIOS_Set()�����������*/
#define	GM_TIME_BIOS_SET_VXWORKS6_6_DAYTIME_ERR			0xA00B0204U			/*VXWORKS6_6�£�GM_Time_BIOS_Set()�����dayTime���ݴ���*/
#define GM_TIME_BIOS_SET_QNX_PAR_ERR					0xA00B0205U			/*QNX�£�GM_Time_BIOS_Set()�����������*/
#define GM_TIME_BIOS_SET_QNX_RTCINIT_ERR				0xA00B0206U			/*QNX�£�rtcInit()����ʧ��*/

/**GM_Time_OS_Set(),0xA00B03XX**/
#define GM_TIME_OS_SET_WIN32_PAR_ERR					0xA00B0300U			/*WIN32�£�GM_Time_OS_Set�����������*/
#define GM_TIME_OS_SET_WIN32_SETLOCALTIME_ERR			0xA00B0301U			/*WIN32�£�SetLocalTime����ʧ��*/
#define GM_TIME_OS_SET_QNX_PAR_ERR						0xA00B0302U			/*QNX�£�GM_Time_OS_Set�����������*/
#define GM_TIME_OS_SET_QNX_MKTIME_ERR					0xA00B0303U			/*QNX�£�mktime����ʧ��*/
#define GM_TIME_OS_SET_QNX_SETTIMEOFDAY_ERR				0xA00B0304U			/*QNX�£�sttimeofday����ʧ��*/
#define GM_TIME_OS_SET_VXWORKS_PAR_ERR					0xA00B0305U			/*VXWORKS�£�GM_Time_OS_Set�����������*/
#define GM_TIME_OS_SET_VXWORKS_CLOCKSETTIME_ERR			0xA00B0306U			/*VXWORKS�£�clock_settime����ʧ��*/

/**GM_Time_Get_UTC(),0xA00B04XX**/
#define GM_TIME_GET_UTC_WIN32_PAR_ERR						0xA00B0400U				/*WIN32�£�GM_Time_Get_UTC�����������*/
#define GM_TIME_GET_UTC_OTHER_PAR_ERR						0xA00B0401U				/*����ϵͳ�£�GM_Time_Get_UTC�����������*/


/*********************TIMER����,�����Ϊ0xA00CXXXX******************/
/**GM_Timer_Init(),0xA00C01XX**/
#define GM_TIMER_INIT_PAR_ERR						0xA00C0100U				/*GM_Timer_Init�����������*/
#define GM_TIMER_INIT_WIN32_THREAD_ERR				0xA00C0101U				/*WIN32�£�����GM_Timer_Thread�߳�ʧ��*/
#define GM_TIMER_INIT_VXWORKS5_5_INIT_ERR			0xA00C0102U				/*VXWORKS5_5�£�Drv_M8247_Timer_Init()����ʧ��*/
#define GM_TIMER_INIT_QNX_PAR_ERR					0xA00C0103U				/*QNX�£�sysTickTmrInit()����ʧ��*/
#define GM_TIMER_INIT_QNX_SYSTTCR_ERR				0xA00C0104U				/*QNX�£�sysTickTmrConnectRoutine()����ʧ��*/
#define GM_TIMER_INIT_QNX_SYSTTSTART_ERR			0xA00C0105U				/*QNX�£�sysTickTmrStart()����ʧ��*/
#define GM_TIMER_INIT_VXWORKS6_6_SMTCR_ERR			0xA00C0106U				/*VXWORKS6_6�£�sysMpc8572TmrConnectRoutine()����ʧ��*/
#define GM_TIMER_INIT_VXWORKS6_6_SMTS_ERR			0xA00C0107U				/*VXWORKS6_6�£�sysMpc8572TmrStart()����ʧ��*/
#define GM_TIMER_INIT_VXWORKS6_6_TIMERNO_ERR		0xA00C0108U				/*VXWORKS6_6�£������timerNo����*/

/**GM_Timer_Set(),0xA00C02XX**/
#define GM_TIMER_SET_WIN32_PAR_ERR						0xA00C0200U				/*WIN32�£�GM_Timer_Set�����������*/
#define GM_TIMER_SET_VXWORKS5_5_PAR_ERR					0xA00C0201U				/*VXWORKS5_5�£�GM_Timer_Set�����������*/
#define	GM_TIMER_SET_VXWORKS5_5_TIMER_SET_ERR			0xA00C0202U				/*VXWORKS5_5�£�Drv_M8247_Timer_Rstint()����ʧ��*/
#define GM_TIMER_SET_QNX_PAR_ERR						0xA00C0203U				/*QNX�£�GM_Timer_Set�����������*/
#define	GM_TIMER_SET_QNX_SYSTICKTMRSTART_ERR			0xA00C0204U				/*QNX�£�sysTickTmrStart()����ʧ��*/
#define GM_TIMER_SET_VXWORKS6_6_SYSTMRDISABLE_ERR		0xA00C0205U				/*VXWORKS6_6�£�sysMpc8572TmrDisable()����ʧ��*/
#define GM_TIMER_SET_VXWORKS6_6_SYSTMRSTART_ERR			0xA00C0206U				/*VXWORKS6_6�£�sysMpc8572TmrStart()����ʧ������*/
#define GM_TIMER_SET_VXWORKS6_6_TIMERNO_ERR				0xA00C0207U				/*VXWORKS6_6�£������timerNo����*/


/***********TIME OF HIGH-PRECISION����,�����Ϊ0xA00DXXXX**********/
/**GM_TimestampInit(),0xA00D01XX**/
#define GM_TIMESTAMPINIT_VXWORKS6_6_FAIL				0xA00D0100U			/*VXWORKS6_6�£�sysTimestampEnable()����ʧ��*/
#define GM_TIMESTAMPINIT_QNX_SYSTICKTMRINIT_FAIL		0xA00D0101U			/*QNX�£�sysTickTmrInit()����ʧ��*/
#define GM_TIMESTAMPINIT_QNX_SYSTICKTMRSTART_FAIL		0xA00D0102U			/*QNX�£�sysTickTmrStart()����ʧ��*/


/*********************OTHER����,�����Ϊ0xA00EXXXX******************/
/**GM_atoui(),0xA00E01XX**/
#define GM_ATOUI_PAR_ERR					0xA00E0100U				/*GM_atoui()�����������*/
#define GM_ATOUI_BUFSIZE_ZERO_ERR			0xA00E0101U				/*GM_atoui()����bufsizeΪ0*/
#define GM_ATOUI_BUFSIZE_BITTOMAX_ERR		0xA00E0102U				/*GM_atoui()����bufsizeλ���������ֵ*/
#define GM_ATOUI_BUFSIZE_CHAR_ERR			0xA00E0103U				/*��ǰ�ַ���������*/
#define GM_ATOUI_BUFSIZE_RTTOMAX_ERR		0xA00E0104U				/*����buf����"4294967295"*/
#define GM_ATOUI_BUFSIZE_2OO3_ERR			0xA00E0105U				/*2OO3��֧��ʹ��*/

/**GM_atoi(),0xA00E02XX**/
#define GM_ATOI_PAR_ERR						0xA00E0200U				/*GM_atoi()�����������*/
#define GM_ATOI_BUFSIZE_ZERO_ERR			0xA00E0201U				/*GM_atoi()����bufsizeΪ0*/
#define GM_ATOI_BUFSIZE_ERR					0xA00E0202U				/*GM_atoui()����bufsizeλ������*/
#define GM_ATOI_BUFSIZE_CHAR_ERR			0xA00E0203U				/*��ǰ�ַ���������*/
#define GM_ATOI_BUFSIZE_RTTOMAX_ERR			0xA00E0204U				/*����buf�������ֵ*/
#define GM_ATOI_BUFSIZE_2OO3_ERR			0xA00E0205U				/*2OO3��֧��ʹ��*/

/*********************DRAM����(VLE),�����Ϊ0xA00FXXXX******************/
/**GM_DRAM_Write32_A(),0xA00F01XX**/
#define GM_DRAM_WRITE32_A_ERR					0xA00F0100U				/*GM_DRAM_Write32_A()�����������*/
#define GM_DRAM_WRITE32_A_WRITE16_ERR			0xA00F0101U				/*GM_DRAM_Write16_A()����ʧ��*/
#define GM_DRAM_WRITE32_A_WRITE16_ERR_2			0xA00F0102U				/*GM_DRAM_Write16_A()����ʧ��*/

/**GM_DRAM_Write64_A(),0xA00F02XX**/
#define GM_DRAM_WRITE64_A_ERR					0xA00F0200U				/*GM_DRAM_Write64_A()�����������*/
#define GM_DRAM_WRITE64_A_WRITE32_ERR			0xA00F0201U				/*GM_DRAM_Write32_A()����ʧ��*/
#define GM_DRAM_WRITE64_A_WRITE32_ERR_2			0xA00F0202U				/*GM_DRAM_Write32_A()����ʧ��*/

/**GM_DRAM_Read32_A(),0xA00F03XX**/
#define GM_DRAM_READ32_A_ERR					0xA00F0300U				/*GM_DRAM_Read32_A()�����������*/
#define GM_DRAM_READ32_A_READ16_ERR				0xA00F0301U				/*GM_DRAM_Read32_A()����ʧ��*/
#define GM_DRAM_READ32_A_READ16_ERR_2			0xA00F0302U				/*GM_DRAM_Read32_A()����ʧ��*/

/**GM_DRAM_READ64_A(),0xA00F04XX**/
#define GM_DRAM_READ64_A_ERR					0xA00F0400U				/*GM_DRAM_READ64_A()�����������*/
#define GM_DRAM_READ64_A_READ32_ERR				0xA00F0401U				/*GM_DRAM_READ64_A()����ʧ��*/
#define GM_DRAM_READ64_A_READ32_ERR_2			0xA00F0402U				/*GM_DRAM_READ64_A()����ʧ��*/

/**GM_DRAM_Write32_B(),0xA00F05XX**/
#define GM_DRAM_WRITE32_B_ERR					0xA00F0500U				/*GM_DRAM_Write32_B()�����������*/
#define GM_DRAM_WRITE32_B_WRITE16_ERR			0xA00F0501U				/*GM_DRAM_Write16_B()����ʧ��*/
#define GM_DRAM_WRITE32_B_WRITE16_ERR_2			0xA00F0502U				/*GM_DRAM_Write16_B()����ʧ��*/

/**GM_DRAM_Write64_B(),0xA00F06XX**/
#define GM_DRAM_WRITE64_B_ERR					0xA00F0600U				/*GM_DRAM_Write64_B()�����������*/
#define GM_DRAM_WRITE64_B_WRITE32_ERR			0xA00F0601U				/*GM_DRAM_Write32_B()����ʧ��*/
#define GM_DRAM_WRITE64_B_WRITE32_ERR_2			0xA00F0602U				/*GM_DRAM_Write32_B()����ʧ��*/

/**GM_DRAM_Read32_B(),0xA00F07XX**/
#define GM_DRAM_READ32_B_ERR					0xA00F0700U				/*GM_DRAM_Read32_B()�����������*/
#define GM_DRAM_READ32_B_READ16_ERR				0xA00F0701U				/*GM_DRAM_Read32_B()����ʧ��*/
#define GM_DRAM_READ32_B_READ16_ERR_2			0xA00F0702U				/*GM_DRAM_Read32_B()����ʧ��*/

/**GM_DRAM_READ64_B(),0xA00F08XX**/
#define GM_DRAM_READ64_B_ERR					0xA00F0800U				/*GM_DRAM_READ64_B()�����������*/
#define GM_DRAM_READ64_B_READ32_ERR				0xA00F0801U				/*GM_DRAM_READ64_B()����ʧ��*/
#define GM_DRAM_READ64_B_READ32_ERR_2			0xA00F0802U				/*GM_DRAM_READ64_B()����ʧ��*/

/**GM_DRAM_Init_B(),0xA00F09XX**/
#define GM_DRAM_INIT_B_ERR						0xA00F0900U				/*GM_DRAM_Init_B()�����������*/
#define GM_DRAM_INIT_B_DVCOM_INIT_FAIL			0xA00F0901U				/*Drv_DVCOM_Init()����ʧ��*/


/*******************DRAMCOM����,�����Ϊ0xA010XXXX****************/
/**GM_DRAM_Com_Write_A(),0xA01001XX**/
#define GM_DRAM_COM_WRITE_A_PAR_ERR						0xA0100100U				/*GM_DRAM_Com_Write_A()�����������*/
#define GM_DRAM_COM_WRITE_A_DATASIZE_ERR				0xA0100101U				/*maxд - current < ��Ϣ���� + crc16(1) + ��Ϣsize(1)*/
#define GM_DRAM_COM_WRITE_A_WRITEDATASIZE_ERR			0xA0100102U				/*��ǰ������Ϣ�Ĵ�С����ʧ��*/
#define GM_DRAM_COM_WRITE_A_CALCRC_ERR					0xA0100103U				/*CRC���㷵��ʧ��*/
#define GM_DRAM_COM_WRITE_A_WRITECRC_ERR				0xA0100104U				/*дCRC����ʧ��*/
#define GM_DRAM_COM_WRITE_A_WRITEDATA_ERR				0xA0100105U				/*д��Ϣ���ݷ���ʧ��*/
#define GM_DRAM_COM_WRITE_A_WRITEMSGNUM_ERR				0xA0100106U				/*д��Ϣ��������ʧ��*/

/**GM_DRAM_Com_Read_A(),0xA01002XX**/
#define GM_DRAM_COM_READ_A_PAR_ERR						0xA0100200U				/*GM_DRAM_Com_Read_A()��ʼ��δ׼����*/
#define GM_DRAM_COM_READ_A_READMSGNUM_ERR				0xA0100201U				/*��DRAM�д�ŵ���Ϣ��������ʧ��*/
#define GM_DRAM_COM_READ_A_MSGNUM_ERR					0xA0100202U				/*��Ϣ����Ϊ0*/
#define GM_DRAM_COM_READ_A_INDEX_ERR					0xA0100203U				/*��Ϣindex�������ֵ*/
#define GM_DRAM_COM_READ_A_READMSGSIZE_ERR				0xA0100204U				/*��ȡ��Ϣ��Сʧ�ܻ��С�д���*/
#define GM_DRAM_COM_READ_A_READCRC_ERR					0xA0100205U				/*��ȡ��ϢCRCʧ��*/
#define GM_DRAM_COM_READ_A_MSGSIZETOMAX_ERR				0xA0100206U				/*��ǰindex + ��Ϣ���ݲ���������ȡindex*/
#define GM_DRAM_COM_READ_A_READDATE_ERR					0xA0100207U				/*��ȡ��Ϣ����ʧ��*/
#define GM_DRAM_COM_READ_A_CHECKCRC_ERR					0xA0100208U				/*У����ϢCRCʧ��*/
#define GM_DRAM_COM_READ_A_CALLBACK_ERR					0xA0100209U				/*GM_DRAM_COM_CALL_BACK_FUN_A����ʧ��*/

/**GM_DRAM_Com_Write_B(),0xA01003XX**/
#define GM_DRAM_COM_WRITE_B_PAR_ERR						0xA0100300U				/*GM_DRAM_Com_Write_B()�����������*/
#define GM_DRAM_COM_WRITE_B_DATASIZE_ERR				0xA0100301U				/*maxд - current < ��Ϣ���� + crc16(1) + ��Ϣsize(1)*/
#define GM_DRAM_COM_WRITE_B_WRITEDATASIZE_ERR			0xA0100302U				/*��ǰ������Ϣ�Ĵ�С����ʧ��*/
#define GM_DRAM_COM_WRITE_B_CALCRC_ERR					0xA0100303U				/*CRC���㷵��ʧ��*/
#define GM_DRAM_COM_WRITE_B_WRITECRC_ERR				0xA0100304U				/*дCRC����ʧ��*/
#define GM_DRAM_COM_WRITE_B_WRITEDATA_ERR				0xA0100305U				/*д��Ϣ���ݷ���ʧ��*/
#define GM_DRAM_COM_WRITE_B_WRITEMSGNUM_ERR				0xA0100306U				/*д��Ϣ��������ʧ��*/

/**GM_DRAM_Com_Read_B(),0xA01004XX**/
#define GM_DRAM_COM_READ_B_PAR_ERR						0xA0100400U				/*GM_DRAM_Com_Read_B()��ʼ��δ׼����*/
#define GM_DRAM_COM_READ_B_READMSGNUM_ERR				0xA0100401U				/*��DRAM�д�ŵ���Ϣ��������ʧ��*/
#define GM_DRAM_COM_READ_B_MSGNUM_ERR					0xA0100402U				/*��Ϣ����Ϊ0*/
#define GM_DRAM_COM_READ_B_INDEX_ERR					0xA0100403U				/*��Ϣindex�������ֵ*/
#define GM_DRAM_COM_READ_B_READMSGSIZE_ERR				0xA0100404U				/*��ȡ��Ϣ��Сʧ�ܻ��С�д���*/
#define GM_DRAM_COM_READ_B_READCRC_ERR					0xA0100405U				/*��ȡ��ϢCRCʧ��*/
#define GM_DRAM_COM_READ_B_MSGSIZETOMAX_ERR				0xA0100406U				/*��ǰindex + ��Ϣ���ݲ���������ȡindex*/
#define GM_DRAM_COM_READ_B_READDATE_ERR					0xA0100407U				/*��ȡ��Ϣ����ʧ��*/
#define GM_DRAM_COM_READ_B_CHECKCRC_ERR					0xA0100408U				/*У����ϢCRCʧ��*/
#define GM_DRAM_COM_READ_B_CALLBACK_ERR					0xA0100409U				/*GM_DRAM_COM_CALL_BACK_FUN_B����ʧ��*/


/********************MEMORY����,�����Ϊ0xA011XXXX****************/
/**GM_Simple_Memory_Ini(),0xA01101XX**/
#define GM_SIMPLE_MEMORY_INI_PAR_ERR					0xA0110100U				/*GM_Simple_Memory_Ini()�������*/
#define GM_SIMPLE_MEMORY_INI_ALREADY_ERR				0xA0110101U				/*GM_Simple_Memory_Ini()��ִ�г�ʼ��*/

/********************VERSION����,�����Ϊ0xA012XXXX****************/
/**GM_Version_Check(),0xA01201XX**/
#define GM_VERSION_CHECK_PAR_ERR					0xA0120100U					/*GM_Version_Check()�������*/
#define GM_VERSION_CHECK_LENGTH_ERR					0xA0120101U					/*GM_Version_Check()�����length����*/
#define GM_VERSION_CHECK_ATOUI_ERR					0xA0120102U					/*�ַ���ת��Ϊ���ֳ���*/
#define GM_VERSION_CHECK_STRING_ERR					0xA0120103U					/*�ַ�����ʽ����*/
#define GM_VERSION_CHECK_VERSIONVALUE_ERR			0xA0120104U					/*�ַ����汾��ֵ�������ֵ*/
#define GM_VERSION_CHECK_DATE_ERR					0xA0120105U					/*���ڲ��Ϸ�*/
#define GM_VERSION_CHECK_PERIOD_ERR					0xA0120106U					/*�����׶β��Ϸ�*/

/**GM_Version_Date_Decode(),0xA01202XX**/
#define GM_VERSION_DATE_DECODE_PAR_ERR				0xA0120200U					/*GM_Version_Date_Decode()�����������*/
#define GM_VERSION_DATE_DECODE_LENGTH_ERR			0xA0120201U					/*GM_Version_Date_Decode()�����length����*/
#define GM_VERSION_DATE_DECODE_CHECK_ERR			0xA0120202U					/*GM_Version_Check()��鲻ͨ��*/
#define GM_VERSION_DATE_DECODE_CHECK_STRING_ERR		0xA0120203U					/*�ַ�����ʽ����*/
#define GM_VERSION_DATE_DECODE_CHECK_PERIOD_ERR		0xA0120204U					/*�����׶β��Ϸ�*/


/**********************MD5����,�����Ϊ0xA013XXXX*****************/
/**GM_MD5_String(),0xA01301XX**/
#define GM_MD5_STRING_POUTPUTMD5BUF_ERR						0xA0130100U					/*GM_MD5_String()�������pOutputMD5Buf����*/
#define GM_MD5_STRING_INPUTSTRINGLEN_ERR					0xA0130101U					/*GM_MD5_String()�������inputString,len����*/
#define GM_MD5_STRING_INIT_ERR								0xA0130102U					/*GM_MD5_Init()����ʧ��*/
#define GM_MD5_STRING_UPDATE_ERR							0xA0130103U					/*GM_MD5_Update()����ʧ��*/
#define GM_MD5_STRING_FINAL_ERR								0xA0130104U					/*GM_MD5_Final()����ʧ��*/

/**GM_MD5_String_Compare(),0xA01302XX**/
#define GM_MD5_STRING_COMPARE_POUTPUTMD5BUF_ERR				0xA0130200U					/*GM_MD5_String_Compare()�������pInputMD5Buf����*/
#define GM_MD5_STRING_COMPARE_NOT_EQUAL						0xA0130201U					/*������ַ�����MD5�벻ƥ��*/
#define GM_MD5_STRING_COMPARE_MD5_STRING_EQUAL				0xA0130202U					/*GM_MD5_String()����ʧ��*/

/**GM_MD5_File(),0xA01303XX**/
#define GM_MD5_FILE_PAR_ERR									0xA0130300U					/*GM_MD5_File()�����������*/
#define GM_MD5_FILE_OPEN_ERR								0xA0130301U					/*fopen()���ļ�ʧ��*/
#define GM_MD5_FILE_INIT_ERR								0xA0130302U					/*GM_MD5_Init()����ʧ��*/
#define GM_MD5_FILE_UPDATE_ERR								0xA0130303U					/*GM_MD5_Update()����ʧ��*/
#define GM_MD5_FILE_FINAL_ERR								0xA0130304U					/*GM_MD5_Final()����ʧ��*/

/**GM_MD5_File_Compare(),0xA01304XX**/
#define GM_MD5_FILE_COMPARE_PAR_ERR							0xA0130400U					/*GM_MD5_File_Compare�����������*/
#define GM_MD5_FILE_COMPARE_FILEA_ERR						0xA0130401U					/*GM_MD5_File()����A��MD5����ʧ��*/
#define GM_MD5_FILE_COMPARE_FILEB_ERR						0xA0130402U					/*GM_MD5_File()����B��MD5����ʧ��*/
#define GM_MD5_FILE_COMPARE_FAIL							0xA0130403U					/*�ȽϷ���ʧ��*/

/**GM_MD5_Context_Compare(),0xA01305XX**/
#define GM_MD5_CONTEXT_COMPARE_PAR_ERR						0xA0130500U					/*GM_MD5_Context_Compare()�����������*/
#define GM_MD5_CONTEXT_COMPARE_FAIL							0xA0130501U					/*�ȽϷ���ʧ��*/
#define GM_MD5_CONTEXT_COMPARE_FINAL_ERR					0xA0130502U					/*GM_MD5_Final()����ʧ��*/


/**********************SHA1����,�����Ϊ0xA014XXXX*****************/
/**GM_SHA1_String(),0xA01401XX**/
#define GM_SHA1_STRING_PAR_ERR								0xA0140100U					/*GM_SHA1_String()�����������*/
#define GM_SHA1_STRING_SHA1_INIT_ERR						0xA0140101U					/*GM_SHA1_Init()����ʧ��*/
#define GM_SHA1_STRING_SHA1_UPDATE_ERR						0xA0140102U					/*GM_SHA1_Update()����ʧ��*/
#define GM_SHA1_STRING_SHA1_FINAL_ERR						0xA0140103U					/*GM_SHA1_Final()����ʧ��*/

/**GM_SHA1_String_Compare(),0xA01402XX**/
#define GM_SHA1_STRING_COMPARE_FAIL							0xA0140200U					/*����ȣ��Ƚ�ʧ��*/
#define GM_SHA1_STRING_COMPARE_STRING_FAIL					0xA0140201U					/*GM_SHA1_String()����ʧ��*/

/**GM_SHA1_File(),0xA01403XX**/
#define GM_SHA1_FILE_PAR_ERR								0xA0140300U					/*GM_SHA1_File()�����������*/
#define GM_SHA1_FILE_OPEN_ERR								0xA0140301U					/*GM_SHA1_File()���ļ�ʧ��*/
#define GM_SHA1_FILE_INIT_ERR								0xA0140302U					/*GM_SHA1_Init()����ʧ��*/
#define GM_SHA1_FILE_UPDATE_ERR								0xA0140303U					/*GM_SHA1_Update()����ʧ��*/
#define GM_SHA1_FILE_FINAL_ERR								0xA0140304U					/*GM_SHA1_Final()����ʧ��*/

/**GM_SHA1_File_Compare(),0xA01404XX**/
#define GM_SHA1_FILE_COMPARE_PAR_ERR						0xA0140400U					/*GM_SHA1_File_Compare()�����������*/
#define GM_SHA1_FILE_COMPARE_FAIL							0xA0140401U					/*GM_SHA1_File_Compare()�����������*/
#define GM_SHA1_FILE_COMPARE_FILE_B_ERR						0xA0140402U					/*����filePathB��SHA1ֵ����ʧ��*/
#define GM_SHA1_FILE_COMPARE_FILE_A_ERR						0xA0140403U					/*����filePathA��SHA1ֵ����ʧ��*/


/**********************CODE����,�����Ϊ0xA015XXXX*****************/
/**GM_Code_HEX2INT(),0xA01501XX**/
#define GM_CODE_HEX2INT_PAR_ERR								0xA0150100U					/*GM_Code_HEX2INT()�����������*/
#define GM_CODE_HEX2INT_HEX_ERR								0xA0150101U					/*pHEX������ʮ���������ĸ�ʽҪ��*/


/*******************MULTITHREAD����,�����Ϊ0xA016XXXX**************/
/**GM_Thread_Create(),0xA01601XX**/
#define GM_THREAD_CREATE_WIN32_STACKSIZE_ERR				0xA0160100U					/*WIN32�£�����stackSizeС��0*/
#define GM_THREAD_CREATE_WIN32_PRIORITY_ERR					0xA0160101U					/*WIN32�£�����priority����*/
#define GM_THREAD_CREATE_WIN32_SETPRIORITY_ERR				0xA0160102U					/*WIN32�£������߳����ȼ�����ʧ��*/
#define GM_THREAD_CREATE_WIN32_BEGINTHREADEX_ERR			0xA0160103U					/*WIN32�£������̷߳���ʧ��*/

#define GM_THREAD_CREATE_QNX_PAR_ERR						0xA0160104U					/*QNX�£������������*/
#define GM_THREAD_CREATE_QNX_SETINHERITSCHED_ERR			0xA0160105U					/*QNX�£�pthread_attr_setinheritsched����ʧ��*/
#define GM_THREAD_CREATE_QNX_SETSCHEDPARAM_ERR				0xA0160106U					/*QNX�£�pthread_attr_setschedparam����ʧ��*/
#define GM_THREAD_CREATE_QNX_SETSTACKSIZE_ERR				0xA0160107U					/*QNX�£�pthread_attr_setstacksize����ʧ��*/
#define GM_THREAD_CREATE_QNX_SETGUARDSIZE_ERR				0xA0160108U					/*QNX�£�pthread_attr_setguardsize����ʧ��*/
#define GM_THREAD_CREATE_QNX_PTHREADCREATE_ERR				0xA0160109U					/*QNX�£�pthread_create����ʧ��*/
#define GM_THREAD_CREATE_QNX_PAR_GUARDSIZE_ERR				0xA016010AU					/*QNX�£��������guardSize����*/

#define GM_THREAD_CREATE_VXWORKS_PAR_ERR					0xA016010BU					/*VXWORKS�£������������*/
#define GM_THREAD_CREATE_VXWORKS_TASKSPAWN_ERR				0xA016010CU					/*VXWORKS�£�taskSpawn����ʧ��*/


/********************NETWORK����,�����Ϊ0xA017XXXX***************/
/**GM_IP_Set(),0xA01701XX**/
#define GM_IP_SET_PAR_ERR								0xA0170100U						/*GM_IP_Set()�����������*/
#define GM_IP_SET_VXWORKS_IPATTACH_ERR					0xA0170101U						/*Vxworks�£�ipAttach()���ش���*/
#define GM_IP_SET_VXWORKS5_5_INETADDR_ERR				0xA0170102U						/*Vxworks5_5�£�inet_addr()���ش���*/
#define GM_IP_SET_VXWORKS5_5_IFMASKSET_ERR				0xA0170103U						/*Vxworks5_5�£�ifMaskSet()���ش���*/
#define GM_IP_SET_VXWORKS5_5_IFADDRSET_ERR				0xA0170104U						/*Vxworks5_5�£�ifAddrSet()���ش���*/
#define GM_IP_SET_VXWORKS6_6_IFCONFIG_ERR				0xA0170105U						/*Vxworks6_6�£�ifconfig()���ش���*/
#define GM_IP_SET_QNX_SYSTEM_ERR						0xA0170106U						/*QNX�£�system()���ش���*/

/**GM_IP_GW_Set(),0xA01702XX**/
#define GM_IP_GW_SET_PAR_ERR							0xA0170200U						/*GM_IP_GW_Set()�����������*/
#define GM_IP_GW_SET_VXWORKS5_5_MROUTEADD_ERR			0xA0170201U						/*VXWORKS5_5�£�mRouteAdd()����ʧ��*/
#define GM_IP_GW_SET_VXWORKS6_6_GATEMASK_ERR			0xA0170202U						/*VXWORKS6_6��,pGateWayMaskΪ��*/
#define GM_IP_GW_SET_VXWORKS6_6_ROUTEC_ERR				0xA0170203U						/*VXWORKS6_6��,routec()����ʧ��*/
#define GM_IP_GW_SET_QNX_GATEMASK_ERR					0xA0170204U						/*QNX��,pGateWayMaskΪ��*/
#define GM_IP_GW_SET_QNX_SYSTEM_ERR						0xA0170205U						/*QNX��,system()���ش���*/

/**GM_Socket_Create(),0xA01703XX**/
#define GM_SOCKET_CREATE_PAR_ERR						0xA0170300U						/*GM_Socket_Create()�����������*/
#define GM_SOCKET_CREATE_WIN32_WSAIOCTL_ERR				0xA0170301U						/*WIN32�£�WSAIoctl()����ʧ��*/
#define GM_SOCKET_CREATE_SOCKET_ERR						0xA0170302U						/*socket()����ʧ��*/

/**GM_TCP_Create_Socket(),0xA01704XX**/
#define GM_TCP_CREATE_SOCKET_PAR_ERR					0xA0170400U						/*GM_TCP_Create_Socket()�����������*/
#define GM_TCP_CREATE_SOCKET_FAIL						0xA0170401U						/*GM_TCP_Create_Socket()����socketʧ��*/
#define GM_TCP_CREATE_SOCKET_IOCTL_ERR					0xA0170402U						/*ioctl()����ʧ��*/
#define GM_TCP_CREATE_SOCKET_BIND_ERR					0xA0170403U						/*bind()����ʧ��*/

/**GM_UDP_Is_Read_Ready(),0xA01705XX**/
#define	GM_UDP_IS_READ_READY_PAR_ERR					0xA0170500U						/*GM_UDP_Is_Read_Ready()�����������*/
#define	GM_UDP_IS_READ_READY_SELECT_ERR					0xA0170501U						/*select()����ʧ��*/
#define	GM_UDP_IS_READ_READY_FD_ISSET_ERR				0xA0170502U						/*FD_ISSET()����ʧ��*/

/**GM_UDP_Is_Write_Ready(),0xA01706XX**/
#define GM_UDP_IS_WRITE_READY_PAR_ERR					0xA0170600U						/*GM_UDP_Is_Write_Ready()�����������*/
#define	GM_UDP_IS_WRITE_READY_SELECT_ERR				0xA0170601U						/*select()����ʧ��*/
#define	GM_UDP_IS_WRITE_READY_FD_ISSET_ERR				0xA0170602U						/*FD_ISSET()����ʧ��*/

/**GM_UDP_Socket_Create(),0xA01707XX**/
#define GM_UDP_SOCKET_CREATE_PAR_ERR					0xA0170700U						/*GM_UDP_Socket_Create()�����������*/
#define GM_UDP_SOCKET_CREATE_FAIL						0xA0170701U						/*GM_UDP_Socket_Create()����socketʧ��*/
#define GM_UDP_SOCKET_CREATE_IOCTL_ERR					0xA0170702U						/*ioctl()����ʧ��*/
#define GM_UDP_SOCKET_CREATE_BIND_ERR					0xA0170703U						/*bind()����ʧ��*/

/**GM_UDP_Send_Msg(),0xA01708XX**/
#define GM_UDP_SEND_MSG_PAR_ERR							0xA0170800U						/*GM_UDP_Send_Msg()�����������*/
#define GM_UDP_SEND_MSG_IS_WRITE_READY_ERR				0xA0170801U						/*GM_UDP_Is_Write_Ready()�����������*/
#define GM_UDP_SEND_MSG_SENDTO_ERR						0xA0170802U						/*sendto()����ʧ��*/

/**GM_UDP_Recv_Msg(),0xA01709XX**/
#define GM_UDP_RECV_MSG_PAR_ERR							0xA0170900U						/*GM_UDP_Recv_Msg()�����������*/
#define GM_UDP_RECV_MSG_RECVNUM_ERR						0xA0170901U						/*recvfrom���ش���*/


/*********************SNTP����,�����Ϊ0xA018XXXX****************/
/*GM_SNTP_Client_Update_Time(),0xA01801XX*/
#define GM_SNTP_CLIENT_UPDATE_TIME_H2N_ERR				0xA0180100U						/*GM_SNTP_Msg_H2N_N2H()����ʧ��*/
#define GM_SNTP_CLIENT_UPDATE_TIME_CLRBUF_ERR			0xA0180101U						/*GM_UDP_UnBlock_Clear_Buffer()����ʧ��*/
#define GM_SNTP_CLIENT_UPDATE_TIME_N2H_ERR				0xA0180102U						/*GM_SNTP_Msg_H2N_N2H()����ʧ��*/
#define GM_SNTP_CLIENT_UPDATE_TIME_SELECT_ERR			0xA0180103U						/*select()����ʧ��*/
#define GM_SNTP_CLIENT_UPDATE_TIME_RECV_ERR				0xA0180104U						/*δ���յ���ȷ����*/
#define GM_SNTP_CLIENT_UPDATE_TIME_N2H_ERR_2			0xA0180105U						/*GM_SNTP_Msg_H2N_N2H()����ʧ��*/

/*GM_SNTP_Client_Init(),0xA01802XX*/
#define GM_SNTP_CLIENT_INIT_PAR_ERR						0xA0180200U						/*GM_SNTP_Client_Init()�����������*/
#define GM_SNTP_CLIENT_INIT_UDPINIT_ERR					0xA0180201U						/*GM_UDP_Init()����ʧ��*/
#define GM_SNTP_CLIENT_INIT_SOCKETCREAT_ERR				0xA0180202U						/*GM_UDP_Socket_Create()����ʧ��*/
#define GM_SNTP_CLIENT_INIT_THREADCREAT_ERR				0xA0180203U						/*GM_Thread_Create()����ʧ��*/


/***********************PD����,�����Ϊ0xA019XXXX****************/
/*GM_PD_Operation(),0xA01901XX*/
#define GM_PD_OPERATION_PAR_ERR							0xA0190100U						/*GM_PD_Operation()�����������*/
#define GM_PD_OPERATION_ISALGORITHMA_ERR				0xA0190101U						/*GM_PD_Operation()����isAlgorithmA����*/

/*GM_PD_Operation2(),0xA01902XX*/
#define GM_PD_OPERATION2_PAR_ERR						0xA0190200U						/*GM_PD_Operation2()�����������*/
#define GM_PD_OPERATION2_ISALGORITHMA_ERR				0xA0190201U						/*GM_PD_Operation2()����isAlgorithmA����*/

/*GM_PD_Inverse(),0xA01903XX*/
#define GM_PD_INVERSE_PAR_ERR							0xA0190300U						/*GM_PD_Inverse()�����������*/
#define GM_PD_INVERSE_ISALGORITHMA_ERR					0xA0190301U						/*GM_PD_Inverse()����isAlgorithmA����*/


/***********************LFSR����,�����Ϊ0xA01AXXXX****************/
/**����**/


/***********************WD����,�����Ϊ0xA01BXXXX****************/
/**GM_WD_Start(),0xA01B01XX**/
#define GM_WD_START_QNX_PAR_ERR						0xA01B0100U						/*QNX��,�������period����*/
#define GM_WD_START_QNX_WDINIT_ERR					0xA01B0101U						/*QNX��,watchDogInit()����ʧ��*/
#define GM_WD_START_VXWORKS6_6_PAR_ERR				0xA01B0102U						/*VXWORKS6_6��,�������period����*/
#define GM_WD_START_VXWORKS6_6_WDINIT_ERR			0xA01B0103U						/*VXWORKS6_6��,sysPldWdgInit()����ʧ��*/
#define GM_WD_START_VXWORKS6_6_WDRESSET_ERR			0xA01B0104U						/*VXWORKS6_6��,sysPldWdgResSet()����ʧ��*/
#define GM_WD_START_VXWORKS6_6_WDSTART_ERR			0xA01B0105U						/*VXWORKS6_6��,sysPldWdgResSet()����ʧ��*/


/***********************VCPTOCRC����,�����Ϊ0xA01CXXXX****************/
/**GM_VCP2CRC_Get_Cnf(),0xA01C01XX**/
#define GM_VCP2CRC_GET_CNF_INIT_ERR					0xA01C0100U						/*��ȡ��������ʱδ���г�ʼ��*/

/**GM_VCP2CRC_Init(),0xA01C02XX**/
#define GM_VCP2CRC_INIT_PAR_ERR						0xA01C0200U						/*��ʼ��ʱ�����������*/
#define GM_VCP2CRC_INIT_DATALENGTH_ERR				0xA01C0201U						/*���������ļ����ݳ����������ļ��м�¼�Ĳ�һ��*/
#define GM_VCP2CRC_INIT_MD5_CAL_ERR					0xA01C0202U						/*���������ļ�MD5ֵ���ش���*/
#define GM_VCP2CRC_INIT_MD5_CMP_ERR					0xA01C0203U						/*�Ƚ�MD5��һ��*/
#define GM_VCP2CRC_INIT_MALLOC_ERR					0xA01C0204U						/*�����ڴ�ʧ��*/

/**GM_VCP2CRC_Encode_Msg(),0xA01C03XX**/
#define GM_VCP2CRC_ENCODE_MSG_INIT_ERR				0xA01C0300U						/*δ���������ĳ�ʼ������*/
#define GM_VCP2CRC_ENCODE_MSG_PAR_ERR				0xA01C0301U						/*����Ĳ�������*/
#define GM_VCP2CRC_ENCODE_MSG_CRCMODE_ERR			0xA01C0302U						/*�����CRC_Mode����*/
#define GM_VCP2CRC_ENCODE_MSG_MSGTYPE_ERR			0xA01C0303U						/*δ�ҵ���Ӧ��msg_type*/
#define GM_VCP2CRC_ENCODE_MSG_VARNUM_ERR			0xA01C0304U						/*�����VarNum���ڸ�������Ϣ���������*/
#define GM_VCP2CRC_ENCODE_MSG_MALLOC_ERR			0xA01C0305U						/*�����ڴ�ʧ��*/
#define GM_VCP2CRC_ENCODE_MSG_GET_CDWLEN_ERR		0xA01C0306U						/*GM_VCP2CRC_Get_Cdw_Length()����ʧ��*/
#define GM_VCP2CRC_ENCODE_MSG_CAL_CRC32_ERR			0xA01C0307U						/*GM_VCP2CRC_Calculate_CRC32()����ʧ��*/

/**GM_VCP2CRC_Decode_Msg(),0xA01C04XX**/
#define GM_VCP2CRC_DECODE_MSG_INIT_ERR				0xA01C0400U						/*δ���������ĳ�ʼ������*/
#define GM_VCP2CRC_DECODE_MSG_PAR_ERR				0xA01C0401U						/*����Ĳ�������*/
#define GM_VCP2CRC_DECODE_MSG_CRCMODE_ERR			0xA01C0402U						/*�����CRC_Mode����*/
#define GM_VCP2CRC_DECODE_MSG_MSGTYPE_ERR			0xA01C0403U						/*δ�ҵ���Ӧ��msg_type*/
#define GM_VCP2CRC_DECODE_MSG_MALLOC_ERR			0xA01C0404U						/*�����ڴ�ʧ��*/
#define GM_VCP2CRC_DECODE_MSG_GET_CDWLEN_ERR		0xA01C0405U						/*GM_VCP2CRC_Get_Cdw_Length()����ʧ��*/
#define GM_VCP2CRC_DECODE_MSG_VARINDEX_ERR			0xA01C0406U						/*�����VCP_VarIndex[i]���ڸ�������Ϣ���������*/
#define GM_VCP2CRC_DECODE_MSG_CAL_CRC32_ERR			0xA01C0407U						/*GM_VCP2CRC_Calculate_CRC32()����ʧ��*/
#define GM_VCP2CRC_DECODE_MSG_CHECKCRC_ERR			0xA01C0408U						/*CRCУ��ʧ��*/

/**GM_VCP2CRC_Calculate_CRC32(),0xA01C05XX**/
#define GM_VCP2CRC_CALCULATE_CRC32_PAR_ERR			0xA01C0500U						/*�����β�����*/
#define GM_VCP2CRC_CALCULATE_CRC32_VARINDEX_ERR		0xA01C0501U						/*VCP_VarIndex����*/
#define GM_VCP2CRC_CALCULATE_CRC32_VARNUM_ERR		0xA01C0502U						/*������ı���������VarNum*/

/**GM_VCP2CRC_Get_Cdw_Length(),0xA01C06XX**/
#define GM_VCP2CRC_GET_CDW_LENGTH_PAR_ERR			0xA01C0600U						/*�����β�����*/
#define GM_VCP2CRC_GET_CDW_LENGTH_VARINDEX_ERR		0xA01C0601U						/*VCP_VarIndex���ݴ����±������ȴ���4*/

/**VCP_check(),0xA01C07XX,����DEBUGģʽ����Ч**/
#define GM_VCP2CRC_VCP_CHECK_FAIL					0xA01C0700U						/*VCPǩ��У��ʧ��*/



/******GM_Utils�д������Ϊ����ֵ���֣���������Ϊ0xA1XXXXXX����108��**********/
/********��Ϣ����MQ����,�����Ϊ0xA101XXXX********/

/*GM_MQ��GM_MQ_Light��GM_MQ_Fixed����Init�����ķ���ֵ�������Ϊ0xA10101XX*/
#define MQ_INIT_OK              0xA10101FFU          /*Init�����ɹ�*/
#define MQ_INIT_PAR_ERR         0xA1010100U          /*Init��δ���*/
#define MQ_INIT_MALLOC_ERR      0xA1010101U          /*Init�����ڴ�ʧ��*/
#define MQ_INIT_MUTEX_ERR       0xA1010102U          /*Init��ʼ��MQ������ʧ��*/

/*GM_MQ��GM_MQ_Light��GM_MQ_Fixed����Read�����ķ���ֵ�������Ϊ0xA10102XX*/
#define MQ_RD_OK                0xA10102FFU          /*Read�����ɹ�*/
#define MQ_RD_PAR_NULL          0xA1010200U          /*Read���ָ��Ϊ��*/
#define MQ_RD_INIT_ERR          0xA1010201U          /*Read������MQδ��ʼ��*/
#define MQ_RD_LOCK_ERR          0xA1010202U          /*Read���MQ��������Դʧ��*/
#define MQ_RD_UNLOCK_ERR        0xA1010203U          /*Read�ͷ�MQ��������Դʧ��*/
#define MQ_RD_MQ_EMPTY          0xA1010204U          /*Read����MQΪ��*/
#define MQ_RD_MSGSIZE_ERR       0xA1010205U          /*Read�����Ϣ������̫С*/

/*GM_MQ��GM_MQ_Light��GM_MQ_Fixed����Get�����ķ���ֵ�������Ϊ0xA10103XX*/
#define MQ_GET_OK               0xA10103FFU          /*Get�����ɹ�*/
#define MQ_GET_PAR_NULL         0xA1010300U          /*Get���ָ��Ϊ��*/
#define MQ_GET_INIT_ERR         0xA1010301U          /*Get������MQδ��ʼ��*/
#define MQ_GET_LOCK_ERR         0xA1010302U          /*Get���MQ��������Դʧ��*/
#define MQ_GET_UNLOCK_ERR       0xA1010303U          /*Get�ͷ�MQ��������Դʧ��*/
#define MQ_GET_INDEX_ERR        0xA1010304U          /*Get��������index����ʵ����Ϣ��*/
#define MQ_GET_MSGSIZE_ERR      0xA1010305U          /*Get�����Ϣ������̫С*/

/*GM_MQ��GM_MQ_Light��GM_MQ_Fixed����Write�����ķ���ֵ�������Ϊ0xA10104XX*/
#define MQ_WR_OK                0xA10104FFU          /*Write�����ɹ�*/
#define MQ_WR_PAR_NULL          0xA1010400U          /*Write���ָ��Ϊ��*/
#define MQ_WR_INIT_ERR          0xA1010401U          /*Write������MQδ��ʼ��*/
#define MQ_WR_MSGSIZE_ERR       0xA1010402U          /*Write��������ϢsizeΪ0���������������ֵ*/
#define MQ_WR_LOCK_ERR          0xA1010403U          /*Write���MQ��������Դʧ��*/
#define MQ_WR_UNLOCK_ERR        0xA1010404U          /*Write�ͷ�MQ��������Դʧ��*/
#define MQ_WR_MQ_FULL           0xA1010405U          /*Write����MQ����*/



/********GAPP/RMS/RMS_UDP����,�����Ϊ0xA102XXXX********/
/*GAPP�㺯������ֵ�������Ϊ0xA10201XX*/
#define GAPP_ADD_OK                    0xA10201FFU          /*GAPP���½ڵ���ӳɹ�*/
#define GAPP_ADD_ALREADY               0xA10201EEU          /*GAPP���Ѿ���ӹ��ýڵ�*/
#define GAPP_INTERWRITE_OK             0xA10201DDU          /*GM_GAPP_Inter_Write�������سɹ�*/
#define GAPP_INITS_OK                  0xA10201CCU          /*GM_GAPP_Init_S��ʼ���ɹ�*/
#define GAPP_WRITE_OK                  0xA10201BBU          /*GM_GAPP_WRITE���سɹ�*/
#define GAPP_INIT_FROM_ADDR_OK         0xA10201AAU          /*GM_GAPP_Init_From_Addr��ʼ���ɹ�*/
#define GAPP_PAR_ERR                   0xA1020100U          /*����GAPP����Ϣ��ʽ�д����RMS��δ��ɳ�ʼ��*/
#define GAPP_NODE_FULL                 0xA1020101U          /*GAPP�㶯̬��ӽڵ����Ѵﵽ���ֵ*/
#define GAPP_CHECK_ERR                 0xA1020102U          /*GAPP����֤ʧ��*/
#define GAPP_ADD_ERR                   0xA1020103U          /*GAPP�㶯̬���ʧ��*/
#define GAPP_MSG_CHANGE_ERR            0xA1020104U          /*�����ֽ���������ֽ���ת��ʧ��*/
#define GAPP_INTERWRITE_PAR_ERR        0xA1020105U          /*GM_GAPP_Inter_Write����Ĳ�������*/
#define GAPP_INTERWRITE_NOT_FOUND      0xA1020106U          /*GM_GAPP_Inter_Writeδ�ҵ��ýڵ������Ϣ*/
#define GAPP_INTERWRITE_CHECK_ERR      0xA1020107U          /*GM_GAPP_Inter_Write���յ�����Ϣ������֤ʧ��*/
#define GAPP_INTERWRITE_MSGID_ERR      0xA1020108U          /*GM_GAPP_Inter_Write�յ�FSFB2��Ϣ��MsgID����ȷ*/
#define GAPP_INTERWRITE_CALLBACK_ERR   0xA1020109U          /*GM_GAPP_Inter_Write�лص���������ʧ��*/
#define GAPP_WRITE_PAR_ERR             0xA102010AU          /*GM_GAPP_Write_Sδ��ʼ�������������*/
#define GAPP_WRITE_NOT_FOUND           0xA102010BU          /*GM_GAPP_Write_Sδ�ҵ��ýڵ����������*/
#define GAPP_WRITE_MSGID_ERR           0xA102010CU          /*GM_GAPP_Write_S��������Ϣ��MsgID����ȷ*/
#define GAPP_WRITE_CALLBACK_ERR        0xA102010DU          /*GM_GAPP_Write_S��GM_GAPP_Write_Callback_Fun����ʧ��*/
#define GAPP_INITS_PAR_ERR             0xA102010EU          /*GM_GAPP_Init_S�������ָ��Ϊ��*/
#define GAPP_INITS_FILESIZE_ERR        0xA102010FU          /*GM_GAPP_Init_S��������ļ���СΪ0����*/
#define GAPP_INITS_MALLOC_ERR          0xA1020110U          /*GM_GAPP_Init_S�����ڴ�ʧ��*/
#define GAPP_INITS_LOAD_FILE_ERR       0xA1020111U          /*GM_GAPP_Init_S��ȡ�����ļ�ʧ��*/
#define GAPP_INITS_READ_FILE_ERR       0xA1020112U          /*GM_GAPP_Init_S�������ļ��ж�ȡ����ʧ��*/
#define GAPP_INITS_UNIQUE_CHECK_ERR    0xA1020113U          /*GM_GAPP_Init_SΨһ�Լ��ʧ��*/     
#define GAPP_INITS_MUTEX_ERR           0xA1020114U          /*GM_GAPP_Init_S��ʼ��mutexʧ��*/
#define GAPP_INITADDR_PAR_ERR          0xA1020115U          /*GM_GAPP_Init_From_Addr����ָ��Ϊ��*/
#define GAPP_INITADDR_GETHOST_ERR      0xA1020116U          /*GM_GAPP_Init_From_Addr��GM_Multibin_GetHostʧ��*/
#define GAPP_INITADDR_MALLOC_ERR       0xA1020117U          /*GM_GAPP_Init_From_Addr�����ڴ�ʧ��*/
#define GAPP_INITADDR_MUTEX_ERR        0xA1020118U          /*GM_GAPP_Init_From_Addr��ʼ��mutexʧ��*/

/*RMS�㺯������ֵ�������Ϊ0xA10202XX*/
#define RMS_ADD_OK                    0xA10202FFU          /*RMS���½ڵ���ӳɹ�*/
#define RMS_ADD_ALREADY               0xA10202EEU          /*RMS���Ѿ���ӹ��ýڵ�*/
#define RMS_INTERWRITE_OK             0xA10202DDU          /*GM_RMS_Inter_Write���سɹ�*/
#define RMS_INITS_OK                  0xA10202CCU          /*GM_RMS_Init_S��ʼ���ɹ�*/
#define RMS_INIT_FROM_ADDR_OK         0xA10202BBU          /*GM_RMS_Init_From_Addr��ʼ���ɹ�*/
#define RMS_PAR_ERR                   0xA1020200U          /*����RMS����Ϣ��ʽ�д����RMS��δ��ɳ�ʼ��*/
#define RMS_NODE_FULL                 0xA1020201U          /*RMS�㶯̬��ӽڵ����Ѵﵽ���ֵ*/
#define RMS_CHECK_ERR                 0xA1020202U          /*RMS����֤ʧ��*/
#define RMS_ADD_ERR                   0xA1020203U          /*RMS�㶯̬���ʧ��*/
#define RMS_MSG_CHANGE_ERR            0xA1020204U          /*�����ֽ���������ֽ���ת��ʧ��*/
#define RMS_INTERWRITE_PAR_ERR        0xA1020205U          /*GM_RMS_Inter_Write����Ĳ�������*/
#define RMS_INTERWRITE_CHECK_ERR      0xA1020206U          /*GM_RMS_Inter_Write���յ�����Ϣ������֤ʧ��*/
#define RMS_INTERWRITE_DATASIZE_ERR   0xA1020207U          /*GM_RMS_Inter_Write��Ϣͷ��ͷ����С��һ��*/
#define RMS_INTERWRITE_RESET_TAB_ERR  0xA1020208U          /*GM_RMS_Inter_Write��GM_RMS_SNTable_Remote_Resetʧ��*/
#define RMS_INTERWRITE_RESET_PAR_ERR  0xA1020209U          /*GM_RMS_Inter_Write��SN=0,����Ϣ��������*/
#define RMS_INITS_PAR_ERR             0xA102020AU          /*GM_RMS_Init_S�������ָ��Ϊ��*/
#define RMS_INITS_FILESIZE_ERR        0xA102020BU          /*GM_RMS_Init_S��������ļ���СΪ0����*/
#define RMS_INITS_MALLOC_ERR          0xA102020CU          /*GM_RMS_Init_S�����ڴ�ʧ��*/
#define RMS_INITS_LOAD_FILE_ERR       0xA102020DU          /*GM_RMS_Init_S��ȡ�����ļ�ʧ��*/
#define RMS_INITS_READ_FILE_ERR       0xA102020EU          /*GM_RMS_Init_S�������ļ��ж�ȡ����ʧ��*/
#define RMS_INITS_MUTEX_ERR           0xA102020FU          /*GM_RMS_Init_S��ʼ��mutexʧ��*/
#define RMS_INITADDR_PAR_ERR          0xA1020210U          /*GM_RMS_Init_From_Addr����ָ��Ϊ��*/
#define RMS_INITADDR_GETHOST_ERR      0xA1020211U          /*GM_RMS_Init_From_Addr��GM_Multibin_GetHostʧ��*/
#define RMS_INITADDR_MUTEX_ERR        0xA1020212U          /*GM_RMS_Init_From_Addr��ʼ��mutexʧ��*/

/*RMS_UDP�㺯������ֵ�������Ϊ0xA10203XX*/
#define RMS_UDP_ADD_OK                0xA10203FFU         /*GM_RMS_UDP��ɹ���Ӷ�̬�ڵ�*/
#define RMS_UDP_CHECK_SRCADDR_OK      0xA10203EEU         /*GM_RMS_UDP_Check_SrcAddr��֤ͨ��*/
#define RMS_UDP_GETCURMSG_OK          0xA10203DDU         /*GM_RMS_UDP_GetCurMsg_Src�����Ϣ��Դindex CON�ͺ��������Գɹ�*/
#define RMS_UDP_INITS_OK              0xA10203CCU         /*GM_RMS_UDP_Init_S��ʼ���ɹ�*/
#define RMS_UDP_INIT_FROM_ADDR_OK     0xA10203BBU         /*GM_RMS_UDP_Init_From_Addr��ʼ���ɹ�*/
#define RMS_UDP_PAR_NULL              0xA1020300U         /*���ָ��Ϊ��*/
#define RMS_UDP_MUTEX_LOCK_ERR        0xA1020301U         /*RMS_UDP����mutexʧ��*/
#define RMS_UDP_MUTEX_UNLOCK_ERR      0xA1020302U         /*RMS_UDP���ͷ�mutexʧ��*/
#define RMS_UDP_CHECK_NEW_NODE        0xA1020303U         /*GM_RMS_UDP_Check_SrcAddr��֤�������½ڵ�*/
#define RMS_UDP_NODE_FULL             0xA1020304U         /*RMS_UDP�㶯̬��ӽڵ����Ѵﵽ���ֵ*/
#define RMS_UDP_ADD_ERR               0xA1020305U         /*RMS_UDP�㶯̬���ʧ��*/
#define RMS_UDP_CHECK_ERR             0xA1020306U         /*GM_RMS_UDP_Check_SrcAddr��֤�ýڵ㲻�ڱ������ñ�*/
#define RMS_UDP_GETCURMSG_PAR_ERR     0xA1020307U         /*GM_RMS_UDP_GetCurMsg_Src����Ĳ���ָ��Ϊ�ջ���sCurMsgSrcIndexΪ0xFF*/ 
#define RMS_UDP_INITS_PAR_ERR         0xA1020308U         /*GM_RMS_UDP_Init_S�������ָ��Ϊ��*/
#define RMS_UDP_INITS_UDP_INIT_ERR    0xA1020309U         /*GM_RMS_UDP_Init_S��ʼ��GM_UDP_Initʧ��*/
#define RMS_UDP_INITS_FILESIZE_ERR    0xA102030AU         /*GM_RMS_UDP_Init_S��������ļ���СΪ0����*/
#define RMS_UDP_INITS_MALLOC_ERR      0xA102030BU         /*GM_RMS_UDP_Init_S�����ڴ�ʧ��*/
#define RMS_UDP_INITS_LOAD_FILE_ERR   0xA102030CU        /*GM_RMS_UDP_Init_S��ȡ�����ļ�ʧ��*/
#define RMS_UDP_INITS_READ_FILE_ERR   0xA102030DU        /*GM_RMS_UDP_Init_S�������ļ��ж�ȡ����ʧ��*/
#define RMS_UDP_INITS_SOCKET_ERR      0xA102030EU        /*GM_RMS_UDP_Init_S����UDP_SOCKETʧ��*/
#define RMS_UDP_INITS_THREAD_ERR      0xA102030FU        /*GM_RMS_UDP_Init_S�����������ݼ�������ʧ��*/
#define RMS_UDP_INITS_MUTEX_ERR       0xA1020310U        /*GM_RMS_UDP_Init_S��ʼ��mutexʧ��*/
#define RMS_UDP_INITADDR_PAR_ERR      0xA1020311U        /*GM_RMS_UDP_Init_From_Addr����ָ��Ϊ��*/
#define RMS_UDP_INITADDR_GETHOST_ERR  0xA1020312U        /*GM_RMS_UDP_Init_From_Addr��GM_Multibin_GetHostʧ��*/
#define RMS_UDP_INITADDR_UDP_INIT_ERR 0xA1020313U        /*GM_RMS_UDP_Init_From_Addr��ʼ��GM_UDP_Initʧ��*/
#define RMS_UDP_INITADDR_SOCKET_ERR   0xA1020314U        /*GM_RMS_UDP_Init_From_Addr����UDP_SOCKETʧ��*/
#define RMS_UDP_INITADDR_THREAD_ERR   0xA1020315U        /*GM_RMS_UDP_Init_From_Addr�����������ݼ�������ʧ��*/
#define RMS_UDP_INITADDR_MUTEX_ERR    0xA1020316U        /*GM_RMS_UDP_Init_From_Addr��ʼ��mutexʧ��*/

/***************************************************************************/
/*                           END of GM_Utils                               */
/***************************************************************************/




#endif