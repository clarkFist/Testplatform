/**
* @file guest_public.c
* @brief
* @author Wang Jiantao
* @date 2013-5-22
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: wangjiantao@casco.com.cn
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/

#include "guest_public.h"
#include "guest_readback.h"
#include "guest_outputport.h"

/*PDDM value definination*/
#define SWITCH_NORMAL                     0x55u
#define SWITCH_REVERSE                    0xAAu
#define SWITCH_NONE                          0xFFu

BOOLEAN g_ISOInvoke = FALSE;                       /* 触发隔离 */
BOOLEAN ISOLATION_STATE = FALSE;                   /* 隔离状态 */
BOOLEAN TRACKSIDE_POWER_MISSING = FALSE;           /* 24V存在状态 */
BOOLEAN TRACKSIDE_POWER_OVERAGE = FALSE;           /* 24V过压状态 */
BOOLEAN FUSE_POWER_MISSING = FALSE;                /* 12V存在状态 */
BOOLEAN g_RITInvoke = FALSE;                       /* 触发RIT */
BOOLEAN g_RITStart = FALSE;
BOOLEAN g_ISOTInvoke = FALSE;
BOOLEAN g_ISOTStart = FALSE;
BOOLEAN g_ISOTFlag = FALSE;
BOOLEAN g_TFTPBUFFULL = FALSE;                     /*判断TFTP buffer 是否存满*/
BOOLEAN g_RITflag = FALSE;
BOOLEAN g_TFTPERRBUFFULL = FALSE;                  /*判断FailCode buffer 是否存满*/

GUST_E_RUN_STATE g_RunState = GUEST_C_STATE_UNDEFINED;

/*FCT 功能检测控制变量*/
INT16U g_StartTest = TFTP_MSG_WAITING;
INT16U g_LastTest = 0xFFFF;
INT16U g_TestStore = TFTP_MSG_WAITING;


INT8U RITestStep = 0;
INT8U RITestPortID = 0;
INT8U ISOTestStep = 0;
INT8U FUSETestStep = 0;

/* 各任务的计数器 */
INT32U g_CycleNO = 0xFFFFFFFF;
INT32U g_ISOTCycleNO = 0xFFFFFFFF;
INT32U g_FUSETCycleNO = 0xFFFFFFFF;
INT32U g_TFTPCycleNO = 0xFFFFFFFF;

INT32U g_InterruptCNT = 0xFFFFFFFF;

INT32U WriteOffset = 0u;
INT32U WriteFCodeOffset = 0u;

INT8U *g_ParaAddr = NULL;                          /* 存放guest parameter的地址 */
INT8U g_TFTPBUF[GUEST_C_TFTP_BUF_SIZE];            /* 存放TFTP消息的内存 */
INT8U g_TFTPBUF_HAND[GUEST_C_TFTP_HAND_BUF_SIZE];            /* 存放TFTP握手包消息的内存 */
INT8U g_TFTPERRBUF[GUEST_C_TFTP_ERRBUF_SIZE];            /* 存放TFTP ERR消息的内存 */

INT16U Snakebody[16][16];

GUEST_T_CFG *g_CFGAddr = NULL;                     /* 存放guest config的地址 */

GUEST_T_SEM g_Sem = {NULL, NULL, NULL, NULL};

GUEST_T_USR_CFG g_Usr_Cfg_A = {0, 0, 0, 0};
GUEST_T_USR_CFG g_Usr_Cfg_B = {0, 0, 0, 0};

/**Guest fuses state initialized from kernel hook*/
GUEST_TEST_E_GuestSafetyFlagState GUEST_TEST_V_brokenSafetyFlags[GUEST_C_MAX_SAFETY_FLAG - GUEST_C_MIN_SAFETY_FLAG + 1];

MSTEP_T_VitalStamp VitalStamp[GUEST_C_TASK_NUM];

//static BOOLEAN g_SetDYNActive = FALSE;


static void GUEST_ISR_F_MainLoop(void);
static void GUEST_F_ToggleDYN(void);

static void GUEST_F_24VLightLED(INT16U cmd);
static void GUEST_F_OUTPUTLightLED(INT16U cmd);


/* 回读输出端口的接口函数 */
void GUEST_F_RBOutput(INT16U cmd)
{
    ALSTOM_T_Status Status = ALSTOM_C_NO_ERROR;

    INT16U RBOutputs = GUEST_C_RESTRICTIVE_CMD;

    INT32U StatusCRC = 0u;
    INT32U RMTStatusCRC = 0u;

    GUEST_M_FPGA_Read(0x0u, RBOutputs);

    if (cmd == RBOutputs)
    {
        Status = ALSTOM_C_NO_ERROR;
    }
    else
    {
        Status = ALSTOM_C_ERROR;
    }

    /* 两CPU返回状态同步 */
    MSTEP_F_CryptlibCRC32FSFB2ChanOneCompute((INT8U *)&Status, sizeof(Status), &StatusCRC);

    MSTEP_F_K2oo2CpuExchange((INT8U *)&RMTStatusCRC,
                             (INT8U *)&StatusCRC,
                             sizeof(StatusCRC),
                             GUEST_C_2oo2_TIMEOUT);

    if (RMTStatusCRC != StatusCRC)
    {
        Status = ALSTOM_C_ERROR;
    }


    DTXT_REPORT1(DTXT_REPORT_INFO, "=========RBOutput Result=========", Status);

    GUEST_F_TFTP_SendTestInfo(GUEST_TFTP_MSG_RESU, 0x03, TFTP_MSG_RBOUTPUT, Status);

    return;
}


/* Light LED */
static void GUEST_F_24VLightLED(INT16U cmd)
{
    GUEST_M_FPGA_Write(GUEST_C_24VLIGHT_LAMP_ADDR, ~cmd);
}

static void GUEST_F_OUTPUTLightLED(INT16U cmd)
{
    GUEST_M_FPGA_Write(GUEST_C_OUTPUTLIGHT_LAMP_ADDR, ~cmd);
}

/* 对外驱动输出的接口函数 */
void GUEST_F_DriveOutput(INT16U cmd, BOOLEAN SetOutput, GUEST_E_LED_Mode Drvmode)
{
    GUEST_M_FPGA_Write(0x0u, cmd);

    if(SetOutput == TRUE)
    {
        GUEST_F_OutputLED(cmd, Drvmode);
    }
}


void GUEST_F_24VLED(GUEST_E_LED_Mode mode)
{
    static BOOLEAN IsTurnOn = FALSE;

    switch (mode)
    {

        case GUEST_C_LED_OPEN:
            GUEST_F_24VLightLED(0x00);
            break;

        case GUEST_C_LED_CLOSE:
            GUEST_F_24VLightLED(0xFF);
            break;

        case GUEST_C_LED_FLASH:

            if (FALSE == IsTurnOn)
            {
                GUEST_F_24VLightLED(0x00);
                IsTurnOn = TRUE;
            }
            else
            {
                GUEST_F_24VLightLED(0xFF);
                IsTurnOn = FALSE;
            }

            break;

        case GUEST_C_LED_LONG_FLASH:

        default:

            break;
    }
}



void GUEST_F_EXTLED(GUEST_E_LED_Mode mode)
{
    static BOOLEAN IsTurnOn = FALSE;

    switch (mode)
    {

        case GUEST_C_LED_OPEN:
            GUEST_F_EXTLightSetOn();
            break;

        case GUEST_C_LED_CLOSE:
            GUEST_F_EXTLightSetOff();
            break;

        case GUEST_C_LED_FLASH:

            if (FALSE == IsTurnOn)
            {
                GUEST_F_EXTLightSetOn();
                IsTurnOn = TRUE;
            }
            else
            {
                GUEST_F_EXTLightSetOff();
                IsTurnOn = FALSE;
            }

            break;

        case GUEST_C_LED_LONG_FLASH:

        default:

            break;
    }
}


void GUEST_F_OutputLED(INT16U cmd, GUEST_E_LED_Mode mode)
{
    static BOOLEAN IsTurnOn = FALSE;

    if ((mode == GUEST_C_LED_OPEN) || (mode == GUEST_C_LED_CLOSE))
    {
        GUEST_F_OUTPUTLightLED(cmd);
    }
    else
    {
        if (mode == GUEST_C_LED_FLASH)
        {
            if (FALSE == IsTurnOn)
            {
                GUEST_F_OUTPUTLightLED(cmd);
                IsTurnOn = TRUE;
            }
            else
            {
                GUEST_F_OUTPUTLightLED(GUEST_C_RESTRICTIVE_CMD);
                IsTurnOn = FALSE;
            }
        }
    }

}/**/


/* Set the DYN signal to stable state */
void GUEST_F_SetDYNStable(void)
{
    /* disable the timeinterrupt */
    MSTEP_F_IsrDisable();
}

/* Set the DYN signal to active state */
void GUEST_F_SetDYNActive(void)
{
    /* Enable the timeinterrupt */
    MSTEP_F_IsrEnable();
}

/* FPGA A对FPGA B产生的方波进行检查，并把检查的结果保存到寄存器，CPU读取进行检查 */
void GUEST_F_CheckFPGACNT(void)
{
    ALSTOM_T_Status Status = ALSTOM_C_NO_ERROR;

    INT16U FPGACNT = 0u;

    INT32U StatusCRC = 0u;
    INT32U RMTStatusCRC = 0u;

    //FCT Normal
    GUEST_M_FPGA_Read(GUEST_C_FPGACNT_ADDR, FPGACNT);
    //FCT Err
    //FPGACNT = *((INT16U *)GUEST_C_FPGACNT_ADDR)+20;

    if ((FPGACNT >= GUEST_C_FPGACNT_MIN) && (FPGACNT <= GUEST_C_FPGACNT_MAX))
    {
        Status = ALSTOM_C_NO_ERROR;
    }
    else
    {
        Status = ALSTOM_C_ERROR;
    }

    /* 两CPU回读的数据进行2oo2 */
    MSTEP_F_CryptlibCRC32FSFB2ChanOneCompute((INT8U *)&Status, (INT32U)sizeof(Status), &StatusCRC);

    MSTEP_F_K2oo2CpuExchange((INT8U *)&RMTStatusCRC, (INT8U *)&StatusCRC, sizeof(StatusCRC), GUEST_C_2oo2_TIMEOUT);

    if (RMTStatusCRC != StatusCRC)
    {
        (void) DTXT_F_Printf("Check FPGA Error", FPGACNT);
        Status = ALSTOM_C_ERROR;

    }

    DTXT_REPORT1(DTXT_REPORT_INFO, "=========FPGACNT Result=========", Status);

    GUEST_F_TFTP_SendTestInfo(GUEST_TFTP_MSG_RESU, 0x03, TFTP_MSG_FPGACNT, Status);

    return;
}

const GUEST_T_CFG * GUEST_F_GetConfigPointer(void)
{
    /*INT8U *pConfigGuestSubArea = NULL;

    MSTEP_F_ConfigGuestSubAreaGet((const INT8U **)(&pConfigGuestSubArea));

    return (const GUEST_T_CFG *)pConfigGuestSubArea;
    */

    /*Only for test*/
    static GUEST_T_CFG EMCconfig;

    EMCconfig.BOARD_KIND_ID = 0x1111u;
    EMCconfig.OUTPUT_TEST_CYCLE_PROD = 3600u;
    EMCconfig.ISOLATION_TEST_CYCLE_PROD = 3600u;
    EMCconfig.FUSE_TEST_CYCLE_PROD = 3600u;
    EMCconfig.TPF = 2u;
    EMCconfig.THL = 10u;
    EMCconfig.THH = 14u;
    EMCconfig.NXT = 2u;
    EMCconfig.DELT = 2u;
EMCconfig.FAULT_FILTER_TIMEOUT=0;
EMCconfig.FAULT_FILTER_ACTIVATION=1;
    EMCconfig.POU = 0xFFFFu;

    return (const GUEST_T_CFG *)&EMCconfig;
    /*End*/

}

void GUEST_F_GetBoardPosition(void)
{
    INT8U RackNumber = 0u;
    INT8U SlotNumber = 0u;

    MSTEP_F_ConfigBoardPositionGet(&RackNumber, &SlotNumber);
    DTXT_F_Printf("-----------RackNO:%d SlotNO%d-------------\n", RackNumber, SlotNumber);
}

void GUEST_F_24V_Test(void)
{
    ALSTOM_T_Status Status = ALSTOM_C_NO_ERROR;

    /* 24V检测 */
    Status = GUEST_F_RBValue((GUST_E_SENSORS)GUEST_C_24V_PST, (INT8U)GUEST_C_READBACK_SENSORS_UNSET, (INT32U)MSTEPINC_C_TASK_ID_1);

    DTXT_REPORT1(DTXT_REPORT_INFO, "=========24VTest Result=========", Status);

    GUEST_F_TFTP_SendTestInfo(GUEST_TFTP_MSG_RESU, 0x03, g_StartTest, Status);

    return;
}


void GUEST_F_12V_Test(void)
{
    ALSTOM_T_Status Status = ALSTOM_C_NO_ERROR;

    Status = GUEST_F_RBValue((GUST_E_SENSORS)GUEST_C_12V_PST, (INT8U)GUEST_C_READBACK_SENSORS_UNSET, MSTEPINC_C_TASK_ID_1);

    DTXT_REPORT1(DTXT_REPORT_INFO, "=========12V_FTest Result=========", Status);
    GUEST_F_TFTP_SendTestInfo(GUEST_TFTP_MSG_RESU, 0x03, TFTP_MSG_12VF, Status);

    return;
}


void GUEST_F_LED_Test(void)
{

    INT16U CPUCMD = 0x00u;
    INT16U PortID = 0;
    static INT8U counter = 0;
    static BOOLEAN IsTurnOn = FALSE;

    if (g_LastTest != TFTP_MSG_LEDTEST)
    {
        counter = 0;
    }

    /*EXTLED
    GUEST_F_EXTLED((GUEST_E_LED_Mode)GUEST_C_LED_FLASH);*/

    /*24VLED*/
    GUEST_F_24VLED((GUEST_E_LED_Mode)GUEST_C_LED_FLASH);

    /*Output LED*/
    if (counter == 0)
    {
        IsTurnOn = FALSE;
    }

    if (counter == 8)
    {
        IsTurnOn = TRUE;
    }

    if (IsTurnOn == FALSE)
    {
        PortID = counter * 2 + 1;
        ++counter;
    }
    else
    {
        --counter;
        PortID = counter * 2;
    }

    CPUCMD = (INT16U)(~(1 << PortID));

    GUEST_F_OutputLED(CPUCMD, (GUEST_E_LED_Mode)GUEST_C_LED_OPEN);

}

void GUEST_F_Output_Test(void)
{
    ALSTOM_T_Status Status = ALSTOM_C_NO_ERROR;

    static INT32U TTloopID = 0;
    static INT32U RITloopID = 0;

    if (g_LastTest != TFTP_MSG_RITTEST)
    {
        RITloopID = 0;
    }

    if (0 == (RITloopID % FCT_RIT_Testcycle))
    {
        g_RITInvoke = TRUE;
    }

    if (g_LastTest != TFTP_MSG_TTTEST)
    {
        TTloopID = 0;
    }

    switch (g_StartTest)
    {

        case TFTP_MSG_SCTTEST:

            GUEST_F_SCT();

            Status = GUEST_F_OPT_Check();

            if (ALSTOM_C_NO_ERROR == Status)
            {
                DTXT_REPORT1(DTXT_REPORT_INFO, "=========SCT Result=========", Status);
                GUEST_F_TFTP_SendTestInfo(GUEST_TFTP_MSG_RESU, 0x03, TFTP_MSG_SCTTEST, Status);
            }
            else
            {
                DTXT_REPORT1(DTXT_REPORT_INFO, "=========SCT Err=========", Status);
                GUEST_F_TFTP_SendTestInfo(GUEST_TFTP_MSG_RESU, 0x03, TFTP_MSG_SCTTEST, Status);
            }

            break;

        case TFTP_MSG_TTTEST:

            if (0 == (TTloopID % FCT_TT_Testcycle))
            {
                GUEST_F_TT(0xFFFF, 0x0000);

                Status = GUEST_F_OPT_Check();

                if (ALSTOM_C_NO_ERROR == Status)
                {
                    DTXT_REPORT1(DTXT_REPORT_INFO, "=========TT Result=========", Status);
                    GUEST_F_TFTP_SendTestInfo(GUEST_TFTP_MSG_RESU, 0x03, TFTP_MSG_TTTEST, Status);
                }
                else
                {
                    DTXT_REPORT1(DTXT_REPORT_INFO, "=========TT Err=========", Status);
                    GUEST_F_TFTP_SendTestInfo(GUEST_TFTP_MSG_RESU, 0x03, TFTP_MSG_TTTEST, Status);
                }

                /* TT检测后恢复输出端口继电器的限制状态 */
                GUEST_F_DriveOutput(0xFFFF, FALSE, GUEST_C_LED_OPEN);
            }

            TTloopID++;

            break;

        case TFTP_MSG_RITTEST:

            if ((TRUE == g_RITInvoke) || (TRUE == g_RITStart))
            {

                GUEST_F_RIT(0xFFFF);

                if ((g_RITflag == FALSE) && (FALSE == g_RITStart))
                {
                    DTXT_REPORT1(DTXT_REPORT_INFO, "=========RITTest Result=========", Status);
                    GUEST_F_TFTP_SendTestInfo(GUEST_TFTP_MSG_RESU, 0x04, TFTP_MSG_RITTEST, Status);/**/
                }
            }

            RITloopID++;

            /* RIT检测后恢复输出端口继电器的限制状态 */
            GUEST_F_DriveOutput(0xFFFF, FALSE, GUEST_C_LED_OPEN);

            break;

        default:
            DTXT_REPORT1(DTXT_REPORT_INFO, "Error TestID", g_StartTest);/**/
            break;
    }
}

/* Timeinterrupt initialization function */
void GUEST_F_ISRINIT(void)
{
    MSTEP_F_IsrConfigure((MSTEP_T_IsrFunctionPtr)GUEST_ISR_F_MainLoop, MSTEP_C_ISR_BOTH_EDGES);
}

/* The ISR function, invoke 5ms */
static void GUEST_ISR_F_MainLoop(void)
{
    /* Acknowledge the interrupt source, already received */
    MSTEP_F_IsrAck();

    /* 翻转动态信号 */
    GUEST_F_ToggleDYN();

    /* 监控任务状态 */

    ++g_InterruptCNT;
}

/* 控制动态信号，翻转或置稳定态 */
static void GUEST_F_ToggleDYN(void)
{

#ifdef ALSTOM_CONFIG_CPU_A
    MSTEP_F_GpioToggle(MSTEPINC_C_GPIO_ID10);
#endif

#ifdef ALSTOM_CONFIG_CPU_B
    MSTEP_F_GpioToggle(MSTEPINC_C_GPIO_ID14);
#endif

}

/* 把执行命令写到buffer中，循环存储 */
void GUEST_F_WriteDataToTFTPBUF(INT16U cmd)
{
   INT8U buf[GUEST_C_TFTP_INFO_LEN];
   
    
    INT8U RackNumber = 0u;
    INT8U SlotNumber = 0u;
    INT32U i = 0u;
    INT32U SumPort = 0u;
    INT8U ByteNum =0u;
    INT8U  errNO = 0;
    INT32U MessLength =0;
    INT32U LeftLength =0;
    MSTEP_T_CanTimeOfDay Time;
	
    const GUEST_T_CFG *pCFGAddr = GUEST_F_GetConfigPointer();	  
    GDF_M_NULL_ASSERT(pCFGAddr);
    STD_F_Memset((void*)buf,0x0u,GUEST_C_TFTP_INFO_LEN);
	
    /*找出配置中所有启用的端口,计算端口数所占用字节数*/

        for (i = 0; i < GUEST_C_OUTPUT_PORT_NUM; ++i)
        {
            if (1u == (((pCFGAddr->POU) >> i) & 1u))
            {
                ++SumPort;
            }
        }

	if(SumPort%8 ==0)
	{
		ByteNum = (SumPort/8);
	}
	else
	{
		ByteNum = ((SumPort/8)+1);
	}
	
    OSW_F_SemPend(g_Sem.TFTP_BUF_EVENT, 10, &errNO, TRUE);

    if (ALSTOM_C_NO_ERROR == errNO)
    {
        /*1bytes-协议号，2bytes-消息类型，4bytes-周期号，8byte-时间，
            1byte-主备标志，1byte-监测信息正常采集标识，
            1bytes-机笼号槽道号*/
	*(INT8U*)buf = 0x01;
        *(INT16U*)(buf + 1) = 0x02;
        *(INT32U*)(buf + 3) = g_CycleNO;
		
	MSTEP_F_TimeGet(&Time);	
	*(INT32U*)(buf + 7) = Time.ms;
	*(INT32U*)(buf + 11) = (INT32U)Time.day;

	*(INT8U*)(buf + 15) = 0x03;
	*(INT8U*)(buf + 16) = 0x02;
		

        /*获取机笼号、槽道号*/
        MSTEP_F_ConfigBoardPositionGet(&RackNumber, &SlotNumber);
        *(buf + 17) = (INT8U)((RackNumber << 4) | (SlotNumber));

	/*2byte-消息长度，1bytes-实际用到面板灯个数，1bytes-1个面板灯占用的bit数，
	    1bytes-PANEL_LIGHT字段所占字节数，ByteNum-面板输出灯状态，
	    1byte-采集码位个数，
	    1byte-使用到的驱动码位个数，1byte-每个码位占用的BIT数，
	    1byte-CMD字段所占字节数，ByteNum-输出命令*/
	if (ByteNum==2)
	{
		*(INT16U*)(buf + 18) = 7+2*ByteNum;
			
	        *(INT8U*)(buf + 20) = SumPort;
		*(INT8U*)(buf + 21) = 0x01;
		*(INT8U*)(buf + 22) = ByteNum;
		*(INT16U*)(buf + 23) = ~cmd;
		
		*(INT8U*)(buf + 23+ByteNum) = 0x0u;
		
		*(INT8U*)(buf + 24+ByteNum) = SumPort;
		*(INT8U*)(buf + 25+ByteNum) = 0x01;
		*(INT8U*)(buf + 26+ByteNum) = ByteNum;
		*(INT16U*)(buf + 27+ByteNum) = cmd;
	}
	else if (ByteNum==1)
	{
		*(INT16U*)(buf + 18) = 7+2*ByteNum;
			
	        *(INT8U*)(buf + 20) = SumPort;
		*(INT8U*)(buf + 21) = 0x01;


		*(INT8U*)(buf + 22) = ByteNum;

		*(INT8U*)(buf + 23) = ~cmd;
		
		*(INT8U*)(buf + 23+ByteNum) = 0x0u;

		*(INT8U*)(buf + 24+ByteNum) = SumPort;
		*(INT8U*)(buf + 25+ByteNum) = 0x01;
		*(INT8U*)(buf + 26+ByteNum) = ByteNum;
		*(INT8U*)(buf + 27+ByteNum) = cmd;

	}
	else
	{
		errNO=ALSTOM_C_ERROR;
	}
	
	MessLength=20+7+2*ByteNum;

        /* g_TFTPBUF大小不一定是存入的消息长度的整数倍 */

        if (MessLength <= (INT32U)(GUEST_C_TFTP_BUF_SIZE - WriteOffset))
        {
            STD_F_Memcpy((g_TFTPBUF + WriteOffset), (GUEST_C_TFTP_BUF_SIZE - WriteOffset),
                         buf, MessLength);
            WriteOffset = WriteOffset + MessLength;
            /* 如果已经存满则从头开始存 */

            if (GUEST_C_TFTP_BUF_SIZE == WriteOffset)
            {
                
		g_TFTPBUFFULL = TRUE;
                WriteOffset = 0u;
            }
			
        }
	else
	{
		LeftLength = GUEST_C_TFTP_BUF_SIZE -WriteOffset;
		STD_F_Memcpy((g_TFTPBUF + WriteOffset), (GUEST_C_TFTP_BUF_SIZE - WriteOffset),
                         buf, LeftLength);
                WriteOffset = WriteOffset + LeftLength;
				
		g_TFTPBUFFULL = TRUE;
                WriteOffset = 0u;
		STD_F_Memcpy((g_TFTPBUF + WriteOffset), (GUEST_C_TFTP_BUF_SIZE - WriteOffset),
                 buf+LeftLength, MessLength-LeftLength);
                WriteOffset = WriteOffset + MessLength-LeftLength;
			
	}

    }
    OSW_F_SemPost(g_Sem.TFTP_BUF_EVENT);
    

}
/* 把报警信息写到buffer中，循环存储 */
void GUEST_F_WriteFailCodeToTFTPBUF(INT8U FailCode)
{
    INT8U buf[GUEST_C_TFTP_ERRINFO_LEN];
    INT8U RackNumber = 0u;
    INT8U SlotNumber = 0u;
    MSTEP_T_CanTimeOfDay Time;

    INT8U  errNO = 0;
    STD_F_Memset((void*)buf,0x0u,GUEST_C_TFTP_ERRINFO_LEN);
    OSW_F_SemPend(g_Sem.TFTP_ERRBUF_EVENT, 10, &errNO, TRUE);

    if (ALSTOM_C_NO_ERROR == errNO)
    {
        /*1bytes-协议号，2bytes-消息类型，4bytes-周期号，8byte-时间，
            1byte-主备标志，1byte-监测信息正常采集标识，
            1bytes-机笼号、槽道号，2byte-消息长度*/
	*(INT8U*)buf = 0x01;
        *(INT16U*)(buf + 1) = 0x0F;
        *(INT32U*)(buf+ 3) = g_CycleNO;
		
	MSTEP_F_TimeGet(&Time);	
	*(INT32U*)(buf + 7) = Time.ms;
	*(INT32U*)(buf + 11) = (INT32U)Time.day;

	*(INT8U*)(buf + 15) = 0x03;
	*(INT8U*)(buf + 16) = 0x02;
		

        /*获取机笼号、槽道号*/
        MSTEP_F_ConfigBoardPositionGet(&RackNumber, &SlotNumber);
        *(buf + 17) = (INT8U)((RackNumber << 4) | (SlotNumber));

	*(INT16U*)(buf + 18) = 0x01;
	
	/*1bytes-错误码*/
        *(INT8U*)(buf + 20) = FailCode;

        /* g_TFTPERRBUF大小是存入的消息长度的整数倍 */

        if (GUEST_C_TFTP_ERRINFO_LEN <= (INT32U)(GUEST_C_TFTP_ERRBUF_SIZE - WriteFCodeOffset))
        {
            STD_F_Memcpy((g_TFTPERRBUF + WriteFCodeOffset),
                         (GUEST_C_TFTP_ERRBUF_SIZE - WriteFCodeOffset), buf, GUEST_C_TFTP_ERRINFO_LEN);
            WriteFCodeOffset = WriteFCodeOffset + GUEST_C_TFTP_ERRINFO_LEN;

            /* 如果已经存满则从头开始存 */

            if (GUEST_C_TFTP_ERRBUF_SIZE == WriteFCodeOffset)
            {
                g_TFTPERRBUFFULL = TRUE;
                WriteFCodeOffset = 0u;
            }
        }
    }

    OSW_F_SemPost(g_Sem.TFTP_ERRBUF_EVENT);
}



/* 把报警信息从g_TFTPBUF拷贝到发送的buf中 */
void GUEST_F_ReadDataFromTFTPBUF(INT8U* pBuf)
{

    INT8U  errNO = 0;
    static INT8U g_TFTPBUFtemp[GUEST_C_TFTP_BUF_SIZE] = {0};

    GDF_M_NULL_ASSERT(pBuf);

    OSW_F_SemPend(g_Sem.TFTP_BUF_EVENT, 10, &errNO, TRUE);

    if (ALSTOM_C_NO_ERROR == errNO)
    {
        STD_F_Memset((void *)pBuf, 0U, GUEST_C_TFTP_BUF_SIZE);

        /*WriteOffset 标识不等于0同时buffer写满，说明已经循环存储*/

        if ((WriteOffset != 0) && (g_TFTPBUFFULL == TRUE))
        {
            /*以WriteOffset为分界线把数据的顺序调换一下*/
            STD_F_Memcpy(g_TFTPBUFtemp, GUEST_C_TFTP_BUF_SIZE, g_TFTPBUF, GUEST_C_TFTP_BUF_SIZE);
            /*Copy WriteOffset往后的数据*/
            STD_F_Memcpy(g_TFTPBUF, (GUEST_C_TFTP_BUF_SIZE - WriteOffset),
                         (g_TFTPBUFtemp + WriteOffset), (GUEST_C_TFTP_BUF_SIZE - WriteOffset));
            /*Copy WriteOffset往前的数据*/
            STD_F_Memcpy((g_TFTPBUF + GUEST_C_TFTP_BUF_SIZE - WriteOffset), WriteOffset,
                         g_TFTPBUFtemp, WriteOffset);
        }

        STD_F_Memcpy(pBuf, GUEST_C_TFTP_BUF_SIZE, g_TFTPBUF, GUEST_C_TFTP_BUF_SIZE);

        /*清空buffer*/
        STD_F_Memset(g_TFTPBUF, 0U, GUEST_C_TFTP_BUF_SIZE);
    }

    OSW_F_SemPost(g_Sem.TFTP_BUF_EVENT);

    return;
}
/* 把报警信息从g_TFTPERRBUF拷贝到发送的buf中 */
void GUEST_F_ReadFailCodeFromTFTPBUF(INT8U* fBuf)
{

    INT8U  errNO = 0;
    static INT8U g_TFTPERRBUFtemp[GUEST_C_TFTP_ERRBUF_SIZE] = {0};

    GDF_M_NULL_ASSERT(fBuf);

    OSW_F_SemPend(g_Sem.TFTP_ERRBUF_EVENT, 10, &errNO, TRUE);

    if (ALSTOM_C_NO_ERROR == errNO)
    {
        STD_F_Memset(fBuf, 0U, GUEST_C_TFTP_ERRBUF_SIZE);

        /*WriteOffset 标识不等于0同时buffer写满，说明已经循环存储*/

        if ((WriteFCodeOffset != 0) && (g_TFTPERRBUFFULL == TRUE))
        {
            /*以WriteOffset为分界线把数据的顺序调换一下*/
            STD_F_Memcpy(g_TFTPERRBUFtemp, GUEST_C_TFTP_ERRBUF_SIZE,
                         g_TFTPERRBUF, GUEST_C_TFTP_ERRBUF_SIZE);
            /*Copy WriteOffset往后的数据*/
            STD_F_Memcpy(g_TFTPERRBUF, (GUEST_C_TFTP_ERRBUF_SIZE - WriteFCodeOffset),
                         (g_TFTPERRBUFtemp + WriteFCodeOffset), (GUEST_C_TFTP_ERRBUF_SIZE - WriteFCodeOffset));
            /*Copy WriteOffset往前的数据*/
            STD_F_Memcpy((g_TFTPERRBUF + GUEST_C_TFTP_ERRBUF_SIZE - WriteFCodeOffset),
                         WriteFCodeOffset, g_TFTPERRBUFtemp, WriteFCodeOffset);
        }

        STD_F_Memcpy(fBuf, GUEST_C_TFTP_ERRBUF_SIZE, g_TFTPERRBUF, GUEST_C_TFTP_ERRBUF_SIZE);

        /*清空buffer*/
        STD_F_Memset(g_TFTPERRBUF, 0U, GUEST_C_TFTP_ERRBUF_SIZE);
    }

    OSW_F_SemPost(g_Sem.TFTP_ERRBUF_EVENT);

    return;
}


/* 把握手包信息从g_TFTPHANDBUF拷贝到发送的buf中 */
void GUEST_F_ReadDataFromTFTPHandBUF(INT8U* pBuf)
{

    INT8U  errNO = 0;

    GDF_M_NULL_ASSERT(pBuf);

    OSW_F_SemPend(g_Sem.TFTP_TESTBUF_EVENT, 10, &errNO, TRUE);

    if (ALSTOM_C_NO_ERROR == errNO)
    {
        STD_F_Memset((void *)pBuf, 0U, GUEST_C_TFTP_HAND_BUF_SIZE);
        STD_F_Memcpy((void *)pBuf, GUEST_C_TFTP_HAND_BUF_SIZE, (void *)g_TFTPBUF_HAND, GUEST_C_TFTP_HAND_BUF_SIZE);
        STD_F_Memset((void *)g_TFTPBUF_HAND, 0U, GUEST_C_TFTP_HAND_BUF_SIZE);
    }

    OSW_F_SemPost(g_Sem.TFTP_TESTBUF_EVENT);

    return;
}


/*读取两个CPU 的代码版本、软件CRC、FPGA版本号*/
void GUEST_F_GetDataInOneCPU(void)
{
#ifdef ALSTOM_CONFIG_CPU_A
    g_Usr_Cfg_A.SwVer1 = GUEST_C_SW_VERSION_1;
    g_Usr_Cfg_A.SwVer2 = GUEST_C_SW_VERSION_2;
    g_Usr_Cfg_A.SwCRC = MSTEP_F_GetSoftwareDataID();
    GUEST_M_FPGA_Read(GUEST_C_FPGA_VERSION_ADDR, g_Usr_Cfg_A.FPGAVer);
    MSTEP_F_K2oo2CpuExchange((INT8U *)&g_Usr_Cfg_B,
                             (INT8U *)&g_Usr_Cfg_A,
                             sizeof(g_Usr_Cfg_A),
                             GUEST_C_2oo2_TIMEOUT);
#endif
#ifdef ALSTOM_CONFIG_CPU_B
    g_Usr_Cfg_B.SwVer1 = GUEST_C_SW_VERSION_1;
    g_Usr_Cfg_B.SwVer2 = GUEST_C_SW_VERSION_2;
    g_Usr_Cfg_B.SwCRC = MSTEP_F_GetSoftwareDataID();
    GUEST_M_FPGA_Read(GUEST_C_FPGA_VERSION_ADDR, g_Usr_Cfg_B.FPGAVer);
    MSTEP_F_K2oo2CpuExchange((INT8U *)&g_Usr_Cfg_A,
                             (INT8U *)&g_Usr_Cfg_B,
                             sizeof(g_Usr_Cfg_B),
                             GUEST_C_2oo2_TIMEOUT);
#endif
}/**/
/* 把版本信息写到buffer中*/
void GUEST_F_SendVerData(INT8U* VerBuf)
{
    INT8U RackNumber = 0u;
    INT8U SlotNumber = 0u;
    MSTEP_T_CanTimeOfDay Time;

    GDF_M_NULL_ASSERT(VerBuf);
	/*1bytes-协议号，2bytes-消息类型，4bytes-周期号，8byte-时间，
            1byte-主备标志，1byte-监测信息正常采集标识，
            1bytes-机笼号、槽道号，2byte-消息长度*/
	*(INT8U*)VerBuf = 0x01;
        *(INT16U*)(VerBuf + 1) = 0x01;
        *(INT32U*)(VerBuf+ 3) = g_CycleNO;
		
	MSTEP_F_TimeGet(&Time);	
	*(INT32U*)(VerBuf + 7) = Time.ms;
	*(INT32U*)(VerBuf + 11) = (INT32U)Time.day;

	*(INT8U*)(VerBuf + 15) = 0x03;
	*(INT8U*)(VerBuf + 16) = 0x02;
		

        /*获取机笼号、槽道号*/
        MSTEP_F_ConfigBoardPositionGet(&RackNumber, &SlotNumber);
        *(VerBuf + 17) = (INT8U)((RackNumber << 4) | (SlotNumber));

	*(INT16U*)(VerBuf + 18) = 28;
		
    /*8bytes-软件版本，4bytes-软件ID，2bytes-FPGA版本*/
    *(INT32U*)(VerBuf + 20)  = g_Usr_Cfg_A.SwVer1;
    *(INT32U*)(VerBuf + 24) = g_Usr_Cfg_A.SwVer2;
    *(INT32U*)(VerBuf + 28) = g_Usr_Cfg_B.SwVer1;
    *(INT32U*)(VerBuf + 32) = g_Usr_Cfg_B.SwVer2;
    *(INT32U*)(VerBuf + 36) = g_Usr_Cfg_A.SwCRC;
    *(INT32U*)(VerBuf + 40) = g_Usr_Cfg_B.SwCRC;
    *(INT16U*)(VerBuf + 44) = g_Usr_Cfg_A.FPGAVer;
    *(INT16U*)(VerBuf + 46) = g_Usr_Cfg_B.FPGAVer;

    return;

}


/* 检查内存，该函数已保证了两CPU返回状态的一致性 */
void GUEST_F_CheckBuf(INT8U* buffer, INT16U len)
{
    ALSTOM_T_Status Status = ALSTOM_C_NO_ERROR;

    INT16U i = 0u;

    GDF_M_NULL_ASSERT(buffer);


    do
    {
        for (i = 0u;i < len;++i)
        {
            *(buffer + i) = 0x5A;
        }

        for (i = 0u;i < len;++i)
        {
            if (0x5A != *(buffer + i))
            {
                DTXT_REPORT3(DTXT_REPORT_INFO, "CheckBuf Error", (INT32U)(buffer + i), 0x5A, *(buffer + i));

                Status = ALSTOM_C_ERROR;
                break;
            }
        }

        if (ALSTOM_C_ERROR == Status)
        {
            break;
        }

        for (i = 0u;i < len;++i)
        {
            *(buffer + i) = 0xA5;
        }

        for (i = 0u;i < len;++i)
        {
            if (0xA5 != *(buffer + i))
            {
                DTXT_REPORT3(DTXT_REPORT_INFO, "CheckBuf Error", (INT32U)(buffer + i), 0xA5, *(buffer + i));

                Status = ALSTOM_C_ERROR;
                break;
            }

            *(buffer + i) = 0x00;
        }

        if (ALSTOM_C_ERROR == Status)
        {
            break;
        }
    }
    while (0);

    return;
}

void GUEST_F_TFTP_SendTestInfo(INT16U cmdtype, INT16U datalen, INT16U cmd, INT8U Status)
{
    INT8U buf[GUEST_C_TFTP_INFO_LEN];
    INT8U RackNumber = 0u;
    INT8U SlotNumber = 0u;

    INT8U  errNO = 0;
    STD_F_Memset((void*)buf,0x0u,GUEST_C_TFTP_INFO_LEN);
	
    OSW_F_SemPend(g_Sem.TFTP_BUF_EVENT, 10, &errNO, TRUE);

    if (ALSTOM_C_NO_ERROR == errNO)
    {
        /*2bytes-消息类型，2bytes-数据长度，4bytes-周期号，1bytes-机笼号、槽道号，2bytes-输出命令，1bytes-测试结果*/
        *(INT16U*)buf = cmdtype;
        *(INT16U*)(buf + 2) = datalen;
        *(INT32U*)(buf + 4) = g_CycleNO;

        /*获取机笼号、槽道号*/
        MSTEP_F_ConfigBoardPositionGet(&RackNumber, &SlotNumber);
        *(buf + 8) = (INT8U)((RackNumber << 4) | (SlotNumber));
        *(INT16U*)(buf + 9) = cmd;
        *(INT8U*)(buf + 11) = Status;

        /*根据测试需要加入信息*/

        if (cmd == TFTP_MSG_ISO)
        {
            *(INT8U*)(buf + 12) = ISOTestStep;
        }
        else if (cmd == TFTP_MSG_FUSE)
        {
            *(INT8U*)(buf + 12) = FUSETestStep;
        }
        else if (cmd == TFTP_MSG_RITTEST)
        {
            *(buf + 12) = (INT8U)((RITestPortID << 4) | (RITestStep));
        }
        else
        {
            *(INT8U*)(buf + 12) = 0x00;
        }

        /* g_TFTPBUF大小是存入的消息长度的整数倍 */

        if (GUEST_C_TFTP_INFO_LEN <= (GUEST_C_TFTP_BUF_SIZE - WriteOffset))
        {
            STD_F_Memcpy(g_TFTPBUF + WriteOffset, GUEST_C_TFTP_BUF_SIZE - WriteOffset, buf, GUEST_C_TFTP_INFO_LEN);
            WriteOffset = WriteOffset + GUEST_C_TFTP_INFO_LEN;
            /* 如果已经存满则从头开始存 */

            if (GUEST_C_TFTP_BUF_SIZE == WriteOffset)
            {
                WriteOffset = 0u;
                g_TFTPBUFFULL = TRUE;
            }
        }
    }
    else
    {
        DTXT_REPORT(DTXT_REPORT_INFO, "GUEST_F_WriteDataToTFTPBUF Get Sem Error");
    }

    OSW_F_SemPost(g_Sem.TFTP_BUF_EVENT);
}

void GUEST_F_TFTP_SendHandInfo(INT16U cmdtype, INT16U cmd)
{
    INT8U buf[GUEST_C_TFTP_HAND_LEN] = {0};
    INT8U RackNumber = 0u;
    INT8U SlotNumber = 0u;

    INT8U  errNO = 0;

    OSW_F_SemPend(g_Sem.TFTP_TESTBUF_EVENT, 10, &errNO, TRUE);

    if (ALSTOM_C_NO_ERROR == errNO)
    {
        /*2bytes-消息类型，2bytes-数据长度，4bytes-周期号，1bytes-机笼号、槽道号，2bytes-输出命令*/
        *(INT16U*)buf = cmdtype;
        *(INT16U*)(buf + 2) = 0x02;
        *(INT32U*)(buf + 4) = g_CycleNO;

        /*获取机笼号、槽道号*/
        MSTEP_F_ConfigBoardPositionGet(&RackNumber, &SlotNumber);
        *(buf + 8) = (INT8U)((RackNumber << 4) | (SlotNumber));
        *(INT16U*)(buf + 9) = cmd;

        STD_F_Memcpy(g_TFTPBUF_HAND, GUEST_C_TFTP_HAND_BUF_SIZE, buf, GUEST_C_TFTP_HAND_LEN);
    }
    else
    {
        DTXT_REPORT(DTXT_REPORT_INFO, "GUEST_F_WriteDataToTFTPBUF Get Sem Error");
    }

    OSW_F_SemPost(g_Sem.TFTP_TESTBUF_EVENT);
}

/* Enable the EXT Light signal */
void GUEST_F_EXTLightSetOn(void)
{
#ifdef ALSTOM_CONFIG_CPU_A
    MSTEP_F_GpioSetOn(MSTEPINC_C_GPIO_ID14);
#endif

#ifdef ALSTOM_CONFIG_CPU_B
    MSTEP_F_GpioSetOn(MSTEPINC_C_GPIO_ID13);
#endif
}

/* Disable the EXT Light signal */
void GUEST_F_EXTLightSetOff(void)
{
#ifdef ALSTOM_CONFIG_CPU_A
    MSTEP_F_GpioSetOff(MSTEPINC_C_GPIO_ID14);
#endif

#ifdef ALSTOM_CONFIG_CPU_B
    MSTEP_F_GpioSetOff(MSTEPINC_C_GPIO_ID13);
#endif
}

void GUEST_F_ResFPGASetOff(void)
{

    /*把FPGA信号拉低*/
#ifdef ALSTOM_CONFIG_CPU_A
    MSTEP_F_GpioSetOff(MSTEPINC_C_GPIO_ID11);
#endif

#ifdef ALSTOM_CONFIG_CPU_B
    MSTEP_F_GpioSetOff(MSTEPINC_C_GPIO_ID5);
#endif

}

void GUEST_F_ResFPGASetOn(void)
{

    /*把FPGA信号拉高*/
#ifdef ALSTOM_CONFIG_CPU_A
    MSTEP_F_GpioSetOn(MSTEPINC_C_GPIO_ID11);
#endif

#ifdef ALSTOM_CONFIG_CPU_B
    MSTEP_F_GpioSetOn(MSTEPINC_C_GPIO_ID5);
#endif

}

/* FEED WATCHING DOG */
void GUEST_F_FeedWDog(void)
{
    static BOOLEAN Feedmode = FALSE;

    if (FALSE == Feedmode)
    {
        GUEST_M_FPGA_Write(GUEST_C_FUSE_BREAK_ADDR, GUEST_C_FEED_WDOG_CMD1);
        Feedmode = TRUE;
    }
    else
    {
        GUEST_M_FPGA_Write(GUEST_C_FUSE_BREAK_ADDR, GUEST_C_FEED_WDOG_CMD2);
        Feedmode = FALSE;
    }

}

/*INT32U GUEST_F_Sqrt(INT32U value)
{
    INT32U rem = 0;
    INT32U root = 0;
    INT32U divisor = 0;
    INT32U i = 0;

    for(i=0; i<16; i++)
    {
        root <<=1;

        rem = (rem << 2) + (value >> 30);

        value <<=2;

        divisor = (root << 1) + 1;

        if(divisor <= rem)
        {
            rem -= divisor;
            root++;
        }
    }

    return root;
}*/

void GUEST_F_GetSnake(void)
{

    INT32U i;
    INT32U j;
    INT16U pid;
    INT16U temp;

    /*Single*/
    for (i = 0, j = 0; i < 16; i++)
    {
        if (i < 8)
        {
            pid = 2 * j + 1;
            ++j;
        }
        else
        {
            --j;
            pid = 2 * j;
        }

        Snakebody[0][i] = (INT16U)(1 << pid);
    }

    temp = Snakebody[0][15];

    for (i = 15; i > 0; i--)
    {
        Snakebody[0][i] = Snakebody[0][i-1];
    }

    Snakebody[0][0] = temp;

    for (j = 1; j < 16; j++)
    {
        for (i = 0; i < 16; i++)
        {
            Snakebody[j][i] = Snakebody[j-1][i] | Snakebody[0][(i+j)%16];
        }
    }

    return;
}/**/



void GUEST_F_LED_Snake(void)
{

    INT16U CPUCMD = 0x00u;

    static INT32U Col = 0;
    static INT32U Row = 0;

    /*if (g_LastTest != TFTP_MSG_LEDTEST)
    {
        Col = 0;
        Row = 0;
    }*/

    /*Output LED*/

    CPUCMD = Snakebody[Col][Row];

    GUEST_F_OutputLED(~CPUCMD, (GUEST_E_LED_Mode)GUEST_C_LED_OPEN);

    Row++;

    if (Row == 16)
    {
        Col ++;
        Row = 0;
    }

    if (Col == 16)
    {
        Col = 0;
    }

}


/* 从IODB中读取驱动命令 */
void GUEST_F_IODB_GetCmd_VOOB(INT16U* pOutputs)
{

    INT32U i = 0u;

    INT16U outputs = 0u;

    MSTEP_T_IodbData l_readIOdBData[GUEST_C_OUTPUT_PORT_NUM];

    GDF_M_NULL_ASSERT(pOutputs);

    do
    {

        STD_F_Memset(l_readIOdBData, 0, sizeof(l_readIOdBData));

        *pOutputs = GUEST_C_RESTRICTIVE_CMD;

        for (i = 0; i < GUEST_C_OUTPUT_PORT_NUM; ++i)
        {
            l_readIOdBData[i].dataId = i + 1;
        }

        (void)MSTEP_F_IodbRequest(GUEST_C_OUTPUT_PORT_NUM, l_readIOdBData);

        /* Read data from IODB */

        for (i = 0; i < GUEST_C_OUTPUT_PORT_NUM; ++i)
        {
            outputs |= (l_readIOdBData[i].value << i);
        }

        outputs = (INT16U)(~outputs);

        *pOutputs = outputs;
    }
    while (0);

    return;
}


/* 从IODB中读取驱动命令 */
void GUEST_F_IODB_GetCmd_CDDM(INT16U* pOutputs)
{

    INT32U i = 0u;

    INT16U outputs = 0u;
    INT16U outputsFB = 0u;
    INT16U ByteDIV = 0u;

    MSTEP_T_IodbData l_readIOdBData[GUEST_C_CDDM_IN_PORT_NUM];

    GDF_M_NULL_ASSERT(pOutputs);

    do
    {

        STD_F_Memset(l_readIOdBData, 0, sizeof(l_readIOdBData));

        *pOutputs = GUEST_C_RESTRICTIVE_CMD;

        for (i = 0; i < GUEST_C_CDDM_IN_PORT_NUM; ++i)
        {
            l_readIOdBData[i].dataId = i + 1;
        }

        (void)MSTEP_F_IodbRequest(GUEST_C_CDDM_IN_PORT_NUM, l_readIOdBData);

        /* Read data from IODB */

        for (i = 0; i < GUEST_C_CDDM_IN_PORT_NUM; ++i)
        {
            outputs |= (l_readIOdBData[i].value << i);
        }

        /*检查编码部分(低8bit)的命令，最多只能有一个允许码位*/
        /*若超过一个允许码位按照限制态输出*/
        outputsFB = outputs & 0x00FF;

        while (0u != outputsFB)
        {
            ++ByteDIV;
            outputsFB = outputsFB & (outputsFB - 1u);
        }

        if (ByteDIV > 1u)
        {
            outputs = outputs & 0xFF00u;
        }

        outputs = (INT16U)(~outputs);

        *pOutputs = outputs;
    }
    while (0);


    return;
}



/* 从IODB中读取驱动命令 */
void GUEST_F_IODB_GetCmd_PDDM(INT16U* pOutputs)
{

    INT32U i = 0u;

    INT16U outputs = 0u;

    MSTEP_T_IodbData l_readIOdBData[GUEST_C_PDDM_IN_PORT_NUM];

    GDF_M_NULL_ASSERT(pOutputs);

    do
    {

        STD_F_Memset(l_readIOdBData, 0, sizeof(l_readIOdBData));

        *pOutputs = GUEST_C_RESTRICTIVE_CMD;

        for (i = 0; i < GUEST_C_PDDM_IN_PORT_NUM; ++i)
        {
            l_readIOdBData[i].dataId = i + 1;
        }

        (void)MSTEP_F_IodbRequest(GUEST_C_PDDM_IN_PORT_NUM, l_readIOdBData);

        /* Read data from IODB */

        for (i = 0; i < GUEST_C_PDDM_IN_PORT_NUM; ++i)
        {
            outputs |= (l_readIOdBData[i].value << i);
        }

        *pOutputs = outputs;
    }
    while (0);

    return;
}




/* 从IODB中读取驱动命令 */
void GUEST_F_IODB_GetCmd_SDDM(INT16U* pOutputs, GUEST_E_LED_Mode* mode)
{

    INT32U i = 0u;

    INT16U outputs = 0u;

    GUEST_E_LED_Mode Drvmode[6] = {GUEST_C_LED_CLOSE};

    MSTEP_T_IodbData l_readIOdBData[GUEST_C_SDDM_IN_PORT_NUM];

    GDF_M_NULL_ASSERT(pOutputs);

    do
    {

        STD_F_Memset(l_readIOdBData, 0, sizeof(l_readIOdBData));

        *pOutputs = GUEST_C_RESTRICTIVE_CMD;
        *mode = GUEST_C_LED_CLOSE;

        for (i = 0; i < GUEST_C_SDDM_IN_PORT_NUM; ++i)
        {
            l_readIOdBData[i].dataId = i + 1;
        }

        (void)MSTEP_F_IodbRequest(GUEST_C_SDDM_IN_PORT_NUM, l_readIOdBData);

        /* Read data from IODB */
        for (i = 0; i < (GUEST_C_SDDM_IN_PORT_NUM/3); ++i)
        {
            
            if((l_readIOdBData[3*i].value == 1)&&(l_readIOdBData[3*i+1].value == 0))
            {
                Drvmode[i] = GUEST_C_LED_OPEN;
            }
            else if((l_readIOdBData[3*i].value == 1)&&(l_readIOdBData[3*i+1].value == 1))
            {
                Drvmode[i] = GUEST_C_LED_FLASH;
            }
            else
            {
                Drvmode[i] = GUEST_C_LED_CLOSE;
            }

            outputs |= (l_readIOdBData[3*i].value << i);

            mode[i] = Drvmode[i];
        }

        outputs = (INT16U)(~outputs);

        *pOutputs = outputs;

    }
    while (0);

    return;
}



/* 从IODB中读取驱动命令 */
void GUEST_F_IODB_DrvCmd_TCIM(void)
{

    INT32U i = 0u;
    INT16U outputs = 0u;
    ALSTOM_T_Status Result = ALSTOM_C_NO_ERROR;

    INT32U StatusCRC = 0u;
    INT32U RMTStatusCRC = 0u;

    MSTEP_T_IodbData l_writeIOdBData[GUEST_C_TCIM_PORT_NUM];

    for (i = 0; i < GUEST_C_TCIM_PORT_NUM; ++i)
    {
        l_writeIOdBData[i].dataId = i + 1;
        l_writeIOdBData[i].value = 0;
    }

    if ((TRACKSIDE_POWER_MISSING == FALSE) && (TRACKSIDE_POWER_OVERAGE == FALSE))
    {
        Result = ALSTOM_C_NO_ERROR;
    }
    else
    {
        Result = ALSTOM_C_ERROR;
    }

    MSTEP_F_CryptlibCRC32FSFB2ChanOneCompute((INT8U *)&Result, sizeof(ALSTOM_T_Status), &StatusCRC);

    MSTEP_F_K2oo2CpuExchange((INT8U *)&RMTStatusCRC, (INT8U *)&StatusCRC, sizeof(StatusCRC), GUEST_C_2oo2_TIMEOUT);

    if (RMTStatusCRC != StatusCRC)
    {
        Result = ALSTOM_C_ERROR;
    }

    if (Result == ALSTOM_C_NO_ERROR)
    {
        for (i = 0; i < (GUEST_C_TCIM_PORT_NUM/2); ++i)
        {
            l_writeIOdBData[2*i].value = 1;
        }
    }
    else
    {
        for (i = 0; i < (GUEST_C_TCIM_PORT_NUM/2); ++i)
        {
            l_writeIOdBData[2*i+1].value = 1;
        }
    }

    for (i = 0; i < GUEST_C_TCIM_PORT_NUM; ++i)
    {
        outputs |= (l_writeIOdBData[i].value << i);
    }

    GUEST_F_OutputLED(~outputs, GUEST_C_LED_OPEN);

    MSTEP_F_IodbSubmit(GUEST_C_TCIM_PORT_NUM, l_writeIOdBData);

    return;
}


void GUEST_F_SendFBJState()
{

    ALSTOM_T_Status Result = ALSTOM_C_NO_ERROR;

    INT32U StatusCRC = 0u;
    INT32U RMTStatusCRC = 0u;

    MSTEP_T_IodbData l_writeIOdBData[GUEST_C_CDDM_OUT_PORT_NUM];

    l_writeIOdBData[0].dataId = 1u;
    l_writeIOdBData[0].value  = 0u;

    if ((TRACKSIDE_POWER_MISSING == FALSE) && (TRACKSIDE_POWER_OVERAGE == FALSE))
    {
        Result = ALSTOM_C_NO_ERROR;
    }
    else
    {
        Result = ALSTOM_C_ERROR;
    }

    MSTEP_F_CryptlibCRC32FSFB2ChanOneCompute((INT8U *)&Result, sizeof(ALSTOM_T_Status), &StatusCRC);

    MSTEP_F_K2oo2CpuExchange((INT8U *)&RMTStatusCRC, (INT8U *)&StatusCRC, sizeof(StatusCRC), GUEST_C_2oo2_TIMEOUT);

    if (RMTStatusCRC != StatusCRC)
    {
        Result = ALSTOM_C_ERROR;
    }

    if (Result == ALSTOM_C_NO_ERROR)
    {
        l_writeIOdBData[0].value  = 0x1u;
    }
    else
    {
        l_writeIOdBData[0].value  = 0x0u;
    }

    MSTEP_F_IodbSubmit(GUEST_C_CDDM_OUT_PORT_NUM, l_writeIOdBData);

    return;
}

void GUEST_F_RBPointStatus(INT16U DriveCommand)
{
    MSTEP_T_IodbData l_writeIOdBData[GUEST_C_PDDM_OUT_PORT_NUM]; /*IODB的数据结构,发送给IODB*/

    INT16U outputs = 0u;

    INT32U StatusCRC = 0u;
    INT32U RMTStatusCRC = 0u;

    /*SWITCH_NORMAL=定操;SWITCH_REVERSE=反操;SWITCH_NONE=无操作*/
    INT8U POINT_ACTION_MODE = SWITCH_NONE;

    l_writeIOdBData[0].dataId = 1u;
    l_writeIOdBData[1].dataId = 2u;

    l_writeIOdBData[0].value  = 0u;
    l_writeIOdBData[1].value  = 0u;

    /*处理道岔命令*/

    if (DriveCommand == 0x03u)
    {
        /*操作动作置为定操(SWITCH_NORMAL)*/
        POINT_ACTION_MODE =  SWITCH_NORMAL;
        outputs = GUEST_C_PERMISSIVE_CMD | 0x01;
    }
    else if (DriveCommand == 0x05u)
    {
        /*操作动作置为反操(SWITCH_NORMAL)*/
        POINT_ACTION_MODE = SWITCH_REVERSE;
        outputs = GUEST_C_PERMISSIVE_CMD | 0x03;
    }
    else/*Other command / No command*/
    {
        /*非操作命令*/
        POINT_ACTION_MODE = SWITCH_NONE;
        outputs = GUEST_C_PERMISSIVE_CMD;
	GUEST_F_WriteFailCodeToTFTPBUF((INT8U)GUEST_C_FC_APPROACHLIFE);
    }


    MSTEP_F_CryptlibCRC32FSFB2ChanOneCompute((INT8U *)&POINT_ACTION_MODE, sizeof(INT8U), &StatusCRC);

    MSTEP_F_K2oo2CpuExchange((INT8U *)&RMTStatusCRC, (INT8U *)&StatusCRC, sizeof(StatusCRC), GUEST_C_2oo2_TIMEOUT);

    if (RMTStatusCRC != StatusCRC)
    {
        POINT_ACTION_MODE = SWITCH_NONE;
    }

    /*驱动输出*/
    GUEST_F_DriveOutput(~outputs, TRUE, GUEST_C_LED_OPEN);

    /*generate the IODB data according to POINT_STATUS, and upload to IPS and SDM.*/
    switch (POINT_ACTION_MODE)
    {

        case SWITCH_NORMAL:
            l_writeIOdBData[0].value = 0x1;
            l_writeIOdBData[1].value = 0x0;
            break;

        case SWITCH_REVERSE:
            l_writeIOdBData[0].value = 0x0;
            l_writeIOdBData[1].value = 0x1;
            break;

        case SWITCH_NONE:
            l_writeIOdBData[0].value = 0x0;
            l_writeIOdBData[1].value = 0x0;
            break;

        default:
            break;
    }

    MSTEP_F_IodbSubmit(GUEST_C_PDDM_OUT_PORT_NUM, l_writeIOdBData);

    return;
}

/* EOF */
