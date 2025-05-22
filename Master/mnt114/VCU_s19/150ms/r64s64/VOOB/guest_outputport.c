/**
* @file guest_outputport.c
* @brief
* @author Wang Jiantao
* @date 2013-7-10
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: wangjiantao@casco.com.cn
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/

#include "guest_public.h"
#include "guest_outputport.h"
#include "guest_readback.h"

/*端口检测之前把g_OPTRESLTCNT拷到g_OPTRESLTCNTTEMP中，检测过程中改变g_OPTRESLTCNTTEMP中的值，
根据g_OPTRESLTCNT中的值来设置隔离态，如果24V不存在则不会把temp中的值拷贝回去，忽略掉这次的检测结果*/
GUEST_T_OPTCEL_TEST_RESULT_COUNTER g_OPTRESLTCNT;
GUEST_T_OPTCEL_TEST_RESULT_COUNTER g_OPTRESLTCNTTEMP;

static ALSTOM_T_Status GUEST_F_RIT_Step(INT8U Step, INT8U RITPortID, INT16U OLDCPUCMD);
static ALSTOM_T_Status GUEST_F_RIT_Step_Check(INT8U Step,
                                              INT8U RITPortID,
                                              GUEST_E_READBACK_SENSORS_STATE *SumRBPValue,
                                              GUEST_E_READBACK_SENSORS_STATE *SumRBNValue);
static ALSTOM_T_Status GUEST_F_TT_Check(GUEST_E_READBACK_SENSORS_STATE *SumRBPValue,
                                        GUEST_E_READBACK_SENSORS_STATE *SumRBNValue,
                                        INT16U CPUCMDDVBIT);
static ALSTOM_T_Status GUEST_F_CheckOPTReslt(void);

static void GUEST_F_Fault_Manage(INT8U *Cnt_OK, INT8U *Cnt_KO, INT8U TestResult);


/* 检查端口检测的结果 */
static ALSTOM_T_Status GUEST_F_CheckOPTReslt(void)
{
    ALSTOM_T_Status Status = ALSTOM_C_NO_ERROR;

    INT32U i = 0;

    const GUEST_T_CFG *pCFGAddr = GUEST_F_GetConfigPointer();

    GDF_M_NULL_ASSERT(pCFGAddr);

    for (i = 0; i < GUEST_C_OUTPUT_PORT_NUM; ++i)
    {
        /* 判断端口检测结果 */
        if (pCFGAddr->TPF <= g_OPTRESLTCNT.Cnt_OK[i])
        {
            /* 端口正常，重置计数器 */
            g_OPTRESLTCNT.Cnt_OK[i] = 0;
            g_OPTRESLTCNT.Cnt_Fault[i] = 0;
        }
        else
        {
            /* 端口认为是失效的，返回错误置隔离态 */
            if (pCFGAddr->TPF <= g_OPTRESLTCNT.Cnt_Fault[i])
            {
                DTXT_REPORT2(DTXT_REPORT_INFO, "Port Test Error", i, g_OPTRESLTCNT.Cnt_Fault[i]);

                Status = ALSTOM_C_ERROR;

                break;
            }
        }
    }

    return Status;
}


/* 端口检测程序入口 */
void GUEST_F_OPT(INT16U OLDCPUCMD, INT16U NEWCPUCMD)
{

    /* 检查内存 */
    GUEST_F_CheckBuf((INT8U*) & g_OPTRESLTCNTTEMP, (INT16U)(sizeof(GUEST_T_OPTCEL_TEST_RESULT_COUNTER)));

    /* 拷贝到temp中，测试时改变temp中的值 */
    STD_F_Memcpy(&g_OPTRESLTCNTTEMP, sizeof(GUEST_T_OPTCEL_TEST_RESULT_COUNTER),
                 &g_OPTRESLTCNT, sizeof(GUEST_T_OPTCEL_TEST_RESULT_COUNTER));

    GUEST_F_CheckBuf((INT8U*) & g_OPTRESLTCNT, (INT16U)(sizeof(GUEST_T_OPTCEL_TEST_RESULT_COUNTER)));

    STD_F_Memcpy(&g_OPTRESLTCNT, sizeof(GUEST_T_OPTCEL_TEST_RESULT_COUNTER),
                 &g_OPTRESLTCNTTEMP, sizeof(GUEST_T_OPTCEL_TEST_RESULT_COUNTER));

    /* 如果需要TT先执行TT检测，需要RIT或RIT正在执行，执行RIT检测，
    TT会打断RIT，每个周期只能执行一种测试 */

    if ((INT16U)0 != (INT16U)(NEWCPUCMD ^ OLDCPUCMD))
    {
        /*(void) DTXT_F_Printf("GUEST_F_TT Begin CycleNO %d OldCMD %x NewCMD %x\n", g_CycleNO, g_LastOutputs, NEWCPUCMD);*/
        GUEST_F_TT(OLDCPUCMD, NEWCPUCMD);
    }
    else
    {
        if ((TRUE == g_RITInvoke) || (TRUE == g_RITStart))
        {
            /*(void) DTXT_F_Printf("GUEST_F_RIT Begin CycleNO %d OldCMD %x NewCMD %x\n", g_CycleNO, OLDCPUCMD, NEWCPUCMD);*/

            GUEST_F_RIT(OLDCPUCMD);

            g_RITInvoke = FALSE;
        }
        else
        {
            GUEST_F_SCT();
        }
    }

    return;
}



/* 端口检测状态*/
ALSTOM_T_Status GUEST_F_OPT_Check(void)
{

    ALSTOM_T_Status Status = ALSTOM_C_NO_ERROR;

    /* 如果24V没有丢失，则把temp中的数据拷贝到g_OPTRESLTCNT，进行检测结果的检查 */

    if (FALSE == TRACKSIDE_POWER_MISSING)
    {
        STD_F_Memcpy(&g_OPTRESLTCNT, sizeof(GUEST_T_OPTCEL_TEST_RESULT_COUNTER),
                     &g_OPTRESLTCNTTEMP, sizeof(GUEST_T_OPTCEL_TEST_RESULT_COUNTER));
    }

    Status = GUEST_F_CheckOPTReslt();

    return Status;
}


void GUEST_F_SCT(void)
{

    INT8U TestResult[GUEST_C_OUTPUT_PORT_NUM];

    INT32U i = 0;
    INT32U StatusCRC[GUEST_C_OUTPUT_PORT_NUM];
    INT32U RMTStatusCRC[GUEST_C_OUTPUT_PORT_NUM];

    GUEST_E_READBACK_SENSORS_STATE SumRBPValue[GUEST_C_OUTPUT_PORT_NUM];
    GUEST_E_READBACK_SENSORS_STATE SumRBNValue[GUEST_C_OUTPUT_PORT_NUM];

    STD_F_Memset((void *)TestResult, 0x0, sizeof(TestResult));

    /* 检查内存 */
    GUEST_F_CheckBuf((INT8U*)SumRBPValue, (INT16U)GUEST_C_OUTPUT_PORT_NUM);

    GUEST_F_CheckBuf((INT8U*)SumRBNValue, (INT16U)GUEST_C_OUTPUT_PORT_NUM);

    /* 该函数已经保证了返回的状态两CPU的一致性 */
    GUEST_F_OPT_RBValueFromFPGA(SumRBPValue, SumRBNValue);

    for (i = 0; i < GUEST_C_OUTPUT_PORT_NUM; ++i)
    {
        /*如果输出允许态做了延迟，P关闭，N打开，SCT检测会出错，应采取TT检测*/
#ifdef ALSTOM_CONFIG_CPU_A

        /* Only For Test
        if ((100 == g_CycleNO)||(150 == g_CycleNO)||(200 == g_CycleNO))
        {
            SumRBPValue[0] = 1;
        }*/
        /* End Only For Test */


        if ((SumRBPValue[i] == GUEST_C_READBACK_SENSORS_UNSET)
            && (SumRBNValue[i] == GUEST_C_READBACK_SENSORS_SET))
        {
            TestResult[i] = ALSTOM_C_NO_ERROR;
        }
        else
        {
            TestResult[i] = ALSTOM_C_ERROR;
        }


#endif

#ifdef ALSTOM_CONFIG_CPU_B

        /* Only For Test
        if ((120 == g_CycleNO)||(150 == g_CycleNO)|| (201 == g_CycleNO))
        {
            SumRBPValue[0] = 0;
        }*/

        /* End Only For Test */


        if ((SumRBPValue[i] == GUEST_C_READBACK_SENSORS_SET)
            && (SumRBNValue[i] == GUEST_C_READBACK_SENSORS_UNSET))
        {
            TestResult[i] = ALSTOM_C_NO_ERROR;
        }
        else
        {
            TestResult[i] = ALSTOM_C_ERROR;
        }

#endif

        /*每个端口计算CRC值*/
        MSTEP_F_CryptlibCRC32FSFB2ChanOneCompute((INT8U *)&TestResult[i], sizeof(INT8U), &StatusCRC[i]);

    }

    /* 两CPU返回状态同步 */
    MSTEP_F_K2oo2CpuExchange((INT8U *)RMTStatusCRC,
                             (INT8U *)StatusCRC,
                             (sizeof(INT32U) * GUEST_C_OUTPUT_PORT_NUM),
                             GUEST_C_2oo2_TIMEOUT);

    for (i = 0u;i < GUEST_C_OUTPUT_PORT_NUM;++i)
    {

        if (RMTStatusCRC[i] != StatusCRC[i])
        {
            TestResult[i] = ALSTOM_C_ERROR;
        }

        GUEST_F_Fault_Manage(&g_OPTRESLTCNTTEMP.Cnt_OK[i], &g_OPTRESLTCNTTEMP.Cnt_Fault[i], TestResult[i]);

    }

    return;
}


/* RIT对每步读取的数据进行检查，该函数已经保证了两CPU返回状态的一致性 */
static ALSTOM_T_Status GUEST_F_RIT_Step_Check(INT8U Step,
                                              INT8U RITPortID,
                                              GUEST_E_READBACK_SENSORS_STATE *SumRBPValue,
                                              GUEST_E_READBACK_SENSORS_STATE *SumRBNValue)
{

    INT8U TestResult = 0;

    INT32U i = RITPortID;
    INT32U StatusCRC = 0;
    INT32U RMTStatusCRC = 0;

    GDF_M_RANGE_ASSERT((Step > 2), Step);
    GDF_M_RANGE_ASSERT((RITPortID > (GUEST_C_OUTPUT_PORT_NUM - 1)), RITPortID);
    GDF_M_NULL_ASSERT(SumRBPValue);
    GDF_M_NULL_ASSERT(SumRBNValue);

    switch (Step)
    {

        case 0:
#ifdef ALSTOM_CONFIG_CPU_A

            if ((SumRBPValue[i] == GUEST_C_READBACK_SENSORS_UNSET)
                && (SumRBNValue[i] == GUEST_C_READBACK_SENSORS_UNSET))
            {
                TestResult = ALSTOM_C_NO_ERROR;
            }
            else
            {
                TestResult = ALSTOM_C_ERROR;
            }

#endif
#ifdef ALSTOM_CONFIG_CPU_B
            if ((SumRBPValue[i] == GUEST_C_READBACK_SENSORS_SET)
                && (SumRBNValue[i] == GUEST_C_READBACK_SENSORS_SET))
            {
                TestResult = ALSTOM_C_NO_ERROR;
            }
            else
            {
                TestResult = ALSTOM_C_ERROR;
            }

#endif
            break;

        case 1:
#ifdef ALSTOM_CONFIG_CPU_A
            if ((SumRBPValue[i] == GUEST_C_READBACK_SENSORS_UNSET)
                && (SumRBNValue[i] == GUEST_C_READBACK_SENSORS_SET))
            {
                TestResult = ALSTOM_C_NO_ERROR;
            }
            else
            {
                TestResult = ALSTOM_C_ERROR;
            }

#endif
#ifdef ALSTOM_CONFIG_CPU_B
            if ((SumRBPValue[i] == GUEST_C_READBACK_SENSORS_SET)
                && (SumRBNValue[i] == GUEST_C_READBACK_SENSORS_UNSET))
            {
                TestResult = ALSTOM_C_NO_ERROR;
            }
            else
            {
                TestResult = ALSTOM_C_ERROR;
            }

#endif
            break;

        case 2:
#ifdef ALSTOM_CONFIG_CPU_A
            if ((SumRBPValue[i] == GUEST_C_READBACK_SENSORS_SET)
                && (SumRBNValue[i] == GUEST_C_READBACK_SENSORS_SET))
            {
                TestResult = ALSTOM_C_NO_ERROR;
            }
            else
            {
                TestResult = ALSTOM_C_ERROR;
            }

#endif
#ifdef ALSTOM_CONFIG_CPU_B
            if ((SumRBPValue[i] == GUEST_C_READBACK_SENSORS_UNSET)
                && (SumRBNValue[i] == GUEST_C_READBACK_SENSORS_UNSET))
            {
                TestResult = ALSTOM_C_NO_ERROR;
            }
            else
            {
                TestResult = ALSTOM_C_ERROR;
            }

#endif
            break;

        default:

            break;
    }

    /*测试端口计算CRC值*/
    MSTEP_F_CryptlibCRC32FSFB2ChanOneCompute((INT8U *)&TestResult, sizeof(INT8U), &StatusCRC);

    /* 两CPU返回状态同步 */
    MSTEP_F_K2oo2CpuExchange((INT8U *)&RMTStatusCRC, (INT8U *)&StatusCRC, sizeof(INT32U), GUEST_C_2oo2_TIMEOUT);

    if (RMTStatusCRC != StatusCRC)
    {
        TestResult = ALSTOM_C_ERROR;
    }

    GUEST_F_Fault_Manage(&g_OPTRESLTCNTTEMP.Cnt_OK[i], &g_OPTRESLTCNTTEMP.Cnt_Fault[i], TestResult);

    return TestResult;
}

static ALSTOM_T_Status GUEST_F_RIT_Step(INT8U Step, INT8U RITPortID, INT16U OLDCPUCMD)
{
    ALSTOM_T_Status Status = ALSTOM_C_NO_ERROR;

    INT8U FaultCNT = 0u;
    INT8U NXTCNT = 0u;
    INT16U CPUCMD = 0x00u;

    GUEST_E_READBACK_SENSORS_STATE SumRBPValue[GUEST_C_OUTPUT_PORT_NUM];
    GUEST_E_READBACK_SENSORS_STATE SumRBNValue[GUEST_C_OUTPUT_PORT_NUM];

    const GUEST_T_CFG *pCFGAddr = GUEST_F_GetConfigPointer();

    GDF_M_NULL_ASSERT(pCFGAddr);
    GDF_M_RANGE_ASSERT((RITPortID > (GUEST_C_OUTPUT_PORT_NUM - 1)), RITPortID);

    switch (Step)
    {

        case 0:
#ifdef ALSTOM_CONFIG_CPU_A
            CPUCMD = (INT16U)(~(((INT16U)(~OLDCPUCMD)) | (0x01 << RITPortID)));
#endif
#ifdef ALSTOM_CONFIG_CPU_B
            CPUCMD = OLDCPUCMD;
#endif
            break;

        case 1:
#ifdef ALSTOM_CONFIG_CPU_A
            CPUCMD = OLDCPUCMD;
#endif
#ifdef ALSTOM_CONFIG_CPU_B
            CPUCMD = OLDCPUCMD;
#endif
            break;

        case 2:
#ifdef ALSTOM_CONFIG_CPU_A
            CPUCMD = OLDCPUCMD;
#endif
#ifdef ALSTOM_CONFIG_CPU_B
            CPUCMD = (INT16U)(~(((INT16U)(~OLDCPUCMD)) | (0x01 << RITPortID)));
#endif
            break;

        default:
            break;
    }

    GUEST_F_DriveOutput(CPUCMD, FALSE, GUEST_C_LED_OPEN);

    OSW_F_TimeDly(DELAY_TM1);

    while (1)
    {
        /* 检查内存 */
        GUEST_F_CheckBuf((INT8U*)SumRBPValue, (INT16U)GUEST_C_OUTPUT_PORT_NUM);

        GUEST_F_CheckBuf((INT8U*)SumRBNValue, (INT16U)GUEST_C_OUTPUT_PORT_NUM);

        /* 该函数已经保证了返回的状态两CPU的一致性 */
        GUEST_F_OPT_RBValueFromFPGA(SumRBPValue, SumRBNValue);

        Status = GUEST_F_RIT_Step_Check(Step, RITPortID, SumRBPValue, SumRBNValue);

        if (ALSTOM_C_NO_ERROR == Status)
        {
            if (1 != Step)
            {
                break;
            }
            else
            {
                /* RIT第二步要执行NXT次 */
                ++NXTCNT;

                if (pCFGAddr->NXT <= NXTCNT)
                {
                    break;
                }
            }
        }
        else
        {
            ++FaultCNT;
            DTXT_REPORT3(DTXT_REPORT_INFO, "GUEST_F_RIT_Step_Check Error RITPortID FaultCNT Cnt_Fault",
                         RITPortID, FaultCNT, g_OPTRESLTCNTTEMP.Cnt_Fault[RITPortID]);
            /*如果重复检测错误次数大于N，或者端口错误计数大于阀值，则返回置隔离态*/

            if ((N <= FaultCNT) || (pCFGAddr->TPF <= g_OPTRESLTCNTTEMP.Cnt_Fault[RITPortID]))
            {
                Status = ALSTOM_C_ERROR;
                break;
            }

        }

    }

    /*(void) DTXT_F_Printf("GUEST_F_RIT EXT Step %d PortID %d FaultCNT %d\n", Step, RITPortID, FaultCNT);*/

    return Status;
}

/* RIT includes 3 steps, between each step, does not need delay,
because after every step read back, and then 2oo2 */
void GUEST_F_RIT(INT16U OLDCPUCMD)
{
    ALSTOM_T_Status Status = ALSTOM_C_NO_ERROR;

    INT8U Step = 0;

    static INT8U RITPortID = 0;

    if (TRUE == g_RITInvoke)
    {
        RITPortID = 0;
        g_RITInvoke = FALSE;
        g_RITStart = TRUE;
        g_RITflag = FALSE;
    }

    /* 执行三步检测 */
    while (1)
    {
        Status = GUEST_F_RIT_Step(Step, RITPortID, OLDCPUCMD);

        /* FCT Err
        if((RITPortID == 7)&&(Step == 1))
        {
            Status = ALSTOM_C_ERROR;
        }*/

        /* 如果回读的端口状态检查错误，则不再执行下一步 */

        if (ALSTOM_C_ERROR == Status)
        {
            RITestPortID = RITPortID;
            RITestStep = Step;
            g_RITflag = TRUE;
            DTXT_REPORT3(DTXT_REPORT_INFO, "=========RITTest Err=========", RITPortID, Step, Status);
            GUEST_F_TFTP_SendTestInfo(GUEST_TFTP_MSG_RESU, 0x04, TFTP_MSG_RITTEST, Status);/**/

            break;
        }

        ++Step;

        if (Step > 2)
        {
            break;
        }
    }

    ++RITPortID;

    if (RITPortID > (GUEST_C_OUTPUT_PORT_NUM - 1))
    {
        /* 已经完成了一轮检测 */
        RITPortID = 0;
        g_RITStart = FALSE;
    }

    return;
}


/* TT对读取的数据进行检查，该函数已经保证了两CPU返回状态的一致性 */
static ALSTOM_T_Status GUEST_F_TT_Check(GUEST_E_READBACK_SENSORS_STATE *SumRBPValue,
                                        GUEST_E_READBACK_SENSORS_STATE *SumRBNValue,
                                        INT16U CPUCMDDVBIT)
{
    ALSTOM_T_Status Status = ALSTOM_C_NO_ERROR;
    INT8U TestResult[GUEST_C_OUTPUT_PORT_NUM];

    INT32U i = 0;
    INT32U StatusCRC[GUEST_C_OUTPUT_PORT_NUM + 1];
    INT32U RMTStatusCRC[GUEST_C_OUTPUT_PORT_NUM + 1];

    GDF_M_NULL_ASSERT(SumRBPValue);
    GDF_M_NULL_ASSERT(SumRBNValue);

    STD_F_Memset((void *)TestResult, 0x0, sizeof(TestResult));

    for (i = 0; i < GUEST_C_OUTPUT_PORT_NUM; ++i)
    {
        if ((INT16U)0x01 == (INT16U)((CPUCMDDVBIT >> i) & 0x01))
        {
#ifdef ALSTOM_CONFIG_CPU_A

            if ((SumRBPValue[i] == GUEST_C_READBACK_SENSORS_UNSET)
                && (SumRBNValue[i] == GUEST_C_READBACK_SENSORS_UNSET))
            {
                TestResult[i] = ALSTOM_C_NO_ERROR;
            }
            else
            {
                TestResult[i] = ALSTOM_C_ERROR;
                Status = ALSTOM_C_ERROR;
            }

#endif

#ifdef ALSTOM_CONFIG_CPU_B
            if ((SumRBPValue[i] == GUEST_C_READBACK_SENSORS_SET)
                && (SumRBNValue[i] == GUEST_C_READBACK_SENSORS_SET))
            {
                TestResult[i] = ALSTOM_C_NO_ERROR;
            }
            else
            {
                TestResult[i] = ALSTOM_C_ERROR;
                Status = ALSTOM_C_ERROR;
            }

#endif

            /*每个端口计算CRC值*/
            MSTEP_F_CryptlibCRC32FSFB2ChanOneCompute((INT8U *)&TestResult[i], sizeof(INT8U), &StatusCRC[i]);

        }
    }

    /*测试状态计算CRC值*/
    MSTEP_F_CryptlibCRC32FSFB2ChanOneCompute((INT8U *)&Status, sizeof(ALSTOM_T_Status),
                                             &StatusCRC[GUEST_C_OUTPUT_PORT_NUM]);

    /* 两CPU返回状态同步 */
    MSTEP_F_K2oo2CpuExchange((INT8U *)RMTStatusCRC,
                             (INT8U *)StatusCRC,
                             (sizeof(INT32U) * (GUEST_C_OUTPUT_PORT_NUM + 1)),
                             GUEST_C_2oo2_TIMEOUT);

    for (i = 0u;i < GUEST_C_OUTPUT_PORT_NUM;++i)
    {

        if (RMTStatusCRC[i] != StatusCRC[i])
        {
            TestResult[i] = ALSTOM_C_ERROR;
        }

        GUEST_F_Fault_Manage(&g_OPTRESLTCNTTEMP.Cnt_OK[i], &g_OPTRESLTCNTTEMP.Cnt_Fault[i], TestResult[i]);

    }

    if (RMTStatusCRC[GUEST_C_OUTPUT_PORT_NUM] != StatusCRC[GUEST_C_OUTPUT_PORT_NUM])
    {
        Status = ALSTOM_C_ERROR;
    }

    return Status;
}

void GUEST_F_TT(INT16U OLDCPUCMD, INT16U NEWCPUCMD)
{

    ALSTOM_T_Status Status = ALSTOM_C_NO_ERROR;

    INT16U CPUCMD = 0x00;
    INT16U CPUCMDDVBIT = 0x00;
    INT8U FaultCNT = 0;
    GUEST_E_READBACK_SENSORS_STATE SumRBPValue[GUEST_C_OUTPUT_PORT_NUM];
    GUEST_E_READBACK_SENSORS_STATE SumRBNValue[GUEST_C_OUTPUT_PORT_NUM];

    CPUCMDDVBIT = (INT16U)(OLDCPUCMD ^ NEWCPUCMD);

#ifdef ALSTOM_CONFIG_CPU_A
    CPUCMD = (INT16U)(OLDCPUCMD & (~(OLDCPUCMD ^ NEWCPUCMD)));
#endif

#ifdef ALSTOM_CONFIG_CPU_B
    CPUCMD = (INT16U)(OLDCPUCMD | (OLDCPUCMD ^ NEWCPUCMD));
#endif

    GUEST_F_DriveOutput(CPUCMD, FALSE, GUEST_C_LED_OPEN);
    OSW_F_TimeDly(DELAY_TM2);

    while (1)
    {
        /* 检查内存 */
        GUEST_F_CheckBuf((INT8U*)SumRBPValue, (INT16U)GUEST_C_OUTPUT_PORT_NUM);

        GUEST_F_CheckBuf((INT8U*)SumRBNValue, (INT16U)GUEST_C_OUTPUT_PORT_NUM);

        /* 该函数已经保证了返回的状态两CPU的一致性 */
        GUEST_F_OPT_RBValueFromFPGA(SumRBPValue, SumRBNValue);

        Status = GUEST_F_TT_Check(SumRBPValue, SumRBNValue, CPUCMDDVBIT);

        if (ALSTOM_C_NO_ERROR == Status)
        {
            break;
        }
        else
        {
            ++FaultCNT;
            DTXT_REPORT1(DTXT_REPORT_INFO, "GUEST_F_TT_Check Error", FaultCNT);

            if (N <= FaultCNT)
            {
                break;
            }
        }

    }

    return;
}

static void GUEST_F_Fault_Manage(INT8U *Cnt_OK, INT8U *Cnt_KO, INT8U TestResult)
{

    GDF_M_NULL_ASSERT(Cnt_OK);
    GDF_M_NULL_ASSERT(Cnt_KO);

    if (TestResult == ALSTOM_C_NO_ERROR)
    {
        /*Test is successful*/
        (*Cnt_OK) ++;
    }
    else
    {
        /*Test is failed*/
        (*Cnt_KO) ++;
        (*Cnt_OK) = 0u;
    }

    return;
}

/* EOF */
