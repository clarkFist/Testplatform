#pragma SCVR_CHECK_OFF 05
#pragma SCVR_CHECK_OFF 06
#pragma SCVR_CHECK_COMMENT "This is a guest module for code example, the header file naming is not important"
#pragma SCVR_CHECK_ALIAS "guest"

/**
* @file guest.c
* @brief
* @author Wang Jiantao
* @date 2013-6-2
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
#include "guest_fuse.h"

#include "dtxt.h"
#define TB_SEND_NUM   32   //32
#define TB_RECV_NUM   128    //128


INT32U TB_Receive_Data[4] = {0,0,0,0};
INT32U Receive_Data = 0u;
MSTEP_T_IodbData TB_readIOdBData[TB_RECV_NUM];
MSTEP_T_IodbData TB_writeIOdBData[TB_SEND_NUM];

#pragma PRQA_MESSAGES_OFF 3218
#pragma PRQA_MESSAGES_COMMENT "Variable only used in one function but defined at the top of the file to increase readability"
/*FCT open GPIO A14 B13*/
#ifdef ALSTOM_CONFIG_CPU_A
static const MSTEPINC_T_GpioDefinition
GUEST_C_GPIODefinition[MSTEPINC_C_GPIO_NB_MAX] =
{
    {MSTEPINC_C_GPIO_NO_ID, MSTEPINC_C_GPIO_UNUSED, MSTEPINC_C_GPIO_DONT_CARE},
    {MSTEPINC_C_GPIO_ID1,  MSTEPINC_C_GPIO_OUTPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID2,  MSTEPINC_C_GPIO_INPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID3,  MSTEPINC_C_GPIO_INPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID4,  MSTEPINC_C_GPIO_OUTPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID5,  MSTEPINC_C_GPIO_INPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID6,  MSTEPINC_C_GPIO_INPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID7,  MSTEPINC_C_GPIO_INPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID8,  MSTEPINC_C_GPIO_INPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID9,  MSTEPINC_C_GPIO_INPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID10, MSTEPINC_C_GPIO_OUTPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID11, MSTEPINC_C_GPIO_OUTPUT, MSTEPINC_C_GPIO_ON},
    {MSTEPINC_C_GPIO_ID12, MSTEPINC_C_GPIO_OUTPUT, MSTEPINC_C_GPIO_ON},
    {MSTEPINC_C_GPIO_ID13, MSTEPINC_C_GPIO_OUTPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID14, MSTEPINC_C_GPIO_OUTPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID15, MSTEPINC_C_GPIO_OUTPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID16, MSTEPINC_C_GPIO_OUTPUT, MSTEPINC_C_GPIO_OFF}
};
#endif

#ifdef ALSTOM_CONFIG_CPU_B
static const MSTEPINC_T_GpioDefinition
GUEST_C_GPIODefinition[MSTEPINC_C_GPIO_NB_MAX] =
{
    {MSTEPINC_C_GPIO_NO_ID, MSTEPINC_C_GPIO_UNUSED, MSTEPINC_C_GPIO_DONT_CARE},
    {MSTEPINC_C_GPIO_ID1,  MSTEPINC_C_GPIO_OUTPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID2,  MSTEPINC_C_GPIO_INPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID3,  MSTEPINC_C_GPIO_INPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID4,  MSTEPINC_C_GPIO_OUTPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID5,  MSTEPINC_C_GPIO_OUTPUT, MSTEPINC_C_GPIO_ON},
    {MSTEPINC_C_GPIO_ID6,  MSTEPINC_C_GPIO_INPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID7,  MSTEPINC_C_GPIO_INPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID8,  MSTEPINC_C_GPIO_INPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID9,  MSTEPINC_C_GPIO_INPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID10, MSTEPINC_C_GPIO_OUTPUT, MSTEPINC_C_GPIO_ON},
    {MSTEPINC_C_GPIO_ID11, MSTEPINC_C_GPIO_INPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID12, MSTEPINC_C_GPIO_OUTPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID13, MSTEPINC_C_GPIO_OUTPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID14, MSTEPINC_C_GPIO_OUTPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID15, MSTEPINC_C_GPIO_OUTPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID16, MSTEPINC_C_GPIO_OUTPUT, MSTEPINC_C_GPIO_OFF}
};
#endif

#pragma PRQA_MESSAGES_ON 3218

/** Definition of the configuration to apply on chip select available on MSTEP
* board.
* Default configuration is valid: MSTEPINC_C_CS_INVALID, baseAddr: 0, SRWS: 0,
* IWS: 15, AA: 1, PS: 0, BEM: 0, BSTR: 0, BSTW: 0, SWWS: 0*/
#pragma PRQA_MESSAGES_OFF 3218
#pragma PRQA_MESSAGES_COMMENT "Variable only used in one function but defined at the top of the file to increase readability"
static const MSTEPINC_T_ChipSelectDefinition
GUEST_C_ChipSelectParam[MSTEPINC_C_CS_NB_MAX] =
{
    /* Address mask, SRWS, IWS, AA, PS, BEM, BSTR, BSTW, SWWS*/
    {MSTEPINC_C_CS_NO_ID, MSTEPINC_C_CS_INVALID, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    /*{MSTEPINC_C_CS_ID1, MSTEPINC_C_CS_VALID, 0, 0, 15, 0, 2, 0, 0, 0, 0},*/
    {MSTEPINC_C_CS_ID1, MSTEPINC_C_CS_VALID, 0, 0, 15, 0, 2, 0, 0, 0, 0},
    {MSTEPINC_C_CS_ID2, MSTEPINC_C_CS_INVALID, 0, 0, 15, 1, 0, 0, 0, 0, 0}
};

#pragma PRQA_MESSAGES_ON 3218

/** Definition of the board type */
#pragma PRQA_MESSAGES_OFF 3218
#pragma PRQA_MESSAGES_COMMENT "Variable only used in one function but defined at the top of the file to increase readability"
static const MSTEPINC_E_BoardMode GUEST_C_BoardMode = MSTEPINC_C_GUEST_NO_ETH;
#pragma PRQA_MESSAGES_ON 3218

/** Network LED configuration */
#pragma PRQA_MESSAGES_OFF 3218
#pragma PRQA_MESSAGES_COMMENT "Variable only used in one function but defined at the top of the file to increase readability"

#ifdef ALSTOM_CONFIG_CPU_A
static const MSTEPINC_T_NetworkLed GUEST_C_NetworkLed =
{
    MSTEPINC_C_GPIO_ID13,  /**< CAN */
    MSTEPINC_C_GPIO_ID14   /**< OutputPort */

};
#endif

#ifdef ALSTOM_CONFIG_CPU_B
static const MSTEPINC_T_NetworkLed GUEST_C_NetworkLed =
{
    MSTEPINC_C_GPIO_ID12,  /**< CAN */
    MSTEPINC_C_GPIO_ID13   /**< OutputPort */

};
#endif

/* CR_reference 2153 - Application Software Version. */
static INT8U GUEST_C_ApplicationSwVersion[APPLICATION_SW_VERSION_MAX_SIZE];

#pragma PRQA_MESSAGES_ON 3218

/** Maintenance datas MSTEPINC_C_MAINTENANCE_ENTRY_COUNT pooled by maintenance Stepper :
* - Mimnimum baud Rate updated via GUEST_MBUSLIB_F_GetMinRate in GUEST_MBUS4_F_MainLoop
* - CPU usage Max updated in GUEST_TEST_F_MainLoop */
#pragma PRQA_MESSAGES_OFF 3218
#pragma PRQA_MESSAGES_COMMENT "Variable only used in one function but defined at the top of the file to increase readability"

#pragma PRQA_MESSAGES_ON 3218

/******************************************************************************
* MACROS DEFINITION
*****************************************************************************/
/** Stack size for the Guest task*/
#define GUEST_C_STACK_SIZE   1500u


/** Number of extra sectors needed for application data storage. Set to max value = 8 sectors */
#define GUEST_C_CONF_APPLI_SECTOR_NUMBER (8u)
/** Size of memory application for configuration */
#define GUEST_C_CONF_APPLI_MEMORY_SIZE (GUEST_C_CONF_APPLI_SECTOR_NUMBER * GUEST_C_CONF_SECTOR_SIZE)

/** Number of sectors needed for parameters data storage. Enable maximum */
#define GUEST_C_PARAM_APPLI_SECTOR_NUMBER (MSTEPINC_C_MAX_PARAM_SECTORS)
/** Size of memory application for parameters */
#define GUEST_C_PARAM_APPLI_MEMORY_SIZE (GUEST_C_PARAM_APPLI_SECTOR_NUMBER * GUEST_C_CONF_SECTOR_SIZE)

/** Size of the memory area used to store safety constants computed during
*  initialization and checked during CBIT */
#define GUEST_C_DCA_MEMORY_SIZE (GUEST_C_DCA_KERNEL_GUEST_MEMORY_SIZE + GUEST_C_CONF_APPLI_MEMORY_SIZE + (100u * 1024u))

/** Default value for Reprog Activation Pattern to disabled the reprog feature */
#ifdef ALSTOM_CONFIG_CPU_A
#define GUEST_C_REPROG_DISABLED 0x4F8B0E65u
#else
#define GUEST_C_REPROG_DISABLED 0x4F5F4096u
#endif
/******************************************************************************
*  TYPE DEFINITION
*****************************************************************************/
typedef enum
{
    /** Unused command */
    GUEST_TFTP_C_CMD_UNUSED,
    /** Write command */
    GUEST_TFTP_C_CMD_WRITE_CMD,
    /** Write command */
    GUEST_TFTP_C_CMD_WRITE_SN,
    /** Read command */
    GUEST_TFTP_C_CMD_READ,
    /** Read Version command */
    GUEST_TFTP_C_CMD_READ_VER,
    /** Read Version command */
    GUEST_TFTP_C_CMD_READ_HAND,
    
    GUEST_TFTP_C_CMD_READ_FCODE,
} GUEST_TFTP_E_ConnectionId;

/* Control block of Fault Filtering Mechnism */
typedef struct
{

    /** Fault Filtering feature activation */
    BOOLEAN                     Activation;

    /** Filtering Timeout */
    INT32U                      FilteringTout;
    
    /** Tempo for Fault Filtering timer */
    OSW_T_Tempo                 FF_Tempo;

    /** Fault Filtering mechnism authority from KERNEL*/
    BOOLEAN                     KernelAuthority;

    /** Is Reboot due to Fault filtering */
    BOOLEAN                     RebootByFF;

    /** Reboot times due to fault filtering */
    INT32U                      RebootTimes;

}GUEST_T_FF_CB;

static GUEST_T_FF_CB       GUEST_V_FF_CB           = 
{ 
    FALSE,

    0u,

    {0u, 0u, 0u, OSW_C_TempoNotStarted}, 

    FALSE,

    FALSE,

    0u
};
/******************************************************************************
*  GLOBAL VARIABLES OF MODULE
*****************************************************************************/

/** Structure which contain configuration elements of guest application*/
#pragma PRQA_MESSAGES_OFF 3218
#pragma PRQA_MESSAGES_COMMENT "Variable only used in one function but defined at the top of the file to increase readability"
static MSTEPINC_T_GuestIoSetup GUEST_V_Setup;
#pragma PRQA_MESSAGES_ON 3218

/** TFT service Name */
#define GUEST_C_TFTP_SERVICE_NAME                  "ftp"

#define GUEST_C_TFTP_SUBSERVICE_NAME_TEST          "cmd"
#define GUEST_C_TFTP_SUBSERVICE_NAME_CMD           "tstcmd"
#define GUEST_C_TFTP_SUBSERVICE_NAME_SN            "sn"
#define GUEST_C_TFTP_SUBSERVICE_NAME_HAND          "hand"
#define GUEST_C_TFTP_SUBSERVICE_FAILCODE_NAME           "fcode"
/*TFTP version*/
#define GUEST_C_TFTP_SUBSERVICE_VERSION_NAME       "vers"
/** TFT Sub Service Name : dmo_subservicename*/
#define GUEST_C_TFTP_SUBSERVICE_NAME_OFFSET        4u

#define GUEST_C_CHECKINTCNT_INTERVAL               600               /* 检查FPGA触发的中断的计数，10分钟检查一次 */

static GUEST_TFTP_E_ConnectionId GUEST_TFTP_V_ConnectionId = GUEST_TFTP_C_CMD_UNUSED;

/*存放CMD信息*/
/*定义为2个字节，用于后续扩展（增加功能检测项）*/
static INT16U g_TstTftpWBuf[GUEST_C_TFTP_WRITE_BUF_LEN];
static CHAR g_TstTftpSNBuf[GUEST_C_TFTP_WRITE_SNBUF_LEN];

OSW_T_pFLAG_GRP* MSTEP_TFTP_V_pFlagGroup = NULL;

/******************************************************************************
*  LOCAL FUNCTION PROTOTYPE
*****************************************************************************/
static ALSTOM_T_Status GUEST_F_Create(MSTEPINC_T_TaskCreateInfo *pCreateInfo);
static ALSTOM_T_Status GUEST_F_Config(void);
static ALSTOM_T_Status GUEST_F_Start(void);
static ALSTOM_T_Status GUEST_F_MainLoop(MSTEPINC_E_FunctionalMode functionalMode);

static void GUEST_F_TftpServiceSubscribe(void);
static ALSTOM_T_Status GUEST_F_TFTP_ServiceOpen(const CHAR *const connectionName,
                                                INT32U remoteIpAddr,
                                                MSTEP_E_TftpConnectionType type,
                                                MSTEP_T_TftpServiceConnectionId *pId,
                                                INT8U **pBuffer,
                                                INT32U *pMaxSize);
static void GUEST_F_TFTP_ServiceClose(MSTEP_T_TftpServiceConnectionId id,
                                      ALSTOM_T_Status status, INT32U size);
static void GUEST_F_SemCreate(void);
static void GUEST_F_GetTestCMD(void);
BOOLEAN    GUEST_F_FaultFilteringAuthorizeGet(GDF_T_EventId eventID, INT32U context, INT8U taskPrio);
void GUEST_F_FaultFiltering_Init(void);
void GUEST_F_FaultFiltering_Running(void);

/************************************************************************/
/* GLOBAL PARA                                                          */
/************************************************************************/

/** Task information for the general startUp process*/
#pragma PRQA_MESSAGES_OFF 3218
#pragma PRQA_MESSAGES_COMMENT "Variable only used in one function but defined at the top of the file to increase readability"
static MSTEPINC_T_TaskInfo GUEST_V_TaskInfo[] =
{
    /* Guest functional task*/
    {
        &GUEST_F_Create,
        &GUEST_F_Config,
        &GUEST_F_Start,
        &GUEST_F_MainLoop,
        MSTEPINC_C_TASK_ID_1,
        MSTEPINC_C_TASK_PRIOLEVEL_HIGH,
        MSTEPINC_C_TASK_PRIO_1,
        "GUEST_MONITOR",
        MSTEPINC_C_ORDER_1,
        GUEST_C_SWWD_REFRESH_PERIOD,
        GUEST_C_TASK_PERIOD,
        MSTEPINC_C_RUN_TASK_ON_BOTH_CPU,
        FALSE
    }
};
#pragma PRQA_MESSAGES_ON 3218

/******************************************************************************
*  LOCAL FUNCTION PROTOTYPE
*****************************************************************************/
static BOOLEAN GUEST_F_ReprogAutho(void)
{
    return TRUE;
}

/**\}*/ /*End of Doxygen GUEST_CORE*/

/******************************************************************************
*  API FUNCTIONS
*****************************************************************************/
void GUEST_F_SystemSetup(INT8U                 *pTaskInfoNumber,
                         MSTEPINC_T_TaskInfo  **pTaskInfo,
                         MSTEPINC_E_BoardMode  *pBoardMode,
                         MSTEPINC_T_NetworkLed *pNetworkLed,
                         MSTEPINC_E_GpioId     *pGPIOForFlashingInter,
                         MSTEPINC_E_GpioId     *pGPIOForFlashingIntra,
                         MSTEPINC_T_MaintenanceTable* pMaintenanceTable,
                         BOOLEAN * pbForceBid,
                         INT8U * pBidValue,
                         MSTEPINC_T_UserData    **pForceKey,
                         INT32U                 *pReprogActivationPattern,
                         MSTEPINC_T_ReprogAutho *pReprogAuthoCallback)
{

    GDF_M_NULL_ASSERT(pTaskInfoNumber);
    GDF_M_NULL_ASSERT(pTaskInfo);
    GDF_M_NULL_ASSERT(pBoardMode);
    GDF_M_NULL_ASSERT(pNetworkLed);
    GDF_M_NULL_ASSERT(pGPIOForFlashingInter);
    GDF_M_NULL_ASSERT(pGPIOForFlashingIntra);
    GDF_M_NULL_ASSERT(pMaintenanceTable);
    GDF_M_NULL_ASSERT(pbForceBid);
    GDF_M_NULL_ASSERT(pBidValue);
    GDF_M_NULL_ASSERT(pForceKey);
    GDF_M_NULL_ASSERT(pReprogActivationPattern);
    GDF_M_NULL_ASSERT(pReprogAuthoCallback);


    *pTaskInfoNumber = (sizeof(GUEST_V_TaskInfo) / sizeof(MSTEPINC_T_TaskInfo));
    //*pTaskInfoNumber = 0;
    *pTaskInfo       = GUEST_V_TaskInfo;
    *pBoardMode      = GUEST_C_BoardMode;
    *pNetworkLed     = GUEST_C_NetworkLed;
    *pGPIOForFlashingInter = MSTEPINC_C_GPIO_NO_ID;
    *pGPIOForFlashingIntra = MSTEPINC_C_GPIO_NO_ID;

    *pbForceBid = FALSE;

    *pBidValue = 0;
    *pForceKey = NULL;

    /* 启用Reprog需要使用inceptor烧录，并在初次烧录完成后调用restore，否则进入BOOTFAILED */
#if 1
    /* enable the re-programmation feature */
    *pReprogActivationPattern = GUEST_C_REPROG_ALLOWED;
    /* Reprog Callback not needed as re-programmation feature is enabled */
    *pReprogAuthoCallback = GUEST_F_ReprogAutho;
#else
    /* Disable the re-programmation feature */
    *pReprogActivationPattern = GUEST_C_REPROG_DISABLED;
    /* Reprog Callback not needed as re-programmation feature is disabled */
    *pReprogAuthoCallback = NULL;
#endif

    /* CR_reference 2153 - Sets the MSTEP Kernel Version as Application Software Version. */
    STD_F_Memcpy(GUEST_C_ApplicationSwVersion, sizeof(GUEST_C_ApplicationSwVersion), MSTEP_KERNEL_VERSION, STD_F_Strlen(MSTEP_KERNEL_VERSION));

    return;
}

MSTEPINC_T_GuestIoSetup* GUEST_F_IoSetupGet(void)
{
    GUEST_V_Setup.guestGpioDefinition = GUEST_C_GPIODefinition;
    GUEST_V_Setup.guestCsDefinition   = GUEST_C_ChipSelectParam;

    return &GUEST_V_Setup;
}

#pragma SCVR_CHECK_OFF 10
#pragma SCVR_CHECK_COMMENT "This name is exported by Kernel, no way to use another one"
BOOLEAN GUEST_F_On_GDFSafe(GDF_T_EventId eventID, INT32U context, INT32U fileNameCrc, INT16U line, INT8U taskPrio)
#pragma SCVR_CHECK_ON 10
{
/*return  value:TRUE: Authorize NOT to filter the fault; FALSE:Authorize  to filter the fault.*/
	BOOLEAN Authorize = FALSE;

/** To avoid complier warning */
	eventID     =  eventID;
    
	context     =  context;

	fileNameCrc =  fileNameCrc;

	line        =  line;

	taskPrio    =  taskPrio;
		
	Authorize = GUEST_F_FaultFilteringAuthorizeGet(eventID, context, taskPrio);
	OSW_F_TempoStop(&GUEST_V_FF_CB.FF_Tempo);
		/* FPGA启用*/
	GUEST_F_ResFPGASetOff();

	/* 12V熔丝使能信号*/
	GUEST_F_FuseTestSetOn();

		if(TRUE == Authorize) 
	{
		/** If the fault is filterd, DISABLE FPGA in order NOT to burn the Hw Fuse */
		GUEST_F_ResFPGASetOn();
	}
	 else
	 {
		 /* 通知FPGA烧熔丝 */
		GUEST_M_FPGA_Write(GUEST_C_FUSE_BREAK_ADDR, 0x5A5A);
	 }
		
		(void) DTXT_F_Printf("GUEST_F_On_GDFSafe\n");

	return (!Authorize);

    return TRUE;
}

#pragma SCVR_CHECK_OFF 10
#pragma SCVR_CHECK_COMMENT "This name is exported by Kernel, no way to use another one"
BOOLEAN GUEST_F_On_GDFEnding(GDF_T_EventId eventID, INT32U context, INT32U fileNameCrc, INT16U line, INT8U taskPrio)
#pragma SCVR_CHECK_ON 10
{
    BOOLEAN bDumpCanOvverrideSoft;
CHAR *FilteringResult= (GUEST_F_FaultFilteringAuthorizeGet(eventID, context, taskPrio) == TRUE) ? GUEST_C_FILTERED : GUEST_C_NOT_FILTERED;
    (void) DTXT_F_Printf("FUSE: event %d, crc %x, line %d, context %d, taskPrio %d,Filtering result: %s\n",
                         eventID, fileNameCrc, line, context, taskPrio,FilteringResult);

    GDF_M_DUMP_STACK();

    bDumpCanOvverrideSoft = FALSE;

    return bDumpCanOvverrideSoft;
}

#pragma SCVR_CHECK_OFF 10
#pragma SCVR_CHECK_COMMENT "This name is exported by Kernel, no way to use another one"
void GUEST_F_On_BootFailed(void)
#pragma SCVR_CHECK_ON 10
{
}

#pragma SCVR_CHECK_OFF 10
#pragma SCVR_CHECK_COMMENT "This name is exported by Kernel, no way to use another one"
void GUEST_F_On_Printf(const char *pChain, INT16U size)
#pragma SCVR_CHECK_ON 10
{
    GDF_M_NULL_ASSERT(pChain);

    /* Use of size input parameters to avoid compilation warning */
#pragma PRQA_MESSAGES_OFF 3199
#pragma PRQA_MESSAGES_COMMENT "Useless assignement to avoid copilation warning: to be removed if size is used"
    size = size;
#pragma PRQA_MESSAGES_ON 3199

    return;
}

void GUEST_F_AllocateMemorySpaceForKernel(INT8U **pKeyAddress,   INT32U *pKeySize,
                                          INT8U **pConfAddress,  INT32U *pConfSize,
                                          INT8U **pParamAddress, INT32U *pParamfSize)
{
    /** Definition of the memory area used to store configuration data relative to the module */
#pragma PRQA_MESSAGES_OFF 612
#pragma PRQA_MESSAGES_COMMENT "Size of configuration data is more than 32k"
    PRAGMA_PLACE_DCA_UDATA static INT32U GUEST_V_ConfMemoryArea[(GUEST_C_CONF_APPLI_MEMORY_SIZE + GUEST_C_CONF_KERNEL_GUEST_MEMORY_SIZE) / sizeof(INT32U)];
#pragma PRQA_MESSAGES_ON 612

#pragma PRQA_MESSAGES_OFF 612
#pragma PRQA_MESSAGES_COMMENT "Size of parameters data is more than 32k"
    /** Parameters sectors area : *2 for switch when new is available */
    static INT32U GUEST_V_ParamMemoryArea[(2 * GUEST_C_PARAM_APPLI_MEMORY_SIZE) / sizeof(INT32U)];
#pragma PRQA_MESSAGES_ON 612

    GDF_M_NULL_ASSERT(pKeyAddress);
    GDF_M_NULL_ASSERT(pKeySize);
    GDF_M_NULL_ASSERT(pConfAddress);
    GDF_M_NULL_ASSERT(pConfSize);
    GDF_M_NULL_ASSERT(pParamAddress);
    GDF_M_NULL_ASSERT(pParamfSize);

    *pKeyAddress = NULL;
    *pKeySize = 0u;

    *pConfAddress = (INT8U *) & GUEST_V_ConfMemoryArea[0];
    *pConfSize = sizeof(GUEST_V_ConfMemoryArea);

    *pParamAddress = (INT8U *) & GUEST_V_ParamMemoryArea[0];
    *pParamfSize = sizeof(GUEST_V_ParamMemoryArea);
}

BOOLEAN GUEST_F_ConfigSubAreaCheck(void)
{
    BOOLEAN ReturnValue = TRUE;

    DTXT_REPORT(DTXT_REPORT_INFO, "Hello World! we are in GUEST_F_ConfigSubAreaCheck");

    return ReturnValue;
}


#pragma SCVR_CHECK_OFF 10
#pragma SCVR_CHECK_COMMENT "This name is exported by Kernel, no way to use another one"
void GUEST_F_On_SafetyFlagsInit(const BOOLEAN *pSafetyFlagsBrokenState, INT32U nbSafetyFlags)
#pragma SCVR_CHECK_ON 10
{

    INT32U i = 0;

    GDF_M_NULL_ASSERT(pSafetyFlagsBrokenState);
    GDF_M_RANGE_ASSERT(((nbSafetyFlags < (GUEST_C_MIN_SAFETY_FLAG + 1)) || (nbSafetyFlags > (GUEST_C_MAX_SAFETY_FLAG + 1U))), nbSafetyFlags);

    DTXT_REPORT1(DTXT_REPORT_WARNING, DTXT_ANSI_FG_MAGENTA"Guest safety flags check", nbSafetyFlags);

    for (i = GUEST_C_MIN_SAFETY_FLAG; i < nbSafetyFlags; i++)
    {
        if (TRUE == pSafetyFlagsBrokenState[i])
        {
            DTXT_REPORT1(DTXT_REPORT_WARNING, DTXT_ANSI_FG_MAGENTA"Broken safety flag: ", i);
            /* Store broken state */
            GUEST_TEST_V_brokenSafetyFlags[i - GUEST_C_MIN_SAFETY_FLAG] = GUEST_TEST_C_safetyFlagBroken;
        }
        else
        {
            /* Store OK state */
            GUEST_TEST_V_brokenSafetyFlags[i - GUEST_C_MIN_SAFETY_FLAG] = GUEST_TEST_C_safetyFlagOK;
        }
    }

    return;
}

/* CR_reference 1965 - Decide if the default can be raised in the OD */
/* Implements <STEP_SwAD_1598> */
BOOLEAN GUEST_F_On_MEVODefaultSet(MSTEPINC_T_FailureCode failureCode,
                                  const MSTEPINC_T_FailureContext* const pFailureContext)
{
    /* to avoid compilation warning */
    GDF_M_NULL_ASSERT(pFailureContext);

#pragma PRQA_MESSAGES_OFF 3199
#pragma PRQA_MESSAGES_COMMENT "Useless assignment to avoid compilation warning: to be removed if nbSafetyFlags is used"
    failureCode = failureCode;
#pragma PRQA_MESSAGES_ON 3199

    return TRUE;
}

/* CR_reference 2153 - Application Software Version hook. */
INT8U* GUEST_F_ApplicationSwVersionGet(void)
{
    return GUEST_C_ApplicationSwVersion;
}

/* Implements <STEP_SwAD_1405> */
INT32U GUEST_F_FaultFilteringTimerGet(void)
{
    INT32U timerFaultFiltering = 0;


   /* Get the timeout timer from configuration data */	
    const GUEST_T_CFG *pCFGAddr = GUEST_F_GetConfigPointer();
    GDF_M_NULL_ASSERT(pCFGAddr);

    timerFaultFiltering=pCFGAddr->FAULT_FILTER_TIMEOUT;	

	
    return timerFaultFiltering;
}

/*****************************************************************************/
/**
* \par Description:  Return the authority to caller.

* \note
*      FILTERING strategy:
*       1. Filtering activated(kernel).
*       2. Filtering_timeout elapsed since the exit of initialization phase(kernel).
*       3. Filtering authorized by application(all kinds of faults are allowed to be filtered) .

* \par Fault cases:
*        - None
*
* \param 
*        - GDF_T_EventId eventID, INT32U context, INT8U taskPrio
*
* \return  
*        - TRUE: Authorize to filter the fault.
*        - FALSE:Authorize NOT to filter the fault.
******************************************************************************/
BOOLEAN    GUEST_F_FaultFilteringAuthorizeGet(GDF_T_EventId eventID, INT32U context, INT8U taskPrio)
{
	BOOLEAN Authority = FALSE;
	BOOLEAN AppAuthority = TRUE;/*all kinds of faults are allowed to be filtered by application*/
	
	/** To avoid complier warning */
	eventID     =  eventID;    
	context     =  context;
	taskPrio    =  taskPrio;
	
	if((TRUE == AppAuthority) && (TRUE == GUEST_V_FF_CB.KernelAuthority))
	{
		Authority = TRUE;
	}
	
	return Authority;
        
}

/*****************************************************************************/
/**
* \par Description:  Initialize fault filtering feature, mainly get fault filtering
*      activation and filtering timeout from configuration data. Retrive fault filtering
*      status of board start.
*
* \par Fault cases:
*        - EVT_C_NULL_PTR_ERROR if can NOT get configuration data.
*
* \param 
*        - None
*
* \return 
*        - None
*
******************************************************************************/
void              GUEST_F_FaultFiltering_Init(void)
{

    const GUEST_T_CFG *pCFGAddr = GUEST_F_GetConfigPointer();
	
    GDF_M_NULL_ASSERT(pCFGAddr);

    /** Get activation of fault filtering feature from configuration data */
    GUEST_V_FF_CB.Activation = (BOOLEAN)(pCFGAddr->FAULT_FILTER_ACTIVATION);

    if ( TRUE == GUEST_V_FF_CB.Activation )
    {
        GUEST_V_FF_CB.FilteringTout = GUEST_C_SECONDS_PER_MINUTE * OSW_C_TICKS_PER_SEC * GUEST_F_FaultFilteringTimerGet();

        GUEST_V_FF_CB.FilteringTout = GUEST_V_FF_CB.FilteringTout + GUEST_V_FF_CB.FilteringTout / 100;
    }

    /** Fault Filtering status retrival */
    GUEST_V_FF_CB.RebootByFF  = MSTEP_F_FaultFilteringStateGet();

    GUEST_V_FF_CB.RebootTimes = MSTEP_F_NbFaultFilteringRebootGet();

    DTXT_REPORT2(DTXT_REPORT_INFO, DTXT_C_ANSI_FG_BRIGHTRED"==============Fault Filtering status:reboot status, reboot times !=========", 
        GUEST_V_FF_CB.RebootByFF,
        GUEST_V_FF_CB.RebootTimes);

    /** Clear failure code in MIB
    MSTEP_F_FaultFilteringAcknowledge();
    */
    
}
/*****************************************************************************/
/**
* \par Description:  Count filtering timer and update authority of fault filtering.

* \par Fault cases:
*        - None
*
* \param 
*        - None
*
* \return  
*        - None
*
******************************************************************************/
void GUEST_F_FaultFiltering_Running(void)
{
	BOOLEAN bElapsed = FALSE;		
	BOOLEAN bElapsedOther = FALSE;
	
/*满足3个条件进行错误过滤:1配置中错误过滤功能使能; 2 超过设定的错误过滤时间;3应用授权；*/
/*此处模拟kernel授权的条件，即条件1、2*/
   do
    {
        /** If Fault Filtering is deactivation or the authority is set, do nothing */
        if (( FALSE == GUEST_V_FF_CB.Activation ) ||
            ( TRUE  == GUEST_V_FF_CB.KernelAuthority ))
        {
            break;
        }
		
	/*evaluate the time is elapsed or not when fault occured*/
	bElapsed = OSW_F_TempoState(&GUEST_V_FF_CB.FF_Tempo);

        /** Authority shall be 2oo2 between 2 CPUs */
        MSTEP_F_K2oo2CpuExchange((INT8U *const)&bElapsedOther,
                                  (const INT8U*)&bElapsed, 
                                  sizeof(INT8U),
                                  GUEST_C_2oo2_TIMEOUT);
		
        if ((TRUE == bElapsedOther) && (TRUE == bElapsed))
        {
		GUEST_V_FF_CB.KernelAuthority =TRUE;           
        }

   }while(0);
}


/******************************************************************************
*  LOCAL FUNCTIONS
*****************************************************************************/
static void GUEST_F_SemCreate(void)
{

    INT32U i = 0u;

    /* 初始化信号量 */
    g_Sem.TFTP_BUF_EVENT = OSW_F_SemCreate(1);
    GDF_M_NULL_ASSERT(g_Sem.TFTP_BUF_EVENT);
	
    g_Sem.TFTP_ERRBUF_EVENT = OSW_F_SemCreate(1);
    GDF_M_NULL_ASSERT(g_Sem.TFTP_ERRBUF_EVENT);

    g_Sem.TFTP_TESTBUF_EVENT = OSW_F_SemCreate(1);/*Add FCT Sem*/
    GDF_M_NULL_ASSERT(g_Sem.TFTP_TESTBUF_EVENT);

    for (i = 0;i < GUEST_C_TASK_NUM;++i)
    {
        VitalStamp[i].pCrcTable[0] = 0xAAAAAAAAu;
        VitalStamp[i].pCrcTable[1] = 0x55555555u;
    }

    return;
}

/* 启动FTP服务 */
static void GUEST_F_TftpServiceSubscribe(void)
{
    ALSTOM_T_Status Status = ALSTOM_C_ERROR;

    Status = MSTEP_F_TftpServiceSubscribe(GUEST_C_TFTP_SERVICE_NAME, &GUEST_F_TFTP_ServiceOpen, &GUEST_F_TFTP_ServiceClose);

    return;
}


static ALSTOM_T_Status GUEST_F_TFTP_ServiceOpen(const CHAR *const connectionName,
                                                INT32U remoteIpAddr,
                                                MSTEP_E_TftpConnectionType type,
                                                MSTEP_T_TftpServiceConnectionId *pId,
                                                INT8U **pBuffer,
                                                INT32U *pMaxSize)
{
    ALSTOM_T_Status Status = ALSTOM_C_NO_ERROR;

    static INT8U SendBuf[GUEST_C_TFTP_BUF_SIZE] = {0};
    static INT8U	SendErrBuf[GUEST_C_TFTP_ERRBUF_SIZE] = {0};
    static INT8U VerBuf[GUEST_C_TFTP_VER_BUF_SIZE] = {0};
    static INT8U HandBuf[GUEST_C_TFTP_HAND_BUF_SIZE] = {0};

    GDF_M_NULL_ASSERT(connectionName);
    GDF_M_NULL_ASSERT(pId);
    GDF_M_NULL_ASSERT(pBuffer);
    GDF_M_NULL_ASSERT(pMaxSize);

    DTXT_REPORT1(DTXT_REPORT_INFO, "GUEST_F_TFTP_ServiceOpen start RMTIP", remoteIpAddr);

    /* 读取文件--文件操作类型 */

    if (GUEST_TFTP_C_CMD_UNUSED == GUEST_TFTP_V_ConnectionId)
    {

        /* 文件名比较一致 ftp_XXXX */
        if (0 == STD_F_Strncmp(&connectionName[0], GUEST_C_TFTP_SERVICE_NAME,
                               (INT32S)(GUEST_C_TFTP_SUBSERVICE_NAME_OFFSET - 1)))
        {

            if (TFTP_C_CONNECTION_READ == type)
            {

                /*发送输出命令 ftp_test*/
                if (0 == STD_F_Strncmp(&connectionName[GUEST_C_TFTP_SUBSERVICE_NAME_OFFSET],
                                       GUEST_C_TFTP_SUBSERVICE_NAME_TEST,
                                       (INT32S)STD_F_Strlen(GUEST_C_TFTP_SUBSERVICE_NAME_TEST)))
                {
                    GUEST_F_ReadDataFromTFTPBUF(SendBuf);

                    *pBuffer = SendBuf;

                    if (g_TFTPBUFFULL == FALSE)
                    {
                        *pMaxSize = WriteOffset;
                    }
                    else
                    {
                        *pMaxSize = GUEST_C_TFTP_BUF_SIZE;
                    }

                    GUEST_TFTP_V_ConnectionId = GUEST_TFTP_C_CMD_READ;
                }
		/*发送错误码命令 ftp_fcode*/
                else if (0 == STD_F_Strncmp(&connectionName[GUEST_C_TFTP_SUBSERVICE_NAME_OFFSET],
                                            GUEST_C_TFTP_SUBSERVICE_FAILCODE_NAME,
                                            (INT32S)STD_F_Strlen(GUEST_C_TFTP_SUBSERVICE_FAILCODE_NAME)))
                {
                    GUEST_F_ReadFailCodeFromTFTPBUF(&SendErrBuf[0]);

                    *pBuffer = SendErrBuf;

                     if (g_TFTPERRBUFFULL == FALSE)
                    {
                        *pMaxSize = WriteFCodeOffset;
		    }
                    else
                    {
                       *pMaxSize = GUEST_C_TFTP_ERRBUF_SIZE;
                    }

                  GUEST_TFTP_V_ConnectionId = GUEST_TFTP_C_CMD_READ_FCODE;
                }

                /*发送握手命令 ftp_hand*/
                else if (0 == STD_F_Strncmp(&connectionName[GUEST_C_TFTP_SUBSERVICE_NAME_OFFSET],
                                            GUEST_C_TFTP_SUBSERVICE_NAME_HAND,
                                            (INT32S)STD_F_Strlen(GUEST_C_TFTP_SUBSERVICE_NAME_HAND)))
                {
                    GUEST_F_ReadDataFromTFTPHandBUF(HandBuf);

                    *pBuffer = HandBuf;
                    *pMaxSize = GUEST_C_TFTP_HAND_BUF_SIZE;

                    GUEST_TFTP_V_ConnectionId = GUEST_TFTP_C_CMD_READ_HAND;
                }

                /*发送版本信息 ftp_vers*/
                else if (0 == STD_F_Strncmp(&connectionName[GUEST_C_TFTP_SUBSERVICE_NAME_OFFSET],
                                            GUEST_C_TFTP_SUBSERVICE_VERSION_NAME,
                                            (INT32S)STD_F_Strlen(GUEST_C_TFTP_SUBSERVICE_VERSION_NAME)))
                {

                    GUEST_F_SendVerData(VerBuf);

                    *pBuffer = VerBuf;
                    *pMaxSize = GUEST_C_TFTP_VER_BUF_SIZE;

                    GUEST_TFTP_V_ConnectionId = GUEST_TFTP_C_CMD_READ_VER;
                }
                else
                {
                    DTXT_REPORT(DTXT_REPORT_INFO, "TFTP SubServie Name Error");
                    Status = ALSTOM_C_ERROR;
                }

            }
            else if (TFTP_C_CONNECTION_WRITE == type)
            {

                if (0 == STD_F_Strncmp(&connectionName[GUEST_C_TFTP_SUBSERVICE_NAME_OFFSET],
                                       GUEST_C_TFTP_SUBSERVICE_NAME_CMD,
                                       (INT32S)STD_F_Strlen(GUEST_C_TFTP_SUBSERVICE_NAME_CMD)))
                {
                    *pBuffer = (INT8U*)g_TstTftpWBuf;/**/
                    *pMaxSize = GUEST_C_TFTP_WRITE_BUF_LEN;

                    GUEST_TFTP_V_ConnectionId = GUEST_TFTP_C_CMD_WRITE_CMD;
                }
                else if (0 == STD_F_Strncmp(&connectionName[GUEST_C_TFTP_SUBSERVICE_NAME_OFFSET],
                                            GUEST_C_TFTP_SUBSERVICE_NAME_SN,
                                            (INT32S)STD_F_Strlen(GUEST_C_TFTP_SUBSERVICE_NAME_SN)))
                {
                    *pBuffer = (INT8U*)g_TstTftpSNBuf;/**/
                    *pMaxSize = GUEST_C_TFTP_WRITE_SNBUF_LEN;

                    GUEST_TFTP_V_ConnectionId = GUEST_TFTP_C_CMD_WRITE_SN;
                }
                else
                {
                    DTXT_REPORT(DTXT_REPORT_INFO, "TFTP SubServie Name Error");
                    Status = ALSTOM_C_ERROR;
                }

            }
            else
            {
                DTXT_REPORT(DTXT_REPORT_INFO, "TFTP Request Type Error");
                Status = ALSTOM_C_ERROR;
            }

        }
        else
        {
            DTXT_REPORT(DTXT_REPORT_INFO, "TFTP Servie Name Error");
            Status = ALSTOM_C_ERROR;
        }

        *pId = GUEST_TFTP_V_ConnectionId;

    }
    else
    {
        /* A command is already running */
        Status = ALSTOM_C_ERROR;
    }

    return Status;
}

static void GUEST_F_TFTP_ServiceClose(MSTEP_T_TftpServiceConnectionId id,
                                      ALSTOM_T_Status status, INT32U size)
{
    if (id == (INT32U)GUEST_TFTP_V_ConnectionId)
    {
        if (ALSTOM_C_NO_ERROR == status)
        {
            switch (id)
            {

                case GUEST_TFTP_C_CMD_READ:

                    if (g_TFTPBUFFULL == TRUE)
                    {
                        g_TFTPBUFFULL = FALSE;
                    }

                    if (size <= GUEST_C_TFTP_BUF_SIZE)
                    {
                        WriteOffset = 0u;
                    }

                    break;

                case GUEST_TFTP_C_CMD_READ_VER:

                case GUEST_TFTP_C_CMD_READ_HAND:

                case GUEST_TFTP_C_CMD_WRITE_CMD:

                case GUEST_TFTP_C_CMD_WRITE_SN:
                    /* Value has been read, nothing to do */
                    break;

                default:
                    break;
            }
        }

        GUEST_TFTP_V_ConnectionId = GUEST_TFTP_C_CMD_UNUSED;
    }

}



/**
******************************************************************************
* \par Description :
*      Create function. Called by the MSTEP kernel before the \ref GUEST_F_Start function
*      The user must manage the config information here.
*
* \par Fault cases :
*      - EVT_C_NULL_PTR_ERROR if pAddress is pCreateInfo
*
* \par Global variable used:
*      None
*
* \par Pseudo-code:
*      -# Check parameters
*      -# Set GUEST task in pCreateInfo structure
*      -# Set GUEST Stack size in pCreateInfo structure
*
* \param *pCreateInfo - In - Range: [!NULL]               \n
*      Pointer on Create information structure
*
* \return Status
*      - ALSTOM_C_NO_ERROR
*
*****************************************************************************/
static ALSTOM_T_Status GUEST_F_Create(MSTEPINC_T_TaskCreateInfo *pCreateInfo)
{
    ALSTOM_T_Status Status = ALSTOM_C_NO_ERROR;
    ALSTOM_T_Status err = ALSTOM_C_ERROR;

    /** stack memory declaration*/
    static INT32U  GUEST_V_TaskStk[GUEST_C_STACK_SIZE];

    GDF_M_NULL_ASSERT(pCreateInfo);

    pCreateInfo->pTaskStack     = GUEST_V_TaskStk;
    pCreateInfo->taskStackSize  = GUEST_C_STACK_SIZE;

#ifdef ALSTOM_CONFIG_CPU_A
    
        MSTEP_TFTP_V_pFlagGroup = OSW_F_FlagCreate(0u, &err);
    
        if ((NULL == MSTEP_TFTP_V_pFlagGroup) || (ALSTOM_C_NO_ERROR != err))
        {
            GDF_M_FAULT_EXT(EVT_C_OS_ERROR, err, "");
        }
        
        OSW_F_FlagPost(MSTEP_TFTP_V_pFlagGroup, 1, OSW_C_FLAG_SET, &err);
    
        Status = err;
#else
        err = err;
#endif

    /* 设置程序运行状态 */
    g_RunState = GUEST_C_STATE_INITIAL;

    /* 初始化信号量 */
    GUEST_F_SemCreate();

    GUEST_F_TftpServiceSubscribe();

    return Status;
}


/**
******************************************************************************
* \par Description :
*      Config function. Called by the MSTEP kernel before the \ref GUEST_F_Start function
*      The user must manage the config information here.
* \par Fault cases : none
*
* \par Global variable used:
*      None
*
* \return Status
*      - ALSTOM_C_NO_ERROR
*
*****************************************************************************/
static ALSTOM_T_Status GUEST_F_Config(void)
{
    ALSTOM_T_Status configStatus = ALSTOM_C_NO_ERROR;

    return configStatus;
}

/**
******************************************************************************
* \par Description :
*      Start function. Called by the MSTEP kernel before starting the main
*      process of the task
*
* \par Fault cases :
*      - None
*
* \par Global variable used:
*      None
*
* \return Status
*      - ALSTOM_C_NO_ERROR
*
*****************************************************************************/
static ALSTOM_T_Status GUEST_F_Start(void)
{
    ALSTOM_T_Status Status = ALSTOM_C_NO_ERROR;

    INT8U i = 0;
    GUEST_F_FaultFiltering_Init();
    /* Initialize the interrupt, and invoke the ISR function */
    GUEST_F_ISRINIT();

    /* Set activity of 5ms square wave */
    GUEST_F_SetDYNActive();

    /* Enable the fuse test control signal */
    GUEST_F_FuseTestSetOn();
    
	//PRINT_F_Printf("init of guest IodbData-------->");
	for(i = 0; i < TB_SEND_NUM; ++i)
	{
		TB_writeIOdBData[i].dataId = i+1;
		TB_writeIOdBData[i].value = 0;
	}
	
	for (i = 0; i < TB_RECV_NUM; ++i)
	{
		TB_readIOdBData[i].dataId = i+1;
		TB_readIOdBData[i].value = 1;
	}
    
    return Status;
}


/**
******************************************************************************
FCT MainLoop
*****************************************************************************/

static ALSTOM_T_Status GUEST_F_MainLoop(MSTEPINC_E_FunctionalMode functionalMode)
{	
    INT16U outputs = GUEST_C_RESTRICTIVE_CMD;

    static INT32U SendTFTFCMDLoopNum = (30 * 1000u) / ((INT32U)GUEST_C_TASK_PERIOD * 50u);

    INT8U i = 0;
    INT8U TB_index = 0;
    INT8U TB_shift = 0;
    
    TB_Receive_Data[0] = 0;
    TB_Receive_Data[1] = 0;
    TB_Receive_Data[2] = 0;
    TB_Receive_Data[3] = 0;
    
    /* Start a K2002 cycle*/
    MSTEP_F_K2oo2CycleBegin();
    

    /* Implements <STEP_SwAD_0170> */
    MSTEP_F_TaskFlagSet();
     
    ++g_CycleNO;

    if ((functionalMode == ALSTOM_C_FUNCTIONAL_GUEST)
        || (functionalMode == ALSTOM_C_FUNCTIONAL_GUEST_RESTRICTIVE))
    {
#if 1				
		MSTEP_F_IodbRequest(TB_RECV_NUM,TB_readIOdBData); 
		
		for(i = 0; i < TB_RECV_NUM; ++i)
		{
			
			TB_Receive_Data[TB_index] = TB_Receive_Data[TB_index] | (TB_readIOdBData[i].value <<TB_shift);
			TB_shift+=1;
			if(0 == (i+1)%32)
			{
				TB_shift = 0;
				TB_index += 1;
			}
		}
		
		if(0 == TB_index)
		{
			GUEST_M_FPGA_Write(GUEST_C_OUTPUTLIGHT_LAMP_ADDR, TB_Receive_Data[0]);	
		}
		else
		{
			GUEST_M_FPGA_Write(GUEST_C_OUTPUTLIGHT_LAMP_ADDR, TB_Receive_Data[g_CycleNO%TB_index]);	
		}	
#endif
      g_ReadIOdBData = (INT16U)TB_Receive_Data[g_CycleNO%TB_index];
		GUEST_F_WriteMNTDataAndSend(g_ReadIOdBData);
				
//		GUEST_M_FPGA_Write(GUEST_C_OUTPUTLIGHT_LAMP_ADDR, TB_readIOdBData[g_CycleNO%TB_RECV_NUM].value << (g_CycleNO%16));
		
//		DTXT_F_Report(DTXT_REPORT_INFO, DTXT_C_LOC, "request SDB data:", 4, TB_Receive_Data[0], TB_Receive_Data[1], TB_Receive_Data[2], TB_Receive_Data[3], 0, 0);   		
    	
        do
        {       	
            GUEST_F_FaultFiltering_Running();

		 /* 设置程序运行状态 */
            if (GUEST_C_STATE_INITIAL == g_RunState)
            {
                /*FPGA启用*/
                GUEST_F_ResFPGASetOff();

                OSW_F_TimeDly(5);

                /*Get Software Data Id of the Code*/
                GUEST_F_GetDataInOneCPU();

                DTXT_REPORT(DTXT_REPORT_INFO, "----Enter PREOPERATIONAL State----");
                g_RunState = GUEST_C_STATE_PREOPERATIONAL;
            }

            /*熔丝看门狗喂狗*/
            GUEST_F_FeedWDog();

            /*获取测试命令*/

            GUEST_F_GetTestCMD();

            /*GUEST_F_LED_Snake();*/
//            g_StartTest =TFTP_MSG_Hand;
            /*测试握手和终止*/
            if (g_StartTest == TFTP_MSG_Hand)
            {
                DTXT_REPORT(DTXT_REPORT_INFO, "=========Monitor Start!!=========");

                g_RunState = GUEST_C_STATE_OPERATIONAL;
            }

            if (g_StartTest == TFTP_MSG_STOP)
            {
                DTXT_REPORT(DTXT_REPORT_INFO, "=========Monitor STOP!!=========");

                g_RunState = GUEST_C_STATE_PREOPERATIONAL;
            }

            if (GUEST_C_STATE_PREOPERATIONAL == g_RunState)
            {
                if ( OSW_C_TempoNotStarted == GUEST_V_FF_CB.FF_Tempo.eState )
                {
                	OSW_F_TempoStart(&GUEST_V_FF_CB.FF_Tempo,GUEST_V_FF_CB.FilteringTout);
                }
                break;
            }
//            g_StartTest = TFTP_MSG_PDDM;
            switch (0)
            {

                case TFTP_MSG_VOOB:
                    DTXT_REPORT(DTXT_REPORT_INFO, "=========VOOB Test=========");

                    /* 从IODB中读驱动命令 */
                    GUEST_F_IODB_GetCmd_VOOB(&outputs);

                    /*驱动输出*/
                    GUEST_F_DriveOutput(outputs, TRUE, GUEST_C_LED_OPEN);

                    break;

                case TFTP_MSG_PDDM:
                    DTXT_REPORT(DTXT_REPORT_INFO, "=========PDDM Test=========");

                    /* 从IODB中读驱动命令 */
                    GUEST_F_IODB_GetCmd_PDDM(&outputs);

                    /* 模拟采表 */
                    GUEST_F_RBPointStatus(outputs);
                    break;

                case TFTP_MSG_SDDM:
                    DTXT_REPORT(DTXT_REPORT_INFO, "=========SDDM Test=========");
                    break;

                case TFTP_MSG_TCIM:
                    DTXT_REPORT(DTXT_REPORT_INFO, "=========TCIM Test=========");

                    /* 采集24V电压状态 */
                    GUEST_F_TSPSTForOPT();

                    /* 模拟股道占用/空闲 */
                    GUEST_F_IODB_DrvCmd_TCIM();
                    break;

                case TFTP_MSG_CDDM:
                    DTXT_REPORT(DTXT_REPORT_INFO, "=========CDDM Test=========");

                    /* 从IODB中读驱动命令 */
                    GUEST_F_IODB_GetCmd_CDDM(&outputs);

                    /*驱动输出*/
                    GUEST_F_DriveOutput(outputs, TRUE, GUEST_C_LED_OPEN);

                    /* 采集24V电压状态 */
                    GUEST_F_TSPSTForOPT();

                    /*  模拟FBJ */
                    GUEST_F_SendFBJState();

                    break;

                case TFTP_MSG_SIOM:
                    DTXT_REPORT(DTXT_REPORT_INFO, "=========SIOM Test=========");
                    break;

                default:
                    DTXT_REPORT1(DTXT_REPORT_INFO, "=========WAITING=========", g_StartTest);
                    break;
            }

            if ((0u != (outputs ^ g_LastTest)) || (0u == (g_CycleNO % SendTFTFCMDLoopNum)))
            {
                GUEST_F_WriteDataToTFTPBUF(outputs);
            }

        }
        while (0);
        
        for (i = 0; i < TB_SEND_NUM; ++i)    //TB_SEND_NUM为提交的数据个数，应参考配置文件，超出最大值系统会宕机
		{  			
			TB_writeIOdBData[i].value = g_CycleNO%2;   //TB_writeIOdBData[i].dataId 要先初始化
			TB_writeIOdBData[i].dataId = i+1;
		}

		MSTEP_F_IodbSubmit(TB_SEND_NUM, TB_writeIOdBData);  

        g_LastTest = outputs;

    }

    //(void) DTXT_F_Printf("-----------cycleNO:%d %d-------------\n", g_CycleNO, OSW_F_TickGet() - ticket);

    MSTEP_F_CryptlibCRC32FSFB2ChanOneCompute((INT8U *)&g_StartTest,
                                             sizeof(g_StartTest),
                                             &VitalStamp[0].pCrcTable[0]);

    MSTEP_F_K2oo2CycleEnd(VitalStamp);
    
    return ALSTOM_C_NO_ERROR;
}

static void GUEST_F_GetTestCMD(void)
{
    INT16U RMTCMDfromTFTP = 0;

#ifdef ALSTOM_CONFIG_CPU_A
    g_StartTest = g_TstTftpWBuf[0];
#endif
    MSTEP_F_K2oo2CpuExchange((INT8U *)&RMTCMDfromTFTP, (INT8U *)&g_StartTest, sizeof(g_StartTest), GUEST_C_2oo2_TIMEOUT);

#ifdef ALSTOM_CONFIG_CPU_B
    g_StartTest = RMTCMDfromTFTP;
#endif
}
INT8U GUEST_F_PDOForgiveCntGet(void)
{
 	return 9u;
}
INT32U GUEST_F_GetPrintLevel(void)
{	
	return 5u;
}
/* EOF */
