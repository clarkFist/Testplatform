
#include "GM_RSSP1_CFM_Init.h"
#include "GM_RSSP1_CFM_Interface.h"

#ifdef CFM_Part

extern GM_RSSP1_UINT32 g_valid_chn;
extern RSSP1_MQ_Inter_struct g_CFM_MQ;
extern RSSP1_MQ_LINK_struct g_Link_MQ[GM_RSSP1_MAX_SAFETY_CONNECTION_NUM];
extern GM_RSSP1_Local_Con_struct g_loc_info;

GM_RSSP1_BOOL GM_RSSP1_Init_Comm_Cfg_CFM_Addr(GM_RSSP1_UINT8 *pFileContent, GM_RSSP1_comm_cfg_struct *pCfg)
{
    GM_RSSP1_INT32 file_len = -1;
    GM_RSSP1_BOOL rt = GM_RSSP1_FALSE;
	GM_RSSP1_UINT8 **rp_file = &pFileContent;
	GM_RSSP1_UINT32 fsize = 0U;
	GM_RSSP1_BOOL isbig = GM_RSSP1_Code_Is_Big_Endian();

    if ((NULL == pFileContent) || (NULL == pCfg))
    {
        return GM_RSSP1_FALSE;
    }

    /* 顺序不可变动。初始化各连接时需要从SFM cfg中获取本地连接通道共享的一些配置 */
    if (GM_RSSP1_FALSE == GM_RSSP1_Init_Comm_Global_CFM_Addr(pCfg, pFileContent, rp_file))
    {
        return GM_RSSP1_FALSE;
    }

    if (GM_RSSP1_FALSE == GM_RSSP1_Init_Comm_Connection_CFM_Addr(pCfg, pFileContent, rp_file))
    {
        return GM_RSSP1_FALSE;
    }

    return GM_RSSP1_TRUE;
}

GM_RSSP1_BOOL GM_RSSP1_Init_Comm_Global_CFM_Addr(GM_RSSP1_comm_cfg_struct *pCfg , GM_RSSP1_UINT8 *pFileContent, GM_RSSP1_UINT8 **r_ptr)
{
    GM_RSSP1_BOOL rt = GM_RSSP1_FALSE;
    GM_RSSP1_UINT32 valueInt = 0U;
	GM_RSSP1_BOOL isbig = GM_RSSP1_Code_Is_Big_Endian();

    if ((NULL == pCfg) || (NULL == pFileContent))
    {
		GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_CFM_Param_Point_Error, 0, 0, 0, 0, 0, 0);
        return GM_RSSP1_FALSE;
    }

    /* RSSP1_GLOBAL 全局配置 */
	valueInt = *(GM_RSSP1_UINT16*)pFileContent;
	pFileContent += sizeof(GM_RSSP1_UINT16);
	if (isbig == GM_RSSP1_FALSE)
	{
		valueInt = GM_RSSP1_Code_Big_Little_Endian16((GM_RSSP1_UINT16)valueInt);
	}
    if (valueInt > GM_RSSP1_MAX_SAFETY_CONNECTION_NUM)
    {
        return GM_RSSP1_FALSE;
    }
    else
    {
        pCfg->CFM_cfg.connection_nums = (GM_RSSP1_UINT16)valueInt;
		g_valid_chn = valueInt;
    }

	valueInt = *(GM_RSSP1_UINT16*)pFileContent;
	pFileContent += sizeof(GM_RSSP1_UINT16);
	pCfg->CFM_cfg.source_addr = (GM_RSSP1_UINT16)valueInt;

	valueInt = *(GM_RSSP1_UINT32*)pFileContent;
	pFileContent += sizeof(GM_RSSP1_UINT32);
	if (isbig == GM_RSSP1_FALSE)
	{
		valueInt = GM_RSSP1_Code_Big_Little_Endian32(valueInt);
	}
	pCfg->CFM_cfg.U2L_pri_Q_size    = (GM_RSSP1_UINT32)valueInt;
	pCfg->CFM_cfg.U2L_pri_Q_offset  = (GM_RSSP1_UINT32)GM_RSSP1_malloc(valueInt * sizeof(GM_RSSP1_CFM_U2L_pri_struct));
    if (0 == pCfg->CFM_cfg.U2L_pri_Q_offset)
    {
		GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_CFM_U2L_Q_Mem_Fail, 0, 0, 0, 0, 0, 0);
        return GM_RSSP1_FALSE;
    }
    GM_RSSP1_memset((void*)pCfg->CFM_cfg.U2L_pri_Q_offset, 0, valueInt*sizeof(GM_RSSP1_CFM_U2L_pri_struct));

	valueInt = *(GM_RSSP1_UINT32*)pFileContent;
	pFileContent += sizeof(GM_RSSP1_UINT32);
	if (isbig == GM_RSSP1_FALSE)
	{
		valueInt = GM_RSSP1_Code_Big_Little_Endian32(valueInt);
	}
    pCfg->CFM_cfg.L2U_pri_Q_size = (GM_RSSP1_UINT32)valueInt;
    pCfg->CFM_cfg.L2U_pri_Q_offset = (GM_RSSP1_UINT32)GM_RSSP1_malloc(valueInt * sizeof(GM_RSSP1_CFM_L2U_pri_struct));

    if (NULL == pCfg->CFM_cfg.L2U_pri_Q_offset)
    {
		GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_CFM_L2U_Q_Mem_Fail, 0, 0, 0, 0, 0, 0);
        return GM_RSSP1_FALSE;
    }
    GM_RSSP1_memset((void*)pCfg->CFM_cfg.L2U_pri_Q_offset, 0, valueInt * sizeof(GM_RSSP1_CFM_L2U_pri_struct));
	
	if (isbig == GM_RSSP1_FALSE)
	{
		pCfg->CFM_cfg.source_addr = GM_RSSP1_Code_Big_Little_Endian16(pCfg->CFM_cfg.source_addr);
	}

	valueInt = *(GM_RSSP1_UINT32*)pFileContent;
	pFileContent += sizeof(GM_RSSP1_UINT32);
	if (isbig == GM_RSSP1_FALSE)
	{
		valueInt = GM_RSSP1_Code_Big_Little_Endian32(valueInt);
	}
	pCfg->CFM_cfg.all0_size = (GM_RSSP1_UINT32)valueInt;

	*r_ptr = pFileContent;
    return GM_RSSP1_TRUE;
}

GM_RSSP1_BOOL GM_RSSP1_Init_Comm_Connection_CFM_Addr(GM_RSSP1_comm_cfg_struct *pCfg , GM_RSSP1_UINT8 *pFileContent,GM_RSSP1_UINT8 **r_ptr)
{
    GM_RSSP1_BOOL rt = GM_RSSP1_FALSE;
    GM_RSSP1_UINT32 valueInt = -1;
    GM_RSSP1_INT32 conn_index = 0;
    GM_RSSP1_INT32 chn_index = 0;
    GM_RSSP1_INT32 fsfb_chn_index = 0;
	GM_RSSP1_BOOL isbig = GM_RSSP1_Code_Is_Big_Endian();
    GM_RSSP1_CFM_connection_cfg_struct *pCFM_conn_cfg = NULL;

    if ((NULL == pCfg) || (NULL == pFileContent))
    {
		GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_CFM_Param_Point_Error, pCfg, pFileContent, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
    }
    g_loc_info.connection_num = pCfg->CFM_cfg.connection_nums;

    /* 各个连接通道配置 */
    for (conn_index = 0; conn_index < pCfg->CFM_cfg.connection_nums; ++conn_index)
    {
        pCFM_conn_cfg = &(pCfg->CFM_cfg.connection_cfg[conn_index]);

		valueInt = *(GM_RSSP1_UINT16*)pFileContent;
		pFileContent += sizeof(GM_RSSP1_UINT16);
        pCFM_conn_cfg->dest_addr = (GM_RSSP1_UINT16)valueInt;

		valueInt = *(GM_RSSP1_BOOL*)pFileContent;
		pFileContent += sizeof(GM_RSSP1_BOOL);
        pCFM_conn_cfg->b_enable_CRSCD_pack = (GM_RSSP1_BOOL)valueInt;

        /* init com type */
		valueInt = *(GM_RSSP1_UINT16*)pFileContent;
		pFileContent += sizeof(GM_RSSP1_UINT16);
        if(0 == valueInt)
        {
            pCFM_conn_cfg->com_type = GM_RSSP1_CFM_COMM_TYPE_UDP;
        }
        else if(1 == valueInt)
        {
            pCFM_conn_cfg->com_type = GM_RSSP1_CFM_COMM_TYPE_SCOM;
        }
        else
        {
            pCFM_conn_cfg->com_type = GM_RSSP1_CFM_COMM_TYPE_UNKNOW;
        }

        if(GM_RSSP1_CFM_COMM_TYPE_UDP == pCFM_conn_cfg->com_type)
        {
            /* init UDP channel param */
			valueInt = *(GM_RSSP1_UINT16*)pFileContent;
			pFileContent += sizeof(GM_RSSP1_UINT16);
			if (isbig == GM_RSSP1_FALSE)
			{
				valueInt = GM_RSSP1_Code_Big_Little_Endian16((GM_RSSP1_UINT16)valueInt);
			}
            if (valueInt > GM_RSSP1_CFM_MAX_CHANNEL_NUM)
            {
				GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_CFM_UDP_Num_Fail, 0, 0, 0, 0, 0, 0);
                return GM_RSSP1_FALSE;
            }
            else
            {
				pCFM_conn_cfg->chn_num = (GM_RSSP1_UINT16)valueInt;
			}
        }

/* 
		if ((GM_RSSP1_TRUE == pCFM_conn_cfg->b_enable_FSFB_on_chn) && (1 != pCFM_conn_cfg->chn_num))
        {
			GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_CFM_Confilct_Fail, 0, 0, 0, 0, 0, 0);
            GM_RSSP1_Log_Msg(0,"FSFB init fail! UDP channel = %d,must = 1.check 'enable_UDP_chn_FSFB' option.\n", pCFM_conn_cfg->chn_num, 0, 0, 0, 0, 0);
            return GM_RSSP1_FALSE;
        }
*/
		g_loc_info.connection[conn_index].chn_num = pCFM_conn_cfg->chn_num;
		/* init UDP msg queues */
		/*Lhh UDP_channel_num */
		for (chn_index = 0 ; chn_index < pCFM_conn_cfg->chn_num ; ++chn_index)
		{
			valueInt = *(GM_RSSP1_UINT32*)pFileContent;
			pFileContent += sizeof(GM_RSSP1_UINT32);
			pCFM_conn_cfg->UDP_chn_cfg[chn_index].loc_ip = valueInt;

			valueInt = *(GM_RSSP1_UINT32*)pFileContent;
			pFileContent += sizeof(GM_RSSP1_UINT32);
			pCFM_conn_cfg->UDP_chn_cfg[chn_index].loc_port= valueInt;

			valueInt = *(GM_RSSP1_UINT32*)pFileContent;
			pFileContent += sizeof(GM_RSSP1_UINT32);
			pCFM_conn_cfg->UDP_chn_cfg[chn_index].rem_ip = valueInt;

			valueInt = *(GM_RSSP1_UINT32*)pFileContent;
			pFileContent += sizeof(GM_RSSP1_UINT32);
			pCFM_conn_cfg->UDP_chn_cfg[chn_index].rem_port= valueInt;
			g_loc_info.connection[conn_index].locl_addr[chn_index].loc_ip = pCFM_conn_cfg->UDP_chn_cfg[chn_index].loc_ip;
			g_loc_info.connection[conn_index].locl_addr[chn_index].loc_port = pCFM_conn_cfg->UDP_chn_cfg[chn_index].loc_port;
		}

		valueInt = *(GM_RSSP1_UINT32*)pFileContent;
		pFileContent += sizeof(GM_RSSP1_UINT32);
		if (isbig == GM_RSSP1_FALSE)
		{
			valueInt = GM_RSSP1_Code_Big_Little_Endian32(valueInt);
		}
		for (chn_index = 0 ; chn_index < pCFM_conn_cfg->chn_num ; ++chn_index)
		{
			pCFM_conn_cfg->UDP_chn_cfg[chn_index].recv_Q_size = valueInt;
			pCFM_conn_cfg->UDP_chn_cfg[chn_index].recv_Q_offset = (GM_RSSP1_UINT32)GM_RSSP1_malloc(valueInt * sizeof(GM_RSSP1_com_pri_struct));

			if (NULL == pCFM_conn_cfg->UDP_chn_cfg[chn_index].recv_Q_offset)
			{
				GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_CFM_Rcv_Offset_Fail, conn_index, chn_index, 0, 0, 0, 0);
				return GM_RSSP1_FALSE;
			}
			GM_RSSP1_memset((void*)pCFM_conn_cfg->UDP_chn_cfg[chn_index].recv_Q_offset, 0, valueInt * sizeof(GM_RSSP1_com_pri_struct));
		}


		valueInt = *(GM_RSSP1_UINT32*)pFileContent;
		pFileContent += sizeof(GM_RSSP1_UINT32);
		if (isbig == GM_RSSP1_FALSE)
		{
			valueInt = GM_RSSP1_Code_Big_Little_Endian32(valueInt);
		}
		for (chn_index = 0 ; chn_index < pCFM_conn_cfg->chn_num ; ++chn_index)
		{
			pCFM_conn_cfg->UDP_chn_cfg[chn_index].send_Q_size   = valueInt;
			pCFM_conn_cfg->UDP_chn_cfg[chn_index].send_Q_offset = (GM_RSSP1_UINT32)GM_RSSP1_malloc(valueInt * sizeof(GM_RSSP1_com_pri_struct));

			if (NULL == pCFM_conn_cfg->UDP_chn_cfg[chn_index].send_Q_offset)
			{
				GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_CFM_Snd_Offset_Fail, conn_index, chn_index, 0, 0, 0, 0);
				return GM_RSSP1_FALSE;
			}
			GM_RSSP1_memset((void*)pCFM_conn_cfg->UDP_chn_cfg[chn_index].send_Q_offset, 0, valueInt * sizeof(GM_RSSP1_com_pri_struct));
		}
	}

	if (isbig == GM_RSSP1_FALSE)
	{
		for (conn_index = 0; conn_index < pCfg->CFM_cfg.connection_nums; ++conn_index)
		{
			pCFM_conn_cfg = &(pCfg->CFM_cfg.connection_cfg[conn_index]);
			pCFM_conn_cfg->dest_addr = GM_RSSP1_Code_Big_Little_Endian16(pCFM_conn_cfg->dest_addr);
			pCFM_conn_cfg->b_enable_CRSCD_pack = GM_RSSP1_Code_Big_Little_Endian32(pCFM_conn_cfg->b_enable_CRSCD_pack);

			/* init UDP msg queues */
			for (chn_index = 0 ; chn_index < pCFM_conn_cfg->chn_num ; ++chn_index)
			{
				pCFM_conn_cfg->UDP_chn_cfg[chn_index].loc_ip = GM_RSSP1_Code_Big_Little_Endian32(pCFM_conn_cfg->UDP_chn_cfg[chn_index].loc_ip);
				pCFM_conn_cfg->UDP_chn_cfg[chn_index].loc_port= GM_RSSP1_Code_Big_Little_Endian32(pCFM_conn_cfg->UDP_chn_cfg[chn_index].loc_port);
				pCFM_conn_cfg->UDP_chn_cfg[chn_index].rem_ip = GM_RSSP1_Code_Big_Little_Endian32(pCFM_conn_cfg->UDP_chn_cfg[chn_index].rem_ip);
				pCFM_conn_cfg->UDP_chn_cfg[chn_index].rem_port= GM_RSSP1_Code_Big_Little_Endian32(pCFM_conn_cfg->UDP_chn_cfg[chn_index].rem_port);
				g_loc_info.connection[conn_index].locl_addr[chn_index].loc_ip = GM_RSSP1_Code_Big_Little_Endian32(g_loc_info.connection[conn_index].locl_addr[chn_index].loc_ip);
				g_loc_info.connection[conn_index].locl_addr[chn_index].loc_port= GM_RSSP1_Code_Big_Little_Endian32(g_loc_info.connection[conn_index].locl_addr[chn_index].loc_port);
			}
		}
	}
    return GM_RSSP1_TRUE;
}

GM_RSSP1_BOOL GM_RSSP1_CFM_Init_Addr(GM_RSSP1_UINT8 *addr)
{
	GM_RSSP1_CFM_object_struct *pCFM = NULL;
	GM_RSSP1_comm_cfg_struct fsfb_cfg = {0};

	pCFM = GM_RSSP1_Get_CFM_Object();

	if ((NULL == addr)||(pCFM == NULL))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_CFM_Param_Point_Error, addr, pCFM, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	GM_RSSP1_memset(pCFM, 0, sizeof(GM_RSSP1_CFM_object_struct));

	if (GM_RSSP1_TRUE == GM_RSSP1_Init_Comm_Cfg_CFM_Addr(addr, &fsfb_cfg))
	{
		if (GM_RSSP1_TRUE == GM_RSSP1_CFM_Init_Lib(&fsfb_cfg , pCFM))
		{
			return GM_RSSP1_TRUE;
		}
	}
	return GM_RSSP1_FALSE;
}
#endif
