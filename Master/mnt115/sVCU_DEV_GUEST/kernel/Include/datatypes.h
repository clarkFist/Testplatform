/******************************************************************************
 * COPYRIGHT (C) Alstom Transport 2008. STEP Project. All rights reserved.
 *****************************************************************************/

/******************** FILE HEADER *******************************************/
/**\addtogroup MSTEP_INTERFACE
 * \{*/
/**
 ******************************************************************************
 * \if API
 * \page DataType Basic Datatypes
 *
 * \section Description
 * Define basic data types (INT8U, ...)
 *
 * \section Features
 * Features are:
 *  - Basic type definition
 *
 * \section Limitations
 * - None
 *
 * \endif
 *****************************************************************************/
/**\}*/

#ifndef _datatypes_h_
#define _datatypes_h_

/**\addtogroup MSTEP_INTERFACE
 * \{*/
/******************************************************************************
 * CONSTANTS OF MODULE
 *****************************************************************************/

/******************************************************************************
 * MACROS DEFINITION
 *****************************************************************************/

/******************************************************************************
 *  TYPE DEFINITION
 *****************************************************************************/
#define  FALSE  ((BOOLEAN)(0u)) /**<Value of FALSE statement */
#define  TRUE   ((BOOLEAN)(1u)) /**<Value of TRUE statement */
#ifndef NULL
#define  NULL   (0u) /**<Definition of NULL value */
#endif
typedef char                    CHAR;      /**<Definition of CHAR type identical as char               */
typedef unsigned char           BOOLEAN;   /**<Definition of 8 bits BOOLEAN type                       */
typedef unsigned char           INT8U;     /**<Unsigned  8 bit quantity                                */
typedef volatile unsigned char  VINT8U;    /**<Volatile unsigned  8 bit quantity                       */
typedef signed   char           INT8S;     /**<Signed    8 bit quantity                                */
typedef unsigned short          INT16U;    /**<Unsigned 16 bit quantity                                */
typedef volatile unsigned short VINT16U;   /**<Volatile unsigned 16 bit quantity                       */
typedef signed   short          INT16S;    /**<Signed   16 bit quantity                                */
typedef unsigned int            INT32U;    /**<Unsigned 32 bit quantity                                */
typedef signed   int            INT32S;    /**<Signed   32 bit quantity                                */
typedef volatile unsigned int   VINT32U;   /**<Volatile signed 32 bit quantity                         */

typedef INT32U                  OS_STK;        /**<Each stack entry is 32-bit wide                         */
typedef INT32U                  OS_CPU_SR;     /**<Define size of CPU status register, 32 bits on ColdFire */
typedef INT8U                   OS_T_TaskPrio; /**<Task priority                       */
typedef INT8U                   OS_T_TaskId;   /**<Task Identifier                     */

typedef void (* vectorTableEntryType)(void);

/******************************************************************************
 *  GLOBAL VARIABLES OF MODULE
 *****************************************************************************/

/******************************************************************************
 *  API FUNCTION PROTOTYPE
 *****************************************************************************/

/**\}*/ /*End of MSTEP_INTERFACE */

#endif /* _datatypes_h_ */

/* EOF */
