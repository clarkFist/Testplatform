
#include "GM_RSSP1_CFM.h"
#include "GM_RSSP1_MQ_Interface.h"
#include "GlueData.h"

extern GM_RSSP1_VSN_GET_CALLBACK_FUN GM_RSSP1_VSN_Get_Callback_Fun;	/*added by huang 201511219*/

static GM_RSSP1_CFM_object_struct g_CFM_obj;

extern RSSP1_MQ_LINK_struct g_Link_MQ[GM_RSSP1_MAX_SAFETY_CONNECTION_NUM];

extern RSSP1_MQ_Inter_struct g_CFM_MQ;

#ifdef	CFM_Stanby_Answer
extern GM_RSSP1_Msg_Queue_struct g_CFM_Debug_MQ;
#endif

GM_RSSP1_CFM_object_struct *GM_RSSP1_Get_CFM_Object(void)
{
    return &g_CFM_obj;
}

GM_RSSP1_library_state_enum GM_RSSP1_CFM_Get_Library_Status(GM_RSSP1_CFM_object_struct *pCFM)
{
    if (NULL == pCFM)
    {
        GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Get_Library_Status_Param_Point_Error, 0, 0, 0, 0, 0, 0);
        return GM_RSSP1_Lib_State_Failure;
    }

    return pCFM->lib_state;
}

GM_RSSP1_BOOL GM_RSSP1_CFM_Is_SaCEPID_Equal(GM_RSSP1_SACEPID_struct *pSaCEPID1 , GM_RSSP1_SACEPID_struct *pSaCEPID2)
{
    if ((NULL == pSaCEPID1) || (NULL == pSaCEPID2))
    {
        GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Is_SaCEPID_Equal_Param_Point_Error, pSaCEPID1, pSaCEPID2, 0, 0, 0, 0);
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


GM_RSSP1_CFM_connection_struct *GM_RSSP1_CFM_Get_Connection_BY_SaCEPID(GM_RSSP1_SACEPID_struct *pSaCEPID , GM_RSSP1_CFM_object_struct *pCFM)
{
    GM_RSSP1_INT32 conn_index = 0;
    GM_RSSP1_CFM_connection_struct *pCFM_conn   = NULL;

    if ((NULL == pCFM) || (NULL == pSaCEPID))
    {
        GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Get_Connection_BY_SaCEPID_Param_Point_Error, pCFM, pSaCEPID, 0, 0, 0, 0);
        return NULL;
    }

    if (GM_RSSP1_Lib_State_Operational != pCFM->lib_state)
    {
        GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Get_Connection_BY_SaCEPID_State_Fail, pCFM->lib_state, 0, 0, 0, 0, 0);
        return NULL;
    }

    for (conn_index = 0 ; conn_index < pCFM->connection_nums ; ++conn_index)
    {
        pCFM_conn = &(pCFM->connection[conn_index]);

        if (GM_RSSP1_TRUE == GM_RSSP1_CFM_Is_SaCEPID_Equal(pSaCEPID , &(pCFM_conn->SaCEPID)))
        {
            return pCFM_conn;
        }
    }
    GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Get_Connection_BY_SaCEPID_Search_Fail, 0, 0, 0, 0, 0, 0);
    return NULL;
}


void GM_RSSP1_CFM_Proc_SFM_Req(GM_RSSP1_CFM_object_struct *pCFM)
{
    GM_RSSP1_CFM_connection_struct *pCFM_con = NULL;
    GM_RSSP1_CFM_U2L_pri_struct pri = {0};
#ifdef GM_RSSP1_ENABLE_CRSCD_PACK
    GM_RSSP1_UINT16 rsd_pri_num[GM_RSSP1_MAX_SAFETY_CONNECTION_NUM] = {0};
    GM_RSSP1_UINT16 sse_pri_num[GM_RSSP1_MAX_SAFETY_CONNECTION_NUM] = {0};
    GM_RSSP1_UINT16 ssr_pri_num[GM_RSSP1_MAX_SAFETY_CONNECTION_NUM] = {0};
    GM_RSSP1_CFM_U2L_pri_struct RSD_pri[GM_RSSP1_MAX_SAFETY_CONNECTION_NUM][GM_RSSP1_MAX_PKT_NUM_PER_CYCLE] = {0};
    GM_RSSP1_CFM_U2L_Dat_SSE_pri_struct SSE_pri[GM_RSSP1_MAX_SAFETY_CONNECTION_NUM][GM_RSSP1_MAX_PKT_NUM_PER_CYCLE] = {0};
    GM_RSSP1_CFM_U2L_Dat_SSR_pri_struct SSR_pri[GM_RSSP1_MAX_SAFETY_CONNECTION_NUM][GM_RSSP1_MAX_PKT_NUM_PER_CYCLE] = {0};
    GM_RSSP1_UINT8 combined_pkt_buf[GM_RSSP1_MAX_SAFETY_CONNECTION_NUM][GM_RSSP1_MAX_PKT_NUM_PER_CYCLE * GM_RSSP1_MAX_SAFETY_DAT_LEN] = {0};
    GM_RSSP1_UINT16 combined_pkt_len[GM_RSSP1_MAX_SAFETY_CONNECTION_NUM] = {0};
    GM_RSSP1_UINT16 pri_index = 0U;
    GM_RSSP1_UINT8 conn_index = 0U;
    GM_RSSP1_UINT8 frame_type = 0U;
#endif

    if (NULL == pCFM)
    {
        GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Proc_SFM_Req_Param_Point_Error, pCFM, 0, 0, 0, 0, 0);
        return;
    }

    if (GM_RSSP1_Lib_State_Operational != pCFM->lib_state)
    {
        GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Proc_SFM_Req_State_Fail, pCFM->lib_state, 0, 0, 0, 0, 0);
        return;
    }

    while (GM_RSSP1_TRUE == FSFB_Msg_Queue_Get(&(g_CFM_MQ.SND_MQ) , &pri))
    {
        if ((pri.index < pCFM->connection_nums) && (GM_RSSP1_CFM_U2L_Unknow != pri.type))
        {
            pCFM_con = &(pCFM->connection[pri.index]);

            if (GM_RSSP1_TRUE == pCFM_con->b_enable_CRSCD_pack)
            {
                /* 设计院方案：多包组合发送。RSD一直放在最前面，SSE、SSR紧接着RSD后面.SSE和SSR顺序没有要求
                 * 此处，只对CRSCD连接进行FSFB小数据包的提取和暂存，暂不发送  */
#ifdef GM_RSSP1_ENABLE_CRSCD_PACK
                switch (pri.type)
                {

                case GM_RSSP1_CFM_Data_req:
                    frame_type = pri.as.data_req.byte[1];

                    if ((RSSP1_RSD_FRAME_TYPE_FROM_A == frame_type) || (RSSP1_RSD_FRAME_TYPE_FROM_B == frame_type))
                    {
                        RSD_pri[pCFM_con->index][rsd_pri_num[pCFM_con->index]] = pri;
                        ++rsd_pri_num[pCFM_con->index];
                    }
                    else if (RSSP1_SSE_FRAME_TYPE == frame_type)
                    {
                        SSE_pri[pCFM_con->index][sse_pri_num[pCFM_con->index]].type         = pri.type;
                        SSE_pri[pCFM_con->index][sse_pri_num[pCFM_con->index]].TC           = pri.as.data_req.TC;
                        SSE_pri[pCFM_con->index][sse_pri_num[pCFM_con->index]].bytes_count  = pri.as.data_req.bytes_count;

                        if (RSSP1_SSE_FRAME_LEN_WITH_CRC == pri.as.data_req.bytes_count)
                        {
                            GM_RSSP1_memcpy(SSE_pri[pCFM_con->index][sse_pri_num[pCFM_con->index]].byte , pri.as.data_req.byte , pri.as.data_req.bytes_count);
                            ++sse_pri_num[pCFM_con->index];
                        }
                        else
                        {
                            continue;
                        }
                    }
                    else if (RSSP1_SSR_FRAME_TYPE == frame_type)
                    {
                        SSR_pri[pCFM_con->index][ssr_pri_num[pCFM_con->index]].type         = pri.type;
                        SSR_pri[pCFM_con->index][ssr_pri_num[pCFM_con->index]].TC           = pri.as.data_req.TC;
                        SSR_pri[pCFM_con->index][ssr_pri_num[pCFM_con->index]].bytes_count  = pri.as.data_req.bytes_count;

                        if (RSSP1_SSR_FRAME_LEN_WITH_CRC == pri.as.data_req.bytes_count)
                        {
                            GM_RSSP1_memcpy(SSR_pri[pCFM_con->index][ssr_pri_num[pCFM_con->index]].byte , pri.as.data_req.byte , pri.as.data_req.bytes_count);
                            ++ssr_pri_num[pCFM_con->index];
                        }
                        else
                        {
                            continue;
                        }
                    }
                    else
                    {
                    }

                    break;

                default:
                    break;
                }
#endif
            }
            else
            {
                /* 标准单包发送 */
                switch (pri.type)
                {
                case GM_RSSP1_CFM_Data_req:
                    GM_RSSP1_CFM_Send_Dat(pri.as.data_req.byte , pri.as.data_req.bytes_count , pCFM_con , pCFM, pri.index);
                    break;			
                default:
                    break;
                }
            }
        }
    }
#ifdef GM_RSSP1_ENABLE_CRSCD_PACK
    /* send combined pkts */
    for (conn_index = 0 ; conn_index < pCFM->connection_nums ; ++conn_index)
    {
        pCFM_con = &(pCFM->connection[conn_index]);

        if (GM_RSSP1_TRUE == pCFM_con->b_enable_CRSCD_pack)
        {
            combined_pkt_len[conn_index] = 0;

            /* RSD在最前面 */

            for (pri_index = 0 ; pri_index < rsd_pri_num[conn_index] ; ++pri_index)
            {
                GM_RSSP1_memcpy(combined_pkt_buf[conn_index] + combined_pkt_len[conn_index] , RSD_pri[conn_index][pri_index].as.data_req.byte , RSD_pri[conn_index][pri_index].as.data_req.bytes_count);
                combined_pkt_len[conn_index] += RSD_pri[conn_index][pri_index].as.data_req.bytes_count;
            }

            /* SSE */
            for (pri_index = 0 ; pri_index < sse_pri_num[conn_index] ; ++pri_index)
            {
                GM_RSSP1_memcpy(combined_pkt_buf[conn_index] + combined_pkt_len[conn_index] , SSE_pri[conn_index][pri_index].byte , SSE_pri[conn_index][pri_index].bytes_count);
                combined_pkt_len[conn_index]  += SSE_pri[conn_index][pri_index].bytes_count;
            }

            /* SSR */
            for (pri_index = 0 ; pri_index < ssr_pri_num[conn_index] ; ++pri_index)
            {
                GM_RSSP1_memcpy(combined_pkt_buf[conn_index] + combined_pkt_len[conn_index] , SSR_pri[conn_index][pri_index].byte , SSR_pri[conn_index][pri_index].bytes_count);
                combined_pkt_len[conn_index]  += SSR_pri[conn_index][pri_index].bytes_count;
            }

            if (combined_pkt_len[conn_index] > 0)
            {
                GM_RSSP1_CFM_Send_Dat(combined_pkt_buf[conn_index] , combined_pkt_len[conn_index] , pCFM_con , pCFM, conn_index);
            }
        }
    }
#endif
    return;
}

GM_RSSP1_BOOL GM_RSSP1_CFM_Send_Dat(GM_RSSP1_UINT8 *pDat , GM_RSSP1_UINT16 dat_len , GM_RSSP1_CFM_connection_struct *pCFM_con , GM_RSSP1_CFM_object_struct *pCFM, GM_RSSP1_UINT8 index)
{
    GM_RSSP1_INT32 chn_index = 0 ;
    GM_RSSP1_com_pri_struct send_pri = {0};
    GM_RSSP1_Write_Q_Return_Enum q_write_rt = Q_WRITE_FAIL;

	GM_RSSP1_memset(&send_pri, 0, sizeof(GM_RSSP1_com_pri_struct));

    if ((dat_len > GM_RSSP1_MAX_SND_DAT_LEN) || (NULL == pDat) || (NULL == pCFM_con) || (NULL == pCFM))
    {
        GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Send_Dat_Param_Val_Error, dat_len, pDat, pCFM_con, pCFM, GM_RSSP1_MAX_SND_DAT_LEN, 0);
        GM_RSSP1_Log_Msg(0,"FSFB CFM GM_RSSP1_CFM_Send_Dat illegal!\n", 0 , 0, 0, 0, 0, 0);
        return GM_RSSP1_FALSE;
    }

    if (GM_RSSP1_Lib_State_Operational != pCFM->lib_state)
    {
        GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Send_Dat_State_Fail, pCFM->lib_state, 0, 0, 0, 0, 0);
        return GM_RSSP1_FALSE;
    }

    GM_RSSP1_memcpy(send_pri.byte , pDat , dat_len);

    send_pri.bytes_count = dat_len;

    for (chn_index = 0 ; chn_index < pCFM_con->chn_num ; ++chn_index)
    {

		if(g_bugInsert.m_nDelayBSDOffset !=0)
		{
			Sleep(400*g_bugInsert.m_nDelayBSDOffset);
			q_write_rt = FSFB_Msg_Queue_Write(&(g_Link_MQ[index].RCV_SND_MQ[chn_index].SND_MQ) , (void*)&send_pri);
		}
		else
		{
        q_write_rt = FSFB_Msg_Queue_Write(&(g_Link_MQ[index].RCV_SND_MQ[chn_index].SND_MQ) , (void*)&send_pri);
		}

        if (Q_WRITE_OK_FULL == q_write_rt)
        {
            GM_RSSP1_Log_Msg(0,"FSFB CFM Queue full!\n", 0 , 0, 0, 0, 0, 0);
        }
        else if (Q_WRITE_FAIL == q_write_rt)
        {
            GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Send_Dat_Write_Fail, 0, 0, 0, 0, 0, 0);
            pCFM->lib_state = GM_RSSP1_Lib_State_Failure;
            return GM_RSSP1_FALSE;
        }
        else
        {
        }
    }

    return GM_RSSP1_TRUE;
}

GM_RSSP1_BOOL GM_RSSP1_CFM_User_Send_Dat(GM_RSSP1_SACEPID_struct *psacepid , GM_RSSP1_UINT8 *pDat , GM_RSSP1_UINT16 len , GM_RSSP1_CFM_object_struct *pCFM)
{
    GM_RSSP1_CFM_U2L_pri_struct cfm_pri = {0};
    GM_RSSP1_CFM_connection_struct *pCFM_con = NULL;
    GM_RSSP1_Write_Q_Return_Enum write_q_rt = Q_WRITE_FAIL;

    if ((NULL == psacepid) || (NULL == pDat) || (NULL == pCFM) || (len > sizeof(cfm_pri.as.data_req.byte)))
    {
        GM_RSSP1_Log_Usr(GM_RSSP1_CFM_User_Send_Dat_Param_Val_Error, psacepid, pDat, pCFM, len, sizeof(cfm_pri.as.data_req.byte), 0);
        return GM_RSSP1_FALSE;
    }

    if (GM_RSSP1_Lib_State_Operational != pCFM->lib_state)
    {
        GM_RSSP1_Log_Usr(GM_RSSP1_CFM_User_Send_Dat_State_Fail, pCFM->lib_state, 0, 0, 0, 0, 0);
        return GM_RSSP1_FALSE;
    }

    pCFM_con = GM_RSSP1_CFM_Get_Connection_BY_SaCEPID(psacepid , pCFM);
    if (NULL == pCFM_con)
    {
		GM_RSSP1_Log_Usr(GM_RSSP1_CFM_User_Send_Dat_Search_Fail, pCFM->lib_state, 0, 0, 0, 0, 0);
        return GM_RSSP1_FALSE;
    }

    cfm_pri.type                = GM_RSSP1_CFM_Data_req;
    cfm_pri.index               = pCFM_con->index;
    cfm_pri.as.data_req.type    = GM_RSSP1_MSG_TYPE_RSD;
    cfm_pri.as.data_req.TC      = 0;

    GM_RSSP1_memcpy(cfm_pri.as.data_req.byte , pDat , len);
    cfm_pri.as.data_req.bytes_count = len;

	write_q_rt = FSFB_Msg_Queue_Write(&(g_CFM_MQ.SND_MQ) , &cfm_pri);
	if (Q_WRITE_FAIL == write_q_rt)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_CFM_User_Send_Dat_Write_Fail, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	else if (Q_WRITE_OK_FULL == write_q_rt)
	{
		/* 成功，但队列已满 */
		GM_RSSP1_Log_Msg(0,"FSFB CFM U2L_pri_Q FULL!\n ", 0, 0, 0, 0, 0, 0);
	}
	else
	{
		;
	}
	return GM_RSSP1_TRUE;
}
/*
GM_RSSP1_BOOL GM_RSSP1_CFM_User_Send_Dat_By_Index(GM_RSSP1_UINT8 index , GM_RSSP1_SACEPID_struct *psacepid , GM_RSSP1_UINT8 *pDat , GM_RSSP1_UINT16 len , GM_RSSP1_CFM_object_struct *pCFM)
{
    GM_RSSP1_CFM_U2L_pri_struct cfm_pri;
	GM_RSSP1_SFM_Put_Pri_To_CFM_Result_enum put_pri_rt = GM_RSSP1_SFM_Put_Pri_MQ_UNKNOWN_FAIL;

    if ((NULL == psacepid) || (NULL == pDat) || (NULL == pCFM) || (len > sizeof(cfm_pri.as.data_req.byte)) || (index >= pCFM->connection_nums) )
    {
        return GM_RSSP1_FALSE;
    }

    if (GM_RSSP1_Lib_State_Operational != pCFM->lib_state)
    {
        return GM_RSSP1_FALSE;
    }

    if(GM_RSSP1_FALSE == GM_RSSP1_CFM_Is_SaCEPID_Equal(psacepid , &(pCFM->connection[index].SaCEPID)))
    {
        return GM_RSSP1_FALSE;
    }

    cfm_pri.type                = GM_RSSP1_CFM_Data_req;

    cfm_pri.index               = index;

    cfm_pri.as.data_req.type    = GM_RSSP1_MSG_TYPE_RSD;
    cfm_pri.as.data_req.TC      = 0;

    GM_RSSP1_memcpy(cfm_pri.as.data_req.byte , pDat , len);
    cfm_pri.as.data_req.bytes_count = len;

    put_pri_rt = GM_RSSP1_SFM_Put_Pri_To_CFM(&cfm_pri , pCFM);

    if ((GM_RSSP1_SFM_Put_Pri_OK == put_pri_rt) || (GM_RSSP1_SFM_Put_Pri_MQ_FULL == put_pri_rt))
    {
        return GM_RSSP1_TRUE;
    }
    else
    {
        return GM_RSSP1_FALSE;
    }
	return GM_RSSP1_TRUE;
}
*/

GM_RSSP1_BOOL GM_RSSP1_CFM_Proc_Con_Rcvd_Dat(GM_RSSP1_CFM_connection_struct *pCFM_con , GM_RSSP1_CFM_object_struct * pCFM, GM_RSSP1_UINT8 index)
{
    GM_RSSP1_INT32 chn_index = 0;
    GM_RSSP1_INT32 valid_chn_index = 0;
    GM_RSSP1_INT32 i = 0;
	GM_RSSP1_CFM_L2U_pri_struct L2U_pri = {0};
    GM_RSSP1_Write_Q_Return_Enum write_q_rt = Q_WRITE_FAIL;
	GM_RSSP1_com_pri_struct rcvd_dat_pri = {0};
    GM_RSSP1_INT32 pkt_len = 0;
    GM_RSSP1_INT32 rsd_user_dat_len = 0;

	if ((pCFM_con == NULL)||(pCFM == NULL)||(index >= pCFM->connection_nums))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Proc_Con_Rcvd_Dat_Param_Val_Error, pCFM_con, pCFM, index, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	valid_chn_index = -1;

	for (chn_index = 0 ; chn_index < pCFM_con->chn_num ; ++chn_index)
	{
		if (GM_RSSP1_FALSE == FSFB_Is_Msg_Queue_Empty(&(g_Link_MQ[index].RCV_SND_MQ[chn_index].RCV_MQ)))
		{
			if (valid_chn_index == -1)
			{
				valid_chn_index = chn_index;
			}
			pCFM_con->com_chn[chn_index].b_connected = GM_RSSP1_TRUE;
		}
		else
		{
			pCFM_con->com_chn[chn_index].b_connected = GM_RSSP1_FALSE;
		}
	}

	if (valid_chn_index < 0)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Proc_Con_Rcvd_Dat_Overall_Error, valid_chn_index, pCFM_con->chn_num, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	/* clear other chn */
	for (chn_index = 0 ; chn_index < pCFM_con->chn_num ; ++chn_index)
	{
		if (chn_index != valid_chn_index)
		{
			FSFB_Msg_Queue_Clear(&(g_Link_MQ[index].RCV_SND_MQ[chn_index].RCV_MQ));
		}
	}

    if (GM_RSSP1_TRUE == pCFM_con->b_enable_CRSCD_pack)
    {
        /* CRSCD方案，收到的一个UDP数据包可能是多个FSFB包合并而成 */

        while (GM_RSSP1_TRUE == FSFB_Msg_Queue_Get(&(g_Link_MQ[index].RCV_SND_MQ[valid_chn_index].RCV_MQ), &rcvd_dat_pri))
        {
            if (rcvd_dat_pri.bytes_count > GM_RSSP1_MAX_SAFETY_DAT_LEN)
            {
                GM_RSSP1_Log_Msg(0,"FSFB CFM ID:0x%X UDP socket %d recvd %d bytes,exceed GM_RSSP1_MAX_SAFETY_DAT_LEN: %d.\n", pCFM_con->SaCEPID.source_addr , chn_index , rcvd_dat_pri.bytes_count, GM_RSSP1_MAX_SAFETY_DAT_LEN, 0, 0);
                continue;
            }

            pkt_len = 0;

            for (i = 0 ; i < rcvd_dat_pri.bytes_count ;)
            {
                if ((RSSP1_RSD_FRAME_TYPE_FROM_A == rcvd_dat_pri.byte[i + 1]) || (RSSP1_RSD_FRAME_TYPE_FROM_B == rcvd_dat_pri.byte[i + 1]))
                {
                    /* find a RSD */
                    rsd_user_dat_len = rcvd_dat_pri.byte[i + 10] + (rcvd_dat_pri.byte[i + 11] << 8) - 8;
                    pkt_len = RSSP1_RSD_FRAME_LEN_WITHOUT_USERDAT + rsd_user_dat_len;
                }
                else if (RSSP1_SSE_FRAME_TYPE == rcvd_dat_pri.byte[i + 1])
                {
                    /* find a SSE */
                    pkt_len = RSSP1_SSE_FRAME_LEN_WITH_CRC;
                }
                else if (RSSP1_SSR_FRAME_TYPE == rcvd_dat_pri.byte[i + 1])
                {
                    /* find a SSR */
                    pkt_len = RSSP1_SSR_FRAME_LEN_WITH_CRC;
                }
                else
                {
                    /* unknown pkt */
                    GM_RSSP1_Log_Msg(0,"FSFB CFM ID:0x%X UDP socket %d recvd unknown pkt,type:%d.\n", pCFM_con->SaCEPID.source_addr , chn_index , rcvd_dat_pri.byte[i + 1], 0, 0, 0);
                }

                if (pkt_len > 0)
                {
                    /* 每提取一包数据就向上报告一次 */
                    L2U_pri.index   = pCFM_con->index;
                    L2U_pri.type    = GM_RSSP1_CFM_Data_Ind;
                    L2U_pri.as.data_indi.bytes_count    = (GM_RSSP1_UINT16)pkt_len;
                    GM_RSSP1_memcpy(L2U_pri.as.data_indi.byte ,  rcvd_dat_pri.byte + i , pkt_len);

                    write_q_rt = FSFB_Msg_Queue_Write(&(g_CFM_MQ.RCV_MQ), (void*)(&L2U_pri));

                    if (Q_WRITE_OK_FULL == write_q_rt)
                    {
                        GM_RSSP1_Log_Msg(0,"FSFB CFM L2U_pri_Q full(CRSCD)!\n", 0, 0, 0, 0, 0, 0);
                    }
                    else if (Q_WRITE_FAIL == write_q_rt)
                    {
                        GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Proc_Con_Rcvd_Dat_Write_Fail, pCFM_con->b_enable_CRSCD_pack, 0, 0, 0, 0, 0);
                        pCFM->lib_state = GM_RSSP1_Lib_State_Failure;
                        return GM_RSSP1_FALSE;
                    }
                    else
                    {
                    }

                    i += pkt_len;
                    pkt_len = 0;
                }
                else
                {
                    break;
                }

            }

        }

    }
    else
    {
        /* 每个完整的UDP数据包只是单个FSFB包 */
        while (GM_RSSP1_TRUE == FSFB_Msg_Queue_Get(&(g_Link_MQ[index].RCV_SND_MQ[valid_chn_index].RCV_MQ), &rcvd_dat_pri))
        {
            /* TODO: 如何挑选最合适包待定 */
            if (rcvd_dat_pri.bytes_count > GM_RSSP1_MAX_SAFETY_DAT_LEN)
            {
                GM_RSSP1_Log_Msg(0,"FSFB CFM ID:0x%X UDP socket %d recvd %d bytes,exceed GM_RSSP1_MAX_SAFETY_DAT_LEN: %d.\n", pCFM_con->SaCEPID.source_addr, chn_index , rcvd_dat_pri.bytes_count, GM_RSSP1_MAX_SAFETY_DAT_LEN, 0, 0);
                continue;
            }

			if (pCFM_con->index == 1 ) /*Alan: test code*/
			{
				i = 0;
			}

            /* TODO:CRC检验 */
            L2U_pri.index   = pCFM_con->index;
            L2U_pri.type    = GM_RSSP1_CFM_Data_Ind;
            L2U_pri.as.data_indi.bytes_count    = rcvd_dat_pri.bytes_count;
            GM_RSSP1_memcpy(L2U_pri.as.data_indi.byte , rcvd_dat_pri.byte , rcvd_dat_pri.bytes_count);
            write_q_rt = FSFB_Msg_Queue_Write(&(g_CFM_MQ.RCV_MQ) , &L2U_pri);
			
            if (Q_WRITE_OK_FULL == write_q_rt)
            {
                GM_RSSP1_Log_Msg(0,"FSFB CFM L2U_pri_Q full!\n", 0, 0, 0, 0, 0, 0);
            }
            else if (Q_WRITE_FAIL == write_q_rt)
            {
                GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Proc_Con_Rcvd_Dat_Write_Fail, pCFM_con->b_enable_CRSCD_pack, 0, 0, 0, 0, 0);
                pCFM->lib_state = GM_RSSP1_Lib_State_Failure;
                return GM_RSSP1_FALSE;
            }
            else
            {
            }

        }/* while */
    }

    return GM_RSSP1_TRUE;
    
}

void GM_RSSP1_CFM_Proc_Recvd_Dat(GM_RSSP1_CFM_object_struct * pCFM)
{
    GM_RSSP1_UINT8 cfm_con_index = 0U;
    GM_RSSP1_CFM_connection_struct *pCFM_con = NULL;

    if (NULL == pCFM)
    {
        GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Proc_Recvd_Dat_Param_Point_Error, pCFM, 0, 0, 0, 0, 0);
        return;
    }

    if (GM_RSSP1_Lib_State_Operational != pCFM->lib_state)
    {
        GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Proc_Recvd_Dat_State_Fail, pCFM->lib_state, 0, 0, 0, 0, 0);
        return;
    }

    for (cfm_con_index = 0 ; cfm_con_index < pCFM->connection_nums ; ++cfm_con_index)
    {
        pCFM_con = &(pCFM->connection[cfm_con_index]);
        GM_RSSP1_CFM_Proc_Con_Rcvd_Dat(pCFM_con , pCFM, cfm_con_index);
    }

    return;

}

void GM_RSSP1_CFM_Report_com_State_To_SFM(GM_RSSP1_CFM_object_struct *pCFM)
{
	GM_RSSP1_CFM_connection_struct *pCFM_con = NULL;
	GM_RSSP1_INT32 con_index = 0;
	GM_RSSP1_INT32 chn_index = 0;
	GM_RSSP1_CFM_L2U_pri_struct pri = {0};
	GM_RSSP1_Write_Q_Return_Enum write_q_rt = Q_WRITE_FAIL;

	if (NULL == pCFM)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Report_com_State_To_SFM_Param_Point_Error, pCFM, 0, 0, 0, 0, 0);
		return;
	}

	if (GM_RSSP1_Lib_State_Operational != pCFM->lib_state)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Report_com_State_To_SFM_State_Fail, pCFM->lib_state, 0, 0, 0, 0, 0);
		return;
	}

	pri.type    = GM_RSSP1_CFM_Indi_com_State;

	pri.index   = 0;    /* 默认为0，不使用 */

	for (con_index = 0 ; con_index < pCFM->connection_nums ; ++con_index)
	{
		pCFM_con = &(pCFM->connection[con_index]);
		pri.as.channel_state_indi.GM_RSSP1_CFM_Channel_states[con_index].index = pCFM_con->index;
		pri.as.channel_state_indi.GM_RSSP1_CFM_Channel_states[con_index].channel_state = 0;

		pri.as.channel_state_indi.GM_RSSP1_CFM_Channel_states[con_index].chn_num = (GM_RSSP1_UINT8)(pCFM_con->chn_num);
		if (pCFM_con->chn_num > 8)
		{
			GM_RSSP1_Log_Msg(0, "FSFB CFM ID:0x%X chn num:%d > %d,cannot report chn state!\n", pCFM_con->SaCEPID.dest_addr, pCFM_con->chn_num , GM_RSSP1_CFM_MAX_CHANNEL_NUM, 0, 0, 0);
			return;
		}

		for (chn_index = 0 ; chn_index < pCFM_con->chn_num ; ++chn_index)
		{
			if (GM_RSSP1_TRUE == pCFM_con->com_chn[chn_index].b_connected)
			{
				pri.as.channel_state_indi.GM_RSSP1_CFM_Channel_states[con_index].channel_state |= (1 << chn_index);
			}
		}

	}

	write_q_rt = FSFB_Msg_Queue_Write(&(g_CFM_MQ.RCV_MQ) , &pri);

	if (Q_WRITE_OK_FULL == write_q_rt)
	{
		GM_RSSP1_Log_Msg(0,"FSFB CFM L2U_pri_Q full , report UDP state fail!\n", 0, 0, 0, 0, 0, 0);
	}
	else if (Q_WRITE_FAIL == write_q_rt)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_CFM_Report_com_State_To_SFM_Write_Fail, 0, 0, 0, 0, 0, 0);
		pCFM->lib_state = GM_RSSP1_Lib_State_Failure;
		return;
	}
	else
	{
	}

	return;
}

/* start:CR:GM00002526, GM162, fulintai,2015-06-09, add this function to creat all 0 message in standby's CFM*/
#ifdef CFM_Stanby_Answer
GM_RSSP1_BOOL GM_RSSP1_Standby_Info(void)
{
	GM_RSSP1_com_pri_struct Sdat = {0};
	GM_RSSP1_CFM_U2L_pri_struct cfm_pri = {0};
	GM_RSSP1_CFM_connection_struct *pCFM_con = NULL;
	GM_RSSP1_Write_Q_Return_Enum write_q_rt = Q_WRITE_FAIL;
	GM_RSSP1_CFM_object_struct *pCFM = GM_RSSP1_Get_CFM_Object();
	Local_Sys_AB_enum sys_a_b = GM_RSSP1_Local_Sys_UNLOWN;
	GM_RSSP1_BOOL b_active = GM_RSSP1_TRUE;
	GM_RSSP1_UINT16 crc16 = 0U;
	GM_RSSP1_UINT32 VSN0 = 0U;
	GM_RSSP1_UINT32 VSN1 = 0U;
	GM_RSSP1_UINT32 VSN2 = 0U;
	GM_RSSP1_UINT8 i =0U;
	
	if (pCFM == NULL)
	{
		return GM_RSSP1_FALSE;
	}

	if (GM_RSSP1_Lib_State_Operational != pCFM->lib_state)
	{
		return GM_RSSP1_FALSE;
	}

	GM_RSSP1_APP_Interface_Get_Syn_AB_AS_Info(&sys_a_b , &b_active);
	if (GM_RSSP1_TRUE == b_active)
	{
		return GM_RSSP1_FALSE;
	}
	
	if (pCFM->all0_size > GM_RSSP1_MAX_USER_DAT_LEN)
	{
		return GM_RSSP1_FALSE;
	}
	
	for(i=0; i<pCFM->connection_nums; i++)
	{
		pCFM_con = &(pCFM->connection[i]);
		
		cfm_pri.type                = GM_RSSP1_CFM_Data_req;
		cfm_pri.index               = pCFM_con->index;
		cfm_pri.as.data_req.type    = GM_RSSP1_MSG_TYPE_RSD;
		cfm_pri.as.data_req.TC      = VSN0;

		GM_RSSP1_VSN_Get_Callback_Fun(&VSN0, &VSN1, &VSN2);

		cfm_pri.as.data_req.bytes_count = 22+pCFM->all0_size;
		cfm_pri.as.data_req.byte[0] = 0x02;
		cfm_pri.as.data_req.byte[1] = sys_a_b;
		cfm_pri.as.data_req.byte[2] = (GM_RSSP1_UINT8)(pCFM_con->SaCEPID.source_addr&0x00ff);
		cfm_pri.as.data_req.byte[3] = (GM_RSSP1_UINT8)((pCFM_con->SaCEPID.source_addr&0xff00)>>8);
		cfm_pri.as.data_req.byte[4] = (GM_RSSP1_UINT8)(pCFM_con->SaCEPID.dest_addr&0x00ff);
		cfm_pri.as.data_req.byte[5] = (GM_RSSP1_UINT8)((pCFM_con->SaCEPID.dest_addr&0xff00)>>8);
		cfm_pri.as.data_req.byte[6] = (GM_RSSP1_UINT8)(VSN0 & 0xff);
		cfm_pri.as.data_req.byte[7] = (GM_RSSP1_UINT8)((VSN0 & 0xff00) >> 8);
		cfm_pri.as.data_req.byte[8] = (GM_RSSP1_UINT8)((VSN0 & 0xff0000) >> 16);
		cfm_pri.as.data_req.byte[9] = (GM_RSSP1_UINT8)((VSN0 & 0xff000000) >> 24);
		cfm_pri.as.data_req.byte[10] = (GM_RSSP1_UINT8)((8+pCFM->all0_size) & 0xff);
		cfm_pri.as.data_req.byte[11] = (GM_RSSP1_UINT8)((8+pCFM->all0_size) >> 8);
		GM_RSSP1_memset((cfm_pri.as.data_req.byte+12), 0U, (8+pCFM->all0_size));
		
		GM_RSSP1_CRC_Calculate_CRC16(cfm_pri.as.data_req.byte, (20+pCFM->all0_size), &crc16, (GM_RSSP1_UINT16*)GM_RSSP1_CRC16_Table_0x10811_LSB);

		cfm_pri.as.data_req.byte[20+pCFM->all0_size] = (GM_RSSP1_UINT8)(crc16 & 0xff);
		cfm_pri.as.data_req.byte[21+pCFM->all0_size] = (GM_RSSP1_UINT8)(crc16 >> 8);

		write_q_rt = FSFB_Msg_Queue_Write(&g_CFM_Debug_MQ , &cfm_pri);
		if (Q_WRITE_FAIL == write_q_rt)
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_CFM_User_Send_Dat_Write_Fail, 0, 0, 0, 0, 0, 0);
			return GM_RSSP1_FALSE;
		}
		else if (Q_WRITE_OK_FULL == write_q_rt)
		{
			/* 成功，但队列已满 */
			GM_RSSP1_Log_Msg(0,"FSFB CFM U2L_pri_Q FULL!\n ", 0, 0, 0, 0, 0, 0);
		}
		else
		{
			;
		}
	}

	return GM_RSSP1_TRUE;
}
#endif
/* end */
