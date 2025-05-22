#include "GM_RSSP1_APP_Interface.h"
#include "GM_RSSP1_Syn.h"


GM_RSSP_GET_ABAS_FUN GM_RSSP_Resource_Get_ABAS = NULL;

GM_RSSP1_VSN_GET_CALLBACK_FUN GM_RSSP1_VSN_Get_Callback_Fun = NULL;	/*added by huang 20151219*/

GM_RSSP1_BOOL RSSP1_Initialized = GM_RSSP1_FALSE;

GM_RSSP1_UINT32 g_valid_chn = 0;

extern RSSP1_MQ_Inter_struct g_SFM_MQ;

extern RSSP1_MQ_Inter_struct g_CFM_MQ;

extern RSSP1_MQ_LINK_struct g_Link_MQ[GM_RSSP1_MAX_SAFETY_CONNECTION_NUM];

#ifdef OFFLINE_TOOL_SUPPORT
extern GM_RSSP1_UINT32 App_SYSCKW[2];
#endif
/*****************FSFB_Interface_Init**************************************
用途:CBI与TCC接口，初始化
参数:无
返回值: 无
************************************************************************/

GM_RSSP1_BOOL GM_RSSP1_APP_Interface_Init_Addr(GM_RSSP_GET_ABAS_FUN fun, GM_RSSP1_UINT8* SFM_Addr, GM_RSSP1_UINT8* CFM_Addr, GM_RSSP1_VSN_GET_CALLBACK_FUN VSN_Get_Fun)
{
#ifdef SFM_Part
	if (GM_RSSP1_FALSE == GM_RSSP1_SFM_Init_Addr(SFM_Addr))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_APP_Interface_Init_Addr_SFM_FAIL, 0, 0, 0, 0, 0, 0);
		GM_RSSP1_Log_Msg(0,"RSSP-I SFM init FAIL!\n", 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	else
	{
		GM_RSSP1_Log_Msg(0,"RSSP-I SFM init OK.\n", 0, 0, 0, 0, 0, 0);
	}
#endif

#ifdef CFM_Part
	if (GM_RSSP1_FALSE == GM_RSSP1_CFM_Init_Addr(CFM_Addr))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_APP_Interface_Init_Addr_CFM_FAIL, 0, 0, 0, 0, 0, 0);
		GM_RSSP1_Log_Msg(0,"RSSP-I CFM init FAIL!\n", 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	else
	{
		GM_RSSP1_Log_Msg(0,"RSSP-I CFM init OK.\n", 0, 0, 0, 0, 0, 0);
	}
#endif

	GM_RSSP_Resource_Get_ABAS = (GM_RSSP_GET_ABAS_FUN)fun;

	/* added by huang 20151219*/
	if ( VSN_Get_Fun != NULL)
	{
		GM_RSSP1_VSN_Get_Callback_Fun = VSN_Get_Fun;
	}
	else
	{
		GM_RSSP1_VSN_Get_Callback_Fun = GM_RSSP1_VSN_Get;
	}
	/*the end 20151219*/

	RSSP1_Initialized = GM_RSSP1_TRUE;

	return GM_RSSP1_TRUE;
}

GM_RSSP1_BOOL GM_RSSP1_APP_Interface_Init(GM_RSSP_GET_ABAS_FUN fun, char* path, GM_RSSP1_BOOL is_path, GM_RSSP1_VSN_GET_CALLBACK_FUN VSN_Get_Fun)
{
	GM_RSSP1_INT32 file_len = -1;
	GM_RSSP1_INT8 *pIniFileContent = NULL; /**< 存储Ini文件内容, 动态分配, 使用完之后释放 */
	GM_RSSP1_BOOL rt = GM_RSSP1_FALSE;

	if(NULL == path)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_APP_Interface_Init_Param_Point_Error, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	
	if (GM_RSSP1_TRUE == is_path)
	{
		/* 为读文件分配内存 */
		file_len = GM_RSSP1_Get_File_Size(path);

		if (0>=file_len)
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_APP_Interface_Init_FILE_Size_Error, 0, 0, 0, 0, 0, 0);
			return GM_RSSP1_FALSE;
		}

		pIniFileContent = (char *)GM_RSSP1_malloc(file_len + 1);

		if (NULL == pIniFileContent)
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_APP_Interface_Init_Memory_Error, 0, 0, 0, 0, 0, 0);
			return GM_RSSP1_FALSE;
		}

		GM_RSSP1_memset((void*)pIniFileContent, 0, file_len + 1);

		/** @brief 初始化ini文件, 即读取其中的内容 */

		rt = GM_RSSP1_Ini_File_Load(path, (char*)pIniFileContent, file_len + 1);

		if (GM_RSSP1_FALSE == rt)
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_APP_Interface_Init_FILE_Load_Error, 0, 0, 0, 0, 0, 0);
			GM_RSSP1_free((void*)pIniFileContent);
			pIniFileContent = NULL;
			return GM_RSSP1_FALSE;
		}
	}
	else
	{
		pIniFileContent = (GM_RSSP1_INT8*) path;
	}

#ifdef SFM_Part
	if (GM_RSSP1_FALSE == GM_RSSP1_SFM_Init(pIniFileContent))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_APP_Interface_Init_SFM_Init_Error, 0, 0, 0, 0, 0, 0);
		GM_RSSP1_Log_Msg(0,"RSSP-I SFM init FAIL!\n", 0, 0, 0, 0, 0, 0);
		GM_RSSP1_free((void*)pIniFileContent);
		pIniFileContent = NULL;
		return GM_RSSP1_FALSE;
	}
	else
	{
		GM_RSSP1_Log_Msg(0,"RSSP-I SFM init OK.\n", 0, 0, 0, 0, 0, 0);
	}
#endif

#ifdef CFM_Part
	if (GM_RSSP1_FALSE == GM_RSSP1_CFM_Init(pIniFileContent))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_APP_Interface_Init_CFM_Init_Error, 0, 0, 0, 0, 0, 0);
		GM_RSSP1_Log_Msg(0,"RSSP-I CFM init FAIL!\n", 0, 0, 0, 0, 0, 0);
		GM_RSSP1_free((void*)pIniFileContent);
		pIniFileContent = NULL;
		return GM_RSSP1_FALSE;
	}
	else
	{
		GM_RSSP1_Log_Msg(0,"RSSP-I CFM init OK.\n", 0, 0, 0, 0, 0, 0);
	}
#endif

	GM_RSSP_Resource_Get_ABAS = (GM_RSSP_GET_ABAS_FUN)fun;

	/* added by huang 20151219*/
	if ( VSN_Get_Fun != NULL)
	{
		GM_RSSP1_VSN_Get_Callback_Fun = VSN_Get_Fun;
    }
	else
	{
		GM_RSSP1_VSN_Get_Callback_Fun = GM_RSSP1_VSN_Get;
	}
	/*the end 20151219*/

	RSSP1_Initialized = GM_RSSP1_TRUE;

	return GM_RSSP1_TRUE;
}


GM_RSSP1_BOOL GM_RSSP1_APP_Interface_Get_Syn_AB_AS_Info(Local_Sys_AB_enum *p_sys_a_b , GM_RSSP1_BOOL *p_bactive)
{
    GM_RSSP1_UINT16 t_usIPS_ROLE = 0U;
	t_usIPS_ROLE = GM_RSSP_Resource_Get_ABAS();

    if ((ABAS_Type_A_AS == t_usIPS_ROLE) || (ABAS_Type_A_AA == t_usIPS_ROLE) ||
            (ABAS_Type_B_AS == t_usIPS_ROLE) || (ABAS_Type_B_AA == t_usIPS_ROLE))
    {

        if ((ABAS_Type_A_AS == t_usIPS_ROLE) || (ABAS_Type_A_AA == t_usIPS_ROLE))
        {
            *p_sys_a_b = GM_RSSP1_Local_Sys_A;
        }
        else
        {
            *p_sys_a_b = GM_RSSP1_Local_Sys_B;
        }

        *p_bactive = GM_RSSP1_TRUE;
    }
    else if ((ABAS_Type_A_SA == t_usIPS_ROLE) || (ABAS_Type_A_SS == t_usIPS_ROLE) ||
             (ABAS_Type_B_SA == t_usIPS_ROLE) || (ABAS_Type_B_SS == t_usIPS_ROLE))
    {
        if ((ABAS_Type_A_SA == t_usIPS_ROLE) || (ABAS_Type_A_SS == t_usIPS_ROLE))
        {
            *p_sys_a_b = GM_RSSP1_Local_Sys_A;
        }
        else
        {
            *p_sys_a_b = GM_RSSP1_Local_Sys_B;
        }

        *p_bactive = GM_RSSP1_FALSE;
    }
    else
    {
        return GM_RSSP1_FALSE;
    }

    return GM_RSSP1_TRUE;
}

GM_RSSP1_BOOL GM_RSSP1_APP_Interface_Is_Local_ACTIVE(void)
{
    Local_Sys_AB_enum sys_a_b = GM_RSSP1_Local_Sys_UNLOWN;
    GM_RSSP1_BOOL b_active = GM_RSSP1_FALSE;

    if (GM_RSSP1_TRUE == GM_RSSP1_APP_Interface_Get_Syn_AB_AS_Info(&sys_a_b , &b_active))
    {
        return b_active;
    }
    else
    {
        /* 异常，视为备机 */
        return GM_RSSP1_FALSE;
    }

}


GM_RSSP1_BOOL GM_RSSP1_Send_App_Dat(GM_RSSP1_UINT8 *pDat , GM_RSSP1_UINT32 dat_len)
{
	GM_RSSP1_SACEPID_struct dest_sacepid = {0};
    Local_Sys_AB_enum sys_a_b = GM_RSSP1_Local_Sys_UNLOWN;
    GM_RSSP1_BOOL b_active = GM_RSSP1_TRUE;
    GM_RSSP1_BOOL rt = GM_RSSP1_FALSE;
	GM_RSSP1_UINT16 crc16 = 0U;
	GM_RSSP1_UINT16 crc16_in = 0U;

    if ((NULL == pDat) || (dat_len > (GM_RSSP1_MAX_USER_DAT_LEN + 4)))
    {
		GM_RSSP1_Log_Usr(GM_RSSP1_Send_App_Dat_Num_Param_Error, 0, 0, 0, 0, 0, 0);
        return GM_RSSP1_FALSE;
    }

	dest_sacepid.source_addr    = (*(pDat) << 8) + (*(pDat + 1));
	dest_sacepid.dest_addr      = (*(pDat + 2) << 8) + (*(pDat + 3));
	
	if (GM_RSSP1_TRUE == GM_RSSP1_APP_Interface_Get_Syn_AB_AS_Info(&sys_a_b , &b_active))
	{
		rt = GM_RSSP1_User_Send_Dat(&(dest_sacepid) , (pDat+4), (dat_len-4), sys_a_b, b_active, GM_RSSP1_Get_SFM_Object());
		return rt;
	}
	else
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_Send_App_Dat_Num_Result_No_Match, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
}

GM_RSSP1_BOOL GM_RSSP1_APP_Clear_Msg_Queue(void)
{
	GM_RSSP1_SFM_object_struct *pSFM_FSFB = NULL;
	register GM_RSSP1_SFM_connection_struct *pSFM_FSFB_con = NULL;
	GM_RSSP1_UINT16 lv_count0 = 0U;
	GM_RSSP1_BOOL rt = GM_RSSP1_FALSE;

	/*clear FSFB SFM queue*/
	pSFM_FSFB = GM_RSSP1_Get_SFM_Object();

	rt = FSFB_Msg_Queue_Clear(&(g_SFM_MQ.SND_MQ));
	if(GM_RSSP1_FALSE == rt)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_APP_Clear_Msg_Queue_Result_No_Match, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	rt = FSFB_Msg_Queue_Clear(&(g_SFM_MQ.RCV_MQ));   
	if(GM_RSSP1_FALSE == rt)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_APP_Clear_Msg_Queue_Result_No_Match, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	/*every connection Q*/
	for(lv_count0 = 0U; lv_count0 < pSFM_FSFB->connection_nums; lv_count0++)
	{
		if(GM_RSSP1_TRUE  == pSFM_FSFB->connection[lv_count0].b_enable_FSFB_on_chn)
		{
			pSFM_FSFB_con = &(pSFM_FSFB->connection[lv_count0]);
			rt =  FSFB_Msg_Queue_Clear(&(pSFM_FSFB_con->L2U_dat_Q));
			if(GM_RSSP1_FALSE == rt)
			{
				GM_RSSP1_Log_Usr(GM_RSSP1_APP_Clear_Msg_Queue_Result_No_Match, lv_count0, 0, 0, 0, 0, 0);
				return GM_RSSP1_FALSE;
			}
		}
	}
	return GM_RSSP1_TRUE;
}


GM_RSSP1_INT8 GM_RSSP1_APP_Interface_Rcv_App_Dat(GM_RSSP1_UINT8* buf, GM_RSSP1_UINT32* Src, GM_RSSP1_UINT32* len, GM_RSSP1_INT32* count)
{
    GM_RSSP1_SFM_object_struct *pFSFB_SFM = NULL;
    GM_RSSP1_INT32 i = 0;
    GM_RSSP1_SFM_L2U_pri_struct fsfb_pri = {0};
    GM_RSSP1_UINT8 dat[GM_MAX_QUEUE_ELE_LEN] = {0};   /* 预留，防止VLE端对SDM打包P1状态时，增加包头长度后超过GM_MAX_QUEUE_ELE_LEN */
    GM_RSSP1_UINT8 tmp_state_dat[GM_MAX_QUEUE_ELE_LEN] = {0};
    GM_RSSP1_UINT8 warning_dat[GM_MAX_QUEUE_ELE_LEN] = {0};
    GM_RSSP1_INT32 warning_dat_len = 0;
    GM_RSSP1_SACEPID_struct peer_syn_sacepid = {0};
    GM_RSSP1_INT32 dat_len = 0;
    GM_RSSP1_INT32 state_len = 0;
    GM_RSSP1_INT32 p1_pkt_sum= 0;
    GM_RSSP1_INT8 rt = -1;

    if ((buf == NULL)||(Src == NULL)||(len == NULL)||(count == NULL))
    {
        GM_RSSP1_Log_Usr(GM_RSSP1_APP_Interface_Rcv_App_Dat_Param_Point_Error, buf, Src, len, 0, 0, 0);
        return -2;
    }

    pFSFB_SFM = GM_RSSP1_Get_SFM_Object();

    GM_RSSP1_memset(dat , 0 , sizeof(dat));
    GM_RSSP1_memset(warning_dat, 0 , sizeof(warning_dat));
    warning_dat[0]  = INTERFACE_TYPE_RSSP1;
    warning_dat[1]  = INTERFACE_DAT_TYPE_RSSP1_WARNING;
    warning_dat_len = 0;
    peer_syn_sacepid.source_addr    = pFSFB_SFM->source_addr;
    peer_syn_sacepid.dest_addr      = GM_RSSP1_SYN_DEST_ADDR;
    *len = 0U;

    *count = 0;
    if (GM_RSSP1_TRUE == FSFB_Msg_Queue_Get(&(g_SFM_MQ.RCV_MQ) , &fsfb_pri))
    {
        *count = FSFB_Get_Queue_Count(&(g_SFM_MQ.RCV_MQ));
        switch (fsfb_pri.type)
        {

        case GM_RSSP1_SFM_Data_Ind:

            if (fsfb_pri.as.data_indi.bytes_count + 6 > sizeof(dat))
            {
                GM_RSSP1_Log_Usr(GM_RSSP1_APP_Interface_Rcv_App_Dat_MSG_Size_Error, 0, 0, 0, 0, 0, 0);
                return -2;
            }

            if (GM_RSSP1_FALSE == GM_RSSP1_SFM_Is_SaCEPID_Equal(&peer_syn_sacepid , &(fsfb_pri.SaCEPID)))
            {
			
                if (GM_RSSP1_FALSE == GM_RSSP1_APP_Interface_Is_Local_ACTIVE())
                {
                    /* 本机为主机才将数据上传 LKD200000216  */
                    GM_RSSP1_Log_Usr(GM_RSSP1_APP_Interface_Rcv_App_Dat_Standby_Log, 0, 0, 0, 0, 0, 0);
                    return 0;
                }

                /* dat from remote devices.report to VLE */
                dat[0]   = INTERFACE_TYPE_RSSP1;
                dat[1]   = INTERFACE_DAT_TYPE_RSSP1_DAT;
                dat[2]   = (GM_RSSP1_UINT8)((fsfb_pri.SaCEPID.source_addr & 0xff00)>> 8);
                dat[3]   = (GM_RSSP1_UINT8)(fsfb_pri.SaCEPID.source_addr & 0x00ff) ;
                dat[4]   = (GM_RSSP1_UINT8)((fsfb_pri.SaCEPID.dest_addr & 0xff00)>> 8);
                dat[5]   = (GM_RSSP1_UINT8)(fsfb_pri.SaCEPID.dest_addr & 0x00ff) ;
                GM_RSSP1_memcpy(dat + 6 , fsfb_pri.as.data_indi.byte , fsfb_pri.as.data_indi.bytes_count);
#ifdef OFFLINE_TOOL_SUPPORT
                GM_RSSP1_memcpy(dat + 6 + fsfb_pri.as.data_indi.bytes_count, fsfb_pri.as.data_indi.CRC, sizeof(GM_RSSP1_UINT32)*2);
                *len = 6 + fsfb_pri.as.data_indi.bytes_count+sizeof(GM_RSSP1_UINT32)*2;
#else
                *len = 6 + fsfb_pri.as.data_indi.bytes_count;
#endif
                *Src = fsfb_pri.SaCEPID.dest_addr;
                GM_RSSP1_memcpy(buf, dat, (*len));
            }
            else
            {
                /* syn dat from peer
                GM_RSSP1_Syn_Proc_Rcvd_Syn_Dat(fsfb_pri.as.data_indi.byte , fsfb_pri.as.data_indi.bytes_count , pFSFB_SFM);*/
            }
            rt = 1;
            break;

        case GM_RSSP1_SFM_Active_Data_Ind:
            /* dat from remote devices.report to VLE */
                if (GM_RSSP1_TRUE== GM_RSSP1_APP_Interface_Is_Local_ACTIVE())
                {
                        return 0;
                }
            dat[0]   = INTERFACE_TYPE_RSSP1;
            dat[1]   = INTERFACE_DAT_TYPE_RSSP1_DAT;
            dat[2]   = ((fsfb_pri.SaCEPID.source_addr & 0xff00) >> 8);
            dat[3]   = (fsfb_pri.SaCEPID.source_addr & 0x00ff) ;
            dat[4]   = ((fsfb_pri.SaCEPID.dest_addr & 0xff00) >> 8);
            dat[5]   = (fsfb_pri.SaCEPID.dest_addr & 0x00ff) ;
            GM_RSSP1_memcpy((void*)(dat + 6) , fsfb_pri.as.data_indi.byte , fsfb_pri.as.data_indi.bytes_count);

            *len = 6 + fsfb_pri.as.data_indi.bytes_count;	
            *Src = fsfb_pri.SaCEPID.dest_addr;
            GM_RSSP1_memcpy((void*)buf, dat, 6 + fsfb_pri.as.data_indi.bytes_count);					
			rt = 1;
			break;

        case GM_RSSP1_SFM_Connection_State_Indi:

            state_len = 0;
            for (i = 0 ; i < fsfb_pri.as.connection_state.connection_num ; ++i)
            {
                tmp_state_dat[state_len++] = (GM_RSSP1_UINT8)((i & 0xff00) >> 8);
                tmp_state_dat[state_len++] = (GM_RSSP1_UINT8)(i & 0xff);
                tmp_state_dat[state_len++] = (GM_RSSP1_UINT8)((fsfb_pri.as.connection_state.connection_states[i].sacepid.dest_addr & 0xff00) >> 8);
                tmp_state_dat[state_len++] = (GM_RSSP1_UINT8)((fsfb_pri.as.connection_state.connection_states[i].sacepid.dest_addr & 0xff));

                tmp_state_dat[state_len++] = (GM_RSSP1_UINT8)((fsfb_pri.as.connection_state.connection_states[i].state & 0xff000000) >> 24);
                tmp_state_dat[state_len++] = (GM_RSSP1_UINT8)((fsfb_pri.as.connection_state.connection_states[i].state & 0x00ff0000) >> 16);
                tmp_state_dat[state_len++] = (GM_RSSP1_UINT8)((fsfb_pri.as.connection_state.connection_states[i].state & 0x0000ff00) >> 8);
                tmp_state_dat[state_len++] = (GM_RSSP1_UINT8)((fsfb_pri.as.connection_state.connection_states[i].state & 0x000000ff));

                if((state_len + 6 + 8> sizeof(dat)) || (i == (GM_RSSP1_INT32)(fsfb_pri.as.connection_state.connection_num - 1)))
                {
                    dat_len = 0;
                    dat[dat_len++] = INTERFACE_TYPE_RSSP1;
                    dat[dat_len++] = INTERFACE_DAT_TYPE_RSSP1_COM_STATE;
                    dat[dat_len++] = (GM_RSSP1_UINT8)(((state_len/8) & 0xff000000)>>24);
                    dat[dat_len++] = (GM_RSSP1_UINT8)(((state_len/8) & 0x00ff0000)>>16);
                    dat[dat_len++] = (GM_RSSP1_UINT8)(((state_len/8) & 0x0000ff00)>>8);
                    dat[dat_len++] = (GM_RSSP1_UINT8)((state_len/8) & 0x000000ff);
                    GM_RSSP1_memcpy(dat + dat_len , tmp_state_dat , state_len);
                    dat_len += state_len;

                    *Src = 0x11;
                    *len = dat_len;
                    GM_RSSP1_memcpy(buf, dat, dat_len);		

                    state_len = 0;
                    GM_RSSP1_memset(tmp_state_dat , 0 , sizeof(tmp_state_dat));
                }
            }
            rt = 1;
            break;

        case GM_RSSP1_SFM_Warning_Indi:
/*             处理报警信息。收集本周期内产生的所有报警信息，集中发送给vle 
            if(warning_dat_len + 2 + 2 <= sizeof(warning_dat))
            {
                warning_dat[1 + warning_dat_len] = fsfb_pri.as.warning.index;
                warning_dat[1 + warning_dat_len + 1] = fsfb_pri.as.warning.type;
                warning_dat_len += 2;
            }
*/			rt = 0;
			break;


        default:
            GM_RSSP1_Log_Msg(0,"####FSFB USER recvd unknow SFM indi:0x%x.Qcnt:%d.ID:(0x%X,0x%X),idx:%d.\n", fsfb_pri.type, FSFB_Get_Queue_Count(&g_SFM_MQ.RCV_MQ), fsfb_pri.SaCEPID.source_addr, fsfb_pri.SaCEPID.dest_addr, 0, 0);
			rt = 0;
			break;
        }
    }

    *count = FSFB_Get_Queue_Count(&(g_SFM_MQ.RCV_MQ));

    return rt;
}
GM_RSSP1_BOOL GM_RSSP1_APP_Interface_Send_App_Dat(GM_RSSP1_UINT8* pData, GM_RSSP1_INT16 len, GM_RSSP1_UINT16 dest_addr)
{
	GM_RSSP1_BOOL rt =GM_RSSP1_FALSE;
	GM_RSSP1_SFM_object_struct *pFSFB_SFM = NULL;
	GM_RSSP1_UINT8 dat[GM_MAX_QUEUE_ELE_LEN] = {0};   /* 预留，防止VLE端对SDM打包P1状态时，增加包头长度后超过GM_MAX_QUEUE_ELE_LEN */
	GM_RSSP1_UINT16 i = 0U;
	GM_RSSP1_UINT16 source_addr = 0U;
	GM_RSSP1_INT16 dat_len = 0;

	if ((len < 0) || (len > GM_RSSP1_MAX_USER_DAT_LEN))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_APP_Interface_Send_App_Dat_Param_Val_Error, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	pFSFB_SFM = GM_RSSP1_Get_SFM_Object();

	GM_RSSP1_memset((void*)dat, 0, (GM_MAX_QUEUE_ELE_LEN));
	
	source_addr = pFSFB_SFM->source_addr;
	
	dat[0]   = ((source_addr & 0xff00) >> 8);
	dat[1]   = (source_addr & 0x00ff) ;
	dat[2]   = ((dest_addr & 0xff00) >> 8);
	dat[3]   = (dest_addr & 0x00ff) ;
	GM_RSSP1_memcpy(dat + 4 , pData , len);
	dat_len = 4 + len;

	rt = GM_RSSP1_Send_App_Dat(dat , dat_len);

	return rt;
}

/*added by huang 20151201, destAddr 可以从GM_RSSP1_APP_Interface_Rcv_App_Dat()输出的形参Src得到*/
GM_RSSP1_INT32 GM_RSSP1_APP_Interface_GetPreciTime(GM_RSSP1_UINT16 destAddr)
{
	GM_RSSP1_SFM_object_struct *pFSFB_SFM = NULL;
	GM_RSSP1_INT32 connIndex = 0;

	pFSFB_SFM = GM_RSSP1_Get_SFM_Object();

	for (connIndex = 0; connIndex < pFSFB_SFM->connection_nums; connIndex++)
	{
		if ( destAddr == pFSFB_SFM->connection[connIndex].dest_addr)
		{
			return pFSFB_SFM->connection[connIndex].PreciTime;
		}
	}
	
	return -1;
}

GM_RSSP1_BOOL GM_RSSP1_APP_Interface_Disconnect(GM_RSSP1_UINT16 destAddr)
{
	GM_RSSP1_SFM_object_struct *pSFM = NULL;
	GM_RSSP1_SFM_connection_struct *pSFM_conn = NULL;
	GM_RSSP1_INT32 connIndex = 0;
	GM_RSSP1_BOOL rt = GM_RSSP1_FALSE;
	pSFM = GM_RSSP1_Get_SFM_Object();

	for (connIndex = 0; connIndex < pSFM->connection_nums; connIndex++)
	{
		if ( destAddr == pSFM->connection[connIndex].dest_addr)
		{
			pSFM_conn = &(pSFM->connection[connIndex]);
			pSFM_conn->state = GM_RSSP1_Layer_State_Free;

			/* reset tolerate vars */
			pSFM_conn->b_rcvd_new_dat_in_cycle = GM_RSSP1_FALSE;
			GM_RSSP1_memset(&(pSFM_conn->last_valid_dat_pri) , 0 , sizeof(pSFM_conn->last_valid_dat_pri));
			pSFM_conn->b_torlerant_dat_valid = GM_RSSP1_FALSE;

			rt = GM_RSSP1_TRUE;
			break;
		}
	}
	return rt;
}


#ifdef OFFLINE_TOOL_SUPPORT
GM_RSSP1_BOOL GM_RSSP1_APP_Interface_CalcCNF_Init(char* path, GM_RSSP1_BOOL is_path)
{
	GM_RSSP1_INT32 file_len = -1;
	GM_RSSP1_INT8 *pIniFileContent = NULL; /**< 存储Ini文件内容, 动态分配, 使用完之后释放 */
	GM_RSSP1_BOOL rt = GM_RSSP1_FALSE;
	GM_RSSP1_UINT32 len = 0U;
	GM_RSSP1_UINT8 i = 0U;
	GM_RSSP1_UINT8 j = 0U;
	GM_RSSP1_UINT8 k = 0U;
	GM_RSSP1_SFM_object_struct *pFSFB_SFM = GM_RSSP1_Get_SFM_Object();
	GM_RSSP1_BOOL isbig = GM_RSSP1_Code_Is_Big_Endian();


	if ((NULL == path)||(NULL == pFSFB_SFM)||(GM_RSSP1_FALSE == RSSP1_Initialized))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_APP_Interface_CalcCNF_Init_Param_Point_Error, path, pFSFB_SFM, RSSP1_Initialized, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	if (GM_RSSP1_TRUE == is_path)
	{
		/* 为读文件分配内存 */
		file_len = GM_RSSP1_Get_File_Size(path);

		if (0>=file_len)
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_APP_Interface_CalcCNF_Init_FILE_Size_Error, 0, 0, 0, 0, 0, 0);
			return GM_RSSP1_FALSE;
		}

		pIniFileContent = (char *)GM_RSSP1_malloc(file_len + 1);

		if (NULL == pIniFileContent)
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_APP_Interface_CalcCNF_Init_Memory_Error, 0, 0, 0, 0, 0, 0);
			return GM_RSSP1_FALSE;
		}

		GM_RSSP1_memset(pIniFileContent, 0, file_len + 1);


		rt = GM_RSSP1_Bin_File_Load(path, pIniFileContent, file_len + 1);

		if (GM_RSSP1_FALSE == rt)
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_APP_Interface_CalcCNF_Init_FILE_Load_Error, 0, 0, 0, 0, 0, 0);
			GM_RSSP1_free((void*)pIniFileContent);
			pIniFileContent = NULL;
			return GM_RSSP1_FALSE;
		}
	}
	else
	{
		pIniFileContent = (GM_RSSP1_INT8*) path;
		file_len = *(GM_RSSP1_INT32*)pIniFileContent;
		pIniFileContent += sizeof(GM_RSSP1_INT32);
	}

	for (i=0U; i<GM_RSSP1_CHECK_CHN_NUM; ++i)
	{
		for (j=0U; j<pFSFB_SFM->connection_nums; ++j)
		{
			len += sizeof(GM_RSSP1_UINT32)* (pFSFB_SFM->connection[j].deltaTime * 3 + pFSFB_SFM->connection[j].torlerate_cycle_cfg - 1);
			if (len>file_len)
			{
				GM_RSSP1_Log_Usr(GM_RSSP1_APP_Interface_CalcCNF_Init_FILE_Size_Error, 0, 0, 0, 0, 0, 0);
				return GM_RSSP1_FALSE;
			}
			else
 			{
				GM_RSSP1_memcpy(&(pFSFB_SFM->connection[j].fsfb_chn[i].CON_1), pIniFileContent, sizeof(GM_RSSP1_UINT32)*pFSFB_SFM->connection[j].deltaTime);
				pIniFileContent += sizeof(GM_RSSP1_UINT32)*pFSFB_SFM->connection[j].deltaTime;

				GM_RSSP1_memcpy(&(pFSFB_SFM->connection[j].fsfb_chn[i].CON_2), pIniFileContent, sizeof(GM_RSSP1_UINT32)*pFSFB_SFM->connection[j].deltaTime);
				pIniFileContent += sizeof(GM_RSSP1_UINT32)*pFSFB_SFM->connection[j].deltaTime;

				GM_RSSP1_memcpy(&(pFSFB_SFM->connection[j].fsfb_chn[i].CON_3), pIniFileContent, sizeof(GM_RSSP1_UINT32)*pFSFB_SFM->connection[j].deltaTime);
				pIniFileContent += sizeof(GM_RSSP1_UINT32)*pFSFB_SFM->connection[j].deltaTime;

				GM_RSSP1_memcpy(&(pFSFB_SFM->connection[j].fsfb_chn[i].CON_4), pIniFileContent, sizeof(GM_RSSP1_UINT32)* (pFSFB_SFM->connection[j].torlerate_cycle_cfg - 1));
				pIniFileContent += sizeof(GM_RSSP1_UINT32)*(pFSFB_SFM->connection[j].torlerate_cycle_cfg-1);
			}
		}
	}
	if (isbig == GM_RSSP1_FALSE)
	{
		for (i=0; i<GM_RSSP1_CHECK_CHN_NUM; ++i)
		{
			for (j=0U; j<pFSFB_SFM->connection_nums; ++j)
			{
				for (k=0U; k<pFSFB_SFM->connection[j].deltaTime; ++k)
				{
					pFSFB_SFM->connection[j].fsfb_chn[i].CON_1[k] = GM_RSSP1_Code_Big_Little_Endian32(pFSFB_SFM->connection[j].fsfb_chn[i].CON_1[k]);
					pFSFB_SFM->connection[j].fsfb_chn[i].CON_2[k] = GM_RSSP1_Code_Big_Little_Endian32(pFSFB_SFM->connection[j].fsfb_chn[i].CON_2[k]);
					pFSFB_SFM->connection[j].fsfb_chn[i].CON_3[k] = GM_RSSP1_Code_Big_Little_Endian32(pFSFB_SFM->connection[j].fsfb_chn[i].CON_3[k]);					
				}
				for (k=0U; k<pFSFB_SFM->connection[j].torlerate_cycle_cfg; ++k)
				{
					pFSFB_SFM->connection[j].fsfb_chn[i].CON_4[k] = GM_RSSP1_Code_Big_Little_Endian32(pFSFB_SFM->connection[j].fsfb_chn[i].CON_4[k]);					
				}
			}
		}
	}
	return GM_RSSP1_TRUE;
}
GM_RSSP1_BOOL GM_RSSP1_APP_Interface_Get_SysCKW(GM_RSSP1_UINT32 sysckw_1, GM_RSSP1_UINT32 sysckw_2)
{
	App_SYSCKW[0] = sysckw_1;
	App_SYSCKW[1] = sysckw_2;
}
#endif

#ifdef PF2oo3
GM_RSSP1_BOOL GM_RSSP1_2oo3_Conn_Recv_Dat(RSSP_Message_t* msg, GM_RSSP1_UINT8 index, GM_RSSP1_UINT8 chn_index)
{
	GM_RSSP1_com_pri_struct pri;
	GM_RSSP1_Write_Q_Return_Enum write_q_rt = Q_WRITE_FAIL;

	pri.bytes_count = msg->MsgSize;
	GM_RSSP1_memcpy(pri.byte , msg->Message, msg->MsgSize);

	write_q_rt = FSFB_Msg_Queue_Write(&(g_Link_MQ[index].RCV_SND_MQ[chn_index].RCV_MQ/*pLNK->connection[i].com_chn[j].recv_udp_dat_Q*/) , &pri);
	if (write_q_rt == Q_WRITE_FAIL)
	{
		return GM_RSSP1_FALSE;
	} 
	return GM_RSSP1_TRUE;
}

GM_RSSP1_BOOL GM_RSSP1_2oo3_Conn_Send_Dat(RSSP_Message_t* msg, GM_RSSP1_UINT8 index, GM_RSSP1_UINT8 chn_index)
{
	GM_RSSP1_com_pri_struct    pri;
	if (GM_RSSP1_FALSE == FSFB_Is_Msg_Queue_Empty(&(g_Link_MQ[index].RCV_SND_MQ[chn_index].SND_MQ)/*&(pLNK_con->com_chn[udp_chn_index].send_udp_dat_Q)*/))
	{

		if (GM_RSSP1_FALSE == FSFB_Msg_Queue_Get(&(g_Link_MQ[index].RCV_SND_MQ[chn_index].SND_MQ)/*&(pLNK_con->com_chn[udp_chn_index].send_udp_dat_Q)*/, &pri))
		{
			return GM_RSSP1_FALSE;
		}

		GM_RSSP1_memcpy(msg->Message,  pri.byte, pri.bytes_count);
		msg->MsgSize = pri.bytes_count;
		return GM_RSSP1_TRUE;
	}
	return GM_RSSP1_FALSE;
}
#endif

/*added by huang 20151219*/
void GM_RSSP1_APP_Interface_VSN_Init()
{
    GM_RSSP1_VSN_Init();
}

void GM_RSSP1_APP_Interface_VSN_Update()
{
    GM_RSSP1_VSN_Update();
}
/*the end 20151219*/

