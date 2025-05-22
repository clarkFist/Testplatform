/******************************************************************************
 * COPYRIGHT (C) Alstom Transport 2008. STEP Project. All rights reserved.
 *****************************************************************************/

/******************** FILE HEADER *******************************************/
/**\addtogroup GDF_MODULE
 * \{*/
/**\addtogroup GDF_INTERFACE
 * \{*/
/**\}*/
/**\}*/


#ifndef GDF_AUTHORIZE_H
#define GDF_AUTHORIZE_H
/* "This undef is used for a special include file which is needed to improve maintenability by specifying section of code where GDF are not authorized"*/
#undef GDF_AUTHORIZE_H

/* This file MUST NOT be protected against multiple
 * inclusion. However, the checker force us to have the previous
 * line. So we immediately undef the define */
/* "gdf"*/
#include "gdf.h"
/**\addtogroup GDF_AUTHORIZE_INTERFACE
 * \{*/
/******************************************************************************
 * INCLUDES
 *****************************************************************************/

/**********************************************************************/
/*  CONSTANTS OF MODULE                                               */
/**********************************************************************/

/******************************************************************************
 * MACROS DEFINITION
 *****************************************************************************/
/* Panic assert macro definition */
/* "This undef is used for a special include file which is needed to improve maintenability by specifying section of code where GDF are not authorized" */

#ifdef GDF_M_RANGE_ASSERT
#undef GDF_M_RANGE_ASSERT
#endif
#ifdef GDF_M_NULL_ASSERT
#undef GDF_M_NULL_ASSERT
#endif
#ifdef GDF_M_FAULT_EXT
#undef GDF_M_FAULT_EXT
#endif

#ifdef QAC
#define GDF_M_NULL_ASSERT(_POINTER) do {                                \
        GDF_M_FAULT_EXT (EVT_C_NULL_PTR_ERROR, GDF_M_CONTEXT(0), "NULL pointer detected"); \
        while(NULL == (_POINTER)){;};                                   \
    }                                       \
    while(0)

#define GDF_M_RANGE_ASSERT(_STATEMENT, _CTXT) do{                       \
        GDF_M_FAULT_EXT (EVT_C_OUT_OF_RANGE_ERROR, GDF_M_CONTEXT(_CTXT), "Out of range error"); \
        while(_STATEMENT){;};                                           \
    }                                                                   \
    while(0)

#else
/* Implements <STEP_SwAD_0298> */
/** Panic assert on NULL pointer*/
#define GDF_M_NULL_ASSERT(_POINTER) do{                                                                  \
        if(NULL == (_POINTER) )                                         \
        {                                                           \
            GDF_M_FAULT_EXT (EVT_C_NULL_PTR_ERROR, GDF_M_CONTEXT(0), "NULL pointer detected"); \
        }                                                               \
        /*else{}        */                                                  \
    }                                                                   \
    while(0)

/** Panic assert on parameter range error*/
#define GDF_M_RANGE_ASSERT(_STATEMENT, _CTXT) do{                                                                           \
        if(_STATEMENT)                                                  \
        {                                                           \
            GDF_M_FAULT_EXT (EVT_C_OUT_OF_RANGE_ERROR, GDF_M_CONTEXT(_CTXT), "Out of range error"); \
        }                                                               \
        /*else{}        */                                                  \
    }                                                                   \
    while(0)
#endif

/* "GDF macro used to parse argument to GDF function keeping caller's line number" */
#ifdef QAC
/** QAC stub, as __FILE__ macro is not handled properly by QAC */
#define GDF_M_FAULT_EXT(A,B, exp) (GDF_F_FaultInternal(A, B, "", (INT16U)(__LINE__), OSW_F_TaskPrioGet()))
#else
/** Addition to the event of the file name, line number and task Id*/
#define GDF_M_FAULT_EXT(A,B, exp) (GDF_F_FaultInternal((GDF_T_EventId)(A), (INT32U)(B), (const char *)(__FILE__), (INT16U)(__LINE__), OSW_F_TaskPrioGet()))
#endif /* (QAC == 1) */


/******************************************************************************
 *  TYPE DEFINITION
 *****************************************************************************/

/******************************************************************************
 *  GLOBAL VARIABLES OF MODULE
 *****************************************************************************/

/******************************************************************************
 *  FUNCTIONS PROTOTYPE
 *****************************************************************************/

/**\}*/ /*End of GDF_AUTHORIZE_INTERFACE */

#endif /* GDF_AUTHORIZE_H */


/* EOF */
