/**
* @file guest_fuse.c
* @brief
* @author Wang Jiantao
* @date 2013-7-2
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: wangjiantao@casco.com.cn
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/

#include "guest_public.h"
#include "guest_fuse.h"

static ALSTOM_T_Status GUEST_F_FuseTest(void);
static ALSTOM_T_Status GUEST_F_FUSET_Step_Check(INT8U TestStep, INT16U EXPTValue, BOOLEAN StableState);

static INT8U FaultCNT_FUSE = 0U;

/* ����˿����ÿһ���ض�������м�飬����������ظ�ִ��N�� */
static ALSTOM_T_Status GUEST_F_FUSET_Step_Check(INT8U TestStep, INT16U EXPTValue, BOOLEAN StableState)
{
    ALSTOM_T_Status Status = ALSTOM_C_NO_ERROR;

    INT16U RBValue = 0u;
    INT32U StatusCRC = 0u;
    INT32U RMTStatusCRC = 0u;

    while (1)
    {
        /* ��ȡFPGA�ض�����˿����� */
        if (StableState == FALSE)
        {
            GUEST_M_FPGA_Read(GUEST_C_FUSE_RBVALUE_ADDR, RBValue);
        }
        else
        {
            GUEST_M_FPGA_Read(GUEST_C_FUSE_RB_ADDR, RBValue);
        }

        if (EXPTValue == RBValue)
        {
            Status = ALSTOM_C_NO_ERROR;
        }
        else
        {
            Status = ALSTOM_C_ERROR;
        }

        /* ��CPU�ض������ݽ���2oo2 */
        MSTEP_F_CryptlibCRC32FSFB2ChanOneCompute((INT8U *)&Status, sizeof(Status), &StatusCRC);

        MSTEP_F_K2oo2CpuExchange((INT8U *)&RMTStatusCRC, (INT8U *)&StatusCRC, sizeof(StatusCRC), GUEST_C_2oo2_TIMEOUT);

        if (RMTStatusCRC != StatusCRC)
        {
            Status = ALSTOM_C_ERROR;
        }

        if (Status == ALSTOM_C_NO_ERROR)
        {
            break;
        }
        else
        {

            ++FaultCNT_FUSE;

            DTXT_REPORT4(DTXT_REPORT_INFO, "GUEST_F_FUSET_Step_Check Error", TestStep, StableState, RBValue, EXPTValue);

            if (N <= FaultCNT_FUSE)
            {
                Status = ALSTOM_C_ERROR;
                break;
            }
        }
    }

    return Status;
}

ALSTOM_T_Status GUEST_F_FuseTest(void)
{
    ALSTOM_T_Status Status = ALSTOM_C_NO_ERROR;

    INT16U InvokeCMD = 0;
    INT16U EXPTValue = 0;
    INT8U Synchro = 0;
    INT8U RMTSynchro = 0;

    INT8U Step = 0;

    /*DTXT_REPORT1(DTXT_REPORT_INFO, "GUEST_F_FuseTest start", g_CycleNO);*/

    FaultCNT_FUSE = 0U;

    while (1)
    {
        switch (Step)
        {

            case 0:
#ifdef ALSTOM_CONFIG_CPU_A
                InvokeCMD = 0x01;
#endif
#ifdef ALSTOM_CONFIG_CPU_B
                InvokeCMD = 0x00;
#endif
                EXPTValue = 0x02;
                break;

            case 1:
#ifdef ALSTOM_CONFIG_CPU_A
                InvokeCMD = 0x02;
#endif
#ifdef ALSTOM_CONFIG_CPU_B
                InvokeCMD = 0x00;
#endif
                EXPTValue = 0x01;
                break;

            case 2:
#ifdef ALSTOM_CONFIG_CPU_A
                InvokeCMD = 0x03;
#endif
#ifdef ALSTOM_CONFIG_CPU_B
                InvokeCMD = 0x00;
#endif
                EXPTValue = 0x00;
                break;

            case 3:
#ifdef ALSTOM_CONFIG_CPU_A
                InvokeCMD = 0x00;
#endif
#ifdef ALSTOM_CONFIG_CPU_B
                InvokeCMD = 0x01;
#endif
                EXPTValue = 0x02;
                break;

            case 4:
#ifdef ALSTOM_CONFIG_CPU_A
                InvokeCMD = 0x00;
#endif
#ifdef ALSTOM_CONFIG_CPU_B
                InvokeCMD = 0x02;
#endif
                EXPTValue = 0x01;
                break;

            case 5:
#ifdef ALSTOM_CONFIG_CPU_A
                InvokeCMD = 0x00;
#endif
#ifdef ALSTOM_CONFIG_CPU_B
                InvokeCMD = 0x03;
#endif
                EXPTValue = 0x00;
                break;

            default:
                break;
        }

        /* ֪ͨFPGA��ʼ��˿��� */
        GUEST_M_FPGA_Write(GUEST_C_FUSE_INVOKE_ADDR, InvokeCMD);

        /* �ض�֮ǰҪȷ����CPU���Ѿ�֪ͨ��FPGA��ʼ��⣬�����Ҫ��CPU����һ�� */
        MSTEP_F_K2oo2CpuExchange((INT8U *)&RMTSynchro, (INT8U *)&Synchro, sizeof(Synchro), GUEST_C_2oo2_TIMEOUT);

        Status = GUEST_F_FUSET_Step_Check(Step, EXPTValue, FALSE);

        if (ALSTOM_C_ERROR == Status)
        {
            FUSETestStep = Step;
            DTXT_REPORT2(DTXT_REPORT_INFO, "=========FUSETest Err=========", Step, Status);
            GUEST_F_TFTP_SendTestInfo(GUEST_TFTP_MSG_RESU, 0x04, TFTP_MSG_FUSE, Status);
            break;
        }

        /*�ڽ�����һ������֮ǰ��ȷ����̬�ض����*/
        OSW_F_TimeDly(GUEST_FUSE_TEST_DELAY);

        EXPTValue = 0x00;

        Status = GUEST_F_FUSET_Step_Check(Step, EXPTValue, TRUE);

        if (ALSTOM_C_ERROR == Status)
        {
            break;
        }

        ++Step;

        if (Step > 5)
        {
            break;
        }

    }

    return Status;
}

/* Enable the fuse test control signal */
void GUEST_F_FuseTestSetOn(void)
{
#ifdef ALSTOM_CONFIG_CPU_A
    MSTEP_F_GpioSetOff(MSTEPINC_C_GPIO_ID12);
#endif

#ifdef ALSTOM_CONFIG_CPU_B
    MSTEP_F_GpioSetOff(MSTEPINC_C_GPIO_ID10);
#endif
}

/* Disable the fuse test control signal */
void GUEST_F_FuseTestSetOff(void)
{
#ifdef ALSTOM_CONFIG_CPU_A
    MSTEP_F_GpioSetOn(MSTEPINC_C_GPIO_ID12);
#endif

#ifdef ALSTOM_CONFIG_CPU_B
    MSTEP_F_GpioSetOn(MSTEPINC_C_GPIO_ID10);
#endif
}

void GUEST_F_FUSE_Test(void)
{
    ALSTOM_T_Status Status = ALSTOM_C_NO_ERROR;

    static INT32U FUSETLoopId = 0u;

    if (TFTP_MSG_FUSE != g_LastTest)
    {
        FUSETLoopId = 0u;
    }

    if (0 == FUSETLoopId % FCT_FUSE_Testcycle)
    {
        Status = GUEST_F_FuseTest();

        if (ALSTOM_C_NO_ERROR == Status)
        {
            /*FCT FUSET*/
            DTXT_REPORT1(DTXT_REPORT_INFO, "=========FuseTest Result=========", Status);
            GUEST_F_TFTP_SendTestInfo(GUEST_TFTP_MSG_RESU, 0x04, TFTP_MSG_FUSE, Status);
        }
    }

    ++FUSETLoopId;
}

/* EOF */
