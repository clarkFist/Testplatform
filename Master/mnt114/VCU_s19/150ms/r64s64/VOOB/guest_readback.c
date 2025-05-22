/**
* @file guest_readback.c
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
#include "guest_public.h"
#include "guest_readback.h"

/*GPIO����Ϊ24VǷѹ�ض�,24V��ѹ�ض�,12VǷѹ�ض�,���뵥Ԫp��N�˻ض�*/

#ifdef ALSTOM_CONFIG_CPU_A
static MSTEPINC_E_GpioId GUEST_V_RB_GPIO[5] =
{
    MSTEPINC_C_GPIO_ID2,

    MSTEPINC_C_GPIO_ID7,

    MSTEPINC_C_GPIO_ID8,

    MSTEPINC_C_GPIO_ID5,

    MSTEPINC_C_GPIO_ID3
};
#endif

#ifdef ALSTOM_CONFIG_CPU_B
static MSTEPINC_E_GpioId GUEST_V_RB_GPIO[5] =
{
    MSTEPINC_C_GPIO_ID3,

    MSTEPINC_C_GPIO_ID6,

    MSTEPINC_C_GPIO_ID7,

    MSTEPINC_C_GPIO_ID9,

    MSTEPINC_C_GPIO_ID2
};
#endif

static GUEST_E_READBACK_SENSORS_STATE GUEST_F_Filter_Check(INT32U RBvalue);

void GUEST_F_TSPSTForOPT(void)
{
    ALSTOM_T_Status Status = ALSTOM_C_NO_ERROR;

    /* 24V��� */
    Status = GUEST_F_RBValue((GUST_E_SENSORS)GUEST_C_24V_PST, (INT8U)GUEST_C_READBACK_SENSORS_UNSET, (INT32U)MSTEPINC_C_TASK_ID_1);

    if (ALSTOM_C_NO_ERROR == Status)
    {
        TRACKSIDE_POWER_MISSING = FALSE;

        TRACKSIDE_POWER_OVERAGE = FALSE;

        GUEST_F_24VLED(GUEST_C_LED_OPEN);
    }
    else if (ALSTOM_C_NOT_IMPLEMENTED == Status)
    {
        //(void) DTXT_F_Printf("24V power supply overage cycle: %d\n", g_CycleNO);
        TRACKSIDE_POWER_OVERAGE = TRUE;

        GUEST_F_24VLED(GUEST_C_LED_FLASH);
    }
    else
    {
        //(void) DTXT_F_Printf("24V power supply missing cycle: %d\n", g_CycleNO);
        TRACKSIDE_POWER_MISSING = TRUE;

        GUEST_F_24VLED(GUEST_C_LED_CLOSE);
    }

}


/* �ض�GPIO�˿�״̬���ݣ���ȡ32�Σ�����Ԥ�ڵ�ֵ���бȽϣ�����֮ǰ��״̬ͬ�� */
ALSTOM_T_Status GUEST_F_RBValue(GUST_E_SENSORS RBSensorKind, INT8U EXPTValue, INT32U tskID)
{
    ALSTOM_T_Status Status = ALSTOM_C_NO_ERROR;

    INT32U i = 0u;
    INT32U j = 0u;

    INT32U StateSum[5] = {0};

    INT32U StateCRC[5] = {0};
    INT32U RMTStateCRC[5] = {0};

    GUEST_E_READBACK_SENSORS_STATE State[5] = {GUEST_C_READBACK_SENSORS_UNDETERMINED};

    GDF_M_RANGE_ASSERT((tskID > (GUEST_C_TASK_NUM - 1)), tskID);

    do
    {
        /*32�β����������*/
        for (i = 0u; i < 32u; ++i)
        {
            /*��ȡGPIO״̬*/
            for (j = 0u; j < 5u; ++j)
            {
                StateSum[j] = StateSum[j] + (INT32U)MSTEP_F_GpioRead(GUEST_V_RB_GPIO[j]);
            }

            /*ʹִ��ʱ�����20ms*/
            if (0 == ((i + 1u) % 2u))
            {
                if ((11u == i) || (13u == i) || (15u == i) || (17u == i) || (19u == i))
                {
                    OSW_F_TimeDly(2);
                }
                else
                {
                    OSW_F_TimeDly(1);
                }
            }
        }

        for (i = 0u; i < 5u; ++i)
        {
            /*�жϻض���״̬*/
            State[i] = GUEST_F_Filter_Check(StateSum[i]);

            /* ����CRC*/
            MSTEP_F_CryptlibCRC32FSFB2ChanOneCompute((INT8U *)&State[i], sizeof(State[i]), &StateCRC[i]);
        }

        /* ��CPU״̬ͬ�� */
        MSTEP_F_K2oo2CpuExchange((INT8U *)RMTStateCRC,
                                 (INT8U *)StateCRC,
                                 (sizeof(INT32U) * 5),
                                 GUEST_C_2oo2_TIMEOUT);

        for (i = 0u; i < 5u; ++i)
        {
            if (RMTStateCRC[i] != StateCRC[i])
            {
                State[i] = GUEST_C_READBACK_SENSORS_UNDETERMINED;
            }
        }

        /*���12V��Դ*/
        if (GUEST_C_R_ISOT != RBSensorKind)
        {
            if (GUEST_C_READBACK_SENSORS_UNSET == State[2])
            {
                FUSE_POWER_MISSING = FALSE;
            }
            else
            {
                FUSE_POWER_MISSING = TRUE;
            }
        }

        if (GUEST_C_12V_PST == RBSensorKind)
        {
            /*12V��Դ״̬��Ԥ��״̬�Ƚ�*/
            if (EXPTValue != (INT8U)State[2])
            {
                /*DTXT_REPORT3(DTXT_REPORT_INFO, "12V Missing Kind EXPTValue tskID", EXPTValue, State12VPS, tskID);*/
                Status = ALSTOM_C_ERROR;
                break;
            }

        }
        else if (GUEST_C_24V_PST == RBSensorKind)
        {
            /*24V��Դ״̬��Ԥ��״̬�Ƚ�*/
            if (GUEST_C_READBACK_SENSORS_SET != State[1])
            {
                /*DTXT_REPORT3(DTXT_REPORT_INFO, "24V Overage Kind EXPTValue tskID", State24VPS, StatePSSum1, tskID);*/
                Status = ALSTOM_C_NOT_IMPLEMENTED;
                break;
            }

            if (EXPTValue != (INT8U)State[0])
            {
                /*DTXT_REPORT3(DTXT_REPORT_INFO, "24V Missing Kind EXPTValue tskID", StatePS, StatePSSum, tskID);*/
                Status = ALSTOM_C_ERROR;
                break;
            }

        }
        else
        {
            /*����ģ��״̬��Ԥ��״̬�Ƚ�*/
            /*����Ǹ�����2��4������Ҫ�ж�24V״̬������������̬Ϊ0��24V����̬Ƿѹ���Ϊ0����ѹ���Ϊ1*/
            if ((INT8U)GUEST_C_READBACK_SENSORS_UNDETERMINED == EXPTValue)
            {

                /*��24V�ڸ�����ʱ�Ѿ���ʧ������������*/
                if (State[0] != GUEST_C_READBACK_SENSORS_UNSET)
                {
                    /*DTXT_REPORT(DTXT_REPORT_INFO, "24V Power Missing in ISOT");*/
                    break;
                }

                if ((State[0] != State[3]) || (State[0] != State[4]))
                {
                    /*DTXT_REPORT3(DTXT_REPORT_INFO, "ISO RB Error", State[0], State[3], State[4]);*/
                    Status = ALSTOM_C_ERROR;
                    break;
                }
            }
            else
            {
                if ((EXPTValue != (INT8U)State[3]) || (EXPTValue != (INT8U)State[4]))
                {
                    /*DTXT_REPORT3(DTXT_REPORT_INFO, "ISO RB Error", EXPTValue, State[3], State[4]);*/

                    Status = ALSTOM_C_ERROR;
                    break;
                }
            }
        }
    }
    while (0);

    return Status;
}


/* ��FPGA�ж�ȡFPGA 32�λض�������˿ڵ�P��N��ֵ���˲��������ص�״̬�Ѿ���֤���ص�״̬��CPUһ�� */
void GUEST_F_OPT_RBValueFromFPGA(GUEST_E_READBACK_SENSORS_STATE * SumRBPValue,
                                 GUEST_E_READBACK_SENSORS_STATE * SumRBNValue)
{
    ALSTOM_T_Status Status = ALSTOM_C_NO_ERROR;

    INT32U i = 0u;
    INT32U j = 0u;
    INT32U ReadErrCNT = 0u;
    INT32U ErrNo = 0u;
    INT16U RBData = 0u;
    INT16U CNTFlag = 0u;
    INT8U CNT = 0u;

    INT8U ReadbackP[GUEST_C_OUTPUT_PORT_NUM];
    INT8U ReadbackN[GUEST_C_OUTPUT_PORT_NUM];

    BOOLEAN IsRBPValue = FALSE;

    static INT8U counter = 0u;

    GDF_M_NULL_ASSERT(SumRBPValue);
    GDF_M_NULL_ASSERT(SumRBNValue);

    STD_F_Memset((void *)ReadbackP, 0x0, sizeof(ReadbackP));
    STD_F_Memset((void *)ReadbackN, 0x0, sizeof(ReadbackN));

    do
    {
        /* һ�ζ�д16bit��ǰ8bit��Ϣλ��8bit��ǰ8bit��У��λ��ȡ������ÿ�ζ�ȡ��P�˺�N�˵�ֵ��
        ÿ�˶�ȡ��16λ��ֵ������δ���FPGA��4*32��4��ʾ��P��N��ֵҪ��д���Σ�32��ʾ32�βɼ�*/

        /* ���FPAG DPRAM������һ����ַ���μ�飬���õļ�鳣��Ϊ0x5A��0xA5�����ͨ������� */
        for (i = 0U;i < (4U*32U);++i)
        {

            GUEST_M_FPGA_Write((GUEST_C_RB_VALUE_ADDR + 2 * i), 0x5A);
            GUEST_M_FPGA_Read((GUEST_C_RB_VALUE_ADDR + 2 * i), RBData);

            if (0xA55A != RBData)
            {

                /*DTXT_REPORT2(DTXT_REPORT_INFO, "Check FPGA error", GUEST_C_RB_VALUE_ADDR + 2 * i, RBData);*/
                Status = ALSTOM_C_ERROR;
                break;
            }

            GUEST_M_FPGA_Write((GUEST_C_RB_VALUE_ADDR + 2 * i), 0xA5);

            GUEST_M_FPGA_Read((GUEST_C_RB_VALUE_ADDR + 2 * i), RBData);

            if (0x5AA5 != RBData)
            {

                /*DTXT_REPORT2(DTXT_REPORT_INFO, "Check FPGA error", GUEST_C_RB_VALUE_ADDR + 2 * i, RBData);*/
                Status = ALSTOM_C_ERROR;
                break;
            }

            GUEST_M_FPGA_Write((GUEST_C_RB_VALUE_ADDR + 2 * i), 0x0);
        }

        /* ��������⣬����do while(0) */
        if (ALSTOM_C_ERROR == Status)
        {
            ErrNo = 1;
            break;
        }
	GUEST_M_FPGA_Read(GUEST_C_CNTFLAG_ADDR, CNTFlag);
	counter=((CNTFlag>>3) & 0x7);
	
        /* ��ռ���������ʾ��bit5-bit3 ��������bit2-bit0 ��ʾ */
        GUEST_M_FPGA_Write(GUEST_C_CNTFLAG_ADDR, 0x0);

        /* �������֪ͨFPGA�ض� */
        GUEST_M_FPGA_Write(GUEST_C_ASKFPGARB_ADDR, 0x5A5A);

        /* �ȴ�FPGA 32�β������(20ms)������20ms�ڶ�ȡ24V��Դ��״̬ */
        GUEST_F_TSPSTForOPT();

        /* ���������Ͳ�����ʾ�������������ȷ����ʾΪ0x111������Զ�ȡ */
//        ++counter;

//        counter = counter % 8;

        while (1)
        {
            ++ReadErrCNT;

            GUEST_M_FPGA_Read(GUEST_C_CNTFLAG_ADDR, CNTFlag);
            /*DTXT_REPORT3(DTXT_REPORT_INFO, "Read CNTFlag",ReadErrCNT, CNTFlag, counter);*/

            if ((counter != (INT8U)((CNTFlag >> 3) & 0x07)) && (0x07 == (CNTFlag & 0x07)))
            {
                /*DTXT_REPORT1(DTXT_REPORT_INFO, "Read CNTFlag",ReadErrCNT);*/
                break;
            }
            else
            {
                /* �����ȡflag��ʱ�䳬��3ms���򷵻ش��� */
                if (ReadErrCNT > (INT32U)GUEST_C_READFPGA_FLAG_NUM)
                {

                    /*DTXT_REPORT3(DTXT_REPORT_INFO, "Read CNTFlag error", ReadErrCNT, counter, CNTFlag);*/
                    Status = ALSTOM_C_ERROR;
                    break;
                }
            }

            OSW_F_TimeDly(1);
        }

        /* ��������⣬����do while(0) */
        if (ALSTOM_C_ERROR == Status)
        {
            ErrNo = 2;
            break;
        }

        /* ��FPGA 32�λض������ݽ��м�飬ǰ8bit��Ϣλ����8bitУ��λ��ǰ8bit��ȡ����������32�λض����ݵĺ� */
        for (i = 0U; i < 4U * 32U; ++i)
        {

            GUEST_M_FPGA_Read((GUEST_C_RB_VALUE_ADDR + 2 * i), RBData);

            if (((INT8U)(~((INT8U)(RBData >> 8U) & (INT8U)0xFF))) == ((INT8U)(RBData & (INT16U)0xFF)))
            {
                if (0 == (i % 4U))
                {
                    IsRBPValue = TRUE;
                }
                else
                {
                    if (0 == (i % 2U))
                    {
                        IsRBPValue = FALSE;
                    }
                }

                if (TRUE == IsRBPValue)
                {
                    if (2 == CNT)
                    {
                        CNT = 0;
                    }

                    for (j = 0; j < 8; ++j)
                    {
                        ReadbackP[(CNT*8)+j] = ReadbackP[(CNT*8)+j] + ((RBData >> j) & 0x01);
                    }

                    ++CNT;
                }
                else
                {
                    if (2 == CNT)
                    {
                        CNT = 0;
                    }

                    for (j = 0; j < 8; ++j)
                    {
                        ReadbackN[(CNT*8)+j] = ReadbackN[(CNT*8)+j] + ((RBData >> j) & 0x01);
                    }

                    ++CNT;
                }
            }
            else
            {

                /*DTXT_REPORT2(DTXT_REPORT_INFO, "Read FPGA error", 0x8000u + 2u * i, RBData);*/
                Status = ALSTOM_C_ERROR;
                break;
            }
        }

        /* ��������⣬����do while(0) */
        if (ALSTOM_C_ERROR == Status)
        {
            ErrNo = 3;
            break;
        }

        /*�˿ڻض��˲�*/
        for (i = 0U; i < GUEST_C_OUTPUT_PORT_NUM; ++i)
        {
            SumRBPValue[i] = GUEST_F_Filter_Check((INT32U)ReadbackP[i]);
            SumRBNValue[i] = GUEST_F_Filter_Check((INT32U)ReadbackN[i]);
        }/**/

    }
    while (0);

    /*DTXT_REPORT6(DTXT_REPORT_INFO, "Read FPGA P",SumRBPValue[0],SumRBPValue[3],SumRBPValue[7],SumRBPValue[8],SumRBPValue[13],SumRBPValue[15]);
    DTXT_REPORT6(DTXT_REPORT_INFO, "Read FPGA N",SumRBNValue[0],SumRBNValue[3],SumRBNValue[7],SumRBNValue[8],SumRBNValue[13],SumRBNValue[15]);*/
    if (Status != ALSTOM_C_NO_ERROR)
    {
        DTXT_REPORT1(DTXT_REPORT_INFO, "GUEST_F_OPT_RBValueFromFPGA Error", ErrNo);
    }

    return;
}



void GUEST_F_MemoryBUSTest(void)
{

    ALSTOM_T_Status Status = ALSTOM_C_NO_ERROR;

    INT32U i = 0u;

    INT16U RBData = 0u;

    INT32U StatusCRC = 0u;
    INT32U RMTStatusCRC = 0u;

    /* ���FPAG DPRAM������һ����ַ���μ�飬���õļ�鳣��Ϊ0x5A��0xA5�����ͨ������� */

    for (i = 0U;i < (4U*32U);++i)
    {

        GUEST_M_FPGA_Write((GUEST_C_RB_VALUE_ADDR + 2 * i), 0x5A);
        GUEST_M_FPGA_Read((GUEST_C_RB_VALUE_ADDR + 2 * i), RBData);

        if (0xA55A != RBData)
        {

            /*DTXT_REPORT2(DTXT_REPORT_INFO, "Check FPGA error", GUEST_C_RB_VALUE_ADDR + 2 * i, RBData);*/
            Status = ALSTOM_C_ERROR;
            break;
        }

        GUEST_M_FPGA_Write((GUEST_C_RB_VALUE_ADDR + 2 * i), 0xA5);

        GUEST_M_FPGA_Read((GUEST_C_RB_VALUE_ADDR + 2 * i), RBData);

        if (0x5AA5 != RBData)
        {

            /*DTXT_REPORT2(DTXT_REPORT_INFO, "Check FPGA error", GUEST_C_RB_VALUE_ADDR + 2 * i, RBData);*/
            Status = ALSTOM_C_ERROR;
            break;
        }

        GUEST_M_FPGA_Write((GUEST_C_RB_VALUE_ADDR + 2 * i), 0x0);
    }

    /* ����Status���鿴ǰ���Ƿ�ɹ�����ˣ�����������򷵻�*/
    MSTEP_F_CryptlibCRC32FSFB2ChanOneCompute((INT8U *)&Status, sizeof(ALSTOM_T_Status), &StatusCRC);

    MSTEP_F_K2oo2CpuExchange((INT8U *)&RMTStatusCRC, (INT8U *)&StatusCRC, sizeof(StatusCRC), GUEST_C_2oo2_TIMEOUT);

    if (RMTStatusCRC != StatusCRC)
    {
        Status = ALSTOM_C_ERROR;
    }

    DTXT_REPORT1(DTXT_REPORT_INFO, "=========MEMBUSTest Result=========", Status);

    GUEST_F_TFTP_SendTestInfo(GUEST_TFTP_MSG_RESU, 0x03, TFTP_MSG_MEMBUS, Status);

    return;
}


static GUEST_E_READBACK_SENSORS_STATE GUEST_F_Filter_Check(INT32U RBvalue)
{
    GUEST_E_READBACK_SENSORS_STATE  Result = GUEST_C_READBACK_SENSORS_UNDETERMINED;

    const GUEST_T_CFG *pCFGAddr = GUEST_F_GetConfigPointer();

    GDF_M_NULL_ASSERT(pCFGAddr);

    if (RBvalue >= pCFGAddr->THH)
    {
        Result = GUEST_C_READBACK_SENSORS_SET;
    }
    else if (RBvalue < pCFGAddr->THL)
    {
        Result = GUEST_C_READBACK_SENSORS_UNSET;
    }
    else
    {
        Result = GUEST_C_READBACK_SENSORS_UNDETERMINED;
    }

    return Result;
}


/* EOF */
