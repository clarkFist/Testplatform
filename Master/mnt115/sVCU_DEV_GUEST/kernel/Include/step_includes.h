/******************************************************************************
 * COPYRIGHT (C) Alstom Transport 2008. STEP Project. All rights reserved.
 *****************************************************************************/

/******************** FILE HEADER *******************************************/
/**\addtogroup MSTEP_INTERFACE
 * \{*/
/**
 ******************************************************************************
 * \if API
 * \page StepIncludes Common include file for application and Kernel
 *
 * \section Description
 * Common types definition for kernel and application.
 *
 * \section Features
 * Features are:
 *  - Types definition for task management.
 *  - Cpu identification
 *
 * \section Limitations
 *  - Task configuration information with Key will be provided later.
 *
 * \endif
 *****************************************************************************/
/**\}*/

#ifndef _guest_includes_h_
#define _guest_includes_h_

/**\addtogroup MSTEP_INTERFACE
 * \{*/
/******************************************************************************
 * FILES TO INCLUDE
 *****************************************************************************/
 /*20201225, coverity test, CR:2511*/
#include "datatypes.h"
#include "step_constant.h"
#include "step_gpio.h"
#include "gdf_authorize.h"
/******************************************************************************
 * CONSTANTS OF MODULE
 *****************************************************************************/
/** Definition of the number of local maintenance base entry mapped in CAN for Stepper Maintenance polling */
#define MSTEPINC_C_MAINTENANCE_ENTRY_COUNT 2u
/** Maximum size of Maintenance data description string */
/* CR_reference 1914 - The MSTEPINC_C_MAINTENANCE_ENTRY_NAME_SIZE constant shall take into account the "LRUXX - " prefix. */
#define MSTEPINC_C_MAINTENANCE_ENTRY_NAME_SIZE (50u - 8u)

/** Size of the Failure Context string associated to Failure Code.
* 20 useful characters + NULL terminated string char */
/* Implements <STEP_SyID_MTN_0041> */
#define MSTEPINC_C_FAILURE_CONTEXT_SIZE 100u

/** Min failure code */
#define MSTEPINC_C_MIN_FAILURE_CODE 0x40u
/** Max failure code */
#define MSTEPINC_C_MAX_FAILURE_CODE 0x7fu

/** Maximum number of parameters sectors per module */
#define MSTEPINC_C_MAX_PARAM_SECTORS 3u

/** Parameter sector size in bytes */
#define MSTEPINC_C_PARAM_SECTOR_SIZE 0x10000u

/* CR_reference 2024 - triggered functions */
/** Maximum size in bytes of the received data of the triggered function */
#define MSTEPINC_C_MAX_TRIGGERED_DATA 0x40u

/******************************************************************************
 * MACROS DEFINITION
 *****************************************************************************/

/******************************************************************************
 *  TYPE DEFINITION
 *****************************************************************************/

/** Definition of CPU on which task is running */
typedef enum
{
    /** Task running information not defined */
    MSTEPINC_C_RUN_TASK_UNDEFINED = 0u,
    /** Task only running on CPU_A */
    MSTEPINC_C_RUN_TASK_ON_CPU_A,
    /** Task only running on CPU_B */
    MSTEPINC_C_RUN_TASK_ON_CPU_B,
    /** Task running on both CPU */
    MSTEPINC_C_RUN_TASK_ON_BOTH_CPU
} MSTEPINC_E_RunOnCpuInfo;


/** Definition of priority level used by the guest software. */
typedef enum
{
    MSTEPINC_C_TASK_PRIOLEVEL_HIGH = 0u,
    MSTEPINC_C_TASK_PRIOLEVEL_LOW
} MSTEPINC_E_PriorityLevel;

/** Definition of task priority used by the guest software. Ordering of task is done
 *  from the highest priority (MSTEPINC_C_TASK_PRIO_1) to the lowest (MSTEPINC_C_TASK_PRIO_10).
 *  Priorities are relative to the priority level (cf. MSTEPINC_E_PriorityLevel). */
typedef enum
{
    MSTEPINC_C_TASK_PRIO_1 = 0u,
    MSTEPINC_C_TASK_PRIO_2,
    MSTEPINC_C_TASK_PRIO_3,
    MSTEPINC_C_TASK_PRIO_4,
    MSTEPINC_C_TASK_PRIO_5,
    MSTEPINC_C_TASK_PRIO_6,
    MSTEPINC_C_TASK_PRIO_7,
    MSTEPINC_C_TASK_PRIO_8,
    MSTEPINC_C_TASK_PRIO_9,
    MSTEPINC_C_TASK_PRIO_10
} MSTEPINC_E_TaskPrio;

/** Definition of task ID used by the guest software. Each guest task must have a unique ID. */
typedef enum
{
    MSTEPINC_C_TASK_ID_1 = 0u,
    MSTEPINC_C_TASK_ID_2,
    MSTEPINC_C_TASK_ID_3,
    MSTEPINC_C_TASK_ID_4,
    MSTEPINC_C_TASK_ID_5,
    MSTEPINC_C_TASK_ID_6,
    MSTEPINC_C_TASK_ID_7,
    MSTEPINC_C_TASK_ID_8,
    MSTEPINC_C_TASK_ID_9,
    MSTEPINC_C_TASK_ID_10
} MSTEPINC_E_TaskID;

/** Definition of guest task order. Allow to define a starting order for each
 * guest tasks. Guest orders are relative to kernel. Guest tasks are started
 * after the last kernel task to be started */
typedef enum
{
    MSTEPINC_C_ORDER_1 = 0u,
    MSTEPINC_C_ORDER_2,
    MSTEPINC_C_ORDER_3,
    MSTEPINC_C_ORDER_4,
    MSTEPINC_C_ORDER_5,
    MSTEPINC_C_ORDER_6,
    MSTEPINC_C_ORDER_7,
    MSTEPINC_C_ORDER_8,
    MSTEPINC_C_ORDER_9,
    MSTEPINC_C_ORDER_10
} MSTEPINC_E_TaskOrder;

/** Type for the definition of the use of the LED associated with CAN and
 * Ethernet. If value is set to MSTEPINC_C_GPIO_NO_ID, no LED will be
 * associated with network. */

typedef struct
{
    MSTEPINC_E_GpioId canLed; /**<Id of the GPIO associated with CAN */
    MSTEPINC_E_GpioId ethLed; /**<Id of the GPIO associated with Ethernet */
} MSTEPINC_T_NetworkLed;

/** Definition of the functional mode of the technical platform
 * all kind of boards shall be connected to CAN */
typedef enum
{
    MSTEPINC_C_GUEST_NO_ETH  =   0u, /**<Board without any Ethernet access */
    MSTEPINC_C_GUEST_INTA    =   1u, /**<Board connected to Ethernet using
                                     * A-interface protocol */
    MSTEPINC_C_STEPPER       =   2u, /**<Board connected to UEVOL network, master
                                     * on CAN. See MSTEP SwRS for detail on this
                                     * mode */
    MSTEPINC_C_UNDEFINED     = 255u /**<Initialisation value, undefined mode
                                     * leads to safety action */
} MSTEPINC_E_BoardMode;

/** Functionnal mode of the application */
typedef ALSTOM_E_FunctionalMode MSTEPINC_E_FunctionalMode;

/** Task Creation information */

typedef struct
{
    INT32U   *pTaskStack;                   /**< Pointer on Start Address of Stack                  */
    INT32U   taskStackSize;                 /**< Task Stack Size                                    */
} MSTEPINC_T_TaskCreateInfo;

/**Structure of task configuration. All fields shall be filled in order to
 * have guest task to function */
/* Implements <STEP_SwAD_0315> */

typedef struct
{
    ALSTOM_T_Status(*taskCreate)(MSTEPINC_T_TaskCreateInfo* pCreateInfo);         /**< Pointer on Create Task Function */
    ALSTOM_T_Status(*taskConfigure)(void);                                        /**< Pointer on Configuration Task Function */
    ALSTOM_T_Status(*taskStart)(void);                                            /**< Pointer on Start task Function */
    ALSTOM_T_Status(*taskMainLoop)(MSTEPINC_E_FunctionalMode functionalMode);     /**< Pointer on Main Loop Task Function */
    MSTEPINC_E_TaskID        taskID;                                              /**< Task ID (must be unique) */
    MSTEPINC_E_PriorityLevel prioLevel;                                           /**< Task Priority level (Utility for Schedule) */
    MSTEPINC_E_TaskPrio      taskPrio;                                            /**< Task Priority in priority level (Utility for Schedule) */
    INT8U                    taskName[16];                                        /**< Task Name (For debug information) */
    MSTEPINC_E_TaskOrder     taskStartOrder;                                      /**< Start order of the task */
    INT16U                   wdgPeriod;                                           /**< Period to check that the task has set the watchdog point. In multiple of check cycle (5 ms) */
    INT16U                   taskPeriod;                                          /**< Period of the task in multiple of 50 ms. The value 0 defines a non-periodic task. */
    MSTEPINC_E_RunOnCpuInfo  cpuTask;                                             /**< Cpu on which the task is running */
    /** If TRUE the task has to notify to MSTEP Kernel that it has finished
        its transition from Normal to Standby state. It is needed if the
        task has to realize some specific action during this transition.
        In this case the kernel wait for the task to have finished its transition
        before reseting the board.
        If FALSE the task do not have to notify MSTEP Kernel. In this case the kernel do not
        wait for any notification of this task before reseting the board.
    */
    BOOLEAN bNotifyRestrictiveOk;
} MSTEPINC_T_TaskInfo;

/** Guest IO configuration information retrieved by the Kernel */

typedef struct
{
    const MSTEPINC_T_GpioDefinition*       guestGpioDefinition; /**<Pointer to guest Gpio configuration */
    const MSTEPINC_T_ChipSelectDefinition* guestCsDefinition;   /**<Pointer to Chip Select configuration */
} MSTEPINC_T_GuestIoSetup;

/*20181029, Modify the code to make it compatible with VOOB16_1, CR:1168*/
/** CPU identification */
typedef enum
{
    MSTEPINC_C_CPU_ID_ERROR = 0u, /**<Cpu Id error*/
    MSTEPINC_C_CPU_A        = 1u, /**<Cpu Id A*/
    MSTEPINC_C_CPU_B        = 2u  /**<Cpu Id B*/
} MSTEPINC_E_CpuId;

/** Definition of the type used to store a maintenance value */
typedef INT32U MSTEPINC_T_MaintenanceData;

/** Definition of the maintenance value description string */
typedef CHAR MSTEPINC_T_MaintenanceDataName[MSTEPINC_C_MAINTENANCE_ENTRY_NAME_SIZE];

/** Definition of one maintenance database entry */

typedef struct
{
    MSTEPINC_T_MaintenanceDataName *pEntryName; /**< Pointer on \ref MSTEPINC_C_MAINTENANCE_ENTRY_NAME_SIZE bytes long string defining
                                              * entry in name the maintenance base */
    MSTEPINC_T_MaintenanceData     *pEntryData; /**< Pointer on MSTEPINC_T_MaintenanceData bytes maintenance
                                              * entry value */
} MSTEPINC_T_MaintenanceEntry;

/** Definition of the maintenance database configuration table */
typedef MSTEPINC_T_MaintenanceEntry MSTEPINC_T_MaintenanceTable[MSTEPINC_C_MAINTENANCE_ENTRY_COUNT];

/** Definition of the Default Code recording type */
typedef INT8U MSTEPINC_T_FailureCode;

/** Definition of the Default Context can be recorded with Failure Code.
 * The string must be NULL terminated for correct display on Stepper Maintenance */
typedef INT8U MSTEPINC_T_FailureContext[MSTEPINC_C_FAILURE_CONTEXT_SIZE];

/** Statistics on messages received/accepted by redundancy layer */

typedef struct
{
    INT32U rejectedNormalCount;    /**<Number of frames received on Normal network rejected by A13 layer  */
    INT32U acceptedNormalCount;    /**<Number of frames accepted on Normal network rejected by A13 layer  */
    INT32U rejectedRedundantCount; /**<Number of frames received on Redundant network rejected by A13 layer */
    INT32U acceptedRedundantCount; /**<Number of frames accepted on Redundant network rejected by A13 layer */
} MSTEPINC_T_RedundancyStatistics;

/** User data for an applicative configuration key */

typedef struct
{
    INT32U ipNormal_1;                                                                   /**< IP address of communication on normal ethernet */
    INT32U ipNormal_2;
    INT32U ipRedundant_1;                                                                /**< IP address of communication on redundant ethernet (00.00.00.00 if none) */
    INT32U ipRedundant_2;
    INT32U subnetMaskNormal_1;                                                           /**< For ethernet communication (normal) */
    INT32U subnetMaskNormal_2;
    INT32U subnetMaskRedundant_1;                                                        /**< For ethernet communication (redundant) */
    INT32U subnetMaskRedundant_2;
    INT32U gtwAdressNormal_1;                                                            /**< Gateway address (normal) */
    INT32U gtwAdressNormal_2;
    INT32U gtwAdressRedundant_1;                                                         /**< Gateway address (redundant) */
    INT32U gtwAdressRedundant_2;
    INT16U timeToLive;                                                                 /**< Upper bound on the time that an IP datagram can exist in an internet system. Default value is 5 */
    INT32U snmpNormal;                                                                 /**< IP address of SNMP server on normal Ethernet */
    INT32U snmpRedundant;                                                              /**< IP address of SNMP server on redundant Ethernet */
    INT32U sntpNormal;                                                                 /**< IP address of SNTP server on normal Ethernet */
    INT32U sntpRedundant;                                                              /**< IP address of SNTP server on redundant Ethernet */
} MSTEPINC_T_UserData;

#ifdef ALSTOM_CONFIG_CPU_A
/** Configuration key status */
typedef enum
{
    MSTEPINC_C_CONF_KEY_VALID = 0u,        /**< Original Configuration key is plugged and valid. */
    MSTEPINC_C_CONF_KEY_MISSING = 1u,      /**< Configuration key is missing */
    MSTEPINC_C_CONF_KEY_CHANGED = 2u,      /**< Configuration key has changed or is corrupted */
    MSTEPINC_C_CONF_KEY_INVALID = 3u,      /**< Configuration key is invalid */
    MSTEPINC_C_CONF_KEY_UNDETERMINED = 4u  /**< Initialization state */
} MSTEPINC_E_ConfKeyStatus;
#endif

/** CAN bus status */
typedef enum
{
    MSTEPINC_C_CAN_BUS_OK           = 0u, /**< CAN bus is working OK */
    MSTEPINC_C_CAN_BUS_OUT_OF_ORDER = 1u  /**< CAN bus is out of order */
} MSTEPINC_E_CanBusStatus;

/** Configuration sector status */
typedef enum
{
    /** The sector is undefined */
    MSTEPINC_C_PARAM_SECTOR_UNDEFINED,
    /** The sector is OK and is now locked */
    MSTEPINC_C_PARAM_SECTOR_OK,
    /** Range check on the sector has failed */
    MSTEPINC_C_PARAM_SECTOR_NOK,
    /** The sector is released by the application code */
    MSTEPINC_C_PARAM_SECTOR_RELEASED

} MSTEPINC_E_ParamSectorState;

/** typedef for callback used to authorize or not a re-programmation request */
typedef BOOLEAN(*MSTEPINC_T_ReprogAutho)(void);

/* CR_reference 2024 - triggered functions */
/** typedef for triggered function using vivisection */
typedef void (*MSTEPINC_T_TriggeredFunction)(INT8U receivedDataLength);

/******************************************************************************
 *  GLOBAL VARIABLES OF MODULE
 *****************************************************************************/

/******************************************************************************
 *  API FUNCTION PROTOTYPE
 *****************************************************************************/

/**\}*/ /*End of MSTEP_INTERFACE*/

#endif /* _guest_includes_h_ */


/* EOF */

