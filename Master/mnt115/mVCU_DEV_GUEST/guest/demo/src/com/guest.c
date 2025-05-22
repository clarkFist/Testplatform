/******************************************************************************
 * COPYRIGHT (C) CASCO Signal 2015. VIOP Project. All rights reserved.
 *****************************************************************************/

#pragma SCVR_CHECK_OFF 05
#pragma SCVR_CHECK_OFF 06
#pragma SCVR_CHECK_COMMENT "This is a guest module for code example, the header file naming is not important"
#pragma SCVR_CHECK_ALIAS "guest"

/**
* @file guest.c
* @brief
* @author Fu Liyu
* @date 2013-6-2
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: fuliyu@casco.com.cn
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/

//#include "guest_isolation.h"
//#include "guest_fuse.h"
//#include "guest_outputport.h"
//#include "guest_default.h"
//#include "guest_readback.h"
#include "com.h"
#include "step_guest.h"
#pragma PRQA_MESSAGES_OFF 3218
#pragma PRQA_MESSAGES_COMMENT "Variable only used in one function but defined at the top of the file to increase readability"

#ifdef ALSTOM_CONFIG_CPU_A
static const MSTEPINC_T_GpioDefinition
GUEST_C_GPIODefinition[MSTEPINC_C_GPIO_NB_MAX] =
{
    {MSTEPINC_C_GPIO_NO_ID, MSTEPINC_C_GPIO_UNUSED, MSTEPINC_C_GPIO_DONT_CARE},
    {MSTEPINC_C_GPIO_ID1,  MSTEPINC_C_GPIO_OUTPUT, MSTEPINC_C_GPIO_ON},
    {MSTEPINC_C_GPIO_ID2,  MSTEPINC_C_GPIO_INPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID3,  MSTEPINC_C_GPIO_INPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID4,  MSTEPINC_C_GPIO_OUTPUT, MSTEPINC_C_GPIO_ON},
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
    {MSTEPINC_C_GPIO_ID1,  MSTEPINC_C_GPIO_OUTPUT, MSTEPINC_C_GPIO_ON},
    {MSTEPINC_C_GPIO_ID2,  MSTEPINC_C_GPIO_INPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID3,  MSTEPINC_C_GPIO_INPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID4,  MSTEPINC_C_GPIO_OUTPUT, MSTEPINC_C_GPIO_ON},
    {MSTEPINC_C_GPIO_ID5,  MSTEPINC_C_GPIO_OUTPUT, MSTEPINC_C_GPIO_ON},
    {MSTEPINC_C_GPIO_ID6,  MSTEPINC_C_GPIO_INPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID7,  MSTEPINC_C_GPIO_INPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID8,  MSTEPINC_C_GPIO_INPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID9,  MSTEPINC_C_GPIO_INPUT, MSTEPINC_C_GPIO_OFF},
    {MSTEPINC_C_GPIO_ID10, MSTEPINC_C_GPIO_OUTPUT, MSTEPINC_C_GPIO_ON},
    {MSTEPINC_C_GPIO_ID11, MSTEPINC_C_GPIO_OUTPUT, MSTEPINC_C_GPIO_OFF},
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
    {MSTEPINC_C_CS_NO_ID, MSTEPINC_C_CS_INVALID, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u},
    {MSTEPINC_C_CS_ID1, MSTEPINC_C_CS_VALID, 0u, 0u, 15u, 0u, 2u, 0u, 0u, 0u, 0u},
    {MSTEPINC_C_CS_ID2, MSTEPINC_C_CS_INVALID, 0u, 0u, 15u, 1u, 0u, 0u, 0u, 0u, 0u}
};

#pragma PRQA_MESSAGES_ON 3218

/** Definition of the board type */
#pragma PRQA_MESSAGES_OFF 3218
#pragma PRQA_MESSAGES_COMMENT "Variable only used in one function but defined at the top of the file to increase readability"
static const MSTEPINC_E_BoardMode GUEST_C_BoardMode = MSTEPINC_C_STEPPER;
#pragma PRQA_MESSAGES_ON 3218

/** Network LED configuration */
#pragma PRQA_MESSAGES_OFF 3218
#pragma PRQA_MESSAGES_COMMENT "Variable only used in one function but defined at the top of the file to increase readability"

#ifdef VCU_CONF_CPU_A
static const MSTEPINC_T_NetworkLed GUEST_C_NetworkLed =
{
    MSTEPINC_C_GPIO_ID13,  /**< CAN */
    MSTEPINC_C_GPIO_ID14/**< ETH */

};
#endif

#ifdef VCU_CONF_CPU_B
static const MSTEPINC_T_NetworkLed GUEST_C_NetworkLed =
{
    MSTEPINC_C_GPIO_ID12,  /**< CAN */
    MSTEPINC_C_GPIO_ID13   /**< ETH */

};
#endif

/* CR_reference 2153 - Application Software Version. */
static INT8U GUEST_C_ApplicationSwVersion[15];

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
#define GUEST_C_STACK_SIZE   (INT32U)700u


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
#ifdef VCU_CONF_CPU_A
#define GUEST_C_REPROG_DISABLED 0x4F8B0E65u
#else
#define GUEST_C_REPROG_DISABLED 0x4F5F4096u
#endif
/******************************************************************************
 *  TYPE DEFINITION
 *****************************************************************************/

/** Command of the cycle counter tftp service */
typedef enum
{
    /** No command */
    GUEST_TFTP_C_CMD_NOTHING,
    /** Read command */
    GUEST_TFTP_C_CMD_READ,
    /** Write command */
    GUEST_TFTP_C_CMD_WRITE,
    /** Version command */
    GUEST_TFTP_C_CMD_VERS,
    /** FailureCode command */
    GUEST_TFTP_C_CMD_FILC
} GUEST_TFTP_E_CommandType;

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



static INT8U g_SelfTest = ALSTOM_C_NOT_RUNNING;



/** Current command */
static GUEST_TFTP_E_CommandType GUEST_TFTP_V_CurrentCommand = GUEST_TFTP_C_CMD_NOTHING;

/** TFT service Name */
#define GUEST_C_TFTP_SERVICE_NAME                  "ftp"
/** TFT Cycle Counter sub service Name  */
#define GUEST_C_TFTP_SUBSERVICE_NAME               "cmd"
/*TFTP version*/
#define GUEST_C_TFTP_SUBSERVICE_VERSION_NAME       "vers"
/*TFTP Failure Code*/
#define GUEST_C_TFTP_SUBSERVICE_FAILCODE_NAME      "fcode"
/** TFT Sub Service Name : dmo_subservicename*/
#define GUEST_C_TFTP_SUBSERVICE_NAME_OFFSET        4u

#define GUEST_C_CHECKINTCNT_INTERVAL               (10u * 60u)              /* 检查FPGA触发的中断的计数，10分钟检查一次 */

#define GUEST_C_RECORDRELAYLIFE_INTERVAL           (60u * 60u)              /*记录继电器动作以及超期报警间隔，一小时一次*/



/************************************************************************/
/* GLOBAL PARA                                                          */
/************************************************************************/

/** Task information for the general startUp process*/
#pragma PRQA_MESSAGES_OFF 3218
#pragma PRQA_MESSAGES_COMMENT "Variable only used in one function but defined at the top of the file to increase readability"

#pragma PRQA_MESSAGES_ON 3218

/******************************************************************************
 *  LOCAL FUNCTION PROTOTYPE
 *****************************************************************************/
/**
******************************************************************************
* \par Description:
*      This function gives authority of remote re-programming.
*
* \param NONE
*
*
* \return authority of remote re-programming
*****************************************************************************/
BOOLEAN GUEST_F_ReprogAutho(void)
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


    *pTaskInfoNumber = 1u;
    *pTaskInfo       = &COM_V_TaskInfo;
    *pBoardMode      = GUEST_C_BoardMode;/*20180528, code review, CR:755*/
   
    pNetworkLed->canLed     = COM_C_NetworkLed.canLed;
    pNetworkLed->masterLed  = COM_C_NetworkLed.masterLed;
    pNetworkLed->ethLed1    = COM_C_NetworkLed.ethLed1;
    pNetworkLed->ethLed2    = COM_C_NetworkLed.ethLed2;
    
    *pGPIOForFlashingInter = MSTEPINC_C_GPIO_NO_ID;
    *pGPIOForFlashingIntra = MSTEPINC_C_GPIO_NO_ID;

    (*pMaintenanceTable)[0] = COM_V_MaintenanceTable[0];
    (*pMaintenanceTable)[1] = COM_V_MaintenanceTable[1];

    *pbForceBid = FALSE;

    *pBidValue = 0u;
    *pForceKey = NULL;

    /* Disable the re-programmation feature */
    *pReprogActivationPattern = GUEST_C_REPROG_ALLOWED;
    /* Reprog Callback not needed as re-programmation feature is disabled */
    *pReprogAuthoCallback = GUEST_F_ReprogAutho;

    STD_F_Memcpy(COM_C_ApplicationSwVersion, sizeof(COM_C_ApplicationSwVersion), MSTEP_KERNEL_VERSION, STD_F_Strlen(MSTEP_KERNEL_VERSION));
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
#pragma PRQA_MESSAGES_OFF 3199
#pragma PRQA_MESSAGES_COMMENT "Useless assignment to avoid compilation warning"
    eventID = eventID;
    context = context;
    fileNameCrc = fileNameCrc;
    line = line;
    taskPrio = taskPrio;
#pragma PRQA_MESSAGES_ON 3199
    return FALSE;
}



#pragma SCVR_CHECK_OFF 10
#pragma SCVR_CHECK_COMMENT "This name is exported by Kernel, no way to use another one"
BOOLEAN GUEST_F_On_GDFEnding(GDF_T_EventId eventID, INT32U context, INT32U fileNameCrc, INT16U line, INT8U taskPrio)
#pragma SCVR_CHECK_ON 10
{
	(void) DTXT_F_Printf("FUSE: event %d, crc %x, line %d, context %d, taskPrio %d\n",
                         eventID, fileNameCrc, line, context, taskPrio);
	GDF_M_DUMP_STACK();
	MSTEP_F_GpioSetOff(MSTEPINC_C_GPIO_ID13);
	MSTEP_F_GpioSetOff(MSTEPINC_C_GPIO_ID14);
    return FALSE;
}

#pragma SCVR_CHECK_OFF 10
#pragma SCVR_CHECK_COMMENT "This name is exported by Kernel, no way to use another one"
void GUEST_F_On_BootFailed(void)
#pragma SCVR_CHECK_ON 10
{
	MSTEP_F_GpioSetOff(MSTEPINC_C_GPIO_ID13);
	MSTEP_F_GpioSetOff(MSTEPINC_C_GPIO_ID14);
return;
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

    static INT32U GUEST_V_StepperKeyMemoryArea[0x530000u/*0x530000u，因编译镜像太大，所以暂时减小分配空间, by SXR*/ / sizeof(INT32U)];
    PRAGMA_PLACE_DCA_UDATA static INT32U GUEST_V_ConfMemoryArea[(GUEST_C_CONF_APPLI_MEMORY_SIZE + GUEST_C_CONF_KERNEL_STEPPER_MEMORY_SIZE) / sizeof(INT32U)];
// static INT32U GUEST_V_ConfMemoryArea[(GUEST_C_CONF_APPLI_MEMORY_SIZE + GUEST_C_CONF_KERNEL_STEPPER_MEMORY_SIZE) / sizeof(INT32U)];
#pragma PRQA_MESSAGES_COMMENT "Size of parameters data is more than 32k"
    /** Parameters sectors area : *2 for switch when new is available */
    static INT32U GUEST_V_ParamMemoryArea[0x70000u / sizeof(INT32U)];

    GDF_M_NULL_ASSERT(pKeyAddress);
    GDF_M_NULL_ASSERT(pKeySize);
    GDF_M_NULL_ASSERT(pConfAddress);
    GDF_M_NULL_ASSERT(pConfSize);
    GDF_M_NULL_ASSERT(pParamAddress);
    GDF_M_NULL_ASSERT(pParamfSize);

    *pKeyAddress = (INT8U *) & GUEST_V_StepperKeyMemoryArea[0];
    *pKeySize = sizeof(GUEST_V_StepperKeyMemoryArea);

    *pConfAddress = (INT8U *) & GUEST_V_ConfMemoryArea[0];
    *pConfSize = sizeof(GUEST_V_ConfMemoryArea);

    *pParamAddress = (INT8U *) & GUEST_V_ParamMemoryArea[0];
    *pParamfSize = sizeof(GUEST_V_ParamMemoryArea);
}

BOOLEAN GUEST_F_ConfigSubAreaCheck(void)
{
    return TRUE;
}

#pragma SCVR_CHECK_OFF 10
#pragma SCVR_CHECK_COMMENT "This name is exported by Kernel, no way to use another one"
void GUEST_F_On_SafetyFlagsInit(const BOOLEAN *pSafetyFlagsBrokenState, INT32U nbSafetyFlags)
#pragma SCVR_CHECK_ON 10
{
    GDF_M_NULL_ASSERT(pSafetyFlagsBrokenState);

    DTXT_REPORT(DTXT_REPORT_WARNING, DTXT_ANSI_FG_MAGENTA"Guest safety flags check");

#pragma PRQA_MESSAGES_OFF 3199
#pragma PRQA_MESSAGES_COMMENT "Useless assignment to avoid compilation warning"
    nbSafetyFlags = nbSafetyFlags;
#pragma PRQA_MESSAGES_ON 3199

    return;
}

/* CR_reference 1965 - Decide if the default can be raised in the OD */
/* Implements <STEP_SwAD_1598> */
BOOLEAN GUEST_F_On_MEVODefaultSet(MSTEPINC_T_FailureCode failureCode,
                                  const MSTEPINC_T_FailureContext* const pFailureContext)
{
    /* to avoid compilation warning */
    GDF_M_NULL_ASSERT(pFailureContext);
/*20180528, dynamic test, CR:723*/
#pragma PRQA_MESSAGES_OFF 3199
#pragma PRQA_MESSAGES_COMMENT "Useless assignment to avoid compilation warning"
    failureCode = failureCode;
#pragma PRQA_MESSAGES_ON 3199

    return TRUE;
}

/* CR_reference 2153 - Application Software Version hook. */
INT8U* GUEST_F_ApplicationSwVersionGet(void)
{
    return COM_C_ApplicationSwVersion;
}

/* Implements <STEP_SwAD_1405> */
INT32U GUEST_F_FaultFilteringTimerGet(void)
{
    INT32U timerFaultFiltering = 0x3cu;

    return timerFaultFiltering;
}
/*20180528, dynamic test, delete useless code, CR:723*/

BOOLEAN GUEST_F_Srtt_RxProcess(INT8U EqId, INT8U* pData, INT16U dataSize, INT16U* pOffset)
{
	return TRUE;
}

BOOLEAN GUEST_F_Srtt_TxProcess(INT8U EqId, INT8U* pData, INT16U buffSize, INT16U* pOffset)
{
	return TRUE;
}	

void GUEST_F_TxProcess(INT8U EqId, const INT8U* pData, INT16U dataSize)
{
	return;
}

INT32U GUEST_F_GetSrttTime(INT8U EqId)
{
    return 1500u;//normal 1500u
}

/*20200422, get network timeout from guest, CR:2081*/ 
INT32U GUEST_F_GetNetLapseTime(INT8U EqId)
{
	return 0x2710u;
}


/*20200629, add app interface to get PDO forgive cycle, CR:2240*/
INT8U GUEST_F_PDOForgiveCntGet(void)
{
 	return 9u;//normal 9u
}

INT32U GUEST_F_GetPrintLevel(void)
{	
	return 5u;
}
void GUEST_F_FSFB2_RxData(INT8U EqId, const INT8U* const pData, INT32U dataSize)
{
	//DTXT_REPORT6(DTXT_REPORT_WARNING, "GUEST_F_FSFB2_RxData",EqId,*pData,*(pData+1),*(pData+2),*(pData+3),dataSize);
}

void GUEST_F_FSFB2_TxData(INT8U EqId, const INT8U* const pData, INT32U dataSize)
{
	//DTXT_REPORT6(DTXT_REPORT_WARNING, "GUEST_F_FSFB2_TxData",EqId,*pData,*(pData+1),*(pData+2),*(pData+3),dataSize);
}

