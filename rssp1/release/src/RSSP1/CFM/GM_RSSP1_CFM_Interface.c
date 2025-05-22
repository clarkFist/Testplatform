#include "GM_RSSP1_CFM_Interface.h"
#include "GM_RSSP1_MQ_Interface.h"
extern RSSP1_MQ_LINK_struct g_Link_MQ[GM_RSSP1_MAX_SAFETY_CONNECTION_NUM];
extern RSSP1_MQ_Inter_struct g_CFM_MQ;
#ifdef	CFM_Stanby_Answer
extern GM_RSSP1_Msg_Queue_struct g_CFM_Debug_MQ;
#endif
GM_RSSP1_Local_Con_struct g_loc_info;
/*接收过程 先CFM，再SFM*/
void GM_RSSP1_CFM_Interface_Proc_Recv(void)
{
	GM_RSSP1_CFM_object_struct *pCFM = NULL;
	pCFM = GM_RSSP1_Get_CFM_Object();

	if (NULL == pCFM)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Interface_Proc_Recv_Get_Object_Fail, 0, 0, 0, 0, 0, 0);
		return;
	}

	if (GM_RSSP1_Lib_State_Operational != pCFM->lib_state)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Interface_Proc_Recv_State_Fail, pCFM->lib_state, 0, 0, 0, 0, 0);
		return;
	}

	GM_RSSP1_CFM_Proc_Recvd_Dat(pCFM);/*功能：将通信层数据放入冗余层，需要拆分*/
	GM_RSSP1_CFM_Report_com_State_To_SFM(pCFM);/*功能：将通信层状态通过冗余层上传，需要拆分*/
}


/*发送过程 先SFM，再CFM*/
void GM_RSSP1_CFM_Interface_Proc_Send(void)
{
	GM_RSSP1_CFM_object_struct *pCFM = NULL;

	pCFM = GM_RSSP1_Get_CFM_Object();

	if (NULL == pCFM)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Interface_Proc_Send_Get_Object_Fail, 0, 0, 0, 0, 0, 0);
		return;
	}

	if (GM_RSSP1_Lib_State_Operational != pCFM->lib_state)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Interface_Proc_Send_State_Fail, pCFM->lib_state, 0, 0, 0, 0, 0);
		return;
	}

	GM_RSSP1_CFM_Proc_SFM_Req(pCFM);

}


GM_RSSP1_BOOL GM_RSSP1_Get_ConIndex(GM_RSSP1_UINT32 r_port, GM_RSSP1_UINT32 r_ip, GM_RSSP1_UINT8* con_index, GM_RSSP1_UINT8* red_index)
{
	GM_RSSP1_CFM_object_struct *pCFM = NULL;
	GM_RSSP1_CFM_connection_struct *pCFM_con = NULL;
	GM_RSSP1_UINT8 chn_index = 0U;
	GM_RSSP1_UINT8 conn_index = 0U;

	if ((con_index == NULL)||(red_index == NULL))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Get_ConIndex_Param_Point_Error, con_index, red_index, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	pCFM = GM_RSSP1_Get_CFM_Object();
	if (pCFM == NULL)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Get_ConIndex_Get_Object_Fail, con_index, red_index, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	for (conn_index=0U; conn_index<pCFM->connection_nums; ++conn_index)
	{
		pCFM_con = &(pCFM->connection[conn_index]);
		for (chn_index=0U;chn_index<pCFM_con->chn_num;++chn_index)
		{
			if ((r_port == pCFM_con->com_chn[chn_index].rem_port)&&(r_ip == pCFM_con->com_chn[chn_index].rem_ip))
			{
				*con_index = conn_index;
				*red_index = chn_index;
				return GM_RSSP1_TRUE; 
			}
		}
	}
	return GM_RSSP1_FALSE;
}

GM_RSSP1_BOOL GM_RSSP1_Get_Address(GM_RSSP1_UINT32* r_port, GM_RSSP1_UINT32* r_ip, GM_RSSP1_UINT8 con_index, GM_RSSP1_UINT8 red_index)
{
	GM_RSSP1_CFM_object_struct *pCFM = NULL;
	GM_RSSP1_CFM_connection_struct *pCFM_con = NULL;

	if ((r_port == NULL)||(r_ip == NULL))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Get_Address_Param_Point_Error, r_port, r_ip, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	pCFM = GM_RSSP1_Get_CFM_Object();
	if (pCFM == NULL)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Get_Address_Get_Object_Fail, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	if (con_index>=pCFM->connection_nums)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Get_Address_Con_index_Fail, con_index, pCFM->connection_nums, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	pCFM_con = &(pCFM->connection[con_index]);

	if (red_index>=pCFM_con->chn_num)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Get_Address_Chn_index_Fail, red_index, pCFM_con->chn_num, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	
	*r_port = pCFM_con->com_chn[red_index].rem_port;
	*r_ip = pCFM_con->com_chn[red_index].rem_ip;

	return GM_RSSP1_TRUE;
}


GM_RSSP1_Local_Con_struct* GM_RSSP1_Get_LocInfo_Obj(void)
{
	return &g_loc_info;
}

/*CR:GM00002527, GM162, fulintai,2013-06-09, much more return type for different exit*/
GM_RSSP1_INT8 GM_RSSP1_Write_SFM_Interface_ergodic(GM_RSSP1_UINT8* p_dat, GM_RSSP1_UINT16* len, GM_RSSP1_UINT32* r_port, GM_RSSP1_UINT32* r_ip, GM_RSSP1_UINT8* con_index, GM_RSSP1_UINT8* reduan_index)
{
	GM_RSSP1_com_pri_struct  temp_send_pkt= {0};
	GM_RSSP1_BOOL write_q_rt = GM_RSSP1_FALSE;
	GM_RSSP1_BOOL rt = GM_RSSP1_FALSE;
	GM_RSSP1_CFM_object_struct *pCFM = NULL;
	GM_RSSP1_CFM_connection_struct *pCFM_con = NULL;

	static GM_RSSP1_UINT8 i = 0U;
	static GM_RSSP1_UINT8 j = 0U;

	if ((p_dat == NULL)||(len == NULL) ||(r_port == NULL) ||(r_ip == NULL) ||(con_index == NULL) ||(reduan_index== NULL))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Write_SFM_Interface_ergodic_Param_Point_Error, p_dat, len, r_port, r_ip, con_index, reduan_index);
		return -1;
	}

	pCFM = GM_RSSP1_Get_CFM_Object();
	if (pCFM == NULL)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Write_SFM_Interface_ergodic_Get_Object_Fail, 0, 0, 0, 0, 0, 0);
		return -1;
	}

	while (i<pCFM->connection_nums)
	{
		pCFM_con = &(pCFM->connection[i]);
		while(j<pCFM_con->chn_num)
		{
			while(GM_RSSP1_TRUE == FSFB_Msg_Queue_Get(&(g_Link_MQ[i].RCV_SND_MQ[j].SND_MQ),&temp_send_pkt))
			{
				rt = GM_RSSP1_Get_Address(r_port, r_ip, i, j);
				if (rt == GM_RSSP1_TRUE)
				{
					GM_RSSP1_memcpy(p_dat, temp_send_pkt.byte, temp_send_pkt.bytes_count);
					*len = temp_send_pkt.bytes_count;
					*con_index = i;
					*reduan_index = j;
					return 1;
				}
			}
            j++;
		}
		j = 0U;
		i++;
	}
	
	i = 0U;
	j = 0U;
	
	return 0;
}

GM_RSSP1_BOOL RSSP1_CFM_Clear_Msg_Queue(void)
{
	GM_RSSP1_CFM_object_struct *pCFM = NULL;
	register GM_RSSP1_CFM_connection_struct *pCFM_con = NULL;
	GM_RSSP1_UINT8 i = 0U;
	GM_RSSP1_UINT8 j = 0U;
	GM_RSSP1_BOOL rt = GM_RSSP1_FALSE;

	/*clear FSFB SFM queue*/
	pCFM = GM_RSSP1_Get_CFM_Object();
	if (pCFM == NULL)
	{
		GM_RSSP1_Log_Usr(RSSP1_CFM_Clear_Msg_Queue_Get_Object_Fail, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	/* u2l l2u*/
	rt = FSFB_Msg_Queue_Clear(&(g_CFM_MQ.SND_MQ));
	if(GM_RSSP1_FALSE == rt)
	{
		return GM_RSSP1_FALSE;
	}
	rt &= FSFB_Msg_Queue_Clear(&(g_CFM_MQ.RCV_MQ));   
	if(GM_RSSP1_FALSE == rt)
	{
		return GM_RSSP1_FALSE;
	}

	/*every connection Q*/
    for (i = 0U; i < pCFM->connection_nums; i++)
	{
		pCFM_con = &(pCFM->connection[i]);
        for (j = 0U;j < pCFM_con->chn_num; j++)
		{
			rt &= FSFB_Msg_Queue_Clear(&(g_Link_MQ[i].RCV_SND_MQ[j].SND_MQ));
		}
	}
	return rt;
}

/* start:CR:GM00002526, GM162, fulintai,2015-06-09, add interface to get all 0 message in standby's CFM*/
#ifdef	CFM_Stanby_Answer
GM_RSSP1_INT8 GM_RSSP1_Write_StandbyInfo_Interface_ergodic(GM_RSSP1_UINT8* p_dat, GM_RSSP1_UINT16* len, GM_RSSP1_UINT8* con_index, GM_RSSP1_UINT8* reduan_index)
{
	GM_RSSP1_CFM_U2L_pri_struct pri = {0};
	GM_RSSP1_BOOL write_q_rt = GM_RSSP1_FALSE;
	GM_RSSP1_BOOL rt = GM_RSSP1_FALSE;
	GM_RSSP1_CFM_object_struct *pCFM = NULL;
	GM_RSSP1_CFM_connection_struct *pCFM_con = NULL;
	GM_RSSP1_UINT8 i = 0U;
	GM_RSSP1_UINT8 j = 0U;

	if ((p_dat == NULL)||(len == NULL) || (con_index == NULL) || (reduan_index== NULL))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Write_SFM_Interface_ergodic_Param_Point_Error, p_dat, len, con_index, reduan_index, 0, 0);
		return -1;
	}

	pCFM = GM_RSSP1_Get_CFM_Object();
	
	if (pCFM == NULL)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Write_SFM_Interface_ergodic_Get_Object_Fail, 0, 0, 0, 0, 0, 0);
		return -1;
	}

	while(GM_RSSP1_TRUE == FSFB_Msg_Queue_Get(&g_CFM_Debug_MQ,&pri))
	{
		i = pri.index;
		j = pCFM->connection[i].chn_num;
		
        if ((pri.index > pCFM->connection_nums) || (GM_RSSP1_CFM_U2L_Unknow == pri.type))
        {
        	continue;
        }
		
		GM_RSSP1_memcpy(p_dat, pri.as.data_req.byte , pri.as.data_req.bytes_count);
		*len = pri.as.data_req.bytes_count;
		*con_index = i;
		*reduan_index = j;
		return 1;
	}
	FSFB_Msg_Queue_Clear(&g_CFM_Debug_MQ);
	return 0;
}
#endif
/* end CR:GM00002526*/
