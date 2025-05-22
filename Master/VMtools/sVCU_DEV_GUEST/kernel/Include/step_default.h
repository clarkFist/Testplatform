/******************************************************************************
 * COPYRIGHT (C) Alstom Transport 2008. STEP Project. All rights reserved.
 *****************************************************************************/

/******************** FILE HEADER *******************************************/
/**\addtogroup MSTEP_INTERFACE
 * \{*/
/**
 ******************************************************************************
 * \if API
 * \page Step Default API
 *
 * \section Description
 * This header defines the event Id available on guest application.
 *
 * \section Limitations
 * Id of the first event shall be equal to 0. MSTEP event Id are limited to 190.
 *
 * \endif
 *****************************************************************************/
/**\}*/

#ifndef _step_default_h_
#define _step_default_h_

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
/*
 * Event code definition
 * Before adding a new event, please read the following instructions:
 *      - Add the new default BEFORE the comment that allowed to,
 *      - Increase the EVT_C_NBDEFAUT and check that it is lower than 192,
 *      - Define the actions joined to the new event in gdf.c.
 */

#define EVT_C_OS_UNDEFINED                   (GDF_T_EventId)0u  /**<Undefined error occurs          */
#define EVT_C_OS_ERROR                       (GDF_T_EventId)1u  /**<An Os error occurs              */
/*Specific K2oo2 errors*/
#define EVT_C_K2OO2_ERROR                    (GDF_T_EventId)2u  /**<Generic K2oo2 error           */
#define EVT_C_UDPSOCK_ERROR                  (GDF_T_EventId)3u  /**<UDP Socket error              */
#define EVT_C_SOCKET_ERROR                   (GDF_T_EventId)4u  /**<Generic socket error          */
#define EVT_C_K2OO2_INT_ERROR                (GDF_T_EventId)5u  /**<K2oo2 interrupt error         */
#define EVT_C_K2OO2_TIMEOUT_ERROR            (GDF_T_EventId)6u  /**<K2oo2 time out error          */
#define EVT_C_K2OO2_HARDWARE_ERROR           (GDF_T_EventId)7u  /**<K2oo2 hardware cell error     */
#define EVT_C_K2OO2_CHARLOST                 (GDF_T_EventId)8u  /**<Character lost on interCPU link */
/*Errors by particular modules*/
#define EVT_C_MSGQ_ERROR                     (GDF_T_EventId)9u  /**<Error on FIFO                   */
#define EVT_C_MODE_SWITCH2NORMAL             (GDF_T_EventId)10u /**<STEPPER board redundancy switch */
#define EVT_C_IPSTACK_ERROR                  (GDF_T_EventId)11u /**<Error on module IPStack         */
#define EVT_C_MODE_ERROR                     (GDF_T_EventId)12u /**<MODE task error                 */
/* Errors by types*/
/* BE CAREFUL: Voluntary no EventId equals to 13 and 14
   used for EVT_C_NULL_PTR_ERROR     -> 13 and
            EVT_C_OUT_OF_RANGE_ERROR -> 14 */
#define EVT_C_TASK_NOT_STARTED_ERROR         (GDF_T_EventId)15u  /**< Task did not start                                           */
#define EVT_C_MEM_GET_ERROR                  (GDF_T_EventId)16u  /**< Error on a memGet                                            */
#define EVT_C_CONFIG_ERROR                   (GDF_T_EventId)17u  /**< Error from a configuration problem                           */
#define EVT_C_INIT_ERROR                     (GDF_T_EventId)18u  /**< Error at the end of task init                                */
#define EVT_C_SAFETY_CORRUPTED               (GDF_T_EventId)19u  /**< Data is corrupted. case impossible, post in a bad FIFO...    */
#define EVT_C_ANORMAL_TIMEOUT                (GDF_T_EventId)20u  /**< Data is corrupted. case impossible, post in a bad FIFO...    */
#define EVT_C_POST_ERROR                     (GDF_T_EventId)21u  /**< Data is corrupted. case impossible, post in a bad FIFO...    */
#define EVT_C_SEM_ERROR                      (GDF_T_EventId)22u  /**< Data is corrupted. case impossible, post in a bad FIFO...    */
#define EVT_C_CPU_SYNC_ERROR                 (GDF_T_EventId)23u  /**< A problem occured in a CPU exchange                          */
#define EVT_C_TASK_DEAD_ERROR                (GDF_T_EventId)24u  /**< The task is dead                                             */
#define EVT_C_BOOT_DONE                      (GDF_T_EventId)25u  /**< Boot up is performed                                         */
#define EVT_C_HOT_REBOOT                     (GDF_T_EventId)26u  /**< A hot reboot was performed                                   */
#define EVT_C_EXCEPTION                      (GDF_T_EventId)27u  /**< An exception was done                                        */
#define EVT_C_NVMEM_ERROR                    (GDF_T_EventId)28u  /**< Nvram Is Corrupted                                           */
#define EVT_C_WRITING_IN_OBD                 (GDF_T_EventId)29u  /**< An error occurs when writing in OBD */
#define EVT_C_CAN_MEDIUM_SWITCH              (GDF_T_EventId)30u  /**< Medium switch requested             */
#define EVT_C_CAN_SPURIOUS_SYNC              (GDF_T_EventId)31u  /**< Medium switch requested             */
#define EVT_C_FUSE_NOT_BURNED                (GDF_T_EventId)32u  /**< This event has been removed and only kept to not shift other event numbers */
#define EVT_C_CAN_PREVIOUS_CYCLE_NOT_ENDED   (GDF_T_EventId)33u  /**< PDO FIFO not empty at new cycle  */
#define EVT_C_CAN_TWO_MASTER                 (GDF_T_EventId)34u  /**< 2 master detected on CAN bus      */
#define EVT_C_BIST_LOW_STACK                 (GDF_T_EventId)35u  /**< A Stack have been passed under limit of free allowed */
#define EVT_C_CAN_NO_FREE_TIME               (GDF_T_EventId)36u  /**< No Free time at the end of CAN cycle */
#define EVT_C_CAN_TWO_MEDIUM_OUT_OF_ORDER    (GDF_T_EventId)37u  /**< Both CAN buses are out of order */
#define EVT_C_OSQPEND_ERROR                  (GDF_T_EventId)38u  /**< Error on OSQPend */
#define EVT_C_OSQACCEPT_ERROR                (GDF_T_EventId)39u  /**< Error on OSQAccept */
#define EVT_C_BIST_EXCHANGE_ERROR            (GDF_T_EventId)40u  /**< Frame Exchange through intercpu in BIST Module in corrupted */
#define EVT_C_BIST_CRC_ERROR                 (GDF_T_EventId)41u  /**< Frame Exchange through intercpu in BIST Module in corrupted */
#define EVT_C_STDIO_PRINTK                   (GDF_T_EventId)42u  /**< printk_putc destination error */
#define EVT_C_ETHERNET_ERROR                 (GDF_T_EventId)43u  /**< Ethernet error (ip stack) */
#define EVT_C_DIVISION_BY_ZERO               (GDF_T_EventId)44u  /**< Division by zero */
#define EVT_C_WDG_SOFT_ERROR                 (GDF_T_EventId)45u  /**< WDG soft not well performed ... */
#define EVT_C_IODB_ERROR                     (GDF_T_EventId)46u  /**< IODB fatal error */
#define EVT_C_BAD_CPU_CODE                   (GDF_T_EventId)47u  /**< Cpu code compiled is flashed on the badcpu */
#define EVT_C_BIST_INCORRECT_TEST            (GDF_T_EventId)48u  /**< Incorrect test is performed in bist module */
#define EVT_C_IODB_TOO_MANY_FAKE_SYNC        (GDF_T_EventId)49u  /**< Too many consecutive fake SYNC   */
#define EVT_C_IDENT_DISCONNECT               (GDF_T_EventId)50u  /**< Pb of disconnection in Identifiation layer   */
#define EVT_C_IODB_BOTH_CPU_NOT_AGREE        (GDF_T_EventId)51u  /**< IODB datas to apply are different on both CPU       */
#define EVT_C_OBD_READ_ERROR                 (GDF_T_EventId)52u  /**< Object dictionary read error */
#define EVT_C_FLAG_ERROR                     (GDF_T_EventId)53u  /**< Error on OSSemPend*/
#define EVT_C_DCA_ERROR                      (GDF_T_EventId)54u  /**< Error in DCA module*/
#define EVT_C_OVERLAPPING_ERROR              (GDF_T_EventId)55u  /**< Pointer overlapping error in STD Library*/
#define EVT_C_GPIO_ERROR                     (GDF_T_EventId)56u  /**< Error during GPIO initializations*/
#define EVT_C_SPI_ERROR                      (GDF_T_EventId)57u  /**< Error in SPI module*/
#define EVT_C_PDA_ERROR                      (GDF_T_EventId)58u  /**< PDA error, data are lost*/
#define EVT_C_CAN_HB_SHIFT_ERROR             (GDF_T_EventId)59u  /**< HeartBeat of 2 CPU is out of range (CPU time shift exceed) */
#define EVT_C_CAN_SDO_ERROR                  (GDF_T_EventId)60u  /**< Fatal error occured during SDO transferts */
#define EVT_C_CONFIG_KEY_MISSING             (GDF_T_EventId)61u  /**< Configuration key missing */
#define EVT_C_CONFIG_KEY_CHANGE              (GDF_T_EventId)62u  /**< Configuration key change */
#define EVT_C_TFTP_ERROR                     (GDF_T_EventId)63u  /**< TFTP error */
#define EVT_C_K2002_DIVERGENCE               (GDF_T_EventId)64u  /**< Divergence between cycle counter or functional mode */
#define EVT_C_CAN_ARD                        (GDF_T_EventId)65u  /**< Error while manage can extended frames */
#define EVT_C_VIVISECTION_EXTERNAL_KILL      (GDF_T_EventId)66u  /**< External kill by vivisection tftp command */
#define EVT_C_MEM_PUT_ERROR                  (GDF_T_EventId)67u  /**< Error on a memPut                         */
#define EVT_C_CONFIG_RECONF_PAIRING_OK       (GDF_T_EventId)68u  /**< Restart after reconf and temporary pairing OK */
#define EVT_C_CONFIG_RECONF_SECTORS_OK       (GDF_T_EventId)69u  /**< All sector sent have been memorized correctly */
#define EVT_C_CONFIG_GLOBAL_AUTH_FAILED      (GDF_T_EventId)70u  /**< Global authorization failed */
#define EVT_C_FLASH_ERROR                    (GDF_T_EventId)71u  /**< Flash error */
#define EVT_C_SWITCH_TO_RESTRICTIVE          (GDF_T_EventId)72u  /**< Evt Switch to restrictive */
#define EVT_C_FS_ERROR                       (GDF_T_EventId)73u  /**< Error in FS */
/* CR_reference 2787 - Reboot possibility for MSTEP/IO board */
#define EVT_C_EXTERNAL_REBOOT                (GDF_T_EventId)74u  /**< External reboot command received */
#define EVT_C_EXTERNAL_UPDATE                (GDF_T_EventId)75u  /**< External update ok */
/* New defaults have to be added before this comment, then EVT_C_NBDEFAUT has to be increased*/
/* Do not forget to define the actions joined to the new event in gdf.c (array GDF_V_TabDefActionSTEP). */

#define EVT_C_NBDEFAUT                       (GDF_T_EventId)76u  /**< BE CAREFUL, this ID shall be <= 191 */

/******************************************************************************
 *  GLOBAL VARIABLES OF MODULE
 *****************************************************************************/

/******************************************************************************
 *  API FUNCTION PROTOTYPE
 *****************************************************************************/

/**\}*/ /*End of MSTEP_INTERFACE*/

#endif /* _step_default_h_ */

/* EOF */

