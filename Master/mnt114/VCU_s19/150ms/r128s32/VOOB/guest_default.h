/******************************************************************************
 * COPYRIGHT (C) CASCO Signal 2015. VIOP Project. All rights reserved.
 *****************************************************************************/

/******************** FILE HEADER *******************************************/
/**\addtogroup GUEST_INTERFACE
 * \{*/
/**
 ******************************************************************************
 * \if API
 * \page Guest Default API
 *
 * \section Description
 * This header defines the event Id available on guest application.
 *
 * \section Limitations
 * Id of the first event shall be equal to 192. Guest event Id are limited to 64.
 *
 * \endif
 *****************************************************************************/
/**\}*/

#ifndef _guest_default_h_
#define _guest_default_h_


/**\addtogroup GUEST_INTERFACE
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
/*
 * Event code definition
 * Before adding a new event, please read the following instructions:
 *      - Add the new default BEFORE the comment that allowed to,
 *      - Increase GUEST_C_LAST_DEFAULT and check that it is lower than 256,
 *      - Define the actions joined to the new event in guest.c.
 */
#define GUEST_C_UNDEFINED       (GDF_T_EventId)192 /**<Undefined error was done      */
#define GUEST_C_CASE_IMPOSSIBLE (GDF_T_EventId)193 /**<Default case leading to fuse  */
#define GUEST_C_SW_FUSE         (GDF_T_EventId)194 /**<Default leading to fuse       */
#define GUEST_C_CHECKBUF_ERR    (GDF_T_EventId)195 /**<Check buffer error       */
#define GUEST_C_FuncMode_ERR    (GDF_T_EventId)196 /**<Check Function Mode error       */
#define GUEST_C_RBData_ERR      (GDF_T_EventId)197 /**<Check RBData error       */
#define GUEST_C_FPGACNT_ERR     (GDF_T_EventId)198 /**<Check FPGACNT error       */
#define GUEST_C_IntCNT_ERR      (GDF_T_EventId)199 /**<Check IntCNT error       */
#define GUEST_C_RBoutput_ERR    (GDF_T_EventId)200 /**<Check RBoutput error       */
#define GUEST_C_CMDVote_ERR     (GDF_T_EventId)201 /**<Check CMDVote error       */
#define GUEST_C_ISOLATION_ERR   (GDF_T_EventId)202 /**<Isolation error       */

/** New defaults have to be added before this comment*/
#define GUEST_C_LAST_DEFAULT    (GDF_T_EventId)203  /**< BE CAREFUL, this ID shall be <= 255 */

/**Number of default defined in guest application */
#define GUEST_C_NBDEFAULT       (GDF_T_EventId)(GUEST_C_LAST_DEFAULT - GUEST_C_UNDEFINED)

/******************************************************************************
 *  GLOBAL VARIABLES OF MODULE
 *****************************************************************************/


/******************************************************************************
 *  API FUNCTION PROTOTYPE
 *****************************************************************************/

/**\}*/ /*End of GUEST_INTERFACE*/

#endif /* _guest_default_h_ */


/* EOF */
