#include "GM_RSSP1_MQ_Interface.h"

extern GM_RSSP1_UINT32 g_valid_chn;
RSSP1_MQ_LINK_struct g_Link_MQ[GM_RSSP1_MAX_SAFETY_CONNECTION_NUM] = {0};

RSSP1_MQ_Inter_struct g_CFM_MQ = {0};

RSSP1_MQ_Inter_struct g_SFM_MQ = {0};

#ifdef CFM_Stanby_Answer
GM_RSSP1_Msg_Queue_struct g_CFM_Debug_MQ = {0};
#endif

GM_RSSP1_BOOL GM_RSSP1_SND_com_Interface(GM_RSSP1_UINT8* p_dat, GM_RSSP1_UINT16* len, GM_RSSP1_UINT8 index, GM_RSSP1_UINT8 chn_index)
{
	GM_RSSP1_com_pri_struct  temp_send_pkt= {0};
	GM_RSSP1_BOOL write_q_rt = GM_RSSP1_FALSE;

	if ((p_dat == NULL)||(len == NULL)||(index>=GM_RSSP1_MAX_SAFETY_CONNECTION_NUM)||(chn_index>=GM_RSSP1_MAX_LINK_CHANNEL))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SND_com_Interface_Param_Val_Error, p_dat, len, index, chn_index, GM_RSSP1_MAX_SAFETY_CONNECTION_NUM, GM_RSSP1_MAX_LINK_CHANNEL);
		return GM_RSSP1_FALSE;
	}
	write_q_rt = FSFB_Msg_Queue_Get(&(g_Link_MQ[index].RCV_SND_MQ[chn_index].SND_MQ),&temp_send_pkt);
	if (GM_RSSP1_TRUE == write_q_rt)
	{
		GM_RSSP1_memcpy(p_dat, temp_send_pkt.byte, temp_send_pkt.bytes_count);
		*len = temp_send_pkt.bytes_count;
		return GM_RSSP1_TRUE;
	}
	else
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_SND_com_Interface_Write_Fail, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
}

GM_RSSP1_BOOL GM_RSSP1_RCV_com_Interface(GM_RSSP1_UINT8* p_dat, GM_RSSP1_INT16 recv_len, GM_RSSP1_UINT8 index, GM_RSSP1_UINT8 chn_index)
{
	GM_RSSP1_Write_Q_Return_Enum write_q_rt = Q_WRITE_FAIL;
	GM_RSSP1_com_pri_struct pri = {0};

	if ((recv_len > GM_RSSP1_MAX_SAFETY_DAT_LEN)||(p_dat ==NULL)||(index>=GM_RSSP1_MAX_SAFETY_CONNECTION_NUM)||(chn_index>=GM_RSSP1_MAX_LINK_CHANNEL))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_RCV_com_Interface_Param_Val_Error, recv_len, index, p_dat, chn_index, p_dat, 0);
		return GM_RSSP1_FALSE;
	}

	pri.bytes_count = recv_len;
	GM_RSSP1_memcpy((void*)pri.byte , p_dat , recv_len);

	write_q_rt = FSFB_Msg_Queue_Write(&(g_Link_MQ[index].RCV_SND_MQ[chn_index].RCV_MQ), &pri);
	if (Q_WRITE_FAIL == write_q_rt)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_RCV_com_Interface_Write_Fail, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	else
	{
		return GM_RSSP1_TRUE;
	}
}


GM_RSSP1_BOOL GM_RSSP1_MessageQueue_Inter_Set(GM_RSSP1_MQ_Type_Enum type, GM_RSSP1_UINT8* pMsg)
{
	GM_RSSP1_Write_Q_Return_Enum write_q_rt = Q_WRITE_FAIL;
	GM_RSSP1_BOOL rt = GM_RSSP1_FALSE;

	if (pMsg == NULL)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_MessageQueue_Inter_Set_Param_Point_Error, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	switch(type)
	{
		case RSSP1_S2C_SND:
			write_q_rt = FSFB_Msg_Queue_Write(&(g_CFM_MQ.SND_MQ) , pMsg);
			if (Q_WRITE_FAIL == write_q_rt)
			{
				GM_RSSP1_Log_Usr(GM_RSSP1_MessageQueue_Inter_Set_Write_Fail, RSSP1_S2C_SND, 0, 0, 0, 0, 0);
				return GM_RSSP1_FALSE;
			}
			else
			{
				rt = GM_RSSP1_TRUE;
			}
			break;

		case RSSP1_C2S_RCV:
			write_q_rt = FSFB_Msg_Queue_Write(&(g_CFM_MQ.RCV_MQ) , pMsg);
			if (Q_WRITE_FAIL == write_q_rt)
			{
				GM_RSSP1_Log_Usr(GM_RSSP1_MessageQueue_Inter_Set_Write_Fail, RSSP1_C2S_RCV, 0, 0, 0, 0, 0);
				return GM_RSSP1_FALSE;
			}
			else
			{
				rt = GM_RSSP1_TRUE;
			}
			break;
		default:
			GM_RSSP1_Log_Usr(GM_RSSP1_MessageQueue_Inter_Set_Write_Fail, 0, 0, 0, 0, 0, 0);
			rt = GM_RSSP1_FALSE;
			break;
	}
	return rt;
}

/*CR:GM00002527, GM162, fulintai,2013-06-09, much more return type for different exit*/
GM_RSSP1_INT8 GM_RSSP1_MessageQueue_Inter_Get(GM_RSSP1_MQ_Type_Enum type, GM_RSSP1_UINT8* pMsg, GM_RSSP1_UINT32* len)
{
	GM_RSSP1_BOOL write_q_rt = GM_RSSP1_FALSE;
	GM_RSSP1_INT8 res = 0;
	GM_RSSP1_CFM_U2L_pri_struct S2C_Msg = {0};
	GM_RSSP1_CFM_L2U_pri_struct C2S_Msg = {0};
	GM_RSSP1_UINT32 count = 0U;
	GM_RSSP1_UINT32 lenth = 0U;

	if ((pMsg == NULL)||(len == NULL))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_MessageQueue_Inter_Get_Param_Point_Error, pMsg, len, 0, 0, 0, 0);
		return -1;
	}

	lenth = sizeof(GM_RSSP1_CFM_L2U_type_enum)+sizeof(GM_RSSP1_UINT8);

	switch(type)
	{
		case RSSP1_S2C_SND:
			write_q_rt = FSFB_Msg_Queue_Get(&(g_CFM_MQ.SND_MQ) , &S2C_Msg);
			if (GM_RSSP1_FALSE == write_q_rt)
			{
				GM_RSSP1_Log_Usr(GM_RSSP1_MessageQueue_Inter_Get_Write_Fail, RSSP1_S2C_SND, 0, 0, 0, 0, 0);
				return 0;
			}
			lenth = sizeof(GM_RSSP1_CFM_U2L_pri_struct)-GM_RSSP1_MAX_SAFETY_DAT_LEN+S2C_Msg.as.data_req.bytes_count;
			GM_RSSP1_memcpy(pMsg, &S2C_Msg, lenth);
			*len = lenth;
			res = 1;
			break;

		case RSSP1_C2S_RCV:
			write_q_rt = FSFB_Msg_Queue_Get(&(g_CFM_MQ.RCV_MQ) , &C2S_Msg);
			if (GM_RSSP1_FALSE == write_q_rt)
			{
				GM_RSSP1_Log_Usr(GM_RSSP1_MessageQueue_Inter_Get_Write_Fail, RSSP1_C2S_RCV, 0, 0, 0, 0, 0);
				return 0;
			}
			switch(C2S_Msg.type)
			{
				case GM_RSSP1_CFM_Data_Ind:
					lenth += sizeof(GM_RSSP1_CFM_L2U_Dat_pri_struct)-GM_RSSP1_MAX_SAFETY_DAT_LEN+C2S_Msg.as.data_indi.bytes_count;
					GM_RSSP1_memcpy(pMsg, &C2S_Msg, lenth);
					*len = lenth;
					res = 1;
					break;

				case GM_RSSP1_CFM_Indi_com_State:
					lenth += sizeof(GM_RSSP1_CFM_L2U_State_Noti_struct)/GM_RSSP1_MAX_SAFETY_CONNECTION_NUM*g_valid_chn;
					GM_RSSP1_memcpy(pMsg, &C2S_Msg, lenth);
					*len = lenth;
					res = 1;
					break;

				case GM_RSSP1_CFM_Indi_Error_Noti:
					lenth += sizeof(GM_RSSP1_CFM_L2U_Error_Noti_struct);
					*len = lenth;
					GM_RSSP1_memcpy(pMsg, &C2S_Msg, lenth);
					res = 1;
					break;

				default:
					res = -1;
					break;
			}
			break;
		default:
			GM_RSSP1_Log_Usr(GM_RSSP1_MessageQueue_Inter_Get_Write_Fail, 0, 0, 0, 0, 0, 0);
			res = -1;
			break;
	}
	return res;
}