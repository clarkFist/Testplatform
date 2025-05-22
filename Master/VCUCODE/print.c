#include "print.h"
#include "dtxt.h"
#include "Schedule_launcher.h"
#include "Schedule_utl.h"
#include "Schedule.h"
#include "wdg.h"
#include "MCF54418.h"
//#include "lib_str.h"
#include "vcu_include.h"
#include "VOTE_tsk.h"

#ifdef TestPlatform
#include "tst.h"
#endif


typedef struct
{
    INT32U strSize;   /*0~3  表示紧接其后的字符串数组的大小*/
    CHAR str[0x248];   /* 4~0x24B  表示字符串数组*/
}PRINT_T_logMsg;  /*大小为0x24C, By ZLP*/


BOOLEAN PRINT_V_Started = FALSE;
BOOLEAN PRINT_V_bPrintfAuth = TRUE;
BOOLEAN PRINT_V_bPrintfTainted = FALSE;
INT32U	PRINT_V_PrintLevel = 5u;
BOOLEAN PRINT_V_bUartInitialized = FALSE;
//BOOLEAN PRINT_V_ActiveIntegrationTrace = FALSE;//规则检查,20210305
//INT32U PRINT_V_FiFoFullMsgLost = 0u;
INT32U PRINT_V_Statistic = 0u;
SCHED_T_TaskInfo PRINT_V_TaskInfo;
OSW_T_pMEM PRINT_V_LogMsgMemBlock = NULL;
OSW_T_pFLAG_GRP* PRINT_V_pFlagGrpNotif = NULL;

INT32S PRINT_F_PrintfWithTaintInfo(const CHAR* fmt, STD_T_VAList ap);
INT32S PRINT_F_PrintfInternal(const CHAR* fmt, ...);
void PRINT_F_ReportActiveIntegration(INT8U activeIntegrationTrace);
static void PRINT_F_PrintLineBreak(PRINT_T_logMsg* pMemBlk,INT32U strLen,INT32U strLen1,INT32U strLen2);

ALSTOM_T_Status PRINT_F_HardwareInit(void)
{
    VINT32U cnt = 0u;
    ALSTOM_T_Status status = ALSTOM_C_ERROR;
    INT32U channel = 0u;
    register INT16U ubgs = 0u;
    INT32U systemClockKHz = 125000u;
    INT32U baudRate = 115200u;
    INT8U edmaChan = 0u;  // uart0 CPUA tx emda7, uart1 CPUB tx edma6;

#ifdef VCU_CONF_CPU_A
    channel = 1u;
    edmaChan = 5u; // edma chan5,CPUA,uart0
#else
    channel = 0u;
    edmaChan = 3u; // edma chan3,CPUB,uart1
#endif

    /* Reset Transmitter*/
    MCF_UART_UCR(channel) = MCF_UART_UCR_RESET_TX;

    /*Reset Receiver*/
    MCF_UART_UCR(channel) = MCF_UART_UCR_RESET_RX;

    /*Reset Mode Register*/
    MCF_UART_UCR(channel) = MCF_UART_UCR_RESET_MR;

    /*Enable receiver and transmitter*/
    MCF_UART_UCR(channel) = (0u
                             | MCF_UART_UCR_RX_DISABLED
                             | MCF_UART_UCR_TX_DISABLED);

    /*No parity, 8-bits per character*/
    MCF_UART_UMR(channel) = (0u
                             | MCF_UART_UMR_PM_NONE
                             | MCF_UART_UMR_BC_8);

    /*No echo or loopback, 1 stop bit*/
    MCF_UART_UMR(channel) = (0u
                             | MCF_UART_UMR_CM_NORMAL
                             | MCF_UART_UMR_SB_STOP_BITS_1);

    /*Set Rx and Tx baud by SYSTEM CLOCK*/
    MCF_UART_UCSR(channel) = (0u
                              | MCF_UART_UCSR_RCS_SYS_CLK
                              | MCF_UART_UCSR_TCS_SYS_CLK);

    /* Calculate baud settings*/
    ubgs = (INT16U)((systemClockKHz * 1000u) / (baudRate * 32u));
    MCF_UART_UBG1(channel) = (INT8U)((ubgs & 0xFF00u) >> 8u);
    MCF_UART_UBG2(channel) = (INT8U)(ubgs & 0x00FFu);
    MCF_eDMA_EDMA_CINT = edmaChan; // clear int
    MCF_eDMA_TCD_ATTR(edmaChan) = (INT16U)(0u
                                           | (INT16U)MCF_eDMA_TCD_ATTR_DSIZE(MCF_eDMA_TCD_ATTR_DSIZE_8BIT)
                                           | (INT16U)MCF_eDMA_TCD_ATTR_SSIZE(MCF_eDMA_TCD_ATTR_SSIZE_8BIT)
                                           | (INT16U)MCF_eDMA_TCD_ATTR_DMOD(0u)
                                           | (INT16U)MCF_eDMA_TCD_ATTR_SMOD(0u)
                                          );
    MCF_eDMA_TCD_SOFF(edmaChan) = 0x01u;

    // MCF_eDMA_TCD_NBYTES(edmaChan) = (INT32U)byteCount;
    // MCF_eDMA_TCD_SLAST(edmaChan) = 0;

    MCF_eDMA_TCD_DADDR(edmaChan) = (INT32U)pMCF_UART_UTB(channel);

    MCF_eDMA_TCD_CITER(edmaChan) = (INT16U)(0u | (INT16U)MCF_eDMA_TCD_CITER_CITER(1u));
    MCF_eDMA_TCD_DOFF(edmaChan) = 0x0u;
    MCF_eDMA_TCD_DLAST_SGA(edmaChan) = 0x0u;
    MCF_eDMA_TCD_BITER(edmaChan) = (INT16U)(0u | (INT16U)MCF_eDMA_TCD_BITER_BITER(1u));


    MCF_eDMA_TCD_NBYTES(edmaChan) = 0x1u;
    MCF_eDMA_TCD_SLAST(edmaChan) = 0u;
    MCF_eDMA_TCD_CITER(edmaChan) = 0u;
    MCF_eDMA_TCD_DOFF(edmaChan) = 0x0u;
    MCF_eDMA_TCD_DLAST_SGA(edmaChan) = 0x0u;
    MCF_eDMA_TCD_BITER(edmaChan) = 0u;

    MCF_eDMA_TCD_CSR(edmaChan) = (INT16U)(0u
                                          //  |MCF_eDMA_TCD_CSR_MAJOR_LINKCH(0)
                                          //  |MCF_eDMA_TCD_CSR_MAJOR_E_LINK
                                          //   |MCF_eDMA_TCD_CSR_INT_MAJOR
                                          | MCF_eDMA_TCD_CSR_D_REQ
                                          | (INT16U)MCF_eDMA_TCD_CSR_BWC(0u));

    for (cnt = 0u; cnt < 1000u; ++cnt)
    {
        if ((0u != (MCF_eDMA_TCD_CSR(edmaChan)&MCF_eDMA_TCD_CSR_ACTIVE))
//    || (0 == (MCF_UART_USR(channel)&MCF_UART_USR_TXRDY))
//    || (0 == (MCF_UART_USR(channel)&MCF_UART_USR_TXEMP))
           )
        {
            OSTimeDly(1u);
        }
        else
        {
            break;
        }
    }

    MCF_UART_UCR(channel) = (0u

                             | MCF_UART_UCR_TX_ENABLED
                            );

    if (1000u == cnt)
    {
        status = ALSTOM_C_ERROR;
    }
    else
    {
        status = ALSTOM_C_NO_ERROR;
    }

    PRINT_V_bUartInitialized = TRUE;
    
    PRINT_V_PrintLevel = GUEST_F_GetPrintLevel();
    
    return status;
}

INT32S PRINT_F_PrintfWithTaintInfo(const CHAR* fmt, STD_T_VAList ap)
{
    OSW_M_CRITICAL_VARIABLE;
    VINT32U i = 0u;   //d2
	INT32S tempNum = -1;
	INT16U NbofChWr = 0u; //Number of Char Written, CRCC: INT32S->INT16U

    static SECTION_UDATA_NO_MEMTEST CHAR DTXTString[0x248];
    INT8U edmaChan = 0u;  // uart0 CPUA tx emda7, uart1 CPUB tx edma6;
    INT32U channel = 0u;

#ifdef VCU_CONF_CPU_A
    channel =  1u;
    edmaChan = 5u; // edma chan5,CPUA,uart0
#else
    channel =  0u;
    edmaChan = 3u; // edma chan3,CPUB,uart1
#endif

    GDF_M_NULL_ASSERT(fmt);

    if (TRUE != PRINT_V_bUartInitialized)
    {
        NbofChWr = 0u;
    }
    else
    {
        for (i = 0u; i < 1000u; ++i)
        {
            if ((0 != (MCF_eDMA_TCD_CSR(edmaChan)&MCF_eDMA_TCD_CSR_ACTIVE))
                || (0 == (MCF_UART_USR(channel)&MCF_UART_USR_TXRDY))
                || (0 == (MCF_UART_USR(channel)&MCF_UART_USR_TXEMP))
               )
            {
                STD_F_GenericPause(1u);
            }
            else
            {
                break;
            }
        }

        if (i >= 1000u)
        {
            GDF_M_FAULT_EXT(EVT_C_ANORMAL_TIMEOUT, i, "");
        }
        else
        {
			tempNum = STD_F_VSnprintf(DTXTString, 0x248u, fmt, ap); 
			if(tempNum >= 0)
			{
				NbofChWr = (INT16U)tempNum;	

	            OSW_M_ENTER_CRITICAL();

	            MCF_UART_UCR(channel) = MCF_UART_UCR_TX_DISABLED;
	            MCF_eDMA_EDMA_CINT = edmaChan; // clear int

	            MCF_eDMA_TCD_SADDR(edmaChan) = (INT32U)DTXTString;

	            MCF_eDMA_TCD_SLAST(edmaChan) = -(tempNum);
	            MCF_eDMA_TCD_CITER(edmaChan) = (NbofChWr) & 0x7FFFu;
	            MCF_eDMA_TCD_BITER(edmaChan) = (NbofChWr) & 0x7FFFu;

	            if (MCF_eDMA_EDMA_ES_ERRCHN(edmaChan) != (MCF_eDMA_EDMA_ES&MCF_eDMA_EDMA_ES_ERRCHN(edmaChan)))
	            {
	                MCF_eDMA_EDMA_SERQ = edmaChan; //enable request signal
	                MCF_UART_UCR(channel) = MCF_UART_UCR_TX_ENABLED;
	                OSW_M_EXIT_CRITICAL();

	                GUEST_F_On_Printf((const char *)DTXTString, (INT16U)((NbofChWr) & 0x7FFFu));
	            }
	            else
	            {
	                GDF_M_FAULT_EXT(EVT_C_VOTE_HARDWARE_ERROR, 0xFF98u, "");
				}
			}
			else
			{
				NbofChWr = 0u; /*sprintf return -1, do nothing*/
			}
        }
    }

    return ((INT32S)NbofChWr);
}

//函数本身入参两个，调用该函数的地方，入参有3个/2个/1个
INT32S PRINT_F_PrintfInternal(const CHAR* fmt, ...)
{
    INT32S len = 0;

    GDF_M_NULL_ASSERT(fmt);

    len = PRINT_F_PrintfWithTaintInfo(fmt, (const STD_T_VAList) & fmt + 4u);  
    return len;
}

ALSTOM_T_Status PRINT_F_Create(MSTEPINC_T_TaskCreateInfo *pCreateInfo)
{
    ALSTOM_T_Status Status = ALSTOM_C_ERROR;
    OS_EVENT* pK2OO2QEvent = NULL;  //By ZhaoLiping  a2
    INT8U err = ALSTOM_C_ERROR;//0xC9  var_1

    static void* pDtxtQMsgTbl[0x20];
    static void* pK2OO2QMsgTbl[0x20];
    static INT32U pMsgArray[0x1260] = {0u};/*20190718, 将INT8U改为INT32U，解决优化后地址可能不是4字节对齐的问题 CR:1634*/
    static INT32U PRINT_V_DtxtStack[MSTEP_C_STACK_SIZE] = {0u};

    GDF_M_NULL_ASSERT(pCreateInfo);

    Status = SCHED_F_TaskInfoConfigure(&PRINT_V_TaskInfo);

    if (ALSTOM_C_NO_ERROR == Status)
    {
        STD_F_Memset(&PRINT_V_Statistic, 0u, 4u);//将PRINT_V_Statistic置为0

        PRINT_V_TaskInfo.pStatisticsAddr = &PRINT_V_Statistic;

        PRINT_V_TaskInfo.pQEvent = OSQCreate(pDtxtQMsgTbl, 0x20u);
        pK2OO2QEvent = OSQCreate(pK2OO2QMsgTbl, 0x20u);

        if ((NULL == PRINT_V_TaskInfo.pQEvent) || (NULL == pK2OO2QEvent))
        {
            GDF_M_FAULT_EXT(EVT_C_OS_ERROR, 0u, "");
        }
        else
        {
            PRINT_V_pFlagGrpNotif = OSW_F_FlagCreate(0u, &err);

            GDF_M_RANGE_ASSERT(ALSTOM_C_NO_ERROR != err, err);

            GDF_M_NULL_ASSERT(PRINT_V_pFlagGrpNotif);

            VOTE_F_Subscribe(MSTEP_TASK_ID_PRINT, pK2OO2QEvent, MSTEP_C_TASK_PRIOLEVEL_4, PRINT_V_pFlagGrpNotif, 1u);

            PRINT_V_LogMsgMemBlock = OSW_F_MemCreate(pMsgArray, 0x20u, 0x24Cu, &err);
            GDF_M_RANGE_ASSERT(ALSTOM_C_NO_ERROR != err, err);

            pCreateInfo->pTaskStack = PRINT_V_DtxtStack;
            pCreateInfo->taskStackSize = MSTEP_C_STACK_SIZE;  //正好是PRINT_V_DtxtStack数组的大小

            PRINT_V_TaskInfo.taskStatus = ALSTOM_C_TASK_NOTRUNNING;
            Status = ALSTOM_C_NO_ERROR;
        }
    }
    else
    {
        Status = ALSTOM_C_ERROR;
    }

    return Status;
}

ALSTOM_T_Status PRINT_F_Config(void)
{
    return ALSTOM_C_NO_ERROR;
}

ALSTOM_T_Status PRINT_F_Start(void)
{
    ALSTOM_T_Status status = ALSTOM_C_NO_ERROR;

    PRINT_V_TaskInfo.taskStatus = ALSTOM_C_TASK_RUNNING; //0x2Au
    PRINT_V_Started = TRUE;

    return status;
}

ALSTOM_T_Status PRINT_F_Task(MSTEPINC_E_FunctionalMode functionalMode)
{
    INT8U taskPri = 0u;
    INT8U err = ALSTOM_C_NO_ERROR;  //var_1
    INT8U bMemPut = 0u;
    INT32U status = 0u;
    INT32U recvFailureCnt = 0u; //var_C
    INT32U sendFailureCnt = 0u;//var_8
    PRINT_T_logMsg* pMsg = NULL;
    OSW_T_FLAGS flags_rdy1 = 0u;//OSW_F_FlagPend的返回值
    /*OSW_T_FLAGS flags_rdy2 = 0u; OSW_F_FlagAccept的返回值*/

    static INT32U PRINT_V_cpt = 1u;//Only for CPUA,  By ZLP, 2017.6.12,  暂时定为INT32U ?????
    static PRINT_T_logMsg localMsg;//是一个结构体


    GDF_M_RANGE_ASSERT(functionalMode > ALSTOM_C_FUNCTIONAL_BOOT, functionalMode);
    OSTimeDly(1u);

    taskPri = SCHED_F_GetPriorityFromTaskId(MSTEP_TASK_ID_PRINT);
    WDG_F_SWSetFlag(taskPri);

#ifdef VCU_CONF_CPU_A
    flags_rdy1 = OSW_F_FlagPend(PRINT_V_pFlagGrpNotif, 3u, OSW_C_FLAG_WAIT_SET_ANY, 0x258u, &err, 1u); //d3

    if (ALSTOM_C_NO_ERROR != err)
    {
        if (OSW_C_TIMEOUT != err)
        {
            GDF_M_FAULT_EXT(EVT_C_OS_ERROR, err, "");
        }
    }
    else
    {
        if (0u != (flags_rdy1 & 0x2u))
        {
            OSFlagPost((OS_FLAG_GRP*)PRINT_V_pFlagGrpNotif, 2u, 0u, &err);

            if (ALSTOM_C_NO_ERROR != err)
            {
                GDF_M_FAULT_EXT(EVT_C_OS_ERROR, err, "");
            }
            else
            {
                while (1)
                {
                    pMsg = (PRINT_T_logMsg*)OSW_F_QAccept((OSW_T_pEVENT)(PRINT_V_TaskInfo.pQEvent), &err);

                    if (ALSTOM_C_NO_ERROR == err)
                    {
                        GDF_M_NULL_ASSERT(pMsg);

                        PRINT_V_cpt++;
                        PRINT_F_PrintfInternal("%d %s\n", PRINT_V_cpt, pMsg->str);
                        bMemPut = OSW_F_MemPut(PRINT_V_LogMsgMemBlock, pMsg);

                        GDF_M_RANGE_ASSERT(0u != bMemPut, bMemPut);
                        OSW_F_FlagAccept(PRINT_V_pFlagGrpNotif, 2u, OSW_C_FLAG_WAIT_SET_ANY, &err);

                        if (ALSTOM_C_NO_ERROR != err)
                        {
                            if (OS_FLAG_ERR_NOT_RDY != err)
                            {
                                GDF_M_FAULT_EXT(EVT_C_OS_ERROR, err, "");
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        if (0x1Fu != err)
                        {
                            GDF_M_FAULT_EXT(EVT_C_OS_ERROR, err, "");
                        }
                        else
                        {
                            break;
                        }
                    }
                }
            }
        }

        if (0u != (flags_rdy1 & 0x1u))
        {
            VOTE_F_GetAndResetStat(MSTEP_TASK_ID_PRINT, &recvFailureCnt, &sendFailureCnt);

            if (0u != recvFailureCnt)
            {
                PRINT_V_cpt++;
                PRINT_F_PrintfInternal("%d ", PRINT_V_cpt);
                PRINT_F_PrintfInternal("LOST-RX-0x%08x\n", recvFailureCnt);
                PRINT_F_PrintfInternal("\x1B[0m");
            }

            localMsg.strSize = 0x248u;

            status = VOTE_F_ReadExt((INT8U * const)localMsg.str, (INT32U * const) & (localMsg.strSize), 0x100u, VOTE_C_FUNCTION_ID_READ, (INT16U)(__LINE__));

            if (ALSTOM_C_NO_ERROR == status)
            {
                PRINT_V_cpt++;
                PRINT_F_PrintfInternal("%d %s", PRINT_V_cpt, localMsg.str);
            }
        }
    }

#else
    OSTimeDly(0x64u);

#endif
    return ALSTOM_C_NO_ERROR;
}

void PRINT_F_ReportActiveIntegration(INT8U activeIntegrationTrace)
{
	static BOOLEAN PRINT_V_ActiveIntegrationTrace = FALSE; //规则检查,20210305
	
	if (1u == activeIntegrationTrace)
    {
        PRINT_V_ActiveIntegrationTrace = TRUE;
    }
    else
    {
        PRINT_V_ActiveIntegrationTrace = FALSE;
    }
}

void PRINT_F_PrintfAuth(BOOLEAN bPrintfAuth)
{
    PRINT_V_bPrintfAuth = bPrintfAuth;
}

/*打印换行和对齐 0xA==\n,0xD==\r 添加\0,计算整个字符串的长度 */
static void PRINT_F_PrintLineBreak(PRINT_T_logMsg* pMemBlk,INT32U strLen,INT32U strLen1,INT32U strLen2)
{
	CHAR* pDA0Start = NULL; //a0  表示字符串"DA0"开始的地址,By Zhaoliping
	CHAR* pDA0End = NULL;  //a1  表示字符串"DA0"结束的地址,By Zhaoliping
	CHAR* pStrStart = NULL; //表示字符串开始的地址,By Wukaiteng
	
	GDF_M_NULL_ASSERT(pMemBlk);
	GDF_M_RANGE_ASSERT((strLen+strLen1+strLen2) >= 0x248u,(strLen+strLen1+strLen2));
	
	pDA0Start = &(pMemBlk->str[strLen+strLen1+strLen2]);
	pStrStart = &(pMemBlk->str[0]);
	
    if ((strLen + strLen1 + strLen2 > 1u)  &&
        (0xAu == pMemBlk->str[strLen+strLen1+strLen2-1u])  &&    //extb.l d0????????
        (0xDu == pMemBlk->str[strLen+strLen1+strLen2-2u]))    //extb.l d0????????
    {
        pDA0Start = &(pMemBlk->str[strLen+strLen1+strLen2-2u]); //a0-2
    }
    else
    {
        if ((strLen + strLen1 + strLen2 > 0u) &&   //loc_3000BA92  a0>a3
            (0xAu == pMemBlk->str[strLen+strLen1+strLen2-1u]))
        {
            pDA0Start = &(pMemBlk->str[strLen+strLen1+strLen2-1u]); //a0-1
        }
    }

    //loc_8000BC54
    pDA0End = pDA0Start + 2u;

    if (pDA0End > (CHAR*)pMemBlk + 0x24Bu) //a1>d5
    {
        pDA0Start = (CHAR*)(pMemBlk + 0x249u);  //a0=d5-2
        pDA0End = (CHAR*)(pMemBlk + 0x24Bu);
    }

    *(pDA0Start) =  0xDu;

    *(pDA0Start + 1u) = 0xAu;
    *(pDA0End) = 0u;

    //表示从第一个字符开始到"DA0"结束，整个字符串的长度(最长为0x248)
    pMemBlk->strSize = (INT32U)(pDA0Start - pStrStart + 3u);//(a0+3-pMemBlk-4);  a3=pMemBlk+4;
}

//#ifndef PRINT_STUB
/*每一个字符串的最后面都要跟一个"DA0"结束(\r\n)，总字符串最长为0x248*/
/*20210302,Add maintenance information,CR:2616*/
void PRINT_F_Report(PRINT_E_ReportType    Type,
                    const char*            locString,
                    const char*            String,
                    INT8U            Numbval,
                    INT32U           Value1,
                    INT32U           Value2,
                    INT32U           Value3,
                    INT32U           Value4,
                    INT32U           Value5,
                    INT32U           Value6)
{
    INT8U bMemPut = 0u;
    INT8U qPost = 0u;
    INT8U err = ALSTOM_C_ERROR;
    INT32U commonTime = 0u; //var_8
    INT32U strLen1 = 0u;
    INT32U strLen2 = 0u;
    INT32U sLen = 0u; //d4
    INT32U n = 0u; //d3
    INT32S strLen = 0;
    ALSTOM_T_Status Status = ALSTOM_C_ERROR;
    PRINT_T_logMsg* pMemBlk = NULL;
	static INT32U PRINT_V_FiFoFullMsgLost = 0u;
	INT8U IrqNestLvl = 0u;
	
#ifdef TestPlatform
if ((TRUE == PRINT_V_Started)
            && (NULL != locString)&& (NULL != String)&& (Numbval < 7u) 
            && Tst_Flag_AllReport == TRUE && Tst_Flag_AllReportFlag[0]==TRUE
   )
{
	Tst_F_ReportStore(Report_Udp, Type, locString, String, Numbval, Value1, Value2, Value3,  Value4,  Value5,  Value6);
}
Tst_Flag_AllReportFlag[0] = TRUE;
#endif

#ifdef VCU_CONF_CPU_A
	if (Type < PRINT_V_PrintLevel)
#else
	IrqNestLvl = irqNestLvlGet();/*CR:2681*/
	if ((Type < PRINT_V_PrintLevel)	 && (IrqNestLvl == 0u))  
#endif
	//if (((PRINT_REPORT_INTEGRATION != Type) && (TRUE != PRINT_V_ActiveIntegrationTrace)) || (PRINT_REPORT_INTEGRATION == Type))
    {
//		
//		if(Type <= PRINT_REPORT_ERROR)
//		{
//	#ifdef VCU_CONF_CPU_A
//		; //最高位0表示CPUA //规则检查,20210305
//	#else
//		Value1 = Value1 | 0x80000000u ;	//最高位1表示CPUB //规则检查,20210305
//	#endif
//		}
		
        if ((TRUE == PRINT_V_Started)
            && (NULL != locString)
            && (NULL != String)
            && (Numbval < 7u))
        {
            SCHED_F_CommonTimeGet(&commonTime);
            pMemBlk = (PRINT_T_logMsg*)OSW_F_MemGet(PRINT_V_LogMsgMemBlock, &err);

            if ((NULL != pMemBlk) && (ALSTOM_C_NO_ERROR == err))
            {
                /*1.打印CPU*/
#ifdef VCU_CONF_CPU_A
                    strLen = strLen + STD_F_Snprintf(pMemBlk->str, 0x247u, "chanel1 ");
#else
                    strLen = strLen + STD_F_Snprintf(pMemBlk->str, 0x247u, "chanel2 ");
#endif

                /*2.打印丢失情况*/
                if (0u != PRINT_V_FiFoFullMsgLost)
                {
                    strLen = strLen + STD_F_Snprintf(&pMemBlk->str[strLen], 0x247u - (INT32U)strLen, "LOST-");
                    strLen = strLen + STD_F_Snprintf(&pMemBlk->str[strLen], 0x247u - (INT32U)strLen, "0x%08x-", PRINT_V_FiFoFullMsgLost);
                }

                /*3.打印时间*/
                strLen = strLen + STD_F_Snprintf(&pMemBlk->str[strLen], 0x247u - (INT32U)strLen, "0x%08x ", commonTime);

                /*4.打印文件及行号*/
                strLen1 = STD_F_Strlen(locString);

                /*5.打印字符串*/
                strLen2 = STD_F_Strlen(String);

                if ((strLen1 < 0x83u) && (strLen2 < 0x83u))
                {
                    strLen1 = strLen1;
                }
                else
                {
                    strLen1 = 0x82u;
                }

                n = strLen2 + strLen1;

                if (n > 0x82u)
                {
                    n = 0x82u;
                }

                STD_F_Memcpy(&pMemBlk->str[strLen], 0x247u - (INT32U)strLen, locString, n);

                STD_F_Memcpy(&pMemBlk->str[(INT32U)strLen+strLen1], 0x247u - (INT32U)strLen - strLen1, String, n - strLen1);
                /*6.打印值*/

                if (0u != Numbval)
                {
                    strLen = strLen + STD_F_Snprintf(&pMemBlk->str[(INT32U)strLen+strLen1+strLen2],
                                                     0x247u - (INT32U)strLen - strLen1 - strLen2, " 0x%08x", Value1);  //arg_10

                    if (Numbval > 1u)
                    {
                        strLen = strLen + STD_F_Snprintf(&pMemBlk->str[(INT32U)strLen+strLen1+strLen2],
                                                         0x247u - (INT32U)strLen - strLen1 - strLen2, " 0x%08x", Value2); //arg_14
                    }

                    if (Numbval > 2u)
                    {
                        strLen = strLen + STD_F_Snprintf(&pMemBlk->str[(INT32U)strLen+strLen1+strLen2],
                                                         0x247u - (INT32U)strLen - strLen1 - strLen2, " 0x%08x", Value3); //arg_18
                    }

                    if (Numbval > 3u)
                    {
                        strLen = strLen + STD_F_Snprintf(&pMemBlk->str[(INT32U)strLen+strLen1+strLen2],
                                                         0x247u - (INT32U)strLen - strLen1 - strLen2, " 0x%08x", Value4); //arg_1C
                    }

                    if (Numbval > 4u)
                    {
                        strLen = strLen + STD_F_Snprintf(&pMemBlk->str[(INT32U)strLen+strLen1+strLen2],
                                                         0x247u - (INT32U)strLen - strLen1 - strLen2, " 0x%08x", Value5); //arg_20
                    }

                    if (6u == Numbval)
                    {
                        strLen = strLen + STD_F_Snprintf(&pMemBlk->str[(INT32U)strLen+strLen1+strLen2],
                                                         0x247u - (INT32U)strLen - strLen1 - strLen2, " 0x%08x", Value6); //arg_24
                    }
                }

                /*6.打印XXX*/
                strLen = strLen + STD_F_Snprintf(&pMemBlk->str[(INT32U)strLen+strLen1+strLen2],
                                                 0x247u - (INT32U)strLen - strLen1 - strLen2, "\x1B[0m");

                /*7.打印换行和对齐 0xA==\n,0xD==\r*/
                PRINT_F_PrintLineBreak(pMemBlk, (INT32U)strLen, strLen1, strLen2);

#ifdef VCU_CONF_CPU_A
                qPost = OSW_F_QPost(PRINT_V_TaskInfo.pQEvent, pMemBlk);

                if (OSW_C_Q_FULL == qPost)
                {
                    if (0xFFFFFFFFu != PRINT_V_Statistic)
                    {
                        PRINT_V_Statistic++;
                    }

                    if (0xFFFFFFFFu != PRINT_V_FiFoFullMsgLost)
                    {
                        PRINT_V_FiFoFullMsgLost++;
                    }
                }
                else if (ALSTOM_C_NO_ERROR == qPost)
                {
                    PRINT_V_FiFoFullMsgLost = 0u;
                    OSFlagPost((OS_FLAG_GRP*)PRINT_V_pFlagGrpNotif, 2u, 1u, &qPost);

                    if (0u != qPost)
                    {
                        GDF_M_FAULT_EXT(EVT_C_SCHED_ERROR, qPost, "");
                    }
                }
                else
                {
                    GDF_M_FAULT_EXT(EVT_C_OS_ERROR, qPost, "");
                }

#else
                Status = VOTE_F_WriteExt(0x42u, (INT8U*) & (pMemBlk->str), pMemBlk->strSize, VOTE_C_FUNCTION_ID_WRITE, (INT16U)(__LINE__));

                if (ALSTOM_C_NO_ERROR == Status)
                {
                    PRINT_V_FiFoFullMsgLost = 0u;
                }
                else
                {
                    if (0xFFFFFFFFu != PRINT_V_Statistic)
                    {
                        PRINT_V_Statistic++;
                    }

                    if (0xFFFFFFFFu != PRINT_V_FiFoFullMsgLost)
                    {
                        PRINT_V_FiFoFullMsgLost++;
                    }
                }

                bMemPut = OSW_F_MemPut(PRINT_V_LogMsgMemBlock, pMemBlk);

                GDF_M_RANGE_ASSERT(ALSTOM_C_NO_ERROR != bMemPut, bMemPut);
#endif
            }
            else
            {
                if (0xFFFFFFFFu != PRINT_V_Statistic)
                {
                    PRINT_V_Statistic++;
                }

                if (0xFFFFFFFFu != PRINT_V_FiFoFullMsgLost)
                {
                    PRINT_V_FiFoFullMsgLost++;
                }
            }
        }
        else
        {
            if (0xFFFFFFFFu != PRINT_V_Statistic)
            {
                PRINT_V_Statistic++;
            }
        }
    }
}

INT32S PRINT_F_Printf(const CHAR *fmt, ...) //调用PRINT_F_Printf的函数的第二个参数是立即数
{
    INT32S len = 0;
#ifdef TestPlatform
    static char logStr[300];
	if (Tst_Flag_AllReport ==TRUE )
	{
		STD_F_VSnprintf(logStr, sizeof(logStr), fmt, (const STD_T_VAList) & fmt + 4u);
		Tst_F_ReportStore(Report_Udp,4u, "print.c:1>", (const char*)logStr, 0u,0u,0u,0u,0u,0u,0u);
	}
#endif
	
    GDF_M_NULL_ASSERT(fmt);

    if (FALSE == PRINT_V_bPrintfAuth)
    {
        PRINT_V_bPrintfTainted = TRUE;
    }

    
    if(PRINT_V_PrintLevel >= 2u)
    {
    	len = PRINT_F_PrintfWithTaintInfo(fmt, (const STD_T_VAList) & fmt + 4u);
    }
    return len;
}

void DTXT_F_Report(DTXT_E_ReportType    Type,
                   const char*            locString,
                   const char*            String,
                   INT8U            Numbval,
                   INT32U           Value1,
                   INT32U           Value2,
                   INT32U           Value3,
                   INT32U           Value4,
                   INT32U           Value5,
                   INT32U           Value6)
{
    PRINT_F_Report((PRINT_E_ReportType)Type, locString, String, Numbval, Value1, Value2, Value3, Value4, Value5, Value6);
}

INT32S DTXT_F_Printf(const CHAR *fmt, ...) //调用DTXT_F_Printf的函数的第二个参数是立即数
{
    INT32S len = 0;
#ifdef TestPlatform
	static char logStr[300];
	if(Tst_Flag_AllReport == TRUE && Tst_Flag_AllReportFlag[1] == TRUE)
	{
		STD_F_VSnprintf(logStr, sizeof(logStr), fmt, (const STD_T_VAList) & fmt + 4u);
		Tst_F_ReportStore(Report_Udp,4u, "print.c:2", (const char*)logStr, 0u,0u,0u,0u,0u,0u,0u);
	}
	Tst_Flag_AllReportFlag[1] = TRUE;

#endif
    GDF_M_NULL_ASSERT(fmt);

    if (FALSE == PRINT_V_bPrintfAuth)
    {
        PRINT_V_bPrintfTainted = TRUE;
    }

    len = PRINT_F_PrintfWithTaintInfo((const CHAR*)fmt, (const STD_T_VAList) & fmt + 4u);

    return len;
}
