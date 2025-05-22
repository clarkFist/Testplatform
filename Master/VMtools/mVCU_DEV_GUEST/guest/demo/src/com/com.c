/*
 * com.c
 *
 *  Created on: Jul 5, 2017
 *      Author: 61239
 */
#include "com.h"
extern INT32U cpuUsage;
#define  TestPlatform
#define VCU_TB

#ifdef TestPlatform
#include "tst.h"
#include "tstswitch.h"
#include "tst_tsk.h"
#endif
#include "dtxt_print_on.h"
#include "dtxt.h"
#include "datatypes.h"
//#include "guest_test.h"

ALSTOM_T_Status  TB_Status;

/** Stack size for the COM task*/
#define COM_C_STACK_SIZE   700u
#ifdef VCU_TB
#include "step_constant.h"

#include "mstep.h"
//#include "SDB.h"
//#include "vote.h"
//#include "vote_tsk.h"
//#include "Schedule_utl.h"
BOOLEAN cycle_Test = FALSE;
BOOLEAN physicalOK = FALSE;
CHAR TB_logString[0x64];
#define TB_SEND_NUM   2   //32 3
#define TB_RECV_NUM   2   //128 3
MSTEP_T_IodbData TB_readIOdBData[TB_RECV_NUM];
MSTEP_T_IodbData TB_writeIOdBData[TB_SEND_NUM];
//extern INT32U TB_count;
INT32U  i;
#endif

static ALSTOM_T_Status COM_F_Create(MSTEPINC_T_TaskCreateInfo *pCreateInfo);
static ALSTOM_T_Status COM_F_Config(void);
static ALSTOM_T_Status COM_F_Start(void);
static ALSTOM_T_Status COM_F_MainLoop(MSTEPINC_E_FunctionalMode functionalMode);

OSW_T_Tempo COM_V_Timer;
INT32U COM_V_OSCPUUsageMax;
INT32U COM_V_OverflowCount;

MSTEPINC_T_NetworkLed COM_C_NetworkLed = {MSTEPINC_C_GPIO_ID13, MSTEPINC_C_GPIO_ID14, MSTEPINC_C_GPIO_ID11, MSTEPINC_C_GPIO_ID12};

INT8U COM_C_ApplicationSwVersion[15u];
MSTEPINC_T_MaintenanceDataName COM_C_MaintenanceCPUUsage = {"Maximum CPU Usage(%)"};
MSTEPINC_T_MaintenanceDataName COM_C_MaintenanceOvfCount = {"Eth overflow count "};

MSTEPINC_T_MaintenanceEntry COM_V_MaintenanceTable[2] =
{
    {&COM_C_MaintenanceCPUUsage, &COM_V_OSCPUUsageMax},
    {&COM_C_MaintenanceOvfCount, &COM_V_OverflowCount}
};

MSTEPINC_T_TaskInfo COM_V_TaskInfo =
{
    &COM_F_Create,
    &COM_F_Config,
    &COM_F_Start,
    &COM_F_MainLoop,
    MSTEPINC_C_TASK_ID_1,
    MSTEPINC_C_TASK_PRIOLEVEL_HIGH,
    MSTEPINC_C_TASK_PRIO_1,
    "COM",
    MSTEPINC_C_ORDER_1,
    0x6Eu,
    0xAu,
    MSTEPINC_C_RUN_TASK_ON_BOTH_CPU,
    FALSE
};


static ALSTOM_T_Status COM_F_Create(MSTEPINC_T_TaskCreateInfo *pCreateInfo)
{
    static INT32U COM_V_TaskStk[COM_C_STACK_SIZE];

    GDF_M_NULL_ASSERT(pCreateInfo);

    pCreateInfo->pTaskStack     = COM_V_TaskStk;
    pCreateInfo->taskStackSize  = COM_C_STACK_SIZE;
	
	#ifdef TestPlatform
	Tst_F_Creat();
#endif
    return ALSTOM_C_NO_ERROR;
}

static ALSTOM_T_Status COM_F_Config(void)
{
	
    return ALSTOM_C_NO_ERROR;
}

static ALSTOM_T_Status COM_F_Start(void)
{
    INT32U softwareDataID = 0u;
#ifdef VCU_TB
    INT8U i = 0;
#endif

    OSW_F_TempoStart(&COM_V_Timer, 0xEA60u);

    softwareDataID = MSTEP_F_GetSoftwareDataID();
    DTXT_REPORT1(DTXT_REPORT_INFO, "Software data identification:", softwareDataID);

#if 1
	for(i = 0; i < TB_SEND_NUM; ++i)
	{
		TB_writeIOdBData[i].dataId = i+1;
		TB_writeIOdBData[i].value = 0;
	}
	for (i = 0; i < TB_RECV_NUM; ++i)
	{
		TB_readIOdBData[i].dataId = i+1;
		TB_readIOdBData[i].value = 0;
	}
#endif
	#ifdef TestPlatform
	Tst_F_Start();
#endif
    return ALSTOM_C_NO_ERROR;
}

static ALSTOM_T_Status COM_F_MainLoop(MSTEPINC_E_FunctionalMode functionalMode)
{
    static INT32U Timer_TO;
    static INT8U loopCount = 0u;
    static INT8U OSCPUUsageAvg;
    static INT8U OSCPUUsageMin = 0xFFu;
    static INT16U OSCPUUsageSum;
    static INT32U MainLoop_seqNb[2];
    static INT32U MainLoop_maxDly[2];
    static MSTEPINC_E_FunctionalMode previousFunctionnalMode = ALSTOM_C_FUNCTIONAL_BOOT;
    INT8U paramSectorNum = 0u;
    BOOLEAN bNewParamAvailable;
    INT32U OSTime;
    INT32U sectorReparamCounter = 0u; //var8
    INT8U* pSectorBuffer = NULL;  //var4，参数定义有问题???????????????????
    CHAR logStr[0x60];
    MSTEPINC_E_ParamSectorState paramSectorState;
    OSW_E_TempoState tempoState;
#ifdef VCU_TB
    INT8U i = 0;
    INT8U TB_index = 0;
    INT8U TB_shift = 0;
    INT8U sessionid = 6;
    INT32U TB_Receive_Data[4];
    MSTEP_T_NetStatusInfo pNetStatus = {0u};
        
    TB_Receive_Data[0] = 0;
    TB_Receive_Data[1] = 0;
    TB_Receive_Data[2] = 0;
    TB_Receive_Data[3] = 0;

#endif
    STD_F_Memset(logStr, 0u, sizeof(logStr));
    MSTEP_F_TaskFlagSet();
   

    if (functionalMode > ALSTOM_C_FUNCTIONAL_STANDBY)
    {
        if ((functionalMode < ALSTOM_C_FUNCTIONAL_NORMAL_RESTRICTIVE) || (functionalMode > ALSTOM_C_FUNCTIONAL_STANDBY_RESTRICTIVE))
        {
            GDF_M_FAULT_EXT(EVT_C_SAFETY_CORRUPTED, functionalMode, "");
        }
        else
        {
            DTXT_REPORT(DTXT_REPORT_INFO, "Board in restrictive state");

            if (previousFunctionnalMode <= ALSTOM_C_FUNCTIONAL_STANDBY)
            {
                MSTEP_F_NotifyEndOfRestrictiveTransition();
            }
        }
    }
    else
    {
#ifdef VCU_TB
//    	    if (TRUE == IsTagOpen(VCU_MVCU_SwRTC_5216002))
//    		{
//    	    	 for (i = 0; i < sessionid; ++i)  
//    	    	 {  
//    	    		 STD_F_Memset(TB_readIOdBData,0,sizeof(TB_readIOdBData));
//    	    		 MSTEP_F_IodbRequestBySessionId(i,TB_readIOdBData,&TB_RECV_NUM); 
//    	    		 DTXT_F_Report(PRINT_REPORT_ERROR, DTXT_C_LOC , "sessionid,data1,data2,data3,data4,data5:", 6u, i,TB_readIOdBData[0],TB_readIOdBData[1],TB_readIOdBData[2],TB_readIOdBData[3],TB_readIOdBData[976]);
//    	    	 } 
//    		}
		if (TRUE == IsTagOpen(VCU_MVCU_SwRTC_5216002)||TRUE == IsTagOpen(VCU_MVCU_SwRTC_5217003)||TRUE == IsTagOpen(VCU_MVCU_SwRTC_5219002))
		{
			MSTEP_F_IodbRequest(TB_RECV_NUM,TB_readIOdBData); 
			DTXT_F_Report(DTXT_REPORT_INFO, DTXT_C_LOC , "data1,data2,data3:", 3u, TB_readIOdBData[0].value,TB_readIOdBData[1].value,TB_readIOdBData[2].value,0u,0u,0u);
		}
	    /*if(TRUE == IsTagOpen(VCU_SyRTC_0002001))
	   	{
	    	MSTEP_F_IodbRequest(TB_RECV_NUM,TB_readIOdBData); 
			for(i = 0; i < TB_RECV_NUM; ++i)
			{			
				TB_Receive_Data[TB_index] = TB_Receive_Data[TB_index] | (TB_readIOdBData[i].value <<TB_shift);
				TB_shift+=1;
				if(0 == (i+1)%TB_RECV_NUM)
				{
					TB_shift = 0;
					TB_index += 1;
				}
			}
	    	DTXT_F_Report(DTXT_REPORT_INFO, DTXT_C_LOC, "request SDB data:", 4, TB_Receive_Data[0], TB_Receive_Data[1], TB_Receive_Data[2], TB_Receive_Data[3], 0, 0);    
	   	}*/
//		if (TRUE == IsTagOpen(VCU_MVCU_SwRTC_5214001))
//		{
//			if(TB_count==0)
//			{
//				TB_writeIOdBData[1].value = 0;
//				TB_writeIOdBData[2].value = 0;
//				TB_writeIOdBData[3].value = 0;
//				//DTXT_F_Report(3u, DTXT_C_LOC,  "!!!!!!!!!!!!!!!!222222222222222",  0u, 0u , 0u, 0u, 0u, 0u, 0u);
//				SDB_F_DataIdSubmit(3,TB_writeIOdBData);
//				TB_count += 1;		
//			}
//		}
#endif   	
        tempoState = OSW_F_TempoRemainingTick(&COM_V_Timer, &Timer_TO);
#if 1//待32所加入cpu最大使用率变量时，重新打开
/*20180529, add =, %, cpuUsage < 0 -> cpuUsage >= 0*/
        if (loopCount >= 0x14u)
        {
            OSCPUUsageAvg = (INT8U)((OSCPUUsageSum / loopCount) & 0xFFu); 
            OSTime = OSW_F_TickGet();
            STD_F_Snprintf(logStr, 0x46u, "[35mAt: %d Avg: %d %%", OSTime, OSCPUUsageAvg);
            DTXT_REPORT(DTXT_REPORT_WARNING, logStr);
            loopCount = 0u;
            OSCPUUsageSum = 0u;
        }
        else
        {
            if (cpuUsage >= 0u)
            {
                //d0 = OSCPUUsageSum;
                //d5 = 0x0000FFFF - (INT32U)OSCPUUsage;
                if (OSCPUUsageSum <= (0x0000FFFFu - (INT32U)cpuUsage))
                {
                    //d0 = (INT16U)OSCPUUsage + OSCPUUsageSum;
                    OSCPUUsageSum = (INT16U)cpuUsage + OSCPUUsageSum;
                }
                else
                {
                    OSCPUUsageSum = 0xFFFFu;
                }
            }
            loopCount++;
        }

        COM_V_OSCPUUsageMax = MSTEP_F_StatCPUUsageMaxGet();

        COM_V_OverflowCount = MSTEP_F_StatFecOverflowCountGet();

        if (cpuUsage < OSCPUUsageMin)
        {
            OSCPUUsageMin = cpuUsage;
        }
#endif
        if (OSW_C_TempoElapsed == tempoState)
        {
#if 1//待32所加入cpu最大使用率变量时，重新打开
            STD_F_Snprintf(logStr, 0x46u, "[32mUsage: Min: %d %%, Cur: %d %%, Max: %d %%", OSCPUUsageMin, (INT32U)cpuUsage, COM_V_OSCPUUsageMax);
            DTXT_REPORT(DTXT_REPORT_WARNING, logStr);
#endif
/*20180528, code review, CR:755*/
#ifdef ALSTOM_CONFIG_CPU_B
            MSTEP_F_StatRedIdMaxSequenceNbDeltaGet((MSTEP_T_MaxRedIdSequenceNb*)MainLoop_seqNb);//注了方便调试，by SXR

            STD_F_Snprintf(logStr, 0x46u, "[35mMax Delta seqNb. Ntwk: Norm=%d Redund=%d", MainLoop_seqNb[0], MainLoop_seqNb[1]);
            DTXT_REPORT(DTXT_REPORT_WARNING, logStr);

            MSTEP_F_StatRedIdMaxPropagationDlyGet((MSTEP_T_MaxRedIdPropagationDly*)MainLoop_maxDly);//注了方便调试，by SXR

            STD_F_Snprintf(logStr, 0x46u, "[35mMax propagation Delay in ms. Ntwk: Norm=%d Redund=%d", MainLoop_maxDly[0], MainLoop_maxDly[1]);
            DTXT_REPORT(DTXT_REPORT_WARNING , logStr);
#endif
            OSW_F_TempoStart(&COM_V_Timer, 0xEA60u);
        }
        else
        {
            if (OSW_C_TempoRunning != tempoState)
            {
                GDF_M_FAULT_EXT(EVT_C_SAFETY_CORRUPTED, tempoState, "");
            }
        }

       /* for (paramSectorNum = 0u; paramSectorNum < 3u; paramSectorNum++)
        {
            bNewParamAvailable = MSTEP_F_ParametersSectorNewIsAvailable(paramSectorNum);

            if (FALSE != bNewParamAvailable)
            {
                paramSectorState = MSTEP_F_ParametersSectorCheckAndGet(paramSectorNum, &pSectorBuffer, &sectorReparamCounter);

                if (MSTEPINC_C_PARAM_SECTOR_OK == paramSectorState) //和1比较
                {
                    STD_F_Snprintf(logStr, 0x40u, "[34mNew Parameters sector[%d] ReParam(%d):  ", paramSectorNum, sectorReparamCounter);
                    STD_F_Strncat(logStr, (CHAR*)(pSectorBuffer + 0x16u), 0x14u);//?????????第二个参数有问题
                    DTXT_REPORT(DTXT_REPORT_WARNING, logStr);
                    MSTEP_F_ParametersSectorRelease(paramSectorNum);
                }
            }
        }*/
    }

    previousFunctionnalMode = functionalMode;

#ifdef VCU_TB    
    if(TRUE == IsTagOpen(VCU_SyRTC_4005001)&&(FALSE == cycle_Test))
    {
     	DTXT_F_Report(4,DTXT_C_LOC,"VCU_SyRTC_4005001 open ",1, IsTagOpen(VCU_SyRTC_4005001),0,0,0,0,0); 
#ifdef  VCU_CONF_CPU_A
		STD_F_Memset(TB_logString,0,sizeof(TB_logString));
		STD_F_Snprintf(TB_logString,0x64,"SET_FailureCode 0x50 MVCU");
		MSTEP_F_FailureCodeSet(0x50u,(MSTEPINC_T_FailureContext*)TB_logString);
		 DTXT_F_Printf("SET FailureCode--->0x50\r\n");
#endif
		cycle_Test = TRUE;
    }
    
    if(TRUE == IsTagOpen(VCU_SyRTC_5001001))
    {
    	DTXT_F_Report(4,DTXT_C_LOC,"MVCU call swith A/S function when check the physical connection is break",0,0,0,0,0,0,0); 
    	physicalOK = MSTEP_F_GetLocalMVCUNetStatus(&pNetStatus);
    	if(physicalOK == ALSTOM_C_NO_ERROR && functionalMode == ALSTOM_C_FUNCTIONAL_NORMAL)
    	{
    		//DTXT_F_Report(4,DTXT_C_LOC,"physicalOK == TRUE",0,0,0,0,0,0,0); 
    		if((pNetStatus.Net1_physicalLayerStatus_A == 2u)&&(pNetStatus.Net1_physicalLayerStatus_B == 2u))
    		{
    			DTXT_F_Report(4,DTXT_C_LOC,"pNetStatus.physicalLayerStatus_N == 2u)&&(pNetStatus.physicalLayerStatus_R == 2u",0,0,0,0,0,0,0); 
    			MSTEP_F_SwitchStandbyModeRequest(0x1FF);    			
    		}
    	}
    }
    
//    if(TRUE == IsTagOpen(VCU_SyRTC_5002001))
//    {
//    	DTXT_F_Report(4,DTXT_C_LOC,"MVCU enter into LIMITATIOM",0,0,0,0,0,0,0); 
//    	SCHED_F_SwitchRestrictiveMode(0xBu, 0u);
//    }
    
    if(TRUE == IsTagOpen(VCU_SyRTC_5002002)||TRUE == IsTagOpen(VCU_SyRTC_5007002)||TRUE == IsTagOpen(VCU_SyRTC_5003001))
    {
    	DTXT_F_Report(4,DTXT_C_LOC,"MVCU call swith A/S function",0,0,0,0,0,0,0); 
    	MSTEP_F_SwitchStandbyModeRequest(0x1FF);  
    }
    
    if(TRUE == IsTagOpen(VCU_SyRTC_2014001))
		
    {
		i = i/0;
    	DTXT_F_Printf("\nMVCU enter into SAFE_STATE mode\n\r***************\n");	
    }
    
#endif
#ifdef TestPlatform
	Tst_F_Task(functionalMode);
#endif
    return ALSTOM_C_NO_ERROR;
}












