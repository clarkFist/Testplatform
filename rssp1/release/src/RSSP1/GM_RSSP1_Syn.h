#ifndef _GM_RSSP1_SYN_H
#define _GM_RSSP1_SYN_H

#include "GM_RSSP1_Utils.h"
#include "GM_RSSP1_Lib_Def.h"
#include "GM_RSSP1_SFM.h"
#include "GM_RSSP1_CFM.h"


#ifdef __cplusplus
extern "C"
{
#endif

	/** Լ������ͬ��ͨ����ID.��������fsfb���ӵ�sacepid��ϲ�������ظ�! */
	#define GM_RSSP1_SYN_DEST_ADDR   ((GM_RSSP1_UINT16)0xffffU)

	typedef struct
	{
		GM_RSSP1_UINT8 index;
		GM_RSSP1_Remote_Dev_AS_State_enum remote_dev_AS_state;
		GM_RSSP1_SACEPID_struct sacepid;
		/* FSFB ������TIME��Ϣ */
		GM_RSSP1_UINT32     time_TC;
		GM_RSSP1_UINT32  time_TS[GM_RSSP1_CHECK_CHN_NUM];
		GM_RSSP1_UINT32  time_oldTS[GM_RSSP1_CHECK_CHN_NUM];

		/* FSFB ������ENV��Ϣ */
		GM_RSSP1_CHN_ENV        env[GM_RSSP1_CHECK_CHN_NUM];
		/*PreciTime��Ϣ,added by huang 20151215*/
		GM_RSSP1_INT32 PreciTime;			 /* ���㾫ȷ����Ϣ��Чʱ��*/
		GM_RSSP1_INT32 SSE_SSR_Delay;
		GM_RSSP1_UINT32 SINIT_TremCycle;       /* ����SINITMʱ��Զ�̽ڵ��RSD��TC */
		GM_RSSP1_UINT32 RxSSRTrem;            /* ���յ���SSR��Ϣ��TC��Ҳ��Remote_TC */
		GM_RSSP1_UINT32 RxSSRTloc;
		GM_RSSP1_UINT32 LastSendSSECycle;		/*���һ�η���SSE��TC, added by huang 20151124*/
		GM_RSSP1_UINT32 TcycleLoc;      /* ���ؽڵ�����ڣ���λ������ */
		GM_RSSP1_UINT32 TcycleRem;      /* Զ�̽ڵ�����ڣ���λ������ */
	} GM_RSSP1_PACKED GM_RSSP1_Syn_Info_struct;

	typedef struct
	{
		GM_RSSP1_Syn_Info_struct base_info;
		GM_RSSP1_connection_state_enum state;
		GM_RSSP1_BOOL b_rcvd_new_dat_in_cycle;
		GM_RSSP1_BOOL b_torlerant_dat_valid;
		GM_RSSP1_UINT8 torlerate_cycle_count1;
		GM_RSSP1_UINT8 torlerate_cycle_count2;
		GM_RSSP1_UINT8 state_check_counter1;
		GM_RSSP1_UINT8 state_check_counter2;
	#ifndef GM_RSSP1_SYN_NO_DATA
		GM_RSSP1_SFM_L2U_pri_struct last_valid_dat;
	#endif
	}GM_RSSP1_PACKED GM_RSSP1_Syn_Extern_Info_struct;

	GM_RSSP1_BOOL GM_RSSP1_Syn_Send_Fsfb_State_To_Peer(Local_Sys_AB_enum sys_a_b , GM_RSSP1_BOOL b_is_active);
	void GM_RSSP1_Syn_Proc_Rcvd_Syn_Dat(GM_RSSP1_UINT8 *pDat , GM_RSSP1_INT32 dat_len);
	void GM_RSSP1_Syn_Send_App_To_Peer(GM_RSSP1_UINT8* pdate, GM_RSSP1_UINT16 len);
	GM_RSSP1_BOOL GM_RSSP1_Syn_Data_Set(GM_RSSP1_UINT8* pdata, GM_RSSP1_UINT32 dataSize);
	GM_RSSP1_BOOL GM_RSSP1_Syn_Data_Get(GM_RSSP1_UINT32* pdata, GM_RSSP1_UINT32* len);

#ifdef __cplusplus
}
#endif

#endif
