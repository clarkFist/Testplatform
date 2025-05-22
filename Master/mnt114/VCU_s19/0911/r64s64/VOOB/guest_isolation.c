/**
* @file guest_isolation.c
* @brief
* @author Wang Jiantao
* @date 2013-5-21
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: wangjiantao@casco.com.cn
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/

#include "guest_public.h"
#include "guest_isolation.h"
#include "guest_readback.h"

static ALSTOM_T_Status GUEST_F_ISOT_Step_Check(INT8U TestStep,
                                               GUST_E_SENSORS RBSensorKind,
                                               INT8U EXPTValue);
static ALSTOM_T_Status GUEST_F_IsolationTest(void);



static INT8U FaultCNT_ISO = 0U;


/* 对隔离检测的每一步回读结果进行检查，检查有问题重复执行N次 */
static ALSTOM_T_Status GUEST_F_ISOT_Step_Check(INT8U TestStep,
                                               GUST_E_SENSORS RBSensorKind,
                                               INT8U EXPTValue)
{
    ALSTOM_T_Status Status = ALSTOM_C_NO_ERROR;

    while (1)
    {
        Status = GUEST_F_RBValue(RBSensorKind, EXPTValue, (INT32U)MSTEPINC_C_TASK_ID_1);

        if (ALSTOM_C_NO_ERROR == Status)
        {
            break;
        }
        else
        {

            ++FaultCNT_ISO;

            DTXT_REPORT2(DTXT_REPORT_INFO, "GUEST_F_ISOT_Step_Check Error", TestStep, FaultCNT_ISO);

            if (N <= FaultCNT_ISO)
            {
                Status = ALSTOM_C_ERROR;
                break;
            }
        }
    }

    return Status;
}

static ALSTOM_T_Status GUEST_F_IsolationTest(void)
{
    ALSTOM_T_Status Status = ALSTOM_C_NO_ERROR;

    static INT8U Step = 0;
    INT8U EXPTValue = 0;
    GUST_E_ISOLATION_RELAYS SetRelay = GUEST_C_ISOLATION_GPIO_BRANCHES_OFF;

    DTXT_REPORT1(DTXT_REPORT_INFO, "GUEST_F_IsolationTest start", g_CycleNO);

    if (g_ISOTInvoke == TRUE)
    {
        Step = 0;
        FaultCNT_ISO = 0;
        g_ISOTInvoke = FALSE;
        g_ISOTStart = TRUE;
        g_ISOTFlag = FALSE;
    }

    do
    {
        switch (Step)
        {

            case 0:
                SetRelay = GUEST_C_ISOLATION_GPIO_BRANCH2_OFF;
                EXPTValue = GUEST_C_READBACK_SENSORS_SET;
                break;

            case 1:
                SetRelay = GUEST_C_ISOLATION_GPIO_BRANCHES_OFF;
                EXPTValue = GUEST_C_READBACK_SENSORS_UNDETERMINED;
                break;

            case 2:
                SetRelay = GUEST_C_ISOLATION_GPIO_BRANCH1_OFF;
                EXPTValue = GUEST_C_READBACK_SENSORS_SET;
                break;

            case 3:
                SetRelay = GUEST_C_ISOLATION_GPIO_BRANCHES_OFF;
                EXPTValue = GUEST_C_READBACK_SENSORS_UNDETERMINED;
                break;

            default:
                return ALSTOM_C_ERROR;
        }

        GUEST_F_SetIsolationRelays((GUST_E_ISOLATION_RELAYS)SetRelay);

        OSW_F_TimeDly((INT16U)GUEST_ISOT_STEP_DELAY);

        Status = GUEST_F_ISOT_Step_Check(Step, (GUST_E_SENSORS)GUEST_C_R_ISOT, EXPTValue);

        if (ALSTOM_C_ERROR == Status)
        {
            ISOTestStep = Step;
            g_ISOTFlag = TRUE;
            g_ISOTStart = FALSE;

            DTXT_REPORT2(DTXT_REPORT_INFO, "=========ISOTest Err=========", Step, Status);

            GUEST_F_TFTP_SendTestInfo(GUEST_TFTP_MSG_RESU, 0x04, TFTP_MSG_ISO, Status);
            break;
        }

        ++Step;

        if (Step > 3)
        {
            g_ISOTStart = FALSE;
            break;
        }
    }
    while (0);

    return Status;
}


/* Set two branches relays states during isolation cell test */
void GUEST_F_SetIsolationRelays(GUST_E_ISOLATION_RELAYS relaysStates)
{

    switch (relaysStates)
    {

        case GUEST_C_ISOLATION_GPIO_BRANCHES_OFF:
#ifdef ALSTOM_CONFIG_CPU_A
            MSTEP_F_GpioSetOff(MSTEPINC_C_GPIO_ID1);
            MSTEP_F_GpioSetOff(MSTEPINC_C_GPIO_ID4);
#endif

#ifdef ALSTOM_CONFIG_CPU_B
            MSTEP_F_GpioSetOff(MSTEPINC_C_GPIO_ID4);
            MSTEP_F_GpioSetOff(MSTEPINC_C_GPIO_ID1);
#endif
            break;

        case GUEST_C_ISOLATION_GPIO_BRANCH2_OFF:
#ifdef ALSTOM_CONFIG_CPU_A
            MSTEP_F_GpioSetOn(MSTEPINC_C_GPIO_ID1);
            MSTEP_F_GpioSetOff(MSTEPINC_C_GPIO_ID4);
#endif

#ifdef ALSTOM_CONFIG_CPU_B
            MSTEP_F_GpioSetOn(MSTEPINC_C_GPIO_ID4);
            MSTEP_F_GpioSetOff(MSTEPINC_C_GPIO_ID1);
#endif
            break;

        case GUEST_C_ISOLATION_GPIO_BRANCH1_OFF:
#ifdef ALSTOM_CONFIG_CPU_A
            MSTEP_F_GpioSetOff(MSTEPINC_C_GPIO_ID1);
            MSTEP_F_GpioSetOn(MSTEPINC_C_GPIO_ID4);
#endif

#ifdef ALSTOM_CONFIG_CPU_B
            MSTEP_F_GpioSetOff(MSTEPINC_C_GPIO_ID4);
            MSTEP_F_GpioSetOn(MSTEPINC_C_GPIO_ID1);
#endif
            break;

        case GUEST_C_ISOLATION_GPIO_BRANCHES_ON:
#ifdef ALSTOM_CONFIG_CPU_A
            MSTEP_F_GpioSetOn(MSTEPINC_C_GPIO_ID1);
            MSTEP_F_GpioSetOn(MSTEPINC_C_GPIO_ID4);
#endif

#ifdef ALSTOM_CONFIG_CPU_B
            MSTEP_F_GpioSetOn(MSTEPINC_C_GPIO_ID4);
            MSTEP_F_GpioSetOn(MSTEPINC_C_GPIO_ID1);
#endif
            break;

        default:
            break;
    }

}

void GUEST_F_ISO_Test(void)
{
    ALSTOM_T_Status Status = ALSTOM_C_NO_ERROR;

    static INT32U ISOTLoopId = 0u;

    if (TFTP_MSG_ISO != g_LastTest)
    {
        ISOTLoopId = 0;
    }

    if (0 == ISOTLoopId % FCT_ISO_Testcycle)
    {
        g_ISOTInvoke = TRUE;
    }

    /* Excute isolation test */
    if ((g_ISOTInvoke == TRUE) || (g_ISOTStart == TRUE))
    {
        Status = GUEST_F_IsolationTest();

        if ((g_ISOTFlag == FALSE) && (g_ISOTStart == FALSE))
        {
            DTXT_REPORT1(DTXT_REPORT_INFO, "=========ISOTest Result=========", Status);
            GUEST_F_TFTP_SendTestInfo(GUEST_TFTP_MSG_RESU, 0x04, TFTP_MSG_ISO, Status);/**/
        }
    }

    ++ISOTLoopId;
}

/* EOF */
