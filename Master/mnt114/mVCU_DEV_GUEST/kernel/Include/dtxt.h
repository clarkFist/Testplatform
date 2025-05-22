/*******************************************************************************
 * COPYRIGHT (C) Alstom Transport 2008. STEP Project. All rights reserved.
 ******************************************************************************/

/******************** FILE HEADER ********************************************/
/**\addtogroup DTXT
 * \{*/
/**\addtogroup DTXT_INTERFACE
 * \{*/
/**
 *******************************************************************************
 * \if API
 * \mainpage DTXT module API
 *
 * \section Description
 * This module provide functions and macros used to trace on the front panel
 * serial line.
 *
 * \section Features
 * This module provide two way of tracing:
 *    - Direct way.
 *    - Queued way.
 *
 * Direct way directly access the hardware to trace. This method have an
 * important impact on sheduling and may leads to failure in case of mis-use.
 *
 * Queued way use task to trace. Protection against over load is
 * implemented. In case of FIFO saturation, the number of message lost is
 * written on link when tracing is available.
 *
 * \section V_Global Global variables used:
 *      None.
 *
 * \section lim Limitations
 * The maximum size of the message to be traced on the UART is limited
 * to 130 bytes.
 *
 * \endif
 ******************************************************************************/
/**\}*/
/**\}*/

#ifndef DTXT_H
#define DTXT_H
/**\addtogroup DTXT_INTERFACE
 *  \{*/

/******************************************************************************/
/*  CONSTANTS OF MODULE                                                       */
/******************************************************************************/

/******************************************************************************/
/*  MACROS DEFINITION                                                         */
/******************************************************************************/
/* "Definition of DTXT_PRINT_ON done in dtxt.h includers" */
#ifdef DTXT_PRINT_ON
/*"Backwards compatibility and portability is not required" */
#define DTXT_M_STRINGOMATIC(x) #x               /**<Used for string conversion */
/* "Macro cannot be replaced by a function" */
#define DTXT_M_TOSTRING(x) DTXT_M_STRINGOMATIC(x) /**<Used for string conversion */

/* Special case of QAC, does not parse correctly # */
#ifdef QAC
#define DTXT_C_LOC /**<QAC stub */
#else
#define DTXT_C_LOC __FILE__ ":" DTXT_M_TOSTRING(__LINE__) ">" /**<Associate line
* number of the
* caller */
#endif /* (QAC == 1) */

/* "Backwards compatibility and portability is not required" */
/**Conversion macro  */
#define DTXT_M_TO_STRING(a) #a
/* "Parameter C cant be enclosed in parenthesis" */

/**Tracing macro without parameter  */
/* "Rules 10 is not followed but there is no risk in maintainability as the DTXT prefix is present. Morevoer, too much code have to be changed (customer impact)" */
/* Implements <STEP_SwAD_0290> */
#define DTXT_REPORT(T,C)              do {DTXT_F_Report((DTXT_E_ReportType)(T),DTXT_C_LOC " ", (const char*)(C),0u,(0u), \
                                                            (0u),(0u),(0u),(0u),(0u));} while(0)

/**Tracing macro allowing 1 parameter  */
/* Implements <STEP_SwAD_0290> */
#define DTXT_REPORT1(T,C,W)           do {DTXT_F_Report((DTXT_E_ReportType)(T),DTXT_C_LOC " ", (const char*)(C),1u,(INT32U)(W), \
                                                            (0u),(0u),(0u),(0u),(0u));} while(0)

/**Tracing macro allowing 2 parameters */
/* Implements <STEP_SwAD_0290> */
#define DTXT_REPORT2(T,C,W,X)         do {DTXT_F_Report((DTXT_E_ReportType)(T),DTXT_C_LOC " ", (const char*)(C),2u,(INT32U)(W), \
                                                            (INT32U)(X),(0u),(0u),(0u),(0u));} while(0)

/**Tracing macro allowing 3 parameters */
/* Implements <STEP_SwAD_0290> */
#define DTXT_REPORT3(T,C,W,X,Y)       do {DTXT_F_Report((DTXT_E_ReportType)(T),DTXT_C_LOC " ", (const char*)(C),3u,(INT32U)(W), \
                                                            (INT32U)(X),(INT32U)(Y),(0u),(0u),(0u));} while(0)

/**Tracing macro allowing 4 parameters */
/* Implements <STEP_SwAD_0290> */
#define DTXT_REPORT4(T,C,W,X,Y,Z)     do {DTXT_F_Report((DTXT_E_ReportType)(T),DTXT_C_LOC " ", (const char*)(C),4u,(INT32U)(W), \
                                                            (INT32U)(X),(INT32U)(Y),(INT32U)(Z),(0u),(0u));} while(0)

/**Tracing macro allowing 5 parameters */
/* Implements <STEP_SwAD_0290> */
#define DTXT_REPORT5(T,C,W,X,Y,Z,A)   do {DTXT_F_Report((DTXT_E_ReportType)(T),DTXT_C_LOC " ", (const char*)(C),5u,(INT32U)(W), \
                                                            (INT32U)(X),(INT32U)(Y),(INT32U)(Z),(INT32U)(A),(0u));} while(0)

/**Tracing macro allowing 6 parameters */
/* Implements <STEP_SwAD_0290> */
#define DTXT_REPORT6(T,C,W,X,Y,Z,A,B) do {DTXT_F_Report((DTXT_E_ReportType)(T),DTXT_C_LOC " ", (const char*)(C),6u,(INT32U)(W), \
                                                            (INT32U)(X),(INT32U)(Y),(INT32U)(Z),(INT32U)(A),(INT32U)(B));} while(0)
#else
#define DTXT_REPORT(T,C)              do { } while (0) /**<Trace off stub */
#define DTXT_REPORT1(T,C,W)           do { } while (0) /**<Trace off stub */
#define DTXT_REPORT2(T,C,W,X)         do { } while (0) /**<Trace off stub */
#define DTXT_REPORT3(T,C,W,X,Y)       do { } while (0) /**<Trace off stub */
#define DTXT_REPORT4(T,C,W,X,Y,Z)     do { } while (0) /**<Trace off stub */
#define DTXT_REPORT5(T,C,W,X,Y,Z,A)   do { } while (0) /**<Trace off stub */
#define DTXT_REPORT6(T,C,W,X,Y,Z,A,B) do { } while (0) /**<Trace off stub */
#endif /* (DTXT_PRINT_ON == TRUE) */

/** ANSI colors escape sequence */
#define DTXT_ANSI_FG_BLACK "\x1B[30m"          /**<Black characters       */
#define DTXT_ANSI_FG_RED "\x1B[31m"            /**<Red characters         */
#define DTXT_ANSI_FG_GREEN "\x1B[32m"          /**<Green characters       */
#define DTXT_ANSI_FG_BROWN "\x1B[33m"          /**<Brown characters       */
#define DTXT_ANSI_FG_BLUE "\x1B[34m"           /**<Blue characters        */
#define DTXT_ANSI_FG_MAGENTA "\x1B[35m"        /**<Magenta characters     */
#define DTXT_ANSI_FG_CYAN "\x1B[36m"           /**<Cyan characters        */
#define DTXT_ANSI_FG_WHITE "\x1B[37m;1"        /**<White characters       */
#define DTXT_ANSI_FG_BRIGHTBLUE "\x1B[34;1m"   /**<Light blue characters  */
#define DTXT_ANSI_FG_BRIGHTCYAN  "\x1B[36;1m"  /**<Light cyan characters  */
#define DTXT_ANSI_FG_BRIGHTGREEN  "\x1B[32;1m" /**<Light green characters */
#define DTXT_ANSI_FG_BRIGHTRED  "\x1B[31;1m"   /**<light red characters   */
#define DTXT_ANSI_FG_PURPLE  "\x1B[35;1m"      /**<Purple characters      */
#define DTXT_ANSI_FG_YELLOW  "\x1B[33;1m"      /**<Yellow characters      */
#define DTXT_ANSI_FG_DARKGRAY  "\x1B[30;1m"    /**<Dark gray characters   */
#define DTXT_ANSI_FG_LIGHTGRAY  "\x1B[37m"     /**<Light gray characters  */

/** ANSI escape sequence */
#define DTXT_ANSI_ATTR_RESET "\x1B[0m" /**<Attributes reset */


/* Macro to provide good coding rules naming */
#define DTXT_M_REPORT(T,C)                  do {DTXT_REPORT((T),(C))} while (0)                 /**< Tracing macro */
#define DTXT_M_REPORT1(T,C,W)               do {DTXT_REPORT1((T),(C),(W))} while (0)              /**< Tracing macro */
#define DTXT_M_REPORT2(T,C,W,X)             do {DTXT_REPORT2((T),(C),(W),(X))} while (0)            /**< Tracing macro */
#define DTXT_M_REPORT3(T,C,W,X,Y)           do {DTXT_REPORT3((T),(C),(W),(X),(Y))} while (0)          /**< Tracing macro */
#define DTXT_M_REPORT4(T,C,W,X,Y,Z)         do {DTXT_REPORT4((T),(C),(W),(X),(Y),(Z))} while (0)        /**< Tracing macro */
#define DTXT_M_REPORT5(T,C,W,X,Y,Z,A)       do {DTXT_REPORT5((T),(C),(W),(X),(Y),(Z),(A))} while (0)      /**< Tracing macro */
#define DTXT_M_REPORT6(T,C,W,X,Y,Z,A,B)     do {DTXT_REPORT6((T),(C),(W),(X),(Y),(Z),(A),(B))} while (0)    /**< Tracing macro */
#define DTXT_C_ANSI_FG_BLACK                DTXT_ANSI_FG_BLACK              /**<Black characters       */
#define DTXT_C_ANSI_FG_RED                  DTXT_ANSI_FG_RED                /**<Red characters         */
#define DTXT_C_ANSI_FG_GREEN                DTXT_ANSI_FG_GREEN              /**<Green characters       */
#define DTXT_C_ANSI_FG_BROWN                DTXT_ANSI_FG_BROWN              /**<Brown characters       */
#define DTXT_C_ANSI_FG_BLUE                 DTXT_ANSI_FG_BLUE               /**<Blue characters       */
#define DTXT_C_ANSI_FG_MAGENTA              DTXT_ANSI_FG_MAGENTA            /**<Magenta characters       */
#define DTXT_C_ANSI_FG_CYAN                 DTXT_ANSI_FG_CYAN               /**<Cyan characters       */
#define DTXT_C_ANSI_FG_WHITE                DTXT_ANSI_FG_WHITE              /**<White characters       */
#define DTXT_C_ANSI_FG_BRIGHTBLUE           DTXT_ANSI_FG_BRIGHTBLUE         /**<Bright blue characters       */
#define DTXT_C_ANSI_FG_BRIGHTCYAN           DTXT_ANSI_FG_BRIGHTCYAN         /**<Bright cyan characters       */
#define DTXT_C_ANSI_FG_BRIGHTGREEN          DTXT_ANSI_FG_BRIGHTGREEN        /**<Bright green characters       */
#define DTXT_C_ANSI_FG_BRIGHTRED            DTXT_ANSI_FG_BRIGHTRED          /**<Bright red characters       */
#define DTXT_C_ANSI_FG_PURPLE               DTXT_ANSI_FG_PURPLE             /**<Bright purple characters       */
#define DTXT_C_ANSI_FG_YELLOW               DTXT_ANSI_FG_YELLOW             /**<Yellow characters       */
#define DTXT_C_ANSI_FG_DARKGRAY             DTXT_ANSI_FG_DARKGRAY           /**<Darkgray characters       */
#define DTXT_C_ANSI_FG_LIGHTGRAY            DTXT_ANSI_FG_LIGHTGRAY          /**<Light brown characters       */

/** ANSI escape sequence */
#define DTXT_C_ANSI_ATTR_RESET             DTXT_ANSI_ATTR_RESET            /**<Attributes reset */



/******************************************************************************/
/*  TYPE DEFINITION                                                           */
/******************************************************************************/
/** Define type of report output. This could be used for filtering
 *  purpose or just display */
typedef enum
{
    DTXT_REPORT_CRITICAL    = 0u, /**<Critical level trace       */
    DTXT_REPORT_ERROR       = 1u, /**<Error level trace          */
    DTXT_REPORT_WARNING     = 2u, /**<Warning level trace        */
    DTXT_REPORT_INFO        = 3u, /**<Information level trace    */
    DTXT_REPORT_INTEGRATION = 4u  /**<Information of integration */
} DTXT_E_ReportType;

/******************************************************************************/
/*  GLOBAL VARIABLES OF MODULE                                               */
/******************************************************************************/

/******************************************************************************/
/*  FUNCTIONS PROTOTYPE                                                       */
/******************************************************************************/

/**
 *******************************************************************************
 * \par Description :
 *      Standard printf function. \n
 *
 *      See STD_F_Snprintf() function for details of the format used.
 *      The printed string is also given to the guest function
 *      GUEST_F_On_Printf().
 *
 * \par Fault cases:
 *      - EVT_C_NULL_PTR_ERROR: In case of fmt arg pointer is NULL.
 *
 * \param fmt    - in - Range: [!NULL]\n
 *        Pointer to string Format
 * \param ... - in - Range: None\n
 *        Variable argument list length. This is normally forbidden but this is
 *        the normal standard printf API.
 *
 * \return INT32S:
 *   - Number of characters written.
 *
 * \note This function shall be used only for debugging purpose. This function
 *       have direct access to hardware cells. No scheduling is involved during
 *       trace.
 *
 * \note While UART is not initialized, the function does not print any character
 *       and always return 0. Furthermore the function GUEST_F_On_Printf() is not
 *       called. Situations where this function is called with UART uninitialized
 *       can only comes from Guest application code, and not from Kernel code.
 *       In this situations most of the kernel is still uninitialized and the
 *       call of function GUEST_F_On_Printf() would useless.
 *
 * \attention This function might have a high impact on OS scheduling. Indeed, any
 *      call to this function is blocked till the access to the UART Hardware is
 *      free. If a large amount of data is being printed by a DTXT_REPORT at the
 *      time this function is called, the Hardware will be busy for a long time.
 *
 * \see STD_F_Snprintf()
 ******************************************************************************/
/*"This function must have a variable number of arguments\
                            as this ability is a key feature of Printf.\
                            Maintainability (which is the aim of this rule)\
                            is not impacted as this implementation is the standard one."
*/
INT32S DTXT_F_Printf(const CHAR *fmt, ...);


/******************************************************************************/
/**
 * \par Description:
 *      Print the requested information after reformatting. Depending of the
 *      CPU the print is:
 *      - CPU A : Send to DTXT Task for UART print.
 *      - CPU B : Send through 2oo2 inter CPU link to CPU A DTXT task
 *      \n
 *      \n
 *      This function SHALL not be called directly, the macro DTXT_REPORT and
 *      DTXT_REPORTx shall be used instead to provide localization.
 *
 * \par Fault cases:
 *      - Queue post error
 *      - Flag post error
 *      - Memput error
 *
 * \param Type    - in - range: [DTXT_REPORT_CRITICAL..DTXT_REPORT_INFO]\n
 *        Report Type, can be used later for filtering.
 * \param locString  - in - range: Not NULL\n
 *        Char string, terminated by "\0". Is a location "file,line" build by DTXT_REPORT macros.
 * \param String  - in - range: Not NULL\n
 *        Char string, terminated by "\0". This string will not be manipulated, it will be directly copied.
 * \param Numbval - in - range: [0..6]\n
 *        Number of value to print. (Validity of next parameters)
 * \param Value1  - in - range: None\n
 *        First value to be printed after String in hex format.
 * \param Value2  - in - range: None\n
 *        Second value to be printed after String in hex format.
 * \param Value3  - in - range: None\n
 *        Third value to be printed after String in hex format.
 * \param Value4  - in - range: None\n
 *        Fourth value to be printed after String in hex format.
 * \param Value5  - in - range: None\n
 *        Fifth value to be printed after String in hex format.
 * \param Value6  - in - range: None\n
 *        Sixth value to be printed after String in hex format.
 *
 * \note This module is for debug only. In case of concurrent call, some frames
 *       could be corrupted, especially for lost characters management. No safety
 *       impact.
 ******************************************************************************/
void DTXT_F_Report(DTXT_E_ReportType Type,
                   const char*            locString,
                   const char*            String,
                   INT8U            Numbval,
                   INT32U           Value1,
                   INT32U           Value2,
                   INT32U           Value3,
                   INT32U           Value4,
                   INT32U           Value5,
                   INT32U           Value6);

/**\}*/
#endif /* DTXT_H */

/* EOF */
