
#include "GM_RSSP1_CFM_Init.h"
#include "GM_RSSP1_CFM_Interface.h"

#ifdef CFM_Part

extern GM_RSSP1_UINT32 g_valid_chn;

extern RSSP1_MQ_Inter_struct g_CFM_MQ;
extern RSSP1_MQ_LINK_struct g_Link_MQ[GM_RSSP1_MAX_SAFETY_CONNECTION_NUM];
#ifdef	CFM_Stanby_Answer
extern GM_RSSP1_Msg_Queue_struct g_CFM_Debug_MQ;
#endif
extern GM_RSSP1_Local_Con_struct g_loc_info;

GM_RSSP1_BOOL GM_RSSP1_Init_Comm_Cfg_CFM(GM_RSSP1_INT8 *pIniFileContent , GM_RSSP1_comm_cfg_struct *pCfg)
{
    if ((NULL == pIniFileContent) || (NULL == pCfg))
    {
        return GM_RSSP1_FALSE;
    }

    /* 顺序不可变动。初始化各连接时需要从SFM cfg中获取本地连接通道共享的一些配置 */
    if (GM_RSSP1_FALSE == GM_RSSP1_Init_Comm_Global_CFM(pCfg, pIniFileContent))
    {
        return GM_RSSP1_FALSE;
    }

    if (GM_RSSP1_FALSE == GM_RSSP1_Init_Comm_Connection_CFM(pCfg, pIniFileContent))
    {
        return GM_RSSP1_FALSE;
    }

    return GM_RSSP1_TRUE;
}

GM_RSSP1_BOOL GM_RSSP1_Init_Comm_Global_CFM(GM_RSSP1_comm_cfg_struct *pCfg , GM_RSSP1_INT8 *pIniFileContent)
{
    GM_RSSP1_BOOL rt = GM_RSSP1_FALSE;
    GM_RSSP1_INT32 valueInt = -1;
    GM_RSSP1_INT8 valueString[256] = {0};

    if ((NULL == pCfg) || (NULL == pIniFileContent))
    {
        GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_CFM_Param_Point_Error, 0, 0, 0, 0, 0, 0);
        return GM_RSSP1_FALSE;
    }

    /* RSSP1_GLOBAL 全局配置 */
    rt = GM_RSSP1_Ini_File_Read_Int("RSSP1_GLOBAL", "connection_num", &valueInt, pIniFileContent);

    if ((GM_RSSP1_FALSE == rt) || (valueInt < 0) || (valueInt > GM_RSSP1_MAX_SAFETY_CONNECTION_NUM))
    {
		GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_CFM_Connect_Num_Fail, 0, 0, 0, 0, 0, 0);
        return GM_RSSP1_FALSE;
    }
    else
    {
        pCfg->CFM_cfg.connection_nums = (GM_RSSP1_UINT16)valueInt;
        g_valid_chn = (GM_RSSP1_UINT32)valueInt;
    }


    rt = GM_RSSP1_Ini_File_Read_String("RSSP1_GLOBAL", "source_addr", valueString , sizeof(valueString), pIniFileContent);

    if (GM_RSSP1_FALSE == rt)
    {
        GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_CFM_Src_Addr_Fail, 0, 0, 0, 0, 0, 0);
        return GM_RSSP1_FALSE;
    }
    else
    {
        pCfg->CFM_cfg.source_addr = (GM_RSSP1_UINT16)(strtoul(valueString , NULL , 16));
    }

    rt = GM_RSSP1_Ini_File_Read_Int("RSSP1_GLOBAL", "CFM_U2L_Q_Size_per_connection", &valueInt, pIniFileContent);

    if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
    {
        GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_CFM_U2L_Q_Fail, 0, 0, 0, 0, 0, 0);
        return GM_RSSP1_FALSE;
    }
    else
    {
        pCfg->CFM_cfg.U2L_pri_Q_size    = (GM_RSSP1_UINT32)valueInt;
        pCfg->CFM_cfg.U2L_pri_Q_offset  = (GM_RSSP1_UINT32)GM_RSSP1_malloc(valueInt * sizeof(GM_RSSP1_CFM_U2L_pri_struct));

        if (0 == pCfg->CFM_cfg.U2L_pri_Q_offset)
        {
            GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_CFM_U2L_Q_Mem_Fail, 0, 0, 0, 0, 0, 0);
            return GM_RSSP1_FALSE;
        }
        GM_RSSP1_memset((void*)pCfg->CFM_cfg.U2L_pri_Q_offset, 0, valueInt*sizeof(GM_RSSP1_CFM_U2L_pri_struct));
    }

    rt = GM_RSSP1_Ini_File_Read_Int("RSSP1_GLOBAL", "CFM_L2U_Q_Size_per_connection", &valueInt, pIniFileContent);

    if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
    {
        GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_CFM_L2U_Q_Fail, 0, 0, 0, 0, 0, 0);
        return GM_RSSP1_FALSE;
    }
    else
    {
        pCfg->CFM_cfg.L2U_pri_Q_size    = (GM_RSSP1_UINT32)valueInt;
        pCfg->CFM_cfg.L2U_pri_Q_offset  = (GM_RSSP1_UINT32)GM_RSSP1_malloc(valueInt * sizeof(GM_RSSP1_CFM_L2U_pri_struct));

        if (NULL == pCfg->CFM_cfg.L2U_pri_Q_offset)
        {
            GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Global_CFM_L2U_Q_Mem_Fail, 0, 0, 0, 0, 0, 0);
            return GM_RSSP1_FALSE;
        }
        GM_RSSP1_memset((void*)pCfg->CFM_cfg.L2U_pri_Q_offset, 0, valueInt * sizeof(GM_RSSP1_CFM_L2U_pri_struct));
    }

    /* start:CR:GM00002526, GM162, fulintai,2015-06-09, add configuration for all 0 message size in standby's CFM*/
    rt = GM_RSSP1_Ini_File_Read_Int("RSSP1_GLOBAL", "UsrData_All0_Size", &valueInt, pIniFileContent);

    if ((GM_RSSP1_FALSE == rt) || (valueInt <= 0) || (valueInt > (GM_RSSP1_MAX_USER_DAT_LEN - 22)))
    {
        pCfg->CFM_cfg.all0_size = 0xFFFFFFFF;
    }
    else
    {
        pCfg->CFM_cfg.all0_size = (GM_RSSP1_UINT32)valueInt;
    }
    /* end:CR:GM00002526*/
    return GM_RSSP1_TRUE;
}

GM_RSSP1_BOOL GM_RSSP1_Init_Comm_Connection_CFM(GM_RSSP1_comm_cfg_struct *pCfg , GM_RSSP1_INT8 *pIniFileContent)
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
    GM_RSSP1_CFM_connection_cfg_struct *pCFM_conn_cfg = NULL;

    if ((NULL == pCfg) || (NULL == pIniFileContent))
    {
        GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_CFM_Param_Point_Error, pCfg, pIniFileContent, 0, 0, 0, 0);
        return GM_RSSP1_FALSE;
    }

    g_loc_info.connection_num = pCfg->CFM_cfg.connection_nums;
    /* 各个连接通道配置 */
    key_len = sizeof("CON_") - 1;

    GM_RSSP1_memcpy(section_key, "CON_", key_len + 1);

    for (conn_index = 0; conn_index < pCfg->CFM_cfg.connection_nums; ++conn_index)
    {
        pCFM_conn_cfg = &(pCfg->CFM_cfg.connection_cfg[conn_index]);

        GM_RSSP1_memset(section_key + key_len, 0, sizeof(section_key) - key_len);
        sprintf(section_key + key_len, "%d", conn_index);

        rt = GM_RSSP1_Ini_File_Read_String(section_key, "dest_addr", valueString , sizeof(valueString), pIniFileContent);

        if (GM_RSSP1_FALSE == rt)
        {
            GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_CFM_Des_Addr_Fail, 0, 0, 0, 0, 0, 0);
            return GM_RSSP1_FALSE;
        }
        else
        {
            pCFM_conn_cfg->dest_addr = (GM_RSSP1_UINT16)(strtoul(valueString , NULL , 16));
        }

        rt = GM_RSSP1_Ini_File_Read_BOOL(section_key , "enable_CRSCD_pack" , &valueBool, pIniFileContent);

        if (GM_RSSP1_FALSE == rt)
        {
            GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_CFM_Enable_Pack_Fail, 0, 0, 0, 0, 0, 0);
            return GM_RSSP1_FALSE;
        }
        else
        {
            pCFM_conn_cfg->b_enable_CRSCD_pack = valueBool;
        }

		/* 对等设备各个UDP通道均使用独立的FSFB。此时UDP通道数必须为1 
		rt = GM_RSSP1_Ini_File_Read_BOOL(section_key , "enable_UDP_chn_FSFB" , &valueBool, pIniFileContent);

		if (GM_RSSP1_FALSE == rt)
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_CFM_Enable_UDP_Fail, 0, 0, 0, 0, 0, 0);
			return GM_RSSP1_FALSE;
		}
		else
		{
			pCFM_conn_cfg->b_enable_FSFB_on_chn = valueBool;
		}*/
        /* init com type */
        rt = GM_RSSP1_Ini_File_Read_Int(section_key, "com_type", &valueInt, pIniFileContent);

        if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
        {
            GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_CFM_Com_Type_Fail, 0, 0, 0, 0, 0, 0);
            return GM_RSSP1_FALSE;
        }
        else
        {
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
        }

        if(GM_RSSP1_CFM_COMM_TYPE_UDP == pCFM_conn_cfg->com_type)
        {
            /* init UDP channel param */
            rt = GM_RSSP1_Ini_File_Read_Int(section_key, "UDP_channel_num", &valueInt, pIniFileContent);

            if ((GM_RSSP1_FALSE == rt) || (valueInt < 0) || (valueInt > GM_RSSP1_CFM_MAX_CHANNEL_NUM))
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
		for (chn_index = 0 ; chn_index < pCFM_conn_cfg->chn_num ; ++chn_index)
		{

			sub_key_len = sizeof("local_ip_") - 1;
			GM_RSSP1_memcpy(sub_section_key, "local_ip_", sub_key_len + 1);
			GM_RSSP1_memset(sub_section_key + sub_key_len, 0, 2);
			sprintf(sub_section_key + sub_key_len, "%d", chn_index);
			rt = GM_RSSP1_Ini_File_Read_String(section_key, sub_section_key, valueString, sizeof(valueString), pIniFileContent);

			if (GM_RSSP1_FALSE == rt)
			{
				GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_CFM_Loc_IP_Fail, 0, 0, 0, 0, 0, 0);
				return GM_RSSP1_FALSE;
			}
			else
			{
				pCFM_conn_cfg->UDP_chn_cfg[chn_index].loc_ip = inet_addr(valueString);
				g_loc_info.connection[conn_index].locl_addr[chn_index].loc_ip = pCFM_conn_cfg->UDP_chn_cfg[chn_index].loc_ip;
			}

			sub_key_len = sizeof("local_port_") - 1;

			GM_RSSP1_memcpy(sub_section_key, "local_port_", sub_key_len + 1);
			GM_RSSP1_memset(sub_section_key + sub_key_len, 0, 2);
			sprintf(sub_section_key + sub_key_len, "%d", chn_index);
			rt = GM_RSSP1_Ini_File_Read_Int(section_key, sub_section_key, &valueInt, pIniFileContent);

			if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
			{
				GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_CFM_Loc_Port_Fail, 0, 0, 0, 0, 0, 0);
				return GM_RSSP1_FALSE;
			}
			else
			{
				pCFM_conn_cfg->UDP_chn_cfg[chn_index].loc_port= (GM_RSSP1_UINT32)valueInt;
				g_loc_info.connection[conn_index].locl_addr[chn_index].loc_port = pCFM_conn_cfg->UDP_chn_cfg[chn_index].loc_port;
			}

			sub_key_len = sizeof("remote_ip_") - 1;
			GM_RSSP1_memcpy(sub_section_key, "remote_ip_", sub_key_len + 1);
			GM_RSSP1_memset(sub_section_key + sub_key_len, 0, 2);
			sprintf(sub_section_key + sub_key_len, "%d", chn_index);
			rt = GM_RSSP1_Ini_File_Read_String(section_key, sub_section_key, valueString, sizeof(valueString), pIniFileContent);

			if (GM_RSSP1_FALSE == rt)
			{
				GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_CFM_Rem_IP_Fail, 0, 0, 0, 0, 0, 0);
				return GM_RSSP1_FALSE;
			}
			else
			{
				pCFM_conn_cfg->UDP_chn_cfg[chn_index].rem_ip = inet_addr(valueString);
			}

			sub_key_len = sizeof("remote_port_") - 1;

			GM_RSSP1_memcpy(sub_section_key, "remote_port_", sub_key_len + 1);
			GM_RSSP1_memset(sub_section_key + sub_key_len, 0, 2);
			sprintf(sub_section_key + sub_key_len, "%d", chn_index);
			rt = GM_RSSP1_Ini_File_Read_Int(section_key, sub_section_key, &valueInt, pIniFileContent);

			if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
			{
				GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_CFM_Rem_Port_Fail, 0, 0, 0, 0, 0, 0);
				return GM_RSSP1_FALSE;
			}
			else
			{
				pCFM_conn_cfg->UDP_chn_cfg[chn_index].rem_port= (GM_RSSP1_UINT32)valueInt;
			}
		}

		rt = GM_RSSP1_Ini_File_Read_Int(section_key, "UDP_recv_MQ_size", &valueInt, pIniFileContent);

		if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_CFM_Rcv_Size_Fail, 0, 0, 0, 0, 0, 0);
			return GM_RSSP1_FALSE;
		}
		else
		{
			for (chn_index = 0 ; chn_index < pCFM_conn_cfg->chn_num ; ++chn_index)
			{
				pCFM_conn_cfg->UDP_chn_cfg[chn_index].recv_Q_size = (GM_RSSP1_UINT32)valueInt;
				pCFM_conn_cfg->UDP_chn_cfg[chn_index].recv_Q_offset = (GM_RSSP1_UINT32)GM_RSSP1_malloc(valueInt * sizeof(GM_RSSP1_com_pri_struct));

				if (NULL == pCFM_conn_cfg->UDP_chn_cfg[chn_index].recv_Q_offset)
				{
					GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_CFM_Rcv_Offset_Fail, conn_index, chn_index, 0, 0, 0, 0);
					return GM_RSSP1_FALSE;
				}
				GM_RSSP1_memset((void*)pCFM_conn_cfg->UDP_chn_cfg[chn_index].recv_Q_offset, 0, valueInt * sizeof(GM_RSSP1_com_pri_struct));
			}
		}


		rt = GM_RSSP1_Ini_File_Read_Int(section_key, "UDP_send_MQ_size", &valueInt, pIniFileContent);

		if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_CFM_Snd_Size_Fail, 0, 0, 0, 0, 0, 0);
			return GM_RSSP1_FALSE;
		}
		else
		{
			for (chn_index = 0 ; chn_index < pCFM_conn_cfg->chn_num ; ++chn_index)
			{
				pCFM_conn_cfg->UDP_chn_cfg[chn_index].send_Q_size   = (GM_RSSP1_UINT32)valueInt;
				pCFM_conn_cfg->UDP_chn_cfg[chn_index].send_Q_offset = (GM_RSSP1_UINT32)GM_RSSP1_malloc(valueInt * sizeof(GM_RSSP1_com_pri_struct));

				if (NULL == pCFM_conn_cfg->UDP_chn_cfg[chn_index].send_Q_offset)
				{
					GM_RSSP1_Log_Usr(GM_RSSP1_Init_Comm_Connection_CFM_Snd_Offset_Fail, conn_index, chn_index, 0, 0, 0, 0);
					return GM_RSSP1_FALSE;
				}
				GM_RSSP1_memset((void*)pCFM_conn_cfg->UDP_chn_cfg[chn_index].send_Q_offset, 0, valueInt * sizeof(GM_RSSP1_com_pri_struct));
			}
		}
	}
    return GM_RSSP1_TRUE;
}

GM_RSSP1_BOOL GM_RSSP1_CFM_Init(GM_RSSP1_INT8 *file)
{
	GM_RSSP1_CFM_object_struct *pCFM = NULL;
	GM_RSSP1_comm_cfg_struct fsfb_cfg = {0};

	pCFM = GM_RSSP1_Get_CFM_Object();

	if ((NULL == file)||(pCFM == NULL))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Init_Param_Point_Error, file, pCFM, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	GM_RSSP1_memset(pCFM, 0, sizeof(GM_RSSP1_CFM_object_struct));

	if (GM_RSSP1_TRUE == GM_RSSP1_Init_Comm_Cfg_CFM(file, &fsfb_cfg))
	{
		if (GM_RSSP1_TRUE == GM_RSSP1_CFM_Init_Lib(&fsfb_cfg , pCFM))
		{
			return GM_RSSP1_TRUE;
		}
	}
	return GM_RSSP1_FALSE;
}

GM_RSSP1_BOOL GM_RSSP1_CFM_Init_Lib(GM_RSSP1_comm_cfg_struct *pcfg, GM_RSSP1_CFM_object_struct *pCFM)
{
	GM_RSSP1_CFM_connection_struct *pCFM_con = NULL;
	GM_RSSP1_CFM_connection_cfg_struct *pConn_cfg = NULL;
	GM_RSSP1_INT32 conn_index = 0;
	GM_RSSP1_INT32 chn_index = 0;
#ifdef	CFM_Stanby_Answer
	GM_RSSP1_UINT32 Debug_Q_size = 0U;
	GM_RSSP1_UINT32 Debug_Q_offset = 0U;
#endif
	if ((NULL == pcfg) || (NULL == pCFM))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Init_Lib_Param_Point_Error, pcfg, pCFM, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	pCFM->lib_state             = GM_RSSP1_Lib_State_Unknown;

	pCFM->connection_nums       = pcfg->CFM_cfg.connection_nums;

	/* start:CR:GM00002526, GM162, fulintai,2015-06-09, add configuration for all 0 message size in standby's CFM*/
	pCFM->all0_size       = pcfg->CFM_cfg.all0_size;
	/* end:CR:GM00002526 */
	/* init connections */

	for (conn_index = 0 ; conn_index < pCFM->connection_nums ; ++conn_index)
	{
		pConn_cfg               = &(pcfg->CFM_cfg.connection_cfg[conn_index]);
		pCFM_con               = &(pCFM->connection[conn_index]);

		pCFM_con->index        = (GM_RSSP1_UINT8)conn_index;
		pCFM_con->state        = GM_RSSP1_Layer_State_Free;
		pCFM_con->b_enable_CRSCD_pack  = pConn_cfg->b_enable_CRSCD_pack;
		pCFM_con->SaCEPID.source_addr  = pcfg->CFM_cfg.source_addr;
		pCFM_con->SaCEPID.dest_addr    = pConn_cfg->dest_addr;
		pCFM_con->com_type              = pConn_cfg->com_type;
		pCFM_con->chn_num          = pConn_cfg->chn_num;
		pCFM_con->chn_num          = pConn_cfg->chn_num;

		for (chn_index=0; chn_index<pCFM_con->chn_num; ++chn_index)
		{
			pCFM_con->com_chn[chn_index].loc_ip = pConn_cfg->UDP_chn_cfg[chn_index].loc_ip;
			pCFM_con->com_chn[chn_index].rem_ip = pConn_cfg->UDP_chn_cfg[chn_index].rem_ip;
			pCFM_con->com_chn[chn_index].loc_port = pConn_cfg->UDP_chn_cfg[chn_index].loc_port;
			pCFM_con->com_chn[chn_index].rem_port = pConn_cfg->UDP_chn_cfg[chn_index].rem_port;
			pCFM_con->com_chn[chn_index].b_connected = GM_RSSP1_FALSE;

			if (GM_RSSP1_FALSE == FSFB_Msg_Queue_Init(
				&(g_Link_MQ[conn_index].RCV_SND_MQ[chn_index].RCV_MQ),
				pConn_cfg->UDP_chn_cfg[chn_index].recv_Q_size ,
				pConn_cfg->UDP_chn_cfg[chn_index].recv_Q_offset ,
				FSFB_Q_TYPE_RCVD_PKT)
				)
			{
				GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Init_Lib_Link_MQ_Fail, conn_index, chn_index, 1, 0, 0, 0);
				return GM_RSSP1_FALSE;
			}

			if (GM_RSSP1_FALSE == FSFB_Msg_Queue_Init(
				&(g_Link_MQ[conn_index].RCV_SND_MQ[chn_index].SND_MQ),
				pConn_cfg->UDP_chn_cfg[chn_index].send_Q_size ,
				pConn_cfg->UDP_chn_cfg[chn_index].send_Q_offset ,
				FSFB_Q_TYPE_SENT_PKT)
				)
			{
				GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Init_Lib_Link_MQ_Fail, conn_index, chn_index, 2, 0, 0, 0);
				return GM_RSSP1_FALSE;
			}
		}

	}

	/* init CFM msg queue */

	if (GM_RSSP1_FALSE == FSFB_Msg_Queue_Init(&(g_CFM_MQ.SND_MQ)/*&(pCFM->U2L_pri_Q)*/ , pcfg->CFM_cfg.U2L_pri_Q_size , pcfg->CFM_cfg.U2L_pri_Q_offset , FSFB_Q_TYPE_CFM_U2L_PRI))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Init_Lib_CFM_MQ_Fail, 1, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	if (GM_RSSP1_FALSE == FSFB_Msg_Queue_Init(&(g_CFM_MQ.RCV_MQ)/*&(pCFM->L2U_pri_Q)*/ , pcfg->CFM_cfg.L2U_pri_Q_size , pcfg->CFM_cfg.L2U_pri_Q_offset , FSFB_Q_TYPE_CFM_L2U_PRI))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Init_Lib_CFM_MQ_Fail, 2, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

/* start:CR:GM00002526, GM162, fulintai,2015-06-09, new MQ init for standby's all 0 message */
#ifdef	CFM_Stanby_Answer
	Debug_Q_size = 2*(pCFM->connection_nums);
	Debug_Q_offset = (GM_RSSP1_UINT32)GM_RSSP1_malloc(Debug_Q_size * sizeof(GM_RSSP1_CFM_U2L_pri_struct));
	if (GM_RSSP1_FALSE == FSFB_Msg_Queue_Init(&g_CFM_Debug_MQ, Debug_Q_size , Debug_Q_offset , FSFB_Q_TYPE_CFM_U2L_PRI))
	{
		return GM_RSSP1_FALSE;
	}
#endif
/* end */
	pCFM->lib_state = GM_RSSP1_Lib_State_Operational;

	return GM_RSSP1_TRUE;
}
#endif
