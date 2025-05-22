#ifndef _GM_RSSP1_CFM_INTERFACE_H
#define _GM_RSSP1_CFM_INTERFACE_H

#include "GM_RSSP1_Utils.h"
#include "GM_RSSP1_CFM.h"

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct
	{
        GM_RSSP1_UINT32                     loc_ip;
        GM_RSSP1_UINT32                     loc_port;
	}GM_RSSP1_PACKED GM_RSSP1_Local_Addr_struct;

	typedef struct
	{
	  GM_RSSP1_UINT16                     chn_num; 
	  GM_RSSP1_Local_Addr_struct  locl_addr[GM_RSSP1_CFM_MAX_CHANNEL_NUM];
	}GM_RSSP1_PACKED GM_RSSP1_Local_CHN_struct;

	typedef struct
	{
	  GM_RSSP1_UINT16                     connection_num; 
	  GM_RSSP1_Local_CHN_struct  connection[GM_RSSP1_MAX_SAFETY_CONNECTION_NUM];
	}GM_RSSP1_PACKED GM_RSSP1_Local_Con_struct;
	
	void GM_RSSP1_CFM_Interface_Proc_Send(void);

	void GM_RSSP1_CFM_Interface_Proc_Recv(void);

	GM_RSSP1_BOOL GM_RSSP1_Get_Address(GM_RSSP1_UINT32* r_port, GM_RSSP1_UINT32* r_ip, GM_RSSP1_UINT8 con_index, GM_RSSP1_UINT8 red_index);
	GM_RSSP1_BOOL GM_RSSP1_Get_ConIndex(GM_RSSP1_UINT32 l_port, GM_RSSP1_UINT32 l_ip, GM_RSSP1_UINT8* con_index, GM_RSSP1_UINT8* red_index);
	GM_RSSP1_INT8 GM_RSSP1_Write_SFM_Interface_ergodic(GM_RSSP1_UINT8* p_dat, GM_RSSP1_UINT16* len, GM_RSSP1_UINT32* r_port, GM_RSSP1_UINT32* r_ip, GM_RSSP1_UINT8* con_index, GM_RSSP1_UINT8* reduan_index);
	GM_RSSP1_BOOL RSSP1_CFM_Clear_Msg_Queue(void);
#ifdef CFM_Stanby_Answer
	GM_RSSP1_INT8 GM_RSSP1_Write_StandbyInfo_Interface_ergodic(GM_RSSP1_UINT8* p_dat, GM_RSSP1_UINT16* len, GM_RSSP1_UINT8* con_index, GM_RSSP1_UINT8* reduan_index);
#endif
#ifdef __cplusplus
}
#endif

#endif