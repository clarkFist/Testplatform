
#include "GM_RSSP1_SFM_Init.h"

#ifdef SFM_Part
extern RSSP1_MQ_Inter_struct g_SFM_MQ;

#ifndef CFM_Part
extern RSSP1_MQ_Inter_struct g_CFM_MQ;
#endif

extern GM_RSSP1_UINT32 g_valid_chn;

GM_RSSP1_BOOL GM_RSSP1_Init_Comm_Cfg_SFM(GM_RSSP1_INT8 *pIniFileContent , GM_RSSP1_comm_cfg_struct *pCfg)
{
	if ((NULL == pIniFileContent) || (NULL == pCfg))
	{
		return GM_RSSP1_FALSE;
	}

	/* 顺序不可变动。初始化各连接时需要从SFM cfg中获取本地连接通道共享的一些配置 */
	if (GM_RSSP1_FALSE == GM_RSSP1_Init_Comm_Global_SFM(pCfg, pIniFileContent))
	{
		return GM_RSSP1_FALSE;
	}

	if (GM_RSSP1_FALSE == GM_RSSP1_Init_Comm_Connection_SFM(pCfg, pIniFileContent))
	{
		return GM_RSSP1_FALSE;
	}

	return GM_RSSP1_TRUE;
}

GM_RSSP1_BOOL GM_RSSP1_Init_Comm_Global_SFM(GM_RSSP1_comm_cfg_struct *pCfg , GM_RSSP1_INT8 *pIniFileContent)
{
	GM_RSSP1_BOOL rt = GM_RSSP1_FALSE;
	GM_RSSP1_INT32 valueInt = -1;
	GM_RSSP1_INT8 valueString[256] = {0};
	GM_RSSP1_UINT32 mem_size = 0U;

	if ((NULL == pCfg) || (NULL == pIniFileContent))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_SFM_Point_Error, pCfg, pIniFileContent, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	/*added by huang 20151201*/
	rt = GM_RSSP1_Ini_File_Read_Int("RSSP1_GLOBAL", "main_cycle", &valueInt, pIniFileContent);
	if (GM_RSSP1_FALSE == rt)
	{
		return GM_RSSP1_FALSE;
	}
	else
	{
		pCfg->SFM_cfg.main_cycle = (GM_RSSP1_UINT32)valueInt;
	}
	/*the end 20151201*/		
	
	rt = GM_RSSP1_Ini_File_Read_Int("RSSP1_GLOBAL", "connection_num", &valueInt, pIniFileContent);

	if ((GM_RSSP1_FALSE == rt) || (valueInt < 0) || (valueInt > GM_RSSP1_MAX_SAFETY_CONNECTION_NUM))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_SFM_Connec_Num_Error, rt, valueInt, GM_RSSP1_MAX_SAFETY_CONNECTION_NUM, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	else
	{
		pCfg->SFM_cfg.connection_nums = (GM_RSSP1_UINT16)valueInt;
		g_valid_chn = (GM_RSSP1_UINT32)valueInt;
	}

	rt = GM_RSSP1_Ini_File_Read_String("RSSP1_GLOBAL", "source_addr", valueString , sizeof(valueString), pIniFileContent);

	if (GM_RSSP1_FALSE == rt)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_SFM_Src_Error, rt, valueInt, GM_RSSP1_MAX_SAFETY_CONNECTION_NUM, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	else
	{
		pCfg->SFM_cfg.source_addr = (GM_RSSP1_UINT16)(strtoul(valueString , NULL , 16));
	}

	rt = GM_RSSP1_Ini_File_Read_String("RSSP1_GLOBAL", "local_sid_A", valueString , sizeof(valueString), pIniFileContent);

	if (GM_RSSP1_FALSE == rt)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_SFM_Loc_Sid_Error, rt, 1, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	else
	{
		pCfg->SFM_cfg.local_cfg[0].local_sid = (GM_RSSP1_UINT32)(strtoul(valueString , NULL , 16));
	}

	rt = GM_RSSP1_Ini_File_Read_String("RSSP1_GLOBAL", "local_sinit_A", valueString , sizeof(valueString), pIniFileContent);

	if (GM_RSSP1_FALSE == rt)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_SFM_Loc_Sinit_Error, rt, 1, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	else
	{
		pCfg->SFM_cfg.local_cfg[0].local_sinit = (GM_RSSP1_UINT32)(strtoul(valueString , NULL , 16));
	}

	rt = GM_RSSP1_Ini_File_Read_String("RSSP1_GLOBAL", "local_dataVer_A", valueString , sizeof(valueString), pIniFileContent);

	if (GM_RSSP1_FALSE == rt)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_SFM_Loc_DataVer_Error, rt, 1, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	else
	{
		pCfg->SFM_cfg.local_cfg[0].local_dataVer = (GM_RSSP1_UINT32)(strtoul(valueString , NULL , 16));
	}

	rt = GM_RSSP1_Ini_File_Read_String("RSSP1_GLOBAL", "local_sys_chk_A", valueString , sizeof(valueString), pIniFileContent);

	if (GM_RSSP1_FALSE == rt)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_SFM_Loc_SysChk_Error, rt, 1, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	else
	{
        pCfg->SFM_cfg.local_cfg[0].local_sys_chk = (~((GM_RSSP1_UINT32)(strtoul(valueString , NULL , 16))));
	}

	rt = GM_RSSP1_Ini_File_Read_String("RSSP1_GLOBAL", "local_sid_B", valueString , sizeof(valueString), pIniFileContent);

	if (GM_RSSP1_FALSE == rt)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_SFM_Loc_Sid_Error, rt, 2, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	else
	{
		pCfg->SFM_cfg.local_cfg[1].local_sid = (GM_RSSP1_UINT32)(strtoul(valueString , NULL , 16));
	}

	rt = GM_RSSP1_Ini_File_Read_String("RSSP1_GLOBAL", "local_sinit_B", valueString , sizeof(valueString), pIniFileContent);

	if (GM_RSSP1_FALSE == rt)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_SFM_Loc_Sinit_Error, rt, 2, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	else
	{
		pCfg->SFM_cfg.local_cfg[1].local_sinit = (GM_RSSP1_UINT32)(strtoul(valueString , NULL , 16));
	}

	rt = GM_RSSP1_Ini_File_Read_String("RSSP1_GLOBAL", "local_dataVer_B", valueString , sizeof(valueString), pIniFileContent);

	if (GM_RSSP1_FALSE == rt)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_SFM_Loc_DataVer_Error, rt, 2, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	else
	{
		pCfg->SFM_cfg.local_cfg[1].local_dataVer = (GM_RSSP1_UINT32)(strtoul(valueString , NULL , 16));
	}

	rt = GM_RSSP1_Ini_File_Read_String("RSSP1_GLOBAL", "local_sys_chk_B", valueString , sizeof(valueString), pIniFileContent);

	if (GM_RSSP1_FALSE == rt)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_SFM_Loc_SysChk_Error, rt, 2, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	else
	{
        pCfg->SFM_cfg.local_cfg[1].local_sys_chk = (~((GM_RSSP1_UINT32)(strtoul(valueString , NULL , 16))));
	}

	rt = GM_RSSP1_Ini_File_Read_Int("RSSP1_GLOBAL", "SFM_U2L_Q_Size_per_connection", &valueInt, pIniFileContent);

	if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_SFM_U2L_Q_Size_Error, rt, valueInt, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	else
	{
		pCfg->SFM_cfg.U2L_pri_Q_size = (GM_RSSP1_UINT32)valueInt;
	}

	rt = GM_RSSP1_Ini_File_Read_Int("RSSP1_GLOBAL", "SFM_L2U_Q_Size_per_connection", &valueInt, pIniFileContent);

	if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_SFM_L2U_Q_Size_Error, rt, valueInt, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	else
	{
		pCfg->SFM_cfg.L2U_pri_Q_size = (GM_RSSP1_UINT32)valueInt;
	}

	pCfg->SFM_cfg.U2L_pri_Q_offset = (GM_RSSP1_UINT32)GM_RSSP1_malloc(pCfg->SFM_cfg.U2L_pri_Q_size * sizeof(GM_RSSP1_SFM_U2L_pri_struct));
	if (0U == pCfg->SFM_cfg.U2L_pri_Q_offset)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_SFM_Malloc_Error, 1, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	GM_RSSP1_memset((void*)pCfg->SFM_cfg.U2L_pri_Q_offset, 0, (pCfg->SFM_cfg.U2L_pri_Q_size * sizeof(GM_RSSP1_SFM_U2L_pri_struct)));

	pCfg->SFM_cfg.L2U_pri_Q_offset  = (GM_RSSP1_UINT32)GM_RSSP1_malloc(pCfg->SFM_cfg.L2U_pri_Q_size*sizeof(GM_RSSP1_SFM_L2U_pri_struct));
	if (0U == pCfg->SFM_cfg.L2U_pri_Q_offset)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_SFM_Malloc_Error, 2, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	GM_RSSP1_memset((void*)pCfg->SFM_cfg.L2U_pri_Q_offset, 0, (pCfg->SFM_cfg.L2U_pri_Q_size*sizeof(GM_RSSP1_SFM_L2U_pri_struct)));


	#ifndef CFM_Part
	rt = GM_RSSP1_Ini_File_Read_Int("RSSP1_GLOBAL", "CFM_U2L_Q_Size_per_connection", &valueInt, pIniFileContent);

	if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_CFM_U2L_Q_Size_Error, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	else
	{
		pCfg->SFM_cfg.U2L_pri_Q_size_CFM    = (GM_RSSP1_UINT32)valueInt;
		pCfg->SFM_cfg.U2L_pri_Q_offset_CFM  = (GM_RSSP1_UINT32)GM_RSSP1_malloc(valueInt * sizeof(GM_RSSP1_CFM_U2L_pri_struct));

		if (NULL == pCfg->SFM_cfg.U2L_pri_Q_offset_CFM)
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_CFM_Malloc_Error, 1, 0, 0, 0, 0, 0);
			return GM_RSSP1_FALSE;
		}
		GM_RSSP1_memset((void*)pCfg->SFM_cfg.U2L_pri_Q_offset_CFM, 0, valueInt*sizeof(GM_RSSP1_CFM_U2L_pri_struct));
	}

	rt = GM_RSSP1_Ini_File_Read_Int("RSSP1_GLOBAL", "CFM_L2U_Q_Size_per_connection", &valueInt, pIniFileContent);

	if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_CFM_L2U_Q_Size_Error, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	else
	{
		pCfg->SFM_cfg.L2U_pri_Q_size_CFM    = (GM_RSSP1_UINT32)valueInt;
		pCfg->SFM_cfg.L2U_pri_Q_offset_CFM  = (GM_RSSP1_UINT32)GM_RSSP1_malloc(valueInt * sizeof(GM_RSSP1_CFM_L2U_pri_struct));

		if (NULL == pCfg->SFM_cfg.L2U_pri_Q_offset_CFM)
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_CFM_Malloc_Error, 2, 0, 0, 0, 0, 0);
			return GM_RSSP1_FALSE;
		}
		GM_RSSP1_memset((void*)pCfg->SFM_cfg.L2U_pri_Q_offset_CFM, 0, valueInt * sizeof(GM_RSSP1_CFM_L2U_pri_struct));
	}
	#endif

	return GM_RSSP1_TRUE;
}

GM_RSSP1_BOOL GM_RSSP1_Init_Comm_Connection_SFM(GM_RSSP1_comm_cfg_struct *pCfg , GM_RSSP1_INT8 *pIniFileContent)
{
	GM_RSSP1_BOOL rt = GM_RSSP1_FALSE;
	GM_RSSP1_INT32 valueInt = -1;
	GM_RSSP1_INT8 valueString[256] = {0};
	GM_RSSP1_BOOL valueBool = GM_RSSP1_FALSE;
	GM_RSSP1_INT8 section_key[256] = {0};   /* 连接通道键名 */
	GM_RSSP1_INT8 sub_section_key[256] = {0};   /* UDP通道键名 */
	GM_RSSP1_INT32 key_len = 0;
	GM_RSSP1_INT32 sub_key_len = 0;
	GM_RSSP1_INT32 conn_index = 0;
	GM_RSSP1_INT32 chn_index = 0;
	GM_RSSP1_INT32 fsfb_chn_index = 0;

	GM_RSSP1_SFM_connection_cfg_struct *pSFM_conn_cfg           = NULL;

	if ((NULL == pCfg) || (NULL == pIniFileContent))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_SFM_Point_Error, pCfg, pIniFileContent, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	/* 各个连接通道配置 */
	key_len = sizeof("CON_") - 1;

	GM_RSSP1_memcpy(section_key, "CON_", key_len + 1);

	for (conn_index = 0; conn_index < pCfg->SFM_cfg.connection_nums; ++conn_index)
	{
		pSFM_conn_cfg = &(pCfg->SFM_cfg.connection_cfg[conn_index]);

		GM_RSSP1_memset(section_key + key_len, 0, sizeof(section_key) - key_len);
		sprintf(section_key + key_len, "%d", conn_index);

		/*added by huang 20151201*/
		rt = GM_RSSP1_Ini_File_Read_Int(section_key, "FSFB_comm_cycle", &valueInt, pIniFileContent);
		if (GM_RSSP1_FALSE == rt)
		{
			return GM_RSSP1_FALSE;
		}
		else
		{
			pSFM_conn_cfg->fsfb_comm_cycle = (GM_RSSP1_UINT32)valueInt;
		}
		/*added by huang 20151201*/

		rt = GM_RSSP1_Ini_File_Read_String(section_key, "dest_addr", valueString , sizeof(valueString), pIniFileContent);

		if (GM_RSSP1_FALSE == rt)
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_SFM_Des_Error, conn_index, 0, 0, 0, 0, 0);
			return GM_RSSP1_FALSE;
		}
		else
		{
			pSFM_conn_cfg->dest_addr = (GM_RSSP1_UINT16)(strtoul(valueString , NULL , 16));
		}

		/* init FSFB param */
		rt = GM_RSSP1_Ini_File_Read_Int(section_key, "deltaTime", &valueInt, pIniFileContent);

		if ((GM_RSSP1_FALSE == rt) || (valueInt < 0) || (valueInt > GM_RSSP1_MAX_DELTATIME_ARRAY_SIZE))
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_SFM_Delta_Error, conn_index, 0, 0, 0, 0, 0);
			return GM_RSSP1_FALSE;
		}
		else
		{
			pSFM_conn_cfg->deltaTime = (GM_RSSP1_UINT16)valueInt;
		}

		/*CR:3894 configuratable for local time protection*/
		rt = GM_RSSP1_Ini_File_Read_Int(section_key, "DelayTime", &valueInt, pIniFileContent);

		if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_SFM_Loss_Error, conn_index, 0, 0, 0, 0, 0);
			return GM_RSSP1_FALSE;
		}
		else
		{
			pSFM_conn_cfg->DelayTime = (GM_RSSP1_UINT16)valueInt;
		}

		rt = GM_RSSP1_Ini_File_Read_Int(section_key, "lifeTime", &valueInt, pIniFileContent);

		if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_SFM_LifeTime_Error, conn_index, 0, 0, 0, 0, 0);
			return GM_RSSP1_FALSE;
		}
		else
		{
			pSFM_conn_cfg->lifeTime = (GM_RSSP1_UINT16)valueInt;
		}

		rt = GM_RSSP1_Ini_File_Read_Int(section_key, "torlerate_cycle", &valueInt, pIniFileContent);

		if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_SFM_TorCyc_Error, conn_index, 0, 0, 0, 0, 0);
			return GM_RSSP1_FALSE;
		}
		else
		{
			pSFM_conn_cfg->torlerate_cycle_cfg = (GM_RSSP1_UINT16)valueInt;
		}

		rt = GM_RSSP1_Ini_File_Read_Int(section_key, "num_data_ver", &valueInt, pIniFileContent);

		if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_SFM_DataVer_Error, conn_index, 0, 0, 0, 0, 0);
			return GM_RSSP1_FALSE;
		}
		else
		{
			pSFM_conn_cfg->num_data_ver = (GM_RSSP1_UINT8)valueInt;
		}

		rt = GM_RSSP1_Ini_File_Read_String(section_key, "remote_sid_A", valueString , sizeof(valueString), pIniFileContent);

		if (GM_RSSP1_FALSE == rt)
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_SFM_Rem_Sid_Error, conn_index, 1, 0, 0, 0, 0);
			return GM_RSSP1_FALSE;
		}
		else
		{
			pSFM_conn_cfg->fsfb_chn_cfg[0].remote_sid = (GM_RSSP1_UINT32)(strtoul(valueString , NULL , 16));
		}

		rt = GM_RSSP1_Ini_File_Read_String(section_key, "remote_sinit_A", valueString , sizeof(valueString), pIniFileContent);

		if (GM_RSSP1_FALSE == rt)
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_SFM_Rem_Sinit_Error, conn_index, 1, 0, 0, 0, 0);
			return GM_RSSP1_FALSE;
		}
		else
		{
			pSFM_conn_cfg->fsfb_chn_cfg[0].remote_sinit = (GM_RSSP1_UINT32)(strtoul(valueString , NULL , 16));
		}

		rt = GM_RSSP1_Ini_File_Read_String(section_key, "remote_dataVer_A", valueString , sizeof(valueString), pIniFileContent);

		if (GM_RSSP1_FALSE == rt)
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_SFM_Rem_DateVer_Error, conn_index, 1, 0, 0, 0, 0);
			return GM_RSSP1_FALSE;
		}
		else
		{
			pSFM_conn_cfg->fsfb_chn_cfg[0].remote_dataVer = (GM_RSSP1_UINT32)(strtoul(valueString , NULL , 16));
		}

		rt = GM_RSSP1_Ini_File_Read_String(section_key, "remote_sid_B", valueString , sizeof(valueString), pIniFileContent);

		if (GM_RSSP1_FALSE == rt)
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_SFM_Rem_Sid_Error, conn_index, 2, 0, 0, 0, 0);
			return GM_RSSP1_FALSE;
		}
		else
		{
			pSFM_conn_cfg->fsfb_chn_cfg[1].remote_sid = (GM_RSSP1_UINT32)(strtoul(valueString , NULL , 16));
		}

		rt = GM_RSSP1_Ini_File_Read_String(section_key, "remote_sinit_B", valueString , sizeof(valueString), pIniFileContent);

		if (GM_RSSP1_FALSE == rt)
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_SFM_Rem_Sinit_Error, conn_index, 2, 0, 0, 0, 0);
			return GM_RSSP1_FALSE;
		}
		else
		{
			pSFM_conn_cfg->fsfb_chn_cfg[1].remote_sinit = (GM_RSSP1_UINT32)(strtoul(valueString , NULL , 16));
		}

		rt = GM_RSSP1_Ini_File_Read_String(section_key, "remote_dataVer_B", valueString , sizeof(valueString), pIniFileContent);

		if (GM_RSSP1_FALSE == rt)
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_SFM_Rem_DateVer_Error, conn_index, 2, 0, 0, 0, 0);
			return GM_RSSP1_FALSE;
		}
		else
		{
			pSFM_conn_cfg->fsfb_chn_cfg[1].remote_dataVer = (GM_RSSP1_UINT32)(strtoul(valueString , NULL , 16));
		}

		/* 对等设备各个UDP通道均使用独立的FSFB。此时UDP通道数必须为1 */
		rt = GM_RSSP1_Ini_File_Read_BOOL(section_key , "enable_UDP_chn_FSFB" , &valueBool, pIniFileContent);

		if (GM_RSSP1_FALSE == rt)
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_SFM_EnableUDP_Error, conn_index, 0, 0, 0, 0, 0);
			return GM_RSSP1_FALSE;
		}
		else
		{
			pSFM_conn_cfg->b_enable_FSFB_on_chn = valueBool;
		}

		if (GM_RSSP1_TRUE == pSFM_conn_cfg->b_enable_FSFB_on_chn)
		{
			/* init UDP chn apply FSFB ID */
			rt = GM_RSSP1_Ini_File_Read_Int(section_key, "Chn_apply_FSFB_ID", &valueInt, pIniFileContent);

			if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
			{
				GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_SFM_FSFB_ID_Error, conn_index, 0, 0, 0, 0, 0);
				return GM_RSSP1_FALSE;
			}
			else
			{
				pSFM_conn_cfg->Chn_apply_FSFB_ID = (GM_RSSP1_UINT32)valueInt;
			}

			/* init msg queue in the SFM connection */
			rt = GM_RSSP1_Ini_File_Read_Int(section_key, "con_L2U_Q_size", &valueInt, pIniFileContent);

			if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
			{
				GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_SFM_L2U_Q_Size_Error, conn_index, 0, 0, 0, 0, 0);
				return GM_RSSP1_FALSE;
			}
			else
			{
				pSFM_conn_cfg->L2U_dat_Q_size = (GM_RSSP1_UINT32)valueInt;
				pSFM_conn_cfg->L2U_dat_Q_offset = (GM_RSSP1_UINT32)GM_RSSP1_malloc( valueInt*sizeof(GM_RSSP1_SFM_L2U_pri_struct));

				if (0U == pSFM_conn_cfg->L2U_dat_Q_offset)
				{
					GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_SFM_Malloc_Error, conn_index, 0, 0, 0, 0, 0);
					return GM_RSSP1_FALSE;
				}
				GM_RSSP1_memset((void*)pSFM_conn_cfg->L2U_dat_Q_offset, 0, valueInt*sizeof(GM_RSSP1_SFM_L2U_pri_struct));
			}
		}

		/* init local params form SFM cfg */
		for (fsfb_chn_index = 0 ; fsfb_chn_index < GM_RSSP1_CHECK_CHN_NUM ; ++fsfb_chn_index)
		{
			pSFM_conn_cfg->fsfb_chn_cfg[fsfb_chn_index].local_sid       = pCfg->SFM_cfg.local_cfg[fsfb_chn_index].local_sid;
			pSFM_conn_cfg->fsfb_chn_cfg[fsfb_chn_index].local_sinit     = pCfg->SFM_cfg.local_cfg[fsfb_chn_index].local_sinit;
			pSFM_conn_cfg->fsfb_chn_cfg[fsfb_chn_index].local_dataVer   = pCfg->SFM_cfg.local_cfg[fsfb_chn_index].local_dataVer;
			pSFM_conn_cfg->fsfb_chn_cfg[fsfb_chn_index].local_sys_chk   = pCfg->SFM_cfg.local_cfg[fsfb_chn_index].local_sys_chk;
		}

		/* init LSFR,CRC table */
		pSFM_conn_cfg->fsfb_chn_cfg[0].pLFSR_left_table     = (GM_RSSP1_UINT32 *)GM_RSSP1_LFSR_LEFT_CHN1_TABLE;
		pSFM_conn_cfg->fsfb_chn_cfg[0].pLFSR_right_table    = (GM_RSSP1_UINT32 *)GM_RSSP1_LFSR_RIGHT_CHN1_TABLE;
		pSFM_conn_cfg->fsfb_chn_cfg[0].pCRC32_table         = (GM_RSSP1_UINT32 *)GM_RSSP1_CRC32_Channel1_Table;
		pSFM_conn_cfg->fsfb_chn_cfg[0].pCRC16_table         = (GM_RSSP1_UINT16 *)GM_RSSP1_CRC16_Table_0x10811_LSB;
		pSFM_conn_cfg->fsfb_chn_cfg[1].pLFSR_left_table     = (GM_RSSP1_UINT32 *)GM_RSSP1_LFSR_LEFT_CHN2_TABLE;
		pSFM_conn_cfg->fsfb_chn_cfg[1].pLFSR_right_table    = (GM_RSSP1_UINT32 *)GM_RSSP1_LFSR_RIGHT_CHN2_TABLE;
		pSFM_conn_cfg->fsfb_chn_cfg[1].pCRC32_table         = (GM_RSSP1_UINT32 *)GM_RSSP1_CRC32_Channel2_Table;
		pSFM_conn_cfg->fsfb_chn_cfg[1].pCRC16_table         = (GM_RSSP1_UINT16 *)GM_RSSP1_CRC16_Table_0x10811_LSB;
	}

	return GM_RSSP1_TRUE;
}

GM_RSSP1_BOOL GM_RSSP1_SFM_Init(GM_RSSP1_INT8 *file)
{
	GM_RSSP1_comm_cfg_struct fsfb_cfg = {0};
	GM_RSSP1_SFM_object_struct *pSFM = NULL;

	pSFM = GM_RSSP1_Get_SFM_Object();

	if ((NULL == file) || (NULL == pSFM))
	{
		return GM_RSSP1_FALSE;
	}

	GM_RSSP1_memset(pSFM, 0, sizeof(GM_RSSP1_SFM_object_struct));

	if (GM_RSSP1_TRUE == GM_RSSP1_Init_Comm_Cfg_SFM(file, &fsfb_cfg))
	{
		if (GM_RSSP1_TRUE == GM_RSSP1_SFM_Init_Lib(&fsfb_cfg , pSFM))
		{
			return GM_RSSP1_TRUE;
		}
	}

	return GM_RSSP1_FALSE;
}

GM_RSSP1_BOOL GM_RSSP1_SFM_Init_Lib(GM_RSSP1_comm_cfg_struct *pFSFB_comm_cfg, GM_RSSP1_SFM_object_struct *pSFM)
{
	GM_RSSP1_INT32 conn_index = 0;
	GM_RSSP1_SFM_connection_struct *pSFM_conn = NULL;
	GM_RSSP1_SFM_connection_cfg_struct *pConn_cfg = NULL;
	GM_RSSP1_INT32 chn_index = 0;
	GM_RSSP1_INT32 rt = GM_RSSP1_ERROR;
	if ((NULL == pFSFB_comm_cfg) || (NULL == pSFM))
	{
		return GM_RSSP1_FALSE;
	}
	pSFM->lib_state = GM_RSSP1_Lib_State_Unknown;
	pSFM->source_addr = pFSFB_comm_cfg->SFM_cfg.source_addr;

	/*FSFB_STF_InitReverseTable();*/
	for (chn_index = 0 ; chn_index < GM_RSSP1_CHECK_CHN_NUM ; ++chn_index)
	{
		pSFM->local_cfg[chn_index]  = pFSFB_comm_cfg->SFM_cfg.local_cfg[chn_index];
	}
	pSFM->connection_nums = pFSFB_comm_cfg->SFM_cfg.connection_nums;
	for (conn_index = 0 ; conn_index < pSFM->connection_nums ; ++conn_index)
	{
		pSFM_conn = &(pSFM->connection[conn_index]);
		pConn_cfg = &(pFSFB_comm_cfg->SFM_cfg.connection_cfg[conn_index]);
#ifdef FSFB_DEBUG
		pSFM_conn->_dbg_fsfb_check_err_num = 0; /* 本通道校验失败次数 */
#endif
		pSFM_conn->index                = (GM_RSSP1_UINT8)conn_index;
		pSFM_conn->state                = GM_RSSP1_Layer_State_Unknow;
		pSFM_conn->source_addr          = pFSFB_comm_cfg->SFM_cfg.source_addr;
		pSFM_conn->dest_addr            = pConn_cfg->dest_addr;
		pSFM_conn->SaCEPID.source_addr  = pFSFB_comm_cfg->SFM_cfg.source_addr;
		pSFM_conn->SaCEPID.dest_addr    = pConn_cfg->dest_addr;
		pSFM_conn->deltaTime            = pConn_cfg->deltaTime;
		pSFM_conn->lifeTime             = pConn_cfg->lifeTime;
		pSFM_conn->DelayTime            = pConn_cfg->DelayTime;

		if(pSFM_conn->deltaTime > GM_RSSP1_MAX_DELTATIME_ARRAY_SIZE)
		{
			return GM_RSSP1_FALSE;
		}

		if(pSFM_conn->lifeTime > GM_RSSP1_MAX_TOLERATE_CYCLE)
		{
			return GM_RSSP1_FALSE;
		}

		pSFM_conn->num_data_ver         = pConn_cfg->num_data_ver;
		for (chn_index = 0 ; chn_index < GM_RSSP1_CHECK_CHN_NUM ; ++chn_index)
		{
			pSFM_conn->fsfb_chn[chn_index].chn_cfg  = pConn_cfg->fsfb_chn_cfg[chn_index];
		}
		if (GM_RSSP1_FALSE == GM_RSSP1_SFM_Ini_FSFB_Offline_Const(pSFM_conn, pSFM))
		{
			return GM_RSSP1_FALSE;
		}
		pSFM_conn->b_RSD_Already_Sent = GM_RSSP1_FALSE;
		pSFM_conn->b_enable_FSFB_on_chn = pConn_cfg->b_enable_FSFB_on_chn;
		if (GM_RSSP1_TRUE == pSFM_conn->b_enable_FSFB_on_chn)
		{
			pSFM_conn->Chn_apply_FSFB_ID = pConn_cfg->Chn_apply_FSFB_ID;
			if (GM_RSSP1_FALSE == FSFB_Msg_Queue_Init(&(pSFM_conn->L2U_dat_Q) , pConn_cfg->L2U_dat_Q_size , pConn_cfg->L2U_dat_Q_offset , FSFB_Q_TYPE_SFM_L2U_PRI))
			{
				return GM_RSSP1_FALSE;
			}
		}
		pSFM_conn->torlerate_cycle_cfg = pConn_cfg->torlerate_cycle_cfg;
		if(pSFM_conn->torlerate_cycle_cfg > GM_RSSP1_MAX_TOLERATE_CYCLE)
		{
			return GM_RSSP1_FALSE;
		}
		GM_RSSP1_memset(&(pSFM_conn->last_valid_dat_pri), 0 , sizeof(GM_RSSP1_SFM_L2U_pri_struct));
		pSFM_conn->b_rcvd_new_dat_in_cycle = GM_RSSP1_FALSE;
		pSFM_conn->b_torlerant_dat_valid   = GM_RSSP1_FALSE;
		rt = GM_RSSP1_TIME_Init(
			&(pSFM_conn->time) ,
			(pSFM_conn->fsfb_chn[0].chn_cfg.pLFSR_left_table),  /* SFM中使用的LFSR表相同，使用第0个通道的LFSR表即可 */
			(pSFM_conn->fsfb_chn[1].chn_cfg.pLFSR_left_table),
			pSFM_conn->fsfb_chn[0].chn_cfg.remote_sid,
			pSFM_conn->fsfb_chn[1].chn_cfg.remote_sid
			);
		if (GM_RSSP1_ERROR == rt)
		{
			return GM_RSSP1_FALSE;
		}

		/*added by huang 20151201*/
		pSFM_conn->PreciTime = -1;
		pSFM_conn->SSE_SSR_Delay = 0U;
		pSFM_conn->SINIT_TremCycle = 0U;
		pSFM_conn->RxSSRTrem = 0U;
		pSFM_conn->RxSSRTloc = 0U;
		pSFM_conn->LastSendSSECycle = 0U;
		pSFM_conn->TcycleLoc = pFSFB_comm_cfg->SFM_cfg.main_cycle;
		pSFM_conn->TcycleRem = pConn_cfg->fsfb_comm_cycle;
		/*the end 20151201*/
		/*CR:GM3943 maintian alignment*/		
		if (pSFM_conn->DelayTime == 0)
		{
			pSFM_conn->keepIdle = ((pSFM_conn->deltaTime * pSFM_conn->TcycleRem)+pSFM_conn->TcycleRem-1)/pSFM_conn->TcycleLoc;
		}
		else
		{
			pSFM_conn->keepIdle = pSFM_conn->DelayTime;
		}
		
		pSFM_conn->state                = GM_RSSP1_Layer_State_Free;
		pSFM_conn->remote_dev_AS_state  = GM_RSSP1_Remote_DEV_Unknow;
		pSFM_conn->state_check_counter1  = 0;
		pSFM_conn->state_check_counter2  = (GM_RSSP1_INT8)pSFM_conn->keepIdle;
	}/* GM_RSSP1_INT32 sfm connections */
	for (conn_index = 0 ; conn_index < pSFM->connection_nums ; ++conn_index)
	{
		pSFM_conn = &(pSFM->connection[conn_index]);
		for (chn_index = 0 ; chn_index < GM_RSSP1_CHECK_CHN_NUM ; ++chn_index)
		{
			pSFM_conn->env[chn_index].TC           = pSFM_conn->time.TC;
			pSFM_conn->env[chn_index].TS           = pSFM_conn->time.TS[chn_index];
			pSFM_conn->env[chn_index].local_TC     = 0U;
			pSFM_conn->env[chn_index].remote_TC    = 0U;
			pSFM_conn->env[chn_index].sseTS        = 0U;
			pSFM_conn->env[chn_index].sseTC        = 0U;
			pSFM_conn->env[chn_index].dynamicKey   = 0U;
			pSFM_conn->env[chn_index].bSendSSE     = 0U;
		}
	}
	if (GM_RSSP1_FALSE == FSFB_Msg_Queue_Init(&(g_SFM_MQ.SND_MQ)/*&(pSFM->U2L_pri_Q)*/ , pFSFB_comm_cfg->SFM_cfg.U2L_pri_Q_size , pFSFB_comm_cfg->SFM_cfg.U2L_pri_Q_offset , FSFB_Q_TYPE_SFM_U2L_PRI))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Init_Lib_MQ_Error, 1, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	if (GM_RSSP1_FALSE == FSFB_Msg_Queue_Init(&(g_SFM_MQ.RCV_MQ)/*&(pSFM->L2U_pri_Q)*/ , pFSFB_comm_cfg->SFM_cfg.L2U_pri_Q_size , pFSFB_comm_cfg->SFM_cfg.L2U_pri_Q_offset , FSFB_Q_TYPE_SFM_L2U_PRI))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Init_Lib_MQ_Error, 2, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
#ifndef CFM_Part
	if (GM_RSSP1_FALSE == FSFB_Msg_Queue_Init(&(g_CFM_MQ.SND_MQ)/*&(pSFM->U2L_pri_Q)*/ , pFSFB_comm_cfg->SFM_cfg.U2L_pri_Q_size_CFM , pFSFB_comm_cfg->SFM_cfg.U2L_pri_Q_offset_CFM , FSFB_Q_TYPE_CFM_U2L_PRI))
	{
		return GM_RSSP1_FALSE;
	}
	if (GM_RSSP1_FALSE == FSFB_Msg_Queue_Init(&(g_CFM_MQ.RCV_MQ)/*&(pSFM->L2U_pri_Q)*/ , pFSFB_comm_cfg->SFM_cfg.L2U_pri_Q_size_CFM , pFSFB_comm_cfg->SFM_cfg.L2U_pri_Q_offset_CFM , FSFB_Q_TYPE_CFM_L2U_PRI))
	{
		return GM_RSSP1_FALSE;
	}
#endif

	pSFM->lib_state = GM_RSSP1_Lib_State_Operational;
	return GM_RSSP1_TRUE;
}

#endif
