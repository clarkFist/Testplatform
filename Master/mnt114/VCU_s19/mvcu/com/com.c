/*
 * com.c
 *
 *  Created on: Jul 5, 2017
 *      Author: 61239
 */
#include "com.h"
#include "com4_mnt.h"
extern INT32U cpuUsage;

/** Stack size for the COM task*/
#define COM_C_STACK_SIZE   700u
#define COM_MNT_C_STACK_SIZE   40900u   //任务栈长度40960*4字节

static ALSTOM_T_Status COM_F_Create(MSTEPINC_T_TaskCreateInfo *pCreateInfo);
static ALSTOM_T_Status COM_F_Config(void);
static ALSTOM_T_Status COM_F_Start(void);
static ALSTOM_T_Status COM_F_MainLoop(MSTEPINC_E_FunctionalMode functionalMode);

OSW_T_Tempo COM_V_Timer;
INT32U COM_V_OSCPUUsageMax;
INT32U COM_V_OverflowCount;
static MSTEP_T_VitalStamp VitalStamp[GUEST_C_TASK_NUM];
COM4_T_SEM g_Sem = {NULL};    //保护buf临界区

MSTEP_T_SocketID*  COM4_MNT_UnSafety_V_UDP_pSocket = NULL;    //song 主系非安全网
MSTEP_T_SocketID*  COM4_MNT_RNonSafety_V_UDP_pSocket = NULL;    //song 备系非安全网

OSW_T_pEVENT* GM_MNT_NonSafety_V_UDP_pFifo = NULL;    //song mnt_001
OSW_T_pEVENT* GM_MNT_RNonSafety_V_UDP_pFifo = NULL;    //song mnt_001


INT32U g_EIOCOM4_NNonSafetyPort =0u;   //
INT32U g_EIOCOM4_RNonSafetyPort =0u;	 //
INT32U g_EIOCOM4_NonSafetyPort =0u;   //  A,B网封装

INT32U g_SNMP_NNonSafetyIp =0u;   //非安全网维护台
INT32U g_SNMP_RNonSafetyIp =0u;  //非安全网维护台
INT32U g_SNMP_NonSafetyIp =0u;  //非安全网维护台A,B网封装

MSTEPINC_E_FunctionalMode	 g_MNT_CurrentFunctionalMode =ALSTOM_C_FUNCTIONAL_NORMAL;

MSTEPINC_T_NetworkLed COM_C_NetworkLed = {MSTEPINC_C_GPIO_ID13, MSTEPINC_C_GPIO_ID14, MSTEPINC_C_GPIO_ID11, MSTEPINC_C_GPIO_ID12};

COM4_T_Connection	 COM4_V_Connection;		/* UDP-APP连接*/   

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

    

static void GUEST_F_SemCreate(void)
{
	INT32U i = 0u;
	for (i = 0;i < GUEST_C_TASK_NUM;++i)
	{
		VitalStamp[i].pCrcTable[0] = 0xAAAAAAAAu;
		VitalStamp[i].pCrcTable[1] = 0x55555555u;    //内核深处用到；另外没必要在信号量创建函数中赋值
	}
	
	/* 初始化MNT_01中间buf信号量 */
	g_Sem.MNT_mnt_01TftpBUF_EVENT = OSW_F_SemCreate(1);	
	GDF_M_NULL_ASSERT(g_Sem.MNT_mnt_01TftpBUF_EVENT);

}

static ALSTOM_T_Status COM_F_Create(MSTEPINC_T_TaskCreateInfo *pCreateInfo)
{
    static INT32U COM_V_TaskStk[COM_MNT_C_STACK_SIZE];
    static void* pUDPMNTMgrQMsgTbl[50]; //song MNTmoni 

    GDF_M_NULL_ASSERT(pCreateInfo);

    pCreateInfo->pTaskStack     = COM_V_TaskStk;
    pCreateInfo->taskStackSize  = COM_MNT_C_STACK_SIZE;

	//创建UDPfifo   
    GM_MNT_NonSafety_V_UDP_pFifo = OSW_F_QCreate(pUDPMNTMgrQMsgTbl, 50); 	
    GUEST_F_SemCreate();	

    return ALSTOM_C_NO_ERROR;
}

static ALSTOM_T_Status COM_F_Config(void)
{
    ALSTOM_T_Status status = ALSTOM_C_NO_ERROR;
    
#ifdef ALSTOM_CONFIG_CPU_A
        status=MSTEP_F_TftpServiceSubscribe("mnt", (MSTEP_T_TftpOpenConnectionCB)COM4_F_MNT_ServiceOpen, (MSTEP_T_TftpCloseConnectionCB)COM4_F_MNT_ServiceClose);  
        /*(void) DTXT_F_Printf("\n COM_MNT_F_Config_mnt_01Subscribe %d \n",status );*/
#endif 	
    
        return status;

}

static ALSTOM_T_Status COM_F_Start(void)
{
    INT32U softwareDataID = 0u;
	INT8U iRackNum=0u;
	INT8U iSlotNum=0u;

    OSW_F_TempoStart(&COM_V_Timer, 0xEA60u);

    softwareDataID = MSTEP_F_GetSoftwareDataID();
    DTXT_REPORT1(DTXT_REPORT_INFO, "Software data identification:", softwareDataID);

    //创建UDPsocket
    MSTEP_F_UnSafetyPortGet(&g_EIOCOM4_NNonSafetyPort,&g_EIOCOM4_RNonSafetyPort);
    MSTEP_F_SnmpIpGet(&g_SNMP_NNonSafetyIp,&g_SNMP_RNonSafetyIp);   
    
    //创建UDPsocket,监测三报文，无论收发都只用一个socket，一个端口号
    GDF_M_NULL_ASSERT(GM_MNT_NonSafety_V_UDP_pFifo);     //song app 
    MSTEP_F_ConfigBoardPositionGet(&iRackNum, &iSlotNum);
    
	if( (iRackNum==1u)&&(iSlotNum==1u) )	//A系MVCU   1-1
	{
		g_EIOCOM4_NonSafetyPort = g_EIOCOM4_NNonSafetyPort;
		g_SNMP_NonSafetyIp = g_SNMP_NNonSafetyIp;
	}
	if( (iRackNum==1u)&&(iSlotNum==15u) )	//B系 1-15
	{
		g_EIOCOM4_NonSafetyPort = g_EIOCOM4_RNonSafetyPort;
		g_SNMP_NonSafetyIp = g_SNMP_RNonSafetyIp;
	}

    COM4_MNT_UnSafety_V_UDP_pSocket = MSTEP_F_UDPOpen((INT16U)g_EIOCOM4_NonSafetyPort, GM_MNT_NonSafety_V_UDP_pFifo,1);  //song UDP的SOCKET应该自己释放
    
    COM4_F_MNT_Init();

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

    STD_F_Memset(logStr, 0u, sizeof(logStr));
    MSTEP_F_TaskFlagSet();

#ifdef ALSTOM_CONFIG_CPU_A	      
   /*从板卡只给主机发监测消息，所以让主机发网络消息*/
   
    COM4_F_MNT_UdpReqRcv(COM4_MNT_UnSafety_V_UDP_pSocket);
   if (functionalMode == ALSTOM_C_FUNCTIONAL_NORMAL)
   {
        COM4_F_MNT_UploadMoniByUdp(COM4_MNT_UnSafety_V_UDP_pSocket, COM4_V_Connection.destAddr , COM4_V_Connection.destPort );
   }
#endif
	g_MNT_CurrentFunctionalMode = functionalMode;		

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
            STD_F_Snprintf(logStr, 0x46, "[35mAt: %d Avg: %d %%", OSTime, OSCPUUsageAvg);
            DTXT_REPORT(DTXT_REPORT_WARNING, logStr);
            loopCount = 0u;
            OSCPUUsageSum = 0u;
        }
        else
        {
            if (cpuUsage >= 0x0u)
            {
                //d0 = OSCPUUsageSum;
                //d5 = 0x0000FFFF - (INT32U)OSCPUUsage;
                if (OSCPUUsageSum <= (0x0000FFFF - (INT32U)cpuUsage))
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
            OSCPUUsageMin = (INT8U)cpuUsage;
        }
#endif
        if (OSW_C_TempoElapsed == tempoState)
        {
#if 1//待32所加入cpu最大使用率变量时，重新打开
            STD_F_Snprintf(logStr, 0x46, "[32mUsage: Min: %d %%, Cur: %d %%, Max: %d %%", OSCPUUsageMin, (INT32U)cpuUsage, COM_V_OSCPUUsageMax);
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

        /*for (paramSectorNum = 0u; paramSectorNum < 3u; paramSectorNum++)
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

    return ALSTOM_C_NO_ERROR;
}












