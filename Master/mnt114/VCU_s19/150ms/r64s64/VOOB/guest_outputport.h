/**
* @file guest_outputport.h
* @brief
* @author Wang Jiantao
* @date 2013-7-10
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: wangjiantao@casco.com.cn
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/

#ifndef __GUEST_OUTPUTPORT__
#define __GUEST_OUTPUTPORT__


#define DELAY_TM0                                  0    /* ms */
#define DELAY_TM1                                  12   /* ms */
#define DELAY_TM2                                  70   /* ms */

/* Output cells check results counters */

typedef struct
{
    INT8U Cnt_OK[GUEST_C_OUTPUT_PORT_NUM];
    INT8U Cnt_Fault[GUEST_C_OUTPUT_PORT_NUM];

} GUEST_T_OPTCEL_TEST_RESULT_COUNTER;

extern GUEST_T_OPTCEL_TEST_RESULT_COUNTER g_OPTRESLTCNT;
extern GUEST_T_OPTCEL_TEST_RESULT_COUNTER g_OPTRESLTCNTTEMP;

extern void GUEST_F_SCT(void);
extern void GUEST_F_RIT(INT16U OLDCPUCMD);
extern void GUEST_F_TT(INT16U OLDCPUCMD, INT16U NEWCPUCMD);
extern void GUEST_F_OPT(INT16U OLDCPUCMD, INT16U NEWCPUCMD);

extern ALSTOM_T_Status GUEST_F_OPT_Check(void);

#endif /* GUEST_OUTPUTPORT */

/* EOF */
