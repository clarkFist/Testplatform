#include "GM_RSSP1_SFM.h"
#include "GM_RSSP1_PROC_Int.h"
#include "GM_RSSP1_Msg_Define.h"
#include "GM_RSSP1_Syn.h"
#include "GM_RSSP1_MQ_Interface.h"
#include "GlueData.h"

extern GM_RSSP1_VSN_GET_CALLBACK_FUN GM_RSSP1_VSN_Get_Callback_Fun;	/*added by huang 201511219*/

static GM_RSSP1_SFM_object_struct g_SFM_obj = {0};

extern RSSP1_MQ_Inter_struct g_SFM_MQ;

extern RSSP1_MQ_Inter_struct g_CFM_MQ;

GM_RSSP1_BOOL RSD_Send_Flag = GM_RSSP1_FALSE;

#ifdef OFFLINE_TOOL_SUPPORT
GM_RSSP1_UINT32 App_SYSCKW[2] = {0U};
#endif
GM_RSSP1_BOOL SYS_Is_Local_Sys_Active(void)
{
	/* TODO:增加接口 */
	if(GM_RSSP1_TRUE == GM_RSSP1_APP_Interface_Is_Local_ACTIVE())
	{
		return GM_RSSP1_TRUE;
	}
	else
	{
		return GM_RSSP1_FALSE;
	}
}

GM_RSSP1_SFM_object_struct *GM_RSSP1_Get_SFM_Object(void)
{
	return &g_SFM_obj;
}

GM_RSSP1_BOOL GM_RSSP1_SFM_Ini_FSFB_Offline_Const(GM_RSSP1_SFM_connection_struct *pSFM_conn , GM_RSSP1_SFM_object_struct *pSFM)
{
	GM_RSSP1_INT32 rt = GM_RSSP1_ERROR;
	GM_RSSP1_INT32 chn_index = 0;

	if ((NULL == pSFM_conn) || (NULL == pSFM))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Ini_FSFB_Offline_Const_Param_Point_Error, pSFM_conn, pSFM, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	for (chn_index = 0 ; chn_index < GM_RSSP1_CHECK_CHN_NUM ; ++chn_index)
	{
		rt = GM_RSSP1_PREC_SINT_CAL_Int(
			pSFM_conn->fsfb_chn[chn_index].chn_cfg.remote_sinit ,
			pSFM_conn->fsfb_chn[chn_index].chn_cfg.remote_sid ,
			pSFM_conn->fsfb_chn[chn_index].PREC_SINIT ,
			(GM_RSSP1_UINT32 *)pSFM_conn->fsfb_chn[chn_index].chn_cfg.pLFSR_left_table ,
			(GM_RSSP1_UINT32 *)pSFM_conn->fsfb_chn[chn_index].chn_cfg.pLFSR_right_table ,
			pSFM_conn->deltaTime
			);

		if (GM_RSSP1_ERROR == rt)
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Ini_FSFB_Offline_Const_SINT_Error, 0, 0, 0, 0, 0, 0);
			return GM_RSSP1_FALSE;
		}


		rt = GM_RSSP1_PREC_FIRSTSINIT_CAL(

			pSFM_conn->fsfb_chn[chn_index].chn_cfg.remote_sinit,
			pSFM_conn->fsfb_chn[chn_index].chn_cfg.local_sid,
			/*
			pSFM_conn->fsfb_chn[chn_index].chn_cfg.local_dataVer,
			*/
			pSFM_conn->fsfb_chn[chn_index].chn_cfg.remote_dataVer,
			&(pSFM_conn->fsfb_chn[chn_index].PREC_FIRSTSINIT),
			(GM_RSSP1_UINT32 *)pSFM_conn->fsfb_chn[chn_index].chn_cfg.pLFSR_left_table,
			(GM_RSSP1_UINT32 *)pSFM_conn->fsfb_chn[chn_index].chn_cfg.pLFSR_right_table
			);

		if (GM_RSSP1_ERROR == rt)
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Ini_FSFB_Offline_Const_FIRSTSINIT_Error, 0, 0, 0, 0, 0, 0);
			return GM_RSSP1_FALSE;
		}

		rt = GM_RSSP1_POST_RXDATA_Int(

			pSFM_conn->fsfb_chn[chn_index].chn_cfg.remote_sinit,
			pSFM_conn->fsfb_chn[chn_index].chn_cfg.remote_sid,
			pSFM_conn->fsfb_chn[chn_index].POST_RXDATA,
			(GM_RSSP1_UINT32 *)pSFM_conn->fsfb_chn[chn_index].chn_cfg.pLFSR_left_table,
			(GM_RSSP1_UINT32 *)pSFM_conn->fsfb_chn[chn_index].chn_cfg.pLFSR_right_table,
			pSFM_conn->deltaTime
			);

		if (GM_RSSP1_ERROR == rt)
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Ini_FSFB_Offline_Const_RXDATA_Error, 0, 0, 0, 0, 0, 0);
			return GM_RSSP1_FALSE;
		}
	}

	return GM_RSSP1_TRUE;
}

GM_RSSP1_BOOL GM_RSSP1_SFM_Update_Time(GM_RSSP1_SFM_object_struct *pSFM)
{
	GM_RSSP1_INT32 con_index = 0;
	GM_RSSP1_INT32 chn_index = 0;
	GM_RSSP1_SFM_connection_struct *pSFM_con = NULL;
	if (NULL == pSFM)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Update_Time_Param_Point_Error, pSFM, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	if (GM_RSSP1_Lib_State_Operational != pSFM->lib_state)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Update_Time_Param_Result_No_Match, pSFM->lib_state, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	/* update ENV TC,TS */
	for (con_index = 0 ; con_index < pSFM->connection_nums ; ++con_index)
	{
		pSFM_con = &(pSFM->connection[con_index]);
		GM_RSSP1_TIME_STEP(&(pSFM_con->time));
		for (chn_index = 0 ; chn_index < GM_RSSP1_CHECK_CHN_NUM ; ++chn_index)
		{
			pSFM_con->env[chn_index].TC     = pSFM_con->time.TC;
			pSFM_con->env[chn_index].TS     = pSFM_con->time.TS[chn_index];
		}
	}

	return GM_RSSP1_TRUE;
}

GM_RSSP1_library_state_enum GM_RSSP1_SFM_Get_Library_Status(GM_RSSP1_SFM_object_struct *pSFM)
{
	if (NULL == pSFM)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Get_Library_Status_Point_Error, pSFM, 0, 0, 0, 0, 0);
		return GM_RSSP1_Lib_State_Failure;
	}

	return pSFM->lib_state;
}

GM_RSSP1_SFM_connection_struct *GM_RSSP1_SFM_Get_Connection_BY_SaCEPID(GM_RSSP1_SACEPID_struct *pSaCEPID , GM_RSSP1_SFM_object_struct *pSFM)
{
	GM_RSSP1_INT32 conn_index = 0;
	GM_RSSP1_SFM_connection_struct *pSFM_conn = NULL;

	if ((NULL == pSFM) || (NULL == pSaCEPID))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Get_Connection_BY_SaCEPID_Point_Error, pSFM, 0, 0, 0, 0, 0);
		return NULL;
	}

	if (GM_RSSP1_Lib_State_Operational != pSFM->lib_state)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Get_Connection_BY_SaCEPID_Result_No_Match, pSFM->lib_state, 0, 0, 0, 0, 0);
		return NULL;
	}

	for (conn_index = 0 ; conn_index < pSFM->connection_nums ; ++conn_index)
	{
		pSFM_conn = &(pSFM->connection[conn_index]);

		if (GM_RSSP1_TRUE == GM_RSSP1_SFM_Is_SaCEPID_Equal(pSaCEPID , &(pSFM_conn->SaCEPID)))
		{
			return pSFM_conn;
		}
	}

	return NULL;
}

GM_RSSP1_BOOL GM_RSSP1_SFM_Is_SaCEPID_Equal(GM_RSSP1_SACEPID_struct *pSaCEPID1 , GM_RSSP1_SACEPID_struct *pSaCEPID2)
{
	if ((NULL == pSaCEPID1) || (NULL == pSaCEPID2))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Is_SaCEPID_Equal_Point_Error, pSaCEPID1, pSaCEPID2, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	if ((pSaCEPID1->source_addr == pSaCEPID2->source_addr) && (pSaCEPID1->dest_addr == pSaCEPID2->dest_addr))
	{
		return GM_RSSP1_TRUE;
	}
	else
	{
		return GM_RSSP1_FALSE;
	}
}


GM_RSSP1_User_Put_Pri_To_SFM_Result_enum GM_RSSP1_User_Put_Pri_To_SFM(GM_RSSP1_SFM_U2L_pri_struct *pU2L , GM_RSSP1_SFM_object_struct *pSFM)
{
	GM_RSSP1_Write_Q_Return_Enum write_rt = Q_WRITE_FAIL;

	if ((NULL == pSFM) || (pSFM->lib_state != GM_RSSP1_Lib_State_Operational))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_User_Put_Pri_To_SFM_Result_No_Match, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_User_Put_Pri_LIB_NOT_OPERATIONAL;
	}

	if (NULL == pU2L)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_User_Put_Pri_To_SFM_Point_Error, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_User_Put_Pri_INVALID_U2L;
	}

	if (GM_RSSP1_Pri_Type_SFM_U2L_Unknow == pU2L->type)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_User_Put_Pri_To_SFM_Type_Unknow, pU2L->type, 0, 0, 0, 0, 0);
		return GM_RSSP1_User_Put_Pri_INVALID_REQ_TYPE;
	}

	if (NULL == GM_RSSP1_SFM_Get_Connection_BY_SaCEPID(&(pU2L->SaCEPID), pSFM))
	{
		/*
		GM_RSSP1_Log_Msg(0,"FSFB SFM SaCEPID (source:0x%X ,dest:0x%X) is INVALID , check cfg!\n", pU2L->SaCEPID.source_addr , pU2L->SaCEPID.dest_addr , 0, 0, 0, 0);
		*/
		GM_RSSP1_Log_Usr(GM_RSSP1_User_Put_Pri_To_SFM_Index_Unknow, 0, 0, 0, 0, 0, 0);

		return GM_RSSP1_User_Put_Pri_INVALID_CONNECTION_ID;
	}

	/*
	GM_RSSP1_Log_Msg(0,"~~~~~~~~~~ FSFB send %d bytes to (dest:0x%2X).\n",pU2L->as.data_req.bytes_count, pU2L->SaCEPID.dest_addr ,0,0,0,0);
	*/
	write_rt = FSFB_Msg_Queue_Write(&(g_SFM_MQ.SND_MQ) , pU2L);

	if (Q_WRITE_FAIL == write_rt)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_User_Put_Pri_To_SFM_WRITE_FAIL, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_User_Put_Pri_LIB_NOT_OPERATIONAL;
	}
	else if (Q_WRITE_OK_FULL == write_rt)
	{
		/* 成功，但队列已满 */
		GM_RSSP1_Log_Usr(GM_RSSP1_User_Put_Pri_To_SFM_WRITE_OK_FULL, 0, 0, 0, 0, 0, 0);
		GM_RSSP1_Log_Msg(0,"FSFB SFM U2L_pri_Q FULL!\n ", 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_User_Put_Pri_MQ_FULL;
	}
	else
	{
		/* 成功 */
		return GM_RSSP1_User_Put_Pri_OK;
	}
}

GM_RSSP1_User_Get_Pri_From_SFM_Result_enum GM_RSSP1_User_Get_Pri_From_SFM(GM_RSSP1_SFM_L2U_pri_struct *pL2U , GM_RSSP1_SFM_object_struct *pSFM)
{
	if (NULL == pL2U)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_User_Get_Pri_From_SFM_Point_Error, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_User_Get_Pri_INVALID_L2U;
	}

	if ((NULL == pSFM) || (GM_RSSP1_Lib_State_Operational != pSFM->lib_state))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_User_Get_Pri_From_SFM_Result_No_Match, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_User_Get_Pri_LIB_NOT_OPERATIONAL;
	}

	if (GM_RSSP1_TRUE == FSFB_Msg_Queue_Get(&(g_SFM_MQ.RCV_MQ/*pSFM->L2U_pri_Q*/), pL2U))
	{
		return GM_RSSP1_User_Get_Pri_OK;
	}
	else
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_User_Get_Pri_From_SFM_Get_Pri_Fail, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_User_Get_Pri_LIB_NOT_OPERATIONAL;
	}
}

GM_RSSP1_BOOL GM_RSSP1_User_Send_Dat(GM_RSSP1_SACEPID_struct *pSaCEPID , GM_RSSP1_UINT8 *pDat , GM_RSSP1_UINT32 dat_len , Local_Sys_AB_enum sysAORB , GM_RSSP1_BOOL bSysActive, GM_RSSP1_SFM_object_struct *pSFM)
{
	GM_RSSP1_SFM_U2L_pri_struct pri = {0};
	GM_RSSP1_User_Put_Pri_To_SFM_Result_enum put_pri_rt = {0};

	if ((NULL == pSaCEPID) || (NULL == pDat) || (NULL == pSFM))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_User_Send_Dat_Point_Error, pSaCEPID, pDat, pSFM, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	if (GM_RSSP1_Lib_State_Operational != pSFM->lib_state)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_User_Send_Dat_Result_No_Match, pSFM->lib_state, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	if ((dat_len > GM_RSSP1_MAX_USER_DAT_LEN) || (dat_len <= 0))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_User_Send_Dat_Data_Error, dat_len, GM_RSSP1_MAX_USER_DAT_LEN, 0, 0, 0, 0);
		GM_RSSP1_Log_Msg(0,"FSFB SFM User dat dat_len exceed GM_RSSP1_MAX_USER_DAT_LEN:%d ,or dat src is null.\n", GM_RSSP1_MAX_USER_DAT_LEN, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	pri.type                    = GM_RSSP1_Pri_Type_SFM_Data_req;

	pri.SaCEPID                 = *(pSaCEPID);

	if (GM_RSSP1_Local_Sys_A == sysAORB)
	{
		pri.as.data_req.sys_A_or_B  = RSSP1_RSD_FRAME_TYPE_FROM_A;
	}
	else if (GM_RSSP1_Local_Sys_B == sysAORB)
	{
		pri.as.data_req.sys_A_or_B  = RSSP1_RSD_FRAME_TYPE_FROM_B;
	}
	else
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_User_Send_Dat_SYS_Unknow, sysAORB, 0, 0, 0, 0, 0);
		GM_RSSP1_Log_Msg(0,"FSFB Synstem AorB:%d error!.\n", sysAORB, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	if (GM_RSSP1_TRUE == bSysActive)
	{
		pri.as.data_req.active_or_standby   = RSSP1_RSD_PROTOCOL_TYPE_ACTIVE;
	}
	else
	{
		pri.as.data_req.active_or_standby   = RSSP1_RSD_PROTOCOL_TYPE_STANDBY;
	}

	pri.as.data_req.bytes_count = (GM_RSSP1_UINT16)dat_len;

	GM_RSSP1_memcpy(pri.as.data_req.byte , pDat , dat_len);

	/* 防护SFM的U2L队列，防止proc_send的通过while(1)从队列取数据的操作被打断，又放入新数据，在内存刷新时被刷掉 */
	put_pri_rt = GM_RSSP1_User_Put_Pri_To_SFM(&pri , pSFM);

	if ((GM_RSSP1_User_Put_Pri_OK == put_pri_rt) || (GM_RSSP1_User_Put_Pri_MQ_FULL == put_pri_rt))
	{
		return GM_RSSP1_TRUE;
	}
	else
	{
		return GM_RSSP1_FALSE;
	}
}

GM_RSSP1_BOOL GM_RSSP1_SFM_Send_RSD(GM_RSSP1_SFM_U2L_pri_struct *pDat_pri , GM_RSSP1_SFM_connection_struct *pSFM_conn , GM_RSSP1_SFM_object_struct *pSFM)
{
	GM_RSSP1_CFM_U2L_pri_struct cfm_pri = {0}; /* RSD包最终以U2L_pri的形式传递至CFM */
	GM_RSSP1_SFM_Put_Pri_To_CFM_Result_enum  put_pri_rt = GM_RSSP1_SFM_Put_Pri_LIB_NOT_OPERATIONAL;
	GM_RSSP1_INT32 rt = GM_RSSP1_ERROR;
	GM_RSSP1_UINT32 crc32[GM_RSSP1_CHECK_CHN_NUM] = {0};
	GM_RSSP1_UINT32 crcm[GM_RSSP1_CHECK_CHN_NUM] = {0};
	GM_RSSP1_INT32 chn_index = 0;
#ifdef OFFLINE_TOOL_SUPPORT
	GM_RSSP1_UINT32 vsn0 =0U;
	GM_RSSP1_UINT32 vsn[2] ={0U};
#endif

	if ((NULL == pDat_pri) || (NULL == pSFM_conn) || (NULL == pSFM))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Send_RSD_Point_Error, pDat_pri, pSFM_conn, pSFM, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	if (GM_RSSP1_Lib_State_Operational != pSFM->lib_state)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Send_RSD_Result_No_Match, pSFM->lib_state, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	GM_RSSP1_memset(&cfm_pri, 0, sizeof(GM_RSSP1_CFM_U2L_pri_struct));
	cfm_pri.type                = GM_RSSP1_CFM_Data_req;

	cfm_pri.index               = pSFM_conn->index;

	cfm_pri.as.data_req.type    = GM_RSSP1_MSG_TYPE_RSD;
	cfm_pri.as.data_req.TC      = pSFM_conn->env[0].TC;

#ifdef OFFLINE_TOOL_SUPPORT
	GM_RSSP1_VSN_Get_Callback_Fun(&vsn0,&vsn[0],&vsn[1]);
	pDat_pri->as.data_req.bytes_count = pDat_pri->as.data_req.bytes_count - sizeof(GM_RSSP1_UINT32)*2;/*Twin CRC32 长度*/

	for (chn_index = 0 ; chn_index < GM_RSSP1_CHECK_CHN_NUM ; ++chn_index)
	{
		crc32[chn_index] = *(GM_RSSP1_UINT32*)(pDat_pri->as.data_req.byte + pDat_pri->as.data_req.bytes_count + chn_index * sizeof(GM_RSSP1_UINT32));
		crcm[chn_index] = crc32[chn_index] ^ App_SYSCKW[chn_index] ^ (pSFM_conn->fsfb_chn[chn_index].chn_cfg.local_sid) ^ (pSFM_conn->env[chn_index].TS) ^vsn[chn_index];
	}
#else
	for (chn_index = 0 ; chn_index < GM_RSSP1_CHECK_CHN_NUM ; ++chn_index)
	{
		GM_RSSP1_CRC_Calculate_CRC32(pDat_pri->as.data_req.byte, pDat_pri->as.data_req.bytes_count, &(crc32[chn_index]), (pSFM_conn->fsfb_chn[chn_index].chn_cfg.pCRC32_table));
              crcm[chn_index] = crc32[chn_index] ^(~(pSFM_conn->fsfb_chn[chn_index].chn_cfg.local_sys_chk)) ^(pSFM_conn->fsfb_chn[chn_index].chn_cfg.local_sid) ^(pSFM_conn->env[chn_index].TS) ;
	}
#endif
	if(g_bugInsert.m_nSYSCKW !=0)
	{
		return GM_RSSP1_FALSE; 
	}


#ifdef RSSP1_FAULT_INJECT
	crcm[0] += *(pFaults + RSSP1_FAULT_CRC1);
	crcm[1] += *(pFaults + RSSP1_FAULT_CRC2);
	pSFM_conn->env[0].TC += *(pFaults + RSSP1_FAULT_SEQ);
#endif 

	rt  = GM_RSSP1_RSD_BUILD_Int(
		cfm_pri.as.data_req.byte ,
		&(cfm_pri.as.data_req.bytes_count) ,
		pDat_pri->as.data_req.byte ,
		pDat_pri->as.data_req.bytes_count ,
		pDat_pri->as.data_req.active_or_standby ,
		pDat_pri->as.data_req.sys_A_or_B ,
		pDat_pri->SaCEPID.source_addr ,   /* 地址使用VLE应用层传下的信息，防止匹配错误 jianghongjun 20110825 safety enhancement */
		pDat_pri->SaCEPID.dest_addr ,
		pSFM_conn->env[0].TC ,  /* env双通道中TC值始终相同，默认取第一个即可 */
		crcm[0],
		crcm[1],
		(GM_RSSP1_UINT32 *)(pSFM_conn->fsfb_chn[0].chn_cfg.pCRC16_table)
		);

	if (GM_RSSP1_ERROR == rt)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Send_RSD_Build_Error, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

#ifdef OFFLINE_TOOL_SUPPORT
		pDat_pri->as.data_req.bytes_count = pDat_pri->as.data_req.bytes_count + sizeof(GM_RSSP1_UINT32)*2;/*Twin CRC32 长度*/
#endif

#ifdef RSSP1_FAULT_INJECT
	if (*(pFaults + RSSP1_FAULT_LOSS) > 0)
	{
		*(pFaults + RSSP1_FAULT_LOSS) = *(pFaults + RSSP1_FAULT_LOSS) - 1;
		return GM_RSSP1_TRUE;
	}
#endif 

	/* 将数据包交给CFM模块发送 */
	put_pri_rt = GM_RSSP1_SFM_Put_Pri_To_CFM(&cfm_pri , pSFM);

	if ((GM_RSSP1_SFM_Put_Pri_OK == put_pri_rt) || (GM_RSSP1_SFM_Put_Pri_MQ_FULL == put_pri_rt))
	{
		/*
		GM_RSSP1_Log_Msg(0,"FSFB SFM ID:0x%X --------> send RSD.%d bytes.\n",pSFM_conn->SaCEPID.dest_addr , cfm_pri.as.data_req.bytes_count,0,0,0,0);
		FSFB_Output_Data_As_HexString(cfm_pri.as.data_req.byte , cfm_pri.as.data_req.bytes_count);
		*/
		return GM_RSSP1_TRUE;
	}
	else
	{
		return GM_RSSP1_FALSE;
	}
}

GM_RSSP1_BOOL GM_RSSP1_SFM_Send_SSE(GM_RSSP1_SFM_connection_struct *pSFM_conn , GM_RSSP1_SFM_object_struct *pSFM)
{
	GM_RSSP1_CFM_U2L_pri_struct cfm_pri = {0};                    /* SSE包最终以U2L_pri的形式传递至CFM */
	GM_RSSP1_SFM_Put_Pri_To_CFM_Result_enum  put_pri_rt = GM_RSSP1_SFM_Put_Pri_LIB_NOT_OPERATIONAL;
	GM_RSSP1_INT32 rt = GM_RSSP1_ERROR;

	if ((NULL == pSFM_conn) || (NULL == pSFM))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Send_SSE_Point_Error, pSFM_conn, pSFM, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	if (GM_RSSP1_Lib_State_Operational != pSFM->lib_state)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Send_SSE_Result_No_Match, pSFM->lib_state, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	GM_RSSP1_memset(&cfm_pri, 0, sizeof(GM_RSSP1_CFM_U2L_pri_struct));
	cfm_pri.type                = GM_RSSP1_CFM_Data_req;
	cfm_pri.index               = pSFM_conn->index;
	cfm_pri.as.data_req.type    = GM_RSSP1_MSG_TYPE_SSE;
	cfm_pri.as.data_req.TC      = pSFM_conn->env[0].TC;

	rt  = GM_RSSP1_SSE_BUILD_Int(
		cfm_pri.as.data_req.byte ,
		&(cfm_pri.as.data_req.bytes_count) ,
		pSFM_conn->source_addr ,
		pSFM_conn->dest_addr ,
		pSFM_conn->env[0].TC ,  /* env双通道中TC值始终相同，默认取第一个即可 */
		pSFM_conn->fsfb_chn[0].chn_cfg.local_sid , pSFM_conn->env[0].TS,
		pSFM_conn->fsfb_chn[1].chn_cfg.local_sid , pSFM_conn->env[1].TS,
		&(pSFM_conn->env[0].sseTC) , &(pSFM_conn->env[1].sseTC),
		&(pSFM_conn->env[0].sseTS) , &(pSFM_conn->env[1].sseTS),
		&(pSFM_conn->env[0].bSendSSE) , &(pSFM_conn->env[1].bSendSSE) ,
		pSFM_conn->lifeTime ,
		(GM_RSSP1_UINT32 *)(pSFM_conn->fsfb_chn[0].chn_cfg.pCRC16_table)
		);

	if (GM_RSSP1_ERROR == rt)
	{
		/*
		GM_RSSP1_SFM_Report_Warning_To_User(pSFM_conn , pSFM , GM_RSSP1_SFM_WARNING_TYPE_SSE_BUILD_ERR);
		GM_RSSP1_Log_Msg(0,"FSFB SFM ID:0x%X SSE build fail! SSE sent,wait for SSR in life_time\n",pSFM_conn->SaCEPID.dest_addr ,0,0,0,0,0);
		*/
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Send_SSE_Build_Error, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	/* 将数据包交给CFM模块发送 */
	put_pri_rt = GM_RSSP1_SFM_Put_Pri_To_CFM(&cfm_pri, pSFM);

	if ((GM_RSSP1_SFM_Put_Pri_OK == put_pri_rt) || (GM_RSSP1_SFM_Put_Pri_MQ_FULL == put_pri_rt))
	{		
		GM_RSSP1_Log_Msg(0,"FSFB SFM ID:0x%X ----> send SSE.%d bytes.(%d.%d),index:%d.\n", pSFM_conn->SaCEPID.dest_addr , cfm_pri.as.data_req.bytes_count, pSFM_conn->env[0].local_TC, pSFM_conn->env[0].remote_TC, pSFM_conn->index, 0);
		return GM_RSSP1_TRUE;
	}
	else
	{
		GM_RSSP1_Log_Msg(0,"@@@FSFB SFM ID:0x%X ----> send SSE.%d bytes.(%d.%d),index:%d. FAIL\n", pSFM_conn->SaCEPID.dest_addr , cfm_pri.as.data_req.bytes_count, pSFM_conn->env[0].local_TC, pSFM_conn->env[0].remote_TC, pSFM_conn->index, 0);
		return GM_RSSP1_FALSE;
	}
}

GM_RSSP1_BOOL GM_RSSP1_SFM_Send_SSR(GM_RSSP1_UINT32 rcvd_SEQENQ_1 , GM_RSSP1_UINT32 rcvd_SEQENQ_2 , GM_RSSP1_UINT32 rcvd_TC , GM_RSSP1_SFM_connection_struct *pSFM_conn , GM_RSSP1_SFM_object_struct *pSFM)
{
	GM_RSSP1_CFM_U2L_pri_struct cfm_pri = {0};                    /* RSD包最终以U2L_pri的形式传递至CFM */
	GM_RSSP1_SFM_Put_Pri_To_CFM_Result_enum  put_pri_rt = GM_RSSP1_SFM_Put_Pri_LIB_NOT_OPERATIONAL;
	GM_RSSP1_INT32 rt = GM_RSSP1_ERROR;

	if ((NULL == pSFM_conn) || (NULL == pSFM))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Send_SSR_Point_Error, pSFM_conn, pSFM, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	if (GM_RSSP1_Lib_State_Operational != pSFM->lib_state)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Send_SSR_Result_No_Match, pSFM->lib_state, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	GM_RSSP1_memset(&cfm_pri, 0, sizeof(GM_RSSP1_CFM_U2L_pri_struct));
	cfm_pri.type                = GM_RSSP1_CFM_Data_req;

	cfm_pri.index               = pSFM_conn->index;

	cfm_pri.as.data_req.type    = GM_RSSP1_MSG_TYPE_SSR;
	cfm_pri.as.data_req.TC      = pSFM_conn->env[0].TC;

	rt  = GM_RSSP1_SSR_BUILD_Int(
		cfm_pri.as.data_req.byte ,
		&(cfm_pri.as.data_req.bytes_count) ,
		pSFM_conn->source_addr ,
		pSFM_conn->dest_addr ,
		rcvd_SEQENQ_1 ,
		rcvd_SEQENQ_2 ,
		pSFM_conn->fsfb_chn[0].chn_cfg.local_sid ,
		pSFM_conn->fsfb_chn[1].chn_cfg.local_sid ,
		pSFM_conn->fsfb_chn[0].chn_cfg.local_dataVer , /* 设计院。 @NOTE:注意!build SSR使用的是本地还是对方的dataver! */
		pSFM_conn->fsfb_chn[1].chn_cfg.local_dataVer ,
		/*
		pSFM_conn->fsfb_chn[0].chn_cfg.remote_dataVer , / * @NOTE:注意!build SSR使用的是本地还是对方的dataver! * /
		pSFM_conn->fsfb_chn[1].chn_cfg.remote_dataVer ,
		*/
		pSFM_conn->env[0].TS,
		pSFM_conn->env[1].TS,
		rcvd_TC,
		pSFM_conn->env[0].TC ,
		0x01,/* RSSP1:预留固定值 jianghongjun LKD00000623 */
		(GM_RSSP1_UINT32 *)(pSFM_conn->fsfb_chn[0].chn_cfg.pCRC16_table)
		);

	if (GM_RSSP1_ERROR == rt)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Send_SSR_Build_Error, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	/* 将数据包交给CFM模块发送 */
	put_pri_rt = GM_RSSP1_SFM_Put_Pri_To_CFM(&cfm_pri, pSFM);

	if ((GM_RSSP1_SFM_Put_Pri_OK == put_pri_rt) || (GM_RSSP1_SFM_Put_Pri_MQ_FULL == put_pri_rt))
	{
		GM_RSSP1_Log_Msg(0,"FSFB SFM ID:0x%X ------> send SSR.%d bytes.(%d.%d),index:%d.\n", pSFM_conn->SaCEPID.dest_addr , cfm_pri.as.data_req.bytes_count, pSFM_conn->env[0].local_TC, pSFM_conn->env[0].remote_TC, pSFM_conn->index, 0);
		return GM_RSSP1_TRUE;
	}
	else
	{
		return GM_RSSP1_FALSE;
	}
}

void GM_RSSP1_SFM_Proc_Rcvd_SSE(GM_RSSP1_SFM_connection_struct *pSFM_conn , GM_RSSP1_CFM_L2U_Dat_pri_struct *pDat_indi , GM_RSSP1_SFM_object_struct *pSFM)
{
	GM_RSSP1_UINT16 crc16 = 0U;
	GM_RSSP1_UINT16 rcvd_crc16 = 0U;
	GM_RSSP1_UINT32 rcvd_TC = 0U;
	GM_RSSP1_UINT32 SEQENQ1 = 0U;
	GM_RSSP1_UINT32 SEQENQ2 = 0U;
	GM_RSSP1_UINT16 rcvd_src_addr = 0U;
	GM_RSSP1_UINT16 rcvd_dest_addr = 0U;

	if ((NULL == pSFM_conn) || (NULL == pDat_indi) || (NULL == pSFM))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Proc_Rcvd_SSE_Point_Error, pSFM_conn, pDat_indi, pSFM, 0, 0, 0);
		return;
	}

	if (GM_RSSP1_Lib_State_Operational != pSFM->lib_state)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Proc_Rcvd_SSE_Result_No_Match, pSFM->lib_state, 0, 0, 0, 0, 0);
		return;
	}

	if (pDat_indi->bytes_count != RSSP1_SSE_FRAME_LEN_WITH_CRC)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Proc_Rcvd_SSE_Byte_Loss, pDat_indi->bytes_count, RSSP1_SSE_FRAME_LEN_WITH_CRC, 0, 0, 0, 0);
		GM_RSSP1_Log_Msg(0,"FSFB SFM ID: 0x%X SSE rcvd frame length err! rcvd:%d , RSSP1_SSE_FRAME_LEN_WITH_CRC:%d.\n", pSFM_conn->SaCEPID.dest_addr , pDat_indi->bytes_count , RSSP1_SSE_FRAME_LEN_WITH_CRC , 0, 0, 0);
		return;
	}

	/* 记录收到SSE事件，证明对方校验失败 */
	GM_RSSP1_SFM_Report_Warning_To_User(pSFM_conn , pSFM , GM_RSSP1_SFM_WARNING_TYPE_RECV_SSE);

	rcvd_src_addr = (pDat_indi->byte[3] << 8) + pDat_indi->byte[2];
	rcvd_dest_addr = (pDat_indi->byte[5] << 8) + pDat_indi->byte[4];
	if ((rcvd_src_addr != pSFM_conn->dest_addr) || (rcvd_dest_addr != pSFM_conn->source_addr))
	{
		/* 收到来源/目的 */
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Proc_Rcvd_SSE_Addr_Error, pSFM_conn->dest_addr, rcvd_src_addr, pSFM_conn->source_addr, rcvd_dest_addr, 0, 0);
		GM_RSSP1_Log_Msg(0,"FSFB SFM ID:0x%X,index:%d, FSFB rcv SSE src/dest err!(from:0x%X,to:0x%X) err! cfg:(from:0x%X,to:0x%X).\n", pSFM_conn->SaCEPID.dest_addr, pSFM_conn->index, rcvd_src_addr, rcvd_dest_addr, pSFM_conn->dest_addr, pSFM_conn->source_addr);
		return;
	}

 	/*FSFB_CRC16_Calc((GM_RSSP1_UINT32 *)(pSFM_conn->fsfb_chn[0].chn_cfg.pCRC16_table) , &crc16 , (GM_RSSP1_INT8 *)(pDat_indi->byte) , pDat_indi->bytes_count - 2);*/
	GM_RSSP1_CRC_Calculate_CRC16(pDat_indi->byte, pDat_indi->bytes_count - 2, &crc16, pSFM_conn->fsfb_chn[0].chn_cfg.pCRC16_table);

	rcvd_crc16 = ((pDat_indi->byte[pDat_indi->bytes_count-1]) << 8) + (pDat_indi->byte[pDat_indi->bytes_count-2]);

	if (crc16 != rcvd_crc16)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Proc_Rcvd_SSE_CRC16_Error, pSFM_conn->dest_addr, pSFM_conn->source_addr, rcvd_src_addr, 0, 0, 0);
		GM_RSSP1_SFM_Report_Warning_To_User(pSFM_conn , pSFM , GM_RSSP1_SFM_WARNING_TYPE_RCVD_SSE_CRC_ERROR);
		GM_RSSP1_Log_Msg(0,"FSFB SFM ID: 0x%X rcvd SSE frame CRC16 err!\n", pSFM_conn->SaCEPID.dest_addr , 0, 0, 0, 0, 0);
		return;
	}
	else
	{
		if (GM_RSSP1_FALSE == SYS_Is_Local_Sys_Active())
		{
			pSFM_conn->state = GM_RSSP1_Layer_State_Connected;
			return;
		}
	}

	rcvd_TC = (pDat_indi->byte[6]) + ((pDat_indi->byte[7]) << 8) + ((pDat_indi->byte[8]) << 16) + ((pDat_indi->byte[9]) << 24);

	SEQENQ1 = (pDat_indi->byte[10]) + ((pDat_indi->byte[11]) << 8) + ((pDat_indi->byte[12]) << 16) + ((pDat_indi->byte[13]) << 24);
	SEQENQ2 = (pDat_indi->byte[14]) + ((pDat_indi->byte[15]) << 8) + ((pDat_indi->byte[16]) << 16) + ((pDat_indi->byte[17]) << 24);

	GM_RSSP1_Log_Msg(0,"Rcving SSE: RcvTC = %d\n", rcvd_TC, 0, 0, 0, 0, 0);
#ifdef WIN32
	printf("<<<<<<<<<<<<<SSE: con:%d, rcvd_TC=%d, SEQENQ1=%x\n", pSFM_conn->index, rcvd_TC, SEQENQ1);
#endif
	GM_RSSP1_SFM_Send_SSR(SEQENQ1 , SEQENQ2 , rcvd_TC , pSFM_conn , pSFM);

	return;
}

void GM_RSSP1_SFM_Proc_Rcvd_SSR(GM_RSSP1_SFM_connection_struct *pSFM_conn , GM_RSSP1_CFM_L2U_Dat_pri_struct *pDat_indi , GM_RSSP1_SFM_object_struct *pSFM)
{
	GM_RSSP1_UINT16 crc16 = 0U;
	GM_RSSP1_UINT16 rcvd_crc16 = 0U;
	GM_RSSP1_UINT32 SEQINI[GM_RSSP1_CHECK_CHN_NUM] = {0};
	GM_RSSP1_UINT32 rcvd_TC_R = 0U;
	GM_RSSP1_UINT32 rcvd_TC_E = 0U;
	GM_RSSP1_INT32 chn_index = 0;
	GM_RSSP1_BOOL bchn_set_fail = GM_RSSP1_FALSE;
	GM_RSSP1_UINT8 rcvd_dataver = 0U;
	GM_RSSP1_UINT16 rcvd_src_addr = 0U;
	GM_RSSP1_UINT16 rcvd_dest_addr = 0U;

	if ((NULL == pSFM_conn) || (NULL == pDat_indi) || (NULL == pSFM))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Proc_Rcvd_SSR_Point_Error, pSFM_conn, pDat_indi, pSFM, 0, 0, 0);
		return;
	}

	if (GM_RSSP1_Lib_State_Operational != pSFM->lib_state)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Proc_Rcvd_SSR_Result_No_Match, pSFM->lib_state, 0, 0, 0, 0, 0);
		return;
	}

	if (pDat_indi->bytes_count != RSSP1_SSR_FRAME_LEN_WITH_CRC)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Proc_Rcvd_SSR_Byte_Loss, pDat_indi->bytes_count, RSSP1_SSR_FRAME_LEN_WITH_CRC, 0, 0, 0, 0);
		GM_RSSP1_Log_Msg(0,"FSFB SFM ID: 0x%X SSR rcvd frame length err! rcvd:%d , RSSP1_SSR_FRAME_LEN_WITH_CRC:%d.\n", pSFM_conn->SaCEPID.dest_addr , pDat_indi->bytes_count , RSSP1_SSR_FRAME_LEN_WITH_CRC , 0, 0, 0);
		return;
	}

	/* 记录收到SSR事件 */
	GM_RSSP1_SFM_Report_Warning_To_User(pSFM_conn , pSFM , GM_RSSP1_SFM_WARNING_TYPE_RECV_SSR);

	rcvd_src_addr = (pDat_indi->byte[3] << 8) + pDat_indi->byte[2];
	rcvd_dest_addr = (pDat_indi->byte[5] << 8) + pDat_indi->byte[4];
	if ((rcvd_src_addr != pSFM_conn->dest_addr) || (rcvd_dest_addr != pSFM_conn->source_addr))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Proc_Rcvd_SSR_Addr_Error, pSFM_conn->dest_addr, rcvd_src_addr, pSFM_conn->source_addr, rcvd_dest_addr, 0, 0);
		/* 收到来源/目的 */
		GM_RSSP1_Log_Msg(0,"FSFB SFM ID:0x%X,index:%d, FSFB rcv SSR src/dest err!(from:0x%X,to:0x%X) err! cfg:(from:0x%X,to:0x%X).\n", pSFM_conn->SaCEPID.dest_addr, pSFM_conn->index, rcvd_src_addr, rcvd_dest_addr, pSFM_conn->dest_addr, pSFM_conn->source_addr);
		return;
	}

 	/*FSFB_CRC16_Calc((GM_RSSP1_UINT32 *)(pSFM_conn->fsfb_chn[0].chn_cfg.pCRC16_table) , &crc16 , (GM_RSSP1_INT8 *)(pDat_indi->byte) , pDat_indi->bytes_count - 2);*/
	GM_RSSP1_CRC_Calculate_CRC16(pDat_indi->byte, pDat_indi->bytes_count - 2, &crc16, pSFM_conn->fsfb_chn[0].chn_cfg.pCRC16_table);

	rcvd_crc16 = ((pDat_indi->byte[pDat_indi->bytes_count-1]) << 8) + (pDat_indi->byte[pDat_indi->bytes_count-2]);

	if (crc16 != rcvd_crc16)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Proc_Rcvd_SSR_CRC16_Error, 0, 0, 0, 0, 0, 0);
		GM_RSSP1_SFM_Report_Warning_To_User(pSFM_conn , pSFM , GM_RSSP1_SFM_WARNING_TYPE_RCVD_SSR_CRC_ERROR);
		GM_RSSP1_Log_Msg(0,"FSFB SFM ID: 0x%X rcvd SSR frame CRC16 err!\n", pSFM_conn->SaCEPID.dest_addr , 0, 0, 0, 0, 0);
		return;
	}

	rcvd_TC_R  = (pDat_indi->byte[6]) + ((pDat_indi->byte[7]) << 8) + ((pDat_indi->byte[8]) << 16) + ((pDat_indi->byte[9]) << 24);

	rcvd_TC_E  = (pDat_indi->byte[10]) + ((pDat_indi->byte[11]) << 8) + ((pDat_indi->byte[12]) << 16) + ((pDat_indi->byte[13]) << 24);
	
	SEQINI[0]   = (pDat_indi->byte[14]) + ((pDat_indi->byte[15]) << 8) + ((pDat_indi->byte[16]) << 16) + ((pDat_indi->byte[17]) << 24);
	SEQINI[1]   = (pDat_indi->byte[18]) + ((pDat_indi->byte[19]) << 8) + ((pDat_indi->byte[20]) << 16) + ((pDat_indi->byte[21]) << 24);

	/*增加对SSR中数据版本(预留固定值0x01)的检查 jianghongjun TSRS00000125 */
	rcvd_dataver = pDat_indi->byte[22];
	if (rcvd_dataver != 0x01U)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Proc_Rcvd_SSR_DataVer_Error, 0, 0, 0, 0, 0, 0);
		GM_RSSP1_Log_Msg(0,"FSFB SFM ID:0x%X SSR dataver %d err!\n", rcvd_dataver , 0, 0, 0, 0, 0);
		return;
	}

	for (chn_index = 0 ; chn_index < GM_RSSP1_CHECK_CHN_NUM ; ++chn_index)
	{
		if (GM_RSSP1_ERROR == GM_RSSP1_SSR_CHN_SET(
			(GM_RSSP1_UINT32 *)(pSFM_conn->fsfb_chn[chn_index].chn_cfg.pLFSR_left_table),
			(GM_RSSP1_UINT32 *)(pSFM_conn->fsfb_chn[chn_index].chn_cfg.pLFSR_right_table),
			SEQINI[chn_index],
			pSFM_conn->fsfb_chn[chn_index].PREC_FIRSTSINIT,
			pSFM_conn->env[chn_index].sseTC,
			pSFM_conn->env[chn_index].sseTS,
			&(pSFM_conn->env[chn_index].dynamicKey),
			pSFM_conn->env[chn_index].TC,
			&(pSFM_conn->env[chn_index].local_TC),
			&(pSFM_conn->env[chn_index].remote_TC),
			rcvd_TC_E,
			rcvd_TC_R,
			&(pSFM_conn->env[chn_index].bSendSSE),
			pSFM_conn->lifeTime
			))
		{
			bchn_set_fail = GM_RSSP1_TRUE;
		}
	}
	if (GM_RSSP1_TRUE == bchn_set_fail)
	{
		GM_RSSP1_Log_Msg(0,"FSFB SFM ID:0x%X SSR channel set err!\n", pSFM_conn->SaCEPID.dest_addr , 0, 0, 0, 0, 0);
	}
	else
	{
		pSFM_conn->LastSendSSECycle = rcvd_TC_E;  /*CR:3894 update prectime*/
		pSFM_conn->RxSSRTrem = rcvd_TC_R;	/*计算preciTime用，added by huang 20151125*/
		pSFM_conn->RxSSRTloc = pSFM_conn->env[0].TC; /*CR:3894 update prectime*/
		pSFM_conn->state = GM_RSSP1_Layer_State_Connected;
	}
}

void GM_RSSP1_SFM_Proc_Rcvd_RSD(GM_RSSP1_SFM_connection_struct *pSFM_conn , GM_RSSP1_CFM_L2U_Dat_pri_struct *pDat_indi , GM_RSSP1_SFM_object_struct *pSFM)
{
	GM_RSSP1_UINT16 crc16 = 0U;
	GM_RSSP1_UINT16 rcvd_crc16 = 0U;
	GM_RSSP1_UINT32 rcvd_crc32[GM_RSSP1_CHECK_CHN_NUM] = {0};
	GM_RSSP1_UINT32 rcvd_crcm[GM_RSSP1_CHECK_CHN_NUM] = {0};
	GM_RSSP1_UINT32 rcvd_TC = 0U;
	GM_RSSP1_INT32 chn_index = 0;
	GM_RSSP1_INT32 rcvd_app_dat_len = 0;    /* 根据接收到的数据长度 - RSSP1协议规定的其他域长度得到的应用数据大小 */
	GM_RSSP1_INT32 app_dat_len_in_pkt = 0;    /* 数据包中SDS域指明的应用数据大小 */
	GM_RSSP1_INT32 rt = GM_RSSP1_ERROR;
	GM_RSSP1_SFM_L2U_pri_struct dat_pri;
	GM_RSSP1_Write_Q_Return_Enum write_q_rt = Q_WRITE_FAIL;
	GM_RSSP1_UINT16 rcvd_src_addr = 0U;
	GM_RSSP1_UINT16 rcvd_dest_addr = 0U;
	GM_RSSP1_UINT32 CRC[GM_RSSP1_CHECK_CHN_NUM] = {0};

	if ((NULL == pSFM_conn) || (NULL == pDat_indi) || (NULL == pSFM))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Proc_Rcvd_RSD_Point_Error, pSFM_conn, pDat_indi, pSFM, 0, 0, 0);
		return;
	}

	if (GM_RSSP1_Lib_State_Operational != pSFM->lib_state)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Proc_Rcvd_RSD_Result_No_Match, pSFM->lib_state, 0, 0, 0, 0, 0);
		return;
	}

	if (pDat_indi->bytes_count > GM_RSSP1_MAX_SAFETY_DAT_LEN)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Proc_Rcvd_RSD_Byte_Error, pDat_indi->bytes_count, GM_RSSP1_MAX_SAFETY_DAT_LEN, 0, 0, 0, 0);
		GM_RSSP1_Log_Msg(0,"FSFB SFM ID: 0x%X RSD rcvd frame length %d exceed GM_RSSP1_MAX_SAFETY_DAT_LEN: %d.dat discarded!\n", pSFM_conn->SaCEPID.dest_addr , pDat_indi->bytes_count , GM_RSSP1_MAX_SAFETY_DAT_LEN , 0, 0, 0);
		return;
	}

	GM_RSSP1_memset(&dat_pri, 0xFF, sizeof(dat_pri));

	dat_pri.type = GM_RSSP1_SFM_L2U_unknow;

	rcvd_src_addr = (pDat_indi->byte[3] << 8) + pDat_indi->byte[2];
	rcvd_dest_addr = (pDat_indi->byte[5] << 8) + pDat_indi->byte[4];

	if ((GM_RSSP1_SYN_DEST_ADDR != pSFM_conn->dest_addr) && (GM_RSSP1_SYN_DEST_ADDR != rcvd_dest_addr))
	{
		/* 主备同步通道外的其他通道，检验来源/目的地址的合法性 */
		if ((rcvd_src_addr != pSFM_conn->dest_addr) || (rcvd_dest_addr != pSFM_conn->source_addr))
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Proc_Rcvd_RSD_Addr_Error, pSFM_conn->dest_addr, rcvd_src_addr, pSFM_conn->source_addr, rcvd_dest_addr, 0, 0);
			/* 收到来源/目的 */
			GM_RSSP1_Log_Msg(0,"FSFB SFM ID:0x%X,index:%d, FSFB rcv dat src/dest err!(from:0x%X,to:0x%X) err! cfg:(from:0x%X,to:0x%X).\n", pSFM_conn->SaCEPID.dest_addr, pSFM_conn->index, rcvd_src_addr, rcvd_dest_addr, pSFM_conn->dest_addr, pSFM_conn->source_addr);
			return;
		}
	}

	/* 接收到对方主和备机发送的消息，都进行验证，以保证维持fsfb连接 */
	if ((RSSP1_RSD_PROTOCOL_TYPE_ACTIVE == pDat_indi->byte[0]) || (RSSP1_RSD_PROTOCOL_TYPE_STANDBY == pDat_indi->byte[0]))
	{
		if (RSSP1_RSD_PROTOCOL_TYPE_ACTIVE == pDat_indi->byte[0])
		{
			pSFM_conn->remote_dev_AS_state = GM_RSSP1_Remote_Dev_Active;
		}
		else
		{
			pSFM_conn->remote_dev_AS_state = GM_RSSP1_Remote_DEV_Standby;
		}

 		/*FSFB_CRC16_Calc((GM_RSSP1_UINT32 *)(pSFM_conn->fsfb_chn[0].chn_cfg.pCRC16_table) , &crc16 , (GM_RSSP1_INT8 *)(pDat_indi->byte) , pDat_indi->bytes_count - 2);*/
		GM_RSSP1_CRC_Calculate_CRC16(pDat_indi->byte, pDat_indi->bytes_count - 2, &crc16, pSFM_conn->fsfb_chn[0].chn_cfg.pCRC16_table);
		rcvd_crc16 = ((pDat_indi->byte[pDat_indi->bytes_count-1]) << 8) + (pDat_indi->byte[pDat_indi->bytes_count-2]);
		if (crc16 != rcvd_crc16)
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Proc_Rcvd_RSD_CRC16_Error, 0, 0, 0, 0, 0, 0);
			GM_RSSP1_SFM_Report_Warning_To_User(pSFM_conn , pSFM , GM_RSSP1_SFM_WARNING_TYPE_RCVD_RSD_CRC_ERROR);
			GM_RSSP1_Log_Msg(0,"FSFB SFM ID: 0x%X rcvd RSD frame CRC16 err!recv crc:%X,crc:%X\n", pSFM_conn->SaCEPID.dest_addr ,rcvd_crc16, crc16, 0, 0, 0);
			return;
		}
		else
		{
			/* crc check OK */
			if ((RSSP1_RSD_PROTOCOL_TYPE_STANDBY == pDat_indi->byte[0]) || (GM_RSSP1_FALSE == SYS_Is_Local_Sys_Active()))
			{
				pSFM_conn->state = GM_RSSP1_Layer_State_Connected;
				return;
			}
		}

		if (pSFM_conn->state == GM_RSSP1_Layer_State_Connected)
		{
			GM_RSSP1_Log_Msg(0,"FSFB SFM CON_%d: state connected.\n", pSFM_conn->index, 0, 0, 0, 0, 0);

			rcvd_app_dat_len = pDat_indi->bytes_count - 22;
			app_dat_len_in_pkt = (pDat_indi->byte[10]) + ((pDat_indi->byte[11]) << 8) - 8 ;
			if (rcvd_app_dat_len != app_dat_len_in_pkt)
			{
				GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Proc_Rcvd_RSD_Byte_Error, rcvd_app_dat_len, app_dat_len_in_pkt, 0, 0, 0, 0);
				GM_RSSP1_Log_Msg(0,"FSFB SFM ID: 0x%X rcvd RSD frame length err! SDS: %d , actual: %d.\n", pSFM_conn->SaCEPID.dest_addr , app_dat_len_in_pkt, rcvd_app_dat_len, 0, 0, 0);
				return;
			}

			/* 校验前 , 计算RSD内应用数据的明文checksum，保存到dat_pri，防止校验过程中改变应用数据 */
			rcvd_TC = (pDat_indi->byte[6]) + ((pDat_indi->byte[7]) << 8) + ((pDat_indi->byte[8]) << 16) + ((pDat_indi->byte[9]) << 24);
			rcvd_crcm[0] = (pDat_indi->byte[12]) + ((pDat_indi->byte[13]) << 8) + ((pDat_indi->byte[14]) << 16) + ((pDat_indi->byte[15]) << 24);
			rcvd_crcm[1] = (pDat_indi->byte[16]) + ((pDat_indi->byte[17]) << 8) + ((pDat_indi->byte[18]) << 16) + ((pDat_indi->byte[19]) << 24);
			
			for (chn_index = 0 ; chn_index < GM_RSSP1_CHECK_CHN_NUM ; ++chn_index)
			{
				/*FSFB_CRC32_Calc((GM_RSSP1_UINT32 *)(pSFM_conn->fsfb_chn[chn_index].chn_cfg.pCRC32_table) , &(rcvd_crc32[chn_index]) , (GM_RSSP1_INT8 *)(pDat_indi->byte + 20) , rcvd_app_dat_len);*/
				GM_RSSP1_CRC_Calculate_CRC32(pDat_indi->byte + 20, rcvd_app_dat_len, &(rcvd_crc32[chn_index]), (pSFM_conn->fsfb_chn[chn_index].chn_cfg.pCRC32_table));
				rt = GM_RSSP1_CRCM_CHN_CHECK_Int(
					rcvd_crcm[chn_index],
					rcvd_crc32[chn_index],
                    (~(pSFM_conn->fsfb_chn[chn_index].chn_cfg.local_sys_chk)),
					(GM_RSSP1_UINT32 *)(pSFM_conn->fsfb_chn[chn_index].chn_cfg.pLFSR_left_table),
					(GM_RSSP1_UINT32 *)(pSFM_conn->fsfb_chn[chn_index].chn_cfg.pLFSR_right_table),
					rcvd_TC,
					pSFM_conn->env[chn_index].TC,
					&(pSFM_conn->env[chn_index].local_TC),
					&(pSFM_conn->env[chn_index].remote_TC),
					&(pSFM_conn->env[chn_index].dynamicKey),
					(GM_RSSP1_UINT32 *)(pSFM_conn->fsfb_chn[chn_index].PREC_SINIT),
					(GM_RSSP1_UINT32 *)(pSFM_conn->fsfb_chn[chn_index].POST_RXDATA),
					pSFM_conn->deltaTime,
					pSFM_conn->DelayTime,
					&(CRC[chn_index])
#ifdef OFFLINE_TOOL_SUPPORT
					,
					&(pSFM_conn->env[chn_index].dynLocKey),
					(GM_RSSP1_UINT32 *)(pSFM_conn->fsfb_chn[chn_index].CON_1),
					(GM_RSSP1_UINT32 *)(pSFM_conn->fsfb_chn[chn_index].CON_2),
					(GM_RSSP1_UINT32 *)(pSFM_conn->fsfb_chn[chn_index].CON_3),
					chn_index
#endif
				);

				if (GM_RSSP1_ERROR == rt)
				{
					GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Proc_Rcvd_RSD_Check_Error, 0, 0, 0, 0, 0, 0);
					GM_RSSP1_SFM_Report_Warning_To_User(pSFM_conn , pSFM , GM_RSSP1_SFM_WARNING_TYPE_FSFB_CRCM_CHECK_ERR);
					/*
					*/
					/*GM_RSSP1_Log_Msg(0,"...........FSFB SFM ID:0x%X RSD index %d check err! rmtTC:%d. msgTC:%d.(chk err:%d).\n", pSFM_conn->SaCEPID.dest_addr, pSFM_conn->index, pSFM_conn->env[chn_index].remote_TC, rcvd_TC, pSFM_conn->_dbg_fsfb_check_err_num, 0);*/
					break;
				}
				else if(GM_RSSP1_RETURN_RESERVED == rt)
				{
					return;
				}
				else
				{
				}
			}
		#ifdef WIN32
			GM_RSSP1_Log_Msg(0,"<<<<<<<<<<<<<RSD: rcvd_TC=%d, RCVD_CRCM=%x, RCVD_CRC32=%x RCVD_CRCM_1=%x, rcvd_crc32_1=%x\n", rcvd_TC, rcvd_crcm[0], rcvd_crc32[0],rcvd_crcm[1], rcvd_crc32[1], 0);
		#endif
		}

		if (GM_RSSP1_OK != rt)
		{
			pSFM_conn->state = GM_RSSP1_Layer_State_Free;

			/* reset tolerate vars */
			pSFM_conn->b_rcvd_new_dat_in_cycle = GM_RSSP1_FALSE;
			GM_RSSP1_memset(&(pSFM_conn->last_valid_dat_pri) , 0 , sizeof(pSFM_conn->last_valid_dat_pri));
			pSFM_conn->b_torlerant_dat_valid = GM_RSSP1_FALSE;

			/* FSFB check fail,send SSE */
			GM_RSSP1_SFM_Send_SSE(pSFM_conn , pSFM);
		}
		else
		{
#ifdef WIN32
			//printf("/////////////////////////////RSD check corrcet!\n");
#endif
			pSFM_conn->b_rcvd_new_dat_in_cycle = GM_RSSP1_TRUE;
			pSFM_conn->b_torlerant_dat_valid = GM_RSSP1_TRUE;
			pSFM_conn->torlerate_cycle_count1 = 0;
			pSFM_conn->torlerate_cycle_count2 = (GM_RSSP1_INT8)pSFM_conn->torlerate_cycle_cfg;
			pSFM_conn->state_check_counter1 = 0;
			pSFM_conn->state_check_counter2 = (GM_RSSP1_INT8)pSFM_conn->keepIdle; /*CR:GM3943 maintian alignment*/
			
			pSFM_conn->SINIT_TremCycle = rcvd_TC;	/*计算preciTime用，added by huang 20151125*/
			dat_pri.type = GM_RSSP1_SFM_Data_Ind;

			/* 地址使用外部收到的信息，防止匹配错误 jianghongjun 20110825 safety enhancement */
			dat_pri.SaCEPID.source_addr = rcvd_dest_addr;
			dat_pri.SaCEPID.dest_addr = rcvd_src_addr;

			GM_RSSP1_SetPreciTime(pSFM_conn);

			/*CR:3894 update prectime and support together with message*/
			#ifdef GM_RSSP1_RxMsgWithTime
			dat_pri.as.data_indi.bytes_count = (GM_RSSP1_UINT16)rcvd_app_dat_len+8;
			GM_RSSP1_memcpy(dat_pri.as.data_indi.byte, &(pSFM_conn->PreciTime), 4);
			GM_RSSP1_memcpy(dat_pri.as.data_indi.byte+4, &(pSFM_conn->SSE_SSR_Delay), 4);
			GM_RSSP1_memcpy(dat_pri.as.data_indi.byte+8, pDat_indi->byte + 20, rcvd_app_dat_len);
			#else
			dat_pri.as.data_indi.bytes_count = (GM_RSSP1_UINT16)rcvd_app_dat_len;
			GM_RSSP1_memcpy(dat_pri.as.data_indi.byte, pDat_indi->byte + 20, rcvd_app_dat_len);
			#endif
			GM_RSSP1_memcpy(dat_pri.as.data_indi.CRC, CRC, sizeof(GM_RSSP1_UINT32)* 2);

			GM_RSSP1_memcpy(&pSFM_conn->last_valid_dat_pri, &dat_pri, sizeof(GM_RSSP1_SFM_L2U_pri_struct));
			if (GM_RSSP1_TRUE == pSFM_conn->b_enable_FSFB_on_chn)
			{
				/* 直接储存到本通道内的队列 */
				write_q_rt = FSFB_Msg_Queue_Write(&(pSFM_conn->L2U_dat_Q) , (void*)&dat_pri);
			}
			else
			{
				/* 存储到SFM的队列 */
				write_q_rt = FSFB_Msg_Queue_Write(&(g_SFM_MQ.RCV_MQ/*pSFM->L2U_pri_Q*/) , (void*)&dat_pri);
			}

			if (Q_WRITE_OK_FULL == write_q_rt)
			{
				GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Proc_Rcvd_RSD_Write_Full, 0, 0, 0, 0, 0, 0);
				GM_RSSP1_Log_Msg(0,"FSFB SFM object/SFM_connection L2U_pri_Q full!SFM full:%d,SFM conn full:%d.con ID:0x%X.\n", FSFB_Is_Msg_Queue_Full(&(g_SFM_MQ.RCV_MQ/*pSFM->L2U_pri_Q*/)), FSFB_Is_Msg_Queue_Full(&(pSFM_conn->L2U_dat_Q)), pSFM_conn->SaCEPID.source_addr, 0, 0, 0);
			}
			else if (Q_WRITE_FAIL == write_q_rt)
			{
				GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Proc_Rcvd_RSD_Write_Error, 0, 0, 0, 0, 0, 0);
				pSFM->lib_state = GM_RSSP1_Lib_State_Failure;
			}
			else
			{
			}
		}

	}
	else
	{
		pSFM_conn->remote_dev_AS_state = GM_RSSP1_Remote_DEV_Unknow;
		/* unkown Active standby flag */
		GM_RSSP1_Log_Msg(0,"FSFB SFM ID:0x%X, remote dev AS state flag:0x%X err!\n", pSFM_conn->SaCEPID.dest_addr, pDat_indi->byte[0], 0, 0, 0, 0);
		/* reset tolerate vars */
		pSFM_conn->b_rcvd_new_dat_in_cycle = GM_RSSP1_FALSE;
		GM_RSSP1_memset(&(pSFM_conn->last_valid_dat_pri) , 0 , sizeof(GM_RSSP1_SFM_L2U_pri_struct));
		pSFM_conn->b_torlerant_dat_valid = GM_RSSP1_FALSE;
	}

	return;
}

void GM_RSSP1_SFM_Reset_Connection_RSD_Sent_Flag(GM_RSSP1_SFM_object_struct *pSFM)
{
	GM_RSSP1_INT32 con_index = 0;
	GM_RSSP1_SFM_connection_struct *pSFM_conn = NULL;

	if (pSFM == NULL)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Reset_Connection_RSD_Sent_Flag_Point_Error, 0, 0, 0, 0, 0, 0);
		return;
	}

	for (con_index = 0 ; con_index < pSFM->connection_nums ; ++con_index)
	{
		pSFM_conn = &(pSFM->connection[con_index]);
		pSFM_conn->b_RSD_Already_Sent = GM_RSSP1_FALSE;
	}

	return;
}


void GM_RSSP1_SFM_Process_User_Req(GM_RSSP1_SFM_object_struct *pSFM)
{
	GM_RSSP1_SFM_U2L_pri_struct pri = {0};
	GM_RSSP1_SFM_connection_struct *pSFM_conn = NULL;
	GM_RSSP1_INT32 rt = GM_RSSP1_ERROR;
	GM_RSSP1_INT32 con_index = 0;

	if (NULL == pSFM)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Process_User_Req_Point_Error, 0, 0, 0, 0, 0, 0);
		return;
	}

	if (GM_RSSP1_Lib_State_Operational != pSFM->lib_state)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Process_User_Req_Result_No_Match, pSFM->lib_state, 0, 0, 0, 0, 0);
		return;
	}

	while (GM_RSSP1_TRUE == FSFB_Msg_Queue_Get(&(g_SFM_MQ.SND_MQ) , &pri))
	{
		for (con_index = 0 ; con_index < pSFM->connection_nums ; ++con_index)
		{
			pSFM_conn = &(pSFM->connection[con_index]);

			if (GM_RSSP1_TRUE == GM_RSSP1_SFM_Is_SaCEPID_Equal(&(pri.SaCEPID) , &(pSFM_conn->SaCEPID)))
			{
				/* 与目的sacepid相同的所有连接都发送数据(同步通道除外)
				* UDP apply FSFB chn时，因为应用层只关心对等的应用层，各FSFB安全连接的SACEPID将不可避免地相同 */
				/* 该数据的远端接收者存在 */
				switch (pri.type)
				{

				case GM_RSSP1_Pri_Type_SFM_Data_req:
					/* send FSFB RSD dat */

					//if (GM_RSSP1_FALSE == pSFM_conn->b_RSD_Already_Sent) //modified by Alan
					{
						rt = GM_RSSP1_SFM_Send_RSD(&pri , pSFM_conn , pSFM);
						RSD_Send_Flag = GM_RSSP1_TRUE;
						pSFM_conn->b_RSD_Already_Sent = GM_RSSP1_TRUE;
					}
					//else
					{

					}

					break;

				default:
					break;
				}
			}
		}
	}

	return ;
}

void GM_RSSP1_SFM_Report_Connection_State_To_User(GM_RSSP1_SFM_object_struct *pSFM , GM_RSSP1_CFM_L2U_pri_struct *pIndi_pri)
{
	GM_RSSP1_INT32 con_index = 0;
	GM_RSSP1_SFM_connection_struct *pSFM_con = NULL;
	GM_RSSP1_SFM_L2U_pri_struct pri = {0};
	GM_RSSP1_Write_Q_Return_Enum write_q_rt = Q_WRITE_FAIL;

	if ((NULL == pIndi_pri) || (NULL == pSFM))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Report_Connection_State_To_User_Point_Error, pIndi_pri, pSFM, 0, 0, 0, 0);
		return;
	}

	if (GM_RSSP1_Lib_State_Operational != pSFM->lib_state)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Report_Connection_State_To_User_Result_No_Match, pSFM->lib_state, 0, 0, 0, 0, 0);
		return;
	}

	pri.type = GM_RSSP1_SFM_Connection_State_Indi;
	pri.as.connection_state.connection_num = pSFM->connection_nums;

	for (con_index = 0 ; con_index < pSFM->connection_nums ; ++con_index)
	{
		pSFM_con = &(pSFM->connection[con_index]);
		pri.as.connection_state.connection_states[con_index].sacepid = pSFM_con->SaCEPID;
		pri.as.connection_state.connection_states[con_index].state = 0;

		if (con_index != pIndi_pri->as.channel_state_indi.GM_RSSP1_CFM_Channel_states[con_index].index)
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Report_Connection_State_To_User_Index_No_Match, con_index, 0, 0, 0, 0, 0);
			GM_RSSP1_Log_Msg(0,"FSFB SFM connection state report:connection INDEX err!\n", 0, 0, 0, 0, 0, 0);
			return;
		}

		/* fsfb state:  4 bits */
		if (GM_RSSP1_Layer_State_Connected == pSFM_con->state)
		{
			pri.as.connection_state.connection_states[con_index].state |= (GM_RSSP1_REPORT_STATE_TO_APP_FSFB_OK << 28);
		}
		else
		{
			pri.as.connection_state.connection_states[con_index].state |= (GM_RSSP1_REPORT_STATE_TO_APP_FSFB_FAIL << 28);
		}

		/* remote active-standby state:  4 bits */
		if (GM_RSSP1_Remote_Dev_Active == pSFM_con->remote_dev_AS_state)
		{
			pri.as.connection_state.connection_states[con_index].state |= (GM_RSSP1_REPORT_STATE_TO_APP_REMOTE_ACTIVE << 24);
		}
		else if (GM_RSSP1_Remote_DEV_Standby == pSFM_con->remote_dev_AS_state)
		{
			pri.as.connection_state.connection_states[con_index].state |= (GM_RSSP1_REPORT_STATE_TO_APP_REMOTE_STANDBY << 24);
		}
		else
		{
			pri.as.connection_state.connection_states[con_index].state |= (GM_RSSP1_REPORT_STATE_TO_APP_REMOTE_UNKOWN << 24);
		}

		/* UDP chn nums:    4 bits */
		pri.as.connection_state.connection_states[con_index].state |= (pIndi_pri->as.channel_state_indi.GM_RSSP1_CFM_Channel_states[con_index].chn_num << 20);

		/* UDP chn state:    8 bits */
		pri.as.connection_state.connection_states[con_index].state |= (pIndi_pri->as.channel_state_indi.GM_RSSP1_CFM_Channel_states[con_index].channel_state << 12);

		/* reserved:    12 bits: */
	}

	write_q_rt = FSFB_Msg_Queue_Write(&(g_SFM_MQ.RCV_MQ/*pSFM->L2U_pri_Q*/) , &pri);

	if (Q_WRITE_OK_FULL == write_q_rt)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Report_Connection_State_To_User_Write_Full, 0, 0, 0, 0, 0, 0);
		GM_RSSP1_Log_Msg(0,"FSFB SFM L2U_pri_Q full , report FSFB connection state fail!\n", 0, 0, 0, 0, 0, 0);
	}
	else if (Q_WRITE_FAIL == write_q_rt)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Report_Connection_State_To_User_Write_Fail, 0, 0, 0, 0, 0, 0);
		pSFM->lib_state = GM_RSSP1_Lib_State_Failure;
		return;
	}
	else
	{
	}

	return;
}

void GM_RSSP1_SFM_Report_Warning_To_User(GM_RSSP1_SFM_connection_struct *pSFM_con , GM_RSSP1_SFM_object_struct *pSFM , GM_RSSP1_UINT8 warning_type)
{
	GM_RSSP1_SFM_L2U_pri_struct pri = {0};
	GM_RSSP1_Write_Q_Return_Enum write_q_rt = Q_WRITE_FAIL;

	if ((NULL == pSFM_con) || (NULL == pSFM))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Report_Warning_To_User_Point_Error, pSFM_con, pSFM, 0, 0, 0, 0);
		return ;
	}

	if (GM_RSSP1_Lib_State_Operational != pSFM->lib_state)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Report_Warning_To_User_Result_No_Match, pSFM->lib_state, 0, 0, 0, 0, 0);
		return ;
	}

	pri.type = GM_RSSP1_SFM_Warning_Indi;

	pri.SaCEPID = pSFM_con->SaCEPID;
	pri.as.warning.type = warning_type;
	pri.as.warning.index = pSFM_con->index;
	pri.as.warning.SaCEPID = pSFM_con->SaCEPID;

	write_q_rt = FSFB_Msg_Queue_Write(&(g_SFM_MQ.RCV_MQ/*pSFM->L2U_pri_Q*/) , &pri);

	if (Q_WRITE_OK_FULL == write_q_rt)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Report_Warning_To_User_Write_Full, 0, 0, 0, 0, 0, 0);
		GM_RSSP1_Log_Msg(0,"FSFB SFM L2U_pri_Q full , report FSFB WARNING info fail!\n", 0, 0, 0, 0, 0, 0);
	}
	else if (Q_WRITE_FAIL == write_q_rt)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Report_Warning_To_User_Write_Fail, 0, 0, 0, 0, 0, 0);
		pSFM->lib_state = GM_RSSP1_Lib_State_Failure;
		return;
	}
	else
	{
	}

	return;
}

void GM_RSSP1_SFM_Process_CFM_Ind(GM_RSSP1_SFM_object_struct *pSFM)
{
	GM_RSSP1_INT32 con_index = 0;
	GM_RSSP1_CFM_L2U_pri_struct cfm_ind = {0};
	GM_RSSP1_SFM_connection_struct *pSFM_conn = NULL;

	if ((pSFM == NULL)||(GM_RSSP1_Lib_State_Operational != pSFM->lib_state))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Process_CFM_Ind_Result_No_Match, 0, 0, 0, 0, 0, 0);
		return ;
	}

	/* reset tolerate vars */
	for (con_index = 0 ; con_index  < pSFM->connection_nums ; ++con_index)
	{
		pSFM_conn = &(pSFM->connection[con_index]);
		pSFM_conn->b_rcvd_new_dat_in_cycle = GM_RSSP1_FALSE;
	}

	while (GM_RSSP1_SFM_Get_Pri_OK == GM_RSSP1_SFM_Get_Pri_From_CFM(&cfm_ind))
	{
		if (cfm_ind.index < pSFM->connection_nums)
		{
			pSFM_conn   = &(pSFM->connection[cfm_ind.index]);

			switch (cfm_ind.type)
			{

			case GM_RSSP1_CFM_Data_Ind:
				if (GM_RSSP1_SYN_DEST_ADDR == pSFM_conn->SaCEPID.dest_addr)
				{
					/* 本通道id为同步通道,则从此处将数据拦截，直接作为同步数据处理，而不再送向上层 */
                    GM_RSSP1_Syn_Proc_Rcvd_Syn_Dat(cfm_ind.as.data_indi.byte , cfm_ind.as.data_indi.bytes_count);
				}
				else
				{
					if (cfm_ind.as.data_indi.byte[1] == RSSP1_SSE_FRAME_TYPE)
					{
						GM_RSSP1_Log_Msg(0,"FSFB SFM ID:0x%X <---- rcvd SSE.%d bytes.(%d.%d),index:%d.\n", pSFM_conn->SaCEPID.dest_addr , cfm_ind.as.data_indi.bytes_count, pSFM_conn->env[0].local_TC, pSFM_conn->env[0].remote_TC, pSFM_conn->index, 0);
						GM_RSSP1_SFM_Proc_Rcvd_SSE(pSFM_conn , &(cfm_ind.as.data_indi) , pSFM);
					}
					else if (cfm_ind.as.data_indi.byte[1] == RSSP1_SSR_FRAME_TYPE)
					{
						GM_RSSP1_Log_Msg(0,"FSFB SFM ID:0x%X <------ rcvd SSR.%d bytes.(%d.%d),index:%d.\n", pSFM_conn->SaCEPID.dest_addr , cfm_ind.as.data_indi.bytes_count, pSFM_conn->env[0].local_TC, pSFM_conn->env[0].remote_TC, pSFM_conn->index, 0);
						GM_RSSP1_SFM_Proc_Rcvd_SSR(pSFM_conn , &(cfm_ind.as.data_indi) , pSFM);
					}
					else if ((cfm_ind.as.data_indi.byte[1] == RSSP1_RSD_FRAME_TYPE_FROM_A) || (cfm_ind.as.data_indi.byte[1] == RSSP1_RSD_FRAME_TYPE_FROM_B))
					{
						GM_RSSP1_Log_Msg(0,"FSFB SFM ID:0x%X <-------- rcvd RSD.%d bytes.(%d.%d),index:%d.\n", pSFM_conn->SaCEPID.dest_addr , cfm_ind.as.data_indi.bytes_count, pSFM_conn->env[0].local_TC, pSFM_conn->env[0].remote_TC, pSFM_conn->index, 0);
						GM_RSSP1_SFM_Proc_Rcvd_RSD(pSFM_conn , &(cfm_ind.as.data_indi) , pSFM);
					}
					else
					{
						GM_RSSP1_SFM_Report_Warning_To_User(pSFM_conn , pSFM , GM_RSSP1_SFM_WARNING_TYPE_RCVD_UNKNOW_TYPE_FRAME);
						GM_RSSP1_Log_Msg(0,"FSFB SFM ID:0x%X <---- rcvd unknow type frame. %d bytes.\n", pSFM_conn->SaCEPID.dest_addr , cfm_ind.as.data_indi.bytes_count, 0, 0, 0, 0);
					}
				}

				break;

			case GM_RSSP1_CFM_Indi_com_State:
				/* collect CFM udp channel state , report state to USER with FSFB state. */
				GM_RSSP1_SFM_Report_Connection_State_To_User(pSFM , &cfm_ind);
				break;

			default:
				break;
			}
		}
	}
}

void GM_RSSP1_SFM_Proc_Connection_Monit_And_Tolerate(GM_RSSP1_SFM_object_struct *pSFM)
{
	GM_RSSP1_UINT8 con_index = 0U;
	GM_RSSP1_SFM_connection_struct *pSFM_con = NULL;
	GM_RSSP1_Write_Q_Return_Enum write_q_rt = Q_WRITE_FAIL;
#ifdef OFFLINE_TOOL_SUPPORT
	GM_RSSP1_SFM_L2U_pri_struct pri = {0U};
	GM_RSSP1_UINT32 CRCM[GM_RSSP1_CHECK_CHN_NUM] = {0U};
	GM_RSSP1_UINT32 VSN0 = 0U;
	GM_RSSP1_UINT32 VSN1 = 0U;
	GM_RSSP1_UINT32 VSN2 = 0U;
	GM_RSSP1_LFSR reg = {0};
	GM_RSSP1_UINT8 i = 0U;
	GM_RSSP1_UINT8 j = 0U;
#endif

	if ((NULL == pSFM)||(GM_RSSP1_Lib_State_Operational != pSFM->lib_state))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Proc_Connection_Monit_And_Tolerate_Result_No_Match, 0, 0, 0, 0, 0, 0);
		return;
	}

	for (con_index = 0 ; con_index < pSFM->connection_nums ; ++con_index)
	{
		pSFM_con = &(pSFM->connection[con_index]);

		++(pSFM_con->state_check_counter1);
		--(pSFM_con->state_check_counter2);

		/* 通过‘或’条件，用于防止当一个计数器失效时，无法进入处理分支的问题 */
		/*CR:GM3943 maintian alignment*/
		if ((pSFM_con->state_check_counter1 > (GM_RSSP1_INT8)(pSFM_con->keepIdle)) || (pSFM_con->state_check_counter2 < 0))
		{
			if(GM_RSSP1_Layer_State_Connected == pSFM_con->state)
			{
				GM_RSSP1_SFM_Report_Warning_To_User(pSFM_con , pSFM , GM_RSSP1_SFM_WARNING_TYPE_RECV_NO_RSD_IN_DELTATIME);
			}
			pSFM_con->state_check_counter1 = 0;
			pSFM_con->state_check_counter2 = (GM_RSSP1_INT8)pSFM_con->keepIdle;
			pSFM_con->b_rcvd_new_dat_in_cycle = GM_RSSP1_FALSE;
			pSFM_con->state = GM_RSSP1_Layer_State_Free;
			/* start:CR:GM00002529, GM162, fulintai,2013-06-09, reset the AS status, otherwise it would conflict inside when remote switch AS*/
			pSFM_con->remote_dev_AS_state = GM_RSSP1_Remote_DEV_Unknow;
			/* end CR:GM00002529*/
		}

		/* proc tolerate cycle */
		/*CR:3894 update prectime and support together with message*/
		if (GM_RSSP1_FALSE == pSFM_con->b_rcvd_new_dat_in_cycle)
		{
			++(pSFM_con->torlerate_cycle_count1);
			--(pSFM_con->torlerate_cycle_count2);
			if ((GM_RSSP1_Layer_State_Connected == pSFM_con->state)
				&& (pSFM_con->torlerate_cycle_count1 < pSFM_con->torlerate_cycle_cfg)
				&& (pSFM_con->torlerate_cycle_count2 > 0)
				&& (GM_RSSP1_TRUE == pSFM_con->b_torlerant_dat_valid))
			{
				GM_RSSP1_SetPreciTime(pSFM_con);

			#ifdef OFFLINE_TOOL_SUPPORT
				GM_RSSP1_memset((void*)(&pri), 0, sizeof(GM_RSSP1_SFM_L2U_pri_struct));
				GM_RSSP1_memcpy((void*)(&pri), (void*)(&(pSFM_con->last_valid_dat_pri)), sizeof(GM_RSSP1_SFM_L2U_pri_struct));
				GM_RSSP1_VSN_Get_Callback_Fun(&VSN0, &VSN1, &VSN2);

				for (i=0U; i<GM_RSSP1_CHECK_CHN_NUM; ++i)
				{
					GM_RSSP1_LFSR_Init(&reg, pSFM_con->fsfb_chn[i].chn_cfg.pLFSR_left_table, pSFM_con->fsfb_chn[i].chn_cfg.pLFSR_right_table);
					GM_RSSP1_LFSR_Load(&reg, pSFM_con->env[i].dynLocKey);
					for (j=0U; j<pSFM_con->torlerate_cycle_count1; ++j)
					{
						GM_RSSP1_LFSR_Add(&reg, 0U);						
					}
					if(GM_RSSP1_FALSE == GM_RSSP1_LFSR_Read(&reg , &CRCM[i]))
					{
						return;
					}
					pri.as.data_indi.CRC[i] ^= pSFM_con->env[i].dynLocKey ^ pSFM_con->fsfb_chn[i].CON_4[pSFM_con->torlerate_cycle_count1-1] ^ CRCM[i];
				}

				if (GM_RSSP1_TRUE == pSFM_con->b_enable_FSFB_on_chn)
				{
					/* 直接储存到本通道内的队列 */
					write_q_rt = FSFB_Msg_Queue_Write(&(pSFM_con->L2U_dat_Q) , (void*)(&pri));
				}
				else
				{
					/* 存储到SFM的队列 */
					write_q_rt = FSFB_Msg_Queue_Write(&(g_SFM_MQ.RCV_MQ) , (void*)(&pri));
				}
			#else
				#ifdef GM_RSSP1_RxMsgWithTime
				GM_RSSP1_memcpy(pSFM_con->last_valid_dat_pri.as.data_indi.byte, &(pSFM_con->PreciTime), 4);
				GM_RSSP1_memcpy(pSFM_con->last_valid_dat_pri.as.data_indi.byte+4, &(pSFM_con->SSE_SSR_Delay), 4);
				#endif
				/* rcv nothing this cycle , but FSFB still connected , tolerate */
				if (GM_RSSP1_TRUE == pSFM_con->b_enable_FSFB_on_chn)
				{
					/* 直接储存到本通道内的队列 */
					write_q_rt = FSFB_Msg_Queue_Write(&(pSFM_con->L2U_dat_Q) , (void*)(&(pSFM_con->last_valid_dat_pri)));
				}
				else
				{
					/* 存储到SFM的队列 */
					write_q_rt = FSFB_Msg_Queue_Write(&(g_SFM_MQ.RCV_MQ) , (void*)(&(pSFM_con->last_valid_dat_pri)));
				}
			#endif

				if (Q_WRITE_OK_FULL == write_q_rt)
				{
					GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Proc_Connection_Monit_And_Tolerate_Write_Full, 0, 0, 0, 0, 0, 0);
					GM_RSSP1_Log_Msg(0, "FSFB SFM object/SFM_connection L2U_pri_Q full!\n", 0, 0, 0, 0, 0, 0);
				}
				else if (Q_WRITE_FAIL == write_q_rt)
				{
					GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Proc_Connection_Monit_And_Tolerate_Write_Fail, 0, 0, 0, 0, 0, 0);
					pSFM->lib_state = GM_RSSP1_Lib_State_Failure;
				}
				else
				{
				}
			}
			else
			{
				pSFM_con->b_torlerant_dat_valid = GM_RSSP1_FALSE;
				pSFM_con->torlerate_cycle_count1 = 0;
				pSFM_con->torlerate_cycle_count2 = pSFM_con->torlerate_cycle_cfg;
				GM_RSSP1_memset(&(pSFM_con->last_valid_dat_pri) , 0 , sizeof(GM_RSSP1_SFM_L2U_pri_struct));
			}
		}
	}
}

void GM_RSSP1_SFM_Dispatch_Dat_Indi(GM_RSSP1_SFM_object_struct *pSFM)
{
	GM_RSSP1_INT32 con_index = 0;
	GM_RSSP1_INT32 i = 0;
	GM_RSSP1_INT32 j = 0;
	GM_RSSP1_INT32 k = 0;
	GM_RSSP1_SFM_connection_struct *pSFM_con = NULL;
	GM_RSSP1_SFM_connection_struct *pSFM_other_con = NULL;
	GM_RSSP1_SFM_connection_struct *pSFM_best_con = NULL;
	GM_RSSP1_SFM_L2U_pri_struct dat_pri = {0};
	GM_RSSP1_Write_Q_Return_Enum write_q_rt = Q_WRITE_FAIL;
	GM_RSSP1_BOOL b_got_pri = GM_RSSP1_FALSE;
	GM_RSSP1_BOOL b_got_best_pri = GM_RSSP1_FALSE;

	if ((NULL == pSFM)||(GM_RSSP1_Lib_State_Operational != pSFM->lib_state))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Dispatch_Dat_Indi_Result_No_Match, 0, 0, 0, 0, 0, 0);
		return;
	}

	for (con_index = 0 ; con_index < pSFM->connection_nums ; ++con_index)
	{
		pSFM_con = &(pSFM->connection[con_index]);
		pSFM_best_con = NULL;
		b_got_best_pri = GM_RSSP1_FALSE;

		if (GM_RSSP1_TRUE == pSFM_con->b_enable_FSFB_on_chn)
		{
			/* @NOTE: 启用b_enable_FSFB_on_chn选项UDP通道的FSFB连接，CFM将数据先送到相应FSFB连接中的L2U队列，Dispatch后才送到SFM中的L2U队列
			* 正常的FSFB连接，CFM处理完的数据将直接送到SFM的L2U队列。 */

			b_got_pri = GM_RSSP1_FALSE;

			for (i = 0 ; i < pSFM->connection_nums ; ++i)
			{
				/* 如果UDP走独立通道时，应该优先使用接收到新数据的连接，而不是没收到数据，经过宽恕的连接 */
				if (i == pSFM_con->index)
				{
					continue;
				}

				pSFM_other_con = &(pSFM->connection[i]);

				if ((GM_RSSP1_TRUE == pSFM_other_con->b_enable_FSFB_on_chn)
					&& (pSFM_other_con->Chn_apply_FSFB_ID == pSFM_con->Chn_apply_FSFB_ID)
					&& (GM_RSSP1_TRUE == pSFM_other_con->b_rcvd_new_dat_in_cycle)
					&& (GM_RSSP1_TRUE == GM_RSSP1_SFM_Is_SaCEPID_Equal(&(pSFM_other_con->SaCEPID) , &(pSFM_con->SaCEPID))))
				{
					while (GM_RSSP1_TRUE == FSFB_Msg_Queue_Get(&(pSFM_other_con->L2U_dat_Q) , &dat_pri))
					{
						b_got_pri = GM_RSSP1_TRUE;
						pSFM_best_con = pSFM_other_con;

						write_q_rt = FSFB_Msg_Queue_Write(&(g_SFM_MQ.RCV_MQ/*pSFM->L2U_pri_Q*/) , &dat_pri);

						if (Q_WRITE_OK_FULL == write_q_rt)
						{
							GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Dispatch_Dat_Indi_Write_Full, 0, 0, 0, 0, 0, 0);
							GM_RSSP1_Log_Msg(0,"FSFB SFM object L2U_pri_Q full!\n", 0, 0, 0, 0, 0, 0);
						}
						else if (Q_WRITE_FAIL == write_q_rt)
						{
							GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Dispatch_Dat_Indi_Write_Fail, 0, 0, 0, 0, 0, 0);
							pSFM->lib_state = GM_RSSP1_Lib_State_Failure;
							return;
						}
						else
						{
						}
					}

					if (GM_RSSP1_TRUE == b_got_pri)
					{
						/* b_rcvd_new_dat_in_cycle为真时，逻辑上不可能发生。一定可以提取到最优数据 */
						b_got_pri = GM_RSSP1_FALSE;
						b_got_best_pri = GM_RSSP1_TRUE;

						/* 其他平行连接中提取到最优包：清空其他平行连接 */

						for (j = 0 ; j < pSFM->connection_nums ; ++j)
						{
							if (j == pSFM_best_con->index)
							{
								continue;
							}

							pSFM_other_con = &(pSFM->connection[j]);

							if ((GM_RSSP1_TRUE == pSFM_other_con->b_enable_FSFB_on_chn) && (pSFM_other_con->Chn_apply_FSFB_ID == pSFM_best_con->Chn_apply_FSFB_ID))
							{
								FSFB_Msg_Queue_Clear(&(pSFM_other_con->L2U_dat_Q));
							}
						}
					}
				}
			}

			if (GM_RSSP1_FALSE == b_got_best_pri)
			{
				b_got_pri = GM_RSSP1_FALSE;

				/* 其他平行连接中未提取到更优的数据:从前至后，提取次优的经过宽恕的数据 */
				while (GM_RSSP1_TRUE == FSFB_Msg_Queue_Get(&(pSFM_con->L2U_dat_Q) , &dat_pri))
				{
					b_got_pri = GM_RSSP1_TRUE;

					write_q_rt = FSFB_Msg_Queue_Write(&(g_SFM_MQ.RCV_MQ/*pSFM->L2U_pri_Q*/) , &dat_pri);

					if (Q_WRITE_OK_FULL == write_q_rt)
					{
						GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Dispatch_Dat_Indi_Write_Full, 0, 0, 0, 0, 0, 0);
						pSFM->lib_state = GM_RSSP1_Lib_State_Operational;
						GM_RSSP1_Log_Msg(0,"FSFB SFM object L2U_pri_Q full!\n", 0, 0, 0, 0, 0, 0);
					}
					else if (Q_WRITE_FAIL == write_q_rt)
					{
						GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Dispatch_Dat_Indi_Write_Fail, 0, 0, 0, 0, 0, 0);
						pSFM->lib_state = GM_RSSP1_Lib_State_Failure;
						return;
					}
					else
					{
					}
				}

				if (GM_RSSP1_TRUE == b_got_pri)
				{
					b_got_pri = GM_RSSP1_FALSE;
					/* 对于Chn_apply_FSFB_ID相同的各个安全连接，只要其中一个提取到数据，就将其他连接的L2U队列清空 */

					for (k = 0 ; k < pSFM->connection_nums ; ++k)
					{
						if (k == pSFM_con->index)
						{
							continue;
						}

						pSFM_other_con = &(pSFM->connection[k]);

						if ((GM_RSSP1_TRUE == pSFM_other_con->b_enable_FSFB_on_chn)&& (pSFM_other_con->Chn_apply_FSFB_ID == pSFM_con->Chn_apply_FSFB_ID))
						{
							FSFB_Msg_Queue_Clear(&(pSFM_other_con->L2U_dat_Q));
						}
					}
				}/*if (GM_RSSP1_TRUE == b_got_pri)*/
			}

		}
	}

}

void GM_RSSP1_Check_Vital_Cfg(GM_RSSP1_SFM_object_struct *pSFM)
{
	GM_RSSP1_INT32 con_index = 0;
	GM_RSSP1_SFM_connection_struct *pSFM_con = NULL;

	if (NULL == pSFM)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Check_Vital_Cfg_Point_Error, 0, 0, 0, 0, 0, 0);
		return;
	}

	for (con_index = 0 ; con_index < pSFM->connection_nums ; ++con_index)
	{
		pSFM_con = &(pSFM->connection[con_index]);

		if((pSFM_con->deltaTime > GM_RSSP1_MAX_DELTATIME_ARRAY_SIZE) 
			|| (pSFM_con->lifeTime> GM_RSSP1_MAX_TOLERATE_CYCLE) 
			|| (pSFM_con->torlerate_cycle_cfg> GM_RSSP1_MAX_TOLERATE_CYCLE))
		{
			pSFM->lib_state = GM_RSSP1_Lib_State_Failure;
		}
	}
	
	return;
}

GM_RSSP1_SFM_Get_Pri_From_CFM_Result_enum GM_RSSP1_SFM_Get_Pri_From_CFM(GM_RSSP1_CFM_L2U_pri_struct *pL2U)
{
	if (NULL == pL2U)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Get_Pri_From_CFM_Point_Error, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_SFM_Get_Pri_INVALID_L2U;
	}

	if (GM_RSSP1_TRUE == FSFB_Msg_Queue_Get(&(g_CFM_MQ.RCV_MQ), pL2U))
	{
		return GM_RSSP1_SFM_Get_Pri_OK;
	}
	else
	{
		return GM_RSSP1_SFM_Get_Pri_FAIL;
	}

}

GM_RSSP1_SFM_Put_Pri_To_CFM_Result_enum GM_RSSP1_SFM_Put_Pri_To_CFM(GM_RSSP1_CFM_U2L_pri_struct *pU2L, GM_RSSP1_SFM_object_struct *pSFM)
{
	GM_RSSP1_Write_Q_Return_Enum write_rt = Q_WRITE_FAIL;

	if ((NULL == pSFM) || (pSFM->lib_state != GM_RSSP1_Lib_State_Operational))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Put_Pri_To_CFM_Result_No_Match, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_SFM_Put_Pri_LIB_NOT_OPERATIONAL;
	}

	if (NULL == pU2L)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Put_Pri_To_CFM_Result_Point_Error, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_SFM_Put_Pri_INVALID_U2L;
	}

	if (GM_RSSP1_CFM_U2L_Unknow == pU2L->type)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Put_Pri_To_CFM_Type_Error, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_SFM_Put_Pri_INVALID_REQ_TYPE;
	}

	if (pU2L->index >= pSFM->connection_nums)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Put_Pri_To_CFM_Index_Error, pU2L->index, pSFM->connection_nums, 0, 0, 0, 0);
		return GM_RSSP1_SFM_Put_Pri_INVALID_CONNECTION_INDEX;
	}

	write_rt = FSFB_Msg_Queue_Write(&(g_CFM_MQ.SND_MQ) , pU2L);

	if (Q_WRITE_FAIL == write_rt)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Put_Pri_To_CFM_Write_Fail, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_SFM_Put_Pri_LIB_NOT_OPERATIONAL;
	}
	else if (Q_WRITE_OK_FULL == write_rt)
	{
		/* 成功，但队列已满 */
		GM_RSSP1_Log_Usr(GM_RSSP1_SFM_Put_Pri_To_CFM_Write_Full, 0, 0, 0, 0, 0, 0);
		GM_RSSP1_Log_Msg(0,"FSFB CFM U2L_pri_Q FULL!\n ", 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_SFM_Put_Pri_MQ_FULL;
	}
	else
	{
		/* 成功 */
		return GM_RSSP1_SFM_Put_Pri_OK;
	}

}
void GM_RSSP1_SFM_Syn_Con(GM_RSSP1_SFM_object_struct *pSFM)
{
	GM_RSSP1_INT32 conn_index = 0;
	GM_RSSP1_INT32 x = 0;
	GM_RSSP1_SFM_connection_struct *pSFM_conn = NULL;
	GM_RSSP1_SFM_connection_struct *pSFM_conn_x = NULL;
	if (NULL == pSFM) 
	{
		return;
	}
	if (GM_RSSP1_Lib_State_Operational != pSFM->lib_state)
	{
		return;
	}
	for (conn_index = 0 ; conn_index < pSFM->connection_nums ; ++conn_index)
	{
		pSFM_conn = &(pSFM->connection[conn_index]);
		for(x = conn_index+1; x<pSFM->connection_nums; ++x)
		{	
			pSFM_conn_x = &(pSFM->connection[x]);
			if (GM_RSSP1_TRUE == GM_RSSP1_SFM_Is_SaCEPID_Equal(&(pSFM_conn_x->SaCEPID) , &(pSFM_conn->SaCEPID)))
			{
				if((GM_RSSP1_Remote_Dev_Active== pSFM_conn->remote_dev_AS_state)&&(GM_RSSP1_Remote_Dev_Active!= pSFM_conn_x->remote_dev_AS_state))
				{
					GM_RSSP1_memcpy(pSFM_conn_x->env, pSFM_conn->env, sizeof(GM_RSSP1_CHN_ENV)*GM_RSSP1_CHECK_CHN_NUM);
					pSFM_conn_x->state  = pSFM_conn->state;
				}
				else if((GM_RSSP1_Remote_Dev_Active== pSFM_conn_x->remote_dev_AS_state)&&(GM_RSSP1_Remote_Dev_Active!= pSFM_conn->remote_dev_AS_state))
				{
					GM_RSSP1_memcpy(pSFM_conn->env, pSFM_conn_x->env, sizeof(GM_RSSP1_CHN_ENV)*GM_RSSP1_CHECK_CHN_NUM);
					pSFM_conn->state  = pSFM_conn_x->state;
				}
				else if((GM_RSSP1_Remote_Dev_Active== pSFM_conn_x->remote_dev_AS_state)&&(GM_RSSP1_Remote_Dev_Active== pSFM_conn->remote_dev_AS_state))
				{
					if(pSFM_conn->env[0].local_TC >= pSFM_conn_x->env[0].local_TC)
					{
						GM_RSSP1_memcpy(pSFM_conn_x->env, pSFM_conn->env, sizeof(GM_RSSP1_CHN_ENV)*GM_RSSP1_CHECK_CHN_NUM);
						pSFM_conn_x->state  = pSFM_conn->state;
						pSFM_conn_x->remote_dev_AS_state = GM_RSSP1_Remote_DEV_Standby;
						pSFM_conn->remote_dev_AS_state = GM_RSSP1_Remote_Dev_Active;
					}
					else
					{
						GM_RSSP1_memcpy(pSFM_conn->env, pSFM_conn_x->env, sizeof(GM_RSSP1_CHN_ENV)*GM_RSSP1_CHECK_CHN_NUM);
						pSFM_conn->state  = pSFM_conn_x->state;
						pSFM_conn->remote_dev_AS_state = GM_RSSP1_Remote_DEV_Standby;
						pSFM_conn_x->remote_dev_AS_state = GM_RSSP1_Remote_Dev_Active;
					}
				}
				else
				{
					/*do nothing*/;
				}
			}
		}
	}
}

/**
* @brief GM_RSSP1_SetPreciTime
* @calculate the Precision Time;
*/
/*CR:3894 update prectime calculation*/
void GM_RSSP1_SetPreciTime(GM_RSSP1_SFM_connection_struct *pSFM_conn)
{
	GM_RSSP1_UINT16 conn_index = 0U;
	GM_RSSP1_UINT32 TremCycle_Diff = 0U;
	GM_RSSP1_UINT32 TlocCycle_Diff = 0U;
	GM_RSSP1_UINT32 TcycleRem = 0U;
	GM_RSSP1_UINT32 TcycleLoc = 0U;
	GM_RSSP1_INT32 tmp = 0;
	
	if (NULL == pSFM_conn) 
	{
		return ;
	}		
	

	if ((pSFM_conn->state != GM_RSSP1_Layer_State_Connected) || (pSFM_conn->index >= GM_RSSP1_MAX_SAFETY_CONNECTION_NUM))
	{
		pSFM_conn->PreciTime = -1;
		pSFM_conn->SSE_SSR_Delay = -1;
		return ;
	}

	if (pSFM_conn->b_rcvd_new_dat_in_cycle == GM_RSSP1_TRUE)	/*本周期收到 RSD消息*/
	{
		TremCycle_Diff = pSFM_conn->SINIT_TremCycle - pSFM_conn->RxSSRTrem-1;
		TlocCycle_Diff = pSFM_conn->time.TC - pSFM_conn->RxSSRTloc+1;
		TcycleRem = pSFM_conn->TcycleRem;
		TcycleLoc = pSFM_conn->TcycleLoc;
		tmp = (TlocCycle_Diff * TcycleLoc) - (TremCycle_Diff * TcycleRem);
		if (tmp > 0)
		{
			pSFM_conn->PreciTime = (tmp + (TcycleLoc - 1)) / TcycleLoc;
		}
		else
		{
			pSFM_conn->PreciTime = 0U;
		}

	}
	else
	{
		pSFM_conn->PreciTime++;
	}

	pSFM_conn->SSE_SSR_Delay = pSFM_conn->RxSSRTloc - pSFM_conn->LastSendSSECycle;
}
