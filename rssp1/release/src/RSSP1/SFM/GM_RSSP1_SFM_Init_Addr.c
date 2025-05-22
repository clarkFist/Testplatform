
#include "GM_RSSP1_SFM_Init.h"
#ifdef SFM_Part
extern RSSP1_MQ_Inter_struct g_SFM_MQ;

#ifndef CFM_Part
extern RSSP1_MQ_Inter_struct g_CFM_MQ;
#endif

extern GM_RSSP1_UINT32 g_valid_chn;
GM_RSSP1_BOOL GM_RSSP1_Init_Comm_Cfg_SFM_Addr(GM_RSSP1_UINT8 *pFileContent , GM_RSSP1_comm_cfg_struct *pCfg)
{
	GM_RSSP1_INT32 file_len = -1;
	GM_RSSP1_BOOL rt = GM_RSSP1_FALSE;
	GM_RSSP1_UINT8 **rp_file = &pFileContent;

	if ((NULL == pFileContent) || (NULL == pCfg))
	{
		return GM_RSSP1_FALSE;
	}

	/* 顺序不可变动。初始化各连接时需要从SFM cfg中获取本地连接通道共享的一些配置 */
	if (GM_RSSP1_FALSE == GM_RSSP1_Init_Comm_Global_SFM_Addr(pCfg, pFileContent, rp_file))
	{
		return GM_RSSP1_FALSE;
	}

	if (GM_RSSP1_FALSE == GM_RSSP1_Init_Comm_Connection_SFM_Addr(pCfg, pFileContent, rp_file))
	{
		return GM_RSSP1_FALSE;
	}
	return GM_RSSP1_TRUE;
}

GM_RSSP1_BOOL GM_RSSP1_Init_Comm_Global_SFM_Addr(GM_RSSP1_comm_cfg_struct *pCfg , GM_RSSP1_UINT8 *pFileContent, GM_RSSP1_UINT8 **r_ptr)
{
	GM_RSSP1_BOOL rt = GM_RSSP1_FALSE;
	GM_RSSP1_UINT32 valueInt = 0U;	
	GM_RSSP1_BOOL isbig = GM_RSSP1_Code_Is_Big_Endian();
	GM_RSSP1_UINT8 i =0U;
	GM_RSSP1_UINT16 con_num = 0U;
	GM_RSSP1_INT8 conn_index = 0;
	GM_RSSP1_UINT8 refresh_mode = 0U;
	GM_RSSP1_UINT32 mem_size = 0U;

	if ((NULL == pCfg) || (NULL == pFileContent))
	{
		return GM_RSSP1_FALSE;
	}

	/* RSSP1_GLOBAL 全局配置 */
	valueInt = *(GM_RSSP1_UINT32*)pFileContent;
	pFileContent += sizeof(GM_RSSP1_UINT32);
	pCfg->SFM_cfg.main_cycle = (GM_RSSP1_UINT32)valueInt;

	valueInt = *(GM_RSSP1_UINT16*)pFileContent;
	pFileContent += sizeof(GM_RSSP1_UINT16);
	pCfg->SFM_cfg.connection_nums = (GM_RSSP1_UINT16)valueInt;

	valueInt = *(GM_RSSP1_UINT16*)pFileContent;
	pFileContent += sizeof(GM_RSSP1_UINT16);
	pCfg->SFM_cfg.source_addr = (GM_RSSP1_UINT16)valueInt;

	for (i=0; i<GM_RSSP1_CHECK_CHN_NUM; ++i)
	{
		valueInt = *(GM_RSSP1_UINT32*)pFileContent;
		pFileContent += sizeof(GM_RSSP1_UINT32);
		pCfg->SFM_cfg.local_cfg[i].local_sid = valueInt;

		valueInt = *(GM_RSSP1_UINT32*)pFileContent;
		pFileContent += sizeof(GM_RSSP1_UINT32);
		pCfg->SFM_cfg.local_cfg[i].local_sinit = valueInt;

		valueInt = *(GM_RSSP1_UINT32*)pFileContent;
		pFileContent += sizeof(GM_RSSP1_UINT32);
		pCfg->SFM_cfg.local_cfg[i].local_dataVer = valueInt;

		valueInt = *(GM_RSSP1_UINT32*)pFileContent;
		pFileContent += sizeof(GM_RSSP1_UINT32);
        pCfg->SFM_cfg.local_cfg[i].local_sys_chk = ~valueInt;
	}

	valueInt = *(GM_RSSP1_UINT32*)pFileContent;
	pFileContent += sizeof(GM_RSSP1_UINT32);
	pCfg->SFM_cfg.U2L_pri_Q_size = valueInt;

	valueInt = *(GM_RSSP1_UINT32*)pFileContent;
	pFileContent += sizeof(GM_RSSP1_UINT32);
	pCfg->SFM_cfg.L2U_pri_Q_size = valueInt;

	if (isbig == GM_RSSP1_FALSE)
	{
		pCfg->SFM_cfg.main_cycle = GM_RSSP1_Code_Big_Little_Endian32(pCfg->SFM_cfg.main_cycle);
		pCfg->SFM_cfg.connection_nums = GM_RSSP1_Code_Big_Little_Endian16(pCfg->SFM_cfg.connection_nums);
		if (pCfg->SFM_cfg.connection_nums > GM_RSSP1_MAX_SAFETY_CONNECTION_NUM)
		{
			return GM_RSSP1_FALSE;
		}
		pCfg->SFM_cfg.source_addr = GM_RSSP1_Code_Big_Little_Endian16(pCfg->SFM_cfg.source_addr);
		for (i=0;i<GM_RSSP1_CHECK_CHN_NUM;i++)
		{
			pCfg->SFM_cfg.local_cfg[i].local_sid = GM_RSSP1_Code_Big_Little_Endian32(pCfg->SFM_cfg.local_cfg[i].local_sid);
			pCfg->SFM_cfg.local_cfg[i].local_sinit = GM_RSSP1_Code_Big_Little_Endian32(pCfg->SFM_cfg.local_cfg[i].local_sinit);
			pCfg->SFM_cfg.local_cfg[i].local_dataVer = GM_RSSP1_Code_Big_Little_Endian32(pCfg->SFM_cfg.local_cfg[i].local_dataVer);
			pCfg->SFM_cfg.local_cfg[i].local_sys_chk = GM_RSSP1_Code_Big_Little_Endian32(pCfg->SFM_cfg.local_cfg[i].local_sys_chk);
		}
		pCfg->SFM_cfg.U2L_pri_Q_size = GM_RSSP1_Code_Big_Little_Endian32(pCfg->SFM_cfg.U2L_pri_Q_size);
		pCfg->SFM_cfg.L2U_pri_Q_size = GM_RSSP1_Code_Big_Little_Endian32(pCfg->SFM_cfg.L2U_pri_Q_size);
	}

	g_valid_chn = pCfg->SFM_cfg.connection_nums;

	pCfg->SFM_cfg.U2L_pri_Q_offset  = (GM_RSSP1_UINT32)GM_RSSP1_malloc(pCfg->SFM_cfg.U2L_pri_Q_size*sizeof(GM_RSSP1_SFM_U2L_pri_struct));
	if (0 == pCfg->SFM_cfg.U2L_pri_Q_offset)
	{
		return GM_RSSP1_FALSE;
	}
	GM_RSSP1_memset((void*)pCfg->SFM_cfg.U2L_pri_Q_offset, 0, pCfg->SFM_cfg.U2L_pri_Q_size*sizeof(GM_RSSP1_SFM_U2L_pri_struct));

	pCfg->SFM_cfg.L2U_pri_Q_offset  = (GM_RSSP1_UINT32)GM_RSSP1_malloc(pCfg->SFM_cfg.L2U_pri_Q_size*sizeof(GM_RSSP1_SFM_L2U_pri_struct));
	if (0 == pCfg->SFM_cfg.L2U_pri_Q_offset)
	{
		return GM_RSSP1_FALSE;
	}
	GM_RSSP1_memset((void*)pCfg->SFM_cfg.L2U_pri_Q_offset, 0, pCfg->SFM_cfg.L2U_pri_Q_size*sizeof(GM_RSSP1_SFM_L2U_pri_struct));


#ifndef CFM_Part
	pCfg->SFM_cfg.U2L_pri_Q_size_CFM    = pCfg->SFM_cfg.U2L_pri_Q_size;
	pCfg->SFM_cfg.U2L_pri_Q_offset_CFM  = (GM_RSSP1_UINT32)GM_RSSP1_malloc(valueInt * sizeof(GM_RSSP1_CFM_U2L_pri_struct));
	if (NULL == pCfg->SFM_cfg.U2L_pri_Q_offset_CFM)
	{
		return GM_RSSP1_FALSE;
	}
	GM_RSSP1_memset((void*)pCfg->SFM_cfg.U2L_pri_Q_offset_CFM, 0, valueInt*sizeof(GM_RSSP1_CFM_U2L_pri_struct));

	pCfg->SFM_cfg.L2U_pri_Q_size_CFM    = pCfg->SFM_cfg.L2U_pri_Q_size;
	pCfg->SFM_cfg.L2U_pri_Q_offset_CFM  = (GM_RSSP1_UINT32)GM_RSSP1_malloc(valueInt * sizeof(GM_RSSP1_CFM_L2U_pri_struct));

	if (NULL == pCfg->SFM_cfg.L2U_pri_Q_offset_CFM)
	{
		return GM_RSSP1_FALSE;
	}
	GM_RSSP1_memset((void*)pCfg->SFM_cfg.L2U_pri_Q_offset_CFM, 0, valueInt * sizeof(GM_RSSP1_CFM_L2U_pri_struct));
#endif

	*r_ptr = pFileContent;
	return GM_RSSP1_TRUE;
}

GM_RSSP1_BOOL GM_RSSP1_Init_Comm_Connection_SFM_Addr(GM_RSSP1_comm_cfg_struct *pCfg , GM_RSSP1_UINT8 *pFileContent, GM_RSSP1_UINT8 **r_ptr)
{
	GM_RSSP1_BOOL rt = GM_RSSP1_FALSE;
	GM_RSSP1_UINT32 valueInt = 0U;
	GM_RSSP1_INT32 conn_index = 0;
	GM_RSSP1_INT32 chn_index = 0;
	GM_RSSP1_INT32 fsfb_chn_index = 0;
	GM_RSSP1_BOOL isbig = GM_RSSP1_Code_Is_Big_Endian();
	GM_RSSP1_UINT8 i =0U;
	GM_RSSP1_SFM_connection_cfg_struct *pSFM_conn_cfg = NULL;

	if ((NULL == pCfg) || (NULL == pFileContent))
	{
		return GM_RSSP1_FALSE;
	}

	/* 各个连接通道配置 */
	for (conn_index = 0; conn_index < pCfg->SFM_cfg.connection_nums; ++conn_index)
	{
		pSFM_conn_cfg = &(pCfg->SFM_cfg.connection_cfg[conn_index]);

		valueInt = *(GM_RSSP1_UINT32*)pFileContent;
		pFileContent += sizeof(GM_RSSP1_UINT32);
		pSFM_conn_cfg->fsfb_comm_cycle = (GM_RSSP1_UINT32)valueInt;

		valueInt = *(GM_RSSP1_UINT16*)pFileContent;
		pFileContent += sizeof(GM_RSSP1_UINT16);
		pSFM_conn_cfg->dest_addr = (GM_RSSP1_UINT16)valueInt;

		/* init FSFB param */
		valueInt = *(GM_RSSP1_UINT16*)pFileContent;
		pFileContent += sizeof(GM_RSSP1_UINT16);
		pSFM_conn_cfg->deltaTime = (GM_RSSP1_UINT16)valueInt;
		
		valueInt = *(GM_RSSP1_UINT16*)pFileContent;
		pFileContent += sizeof(GM_RSSP1_UINT16);
		pSFM_conn_cfg->DelayTime = (GM_RSSP1_UINT16)valueInt;

		valueInt = *(GM_RSSP1_UINT16*)pFileContent;
		pFileContent += sizeof(GM_RSSP1_UINT16);
		pSFM_conn_cfg->lifeTime = (GM_RSSP1_UINT16)valueInt;

		valueInt = *(GM_RSSP1_UINT16*)pFileContent;
		pFileContent += sizeof(GM_RSSP1_UINT16);
		pSFM_conn_cfg->torlerate_cycle_cfg = (GM_RSSP1_UINT16)valueInt;

		valueInt = *(GM_RSSP1_UINT8*)pFileContent;
		pFileContent += sizeof(GM_RSSP1_UINT8);
		pSFM_conn_cfg->num_data_ver = (GM_RSSP1_UINT8)valueInt;

		for (i=0; i<GM_RSSP1_CHECK_CHN_NUM; i++)
		{
			valueInt = *(GM_RSSP1_UINT32*)pFileContent;
			pFileContent += sizeof(GM_RSSP1_UINT32);
			pSFM_conn_cfg->fsfb_chn_cfg[i].remote_sid = valueInt;

			valueInt = *(GM_RSSP1_UINT32*)pFileContent;
			pFileContent += sizeof(GM_RSSP1_UINT32);
			pSFM_conn_cfg->fsfb_chn_cfg[i].remote_sinit = valueInt;

			valueInt = *(GM_RSSP1_UINT32*)pFileContent;
			pFileContent += sizeof(GM_RSSP1_UINT32);
			pSFM_conn_cfg->fsfb_chn_cfg[i].remote_dataVer = valueInt;
		}

		/* 对等设备各个UDP通道均使用独立的FSFB。此时UDP通道数必须为1 */
		valueInt = *(GM_RSSP1_BOOL*)pFileContent;
		pFileContent += sizeof(GM_RSSP1_BOOL);
		if (isbig == GM_RSSP1_FALSE)
		{
			valueInt = GM_RSSP1_Code_Big_Little_Endian32(valueInt);
		}
		pSFM_conn_cfg->b_enable_FSFB_on_chn = (GM_RSSP1_BOOL)valueInt;

		if (GM_RSSP1_TRUE == pSFM_conn_cfg->b_enable_FSFB_on_chn)
		{
			/* init UDP chn apply FSFB ID */
			valueInt = *(GM_RSSP1_UINT32*)pFileContent;
			pFileContent += sizeof(GM_RSSP1_UINT32);
			pSFM_conn_cfg->Chn_apply_FSFB_ID = (GM_RSSP1_UINT32)valueInt;

			/* init msg queue in the SFM connection */
			valueInt = *(GM_RSSP1_UINT32*)pFileContent;
			pFileContent += sizeof(GM_RSSP1_UINT32);
			if (isbig == GM_RSSP1_FALSE)
			{
				valueInt = GM_RSSP1_Code_Big_Little_Endian32(valueInt);
			}
			pSFM_conn_cfg->L2U_dat_Q_size = valueInt;
			pSFM_conn_cfg->L2U_dat_Q_offset = (GM_RSSP1_UINT32)GM_RSSP1_malloc( valueInt*sizeof(GM_RSSP1_SFM_L2U_pri_struct));

			if (0 == pSFM_conn_cfg->L2U_dat_Q_offset)
			{
				return GM_RSSP1_FALSE;
			}
			GM_RSSP1_memset((void*)pSFM_conn_cfg->L2U_dat_Q_offset, 0, valueInt*sizeof(GM_RSSP1_SFM_L2U_pri_struct));
		}
		else
		{
			pFileContent += sizeof(GM_RSSP1_UINT32)*2;
		}

		if (isbig == GM_RSSP1_FALSE)
		{
			for (conn_index = 0; conn_index < pCfg->SFM_cfg.connection_nums; ++conn_index)
			{
				pSFM_conn_cfg = &(pCfg->SFM_cfg.connection_cfg[conn_index]);

				pSFM_conn_cfg->fsfb_comm_cycle = GM_RSSP1_Code_Big_Little_Endian32(pSFM_conn_cfg->fsfb_comm_cycle);
				pSFM_conn_cfg->dest_addr = GM_RSSP1_Code_Big_Little_Endian16(pSFM_conn_cfg->dest_addr);
				pSFM_conn_cfg->deltaTime = GM_RSSP1_Code_Big_Little_Endian16(pSFM_conn_cfg->deltaTime);
				pSFM_conn_cfg->DelayTime = GM_RSSP1_Code_Big_Little_Endian16(pSFM_conn_cfg->DelayTime);
				pSFM_conn_cfg->lifeTime = GM_RSSP1_Code_Big_Little_Endian16(pSFM_conn_cfg->lifeTime);
				pSFM_conn_cfg->torlerate_cycle_cfg = GM_RSSP1_Code_Big_Little_Endian16(pSFM_conn_cfg->torlerate_cycle_cfg);
				for (i=0; i<2; ++i)
				{
					pSFM_conn_cfg->fsfb_chn_cfg[i].remote_sid = GM_RSSP1_Code_Big_Little_Endian32(pSFM_conn_cfg->fsfb_chn_cfg[i].remote_sid);
					pSFM_conn_cfg->fsfb_chn_cfg[i].remote_sinit = GM_RSSP1_Code_Big_Little_Endian32(pSFM_conn_cfg->fsfb_chn_cfg[i].remote_sinit);
					pSFM_conn_cfg->fsfb_chn_cfg[i].remote_dataVer = GM_RSSP1_Code_Big_Little_Endian32(pSFM_conn_cfg->fsfb_chn_cfg[i].remote_dataVer);
				}
				if (GM_RSSP1_TRUE == pSFM_conn_cfg->b_enable_FSFB_on_chn)
				{
					pSFM_conn_cfg->Chn_apply_FSFB_ID = GM_RSSP1_Code_Big_Little_Endian32(pSFM_conn_cfg->Chn_apply_FSFB_ID);
				}
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

GM_RSSP1_BOOL GM_RSSP1_SFM_Init_Addr(GM_RSSP1_UINT8 *addr)
{
	GM_RSSP1_comm_cfg_struct fsfb_cfg = {0};
	GM_RSSP1_SFM_object_struct *pSFM = NULL;

	pSFM = GM_RSSP1_Get_SFM_Object();

	if ((NULL == addr) || (NULL == pSFM))
	{
		return GM_RSSP1_FALSE;
	}

	GM_RSSP1_memset(pSFM, 0, sizeof(GM_RSSP1_SFM_object_struct));

	if (GM_RSSP1_TRUE == GM_RSSP1_Init_Comm_Cfg_SFM_Addr(addr, &fsfb_cfg))
	{
		if (GM_RSSP1_TRUE == GM_RSSP1_SFM_Init_Lib(&fsfb_cfg , pSFM))
		{
			return GM_RSSP1_TRUE;
		}
	}

	return GM_RSSP1_FALSE;
}

#endif
