
#include "GM_RSSP1_Syn.h"
#include "GM_RSSP1_APP_Interface.h"

static GM_RSSP1_UINT8* synData = NULL;
extern RSSP1_MQ_Inter_struct g_SFM_MQ;



GM_RSSP1_BOOL GM_RSSP1_Syn_Send_Fsfb_State_To_Peer(Local_Sys_AB_enum sys_a_b , GM_RSSP1_BOOL b_is_active)
{
    GM_RSSP1_SFM_connection_struct *pSFM_con = NULL;
    GM_RSSP1_INT32 con_index = 0;
	GM_RSSP1_Syn_Info_struct syn_info = {0};
    GM_RSSP1_UINT8 syn_dat[GM_RSSP1_MAX_SAFETY_DAT_LEN] = {0};
    GM_RSSP1_UINT16 state_dat_len = 0;
    GM_RSSP1_UINT16 crc16 = 0U;
	GM_RSSP1_SACEPID_struct peer_syn_sacepid = {0};
    GM_RSSP1_BOOL rt = GM_RSSP1_FALSE;
	GM_RSSP1_CFM_object_struct *pFSFB_CFM = NULL;
	GM_RSSP1_SFM_object_struct *pSFM = NULL;

	pSFM = GM_RSSP1_Get_SFM_Object();
	pFSFB_CFM = GM_RSSP1_Get_CFM_Object();
	
    if ((NULL == pSFM)||(NULL == pFSFB_CFM))
    {
        return GM_RSSP1_FALSE;
    }

    if (GM_RSSP1_Lib_State_Operational != pSFM->lib_state)
    {
        return GM_RSSP1_FALSE;
    }

    if (1U >= pSFM->connection_nums)
    {
        /* 只有一个连接，不可能存在同步通道 */
        return GM_RSSP1_FALSE;
    }

    peer_syn_sacepid.source_addr    = pSFM->source_addr;
    peer_syn_sacepid.dest_addr      = GM_RSSP1_SYN_DEST_ADDR;

    syn_dat[0] = INTERFACE_TYPE_SYN;
    syn_dat[1] = INTERFACE_DAT_TYPE_SYN_RSSP1_STATUS_DAT;
	state_dat_len += 2;
	GM_RSSP1_memcpy(syn_dat + state_dat_len, &peer_syn_sacepid,  sizeof(GM_RSSP1_SACEPID_struct));
	state_dat_len += sizeof(GM_RSSP1_SACEPID_struct);

    for (con_index = 0 ; con_index < pSFM->connection_nums ; ++con_index)
    {
        pSFM_con = &(pSFM->connection[con_index]);

        if (GM_RSSP1_FALSE == GM_RSSP1_SFM_Is_SaCEPID_Equal(&peer_syn_sacepid , &(pSFM_con->SaCEPID)))
        {
            syn_info.sacepid = pSFM_con->SaCEPID;
            syn_info.time_TC    = pSFM_con->time.TC;
            syn_info.time_TS[0] = pSFM_con->time.TS[0];
            syn_info.time_TS[1] = pSFM_con->time.TS[1];
            syn_info.time_oldTS[0] = pSFM_con->time.oldTS[0];
            syn_info.time_oldTS[1] = pSFM_con->time.oldTS[1];
            syn_info.env[0] = pSFM_con->env[0];
            syn_info.env[1] = pSFM_con->env[1];

            /*added by huang 20151215*/
            syn_info.PreciTime = pSFM_con->PreciTime;
			syn_info.SSE_SSR_Delay = pSFM_con->SSE_SSR_Delay;
            syn_info.SINIT_TremCycle = pSFM_con->SINIT_TremCycle;
            syn_info.RxSSRTrem = pSFM_con->RxSSRTrem;
			syn_info.RxSSRTloc = pSFM_con->RxSSRTloc;
            syn_info.LastSendSSECycle = pSFM_con->LastSendSSECycle;
            syn_info.TcycleLoc = pSFM_con->TcycleLoc;
            syn_info.TcycleRem = pSFM_con->TcycleRem;
            /*the end 20151215*/
            GM_RSSP1_memcpy(syn_dat + state_dat_len , &syn_info , sizeof(syn_info));
            state_dat_len += sizeof(syn_info);
        }
    }

    /* add crc16 at tail, send to peer  */
    GM_RSSP1_CRC_Calculate_CRC16(syn_dat, state_dat_len, &crc16, GM_RSSP1_CRC16_Table_0x11021_MSB);

    syn_dat[state_dat_len] = (crc16 & 0xff00) >> 8;
	state_dat_len += 1;
    syn_dat[state_dat_len] = (crc16 & 0xff);
	state_dat_len += 1;

    /* send syn dat to peer via FSFB syn channel.CRC checksum not used here */
	for (con_index = 0 ; con_index < pSFM->connection_nums ; ++con_index)
	{
		pSFM_con = &(pSFM->connection[con_index]);

		if (GM_RSSP1_TRUE == GM_RSSP1_SFM_Is_SaCEPID_Equal(&peer_syn_sacepid , &(pSFM_con->SaCEPID)))
		{
			GM_RSSP1_CFM_User_Send_Dat(&peer_syn_sacepid , syn_dat , state_dat_len , pFSFB_CFM);
		}
	}
	return rt;
}

void GM_RSSP1_Syn_Send_App_To_Peer(GM_RSSP1_UINT8* pdate, GM_RSSP1_UINT16 len)
{
    GM_RSSP1_INT32  i = 0;
    GM_RSSP1_UINT16 crc16 = 0U;
    GM_RSSP1_INT32  con_index = 0;
    GM_RSSP1_UINT8 dat_to_peer[GM_RSSP1_MAX_SAFETY_DAT_LEN] = {0};
    GM_RSSP1_SACEPID_struct peer_syn_sacepid = {0};
    GM_RSSP1_SFM_connection_struct *pSFM_con = NULL;
	GM_RSSP1_CFM_object_struct *pFSFB_CFM = NULL;
	GM_RSSP1_SFM_object_struct *pFSFB_SFM = NULL;
	GM_RSSP1_UINT16 byte_count = 0;

    pFSFB_CFM = GM_RSSP1_Get_CFM_Object();
	pFSFB_SFM = GM_RSSP1_Get_SFM_Object();

	if ((NULL == pFSFB_SFM) || (NULL == pFSFB_CFM)||(NULL == pdate))
	{
		return;
	}

	
    i = 0;

	byte_count = len - 6;/*接收端应用组包包头*/
    /*9: Flag(2) + GM_RSSP1_SACEPID_struct(4) + bytes_count(2) + crc16(2)*/
    if ((len + 10) > GM_RSSP1_MAX_SAFETY_DAT_LEN)
    {
        return;
    }
 
    /* syn pkt type */
    dat_to_peer[i] = INTERFACE_TYPE_SYN;
	i += 1;
    dat_to_peer[i] = INTERFACE_DAT_TYPE_SYN_RSSP1_ACTIVE_DAT;
	i += 1;

    peer_syn_sacepid.source_addr    = pFSFB_SFM->source_addr;

    peer_syn_sacepid.dest_addr      = (GM_RSSP1_UINT16)(pdate[4] * 256U + pdate[5]);

    GM_RSSP1_memcpy(dat_to_peer + i, &peer_syn_sacepid,  sizeof(GM_RSSP1_SACEPID_struct));

    i += sizeof(GM_RSSP1_SACEPID_struct);

    peer_syn_sacepid.source_addr    = pFSFB_SFM->source_addr;

    peer_syn_sacepid.dest_addr      = GM_RSSP1_SYN_DEST_ADDR;

    GM_RSSP1_memcpy(dat_to_peer+i, &byte_count, sizeof(GM_RSSP1_UINT16));
 
    i += sizeof(GM_RSSP1_UINT16);

	GM_RSSP1_memcpy(dat_to_peer+i, pdate+6, byte_count);

 
    i += byte_count;
 
    /* add crc16 */
	GM_RSSP1_CRC_Calculate_CRC16(dat_to_peer, i, &crc16, GM_RSSP1_CRC16_Table_0x11021_MSB);

    dat_to_peer[i] = (crc16 & 0xff00) >> 8;
	i += 1;
    dat_to_peer[i]   = (crc16 & 0xff);
	i += 1;
    for (con_index = 0 ; con_index < pFSFB_SFM->connection_nums ; ++con_index)
    {
        pSFM_con = &(pFSFB_SFM->connection[con_index]);
 
        if (GM_RSSP1_TRUE == GM_RSSP1_SFM_Is_SaCEPID_Equal(&peer_syn_sacepid , &(pSFM_con->SaCEPID)))
        {
            GM_RSSP1_CFM_User_Send_Dat(&peer_syn_sacepid , dat_to_peer , i , pFSFB_CFM);
        }
    }
}

void GM_RSSP1_Syn_Proc_Rcvd_Syn_Dat(GM_RSSP1_UINT8 *pDat , GM_RSSP1_INT32 dat_len)
{
    GM_RSSP1_Syn_Info_struct syn_info = {0};      /* 同步过来的连接状态数据 */
    GM_RSSP1_UINT8 interface_type = 0U;
    GM_RSSP1_UINT8 syn_dat_type = 0U;
    GM_RSSP1_INT32 syn_con_index = 0;
    GM_RSSP1_INT32 syn_con_nums = 0;   /* 同步过来的连接数 */
    GM_RSSP1_SFM_connection_struct *pSFM_con = NULL;
    GM_RSSP1_SFM_object_struct *pSFM = NULL;
    GM_RSSP1_UINT16 crc16 = 0U;
    GM_RSSP1_UINT16 rcvd_crc16 = 0U;
    GM_RSSP1_Write_Q_Return_Enum write_q_rt = Q_WRITE_FAIL;
    GM_RSSP1_SFM_L2U_pri_struct dat_pri = {0};
    GM_RSSP1_INT32  i = 0;
    GM_RSSP1_UINT32 cal_crc32 = 0U;
    GM_RSSP1_SACEPID_struct peer_syn_sacepid = {0};

    pSFM = GM_RSSP1_Get_SFM_Object();
    if ((pDat == NULL) ||(pSFM == NULL))
    {
        return;
    }

    interface_type = *(pDat);
    i++;
    if((INTERFACE_TYPE_SYN != interface_type)||(dat_len<0))
    {
        GM_RSSP1_Log_Msg(0,"RSSP1 syn rcv interface type err:0x%X.\n",interface_type,0,0,0,0,0);
        return;
    }
    
    /* check crc16 first */
    GM_RSSP1_CRC_Calculate_CRC16(pDat, dat_len - 2, &crc16, GM_RSSP1_CRC16_Table_0x11021_MSB);
    rcvd_crc16 = ((*(pDat + dat_len - 2)) << 8) + *(pDat + dat_len - 1);

    if (rcvd_crc16 != crc16)
    {
        GM_RSSP1_Log_Msg(0,"FSFB rcvd Syn dat from peer,%d bytes , CRC16 err! rcv:0x%X,cal:0x%X.\n", dat_len, rcvd_crc16, crc16, 0, 0, 0);
        return;
    }

    syn_dat_type = *(pDat + 1);
    i++;
    switch (syn_dat_type)
    {
        case INTERFACE_DAT_TYPE_SYN_RSSP1_STATUS_DAT:
            GM_RSSP1_memcpy(&peer_syn_sacepid, (pDat + i), sizeof(GM_RSSP1_SACEPID_struct));
            i += sizeof(GM_RSSP1_SACEPID_struct);

            for (syn_con_index = 0 ; syn_con_index < pSFM->connection_nums ; ++syn_con_index)
            {
                GM_RSSP1_memcpy(&syn_info , pDat + i + syn_con_index * sizeof(syn_info) , sizeof(syn_info));
                pSFM_con = &(pSFM->connection[syn_con_index]);
                if (GM_RSSP1_FALSE == GM_RSSP1_SFM_Is_SaCEPID_Equal(&peer_syn_sacepid , &(pSFM_con->SaCEPID)))
                {
                    /* if local is standby,set local fsfb connection state. */
                    pSFM_con->time.TC       = syn_info.time_TC;
                    pSFM_con->time.TS[0]    = syn_info.time_TS[0];
                    pSFM_con->time.TS[1]    = syn_info.time_TS[1];
                    pSFM_con->time.oldTS[0] = syn_info.time_oldTS[0];
                    pSFM_con->time.oldTS[1] = syn_info.time_oldTS[1];
                    pSFM_con->env[0] = syn_info.env[0];
                    pSFM_con->env[1] = syn_info.env[1];
                    /*added by huang 20151215*/
                    pSFM_con->PreciTime = syn_info.PreciTime;
					pSFM_con->SSE_SSR_Delay = syn_info.SSE_SSR_Delay;
                    pSFM_con->SINIT_TremCycle = syn_info.SINIT_TremCycle;
                    pSFM_con->RxSSRTrem = syn_info.RxSSRTrem;
					pSFM_con->RxSSRTloc = syn_info.RxSSRTloc;
                    pSFM_con->LastSendSSECycle = syn_info.LastSendSSECycle;
                    pSFM_con->TcycleLoc = syn_info.TcycleLoc;
                    pSFM_con->TcycleRem = syn_info.TcycleRem;
                    /*the end 20151215*/
                }
            }
            break;
			
        case INTERFACE_DAT_TYPE_SYN_RSSP1_ACTIVE_DAT:
            /*9: Flag(1) + FSFB_SACEPID_struct(4) + bytes_count(2) + crc16(2)*/

            GM_RSSP1_memset(&dat_pri, 0, sizeof(dat_pri));
            dat_pri.type = GM_RSSP1_SFM_Active_Data_Ind;

            GM_RSSP1_memcpy(&(dat_pri.SaCEPID), (pDat + i), sizeof(GM_RSSP1_SACEPID_struct));
            i += sizeof(GM_RSSP1_SACEPID_struct);

            pSFM_con = GM_RSSP1_SFM_Get_Connection_BY_SaCEPID(&(dat_pri.SaCEPID) , pSFM);
            if (NULL != pSFM_con)
            {
            /*
            pSFM_con->b_rcvd_new_dat_in_cycle = GM_RSSP1_TRUE;
            pSFM_con->b_torlerant_dat_valid = GM_RSSP1_TRUE;
            pSFM_con->torlerate_cycle_count1 = 0;
            pSFM_con->torlerate_cycle_count2 = (GM_RSSP1_INT8)pSFM_con->torlerate_cycle_cfg;
            pSFM_con->state_check_counter1 = 0;
            pSFM_con->state_check_counter2 = (GM_RSSP1_INT8)pSFM_con->deltaTime;
            */
                if (dat_len > GM_RSSP1_MAX_SAFETY_DAT_LEN)
                {
                    return;
                }

                dat_pri.as.data_indi.bytes_count = (*(GM_RSSP1_UINT16*)(pDat + i));
                i += sizeof(GM_RSSP1_UINT16);
/*#ifdef OFFLINE_TOOL_SUPPORT
                dat_pri.as.data_indi.bytes_count = dat_pri.as.data_indi.bytes_count - (sizeof(GM_RSSP1_UINT32) * 2);
#endif
*/
                GM_RSSP1_memcpy((void*)dat_pri.as.data_indi.byte, (void*)(pDat + i), dat_pri.as.data_indi.bytes_count);
                i += dat_pri.as.data_indi.bytes_count;
/*                GM_RSSP1_memcpy((void*)dat_pri.as.data_indi.CRC, (void*)(pDat + i), sizeof(GM_RSSP1_UINT32)*2);
				i += sizeof(GM_RSSP1_UINT32)*2;

            /*@放入上层队列**/
                if (GM_RSSP1_TRUE == pSFM_con->b_enable_FSFB_on_chn)
                {
                    /* 直接储存到本通道内的队列 */
                    write_q_rt = FSFB_Msg_Queue_Write(&(pSFM_con->L2U_dat_Q) , &dat_pri);
                }
                else
                {
                    /* 存储到SFM的队列 */
                    write_q_rt = FSFB_Msg_Queue_Write(&(g_SFM_MQ.RCV_MQ/*pSFM->L2U_pri_Q*/) , &dat_pri);
                }
            }
            break;

        default:
            GM_RSSP1_Log_Msg(0,"FSFB SFM rcv unknow syn dat type:0x%X.\n", syn_dat_type , 0, 0, 0, 0, 0);
            break;
    }
    return;
}
/*CR:GM00003786  Function set/get ---update the synchronization structure include usrdata 2017-6-10*/
GM_RSSP1_BOOL GM_RSSP1_Syn_Data_Get(GM_RSSP1_UINT32* pdata, GM_RSSP1_UINT32* len)
{
	GM_RSSP1_SFM_object_struct *pSFM = NULL;
	GM_RSSP1_SFM_connection_struct *pSFM_con = NULL;
	GM_RSSP1_INT32 con_index = 0;
	GM_RSSP1_Syn_Extern_Info_struct syn_info = {0};
	GM_RSSP1_INT32 state_dat_len = 0;
	GM_RSSP1_SACEPID_struct peer_syn_sacepid = {0};
	GM_RSSP1_UINT16 msg_num = 0U;
	static GM_RSSP1_BOOL FirstCallFlag = GM_RSSP1_TRUE;
	static GM_RSSP1_UINT32 synDataSize = 0U;

	pSFM = GM_RSSP1_Get_SFM_Object();
	if (NULL == pSFM)
	{
		return GM_RSSP1_FALSE;
	}

	if (GM_RSSP1_Lib_State_Operational != pSFM->lib_state)
	{
		return GM_RSSP1_FALSE;
	}

	if (0U == pSFM->connection_nums)
	{
		/* 只有一个连接，不可能存在同步通道 */
		return GM_RSSP1_FALSE;
	}

	if (FirstCallFlag == GM_RSSP1_TRUE)
	{
		synDataSize = pSFM->connection_nums * sizeof(GM_RSSP1_Syn_Extern_Info_struct);
		synData = (GM_RSSP1_UINT8*)GM_RSSP1_malloc((size_t)synDataSize);

		if (synData == NULL)
		{
			return GM_RSSP1_FALSE;
		}
		FirstCallFlag = GM_RSSP1_FALSE;
	}

	GM_RSSP1_memset(synData, 0, synDataSize);

	peer_syn_sacepid.source_addr    = pSFM->source_addr;
	peer_syn_sacepid.dest_addr      = GM_RSSP1_SYN_DEST_ADDR;

	for (con_index = 0 ; con_index < pSFM->connection_nums ; ++con_index)
	{
		pSFM_con = &(pSFM->connection[con_index]);

		if (GM_RSSP1_FALSE == GM_RSSP1_SFM_Is_SaCEPID_Equal(&peer_syn_sacepid , &(pSFM_con->SaCEPID)))
		{
			GM_RSSP1_memset(&syn_info, 0, sizeof(GM_RSSP1_Syn_Extern_Info_struct));
			syn_info.base_info.index = pSFM_con->index;
			syn_info.base_info.sacepid = pSFM_con->SaCEPID;
			syn_info.base_info.time_TC    = pSFM_con->time.TC;
			syn_info.base_info.time_TS[0] = pSFM_con->time.TS[0];
			syn_info.base_info.time_TS[1] = pSFM_con->time.TS[1];
			syn_info.base_info.time_oldTS[0] = pSFM_con->time.oldTS[0];
			syn_info.base_info.time_oldTS[1] = pSFM_con->time.oldTS[1];
			syn_info.base_info.remote_dev_AS_state = pSFM_con->remote_dev_AS_state;
			GM_RSSP1_memcpy(syn_info.base_info.env, pSFM_con->env, GM_RSSP1_CHECK_CHN_NUM*(sizeof(GM_RSSP1_CHN_ENV)));
			/*added by huang 20151215*/
			syn_info.base_info.PreciTime = pSFM_con->PreciTime;
			syn_info.base_info.SSE_SSR_Delay = pSFM_con->SSE_SSR_Delay;
			syn_info.base_info.SINIT_TremCycle = pSFM_con->SINIT_TremCycle;
			syn_info.base_info.RxSSRTrem = pSFM_con->RxSSRTrem;
			syn_info.base_info.RxSSRTloc = pSFM_con->RxSSRTloc;
			syn_info.base_info.LastSendSSECycle = pSFM_con->LastSendSSECycle;
			syn_info.base_info.TcycleLoc = pSFM_con->TcycleLoc;
			syn_info.base_info.TcycleRem = pSFM_con->TcycleRem;
			/*the end 20151215*/
			syn_info.state = pSFM_con->state;
			
			syn_info.b_rcvd_new_dat_in_cycle = pSFM_con->b_rcvd_new_dat_in_cycle;
			syn_info.b_torlerant_dat_valid = pSFM_con->b_torlerant_dat_valid;
			syn_info.torlerate_cycle_count1 = pSFM_con->torlerate_cycle_count1;
			syn_info.torlerate_cycle_count2 = pSFM_con->torlerate_cycle_count2;
			syn_info.state_check_counter1 = pSFM_con->state_check_counter1;
			syn_info.state_check_counter2 = pSFM_con->state_check_counter2;
#ifndef GM_RSSP1_SYN_NO_DATA
			GM_RSSP1_memcpy(&syn_info.last_valid_dat, &(pSFM_con->last_valid_dat_pri), sizeof(GM_RSSP1_SFM_L2U_pri_struct));
#endif
			GM_RSSP1_memcpy(synData+state_dat_len , &syn_info , sizeof(GM_RSSP1_Syn_Extern_Info_struct));
			state_dat_len += sizeof(GM_RSSP1_Syn_Extern_Info_struct);
		}
	}
	*pdata = synData;
	*len = (GM_RSSP1_UINT32)state_dat_len;
	return GM_RSSP1_TRUE;
}

GM_RSSP1_BOOL GM_RSSP1_Syn_Data_Set(GM_RSSP1_UINT8* pdata, GM_RSSP1_UINT32 dataSize)
{
	GM_RSSP1_Syn_Extern_Info_struct syn_info = {0};      /* 同步过来的连接状态数据 */
	GM_RSSP1_UINT8 interface_type = 0U;
	GM_RSSP1_UINT8 syn_dat_type = 0U;
	GM_RSSP1_INT32 syn_con_index = 0;
	GM_RSSP1_INT32 syn_con_nums = 0;   /* 同步过来的连接数 */
	GM_RSSP1_SFM_object_struct *pSFM = NULL;
	GM_RSSP1_SFM_connection_struct *pSFM_con = NULL;
	GM_RSSP1_UINT16 crc16 = 0U;
	GM_RSSP1_UINT16 rcvd_crc16 = 0U;
	GM_RSSP1_Write_Q_Return_Enum write_q_rt = Q_WRITE_FAIL;
	GM_RSSP1_SFM_L2U_pri_struct dat_pri = {0};
	GM_RSSP1_INT32  i = 0;
	GM_RSSP1_UINT32 cal_crc32 = 0U;

	pSFM = GM_RSSP1_Get_SFM_Object();
	if ((NULL == pSFM)||(dataSize <sizeof(GM_RSSP1_Syn_Extern_Info_struct)))
	{
		return GM_RSSP1_FALSE;
	}

	if (GM_RSSP1_Lib_State_Operational != pSFM->lib_state)
	{
		return GM_RSSP1_FALSE;
	}

	for (syn_con_index = 0 ; syn_con_index < pSFM->connection_nums ; ++syn_con_index)
	{
		GM_RSSP1_memcpy(&syn_info , pdata + syn_con_index * sizeof(GM_RSSP1_Syn_Extern_Info_struct) , sizeof(GM_RSSP1_Syn_Extern_Info_struct));

		pSFM_con = &(pSFM->connection[syn_info.base_info.index]);
		if (GM_RSSP1_TRUE == GM_RSSP1_SFM_Is_SaCEPID_Equal(&(syn_info.base_info.sacepid) , &(pSFM_con->SaCEPID)))
		{
			/* if local is standby,set local fsfb connection state. */
			pSFM_con->time.TC = syn_info.base_info.time_TC;
			pSFM_con->time.TS[0] = syn_info.base_info.time_TS[0];
			pSFM_con->time.TS[1] = syn_info.base_info.time_TS[1];
			pSFM_con->time.oldTS[0] = syn_info.base_info.time_oldTS[0];
			pSFM_con->time.oldTS[1] = syn_info.base_info.time_oldTS[1];
			pSFM_con->remote_dev_AS_state = syn_info.base_info.remote_dev_AS_state;
			GM_RSSP1_memcpy((void*)pSFM_con->env, (void*)syn_info.base_info.env, sizeof(GM_RSSP1_CHN_ENV)*2);
			
			pSFM_con->b_rcvd_new_dat_in_cycle = syn_info.b_rcvd_new_dat_in_cycle;
			pSFM_con->b_torlerant_dat_valid = syn_info.b_torlerant_dat_valid;
			pSFM_con->torlerate_cycle_count1 = syn_info.torlerate_cycle_count1;
			pSFM_con->torlerate_cycle_count2 = syn_info.torlerate_cycle_count2;
			pSFM_con->state_check_counter1 = syn_info.state_check_counter1;
			pSFM_con->state_check_counter2 = syn_info.state_check_counter2;
			/*added by huang 20151215*/
			pSFM_con->PreciTime = syn_info.base_info.PreciTime;
			pSFM_con->SSE_SSR_Delay = syn_info.base_info.SSE_SSR_Delay;
			pSFM_con->SINIT_TremCycle = syn_info.base_info.SINIT_TremCycle;
			pSFM_con->RxSSRTrem = syn_info.base_info.RxSSRTrem;
			pSFM_con->RxSSRTloc = syn_info.base_info.RxSSRTloc;
			pSFM_con->LastSendSSECycle = syn_info.base_info.LastSendSSECycle;
			pSFM_con->TcycleLoc = syn_info.base_info.TcycleLoc;
			pSFM_con->TcycleRem = syn_info.base_info.TcycleRem;
			/*the end 20151215*/
			pSFM_con->state = syn_info.state;
#ifndef GM_RSSP1_SYN_NO_DATA
			GM_RSSP1_memcpy(&(pSFM_con->last_valid_dat_pri), &(syn_info.last_valid_dat), sizeof(GM_RSSP1_SFM_L2U_pri_struct));
#endif
		}
		else
		{
			return GM_RSSP1_FALSE;
		}
	}
	return GM_RSSP1_TRUE;
}
