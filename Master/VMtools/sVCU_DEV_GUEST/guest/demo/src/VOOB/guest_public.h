/**
* @file guest_public.h
* @brief
* @author Wang Jiantao
* @date 2013-5-22
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: wangjiantao@casco.com.cn
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/

#ifndef __GUEST_PUBLIC__
#define __GUEST_PUBLIC__

#include "guest_include.h"

#define VOOB16_E

#define GUEST_C_SW_VERSION_1                       0x01000200u        /* 软件版本1 */
#define GUEST_C_SW_VERSION_2                       0x20170331u        /* 软件版本2 */

/** Main task period definition */
#define GUEST_C_TASK_PERIOD                        5                 /**<Guest task period = 5*50ms */
/** Isolation test task period definition */
#define GUEST_C_TEST_TASK_PERIOD                   20                /**<Guest task period = 20*50ms */
#define GUEST_C_TASK_NON_PERIOD                    0                 /**<Guest non period task */

/** SW watchdog refresh period definition */
#define GUEST_C_SWWD_REFRESH_PERIOD                60                /**<Refresh period: 60*5 ms */
#define GUEST_C_TEST_SWWD_REFRESH_PERIOD           202               /**<Refresh period: 202*5 ms */

#define GUEST_C_OUTPUT_PORT_NUM                    16                /* The number of the output port */
#define GUEST_C_PDDM_IN_PORT_NUM                   3                /* The number of the output port */
#define GUEST_C_PDDM_OUT_PORT_NUM                  2                /* The number of the output port */
#define GUEST_C_SDDM_IN_PORT_NUM                   (3 * 6)                /* The number of the output port */
#define GUEST_C_SDDM_OUT_PORT_NUM                  (2 * 6)                /* The number of the output port */
#define GUEST_C_TCIM_PORT_NUM                      (8 * 2)                /* The number of the output port */
#define GUEST_C_CDDM_IN_PORT_NUM                   (5 + 8)                /* The number of the output port */
#define GUEST_C_CDDM_OUT_PORT_NUM                  1                /* The number of the output port */
#define GUEST_C_SIOM_IN_PORT_NUM                   (2 * 3)                /* The number of the output port */
#define GUEST_C_SIOM_OUT_PORT_NUM                  (2 * 3)                /* The number of the output port */

#define GUEST_C_2oo2_TIMEOUT                       5000              /* 2oo2 timeout -- ms */
#define GUEST_C_SECONDS_PER_MINUTE                 (60u)
#define GUEST_C_BRANCH_PST_NUM                     1                 /* The num of relay for one branch of power supply test */

#define GUEST_C_INTCNT_MIN                         120016            /* 时钟检测10分钟中断计数最小值(120023) */
#define GUEST_C_INTCNT_MAX                         120047            /* 时钟检测10分钟中断计数最大值(120026) */
#define GUEST_C_INTCNT_SYN                         119840            /* 时钟检测10分钟中断计数用于两CPU互传的预期值 */

#define GUEST_C_FPGACNT_MIN                        (GUEST_C_FPGACNT_SYN - GUEST_C_FPGACNT_DETA)              /* FPGA时钟互检结果最小值（包括） */
#define GUEST_C_FPGACNT_MAX                        (GUEST_C_FPGACNT_SYN + GUEST_C_FPGACNT_DETA)              /* FPGA时钟互检结果最大值（包括）*/
#define GUEST_C_FPGACNT_SYN                        5000                                                      /* FPGA时钟互检用于两CPU互传的预期值 */
#define GUEST_C_FPGACNT_DETA                       (GUEST_C_FPGACNT_SYN / GUEST_C_FPGA_DETA)
#define GUEST_C_FPGA_DETA                          800

#define GUEST_C_TFTP_INFO_LEN                      31               /*发送给SDM的每条输出命令的消息长度 */

#define GUEST_C_TFTP_BUF_SIZE                      (100 * GUEST_C_TFTP_INFO_LEN)               /* 存放TFTP消息的内存大小 */
#define GUEST_C_TFTP_VER_BUF_SIZE                  48                /* 发送给SDM的版本信息的消息长度 */
#define GUEST_C_TFTP_ERRINFO_LEN                   21u                /* 发送给OMAP的每条错误码的消息长度 */
#define GUEST_C_TFTP_ERRBUF_SIZE                   (100 * GUEST_C_TFTP_ERRINFO_LEN)            /* 存放错误码消息的内存大小 */
#define GUEST_C_TFTP_HAND_LEN                      11
#define GUEST_C_TFTP_HAND_BUF_SIZE                 GUEST_C_TFTP_HAND_LEN
#define GUEST_C_TFTP_WRITE_BUF_LEN                 10
#define GUEST_C_TFTP_WRITE_SNBUF_LEN               16

#define FCT_ISO_Testcycle                          (10*1000/(GUEST_C_TASK_PERIOD*50))
#define FCT_FUSE_Testcycle                         (10*1000/(GUEST_C_TASK_PERIOD*50))
#define FCT_TT_Testcycle                           (10*1000/(GUEST_C_TASK_PERIOD*50))
#define FCT_RIT_Testcycle                          (10*1000/(GUEST_C_TASK_PERIOD*50))

#define FCT_AUTO_Test                              0

#define FCT_Test_Onebyone                          1
#define GUEST_C_NOT_FILTERED	"NOT_FILTERED"

#define GUEST_C_FILTERED	"FILTERED"


/*FCT TFTP TEST MSG*/
#define TFTP_MSG_24V                               0x0001
#define TFTP_MSG_24VM                              0x000D
#define TFTP_MSG_24VO                              0x000E
#define TFTP_MSG_ISO                               0x0002
#define TFTP_MSG_FUSE                              0x0003
#define TFTP_MSG_MEMBUS                            0x0004
#define TFTP_MSG_RBOUTPUT                          0x0005
#define TFTP_MSG_LEDTEST                           0x0006
#define TFTP_MSG_RITTEST                           0x0007
#define TFTP_MSG_FPGACNT                           0x0008
#define TFTP_MSG_12VF                              0x0009
#define TFTP_MSG_VOLTEST                           0x000A
#define TFTP_MSG_OUTPUTTESTA                       0x000B
#define TFTP_MSG_OUTPUTTESTB                       0x000C
#define TFTP_MSG_TTTEST                            0x0010
#define TFTP_MSG_SCTTEST                           0x0011
#define TFTP_MSG_BOARDPOSITION                     0x0020
/*FCT TFTP CMD MSG*/
#define TFTP_MSG_WAITING                           0x0000
#define TFTP_MSG_Hand                              0x8000
#define TFTP_MSG_STOP                              0xF000
#define TFTP_MSG_ERR                               0xFFFF

/* GUEST Monitor */
#define TFTP_MSG_VOOB                              0x1111
#define TFTP_MSG_PDDM                              0x2222
#define TFTP_MSG_SDDM                              0x3333
#define TFTP_MSG_TCIM                              0x4444
#define TFTP_MSG_CDDM                              0x5555
#define TFTP_MSG_SIOM                              0x6666



#define N                                          2                 /* The number of re-run steps in case of failure of step test */

#define GUEST_C_FUSE_INVOKE_ADDR                   (0x0002)         /* 熔丝检测触发的地址 */
#define GUEST_C_FUSE_BREAK_ADDR                    (0x0004)         /* 熔丝喂狗及烧毁命令的地址 */
#define GUEST_C_24VLIGHT_LAMP_ADDR                 (0x0006)         /* 24V面板指示灯控制地址 */
#define GUEST_C_ASKFPGARB_ADDR                     (0x0008)         /* 通知FPGA回读端口状态的地址 */
#define GUEST_C_OUTPUTLIGHT_LAMP_ADDR              (0x000A)         /* 面板输出指示灯控制地址 */
#define GUEST_C_FPGACNT_ADDR                       (0x0010)         /* FPGA时钟互检结果存放地址 */
#define GUEST_C_FUSE_RBVALUE_ADDR                  (0x0012)         /* 熔丝检测回读数据存放地址 */
#define GUEST_C_FUSE_RB_ADDR                       (0x0014)         /* 回读熔丝状态的地址 */
#define GUEST_C_FPGA_VERSION_ADDR                  (0x0020)         /* FPGA版本号存放地址 */
#define GUEST_C_RB_VALUE_ADDR                      (0x8000)         /* FPGA中存放32次回读结果的地址，是在DPRAM中 */
#define GUEST_C_CNTFLAG_ADDR                       (0x8100)         /* FPGA中存放计数器和回读完成标示的地址，是在DPRAM中 */

#define GUEST_C_TASK_NUM                           1                 /* 任务数目 */
#define GUEST_C_TASKLOG_SIZE                       100               /* 存放报警信息的内存大小 */

#define GUEST_C_PORT_RESTRICTIVE_STATUS            1                 /* 端口限制态 */

#define GUEST_C_BOARD_KIND_ID                      0x1111            /* 板卡类型，0x1111代表是VOOB板 */

#define GUEST_C_ISOLATION_SAFETY_FLAG              GUEST_C_MIN_SAFETY_FLAG     /* Index of isolation safety flags */
#define GUEST_C_FUSE_SAFETY_FLAG                   GUEST_C_MIN_SAFETY_FLAG+1u  /* Index of fuse safety flags */

#define GUEST_C_TSPS_THRESHOLD_TIME                5000              /* 24V检测的时间阀值，ms单位(5s) */
#define GUEST_C_READFPGA_FLAG_NUM                  1                 /* 重复读取FPGA FLAG次数 */

#ifdef ALSTOM_CONFIG_CPU_A
#define GUEST_C_FEED_WDOG_CMD1                     0x5AF0
#define GUEST_C_FEED_WDOG_CMD2                     0xA50F
#endif

#ifdef ALSTOM_CONFIG_CPU_B
#define GUEST_C_FEED_WDOG_CMD1                     0xF05A
#define GUEST_C_FEED_WDOG_CMD2                     0x0FA5
#endif

#define GUEST_C_RESTRICTIVE_CMD                    (INT16U)0xFFFFu
#define GUEST_C_PERMISSIVE_CMD                     (INT16U)0x0000u
#define   GUEST_C_FC_APPROACHLIFE                 0x60u

extern INT32U g_CycleNO;
extern INT32U g_ISOTCycleNO;
extern INT32U g_FUSETCycleNO;
extern INT32U g_TFTPCycleNO;
extern INT32U g_InterruptCNT;

extern BOOLEAN g_ISOInvoke;
extern BOOLEAN ISOLATION_STATE;
extern BOOLEAN TRACKSIDE_POWER_MISSING;
extern BOOLEAN TRACKSIDE_POWER_OVERAGE;
extern BOOLEAN FUSE_POWER_MISSING;
extern BOOLEAN g_RITInvoke;
extern BOOLEAN g_RITStart;
extern BOOLEAN g_ISOTStart;
extern BOOLEAN g_ISOTFlag;
extern BOOLEAN g_ISOTInvoke;
extern BOOLEAN g_RITflag;
extern BOOLEAN g_TFTPBUFFULL;
extern BOOLEAN g_TFTPERRBUFFULL;

extern INT16U g_StartTest;
extern INT16U g_LastTest;
extern INT16U g_TestStore;

extern INT8U RITestStep;
extern INT8U RITestPortID;
extern INT8U ISOTestStep;
extern INT8U FUSETestStep;

extern INT8U g_TFTPBUF[GUEST_C_TFTP_BUF_SIZE];
extern INT8U g_TFTPERRBUF[GUEST_C_TFTP_ERRBUF_SIZE];

extern INT32U WriteOffset;
extern INT32U WriteFCodeOffset;
#define GUEST_M_FPGA_Write(offset, value)          (*(( volatile INT16U *)(MSTEPINC_C_BASE_ADDR1_SW + (INT16U)(offset))) = (INT16U)(value))

#define GUEST_M_FPGA_Read(offset, value)           ((value) = *(( volatile INT16U *)(MSTEPINC_C_BASE_ADDR1_SW + (INT16U)(offset))))


#ifdef VOOB16_E

typedef struct
{
    INT32U BOARD_KIND_ID;
    INT32U OUTPUT_TEST_CYCLE_PROD;
    INT32U ISOLATION_TEST_CYCLE_PROD;
    INT32U FUSE_TEST_CYCLE_PROD;
    INT32U TPF;
    INT32U THL;
    INT32U THH;
    INT32U NXT;    
    INT32U DELT;
INT32U FAULT_FILTER_TIMEOUT;
INT32U FAULT_FILTER_ACTIVATION;	
    INT32U POU;

} GUEST_T_CFG;

#endif

typedef struct
{
    //OSW_T_pEVENT TSPST_OVER_EVENT;                 /* 24V电源检测后释放该信号量，主任务判断是否进入隔离态 */
    OSW_T_pEVENT TFTP_BUF_EVENT;                   /* 控制访问存放TFTP消息内存的信号量 */
    OSW_T_pEVENT LOG_BUF_EVENT;                    /* 控制访问存放宕机消息内存的信号量 */
    OSW_T_pEVENT TFTP_TESTBUF_EVENT;                   /* 控制功能测试结果访问存放TFTP消息内存的信号量 */
    //OSW_T_pEVENT SET_ISOLAT_STATUS;              /* 设置隔离态的控制信号量，防止多个任务同时设置 */
    //OSW_T_pEVENT SET_SAFETY_STATUS;              /* 设置安全态的控制信号量，防止多个任务同时设置 */
   OSW_T_pEVENT TFTP_ERRBUF_EVENT;
} GUEST_T_SEM;

typedef enum
{
    GUEST_C_LED_OPEN       = 0u,
    GUEST_C_LED_CLOSE      = 1u,
    GUEST_C_LED_FLASH      = 2u,
    GUEST_C_LED_LONG_FLASH = 3u,
} GUEST_E_LED_Mode;

/** Definition of the run mode of the guest board */
typedef enum
{
    GUEST_C_STATE_UNDEFINED      = 0u,             /**<Undefined state          */
    GUEST_C_STATE_INITIAL        = 1u,             /**<Initial state     */
    GUEST_C_STATE_PREOPERATIONAL = 2u,             /**<Pre-Operational state     */
    GUEST_C_STATE_OPERATIONAL    = 3u,             /**<Operational state     */
    GUEST_C_STATE_INITFAILED     = 4u,             /**<Init Failed state     */
    GUEST_C_STATE_PS_ISOLATION   = 5u,             /**<24V Error Set Isolation state       */
    GUEST_C_STATE_OPT_ISOLATION  = 6u,             /**<Output Port Test Error Set Isolation state       */
    GUEST_C_STATE_RST_ISOLATION  = 7u,             /**<Restrictive Set Isolation state       */
    GUEST_C_STATE_SAFETY         = 8u,             /**<Safety state          */
} GUST_E_RUN_STATE;

/** Definition of the failure code --0x40-0x7F-- */
typedef enum
{
    GUEST_C_FC_24VMISSING    = 0x40u,
    GUEST_C_FC_24VOVERAGE     = 0x41u,
    GUEST_C_FC_12VMISSING    = 0x42u,
    GUEST_C_FC_PORTERROR     = 0x43u,
    GUEST_C_FC_PORTCRCERROR  = 0x44u,
    GUEST_C_FC_INITERROR     = 0x45u,
} GUST_E_FAILURECODE;

/** Definition of a Guest safety flag State */
typedef enum
{
    GUEST_TEST_C_safetyFlagOK = 0,                 /**<The safety flag is OK */
    GUEST_TEST_C_safetyFlagBroken = 1,             /**<The safety flag is broken */
} GUEST_TEST_E_GuestSafetyFlagState;

typedef enum
{
    GUEST_TFTP_MSG_HAND = 0x000A,                 /* */
    GUEST_TFTP_MSG_VERS = 0x000B,                 /* */
    GUEST_TFTP_MSG_RESU = 0x000D,                 /* */
    GUEST_TFTP_MSG_STOP = 0x000F,             /**/
} GUEST_TFTP_MSG_TYPE;


typedef struct
{
    INT32U SwVer1;
    INT32U SwVer2;
    INT32U SwCRC;
    INT16U FPGAVer;

} GUEST_T_USR_CFG;

extern INT8U *g_ParaAddr;
extern GUEST_T_CFG *g_CFGAddr;
extern GUEST_T_SEM g_Sem;

extern GUST_E_RUN_STATE g_RunState;

extern GUEST_TEST_E_GuestSafetyFlagState GUEST_TEST_V_brokenSafetyFlags[GUEST_C_MAX_SAFETY_FLAG - GUEST_C_MIN_SAFETY_FLAG + 1];
extern MSTEP_T_VitalStamp VitalStamp[GUEST_C_TASK_NUM];

extern void GUEST_F_ISRINIT(void);
extern void GUEST_F_SetDYNStable(void);
extern void GUEST_F_SetDYNActive(void);
extern void GUEST_F_EXTLightSetOn(void);
extern void GUEST_F_EXTLightSetOff(void);
extern void GUEST_F_DriveOutput(INT16U cmd, BOOLEAN SetOutput, GUEST_E_LED_Mode Drvmode);
extern void GUEST_F_TFTP_SendTestInfo(INT16U cmdtype, INT16U datalen, INT16U cmd, INT8U Status);
extern void GUEST_F_TFTP_SendHandInfo(INT16U cmdtype, INT16U cmd);
extern void GUEST_F_WriteDataToTFTPBUF(INT16U cmd);
extern void GUEST_F_GetBoardPosition(void);
extern void GUEST_F_LED_Test(void);
extern void GUEST_F_Output_Test(void);
extern void GUEST_F_ResFPGASetOff(void);
extern void GUEST_F_ResFPGASetOn(void);
extern void GUEST_F_FeedWDog(void);
extern void GUEST_F_OutputLED(INT16U cmd, GUEST_E_LED_Mode mode);
extern void GUEST_F_CheckFPGACNT(void);
extern void GUEST_F_24VLED(GUEST_E_LED_Mode mode);
extern void GUEST_F_EXTLED(GUEST_E_LED_Mode mode);
extern void GUEST_F_RBOutput(INT16U LastCMD);
extern void GUEST_F_ReadDataFromTFTPBUF(INT8U* pBuf);
extern void GUEST_F_ReadDataFromTFTPHandBUF(INT8U* pBuf);
extern void GUEST_F_SendVerData(INT8U* VerBuf);
extern void GUEST_F_CheckBuf(INT8U* buffer, INT16U len);
extern void GUEST_F_24V_Test(void);
extern void GUEST_F_12V_Test(void);
extern void GUEST_F_GetDataInOneCPU(void);

extern void GUEST_F_GetSnake(void);
extern void GUEST_F_LED_Snake(void);

extern void GUEST_F_IODB_GetCmd_VOOB(INT16U* pOutputs);
extern void GUEST_F_IODB_GetCmd_CDDM(INT16U* pOutputs);
extern void GUEST_F_IODB_GetCmd_PDDM(INT16U* pOutputs);
extern void GUEST_F_IODB_GetCmd_SDDM(INT16U* pOutputs, GUEST_E_LED_Mode* mode);
extern void GUEST_F_IODB_DrvCmd_TCIM(void);
extern void GUEST_F_SendFBJState();
extern void GUEST_F_RBPointStatus(INT16U DriveCommand);

extern const GUEST_T_CFG * GUEST_F_GetConfigPointer(void);
extern void GUEST_F_ReadFailCodeFromTFTPBUF(INT8U* fBuf);
extern void GUEST_F_WriteFailCodeToTFTPBUF(INT8U FailCode);
#endif /* GUEST_INCLUDE */

/* EOF */
