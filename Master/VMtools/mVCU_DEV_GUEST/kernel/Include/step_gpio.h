/******************************************************************************
 * COPYRIGHT (C) Alstom Transport 2008. STEP Project. All rights reserved.
 *****************************************************************************/

/******************** FILE HEADER *******************************************/
/**\addtogroup MSTEP_INTERFACE
 * \{*/
/**
 ******************************************************************************
 * \if API
 * \page Step GPIO API
 *
 * \section Description
 * This header defines configuration elements for the 16 GPIO available on the
 *  MSTEP board.
 *
 * \section Limitations
 * None
 *
 * \endif
 *****************************************************************************/
/**\}*/

#ifndef _guest_gpio_h_
#define _guest_gpio_h_
#include "datatypes.h"
/**\addtogroup MSTEP_INTERFACE
 * \{*/
/******************************************************************************
 * CONSTANTS OF MODULE
 *****************************************************************************/
/** Base Address for the memory dedicated to chip select 1*/
#ifdef ALSTOM_CONFIG_CPU_A
#define MSTEPINC_C_BASE_ADDR1   0x1000u
#else
#define MSTEPINC_C_BASE_ADDR1   0x1000u
#endif

/** Base Address for the memory dedicated to chip select 2*/
#ifdef ALSTOM_CONFIG_CPU_A
#define MSTEPINC_C_BASE_ADDR2   0x9000u
#else
#define MSTEPINC_C_BASE_ADDR2   0xC000u
#endif

/** Base Address for the memory dedicated to chip select 1 for software access */
#define MSTEPINC_C_BASE_ADDR1_SW ((INT32U)(MSTEPINC_C_BASE_ADDR1) << 16u)

/** Base Address for the memory dedicated to chip select 2 for software access */
#define MSTEPINC_C_BASE_ADDR2_SW ((INT32U)(MSTEPINC_C_BASE_ADDR2) << 16u)

/******************************************************************************
 * MACROS DEFINITION
 *****************************************************************************/


/******************************************************************************
 *  TYPE DEFINITION
 *****************************************************************************/

/** Definition of configurable GPIO available on MSTEP board for guest */

typedef enum
{
    MSTEPINC_C_GPIO_NO_ID  = 0u,  /**<GPIO No Id */
    MSTEPINC_C_GPIO_ID1    = 1u,  /**<GPIO  1 */
    MSTEPINC_C_GPIO_ID2    = 2u,  /**<GPIO  2 */
    MSTEPINC_C_GPIO_ID3    = 3u,  /**<GPIO  3 */
    MSTEPINC_C_GPIO_ID4    = 4u,  /**<GPIO  4 */
    MSTEPINC_C_GPIO_ID5    = 5u,  /**<GPIO  5 */
    MSTEPINC_C_GPIO_ID6    = 6u,  /**<GPIO  6 */
    MSTEPINC_C_GPIO_ID7    = 7u,  /**<GPIO  7 */
    MSTEPINC_C_GPIO_ID8    = 8u,  /**<GPIO  8 */
    MSTEPINC_C_GPIO_ID9    = 9u,  /**<GPIO  9 */
    MSTEPINC_C_GPIO_ID10   = 10u, /**<GPIO 10 */
    MSTEPINC_C_GPIO_ID11   = 11u, /**<GPIO 11 */
    MSTEPINC_C_GPIO_ID12   = 12u, /**<GPIO 11 */
    MSTEPINC_C_GPIO_ID13   = 13u, /**<GPIO 12 */
    MSTEPINC_C_GPIO_ID14   = 14u, /**<GPIO 14 */
    MSTEPINC_C_GPIO_ID15   = 15u, /**<GPIO 15 */
    MSTEPINC_C_GPIO_ID16   = 16u, /**<GPIO 16 */
    MSTEPINC_C_GPIO_NB_MAX = 17u  /**<Maximum number of GPIO*/
} MSTEPINC_E_GpioId;

/*20181218, Modify the code to make it compatible with VOOB16_1, CR:1246*/


/** Definition of direction of GPIO available on MSTEP board */
typedef enum
{
    MSTEPINC_C_GPIO_UNUSED = 0u,  /**<GPIO unused            */
    MSTEPINC_C_GPIO_INPUT  = 1u,  /**<General Purpose Input  */
    MSTEPINC_C_GPIO_OUTPUT = 2u   /**<General Purpose Output */
} MSTEPINC_E_GpioDirection;

/** Definition of the initial GPIO value when configured as an output */
typedef enum
{
    MSTEPINC_C_GPIO_ON        = 0u,   /**< At initialisation output GPIO is defined as ON */
    MSTEPINC_C_GPIO_OFF       = 1u,   /**< At initialisation output GPIO is set to OFF    */
    MSTEPINC_C_GPIO_DONT_CARE = 255u  /**< Used for input or output GPIO when initial value does not matter
                                      * in this case, output value is set to ON          */
} MSTEPINC_E_GpioInitValue;

/** Definition of the type returned by a GPIO reading */
typedef BOOLEAN MSTEPINC_T_GpioReadValue;

/** Validation flag parameters for configurable CS */
typedef enum
{
    MSTEPINC_C_CS_INVALID = 0u,  /**< Chip select invalid */
    MSTEPINC_C_CS_VALID   = 1u   /**< Chip select valid */
} MSTEPINC_E_ChipSelectValidation;

/** Complete definition of configurable GPIO available on MSTEP board */

typedef struct
{
    MSTEPINC_E_GpioId        id;        /**< Id of the GPIO               */
    MSTEPINC_E_GpioDirection direction; /**< Direction of the GPIO        */
    MSTEPINC_E_GpioInitValue initValue; /**< Initial value of output GPIO, this value is ignored for GPIO defined as intput */
} MSTEPINC_T_GpioDefinition;

/*20181218, Modify the code to make it compatible with VOOB16_1, CR:1246*/

/** Definition of configurable CS available on MSTEP board */
typedef enum
{
    MSTEPINC_C_CS_NO_ID   = 0u,  /**< Chip select No Id */
    MSTEPINC_C_CS_ID1     = 1u,  /**< Chip select 1. The associated base address is MSTEPINC_C_BASE_ADDR1. */
    MSTEPINC_C_CS_ID2     = 2u,  /**< Chip select 2. The associated base address is MSTEPINC_C_BASE_ADDR2. */
    MSTEPINC_C_CS_NB_MAX  = 3u   /**< Maximum number of chip select*/
} MSTEPINC_E_ChipSelectId;

/** Structure which initializes a chip select configuration (auto-acknowledge,
    port size, burst capability, and activation of guest chip select).
    Please, refer to MCF5235 Reference Manual,
Table 16-7 for more details. */

typedef struct
{
    /**Id of the Chip Select. The base address associated with this Id is decribed into
       MSTEPINC_E_ChipSelectId documentation*/
    MSTEPINC_E_ChipSelectId CSId;
    /**Valid bit.\n
     * Indicates whether the selected Chip Select shall be activated or not.
     *   - MSTEPINC_C_CS_INVALID: Chip select invalid
     *   - MSTEPINC_C_CS_VALID: Chip select valid and configured for read and write access */
    MSTEPINC_E_ChipSelectValidation valid;
    /** Base address mask (high order bits). Defines the chip select block by masking
        address bits.
        Setting a maskAddr bit causes the corresponding base address bit to be ignored
        in the decode (the base address is either MSTEPINC_C_BASE_ADDR1 or
        MSTEPINC_C_BASE_ADDR2 accoring to the configured chip select). \n
        For exemple a maskAddr value of 0x1F used for chip select 1 give the
        following configuration:
         - base address MSTEPINC_C_BASE_ADDR1
         - base address mask = (maskAddr << 16) |  0xFFFF = 0x1FFFFF
        So a given address will be managed by the chip select configuration if:
        givenAddress & ~0x1FFFFF = MSTEPINC_C_BASE_ADDR1.
        It corresponds to an address range of  2Mbytes.
        \n
        \note The software shall acess to the memory throuh MSTEPINC_C_BASE_ADDR1_SW address.
    */
    INT16U maskAddr;
    /** Secondary Read Wait States. /n
     * The number of wait states applied to all reads after the initial one if
     * properly enabled (AA = 1). The default value is 00.
     *   - 0: Secondary read wait states are disabled. Use IWS for all accesses.
     *   - 1: 0 wait states for the secondary read accesses
     *   - 2: 1 wait state for the secondary read accesses
     *   - 3: 2 wait states for the secondary read accesses*/
    INT8U SRWS;
    /** Initial Wait States. /n
     * The number of wait states inserted before an internal transfer acknowledge
     * is generated (IWS = 0 inserts zero wait states, IWS = 0xF inserts 15 wait
     * states). If AA = 0, /TA must be asserted by the external system regardless
     * of the number of wait states generated. In that case, the external transfer
     * acknowledge ends the cycle. An external /TA supercedes the generation of an
     * internal /TA.*/
    INT8U IWS;
    /** Auto-acknowledge enable. /n
     * Determines the assertion of the internal transfer acknowledge for accesses
     * specified by the chip select address.
     *   - 0: No internal /TA is asserted. Cycle is terminated externally.
     *   - 1: Internal /TA is asserted as specified by WS. Note that if AA = 1 for
     * a corresponding CSn and the external system asserts an external TA before
     * the wait-state countdown asserts the internal /TA, the cycle is terminated.
     * Burst cycles increment the address bus between each internal termination.*/
    BOOLEAN AA;
    /** Port size. \n
     * Specifies the width of the data associated with each chip select. It
     * determines where data is driven during write cycles and where data is
     * sampled during read cycles.
     *   - 0: 32-bit port size. Valid data sampled and driven on D[31:0]
     *   - 1:  8-bit port size. Valid data sampled and driven on D[31:24]
     *   - 2: 16-bit port size. Valid data sampled and driven on D[31:16]*/
    INT8U PS;
    /** Byte enable mode. \n
     * Specifies the byte enable operation. Certain SRAMs have byte enables
     * that must be asserted during reads as well as writes. BEM can be set in
     * the relevant CSCR to provide the appropriate mode of byte enable in
     * support of these SRAMs.
     *   - 0: BS is not asserted for read. BS is asserted for data write only.
     *   - 1: BS is asserted for read and write accesses. */
    BOOLEAN BEM;
    /** Burst read enable. \n
     *  Specifies whether burst reads are used for memory associated with each CSn.
     *    - 0: Data exceeding the specified port size is broken into individual,
     *          port-sized non-burst reads. For example, a longword read from an
     *          8-bit port is broken into four 8-bit reads.
     *    - 1: Enables data burst reads larger than the specified port size,
     *          including longword reads from 8- and 16-bit ports, word reads
     *          from 8-bit ports, and line reads from 8-, 16-, and 32-bit ports.*/
    BOOLEAN BSTR;
    /** Burst write enable. \n
     * Specifies whether burst writes are used for memory associated with each CSn.
     *   - 0: Break data larger than the specified port size into individual
     *      port-sized, non-burst writes. For example, a longword write to an
     *      8-bit port takes four byte writes.
     *   - 1: Enables burst write of data larger than the specified port size,
     *      including longword writes to 8 and 16-bit ports, word writes to 8-bit
     *      ports and line writes to 8-, 16-, and 32-bit ports.*/
    BOOLEAN BSTW;
    /** Secondary write wait states. \n
         * The number of wait states applied to all writes after the initial one if
         * properly enabled (SWWS is non-zero and CSCR[AA] = 1). The default for
         * this field is secondary write wait states disabled. This field is encoded as:
         *  - 0: Secondary write wait states are disabled. Use IWS for all accesses.
         *  - 1: 0 wait states for the secondary write accesses
         *  - 2: 1 wait state for the secondary write accesses
         *  - 3: 2 wait states for the secondary write accesses
         *  - 4: 3 wait states for the secondary write accesses
         *  - 5: 4 wait states for the secondary write accesses
         *  - 6: 5 wait states for the secondary write accesses
         *  - 7: 6 wait states for the secondary write accesses */
    INT8U SWWS;
}MSTEPINC_T_ChipSelectDefinition;



/******************************************************************************
 *  GLOBAL VARIABLES OF MODULE
 *****************************************************************************/


/******************************************************************************
 *  API FUNCTION PROTOTYPE
 *****************************************************************************/


/**\}*/ /*End of MSTEP_INTERFACE*/

#endif /* _guest_gpio_h_ */


/* EOF */
