/*
 * com.c
 *
 *  Created on: Jul 5, 2017
 *      Author: 61239
 */
#include "com.h"
extern INT32U cpuUsage;

#include "public.h"

#ifdef TestPlatform
#include "tst.h"
#include "tstswitch.h"
#include "tst_tsk.h"
#endif
/** Stack size for the COM task*/
#define COM_C_STACK_SIZE   700u

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
#ifdef TestPlatform
	Tst_F_Creat();
#endif
    GDF_M_NULL_ASSERT(pCreateInfo);

    pCreateInfo->pTaskStack     = COM_V_TaskStk;
    pCreateInfo->taskStackSize  = COM_C_STACK_SIZE;

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
	INT8U i;
#endif  
    OSW_F_TempoStart(&COM_V_Timer, 0xEA60u);

    softwareDataID = MSTEP_F_GetSoftwareDataID();
    DTXT_REPORT1(DTXT_REPORT_INFO, "Software data identification:", softwareDataID);
#ifdef VCU_TB
	for(i = 0; i < 128; ++i)
	{
		TB_writeIOdBData[i].dataId = i+1;
		TB_writeIOdBData[i].value = 0;
	}
	
	for (i = 0; i < 128; ++i)
	{
		TB_readIOdBData[i].dataId = i+1;
		TB_readIOdBData[i].value = 1;
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
    MSTEP_T_NetStatusInfo LocalNetStatus;
    MSTEP_T_NetStatusInfo OtherNetStatus;

    STD_F_Memset(logStr, 0u, sizeof(logStr));
    STD_F_Memset(&LocalNetStatus, 0u, sizeof(OtherNetStatus));
    STD_F_Memset(&OtherNetStatus, 0u, sizeof(OtherNetStatus));
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
            MSTEP_F_GetLocalMVCUNetStatus(&LocalNetStatus);
            MSTEP_F_GetAnotherMVCUNetStatus(&OtherNetStatus);
            DTXT_REPORT6(DTXT_REPORT_WARNING, "LocalNetStatus net0 A B, NET1 A B, COMMUN A B", LocalNetStatus.Net0_physicalLayerStatus_A,LocalNetStatus.Net0_physicalLayerStatus_B,LocalNetStatus.Net1_physicalLayerStatus_A,LocalNetStatus.Net1_physicalLayerStatus_B,LocalNetStatus.communicateLayerStatus_N[0],LocalNetStatus.communicateLayerStatus_R[0]);
            DTXT_REPORT6(DTXT_REPORT_WARNING, "OtherNetStatus net0 A B, NET1 A B, SAFETY A B",OtherNetStatus.Net0_physicalLayerStatus_A,OtherNetStatus.Net0_physicalLayerStatus_B,OtherNetStatus.Net1_physicalLayerStatus_A,OtherNetStatus.Net1_physicalLayerStatus_B,OtherNetStatus.safetyLayerStatus_A[0],OtherNetStatus.safetyLayerStatus_B[0]);
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

//        for (paramSectorNum = 0u; paramSectorNum < 3u; paramSectorNum++)
//        {
//            bNewParamAvailable = MSTEP_F_ParametersSectorNewIsAvailable(paramSectorNum);
//
//            if (FALSE != bNewParamAvailable)
//            {
//                paramSectorState = MSTEP_F_ParametersSectorCheckAndGet(paramSectorNum, &pSectorBuffer, &sectorReparamCounter);
//
//                if (MSTEPINC_C_PARAM_SECTOR_OK == paramSectorState) //和1比较
//                {
//                    STD_F_Snprintf(logStr, 0x40u, "[34mNew Parameters sector[%d] ReParam(%d):  ", paramSectorNum, sectorReparamCounter);
//                    STD_F_Strncat(logStr, (CHAR*)(pSectorBuffer + 0x16u), 0x14u);//?????????第二个参数有问题
//                    DTXT_REPORT(DTXT_REPORT_WARNING, logStr);
//                    MSTEP_F_ParametersSectorRelease(paramSectorNum);
//                }
//            }
//        }
    }

    previousFunctionnalMode = functionalMode;

#ifdef TestPlatform
	Tst_F_Task(functionalMode);
#endif
    return ALSTOM_C_NO_ERROR;
}












