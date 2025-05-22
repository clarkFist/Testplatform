/******************************************************************************
 * COPYRIGHT (C) Alstom Transport 2008. STEP Project. All rights reserved.
 *****************************************************************************/

/******************** FILE HEADER *******************************************/
/**\addtogroup OSW_MODULE
 * \{*/
/**\addtogroup OSW_INTERFACE
 * \{*/
/**
 ******************************************************************************
 * \if API
 * \mainpage OSW module API
 *
 * \section Description
 * The OSWrapper module provides an interface for the OS functions available for
 * the MSTEP users software development.
 *
 * \section Features
 * Features are:
 *  - Semaphore management
 *    - Creation of a semaphore object
 *    - Waiting for a semaphore object
 *    - Signaling a semaphore object
 *    - Accept a semaphore object
 *
 *  - Memory management
 *    - Creation of a memory partition
 *    - Getting a memory block from a partition
 *    - Release of block from a memory partition
 *
 *  - Message queue management
 *    - Creation of a message queue
 *    - Waiting on a message queue
 *    - Sending a message in queue
 *    - Accepting data from message queue
 *
 *  - Flag management
 *    - Creation of flag
 *    - posting a flag
 *    - pending on flag
 *
 *  - Time management:
 *    - Delaying a task
 *    - Get the OS tick
 *
 *  \note The OS manages the time through a tick which is incremented every
 *        millisecond. The tick is stored in a 4 bytes variable. Therefore,
 *        after 2^32 milliseconds, the tick loop back to the 0 value.
 *
 *  - Tempo management:
 *    - Definition of a tempo
 *    - Start a tempo
 *    - Reload a tempo to avoid time drift
 *    - Stop a tempo
 *    - State of a tempo
 *    - Remaining time of a tempo
 *
 *  - taskId of the currently activated task
 *
 *  - Chained list
 *    - Creation of chained list
 *    - Insert element after another one
 *    - Insert element before another one
 *    - Insert element at the end (head) of a list
 *    - Insert element at the beginning (tail) of a list
 *    - Extract element from beginning (head) of list
 *    - Extract element from end (tail) of list
 *    - Remove an element from a list
 *
 *  - FIFO
 *    - Creation of FIFO
 *    - Insert element into FIFO
 *    - Extract element from FIFO
 *
 *  - PrioQueue (Message queue with multiple message priority)
 *    - Create PrioQueue
 *    - Add a priority level
 *    - Waiting for a message from PrioQueue
 *    - Sending a message to PrioQueue
 *    - Accepting data from PrioQueue
 *
 * \section justif Justification of design choices
 * This module is a wrapper used to hide OS function for MSTEP kernel customers.
 *
 * \section V_Global Global variables
 * - OSW_V_MemPartTable: Table containing pointer on all the memory partition
 *     - Protection: None needed, the table is filled at initialisation and only
 *                   read accessed therefore.
 *
 * \section justif Justification of design choices
 * - See SwMD part
 *
 * \section lim Limitations
 * Object creation shall only be done during initialisation. Call to
 * \ref OSW_F_MemCreate, \ref OSW_F_SemCreate, \ref OSW_F_QCreate or
 * \ref OSW_F_FlagCreate once main loop entered is forbidden.
 *
 * \note The number of object to be created are:
 * - Maximum number of semaphore       : 10
 * - Maximum number of flags           : 10
 * - Maximum number of message queue   : 10
 * - Maximum number of PrioQueue       : 10
 * - Maximum number of memory partition:  5
 *
 * \section safety Safety Aspects
 *  - See SwMD part
 *
 * \endif
 *****************************************************************************/
/**\}*/
/**\}*/

#ifndef OSW_H
#define OSW_H

#include "datatypes.h"
#include "step_constant.h"
//#include "os_cpu.h"
/**\addtogroup OSW_INTERFACE
 * \{*/

/******************************************************************************
 * INCLUDES
 *****************************************************************************/

/******************************************************************************
 * CONSTANTS OF MODULE
 *****************************************************************************/

/** Error code from the OS */
/** CR_reference 19 - Guest access to OS error return code */
#define OSW_C_ERR_EVENT_TYPE       1u /**<The pointer given in argument does not point on the right type */
#define OSW_C_ERR_PEND_ISR         2u /**<Call to the function from an ISR                               */
#define OSW_C_TIMEOUT             10u /**<Pend not unlocked within the defined timeout                   */
#define OSW_C_ERR_PEND_LOCKED     17u /**<Call to a pend function with the scheduller locked             */
#define OSW_C_Q_FULL              30u /**<Fifo full, no more posting is allowed                          */
#define OSW_C_Q_EMPTY             31u /**<The fifo read is empty                                         */
#define OSW_C_SEM_OVF             50u /**<The value of the semaphore exceed its limit                    */
#define OSW_C_MEM_INVALID_PART   110u /**<No free memory partition are available                         */
#define OSW_C_MEM_INVALID_BLKS   111u /**<The number of block specified for the partition is less than 2 */
#define OSW_C_MEM_INVALID_SIZE   112u /**<Invalid block size                                             */
#define OSW_C_MEM_NO_FREE_BLKS   113u /**<No more free block to allocate                                 */
#define OSW_C_MEM_FULL           114u /**<The memory block accessed is full                              */
#define OSW_C_MEM_INVALID_PMEM   116u /**<The memory pointer is invalid                                  */
#define OSW_C_MEM_INVALID_ADDR   118u /**<Invalid address for the memory storage in the partition        */
#define OSW_C_ERR_CREATE_ISR     141u /**<Attempt to create an OS object while in an ISR                 */
#define OSW_C_FLAG_ERR_WAIT_TYPE 151u /**<Wrong wait type specified for flag accept/pend                 */
#define OSW_C_FLAG_ERR_NOT_RDY   152u /**<Flag not ready                                                 */
#define OSW_C_FLAG_INVALID_OPT   153u /**<Invalid flag option                                            */
#define OSW_C_FLAG_GRP_DEPLETED  154u /**<No more flag group can be created                              */

/** CR_reference 29 - Use of define instead of enum for flag pend/accept wait type */
/**Definition of different flag pending methods to be used with waitType parameter
 * in FlagAccept and FlagPend functions */
#define OSW_C_FLAG_WAIT_CLR_ALL 0u /**<Wait for all bit specified to be cleared */
#define OSW_C_FLAG_WAIT_CLR_ANY 1u /**<Wait for any bit specified to be cleared */
#define OSW_C_FLAG_WAIT_SET_ALL 2u /**<Wait for all bit specified to be set     */
#define OSW_C_FLAG_WAIT_SET_ANY 3u  /**<Wait for any bit specified to be set     */

/**Value to be added to the pending type to consume the flags if condition
 * satisfied  */
#define OSW_C_FLAG_CONSUME 0x80u

/** Maximum number of levels in a PrioQueue */
#define OSW_C_PRIO_QUEUE_NB_MAX_PRIO_LEVEL 10u

/**Definition of variable used to store IT context during critical sections */
/* "No impact, not changed for compatibility with previous version" */
//#ifdef OSW_STUB
//#define OSW_M_CRITICAL_VARIABLE OS_CPU_SR cpu_sr
//#else
#define OSW_M_CRITICAL_VARIABLE OS_CPU_SR cpu_sr = 0u
//#endif
/******************************************************************************
 * MACROS DEFINITION
 *****************************************************************************/

#ifdef OSW_STUB
/* "osw stub" */
//#define OSW_M_ENTER_CRITICAL() do { cpu_sr ++; } while (cpu_sr < 3u)
//#define OSW_M_EXIT_CRITICAL() do { cpu_sr --; } while (cpu_sr > 1u)
#define OSW_M_ENTER_CRITICAL() do { (cpu_sr) ++; } while (0)
#define OSW_M_EXIT_CRITICAL() do { (cpu_sr) --; } while (0)

#else

/** define used by the OS*/
/* "uCOS-II compatibility" */
#define  OS_CRITICAL_METHOD 3

#ifdef __GNUC__

#define OSW_M_CLI() __asm__ __volatile__ (  "move %/sr,%%d0\n\t"            \
                                            "ori.l #0x0700,%%d0\n\t"        \
                                            "move %%d0,%/sr\n"              \
                                            : /* no outputs */              \
                                            :                               \
                                            : "cc", "%d0", "memory")

#define OSW_M_SAVEFLAGS(x)    asm volatile ("movew %%sr,%0":"=d" (x) : : "memory")
#define OSW_M_RESTOREFLAGS(x) asm volatile ("movew %0,%%sr": :"d" (x) : "memory")

/* Implements <STEP_SwAD_0284> */
/**Save IT context and disable IT*/
#define OSW_M_ENTER_CRITICAL()     ({ OSW_M_SAVEFLAGS(cpu_sr); OSW_M_CLI(); })
/**Restore previous IT context*/
#define OSW_M_EXIT_CRITICAL()      OSW_M_RESTOREFLAGS(cpu_sr)


#else /*__GNUC__*/
/* Implements <STEP_SwAD_0284> */
/**Save IT context and disable all IT except level 7*/

//#define OSW_M_ENTER_CRITICAL() {OS_ENTER_CRITICAL();};
//#define OSW_M_EXIT_CRITICAL() {OS_EXIT_CRITICAL();};


/**Save IT context and disable all IT except level 7*/
#define OSW_M_ENTER_CRITICAL() asm {move.l D0,-(A7);  /* Save D0 into stack */                                  \
        move.w SR, D0;    /* Save SR into D0 */                                     \
        move D0, cpu_sr;  /* Save D0 (SR content) into cpu_sr variable */           \
        ori.l %0x0700,D0; /* Set bit 0x0700 to high (mask all IT except level 7) */ \
        move.w D0, SR;    /* Lock all interuptions */                               \
        move.l (A7)+, D0; /* Restore D0 from stack */                               \
    }

/**Restore previous IT context*/
#define OSW_M_EXIT_CRITICAL() asm {MOVE.L D0,-(A7);  /* Save D0 into stack */                 \
        MOVE (cpu_sr),D0; /* Restore to D0 from cpu_sr variable */ \
        MOVE.W D0, SR;    /* Restore previous IT mask */           \
        MOVE.L (A7)+, D0; /* Restore D0 from stack */              \
    }


#endif /*__GNUC__*/

#endif /*QAC == 1*/

/**Set the number of ticks in one second */
#define OSW_C_TICKS_PER_SEC  1000u

/* Implements <STEP_SwAD_0283> */
/**Convert x expressed in millisecond in ticks value */
/* "Definition of macro to avoid function call when conversion is needed" */
/* "No impact, not changed for compatibility with previous version" */
#define OSW_MS(x)          (((INT32U)(x)* OSW_C_TICKS_PER_SEC) /1000 )

/**Convert x expressed in ticks in a ms value. WARNING should not be used to
 *set in a register */
/* Implements <STEP_SwAD_0283> */
/* "Definition of macro to avoid function call when conversion is needed" */
/* "No impact, not changed for compatibility with previous version" */
#define OSW_TICKS_TO_MS(x) ((INT32U)( ((INT32U)(x)* 1000u) / OSW_C_TICKS_PER_SEC ))

/******************************************************************************
 *  TYPE DEFINITION
 *****************************************************************************/
/** Definition of a Tempo State */
typedef enum
{
    OSW_C_TempoNotStarted = 0u, /**<The tempo is not started */
    OSW_C_TempoElapsed    = 1u, /**<The tempo has elapsed    */
    OSW_C_TempoRunning    = 2u  /**<The tempo is started     */
} OSW_E_TempoState;

/** Full definition of a temporisation */

typedef struct
{
    INT32U t1stStart;        /**<Start time of the tempo               */
    INT32U tZero;            /**<Start time of the tempo when reloaded */
    INT32U nbTickToWait;     /**<Number of tick to wait                */
    OSW_E_TempoState eState; /**<Tempo State                           */
} OSW_T_Tempo;

/** Pointer to memory partition data structures descriptor */
/* "No impact, not changed for compatibility with previous version"*/

typedef void* OSW_T_pMEM;

/** Pointer to event control block descriptor */
typedef void* OSW_T_pEVENT;

/** Pointer to a flags group descriptor */
typedef void* OSW_T_pFLAG_GRP;

/** Definition of the flags control block */
typedef INT32U OSW_T_FLAGS;

/** Definition of the actions associated with flags */
/* "No impact, not changed for compatibility with previous version" */
typedef enum
{
    OSW_C_FLAG_CLR = 0u, /**<Clear the selected flag */
    OSW_C_FLAG_SET = 1u  /**<Set the selected flag */
} OSW_T_FLAG_ACT;

/** Chain list element */

typedef struct OSW_T_ListElemStruct
{
    /** Pointer to previous element */

    struct OSW_T_ListElemStruct * pPrev;
    /** Pointer to next element */

    struct OSW_T_ListElemStruct * pNext;
} OSW_T_ListElem;

/** FIFO structure */

typedef struct
{
    /** Pointer to start of array containing FIFO values  */
    void ** pStartArray;
    /** Pointer to end of array containing FIFO values  */
    void ** pEndArray;
    /** Pointer which will received the next value to store into FIFO */
    void ** pIn;
    /** Pointer to next value to be extracted from FIFO */
    void ** pOut;
    /** Maximum number of element which can be stored into FIFO */
    INT32U sizeFifo;
    /** Number of element currently stored into the FIFO */
    INT32U nbCurrent;
} OSW_T_Fifo;

/** PrioQueue level structure */

typedef struct OSW_T_PrioQueueLevelConfigStruct
{
    /** Used to put this structure into a chained list */
    OSW_T_ListElem listElem;
    /** Priority value of level */
    INT8U priority;
    /** FIFO associated to level */
    OSW_T_Fifo fifo;
} OSW_T_PrioQueueLevelConfig;

/** PrioQueue structure */

typedef struct
{
    /** Number of level in PrioQueue */
    INT8U nbLevel;
    /** Chained list of PrioQueue level ordered by priority */
    OSW_T_ListElem list;
    /** Array of PrioQueue level structure used to store into the chained list */
    OSW_T_PrioQueueLevelConfig prioLevelConfig[OSW_C_PRIO_QUEUE_NB_MAX_PRIO_LEVEL];
    /** Semaphore associated to PrioQueue used to pend on PrioQueue */
    OSW_T_pEVENT pSem;
    /** Maximum number of element which can be inserted into the PrioQueue by counting all priority levels */
    INT32U nbMaxPendingElem;
} OSW_T_PrioQueue;



/******************************************************************************
 *  GLOBAL VARIABLES OF MODULE
 *****************************************************************************/


/******************************************************************************
 *  API FUNCTION PROTOTYPE
 *****************************************************************************/

/**
 ******************************************************************************
 * \par Description :
 *      Create a fixed-sized memory partition that will be managed by the OS and
 *      check the validity of the address of the this memory partition.
 *
 * \par Fault cases:
 *      - EVT_C_OS_ERROR if memory partition creation fails
 *      - EVT_C_OS_ERROR if no more memory is available for the application
 *      - EVT_C_NULL_PTR_ERROR if addr or err are NULL pointers
 *
 * \param addr - In - Range: [!NULL]        \n
 *        is the starting address of the memory partition
 * \param nblks - In - Range: None        \n
 *        is the number of memory blocks to create from the partition.
 * \param blksize - In - Range: a multiple of pointer size        \n
 *        is the size (in bytes) of each block in the memory partition.
 * \param err - Out - Range: [!NULL]        \n
 *        is a pointer to a variable containing an error message which will be
 *        set by this function to either:
 *        - ALSTOM_C_NO_ERROR     if the memory partition has been created
 *                                correctly.
 *        - OSW_C_MEM_INVALID_ADDR  you are specifying an invalid address for the
 *                                 memory storage of the partition or, the block
 *                                 does not align on a pointer boundary
 *        - OSW_C_MEM_INVALID_PART  no free partitions available
 *        - OSW_C_MEM_INVALID_BLKS  user specified an invalid number of blocks
 *                                 (must be >= 2)
 *        - OSW_C_MEM_INVALID_SIZE  user specified an invalid block size
 *
 * \return OSW_T_pMEM:
 *      - != (OSW_T_pMEM)0  if the partition was created
 *      - == (OSW_T_pMEM)0  if the partition was not created because of invalid
 *                          arguments or, no free partition is available.
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - Number of memory partition is limited. Creation of more than
 * OS_MAX_MEM_PART partition leads to safe state.
 * \endif
 *
 * \note This function can no longer be called once the initialization of the
 *       board is done
 *****************************************************************************/
OSW_T_pMEM OSW_F_MemCreate(void*  addr, INT32U nblks, INT32U blksize,
                           INT8U* err);


/**
 ******************************************************************************
 * \par Description :
 *      Get a memory block from a partition. This function checks the validity
 *      of the address of the memory bloc allocated.
 *
 * \par Fault cases:
 *      - EVT_C_NULL_PTR_ERROR if pmem or err are NULL pointers
 *
 * \param pmem - In - Range: [!NULL]        \n
 *        is a pointer to the memory partition control block
 * \param err - Out - Range: [!NULL]        \n
 *        is a pointer to a variable containing an error message which will be
 *        set by this function to either:
 *        - ALSTOM_C_NO_ERROR    if the memory partition has been created
 *                               correctly.
 *        - OSW_C_MEM_NO_FREE_BLKS if there are no more free memory blocks to
 *                                 allocate to caller
 *        - OSW_C_MEM_INVALID_PMEM if the memory pointer is invalid
 *
 * \return void*:
 *      - A pointer to a memory block if no error is detected
 *      - A pointer to NULL if an error is detected
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *****************************************************************************/
void* OSW_F_MemGet(OSW_T_pMEM pmem, INT8U *err);


/**
 ******************************************************************************
 * \par Description :
 *      Releases a memory block from a partition
 *
 * \par Fault cases:
 *      - EVT_C_NULL_PTR_ERROR if pmem or pblk are NULL pointers
 *
 * \param pmem - In - Range: [!NULL]        \n
 *        is a pointer to the memory partition control block
 * \param pblk - Out - Range: [!NULL]        \n
 *        is a pointer to the memory block being released.
 *
 * \return INT8U:
 *        - ALSTOM_C_NO_ERROR     if the memory block was inserted into the
 *                                partition
 *        - OSW_C_MEM_FULL        if you are returning a memory block to an
 *                                already FULL memory to caller
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 *****************************************************************************/
INT8U OSW_F_MemPut(OSW_T_pMEM pmem, void *pblk);

/**
 ******************************************************************************
 * \par Description :
 *      Gives the number of free memory blocks
 *
 * \par Fault cases:
 *      - EVT_C_NULL_PTR_ERROR if pmem is NULL pointers
 *      - EVT_C_SAFETY_CORRUPTED if OSMemQuery doesn't return OS_NO_ERR
 *
 * \par Pseudo-code:
 *      - Query informations about the memBlock
 *      - Return the number of free blocks
 *
 * \param pMem - In - Range: [!NULL]        \n
 *        is a pointer to the memory partition control block
 *
 * \return INT32U: The number of memory blocks free
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 *****************************************************************************/
/* CR_Reference 2362 - Add the possibility to get the number of free memblock */
INT32U OSW_F_MemQuery(OSW_T_pMEM pmem);

/**
 ******************************************************************************
 * \par Description :
 *      This function creates a semaphore.
 *
 * \par Fault cases:
 *      - None
 *
 * \param cnt - In - Range: None        \n
 *      is the initial value for the semaphore. If the value is 0, no resource
 *      is available (or no event has occurred). You initialize the semaphore
 *      to a non-zero value to specify how many resources are available
 *      (e.g. if you have 10 resources, you would initialize the semaphore to
 *      10).
 *
 * \return OSW_T_pEVENT:
 *      - != (void *)0  is a pointer to the event control clock (OSW_T_pEVENT)
 *                      associated with the created semaphore\n
 *      - == (void *)0  if no event control blocks were available
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 * \note This function can no longer be called once the initialization of the
 *       board is done
 *****************************************************************************/
OSW_T_pEVENT OSW_F_SemCreate(INT16U cnt);


/**
 ******************************************************************************
 * \par Description :
 *      This function waits for a semaphore.
 *
 * \par Fault cases:
 *      - EVT_C_OUT_OF_RANGE_ERROR if timeout equal 0 (infinite timeOut)
 *      - EVT_C_ANORMAL_TIMEOUT if a timeout is detected (if timeout_allowed is
 *        FALSE)
 *      - EVT_C_SEM_ERROR if an error different than timeout is detected
 *      - EVT_C_NULL_PTR_ERROR if pevent or err are NULL pointers
 *
 * \param pEvent - In - Range: [!NULL]        \n
 *        is a pointer to the event control block associated with the desired
 *        semaphore.
 * \param timeout - In - Range: [/=0]        \n
 *        Timeout period (in clock ticks). Task will wait for the resource up
 *        to the amount of time specified by this argument.
 * \param pErr - Out - Range: [!NULL]        \n
 *        pErr is a pointer to where an error message will be deposited.
 *        Possible error messages are:
 *        - ALSTOM_C_NO_ERROR    The call was successful and your task owns the
 *                               resource
 *                               or, the event you are waiting for occurred.
 *        - OSW_C_TIMEOUT        The semaphore was not received within the
 *                               specified timeout.\n
 *        - OSW_C_ERR_EVENT_TYPE If you didn't pass a pointer to a semaphore.
 *        - OSW_C_ERR_PEND_ISR   If you called this function from an ISR and the
 *                               result would lead to a suspension.
 * \param timeoutAllowed - In - Range: None        \n
 *         is a boolean that allows functional timeouts. If TRUE, error message
 *         could be OSW_C_TIMEOUT. If FALSE, software enters in lock mode
 *         if a timeout is detected.
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 * \note Infinite time out is not allowed
 *****************************************************************************/
void OSW_F_SemPend(OSW_T_pEVENT pEvent, INT16U timeout, INT8U *pErr,
                   BOOLEAN timeoutAllowed);


/**
 ******************************************************************************
 * \par Description :
 *      This function signals a semaphore.
 *
 * \par Fault cases:
 *      - EVT_C_NULL_PTR_ERROR if pevent is NULL pointer
 *
 * \param pevent - In - Range: [!NULL]        \n
 *        is a pointer to the event control block associated with the desired
 *        semaphore.
 *
 * \return INT8U:
 *      - ALSTOM_C_NO_ERROR    The call was successful and the semaphore was
 *                             signaled.\n
 *      - OSW_C_SEM_OVF        If the semaphore count exceeded its limit. In other
 *                             words, you have signalled the semaphore more often
 *                             than you waited on it with either
 *                             \ref OSW_F_SemAccept or \ref OSW_F_SemPend.\n
 *      - OSW_C_ERR_EVENT_TYPE If you didn't pass a pointer to a semaphore.\n
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 *****************************************************************************/
INT8U OSW_F_SemPost(OSW_T_pEVENT pevent);

/**
 ******************************************************************************
 * \par Description :
 *      This function gives the number of token in a semaphore
 *
 * \par Fault cases:
 *      - EVT_C_NULL_PTR_ERROR if pevent is NULL pointer
 *
 * \par Pseudo-code:
 *      - Get informations about the semaphore
 *      - Return the counter
 *
 * \param pEvent - In - Range: [!NULL]        \n
 *        is a pointer to the event control block associated with the desired
 *        semaphore.
 *
 * \param pErr - Out - Range: [!NULL]        \n
 *        pErr is a pointer to where an error message will be deposited.
 *        Possible error messages are:
 *        - ALSTOM_C_NO_ERROR    The call was successful and your task owns the
 *                               resource
 *                               or, the event you are waiting for occurred.
 *        - OSW_C_ERR_EVENT_TYPE If you didn't pass a pointer to a semaphore.
 *
 * \return INT16U: Semaphore count
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 *****************************************************************************/
/* CR_Reference 2362 - Add the possibility to get the counter of a semaphore */
INT16U OSW_F_SemQuery(OSW_T_pEVENT pEvent, INT8U * pErr);

/**
 ******************************************************************************
 * \par Description :
 *      This function checks the semaphore to see if a resource is available or,
 *      if an event occurred.  Unlike \ref OSW_F_SemPend, \ref OSW_F_SemAccept
 *      does not suspend the calling task if the resource is not available or
 *      the event did not occur.
 *
 * \par Fault cases:
 *      - EVT_C_NULL_PTR_ERROR if pevent is NULL pointer
 *
 * \param pevent - In - Range: [!NULL]        \n
 *        is a pointer to the event control block
 *
 * \return INT16U:
 *      - > 0   if the resource is available or the event did not occur
 *              the semaphore is decremented to obtain the resource.\n
 *      - == 0  if the resource is not available or the event did not occur
 *              or, if 'pevent' is a NULL pointer or, if you didn't pass
 *              a pointer to a semaphore\n
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 *****************************************************************************/
INT16U  OSW_F_SemAccept(OSW_T_pEVENT pevent);

/**
 ******************************************************************************
 * \par Description :
 *      This function creates a message queue if free event control blocks are
 *      available.
 *
 * \par Fault cases:
 *      - EVT_C_NULL_PTR_ERROR if pBuffer is NULL pointer
 *
 * \param pBuffer - in - range: [!NULL]\n
 *        Pointer to the base address of the message queue storage area. The
 *        storage area must be declared as an array of pointers to 'void' as
 *        follows: \code void *MessageStorage[size] \endcode
 * \param size    - in - range: none\n
 *        Number of elements in the storage area
 *
 * \return OSW_T_pEVENT:
 *      - != (void *)0  A pointer to the event control clock (OSW_T_pEVENT)
 *                      associated with the created queue
 *      - == (void *)0  if no event control blocks were available or an error
 *                      was detected
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 * \note This function can no longer be called once the initialization of the
 *       board is done
 *****************************************************************************/
OSW_T_pEVENT OSW_F_QCreate(void **pBuffer, INT16U size);

/**
 ******************************************************************************
 * \par Description :
 *      This function sends message to a queue
 *
 * \par Fault cases:
 *      - EVT_C_NULL_PTR_ERROR if pEvent or pMsg are NULL pointers
 *
 * \param pEvent - in - range: [!NULL]\n
 *        Pointer to the event control block associated with the desired queue
 * \param pMsg   - in - range: [!NULL]\n
 *        Pointer to the message to send.
 *
 * \return INT8U:
 *      - ALSTOM_C_NO_ERROR   The call was successful and the semaphore was
 *                            signaled.\n
 *      - OSW_C_Q_FULL        If the queue cannot accept any more messages
 *                            because it is full.\n
 *      - OSW_C_ERR_EVENT_TYPE  If you didn't pass a pointer to a semaphore.\n
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 *****************************************************************************/
INT8U OSW_F_QPost(OSW_T_pEVENT pEvent, void* pMsg);

/**
 ******************************************************************************
 * \par Description :
 *      This function sends message to the front of a queue instead to the end.
 *
 * \par Fault cases:
 *      - EVT_C_NULL_PTR_ERROR if pEvent or pMsg are NULL pointers
 *
 * \param pEvent - in - range: [!NULL]\n
 *        Pointer to the event control block associated with the desired queue
 * \param pMsg   - in - range: [!NULL]\n
 *        Pointer to the message to send.
 *
 * \return INT8U:
 *      - ALSTOM_C_NO_ERROR   The call was successful and the semaphore was
 *                            signaled.\n
 *      - OSW_C_Q_FULL        If the queue cannot accept any more messages
 *                            because it is full.\n
 *      - OSW_C_ERR_EVENT_TYPE  If you didn't pass a pointer to a semaphore.\n
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 *****************************************************************************/
INT8U OSW_F_QPostFront(OSW_T_pEVENT pEvent, void* pMsg);

/**
 ******************************************************************************
 * \par Description :
 *      This function waits for a message to be sent to a queue.
 *
 * \par Fault cases:
 *      - EVT_C_OUT_OF_RANGE_ERROR if timeout equal 0 (infinite timeOut)
 *      - EVT_C_NULL_PTR_ERROR if pEvent or pErr are NULL pointers
 *      - EVT_C_ANORMAL_TIMEOUT if a timeout is detected (if timeoutAllowed is
 *        FALSE)
 *      - EVT_C_OSQPEND_ERROR if an error different than timeout is detected
 *
 * \param pEvent  - in - range: [!NULL]\n
 *        Pointer to the event control block associated with the desired queue
 * \param timeout - in - range:[/=0]\n
 *        Timeout period (in clock ticks).  Task will wait for a message to
 *        arrive at the queue up to the amount of time specified by this
 *        argument.
 * \param pErr    - in - range: [!NULL]
 *        Pointer to where an error message will be deposited.  Possible error
 *        messages are:
 *        - ALSTOM_C_NO_ERROR:   The call was successful and your task received a message.
 *        - OSW_C_TIMEOUT:       A message was not received within the specified
 *                                timeout
 *        - OSW_C_ERR_EVENT_TYPE:  You didn't pass a pointer to a queue
 *        - OSW_C_ERR_PEND_ISR:    If you called this function from an ISR and the
 *                               result would lead to a suspension.
 *        - OSW_C_ERR_PEND_LOCKED: If you called this function with the scheduler
 *                               is locked
 * \param timeoutAllowed  - In - Range: None        \n
 *         is a boolean that allows functional timeouts. If TRUE, error message
 *         could be OSW_C_TIMEOUT. If FALSE, software enters in lock mode
 *         if a timeout is detected.
 *
 * \return void:
 *   != (void *)0  is a pointer to the message received
 *   == (void *)0  if you received a NULL pointer message or,
 *                 if no message was received or,
 *                 if 'pEvent' is a NULL pointer or,
 *                 if you didn't pass a pointer to a queue.
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 * \note Infinite time out is not allowed
 *****************************************************************************/
void* OSW_F_QPend(OSW_T_pEVENT pEvent, INT16U timeout, INT8U *pErr,
                  BOOLEAN timeoutAllowed);

/**
 ******************************************************************************
 * \par Description :
 *      This function checks the queue to see if a message is available. Unlike
 *      \ref OSW_F_QPend, \ref OSW_F_QAccept does not suspend the calling task
 *      if a message is not available.
 *
 * \par Fault cases:
 *      - EVT_C_NULL_PTR_ERROR if pEvent or pErr are NULL pointers
 *
 * \param pEvent  - in - range: [!NULL]\n
 *        Pointer to the event control block associated with the desired queue
 * \param pErr    - in - range: [!NULL]\n
 *        Pointer to where an error message will be deposited.  Possible error
 *        messages are:
 *        - ALSTOM_C_NO_ERROR: The call was successful and your task received a message.
 *        - OSW_C_ERR_EVENT_TYPE:  You didn't pass a pointer to a queue
 *        - OSW_C_Q_EMPTY: The queue did not contain any messages
 *
 * \return void:
 *   != (void *)0  is the message in the queue if one is available.  The message
 *                 is removed from the so the next time \ref OSW_F_QAccept is
 *                 called, the queue will contain one less entry.
 *   == (void *)0  if you received a NULL pointer message
 *                 if the queue is empty or,
 *                 if 'pevent' is a NULL pointer or,
 *                 if you passed an invalid event type
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 *****************************************************************************/
void* OSW_F_QAccept(OSW_T_pEVENT pEvent, INT8U *pErr);

/**
 ******************************************************************************
 * \par Description :
 *      This function gives the number message in a queue
 *
 * \par Fault cases:
 *      - EVT_C_NULL_PTR_ERROR if pevent or pErr is NULL pointer
 *
 * \par Pseudo-code:
 *      - Get informations about the queue
 *      - Return the number of message in the queue
 *
 * \param pEvent - In - Range: [!NULL]        \n
 *        is a pointer to the event control block associated with the desired
 *        queue.
 * \param pErr - Out - Range: [!NULL]        \n
 *        pErr is a pointer to where an error message will be deposited.
 *        Possible error messages are:
 *        - ALSTOM_C_NO_ERROR    The call was successful and your task owns the
 *                               resource
 *                               or, the event you are waiting for occurred.
 *        - OSW_C_ERR_EVENT_TYPE If you didn't pass a pointer to a queue.
 *
 * \return INT16U: Number of message in the queue
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 *****************************************************************************/
/* CR_Reference 2362 - Add the possibility to get the number of message in a queue */
INT16U OSW_F_QQuery(OSW_T_pEVENT pEvent, INT8U * pErr);


/**
 ******************************************************************************
 * \par Description :
 *      This function is called to create an event flag group.
 *
 * \par Fault cases:
 *      - EVT_C_NULL_PTR_ERROR if pErr is NULL pointer
 *
 * \param flags - in - range: [!NULL]\n
 *        Contains the initial value to store in the event flag group.
 * \param pErr  - in - range: [!NULL]\n
 *        Pointer to an error code which will be returned to your application:
 *          ALSTOM_C_NO_ERROR       if the call was successful.
 *          OSW_C_ERR_CREATE_ISR    if you attempted to create an Event Flag from
 *                                an ISR.
 *          OSW_C_FLAG_GRP_DEPLETED if there are no more event flag groups
 *
 * \return OSW_T_pFlagGrp:
 *      Pointer to an event flag group or a NULL pointer if no more groups are
 *      available.
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 * \note This function can no longer be called once the initialization of the
 *       board is done
 *****************************************************************************/
OSW_T_pFLAG_GRP* OSW_F_FlagCreate(OSW_T_FLAGS flags, INT8U *pErr);


/**
 ******************************************************************************
 * \par Description :
 *      This function is called to set or clear some bits in an event flag
 *      group. The bits to set or clear are specified by a 'bit mask'.
 *
 * \par Fault cases:
 *      - EVT_C_NULL_PTR_ERROR if pErr or pFlagGrp are NULL pointers
 *
 * \param pFlagGrp - in - range: [!NULL]\n
 *        pointer to the desired event flag group.
 * \param flags   - in - range: none\n
 *        If 'action' (see below) is OSW_C_FLAG_SET, each bit that is set in
 *        'flags' will set the corresponding bit in the event flag group.\n
 *        If 'action' (see below) is OSW_C_FLAG_CLR, each bit that is set in
 *        'flags' will CLEAR the corresponding bit in the event flag group.
 * \param action   - in - range: [OSW_C_FLAG_CLR..OSW_C_FLAG_SET]\n
 *        Action to be performed on the flags;
 * \param pErr    - in - range: [!NULL]
 *        Pointer to an error code
 *        ALSTOM_C_NO_ERROR         The call was successfull
 *        OSW_C_ERR_EVENT_TYPE      You are not pointing to an event flag group
 *        OSW_C_FLAG_INVALID_OPT    You specified an invalid option
 *
 * \return OSW_T_pFlagGrp:
 *       The new value of the event flags bits that are still set.
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *****************************************************************************/
OSW_T_FLAGS OSW_F_FlagPost(OSW_T_pFLAG_GRP *pFlagGrp, OSW_T_FLAGS flags,
                           OSW_T_FLAG_ACT action,     INT8U *pErr);


/**
 ******************************************************************************
 * \par Description :
 *      This function is called to accept for a combination of bits to be set in
 *      an event flag group. Your application can accept for ANY bit to be set or
 *      ALL bits to be set.
 *
 * \par Fault cases:
 *      - EVT_C_NULL_PTR_ERROR if pErr or pFlagGrp are NULL pointers
 *
 * \param pFlagGrp - in - range: [!NULL]\n
 *        pointer to the desired event flag group.
 * \param flags   - in - range: none\n
 *        Bit pattern indicating which bit(s) (i.e. flags) you wish to wait for.
 *        The bits you want are specified by setting the corresponding bits in
 *        'flags'.
 * \param waitType - in - range:none\n
 *        Specifies whether you want ALL bits to be set or ANY of the bits to be
 *        set.
 *        You can specify the following argument:
 *        - OSW_C_FLAG_WAIT_CLR_ALL: wait for ALL bits in 'mask' to be clear
 *        - OSW_C_FLAG_WAIT_SET_ALL: wait for ALL bits in 'mask' to be set
 *        - OSW_C_FLAG_WAIT_CLR_ANY: wait for ANY bit  in 'mask' to be clear
 *        - OSW_C_FLAG_WAIT_SET_ANY: wait for ANY bit  in 'mask' to be set
 *
 *         \note Add OSW_C_FLAG_CONSUME if you want the event flag to be 'consumed'
 *               by the call. Example, to wait for any flag in a group AND then
 *               clear the flags that are present, set 'wait_type' to:
 *               \code  OSW_C_FLAG_WAIT_SET_ANY + OSW_C_FLAG_CONSUME \endcode
 * \param pErr    - in - range: [!NULL]\n
 *        Pointer to an error code
 *        ALSTOM_C_NO_ERROR        No error
 *        OSW_C_ERR_EVENT_TYPE     You are not pointing to an event flag group
 *        OSW_C_FLAG_ERR_WAIT_TYPE You didn't specify a proper 'wait_type' argument
 *        OSW_C_FLAG_ERR_NOT_RDY   The desired flags you are waiting for are not
 *                               available.
 *
 * \return OSW_T_pFlagGrp:
 *       The new value of the event flags bits that are still set.
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 *****************************************************************************/
/** CR_reference 29 - Use of define instead of enum for flag pend/accept wait type */
OSW_T_FLAGS OSW_F_FlagAccept(OSW_T_pFLAG_GRP* pFlagGrp,
                             OSW_T_FLAGS      flags,
                             INT8U            waitType,
                             INT8U*           pErr);


/**
 ******************************************************************************
 * \par Description :
 *      This function is called to wait for a combination of bits to be set in
 *      an event flag group. Your application can wait for ANY bit to be set or
 *      ALL bits to be set.
 *
 * \par Fault cases:
 *      - EVT_C_NULL_PTR_ERROR if pErr or pFlagGrp are NULL pointers
 *      - EVT_C_ANORMAL_TIMEOUT if a timeout is detected (if timeout_allowed is
 *        FALSE)
 *      - EVT_C_OUT_OF_RANGE_ERROR if timeout equal 0 (infinite timeOut)
 *      - EVT_C_FLAG_ERROR if any flag pending error occurs
 *
 * \param pFlagGrp - in - range: [!NULL]\n
 *        pointer to the desired event flag group.
 * \param flags   - in - range: none\n
 *        Bit pattern indicating which bit(s) (i.e. flags) you wish to wait for.
 *        The bits you want are specified by setting the corresponding bits in
 *        'flags'.
 * \param waitType - in - range:none\n
 *        Specifies whether you want ALL bits to be set or ANY of the bits to be
 *        set.
 *        You can specify the following argument:
 *        - OSW_C_FLAG_WAIT_CLR_ALL: wait for ALL bits in 'mask' to be clear
 *        - OSW_C_FLAG_WAIT_SET_ALL: wait for ALL bits in 'mask' to be set
 *        - OSW_C_FLAG_WAIT_CLR_ANY: wait for ANY bit  in 'mask' to be clear
 *        - OSW_C_FLAG_WAIT_SET_ANY: wait for ANY bit  in 'mask' to be set
 *
 *         \note Add OSW_C_FLAG_CONSUME if you want the event flag to be 'consumed'
 *               by the call. Example, to wait for any flag in a group AND then
 *               clear the flags that are present, set 'wait_type' to:
 *               \code  OSW_C_FLAG_WAIT_SET_ANY + OSW_C_FLAG_CONSUME \endcode
 * \param timeout - in - range: [/=0]\n
 *        Timeout period (in clock ticks). Task will wait for a message to
 *        arrive at the queue up to the amount of time specified by this
 *        argument.
 * \param pErr    - in - range: [!NULL]\n
 *        Pointer to an error code
 *        ALSTOM_C_NO_ERROR        The desired bits have been set within the
 *                              specified 'timeout'.
 *        OSW_C_ERR_PEND_ISR       If you tried to PEND from an ISR
 *        OSW_C_ERR_EVENT_TYPE     You are not pointing to an event flag group
 *        OSW_C_TIMEOUT            The bit(s) have not been set in the specified
 *                              'timeout'.
 *        OSW_C_FLAG_ERR_WAIT_TYPE You didn't specify a proper 'wait_type' argument
 * \param timeoutAllowed  - In - Range: None\n
 *        is a boolean that allows functional timeouts. If TRUE, error message
 *        could be OSW_C_TIMEOUT. If FALSE, software enters in lock mode
 *        if a timeout is detected.
 *
 * \return OSW_T_pFlagGrp:
 *       The new value of the event flags bits that are still set.
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 * \note Infinite time out is not allowed
 *****************************************************************************/
/** CR_reference 29 - Use of define instead of enum for flag pend/accept wait type */
OSW_T_FLAGS OSW_F_FlagPend(OSW_T_pFLAG_GRP* pFlagGrp,
                           OSW_T_FLAGS      flags,
                           INT8U            waitType,
                           INT16U           timeout,
                           INT8U*           pErr,
                           BOOLEAN          timeoutAllowed);


/**
 ******************************************************************************
 * \par Description :
 *      This function is called to delay execution of the currently running task
 *      until the specified number of system ticks expires.  This, of course,
 *      directly equates to delaying the current task for some time to expire.
 *      No delay will result If the specified delay is 0.  If the specified
 *      delay is greater than 0 then, a context switch will result.
 *
 * \par Fault cases:
 *      None
 *
 * \param ticks - In - Range: None        \n
 *        is the time delay that the task will be suspended in number of clock
 *        'ticks'.
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 *\note By setting to  0 the 'ticks parameters, the task will not be delayed.
 *****************************************************************************/
void OSW_F_TimeDly(INT16U ticks);


/**
 ******************************************************************************
 * \par Description :
 *      This function is used by your application to obtain the current value of
 *      the 32-bit counter which keeps track of the number of clock ticks.
 *
 * \par Fault cases:
 *      None
 *
 * \return INT32U:
 *      The current value of OSTime
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *****************************************************************************/
INT32U OSW_F_TickGet(void);


/**
 ******************************************************************************
 * \par Description :
 *      This function starts a 32 bits temporisation. it stores the current tick
 *      and the number of ticks to wait
 *
 * \par Fault cases:
 *      - EVT_C_NULL_PTR_ERROR if pTempo is NULL
 *
 * \par Global variable used:
 *      None
 *
 * \param pTempo  - In - Range: [!null]       \n
 *        Pointer to the temporisation
 * \param nbTicks - In - Range: None          \n
 *        Number of OS ticks before end
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 * \see \ref OSW_F_TempoState
 * \see \ref OSW_F_TempoRemainingTick
 * \see \ref OSW_F_TempoStop
 *
 * \note temporisation storage under critical section for concurrent access protection
 *****************************************************************************/
void OSW_F_TempoStart(OSW_T_Tempo *pTempo, INT32U nbTicks);


/**
 ******************************************************************************
 * \par Description :
 * This function extends an elapsed temporisation final time with nbTicks.
 * It also updates the tempo state. If reloaded with a tick number making
 * the tempo already elapsed, the state tempo will be updated to "elapsed"
 *
 * If the reloaded temporisation is currently running, the tempo is restarted
 * at the current time.
 *
 * \par Fault cases:
 *      - EVT_C_NULL_PTR_ERROR if pTempo is NULL
 *      - EVT_C_OUT_OF_RANGE_ERROR if the tempo was not started
 *
 * \par Global variable used:
 *      None
 *
 * \param pTempo  - In & Out - Range: [!null]       \n
 *        Pointer to the temporisation
 * \param nbTicks - In - Range: None          \n
 *        Number of OS ticks before end
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 * \see \ref OSW_F_TempoStart
 * \see \ref OSW_F_TempoState
 * \see \ref OSW_F_TempoRemainingTick
 * \see \ref OSW_F_TempoStop
 *
 * \note temporisation update under critical section for concurrent access protection
 *****************************************************************************/
void OSW_F_TempoReload(OSW_T_Tempo *pTempo, INT32U nbTicks);


/**
 ******************************************************************************
 * \par Description :
 *      This function returns if the temporisation is elpased or not.
 *
 * \par Fault cases:
 *      - EVT_C_NULL_PTR_ERROR if pTempo is NULL
 *
 * \par Global variable used:
 *      None
 *
 * \param pTempo  - In - Range: [!null]       \n
 *        Pointer to the temporisation
 *
 * \return BOOLEAN: Status of the temporisation:
 *                  - TRUE if the temporation is elapsed
 *                  - FALSE if the temporisation is running or not activated
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 * \see \ref OSW_F_TempoStart
 * \see \ref OSW_F_TempoRemainingTick
 * \see \ref OSW_F_TempoStop
 *****************************************************************************/
BOOLEAN OSW_F_TempoState(OSW_T_Tempo *pTempo);

/**
 ******************************************************************************
 * \par Description :
 *      This function returns the state of a temporisation and fill the tmout
 *      variable with the remaining time to wait before the end of the tempo
 *
 * \par Fault cases :
 *      - EVT_C_NULL_PTR_ERROR if pTempo is NULL
 *      - EVT_C_NULL_PTR_ERROR if pTickRemaining is NULL
 *
 * \par Global variable used:
 *      None
 *
 * \param pTempo - In - Range: [!null]       \n
 *      Pointer to tempo
 * \param pTickRemaining - Out - Range: [!null]       \n
 *      Pointer to time still remaining to achieve count value if tempo has not
 *      elapsed.\n
 *      Pointer to time since elapsed if tempo has elapsed.
 *
 * \return OSW_E_TempoState status:
 *       - OSW_C_TempoNotStarted if the tempo is not started
 *       - OSW_C_TempoElapsed if the tempo has elapsed
 *       - OSW_C_TempoRunning if the tempo is running
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 * \see \ref OSW_F_TempoStart
 * \see \ref OSW_F_TempoState
 * \see \ref OSW_F_TempoStop
 *
 * \note temporisation update under critical section for concurrent access protection
 *****************************************************************************/
OSW_E_TempoState OSW_F_TempoRemainingTick(OSW_T_Tempo* pTempo,
                                          INT32U*      pTickRemaining);


/**
 ******************************************************************************
 * \par Description :
 *      This function stops a temporisation.
 *
 * \par Fault cases:
 *      - EVT_C_NULL_PTR_ERROR if pTempo is NULL
 *
 * \par Global variable used:
 *      None
 *
 * \param pTempo  - In - Range: [!null]       \n
 *        Pointer to the temporisation
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 * \see \ref OSW_F_TempoStart
 * \see \ref OSW_F_TempoState
 * \see \ref OSW_F_TempoRemainingTick
 *
 * \note temporisation update under critical section for concurrent access protection
 *****************************************************************************/
void OSW_F_TempoStop(OSW_T_Tempo *pTempo);

/**
 ******************************************************************************
 * \par Description :
 *      This function returns the priority of the task currently activated.
 *
 * \par Fault cases:
 *      None
 *
 * \par Global variable used:
 *      None
 *
 * \return INT8U: Tasks priority
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *****************************************************************************/
INT8U OSW_F_TaskPrioGet(void);

/**
 ******************************************************************************
 * \par Description:
 *      Tell if OS is started or not.
 *
 * \par Fault cases:
 *      - None
 *
 *
 * \return BOOLEAN - TRUE is OS is started, FALSE otherwise.
 *
 *****************************************************************************/
BOOLEAN OSW_F_IsOsStarted(void);

/**
 ******************************************************************************
 * \par Description:
 *      Create a PrioQueue.
 *
 * \par Fault cases:
 *      - No more PrioQueue available
 *
 * \par Pseudo-code:
 *      -# Get a new PrioQueue from level of all PrioQueue
 *      -# Increment number of used PrioQueue
 *      -# Initialize PrioQueue (structure, chained list and semaphore)
 *
 *
 * \return OSW_T_PrioQueue * - Pointer to the created PrioQueue
 *
 *****************************************************************************/
OSW_T_PrioQueue * OSW_F_PrioQCreate(void);

/**
 ******************************************************************************
 * \par Description:
 *      Add a priority level into a PrioQueue.
 *
 * \par Fault cases:
 *      - Parameter range error
 *      - Priority value already used
 *      - Too many possible pending entries
 *      - No more Priority level available
 *
 * \par Pseudo-code:
 *      -# Get first element in PrioQueue chained list
 *      -# Search into chained list for a PrioQueue level with a bigger priority
 *      -# Initialize the new priority level
 *      -# If a bigger priority level has been found
 *        -# Insert the new level before the found one into the chained list
 *      -# Else
 *        -# Insert the new level at the end (tail) of the chained list
 *
 * \param pPrioQueue - In & Out - Range: [!NULL] \n
 *              PrioQueue to which a level have to be added
 *
 * \param prioLevel - In - Range: None \n
 *              Priority value
 *
 * \param pBuffer - In/Out - Range: [!NULL] \n
 *              Pointer to array of 'void*' which will be used to store messages
 *              associated to the new level
 *
 * \param size - In - Range: None \n
 *              Size of array defined by pBuffer
 *
 *****************************************************************************/
void OSW_F_PrioQAddLevel(OSW_T_PrioQueue * pPrioQueue, INT8U prioLevel, void **pBuffer, INT16U size);


/**
 ******************************************************************************
 * \par Description:
 *      Pend for a message from PrioQueue. The PrioQueue will return the first
 *      message available from the most priority level.
 *
 * \par Fault cases:
 *      - Parameter range error
 *      - pPrioQueue->pSem is NULL
 *      - Unexpected error while pending on semaphore
 *      - The extracted message is NULL
 *      - Timeout which is not authorized
 *
 * \par Pseudo-code:
 *      -# Pend on internal semaphore associated to PrioQueue
 *      -# If The pend operation on semaphore is successfull
 *        -# Extract the most priority message from internal FIFO
 *      -# If The pend operation on semaphore is in timeout
 *        -# If timeout is not allowed
 *          -# Trigger FUSE
 *
 * \param pPrioQueue - In - Range: [!NULL] \n
 *              Pointer to the PrioQueue from which pend must be realized.
 *
 * \param timeout - In - Range: [!0] \n
 *              Timeout of the pend.
 *
 * \param pErr - Out - Range: [!NULL] \n
 *              Pointer to the result of function
 *              - ALSTOM_C_NO_ERROR if success
 *              - OSW_C_TIMEOUT in case of timeout
 *
 * \param pPrio - Out - Range: None \n
 *              If not NULL it points to priority level into which the message has been found.
 *
 * \param timeoutAllowed - In - Range: None \n
 *              Tell if a fuse must be triggered if timeout occurs.
 *
 * \return void * - Extracted message, or NULL if no message has been extracted.
 *
 *****************************************************************************/
void * OSW_F_PrioQPend(const OSW_T_PrioQueue * pPrioQueue, INT16U timeout,
                       INT8U *pErr, INT8U * pPrio, BOOLEAN timeoutAllowed);

/**
 ******************************************************************************
 * \par Description:
 *      Get most priority message immediately available from PrioQueue. No pending
 *      operation is realized.
 *
 * \par Fault cases:
 *      - Parameter range error
 *      - pPrioQueue->pSem is NULL
 *      - Unexpected error while accepting semaphore
 *      - The extracted message is NULL
 *
 * \par Pseudo-code:
 *      -# Accept semaphore
 *      -# If semaphore was available
 *        -# Extract the most priority message from internal FIFO
 *
 * \param pPrioQueue - In - Range: [!NULL] \n
 *              Pointer to the PrioQueue from which 'accept' operation must be realized.
 *
 * \param pErr - Out - Range: [!NULL] \n
 *              Pointer to the result of function:
 *              - ALSTOM_C_NO_ERROR if success
 *              - OSW_C_Q_EMPTY if PrioQueue is empty and no message can be extracted
 *
 * \param pPrio - Out - Range: None \n
 *              If not NULL it points to priority level into which the message has been found.
 *
 * \return void * - Extracted message, or NULL if no message has been extracted.
 *
 *****************************************************************************/
void * OSW_F_PrioQAccept(const OSW_T_PrioQueue * pPrioQueue, INT8U *pErr, INT8U * pPrio);

/**
 ******************************************************************************
 * \par Description:
 *      Post message to PrioQueue
 *
 * \par Fault cases:
 *      - Parameter range error
 *      - pPrioQueue->pSem is NULL
 *      - Priority level has not been found
 *      - Semaphore post error
 *
 * \par Pseudo-code:
 *      -# Loop on chain list of priority level of PrioQueue
 *        -# If current priority level is lower than prio
 *          -# Check next level
 *        -# Else if current priority level is equal to prio
 *          -# Post message in FIFO associated to priority level
 *          -# Post semaphore of PrioQueue
 *
 * \param pPrioQueue - In - Range: [!NULL] \n
 *              PrioQueue into which message must be posted
 *
 * \param pMsg - In - Range: [!NULL] \n
 *              Message to be posted
 *
 * \param prio - In - Range: None \n
 *              Priority level of message
 *
 *
 * \return ALSTOM_T_Status - ALSTOM_C_NO_ERROR if post is a success, and
 * ALSTOM_C_ERROR if the FIFO associated to the given priority is full
 *
 *****************************************************************************/
ALSTOM_T_Status OSW_F_PrioQPost(const OSW_T_PrioQueue * pPrioQueue, void* pMsg, INT8U prio);

/**
******************************************************************************
* \par Description:
*       Tell if the PrioQueue contains some message with priority higher than
*       the given one (ie with message priority value < prio)
*
* \par Fault cases:
*      - Parameter range error
*
* \par Pseudo-code:
*       -# Loop on chain list of priority level of PrioQueue
*        -# If current priority level is lower than prio
*         -# If there are message in FIFO of current priority level
*           -# Return TRUE
*        -# Else
*          -# Return FALSE
*
* \param pPrioQueue - In - Range: [!NULL] \n
*              PrioQueue which is interrogated
*
* \param prio - In - Range: None \n
*              Priority level
*
*
* \return BOOLEAN - TRUE if some messages with priority higher
*           then the given one are available, FALSE otherwise.
*
*****************************************************************************/
BOOLEAN OSW_F_PrioQHasPrioMessage(const OSW_T_PrioQueue * pPrioQueue, INT8U prio);

/**
 ******************************************************************************
 * \par Description:
 *      Initialize a chained list
 *
 * \par Fault cases:
 *      - Parameter range error
 *
 * \param pList - In - Range: [!NULL] \n
 *              List which must be initialized
 *
 *****************************************************************************/
void OSW_F_ListInit(OSW_T_ListElem * pList);

/**
 ******************************************************************************
 * \par Description:
 *      Insert a list element before another one
 *
 * \par Fault cases:
 *      - Parameter range error
 *
 * \param pEntryRef - In & Out - Range: [!NULL] \n
 *              List element before which the new entry must be inserted
 *
 * \param pEntry - In & Out - Range: [!NULL] \n
 *              List element which must be inserted
 *
 *****************************************************************************/
void OSW_F_ListInsertBefore(OSW_T_ListElem * pEntryRef, OSW_T_ListElem * pEntry);

/**
 ******************************************************************************
 * \par Description:
 *      Insert List element at the end (tail) of a list.
 *
 * \par Fault cases:
 *      - Parameter range error
 *
 * \param pList - In - Range: [!NULL] \n
 *              List into which element must be inserted
 *
 * \param pEntry - In - Range: [!NULL] \n
 *              List element which must be inserted
 *****************************************************************************/
void OSW_F_ListAddToTail(OSW_T_ListElem * pList, OSW_T_ListElem * pEntry);

/**
 ******************************************************************************
 * \par Description:
 *      Insert a list element after another one
 *
 * \par Fault cases:
 *      - Parameter range error
 *
 * \param pEntryRef - In & Out - Range: [!NULL] \n
 *              List element after which the new entry must be inserted
 *
 * \param pEntry - In & Out - Range: [!NULL] \n
 *              List element which must be inserted
 *
 *****************************************************************************/
void OSW_F_ListInsertAfter(OSW_T_ListElem * pEntryRef, OSW_T_ListElem * pEntry);

/**
 ******************************************************************************
 * \par Description:
 *      Insert List element at the beginning (head) of a list.
 *
 * \par Fault cases:
 *      - Parameter range error
 *
 * \param pList - In - Range: [!NULL] \n
 *              List into which element must be inserted
 *
 * \param pEntry - In - Range: [!NULL] \n
 *              List element which must be inserted
 *****************************************************************************/
void OSW_F_ListAddToHead(OSW_T_ListElem * pList, OSW_T_ListElem * pEntry);

/**
 ******************************************************************************
 * \par Description:
 *      Extract list element from beginning (head) of list.
 *
 * \par Fault cases:
 *      - Parameter range error
 *
 * \param pList - In/Out - Range: [!NULL] \n
 *              List from which element must be extracted
 *
 * \return Extracted FIFO element or NULL if there are no element in FIFO
 *
 *****************************************************************************/
OSW_T_ListElem * OSW_F_ListExtractFromHead(OSW_T_ListElem * pList);

/**
 ******************************************************************************
 * \par Description:
 *      Extract list element from end (tail) of list.
 *
 * \par Fault cases:
 *      - Parameter range error
 *
 * \param pList - In/Out - Range: [!NULL] \n
 *              List from which element must be extracted
 *
 * \return Extracted FIFO element or NULL if there are no element in FIFO
 *
 *****************************************************************************/
OSW_T_ListElem * OSW_F_ListExtractFromTail(OSW_T_ListElem * pList);

/**
 ******************************************************************************
 * \par Description:
 *      Unchained a list element.
 *
 * \par Fault cases:
 *      - Parameter range error
 *      - The next or previous element of given entry element is NULL
 *      - The given element is an empty list header
 *
 * \param pEntry - In & Out - Range: [!NULL] \n
 *              List element to be unchained
 *
 *
 *****************************************************************************/
void OSW_F_ListRemove(OSW_T_ListElem * pEntry);

/**
 ******************************************************************************
 * \par Description:
 *      Get next FIFO element.
 *
 * \par Fault cases:
 *      - Parameter range error
 *
 * \param pList - In - Range: [!NULL] \n
 *              List from which the element comes from.
 *
 * \param pEntryRef - In - Range: [!NULL] \n
 *              The function returns the next element from this one.
 *
 * \return The next FIFO element or NULL if there are no next element in FIFO
 *
 *****************************************************************************/
// "No side effect, function parameters are not modified"
OSW_T_ListElem * OSW_F_ListGetNext(const OSW_T_ListElem * pList, const OSW_T_ListElem * pEntryRef);

/**
 ******************************************************************************
 * \par Description:
 *      Initialize FIFO. A FIFO store element which are 'void *'.
 *
 * \par Fault cases:
 *      - Parameter range error
 *
 * \param pFifo - In - Range: [!NULL] \n
 *              Pointer to FIFO to be initialized
 *
 * \param pBuffer - In - Range: [!NULL] \n
 *              Array used to store message (of type 'void*') into FIFO.
 *
 * \param size - In/Out - Range: None \n
 *              Number of element in array pBuffer
 *
 *****************************************************************************/
void OSW_F_FifoInit(OSW_T_Fifo * pFifo, void **pBuffer, INT16U size);

/**
 ******************************************************************************
 * \par Description:
 *      Extract value from FIFO.
 *
 * \par Fault cases:
 *      - Parameter range error
 *
 * \param pFifo - In/Out - Range: [!NULL] \n
 *              FIFO from which a value must be extracted.
 *
 *
 * \return void * - The extracted element, or NULL if no element can be extracted.
 *
 *****************************************************************************/
void * OSW_F_FifoExtract(OSW_T_Fifo * pFifo);

/**
 ******************************************************************************
 * \par Description:
 *      Insert value into FIFO.
 *
 * \par Fault cases:
 *      - Parameter range error.
 *
 * \param pFifo - In/Out - Range: [!NULL] \n
 *              FIFO into which value has to be inserted
 *
 * \param insertElem - In - Range: [!NULL] \n
 *              Value to insert.
 *
 * \return ALSTOM_T_Status - ALSTOM_C_NO_ERROR if success, ALSTOM_C_ERROR if
 *      FIFO is full.
 *
 *****************************************************************************/
ALSTOM_T_Status OSW_F_FifoInsert(OSW_T_Fifo * pFifo, void * insertElem);

/**\}*/ /*End of OSW_INTERFACE*/

#endif /* OSW_H */


/* EOF */
