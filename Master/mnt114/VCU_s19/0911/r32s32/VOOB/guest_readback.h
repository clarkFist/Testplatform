/**
* @file guest_readback.h
* @brief
* @author Wang Jiantao
* @date 2013-7-19
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: wangjiantao@casco.com.cn
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/

#ifndef __GUEST_READBACK__
#define __GUEST_READBACK__

/** Storage "Read-back state (set/unset/undetermined)". */
typedef enum
{
    GUEST_C_READBACK_SENSORS_UNSET = 0u,         /**Evaluate the read-backs shall consider read-back as unset (logical '0') if:Sum < THL.*/
    GUEST_C_READBACK_SENSORS_SET = 1u,           /**Evaluate the read-backs shall consider read-back as set (logical '1') if:Sum >= THH.*/
    GUEST_C_READBACK_SENSORS_UNDETERMINED = 3u,  /**Evaluate the read-backs shall consider read-back as undetermined if:THL <= Sum < THH.*/
}GUEST_E_READBACK_SENSORS_STATE;

/** Storage "Read-back status (OK/KO)". */
typedef enum
{
    GUEST_C_READBACK_SENSORS_KO = 0u,   /**Read-back status is KO.*/
    GUEST_C_READBACK_SENSORS_OK = 1u,   /**Read-back status is OK.*/
}GUEST_E_READBACK_SENSORS_STATUS;

/** Storage type of sensors. */
typedef enum
{
    GUEST_C_24V_PST          = 0u,               /**24V DC power supply readback sensors.*/
    GUEST_C_12V_PST          = 1u,               /**12V DC power supply readback sensors.*/
    GUEST_C_R_ISOT           = 2u,               /**Isolation cell collect sensors.*/

}GUST_E_SENSORS;

/** The kind data from FPGA */
typedef enum
{
    GUEST_C_RBV_SCT          = 0u,
    GUEST_C_RBV_RIT          = 1u,
    GUEST_C_RBV_TT           = 2u,
    GUEST_C_RBV_FUSET        = 3u,
    GUEST_C_RBV_OTHER        = 4u,

}GUST_E_RBV_KIND;

/** Storage "Read-back state (set/unset/undetermined)" and "status (OK/KO)" of sensors. */

typedef struct
{
    INT8U readbackState;
    INT8U readbackStatus;
} GUEST_T_READBACK_SENSORS;

extern ALSTOM_T_Status GUEST_F_RBValue(GUST_E_SENSORS RBSensorKind, INT8U EXPTValue, INT32U tskID);
extern void GUEST_F_OPT_RBValueFromFPGA(GUEST_E_READBACK_SENSORS_STATE * SumRBPValue,
                                            GUEST_E_READBACK_SENSORS_STATE * SumRBNValue);
extern void GUEST_F_MemoryBUSTest(void);

extern void GUEST_F_TSPSTForOPT(void);

#endif /* GUEST_READBACK */

/* EOF */
