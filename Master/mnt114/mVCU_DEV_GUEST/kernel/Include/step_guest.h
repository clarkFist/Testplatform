/******************************************************************************
 * COPYRIGHT (C) Alstom Transport 2009. STEP Project. All rights reserved.
 *****************************************************************************/

/******************** FILE HEADER *******************************************/
/**\addtogroup GUEST_MODULE
 * \{*/
/**\addtogroup GUEST_INTERFACE
 * \{*/
/**
 ******************************************************************************
 * \if API
 * \page GUEST module API
 *
 * \section Description
 * This module contains the whole definition of a guest application.
 *
 * \section Features
 * Features are:
 * - Definition of the GPIO available for the application.
 * - Definition of the chip select available, not used for this demo application.
 * - Guest board type definition: Non Ethernet capable board.
 * - Network led affectation: None defined for this application.
 * - Five guest tasks definition.
 * - Body definition of the base functions that are called by the kernel for
 *   configuration.
 * - Body definition of GDF and DTXT hook functions.
 *
 * \section V Global variables of the module
 *  - GUEST_V_Setup: Global variable used to give board setup to kernel
 *      - Protection: None
 *        - Written once by \ref GUEST_F_IoSetupGet during initialization
 *        - Only read by the kernel.
 *  - GUEST_V_TaskInfo: Global variable used to give tasks definition to
 *         the kernel.
 *        - Written once by \ref GUEST_F_SystemSetup during initialization
 *        - Only read by the kernel.
 *
 * \section Limitations
 *  - None
 *
 * \endif
 *****************************************************************************/
/**\}*/
/**\}*/

#ifndef _step_guest_h_
#define _step_guest_h_

#include "gdf.h"
#include "step_includes.h"

/**\addtogroup GUEST_INTERFACE
 * \{*/
/******************************************************************************
 * CONSTANTS OF MODULE
 *****************************************************************************/

/** Size of the memory area used to store physical configuration key on STEPPER board (5Mo) */
/* Implements <STEP_SwAD_0554> */
#define GUEST_C_PHYSICAL_KEY_MEMORY_AREA_SIZE     ((5242880u) + (3u * (64u * 1024u))) /* 5Mo + 3 param sectors for failed quickboot*/

/** Definition of one configuration data sector size */
#define GUEST_C_CONF_SECTOR_SIZE                  (64u * 1024u) /* 64 Ko */

/** Number of sectors used by kernel on a guest module to store configuration data */
#define GUEST_C_CONF_KERNEL_GUEST_SECTOR_NUMBER   (2u)
/** Number of sectors used by kernel on a stepper module to store configuration data.
  * One sector is added compared to guest modules so as to store configuration key global header */
#define GUEST_C_CONF_KERNEL_STEPPER_SECTOR_NUMBER (GUEST_C_CONF_KERNEL_GUEST_SECTOR_NUMBER + 1u)
/** Number of sectors used by kernel on a stepper module to store parameters global header */
#define GUEST_C_PARAM_KERNEL_STEPPER_SECTOR_NUMBER (1u)

/** Maximum size used by kernel on a guest module to store configuration data */
#define GUEST_C_CONF_KERNEL_GUEST_MEMORY_SIZE     (GUEST_C_CONF_KERNEL_GUEST_SECTOR_NUMBER * GUEST_C_CONF_SECTOR_SIZE)
/** Maximum size used by kernel on a stepper module to store configuration data */
#define GUEST_C_CONF_KERNEL_STEPPER_MEMORY_SIZE   (GUEST_C_CONF_KERNEL_STEPPER_SECTOR_NUMBER * GUEST_C_CONF_SECTOR_SIZE)
/** Size used by kernel on a stepper module to store parameters global header */
#define GUEST_C_PARAM_KERNEL_STEPPER_MEMORY_SIZE   (GUEST_C_PARAM_KERNEL_STEPPER_SECTOR_NUMBER * GUEST_C_CONF_SECTOR_SIZE)

/* CR_Reference 2677 - Increase number of flags for application needs */
/* CR_Reference 2676 - Increase number of memory parts for application needs */
/** Definition of the maximum OS events guarantee for the GUEST application          */
#define GUEST_C_NB_MAX_OS_SEMAPHORE 10u /**<Maximum number of semaphores available        */
#define GUEST_C_NB_MAX_OS_FLAGS     20u /**<Maximum number of flags available             */
#define GUEST_C_NB_MAX_OS_MSG_QUEUE 15u /**<Maximum number of message queues available    */
#define GUEST_C_NB_MAX_OS_MEM_PART  15u /**<Maximum number of memory partitions available */

/* Implements <STEP_SwAD_1258> */
#define GUEST_C_MIN_SAFETY_FLAG  4u /**<Minimum index of available safety flags */
#define GUEST_C_MAX_SAFETY_FLAG 19u /**<Maximum index of available safety flags */

#define GUEST_C_REPROG_ALLOWED  0u /**< Value to enable the re-programmation feature */

/* Available priority for IP over CAN communication */
/** Guest low priority value for IP over CAN transfer */
#define GUEST_C_LOW_PRIO               0x02u
/** Guest high priority value for IP over CAN transfer */
#define GUEST_C_HIGH_PRIO              0x04u

/******************************************************************************
 * MACROS DEFINITION
 *****************************************************************************/
/* Following macros shall be used for data allocation into DCA section:
 * - PRAGMA_PLACE_DCA_IDATA shall be used for initialized variables
 * - PRAGMA_PLACE_DCA_UDATA shall be used for uninitialized variables
 * DCA variables content can be modified till all tasks are started. Once all
 * tasks are started, DCA section is locked and Built In Self Test module is
 * then periodically verifying that no data change or corruption happened */
#ifdef QAC
/** No pragma for QAC */
#define PRAGMA_PLACE_DCA_IDATA
/** No pragma for QAC */
#define PRAGMA_PLACE_DCA_UDATA
#else
#ifdef __GNUC__
/** Following macro shall be used for initialized variables allocation into DCA section */
/* Implements <STEP_SwAD_0763> */
#define PRAGMA_PLACE_DCA_IDATA  __attribute__ ((section(".idata_dca")))
#else
#pragma explicit_zero_data on
#pragma define_section MyidataDca ".idata_dca" RWX
/** Following macro shall be used for initialized variables allocation into DCA section */
/* Implements <STEP_SwAD_0763> */
#define PRAGMA_PLACE_DCA_IDATA  __declspec (MyidataDca)
#endif

#ifdef __GNUC__
/** Following macro shall be used for uninitialized variables allocation into DCA section */
/* Implements <STEP_SwAD_0763> */
#define PRAGMA_PLACE_DCA_UDATA  __attribute__ ((section(".udata_dca")))
#else
#pragma define_section MyudataDca ".udata_dca" RWX
/** Following macro shall be used for uninitialized variables allocation into DCA section */
/* Implements <STEP_SwAD_0763> */
#define PRAGMA_PLACE_DCA_UDATA  __declspec (MyudataDca)
#endif
#endif

/** Next Macro shall be used to build IP address for IP over CAN communication.
 * The based IP address for IP over CAN protocol is 127."prio".0."node_id"
 * The Macro take into parameters both :
 *       - Node_ID of CAN receiver
 *       - Priority desired.
 *
 * Note : Node_ID = Module_ID * 2, for CPU_A
 *        Node_ID = (Module_ID * 2) + 1, for CPU_B
 *
 *        Priority for IP over CAN can be :
 *            - GUEST_C_LOW_PRIO
 *            - GUEST_C_HIGH_PRIO */

/* "macro used to build CAN Address including IP over CAN priority, not done by a function to avoid call" */
#define IP_M_BUILD_IPOVERCAN(node_id, prio) ((((INT32U)(127u))<<24u) | \
                                             (((INT32U)(prio)) << 16u) | \
                                             (((INT32U)(0u))<< 8u) | \
                                             ((INT32U)(node_id)))

/******************************************************************************
 *  TYPE DEFINITION
 *****************************************************************************/

/******************************************************************************
 *  GLOBAL VARIABLES OF MODULE
 *****************************************************************************/

/******************************************************************************
 *  API FUNCTION PROTOTYPE
 *****************************************************************************/

/**
 ******************************************************************************
 * \par Description:
 *      This function returns information needed for the MSTEP kernel.
 *      It is called by the MSTEP kernel in the starUp.
 *
 * \par Fault cases:
 *      - NULL parameters
 *      - GPIO for flashing purpose is not defined as output
 *
 * \par Global variable used:
 *      - COM_V_TaskInfo
 *      - COM_C_BoardMode
 *      - COM_C_NetworkLed
 *      - COM_V_MaintenanceTable
 *
 * \param pTaskInfoNumber - Out - Range: [!null]\n
 *      Pointer on the number of guest tasks.
 * \param pTaskInfo - Out - Range: [!null]\n
 *      Pointer on a task info structure pointer.
 * \param pBoardMode - Out - Range: [!null]\n
 *      Pointer on the definition of the board type.
 * \param pNetworkLed - Out - Range: [!null]\n
 *      Pointer on the definition of the network LED associated with the application.
 * \param pGPIOForFlashingInter - Out - Range: [!null]\n
 *      Pointer on the OUTPUT GPIO used for flashing mode INTER board:
 *      Flashing on CPU_A and CPU_B of the board, the synchronization is common to whole cubicle (low precision).
 *      This pointer shall be set to MSTEPINC_C_GPIO_NO_ID if flashing INTER board is not used.
 * \param pGPIOForFlashingIntra - Out - Range: [!null]\n
 *      Pointer on the OUTPUT GPIO used for flashing mode INTRA Board:
 *      Flashing on CPU_A and CPU_B of the board, the synchronization is only made between the 2 CPUs of the board (high precision).
 *      This pointer shall be set to MSTEPINC_C_GPIO_NO_ID if flashing INTRA Board is not used.
 *      Observation: The configured GPIO might be toggled for another reason before the "GUEST_F_IoSetupGet()" function is called,
 *      when they are effectively initialized.
 * \param pMaintenanceTable - Out - Range: [!null]\n
 *      Pointer on the application maintenance data.
 * \param pbForceBid - Out - Range: [!null] \n
 *      Pointer on the boolean defining if the BID is read on backplane or
 *      provided by guest application
 * \param pBidValue - Out - Range: [!null]\n
 *     Pointer on the BID value if provided by the guest application.
 * \param pForceKey - Out - Range: [none]\n
 *     Pointer on a structure that define user data when using an applicative key. If NULL, use of the SPI key.
 *     In case of using an applicative key pbForceBid shall be TRUE and pBidValue shall be forced to 0x01 on
 *     CPU_A and 0x41 on CPU_B
 * \param pReprogActivationPattern - Out - Range : [!null]\n
 *     Pointer to the parameter that Enable or not the re-programmation feature depending on its value.
 *     Setting  this parameter to GUEST_C_REPROG_ALLOWED (0) enable the re-programmation feature.
 *     If this parameter is different from GUEST_C_REPROG_ALLOWED, the re-programmation feature is not
 *     allowed and this parameter is used to compute the kernel System CheckWord.
 * \param pReprogAuthoCallback - Out - Range : [!null]\n
 *     Pointer to the callback that is called each time a re-programmation request is received.
 *     If re-programmation feature is not allowed (see previous parameter), no callback shall be defined
 *     If the re-programmation feature is allowed, this callback shall be defined and has to decide
 *     if the current re-programmation request is accepted or not.
 *
 * \if SwMD
 * \par Safety aspects:
 *      - No Safety aspects: Demo application.
 * \endif
 *
 * \note User shall complete the function with all the asked parameters.
 *****************************************************************************/
/* Implements <STEP_SwAD_0314> */
/* Implements <STEP_SwAD_0444> */
/* Implements <STEP_SwAD_0504> */
void GUEST_F_SystemSetup(INT8U                       *pTaskInfoNumber,
                         MSTEPINC_T_TaskInfo         **pTaskInfo,
                         MSTEPINC_E_BoardMode        *pBoardMode,
                         MSTEPINC_T_NetworkLed       *pNetworkLed,
                         MSTEPINC_E_GpioId           *pGPIOForFlashingInter,
                         MSTEPINC_E_GpioId           *pGPIOForFlashingIntra,
                         MSTEPINC_T_MaintenanceTable *pMaintenanceTable,
                         BOOLEAN                     *pbForceBid,
                         INT8U                       *pBidValue,
                         MSTEPINC_T_UserData         **pForceKey,
                         INT32U                      *pReprogActivationPattern,
                         MSTEPINC_T_ReprogAutho      *pReprogAuthoCallback);

/**
 ******************************************************************************
 * \par Description:
 * This function gives configuration elements to MSTEP application:
 *      - Address of the configuration table of all configurable GPIO.
 *      - Address of the configuration table of all configurable
 *        Chip select.
 * Observation: If the flashing functionality (INTRA board) is used, the
 * initial value of the GPIO, configured for this purpose, will determine
 * the moment of the first synchronization (once the first transition occurs).
 * Suggestion: For better performances, the selected GPIO should use
 * "MSTEPINC_C_GPIO_OFF" as initial value, since the first transition occurs
 * from OFF to ON. Otherwise, half cycle will be necessary for the first
 * synchronization.
 *
 * \par Fault cases:
 *      - None
 *
 * \par Global variable used:
 *      - GUEST_V_Setup
 *
 * \return MSTEPINC_T_GuestIoSetup
 *      Pointer to the structure containing the configuration elements of
 *      guest application.
 *
 * \note When this function is called by the Kernel the UART has not been
 *      initialized yet. Therefore the PRINT_Printf or PRINT_Report functions
 *      cannot be used.
 *
 *****************************************************************************/
/* Implements <STEP_SwAD_1493> */
MSTEPINC_T_GuestIoSetup* GUEST_F_IoSetupGet(void);



/**
 ******************************************************************************
 * \par Description:
 * This function provides the Application Software version to the MSTEP Kernel
 * software.
 *
 * \par Fault cases:
 *      - None
 *
 * \return INT8U* - Pointer to the Application Software version.
 *
 * \note The associated variable shall be filled at the "GUEST_F_SystemSetup"
 * function.
 *
 *****************************************************************************/
/* CR_reference 2153 - Application Software Version hook. */
/* Implements <STEP_SwAD_1664> */
INT8U* GUEST_F_ApplicationSwVersionGet(void);

/**
 ******************************************************************************
 * \par Description:
 * This function is called by the MSTEP Kernel when the software fuse is about to be broken.
 * This function purpose is to perform GUEST safety processing needed after failure and to choose
 * whether or not fault filtering mechanism shall be used.
 * Only the following function can be called :
 * - OSW_F_TickGet (also note that the value does not evolve anymore one in Safe state)
 * - MSTEP_F_TimeGet (also note that the value does not evolve anymore one in Safe state)
 * - MSTEP_F_GpioSetOn
 * - MSTEP_F_GpioSetOff
 * - MSTEP_F_GpioToggle
 * - MSTEP_F_GpioRead
 * - STD_F_Memcpy
 * - STD_F_Memset
 * - STD_F_Memcmp
 * Note: The action to be performed at this stage (in GUEST_F_On_GDFSafe) shall be shortest
 * as possible as SW_FUSE pattern is still not corrupted.
 *
 * \note this function is called after the kernel set itself in safety:
 *       ** Interrupts are locked
 *       ** Distant CPU is killed
 *       ** System checkword is set to 0
 *
 * \note Maintenance and information process shall be done in the hook GUEST_F_On_GDF
 *
 * \par Fault cases:
 *      - None
 *
 * \par Global variable used:
 *      - None
 * \param eventID - in - Range: none\n
 *        Identifier of the event
 * \param context - in - Range: none\n
 *        Context associated with the GDF
 * \param fileNameCrc - in - Range: none\n
 *        CRC computed on file name of the GDF caller
 * \param line - in - Range: none\n
 *        Line of the GDF caller
 * \param taskPrio - in - Range: none\n
 *        Prio of the task running when GDF occurs
 *
 * \return BOOLEAN - TRUE if the fault filtering mechanism shall be disabled (so SW_FUSE pattern
 *  will be broken), FALSE otherwise.
 *
 *****************************************************************************/
/* Implements <STEP_SwAD_0316> */
/* CR_reference 2977 Dynamically deactivate fault filtering*/
BOOLEAN GUEST_F_On_GDFSafe(GDF_T_EventId eventID, INT32U context, INT32U fileNameCrc, INT16U line, INT8U taskPrio);

/**
 ******************************************************************************
 * \par Description:
 * This function is called by the MSTEP Kernel as the Software fuse is broken.
 * This function purpose is to perform maintenance and informative process.
 * Only the following function can be called :
 * - OSW_F_TickGet (also note that the value does not evolve anymore one in Safe state)
 * - MSTEP_F_TimeGet (also note that the value does not evolve anymore one in Safe state)
 * - MSTEP_F_GpioSetOn
 * - MSTEP_F_GpioSetOff
 * - MSTEP_F_GpioToggle
 * - MSTEP_F_GpioRead
 * - STD_F_Memcpy
 * - STD_F_Memset
 * - STD_F_Memcmp
 * - PRINT_F_Printf
 * - STD_F_GenericPause
 * - STD_F_Snprintf
 * - STD_F_Strlen
 * - STD_F_Strncat
 * - STD_F_Strncpy
 * - STD_F_Strncmp
 * - STD_F_StrnLowerCaseSet
 *
 * \note GUEST application shall have set its output into safe state in the
 *       hook GUEST_F_On_GDFSafe which is called much earlier than this one.
 *
 * \par Fault cases:
 *      - None
 *
 * \par Global variable used:
 *      - None
 *
 * \param eventID - in - Range: none\n
 *        Identifier of the event
 * \param context - in - Range: none\n
 *        Context associated with the GDF
 * \param fileNameCrc - in - Range: none\n
 *        CRC computed on file name of the GDF caller
 * \param line - in - Range: none\n
 *        Line of the GDF caller
 * \param taskPrio - in - Range: none\n
 *        Prio of the task running when GDF occurs
 *
 * \return BOOLEAN - TRUE if the memory dump can override the software in
 *          flash memory, FALSE if it cannot.
 *
 * \note The whole process of memory dump will last about one minute
 *****************************************************************************/
/* Implements <STEP_SwAD_0316> */
BOOLEAN GUEST_F_On_GDFEnding(GDF_T_EventId eventID, INT32U context, INT32U fileNameCrc, INT16U line, INT8U taskPrio);

/**
 ******************************************************************************
 * \par Description:
 * This function is called by the MSTEP Kernel every time a character string
 * is emitted on the physical Debug UART of the target CPU. It is then triggerd
 * by calls to PRINT_F_Printf() function and indirectly, on CPU_A only,
 * by calls to PRINT_F_Report() function. Thus, a PRINT_F_Report() call on CPU_B
 * will trigger this hook on CPU_A.
 * To be used to receive strings of printf and process them locally.
 *
 * \par Fault cases:
 *      EVT_C_NULL_POINTER if pChain is NULL pointer.
 *
 * \par Global variable used:
 *      - None
 *
 * \param pChain - in - Range: [!null]\n
 *        Pointer on DTXT string
 * \param size - in - Range: none\n
 *        Size of DTXT string
 *
 * \par limitation
 *      Function must return quickly
 *****************************************************************************/
/* Implements <STEP_SwAD_0316> */
void GUEST_F_On_Printf(const char *pChain, INT16U size);

/**
 ******************************************************************************
 * \par Description:
 *  This function is called by the MSTEP Kernel to retrieve:
 *      ** a pointer on a memory zone allocated by application, along with its size. This
 *         memory space is used to store the global key read from SPI bus by
 *         stepper board. In case of non-stepper application, this function
 *         returns a NULL pointer and a size set to 0.
 *      ** a pointer on a memory zone allocated by application, along with its size. This
 *         memory space is used to store configuration data relative to the module.
 *         This memory are will be used to store configuration sector of the module used
 *         by the application and by the kernel. A stepper board need a size of
 *         GUEST_C_CONF_KERNEL_STEPPER_MEMORY_SIZE bytes by module. A non-stepper board
 *         need a size of GUEST_C_CONF_KERNEL_GUEST_MEMORY_SIZE bytes by module.
 *         This area has to be placed in a DCA protected zone by a PRAGMA_PLACE_DCA_UDATA
 *         instruction. DCA section is periodically checked by kernel so as to detect
 *         any data corruption.
 *      ** a pointer on a memory zone allocated by application, along with its size. This
 *         memory space is used to store parameters data relative to the module.
 *         This zone is used in double-buffering. It means that the given buffer shall
 *         be able to store two set of parameters data). In the case of a stepper
 *         board this memory zone shall have an additional size of
 *         GUEST_C_PARAM_KERNEL_STEPPER_MEMORY_SIZE bytes.
 *         So the memory zone shall have the following size:
 *           - Non-stepper board:
 *             2 * (nbsector used by module * GUEST_C_CONF_SECTOR_SIZE)
 *           - Stepper board:
 *             (2 * (nbsector used by module * GUEST_C_CONF_SECTOR_SIZE)) + GUEST_C_PARAM_KERNEL_STEPPER_MEMORY_SIZE
 *         This area shall not be placed in a DCA protected zone
 *         as a re-parametrisation can change its content.
 *
 * \note Memory space given to store key and DCA shall be aligned on 32 bits
 *
 * \param pKeyAddress - Out - Range: [] \n
 *   Reserved RAM area start address if stepper, NULL otherwise
 *
 * \param pKeySize - Out - Range: [] \n
 *   Reserved RAM area size if stepper, NULL otherwise
 *
 * \param pConfAddress - Out - Range: [!NULL] \n
 *        Start address of the whole (guest+kernel) configuration data
 *
 * \param pConfSize - Out - Range: [!NULL] \n
 *        Size of the whole (guest+kernel) configuration data
 *
 * \param pParamAddress - Out - Range: [!NULL] \n
 *        Start address of the whole (guest) parameter data
 *
 * \param pParamfSize - Out - Range: [!NULL] \n
 *        Size of the whole (guest) parameter data
 *
 *****************************************************************************/
/* Implements <STEP_SwAD_0317> */
void GUEST_F_AllocateMemorySpaceForKernel(INT8U **pKeyAddress,    INT32U *pKeySize,
                                          INT8U **pConfAddress,   INT32U *pConfSize,
                                          INT8U **pParamAddress, INT32U *pParamfSize);

/**
 ******************************************************************************
 * \par Description:
 *      This function is called by MSTEP Kernel to ask to guest application to
 *      check the application data in configuration. The function has to return
 *      TRUE if the configuration is OK, and FALSE if it is not. The function has to return
 *      the same value on each CPU, else software will enter in safe state.
 *
 * Boolean returned is processed as follow by kernel:
 *      ** If both CPU return TRUE, initialization continue
 *      ** If both CPU return FALSE, software is set in BOOT_FAILED mode
 *      ** If value returned by CPU are different, software is set in SAFE_STATE
 *
 * \par Fault cases:
 *      None
 *
 * \return BOOLEAN - TRUE if the configuration is OK, FALSE otherwise.
 *
 * \note The configuration area corresponding to guest is retrieved
 *       with MSTEP_F_ConfigGuestSubAreaGet() function.
 *
 * \note If there is no need to check application configuration data, boolean
 *       value returned shall be set to TRUE on both CPU.
 *
 * \see MSTEP_F_ConfigGuestSubAreaGet()
 *****************************************************************************/
/* Implements <STEP_SwAD_0643> */
BOOLEAN GUEST_F_ConfigSubAreaCheck(void);


/**
 ******************************************************************************
 * \par Description:
 *      Function called by MODE to notify guest application that configuration
 *      key has not been retrieve correctly. It is called boot failed state.
 *
 * \par Fault cases:
 *      None
 *
 * \par Pseudo-code:
 *      Switch Off Ethernet and CAN Led
 *
 *
 *****************************************************************************/
void GUEST_F_On_BootFailed(void);

/* Notify fuses state to GUEST */
/**
 ******************************************************************************
 * \par Description:
 *      Function called by BIT_FUSE during BIT_F_Config to notify guest
 *      application of the state (broken or not) of each safety flag.
 *
 * \par Fault cases:
 *      None
 *
 * \param pSafetyFlagsBrokenState - in - Range: none\n
 *        array of safety flags states. TRUE if broken, FALSE if not broken
 *
 * \param nbSafetyFlags - in - Range: none\n
 *        number of safety flags states in the array.
 *
 *****************************************************************************/
/* Implements <STEP_SwAD_0316> */
void GUEST_F_On_SafetyFlagsInit(const BOOLEAN *pSafetyFlagsBrokenState, INT32U nbSafetyFlags);


/* Initilialise fault filtering timer value */
/**
 ******************************************************************************
 * \par Description:
 *
 *      The Fault Filtering function allows to confirm one or several time a
 *      fault before to enter safe state.
 *
 *      Fault filtering condition:
 *          -# FAULT_FILTERING_TIMER is elapsed (timer start at INIT to NORMAL
 *                                              transition)
 *          -# AND Fault filtering feature has been enabled through
 *             Configuration key
 *
 *      In continuous presence of fault, a reboot of the system is triggered
 *      If Fault Filtering condition is reached, system restarts without
 *      erasing software fuse.
 *
 *
 *      This function returns the FAULT_FILTERING_TIMER
 *
 * \par Fault cases:
 *      None
 *
 * \return INT32U
 *         Timer value, in minutes - Range [0 / (STD_C_INT32U_MAX_VAL /
 *                                             (60u * OSW_C_TICKS_PER_SEC)]
 *
 *****************************************************************************/
/* Implements <STEP_SwAD_1405> */
INT32U GUEST_F_FaultFilteringTimerGet(void);

/**
 ******************************************************************************
 * \par Description:
 *      Function called by MEVO during FCODE_F_FailureCodeSetExt to notify guest
 *      application that a fault is raised. The function has to return
 *      TRUE if the Failure can be recorded in object dictionary, and FALSE
 *      if it can't.
 *
 * \par Fault cases:
 *      None
 *
 * \param failureCode - In - Range: [0..MSTEPINC_C_MAX_FAILURE_CODE] \n
 *           failure code to log
 * \param pFailureContext - In - Range: [!NULL] \n
 *           default context linked to to the failure code
 *
 *
 * \return BOOLEAN - TRUE: Failure code can be recorded, FALSE otherwise.
 *
 *****************************************************************************/
/* Implements <STEP_SwAD_1598> */
BOOLEAN GUEST_F_On_MEVODefaultSet(MSTEPINC_T_FailureCode failureCode,
                                  const MSTEPINC_T_FailureContext* const pFailureContext);
/*20190718, add new API functions and update some API functions, CR:1633*/
BOOLEAN GUEST_F_Srtt_RxProcess(INT8U EqId, INT8U* pData, INT16U dataSize, INT16U* pOffset);

BOOLEAN GUEST_F_Srtt_TxProcess(INT8U EqId, INT8U* pData, INT16U buffSize, INT16U* pOffset);

void GUEST_F_TxProcess(INT8U EqId, const INT8U* pData, INT16U dataSize);

INT32U GUEST_F_GetSrttTime(INT8U EqId);

/*20210803, add APIs to get safe input/output value, CR:2921*/
/**
 ******************************************************************************
 * \par Description:
 *      Function called by FSFB2_MGT when set SDB value to notify guest
 *      application the safe session input value and size. the value can not be modified.
 *
 * \par Fault cases:
 *      None
 *
 * \param EqId - In - Range: [0,5] \n
 *           session Id
 * \param pData - In - Range: [!NULL] \n
 *           input value, the value can not be modified.
 * \param dataSize - In - Range: [none] \n
 *           input value size
 *
 * \return viod.
 *
 *****************************************************************************/
void GUEST_F_FSFB2_RxData(INT8U EqId, const INT8U* const pData, INT32U dataSize);

/**
 ******************************************************************************
 * \par Description:
 *      Function called by FSFB2_TXP when get SDB value to notify guest
 *      application the safe session output value and size. the value can not be modified.
 *
 * \par Fault cases:
 *      None
 *
 * \param EqId - In - Range: [0,5] \n
 *           session Id
 * \param pData - In - Range: [!NULL] \n
 *           output value, the value can not be modified.
 * \param dataSize - In - Range: [none] \n
 *           output value size
 *
 * \return viod.
 *
 *****************************************************************************/

void GUEST_F_FSFB2_TxData(INT8U EqId, const INT8U* const pData, INT32U dataSize);

/*20200422, get network timeout from guest, CR:2081*/
/**
 ******************************************************************************
 * \par Description:
 *      get the timer's laspe time by EqId,when the time is reached,
 *      net communication layer status shall be setted disconnet,
 *      external device status shall be setted unknown.
 *
 * \par Fault cases:
 *      None
 *
 * \param EqId - In - Range: [0,5] \n
 *        equipment number (session number)
 * \return INT32U:lapse time of the timer

 *****************************************************************************/
/*  */
INT32U GUEST_F_GetNetLapseTime(INT8U EqId);


/*20200629, add app interface to get PDO forgive cycle, CR:2240*/
/**
 ******************************************************************************
 * \par Description:
 *      get PDO Forgive Count from guest, Lifetime of PDO data is 50ms* PDOForgiveCnt.
 *
 * \par Fault cases:
 *      None
 *
 * \return INT8U:PDO Forgive Count
 *
 *****************************************************************************/
INT8U GUEST_F_PDOForgiveCntGet(void);

/*20210302,Add maintenance information,CR:2616*/
/**
 ******************************************************************************
 * \par Description:
 *      get Print Level from guest, 
 *      Print Level 0 means trace NOTHING;
 *      Print Level 1 means trace CRITICAL;
 *      Print Level 2 means trace CRITICAL/ERROR;
 *      Print Level 3 means trace CRITICAL/ERROR/WARNING;
 *      Print Level 4 means trace CRITICAL/ERROR/WARNING/INFO;
 *      Print Level 5 means trace CRITICAL/ERROR/WARNING/INFO/INTEGRATION;
 *
 * \par Fault cases:
 *      None
 *
 * \return INT32U:Print Level
 *
 *****************************************************************************/
INT32U GUEST_F_GetPrintLevel(void);
/**\}*/ /*End of GUEST_INTERFACE*/

#endif /* _step_guest_h_ */

/* EOF */

