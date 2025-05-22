/******************************************************************************
 * COPYRIGHT (C) Alstom Transport 2008. STEP Project. All rights reserved.
 *****************************************************************************/

/******************** FILE HEADER *******************************************/
/**\addtogroup GDF_MODULE
 * \{*/
/**\addtogroup GDF_INTERFACE
 * \{*/
/**
 ******************************************************************************
 * \if API
 * \mainpage GDF module API
 *
 * \section Description
 * This module provides the mechanism to log a default. 63 id are available to
 * guest application, they are defined in guest_default.h. Each id has to be
 * associated to actions listed in this document.
 *
 * \section Features
 * Features are:
 *  - Panic assert definition:
 *      - Log a default if null pointer is detected: \ref GDF_M_NULL_ASSERT
 *      - Log a default if range error is detected: \ref GDF_M_RANGE_ASSERT
 *  - GDF mechanism to stop the software in safety and log information:
 *    \ref GDF_M_FAULT_EXT
 *      - Lock interrupt and scheduling
 *      - Call Software Fuse (break pattern, switch on the red Led, isolate
 *      Ethernet medium,
 *      - Log in persistent memory (no need to flush action is always performed,
 *      - Log on RS232.
 *      - Dump into persistent memory the content of ram variables.
 *  - GDF mechanism to log an event: \ref GDF_M_LOG
 *      - Post to task to record the event information.
 *      \note: If a \ref GDF_M_FAULT_EXT is called before the task record the
 *      event it will not be available. But stored in dump.
 *
 * \section V_Global Global variables of the module
 *  - GDF_V_TabDefActionSTEP : no possible conflict as this global variable is
 *      only read accessed.
 *  - GDF_DUMP_TOOLS_V_CurrentAddrInFlash: variable uses while dumping memory when
 *      software fuse is broken. In this case, the OS is stopped and interrupt are
 *      disabled, so no concurrent access can occur.
 *  - GDF_DUMP_TOOLS_V_bModeBeforeSafetStateDefined: variable uses while dumping memory when
 *      software fuse is broken. In this case, the OS is stopped and interrupt are
 *      disabled, so no concurrent access can occur.
 *  - GDF_DUMP_TOOLS_V_BeforeSafeStateMode: variable uses while dumping memory when
 *      software fuse is broken. In this case, the OS is stopped and interrupt are
 *      disabled, so no concurrent access can occur.
 *  - GDF_V_LastEvent: the variable is set while interrupt are masked,
 *      and it is only used for maintenance purpose, it has no
 *      functional use.
 *
 * \section lim Limitations
 * - Only 63 event id are available to Guest application and shall be numbered
 *   from GDF_C_FIRST_GUEST_DEFAULT_ID to 254.
 * - In case of terminal GDF, events FIFO is not emptied, so the log of all
 *   informative events is not guarantee in that case.
 * - In case of LOG (non terminal GDF), no more than a log per minute is
 *   assured by the Kernel. If too much log is entered (FIFO of module filled a
 *   SAFE_STATE will be triggered.
 *
 * \endif
 *****************************************************************************/
/**\}*/
/**\}*/

#ifndef GDF_H
#define GDF_H

#include "step_default.h"
#include "osw.h"

/**\addtogroup GDF_INTERFACE
 * \{*/
/******************************************************************************
 * INCLUDES
 *****************************************************************************/

/**********************************************************************/
/*  CONSTANTS OF MODULE                                               */
/**********************************************************************/
/** Error caused by a null pointer in function's parameters, this event ID must
 * be equal to 13 */
/* "This specific error id is better named with the same prefix as other error id" */
#define EVT_C_NULL_PTR_ERROR       (GDF_T_EventId)13u

/** Error caused by a range of function's parameters, this event ID must be
 * equal to 14 */
/* "This specific error id is better named with the same prefix as other error id" */
#define EVT_C_OUT_OF_RANGE_ERROR   (GDF_T_EventId)14u

/** Flag to activate call to software fuse*/
#define GDF_C_SW_FUSE              (TRUE)
/** Flag to activate log in NVMEM*/
#define GDF_C_LOG_NVMEM            (TRUE)
/** Flag to deactivate an action*/
#define GDF_C_NONE                 (FALSE)
/** Depth of the stack to dump on serial link */
#define GDF_C_DUMP_STACK_SIZE        120u
/** Definition of the event Id of the first Guest default */
#define GDF_C_FIRST_GUEST_DEFAULT_ID 192u
/** Software fuse 1 address */
#define GDF_C_SOFT_FUSE_FLASH_1_ADDR        0x480000u//0x3C0000u GJC 20170918
/** Software fuse 2 address */
#define GDF_C_SOFT_FUSE_FLASH_2_ADDR        0x4A0000u//0x3D0000u modified by SXR,20161108,0x3E0000u GJC 20170918
/** Software fuse toggle Flag Address */
#define GDF_C_SOFT_FUSE_FLASH_TOGGLE_ADDR   0x4C0000u//0x3E0000u modified by SXR,20161108,0x400000u GJC 20170918
/** Memory offset of the safety flags */
/* CR_Reference 1319: safety flags are written in memory using an offset to
 * avoid being overwritten by the dump allocation table */
#define GDF_C_AUX_MEMORY_OFFSET 0xFF00u

/* CR_reference 1486 - Fault filtering feature :
 * Fault filtering informations are store with
 * the global dump informations. Store these informations in SPI Flash because
 * in Parallel flash, these informations will be erased by BIST module */
/** Sector of memory dump global information in SPI FLASH */
#define GDF_C_ADDR_DUMP_INFO    0x00000u
/** size of SPI flash sector */
#define GDF_C_DUMP_INFO_SIZE    0x10000u

/** Maximum number of memory part which can be recorded */
#define GDF_C_NB_MAX_DUMP_PART       4u

/** Address in SPI FLAH where the information of reboot following fault filtering are store
 * Don't store these informations jest after the dump part info. The extract dump external
 * tools, need to have a 0xFF value to know that there is no more dump information to extract */
#define GDF_C_ADDR_SPI_DUMP_INFO_FAULT_FILTERING_REBOOT (GDF_C_ADDR_DUMP_INFO + 2u + \
                                                         ((GDF_C_NB_MAX_DUMP_PART + 1u) * sizeof(NVMEM_T_DumpPartInfo)))

/******************************************************************************
 * MACROS DEFINITION
 *****************************************************************************/
#ifndef GDF_UNAUTHORIZE_C_UNAUTHO
/*20201225, coverity test, CR:2511*/
//#include "gdf_authorize.h"
#endif

/* "GDF macro used to parse argument to GDF function keeping caller's line number" */
#ifdef QAC
/* Implements <STEP_SwAD_1105> */
/** QAC stub, as __FILE__ macro is not handled properly by QAC */
#define GDF_M_LOG(A,B, exp) (GDF_F_LogInternal(A, B, "", (INT16U)(__LINE__), OSW_F_TaskPrioGet(), NULL))
/* CR_Reference 2832:  Implement a notification mechanism for NVMEM log event */
/** QAC stub, as __FILE__ macro is not handled properly by QAC */
#define GDF_M_LOG_NOTIF(A,B, exp, C) (GDF_F_LogInternal(A, B, "", (INT16U)(__LINE__), OSW_F_TaskPrioGet(), C))
#else
/* Implements <STEP_SwAD_1105> */
/** Addition to the event of the file name, line number and task Id*/
#define GDF_M_LOG(A,B, exp) (GDF_F_LogInternal((GDF_T_EventId)(A), (INT32U)(B), (const char *)(__FILE__), (INT16U)(__LINE__), OSW_F_TaskPrioGet(), (GDF_T_EventInfoNotif)(NULL)))
/* CR_Reference 2832:  Implement a notification mechanism for NVMEM log event */
/** Addition to the event of the file name, line number, task Id and Callback function called at the end of event write */
#define GDF_M_LOG_NOTIF(A,B, exp, C) (GDF_F_LogInternal((GDF_T_EventId)(A), (INT32U)(B), (const char *)(__FILE__), (INT16U)(__LINE__), OSW_F_TaskPrioGet(), (GDF_T_EventInfoNotif)(C)))
#endif /* (QAC == 1) */


/** Macro to cast context into INT32U*/
/* "GDF macro only used to cast the context given in argument to fault macro" */
#define GDF_M_CONTEXT(var)  ((INT32U)(var))

/** Dump the current processor stack. \sa GDF_F_DumpStack() */
#define GDF_M_DUMP_STACK()      (GDF_F_DumpStack((OS_STK *)(NULL), (INT32U)(GDF_C_DUMP_STACK_SIZE)))

/* Implements <STEP_SwAD_0557> */
/** Macro used to toggle the interrupt line used to kill the distant CPU */
#define GDF_M_TOGGLE_XIRQ()          do                                                           \
    {                                                                                    \
        if ((MCF_GPIO_PODR_D & MCF_GPIO_PODR_D_PODR_D0) == MCF_GPIO_PODR_D_PODR_D0)      \
        {                                                                                \
            MCF_GPIO_PODR_D &= ~MCF_GPIO_PODR_D_PODR_D0;                          \
        }                                                                                \
        else                                                                             \
        {                                                                                \
            MCF_GPIO_PODR_D |= MCF_GPIO_PODR_D_PODR_D0;                                  \
        }                                                                                \
    }                                                                                    \
    while (0)

/******************************************************************************
 *  TYPE DEFINITION
 *****************************************************************************/
/** Action fault descriptor*/

typedef struct
{
    BOOLEAN SwFuse;     /**<Corruption of the software fuse pattern, Switch on
                         * the red Led, isolation of CAN and Ethernet medium */
    BOOLEAN logNvmem;   /**<Log event into non volatile memory      */
} GDF_T_Action ;

/** Event Id descriptor*/
typedef INT8U GDF_T_EventId;

/** Guest default descriptor:
 *  This structure is used to describe behavior of a default.
 *  It is also used to specify action to do when fault trig.
 */

typedef struct
{
    GDF_T_EventId EventId; /**<code to identify fault in case of log action */
} GDF_T_Default;

/** Identifier of Fuse addresses */
/*20210803, change fuse check method to use one big group, CR:2924*/
typedef enum
{
    /** Fuse sector 1 */
    GDF_C_FUSE_ADDR_ID_1,
    /** Fuse sector 2 */
    GDF_C_FUSE_ADDR_ID_2,
    /** Fuse toggle sector */
    GDF_C_FUSE_ADDR_ID_TOGGLE,
    /** Unknown Id */
    GDF_C_FUSE_ADDR_ID_UNKNOWN
} GDF_E_FuseAddrId;

/** Typedef used for the definition of callback when an event is log using the GDF_M_LOG_NOTIF maccro */
typedef void (*GDF_T_EventInfoNotif)(void);

/******************************************************************************
 *  GLOBAL VARIABLES OF MODULE
 *****************************************************************************/

/******************************************************************************
 *  FUNCTIONS PROTOTYPE
 *****************************************************************************/

/**
 ******************************************************************************
 * \par Description:
 *      This function is the core of the GDF process. It allows to log an event
 *      Id associated to 32 bits of context, the file, line and task ID where the
 *      log appears.
 *      If error occurs during this function runs, software fuse is launched. \n
 *      This function:
 *          -# Locks ITs,
 *          -# Checks the origin of the default (STEP or Guest),
 *          -# Checks that eventId logged fit with the content of default tables,
 *          -# Executes actions joined to the specified event Id.
 *
 *      CAUTION: This function shall NOT be called directly by the developper.
 *      Only call to \ref GDF_M_FAULT_EXT is allowed.
 *
 * \par Fault cases:
 *   - eventID not found in guest or step tables.
 *   - eventID does not match eventID of guest or step tables.
 *
 * \param eventID  - In - Range: None \n
 *        Id of the event to log.
 * \param context - In - Range: None                      \n
 *        Free 32 bits at developper disposal.
 * \param pFileName - In - Range: None                    \n
 *        File from where the log was called.
 * \param line - In - Range: None                         \n
 *        Line where the log was called.
 * \param taskPrio - In - Range: None                     \n
 *        Id of the task which log the event.
 *
 * \return void
 *
 * \if SwMD
 * \par Safety Aspects:
 *      - Function must break fuse (and thus stop kernel) is requested action.
 * \endif
 *
 * \note
 *
 * \see MSTEP_INTERFACE for list of event
 *****************************************************************************/
void GDF_F_FaultInternal(GDF_T_EventId eventID, INT32U context, const char *pFileName, INT16U line, INT8U taskPrio);


/* CR_Reference 2832:  Implement a notification mechanism for NVMEM log event.
 * Add callBackNotif optional parameter (can be NULL) */
/**
 ******************************************************************************
 * \par Description:
 *      This function logs evet ID in the NVMEM memory
 *          -# Checks the origin of the default (STEP or Guest),
 *          -# Checks that eventId logged fit with the content of default tables,
 *
 *      CAUTION: This function shall NOT be called directly by the developper.
 *      Only call to \ref GDF_M_LOG is allowed.
 *
 *      Limitation: No more than a log per minute is assured by the Kernel.
 *
 * \par Fault cases:
 *   - none
 *
 * \param eventID  - In - Range: None \n
 *        Id of the event to log.
 * \param context - In - Range: None                      \n
 *        Free 32 bits at developer disposal.
 * \param pFileName - In - Range: None                    \n
 *        File from where the log was called.
 * \param line - In - Range: None                         \n
 *        Line where the log was called.
 * \param taskPrio - In - Range: None                     \n
 *        Id of the task which log the event.
 * \param callBackNotif - In - Range: None                \n
 *        Notification Callback which be call at the end of
 *        NVMEM event log. If this parameter is NULL, no
 *        callBack call is done.
 * \return void
 *
 * \if SwMD
 * \par Safety Aspects:
 *      - none.
 * \endif
 *
 * \note
 *
 * \see MSTEP_INTERFACE for list of event
 *****************************************************************************/
void GDF_F_LogInternal(GDF_T_EventId eventID, INT32U context, const char *pFileName, INT16U line, INT8U taskPrio, GDF_T_EventInfoNotif callBackNotif);

/**
 ******************************************************************************
 * \par Description:
 *      Dump the current processor stack.
 *
 * \par Fault cases:
 *      None
 *
 * \par Safety Aspects:
 *      None
 *
 * \param size - In - Range: [!0] \n
 *       Number of stack element to dump.
 *
 * \param pStk - In - Range: None \n
 *       Stack pointer to dump. If its value is NULL, the current stack is
 *       dumped.
 *
 * \note This function uses STD_F_Printf. So it can have a significant impact.
 *       on realtime constraints. It must be taken into account when this
 *       function is used.
 *
 * \see GDF_M_DUMP_STACK()
 *****************************************************************************/
void GDF_F_DumpStack(OS_STK * pStk, INT32U stkSize);

/**
 ******************************************************************************
 * \par Description:
 *      Write FUSE with data given in parameter.
 *
 * \par Fault cases:
 *      - None
 *
 * \param fuseAddrId - In - Range: None \n
 *              Id of fuse address to be written.
 *
 * \param startIndex - In - Range: None \n
 *              Index of first fuse to write
 *
 * \param pFusesValues - In - Range: None \n
 *              Values of fuses
 *
 * \param nbFuses - In - Range: None \n
 *              Number of fuses to write
 *
 * \return ALSTOM_T_Status - ALSTOM_C_NO_ERROR if data have been writen successfully,
 *      ALSTOM_C_ERROR otherwise
 *
 *****************************************************************************/
ALSTOM_T_Status GDF_F_FusesWrite(GDF_E_FuseAddrId fuseAddrId, INT32U startIndex, INT32U *pFusesValues, INT32U nbFuses);

/**\}*/ /*End of GDF_INTERFACE */

#endif /* GDF_H */


/* EOF */
