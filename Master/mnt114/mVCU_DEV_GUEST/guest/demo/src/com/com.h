/*
 * com.h
 *
 *  Created on: Jul 5, 2017
 *      Author: 61239
 */

#ifndef COM_H_
#define COM_H_
#include "step_constant.h"
#include "step_includes.h"
#include "mstep.h"
#include "dtxt_print_on.h"
#include "dtxt.h"
#include "std.h"
#include "osw.h"
#include "gdf.h"

#define GUEST_C_TASK_NUM                           1u                 /* COM4任务数目 */

#pragma pack(1)
typedef struct
{
	BOOLEAN	isConnect;/*0:fail; 1:success */
	INT16U	localport;	/* */
	INT16U	destPort;	/* */
	INT32U	destAddr;	/* */
	MSTEP_T_SocketID*	pSocket;/*0x24*/	
}COM4_T_Connection;		/* UDP-APP连接类型*/
#pragma pack()


typedef struct
{
    OSW_T_pEVENT MNT_mnt_01TftpBUF_EVENT;                   /* mnt01中转buf */
} COM4_T_SEM;


extern MSTEPINC_T_NetworkLed COM_C_NetworkLed;
extern MSTEPINC_T_MaintenanceTable COM_V_MaintenanceTable;
extern INT8U COM_C_ApplicationSwVersion[15u];
extern MSTEPINC_T_TaskInfo COM_V_TaskInfo;
extern MSTEPINC_E_FunctionalMode g_MNT_CurrentFunctionalMode;
extern COM4_T_Connection COM4_V_Connection;
extern COM4_T_SEM g_Sem;
#endif /* COM_H_ */
