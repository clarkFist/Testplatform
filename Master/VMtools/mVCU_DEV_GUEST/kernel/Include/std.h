/******************************************************************************
 * COPYRIGHT (C) Alstom Transport 2008. STEP Project. All rights reserved.
 *****************************************************************************/

/******************** FILE HEADER *******************************************/
/**
 ******************************************************************************
 * \if API
 * \mainpage STD module API
 *
 * \section Description
 * This module is a C - library which provides standard C function for
 * software developpement. It provides customized, safety function normally
 * provided by stdlib.h and stdio.h and others tool case functions.
 *
 * \section Features
 * Features are:
 *  - String formating
 *  - Memory and String manipulation
 *  - Endianness swap on 16 and 32 bits
 *
 * \section V_Global Global variables used: None
 *
 * \section lim Limitations
 * - This library supports only ASCII strings, no UNICODE or other charset are
 *   supported.
 * - CR2332: The "STD_F_Strtoul" API has known issues and therefore shall not be
 *   used in this release.
 *
 * \endif
 *****************************************************************************/
/**\}*/
/**\}*/
#ifndef STD_H
#define STD_H
#include "datatypes.h"
#include "step_constant.h"
/**\addtogroup STD_INTERFACE
 * \{*/
/******************************************************************************
 * CONSTANTS OF MODULE
 *****************************************************************************/
/** Minimum value for a INT32U variable */
#define STD_C_INT32U_MIN_VAL      0u
/** Maximum value for a INT32U variable */
#define STD_C_INT32U_MAX_VAL      0xFFFFFFFFu
/* CR_Reference 2781 - Add Maximum value for a INT16U variable for overflow assertion */
/** Maximum value for a INT16U variable */
#define STD_C_INT16U_MAX_VAL      0xFFFFu

/** Maximum value for a INT8U variable */
#define STD_C_INT8U_MAX_VAL      0xFFu


/******************************************************************************
 * MACROS DEFINITION
 *****************************************************************************/
/** Macro to swap endianness on 32 bits */
/* Implements <STEP_SwAD_0287> */
/* "Definition of macro modifying parameters" */
#define STD_M_SWAP_ENDIANNESS_DATA32(_VAL) ((INT32U)(((_VAL)>>24U) & 0x000000FFU) | \
                                            (INT32U)(((_VAL)>>8U)  & 0x0000FF00U) | \
                                            (((INT32U)(_VAL)<<8U)  & 0x00FF0000U) | \
                                            (((INT32U)(_VAL)<<24U) & 0xFF000000U))

/** Macro to swap endianness on 16 bits */
/* Implements <STEP_SwAD_0287> */
/* "Definition of macro modifying parameters" */
#define STD_M_SWAP_ENDIANNESS_DATA16(_VAL) ((INT16U)(((_VAL) & 0xFF00U) >> 8U) | \
                                            (INT16U)(((INT32U)(_VAL) & 0x00FFU) << 8U))

/** Macro to increment a 32 bits statistics variable taking care of overflow */
#define STD_M_INT32U_INC(var)     do { if ((var) < STD_C_INT32U_MAX_VAL) { (var)++; } } while (0)

/** Macro to increment a 16 bits statistics variable taking care of overflow */
#define STD_M_INT16U_INC(var)     do { if ((var) < STD_C_INT16U_MAX_VAL) { (var)++; } } while (0)

/** Macro to increment a 8 bits statistics variable taking care of overflow */
#define STD_M_INT8U_INC(var)     do { if ((var) < STD_C_INT8U_MAX_VAL) { (var)++; } } while (0)

/* CR_reference 2022 - Stat module */
/** Macro to get the minimum of two values */
#define STD_M_MIN(val_dest, val_a, val_b) do { if ((val_a) < (val_b)) \
            { (val_dest) = (val_a); } else \
        { (val_dest) = (val_b); } } while (0)

/* CR_reference 2022 - Stat module */
/** Macro to get the maximum of two values */
#define STD_M_MAX(val_dest, val_a, val_b) do { if ((val_a) > (val_b)) \
            { (val_dest) = (val_a); } else \
        { (val_dest) = (val_b); } } while (0)

/* CR_reference 2022 - Stat module */
/** Macro to add two values without overflow. The result is capped to the STD_C_INT32U_MAX_VAL */
#define STD_M_INT32U_ADD(val_dest, val_a, val_b) do { if ((val_a) <= (STD_C_INT32U_MAX_VAL - (val_b))) \
            { (val_dest) = (val_a) + (val_b); } else \
        { (val_dest) = STD_C_INT32U_MAX_VAL; } } while (0)

/**
 * Use this macro to do static check (at the compilation time).
 * It has to be placed in a compiled function (not in dead code!). The best choice is Init or Create functions.
 *
 * This macro leads to a compilation error if the _statement is TRUE: "case constant defined more than once"
 * This macro does not raise QAC error or compilation warning if the _statement is false.
 */
#define STD_M_STATIC_ASSERT(_statement) do{                          \
        switch(0){              \
            case 0:             \
            case !(_statement): \
            default:break;      \
        }                       \
    }while(0)

/*把4个字符转换为1个INT32U数*/
#define STD_M_ATO_INT32U(val_a, val_b, val_c, val_d) ((((INT32U)(val_a) << 24u) & 0xFF000000u) | (((INT32U)(val_b) << 16u) & 0xFF0000u) | (((INT32U)(val_c) << 8u) & 0xFF00u) | ((INT32U)(val_d) & 0xFFu))

/*把2个字符转换为1个INT16U数*/
#define STD_M_ATO_INT16U(val_a, val_b) ((INT16U)(((INT32U)(val_a) << 8u) & 0xFF00u) | (INT16U)((INT32U)(val_b) & 0xFFu))
/******************************************************************************
 *  TYPE DEFINITION
 *****************************************************************************/
/** ANSI-C compatible STD_T_Size definition */
typedef INT32U STD_T_Size;

/** define a parameter list for printf functions*/
typedef char* STD_T_VAList;

/** base used for the Strtoul function */
typedef enum
{
    STD_C_BINARY_BASE = 0x0u,       /**< Binary base */
    STD_C_OCTAL_BASE = 0x1u,        /**< Octal base */
    STD_C_DECIMAL_BASE = 0x2u,      /**< Decimal base */
    STD_C_HEXA_BASE = 0x3u,         /**< Hexa-decimal base */
    STD_C_FORBIDDEN_BASE = 0x4u      /**< Enum used for range checking */
}STD_E_INT_BASE;

/******************************************************************************
 *  GLOBAL VARIABLES OF MODULE
 *****************************************************************************/


/******************************************************************************
 *  API FUNCTION PROTOTYPE
 *****************************************************************************/
/**
 *****************************************************************************
 * \par Description :
 * Standard snprintf function. \n
 * This function produces output according to a format as described below,
 * the output is written to the provided string.\n
 *
 * The function does not write more than size characters (including
 * leading '\\0'. If trunc occurs, it returns the number of chars written
 * except the leading '\\0'.
 *
 * \par Format:
 * See standard printf format for details of formats definition.\n \n
 * The format string is composed of zero or more directives: ordinary char
 * (not %), which are copied to the output stream; and conversion each of
 * which results in fetching zero or more subsequent arguments. '%' introduce
 * a conversion. Only the sequence "%d" is officialy supported. However the
 * following could also be working but without warranty:
 *      - (Optional) Zero or more of the following flags:
 *          - '-': Left justifies the field
 *          - '+': Always prefixes a signed value with a sign (+ or -).
 *          - ' ': Reserves a character position at the start of the string for the minus sign (for negative numbers) or a space (for positive numbers)
 *          - '0': pads numeric values with leading zeros
 *          - '#'! prefixes octal values with 0 and hexadecimal values with 0x or 0X
 *          .
 *      - (Optional) Field width in '0..9' ascii char
 *      - (Optional) char '.' defining a precision (but precision is not
 *        taken into account for the rendering, only for compatibility with
 *        standard format.
 *          - precision in '0..9' ascii char
 *          .
 *      - (Optional) Length modifier (not taken into account for the rendering,
 *        only for compatibility with standard format).
 *          - 'h'
 *          - 'l'
 *          - 'L'
 *          .
 *      - Format
 *          - 'd' or 'i': converted to Signed 32 bits INT, printed in Decimal.
 *          - 'x' or 'X': converted to Unsigned 32 bits INT, printed in hexa.
 *          - 'o': converted to Unsigned 32 bits INT, printed in octal.
 *          - 'b': converted to Unsigned 32 bits INT, printed in binary.
 *          - 'p': converted to Unsigned 32 bits INT, printed in hexa.
 *          - 'u': converted to Unsigned 32 bits INT, printed in decimal.
 *          - 'c': converted to CHAR, and result char is printed.
 *          - 's': printed as a string of char.
 *          - 'n': the number of char written so far is stored in the pointer argument.
 *          - other: printed as a string of char.
 *          .
 *      .
 *
 * \par Fault cases:
 *      - EVT_C_NULL_PTR_ERROR if s is NULL,
 *      - EVT_C_NULL_PTR_ERROR if fmt is NULL,
 *      - EVT_C_STDIO_PRINTK: Defensive programming against imposible case at
 *         STD_F_Snprintf level.
 *
 * \param s - In - Range: [!NULL]         \n
 *        Pointer to string to write in.
 * \param size - In - Range: None.         \n
 *        Max number of char to print into string
 * \param fmt - In - Range: [!NULL]       \n
 *        Pointer to string Format
 * \param va_arg - In - Range: None        \n
 *        Variable argument list length. This is normaly forbidden but this is
 *        the normal standard printf API.
 *
 * \return INT32S:
 *   - Number of char written.
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 * \see STD_F_VSnprintf()
 *****************************************************************************/
/* Implements <STEP_SwAD_0285> */
/* "The rule is not followed as this is a standard API function. There is no safety issue as this is only for string management which is used for maintenance only" */
INT32S STD_F_Snprintf(CHAR *s, STD_T_Size size, const CHAR *fmt, ...);

/**
 *****************************************************************************
 * \par Description :
 * Standard vSnprintf function. \n
 * This function produces output according to a format as described below,
 * the output is written to the provided string.\n
 *
 * The function does not write more than size characters (including
 * leading '\\0'. If trunc occurs, it returns the number of chars written
 * except the leading '\\0'.
 *
 * \par Format:
 * See STD_F_Snprintf() function for details of the format used.
 *
 * \par Fault cases:
 *      - EVT_C_NULL_PTR_ERROR if s is NULL,
 *      - EVT_C_NULL_PTR_ERROR if fmt is NULL,
 *      - EVT_C_STDIO_PRINTK: Defensive programming against imposible case at
 *         STD_F_Snprintf level.
 *
 * \param s - In - Range: [!NULL]         \n
 *        Pointer to string to write in.
 * \param size - In - Range: None.         \n
 *        Max number of char to print into string
 * \param fmt - In - Range: [!NULL]       \n
 *        Pointer to string Format
 * \param ap - In/Out - Range: [!NULL]     \n
 *        Pointer to a list of parameter
 *
 * \return INT32S:
 *   - Number of char written.
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 * \see STD_F_Snprintf()
 *****************************************************************************/
/* Implements <STEP_SwAD_0285> */
INT32S STD_F_VSnprintf(CHAR *s, STD_T_Size size, const CHAR *fmt, STD_T_VAList ap);


/**
 *****************************************************************************
 * \par Description :
 *      Find the length of a string.
 *
 * \par Fault cases:
 *      - none
 *
 * \param str - In - Range: [!NULL]       \n
 *        Pointer to string which length shall be computed.
 *
 * \return INT32U:
 *   - Returns the number of characters that precede the terminating NULL character.
 *   - returns 0 if str is NULL
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *****************************************************************************/
/* Implements <STEP_SwAD_0285> */
INT32U STD_F_Strlen(const CHAR *str);

/**
 *****************************************************************************
 * \par Description :
 * Concatenate strings: append a copy of the null-terminated string src
 * to the end of the null-terminated string dest, then add a terminating '\\0'.
 * The string dest must have sufficient space to hold the result. The function
 * appends not more than n characters from src. This function always write as
 * last char '\\0' to avoid overflow.
 *
 * \par Fault cases :
 *      - EVT_C_NULL_PTR_ERROR: In case of src or dest pointer is NULL.
 *      - EVT_C_RANGE_ERROR: if the number of char to concatenate is lesser or equal to 0.
 *
 * \param src - In - Range: [!NULL]       \n
 *        Pointer to source string.
 * \param dest - Out - Range: [!NULL]       \n
 *        Pointer to destination string.
 * \param n - In - Range: strictly positive \n
 *        Number of char to concatenate at max
 *
 * \return CHAR*:
 *   - Returns the pointer to the destination string.
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 * \see STD_F_Strlen() STD_F_Strncpy()
 *****************************************************************************/
/* Implements <STEP_SwAD_0285> */
CHAR * STD_F_Strncat(CHAR *dest, const CHAR *src, INT32U n);

/**
 *****************************************************************************
 * \par Description :
 * The function copies from src into dst, number of char copied is the minimun
 * of src length or n. This function always write as last char '\\0'.
 *
 * \par Fault cases :
 *      - EVT_C_NULL_PTR_ERROR: In case of src or dest pointer is NULL.
 * - EVT_C_RANGE_ERROR: if the number of char to copy is lesser or equal to 0.
 *
 * \param src - In - Range: [!NULL]       \n
 *        Pointer to source string.
 * \param dest - Out - Range: [!NULL]       \n
 *        Pointer to destination string.
 * \param n - In - Range: strictly positive \n
 *        Number of char to copy at max
 *
 * \return CHAR*:
 *   - Returns the pointer to the destination string.
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 * \see STD_F_Strlen() STD_F_Strncat()
 *****************************************************************************/
/* Implements <STEP_SwAD_0285> */
CHAR * STD_F_Strncpy(CHAR *dest, const CHAR *src, INT32U n);

/**
 *****************************************************************************
 * \par Description :
 * Compares two strings, the comparaison ends if a char '\\0' is seen or
 * that the number of compared char reach the value n.
 *
 * \par Fault cases :
 *      - EVT_C_NULL_PTR_ERROR: In case of pSrc1 or pSrc2 pointer is NULL.
 * - EVT_C_RANGE_ERROR: if the number of char to compare is lesser or equal to 0.
 *
 * \param pSrc1 - In - Range: [!NULL]       \n
 *        Pointer to source string.
 * \param pSrc2 - In - Range: [!NULL]       \n
 *        Pointer to destination string.
 * \param n - In - Range: None. \n
 *        Number of char to compare at max
 *
 * \return INT32S:
 *  Returns an unsigned 32 bits INT representing that the string s1 is greater
 *  than(+1), equal to (0), or less (-1) than the string s2. The comparison is
 *  done using signed characters.
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 * \see STD_F_Strlen()
 *****************************************************************************/
/* Implements <STEP_SwAD_0285> */
INT32S STD_F_Strncmp(const CHAR *pSrc1, const CHAR *pSrc2, INT32U n);

/**
 *****************************************************************************
 * \par Description :
 * Copies from src into dest, number of byte copied is n. Before starting to
 * copy, checks that the length of the destination buffer is enough using
 * the size which is provided in parameter.
 *
 * \par Fault cases :
 *      - EVT_C_NULL_PTR_ERROR: In case of pSrc or pDest pointer is NULL.
 *      - EVT_C_RANGE_ERROR: In case of length of dest is smaller than n
 *        (size of copied data).
 *      - EVT_C_OVERLAPPING_ERROR: In case of src/dest memory overlapping.
 *
 * \param pDest - Out - Range: [!NULL]       \n
 *        Pointer to destination byte array.
 * \param len - In - Range: None  \n
 *        Length of the dest buffer.
 * \param pSrc - In - Range: [!NULL]       \n
 *        Pointer to source byte array.
 * \param n - In - Range: None  \n
 *        Number of bytes to copy.
 *
 * \return void
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 * \note
 *   - Copy is optimized, first data are copied by 32 bits and copy is finished
 *     using bytes to ensure no overrun.
 *
 * \see STD_F_Memset() STD_F_Memcmp()
 *****************************************************************************/
/* Implements <STEP_SwAD_0286> */
void STD_F_Memcpy(void *pDest, INT32U len, const void *pSrc, INT32U n);

/**
 ******************************************************************************
 * \par Description:
 *      This function transforms all upper cases in string in lower cases.
 *      If string given is bigger than n parameters, only the n first characters
 *      will be change if needed.
 *
 * \par Fault cases:
 *      - EVT_C_NULL_PTR_ERROR: In case of pStr is NULL.
 *
 * \par Pseudo-code:
 *
 * \par Safety Aspects:
 *      None
 *
 * \param pStr - In/Out - Range: [!null] \n
 *              String that will be changed by the function if needed
 * \param n - In - Range: None  \n
 *        Number of bytes to change if string pStr bigger than n.
 *
 *****************************************************************************/
/* Implements <STEP_SwAD_0285> */
void STD_F_StrnLowerCaseSet(CHAR * pStr, INT32U n);

/**
 *****************************************************************************
 * \par Description :
 * Set value into dest, number of byte set is n. Before starting to
 * set value, checks that the length of the destination buffer is enough using
 * the size which is provided in parameter.
 *
 * \par Fault cases :
 *      - EVT_C_NULL_PTR_ERROR: In case of dest pointer is NULL.
 *
 * \param pDest - Out - Range: [!NULL]       \n
 *        Pointer to destination byte array.
 * \param c - In - None       \n
 *        Value to set.
 * \param n - In - Range: None \n
 *        Number of bytes to set.
 *
 * \return void
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 * \see STD_F_Memcpy() STD_F_Memcmp()
 *****************************************************************************/
/* Implements <STEP_SwAD_0286> */
void STD_F_Memset(void *pDest, INT8U c, INT32U n);

/**
 *****************************************************************************
 * \par Description :
 * Compare n bytes of two buffers.
 *
 * \par Fault cases :
 *      - EVT_C_NULL_PTR_ERROR: In case of src1 or src2 pointer is NULL.
 *
 * \param src1 - In - [!NULL]       \n
 *        Pointer to source1 buffer.
* \param src2 - In - [!NULL]       \n
 *        Pointer to source2 buffer.
 * \param n - In - Range: None \n
 *        Number of bytes to compare.
 *
 * \return INT32S:
 *  - Zero if the two buffers are identical, otherwise returns the difference between the first two differing bytes
 *
 * \if SwMD
 * \par Safety Aspects:
 *              - No Safety Aspects
 * \endif
 *
 * \see STD_F_Memcpy() STD_F_Memset()
 *****************************************************************************/
/* Implements <STEP_SwAD_0286> */
INT32S STD_F_Memcmp(const void * src1, const void * src2, INT32U n);

/**
 *****************************************************************************
 * \par Description :
 *      Convert an ASCII string into an integer.
 *      The integer value is calculated according to :
 *          - the max length
 *          - the base.
 *
 * \par Fault cases :
 *      - Null pointer on pStr and pInteger \n
 *
 * \param pStr - In - Range : [!NULL] \n
 *        Pointer to the ASCII string
 *
 * \param maxlen - In - Range : None \n
 *        Max length authorized for the integer computation.
 *
 * \param endptr - In/Out - Range : None \n
 *        If not null, endptr will point on the last char used for
 *        integer computation. In case of error during process, endptr
 *        will point on the beginning of ASCII string.
 *
 * \param base - In - Range : [STD_C_BINAIRE_BASE..STD_C_HEXA_BASE] \n
 *        base to use for integer computation.
 *
 * \param pInteger - In/Out - Range : [!NULL] \n
 *        pointer to the calculated integer.
 *
 * \return ALSTOM_C_NO_ERROR if integer was well computed.
 * ALSTOM_C_ERROR otherwise.
 *****************************************************************************/
ALSTOM_T_Status STD_F_Strtoul(const CHAR * pStr, INT32U maxlen, CHAR ** endptr,
                              STD_E_INT_BASE base, INT32U* pInteger);

/**
 ******************************************************************************
 * \par Description:
 *      This function realize a pause (sleep) in a generic way. If the OS is
 *      started and the current execution is not in interrupt context, then
 *      the function OSTimeDly() from OS is called. Otherwise function
 *      STD_F_WaitOneMs() is used.
 *
 * \par Fault cases:
 *      None
 *
 * \par Pseudo-code:
 *      -# Check if OS is started
 *      -# Check if interrupts are masked
 *      -# If OS not started or if interrupts are masked
 *         -# call STD_F_WaitOneMs() ms time
 *      -# Else
 *         -# Call OSTimeDly()
 *
 * \par Safety Aspects:
 *      None
 *
 * \param ms - Int - Range: none \n
 *              Time of the pause in ms
 *
 *****************************************************************************/
/* Implements <STEP_SwAD_0285> */
void STD_F_GenericPause(INT16U ms);

/**\}*/ /*End of STD_INTERFACE*/

#endif /* STD_H */

/* EOF */

