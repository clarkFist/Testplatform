/*********************************************************************
 * COPYRIGHT (C) Alstom Transport 2008. STEP. All rights reserved.
 *********************************************************************/
/******************** FILE HEADER ************************************/
/**\addtogroup MSTEP_INTERFACE
 * \{*/
/**
 ******************************************************************************
 * \if API
 * \page step_constant STEP constants definition
 *
 * \section Description
 * This header is the constants definition shared with the kernel and the
 * guest application.
 *
 * \section Limitations
 *  - None
 *
 * \endif
 *****************************************************************************/
/**\}*/

/**\addtogroup MSTEP_INTERFACE
 * \{*/
#ifndef STEP_CONSTANT_H
#define STEP_CONSTANT_H

/**********************************************************************/
/*  MACROS DEFINITION                                                 */
/**********************************************************************/

/**********************************************************************/
/*  CONSTANTS OF MODULE                                               */
/**********************************************************************/

#define ALSTOM_C_NO_ERROR               0u /**<Base not error return, eq. to OS_NO_ERR */
#define ALSTOM_C_TIMEOUT               10u /**<General timetout error, eq. OS_TIMEOUT  */
#define ALSTOM_C_ERROR                201u /**<Base error return                       */
#define ALSTOM_C_NOT_RUNNING          202u /**<Call to function associated to a not
* running task                             */
#define ALSTOM_C_NOT_INIT             203u /**<Call to a non ititialised feature       */
#define ALSTOM_C_BAD_PARAMETER        204u /**<Function call with bad parameters       */
#define ALSTOM_C_NOT_IMPLEMENTED      205u /**<Error code for not implemented function */
#define ALSTOM_C_TOO_SMALL            206u /**<A given buffer is too small */
#define ALSTOM_C_CONFIG_ERROR         207u /**<Error during task configuration */

#define ALSTOM_C_BASE_IPSTACK         220u /**<Range until 225 is reserved for IPSTACK     */
#define ALSTOM_C_BASE_K2OO2_DRV       225u /**<Range until 229 is reserved for K2OO2_DRV   */
#define ALSTOM_C_BASE_IODB            230u /**<Range until 239 is reserved for IODB        */
#define ALSTOM_C_BASE_CONFIG          240u /**<Range until 244 is reserved for CONFIG      */
#define ALSTOM_C_FS_SRC               245u /**<Range until 249 is reserved for FS_SRC      */
#define ALSTOM_C_FS                   250u /**<Range until 259 is reserved for FS          */

/* CR_reference 1655 - MTN / Addition of the Sw version in the MIB */
/** Definition of the MSTEP Kernel version */
/*20180925, the print kernel version is wrong, CR:1139*/
/*20200228, reorganize code and add comments, CR:2006*/
/*20201209, solve pairing check problem and integrate new version of FSFB2, CR:2507*/
#define MSTEP_KERNEL_VERSION          "V1.1.5"

/* CR_reference 2153 - Application Software Version maximum size. */
/** Max size for Software version name */
#define APPLICATION_SW_VERSION_MAX_SIZE    15u

/** Definition of the software revision */
#ifdef QAC
#define ALSTOM_C_REVISION             "MSTEP-KERNEL_QAC"
#else
/*20181218, Modify the code to make it compatible with VOOB16_1, CR:1246*/
#define ALSTOM_C_REVISION             "VCU-KERNEL - "MSTEP_KERNEL_VERSION"  - "__DATE__"-"__TIME__" "
#endif

/**********************************************************************/
/*  TYPE DEFINITION                                                   */
/**********************************************************************/

/** Status Error code used for all the software stack.
 * The first 200 values are reserved for the OS. */
typedef unsigned char ALSTOM_T_Status;

/**
 * Task status information.
 * \par Description
 * Values of the Task status:
 */
typedef enum
{
    ALSTOM_C_TASK_NOTINIT     = 0x07u, /**<Task is in a not initialized state */
    ALSTOM_C_TASK_NOTRUNNING  = 0x19u, /**<Task is not running state */
    ALSTOM_C_TASK_STARTING    = 0x1Eu, /**<Task is in started state */
    ALSTOM_C_TASK_RUNNING     = 0x2Au, /**<Task is in running state */
    ALSTOM_C_TASK_ERROR       = 0x2Du, /**<Task is in Error state */
    ALSTOM_C_TASK_SUBSCRIBING = 0x33u  /**<Task is currently subscribing */
} ALSTOM_E_TaskStatus;

/** Information provided to task at creation through the pointer
 * passed by the OS function.
 *
 * This structure is a generic one, each task must provide a global
 * acces to an instance of it. Nobody except the MODE task is allowed
 * to modify it. The MODE task configure it, one time at init before
 * creating the tasks.
 */

typedef struct
{
    /** Synchro object used to unlock the task. */
    void                  *pSynchroObject;
    /** Input Message Queue FIFO of the task. */
    void                  *pFifoIn;
    /** Pointer to the statistic info of the task. Type is defined in the module header file itself. */
    void                  *pStatistic;
    /** Task status. Before task startup this field should be defined at  */
    ALSTOM_E_TaskStatus   TaskStatus;
} ALSTOM_TASK_T_TaskInfo;

/**Definition of the functional mode available on the board */
typedef enum
{
    ALSTOM_C_FUNCTIONAL_NORMAL = 0u,              /**< STEPPER board running in NORMAL mode */
    ALSTOM_C_FUNCTIONAL_STANDBY,                 /**< STEPPER board running in STANDBY mode */
    ALSTOM_C_FUNCTIONAL_GUEST,                   /**< GUEST board (non STEPPER) functional running mode */
    ALSTOM_C_FUNCTIONAL_NORMAL_RESTRICTIVE,      /**< Restrictive mode for NORMAL STEPPER board */
    ALSTOM_C_FUNCTIONAL_STANDBY_RESTRICTIVE,     /**< Restrictive mode for STANDBY STEPPER board */
    ALSTOM_C_FUNCTIONAL_GUEST_RESTRICTIVE,       /**< Restrictive mode for GUEST board */
    ALSTOM_C_FUNCTIONAL_BOOT_FAILED_RESTRICTIVE, /**< Boot-failed mode is reached when the Configuration Key is rejected.
                                                      It shall not be reached in GUEST or GUEST_RESTRICTIVE modes */
    /* CR_reference 1319 - mode REPROG */
    ALSTOM_C_FUNCTIONAL_REPROG,                  /**< mode REPROG when REPROG fuse is broken */
    ALSTOM_C_FUNCTIONAL_BOOT                     /**< Impossible case, defined for variable initialization purposes */
} ALSTOM_E_FunctionalMode;

/**********************************************************************/
/*  GLOBAL  VARIABLES OF MODULE                                       */
/**********************************************************************/

/**********************************************************************/
/*  FUNCTIONS PROTOTYPE                                               */
/**********************************************************************/

/**\}*/ /*End of MSTEP_INTERFACE*/

#endif /* _STEP_CONSTANT_H */
/**\}*/

/* EOF */
