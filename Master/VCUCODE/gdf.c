/*20180608, change fuse check method, CR:871*/
#include "gdf.h"
#include "gdf_dump_tools.h"
#include "BIT_fuse.h"
#include "std.h"
#include "ucos_ii.h"
#include "step_guest.h"
#include "Flash.h"
#include "Schedule_utl.h"
#include "step_guest.h"
#include "RECORD_evt.h"
#include "mcf54418.h"
#include "std.h"
#include "Drv_Led_Lib.h"
#include "BIT.h"
#include "Drv_Boot_Utils.h"
#include "MATH.h"        //��������,added by zhaoxifa
#include "Drv_AxB_Lib.h"
#ifndef OS_UCOS
#include "reworks.h"
#include "core/private/coretask.h"
#include <core/private/core.h>
#include "RECORD_evt.h"
#endif

#ifdef TestPlatform
void Tst_F_BreakReporg(INT32U time);
#include "tstswitch.h"
#include "test_def.h"
#endif
#ifdef VCU_TB
static INT32U testfuse1[1];
static INT32U testfuse2[1];
static INT32U testfusetoggle = 0;
#endif
SECTION_UDATA_SRAM GDF_T_LastEvent GDF_V_LastEvent;

void GDF_F_FuseXirq(void);
void GDF_F_FuseSystemInSafety(GDF_T_EventId eventID,
                              INT32U context,
                              const char*pFileName,
                              INT16U line,
                              INT8U taskPrio);
BOOLEAN GDF_F_FuseSwBurnArea(GDF_E_FuseAddrId fuseAddrId);
INT32U GDF_F_FileNameCrcCompute(const char* buf);

/*�����ļ�����CRC*/
INT32U GDF_F_FileNameCrcCompute(const char* buf)
{
    INT32U CRC = 0u;
    INT32U strLen = 0u;
    INT32U initVal = 0u;
    const char *fileNamePos = NULL;
    /*20190516, input argument * buf should not be NULL,CR:1435*/
    /*GDF_M_NULL_ASSERT(buf);  CRCC-6D*/

    strLen = STD_F_Strlen((const char*)buf);
    /*20181218, file name CRC is wrong, CR:1249*/
    initVal = 0xFFFFFFFFu;

    /*ͨ��б�ܷ��ҵ�·�������������ļ�����ʼλ�ã�������fileNamePos��*/

    while (strLen > 0u)
    {
        --strLen;

        if (0x2Fu != (INT32U)(buf[strLen]))/*ASCII��0x2F��Ӧ'/'б�ܷ�*/
        {
            /*20200629, File name CRC calculation error, CR:2239*/
            /*--strLen;  ���ֽ��ļ���CRC�������*/

            fileNamePos = &buf[strLen];
        }

        if (0u != strLen)
        {
            if (0x2Fu == (INT32U)(buf[strLen]))
            {
                break;
            }
        }
    }

    strLen = STD_F_Strlen((const char*)fileNamePos);

    MATH_F_ComputeCRC32CCITT((const INT8U*)fileNamePos, strLen, &initVal, &CRC);

    return CRC;
}

/*��2����˿����˿���ؽ���дֵ����,zlp,20210506*/
/*20210803, change fuse check method to use one big group, CR:2915*/
ALSTOM_T_Status GDF_F_FusesWrite(GDF_E_FuseAddrId fuseAddrId,
                                 INT32U startIndex,
                                 INT32U* pFusesValues,
                                 INT32U nbFuses)
{
    INT32U fuseaddr = 0u;
    ALSTOM_T_Status ret = ALSTOM_C_ERROR;
    ALSTOM_T_Status errCode = ALSTOM_C_NO_ERROR;//gjc 20170920

    if (fuseAddrId > GDF_C_FUSE_ADDR_ID_TOGGLE)
    {
    	errCode = ALSTOM_C_ERROR;
    }
    else
    {
		if (GDF_C_FUSE_ADDR_ID_TOGGLE != fuseAddrId)
		{
			if (GDF_C_FUSE_ADDR_ID_1 == fuseAddrId)
			{
				fuseaddr = GDF_C_SOFT_FUSE_FLASH_1_ADDR;
			}
			else if (GDF_C_FUSE_ADDR_ID_2 == fuseAddrId)
			{
				fuseaddr = GDF_C_SOFT_FUSE_FLASH_2_ADDR;
			}
			else
			{
				; /*nothing to do*/
			}
	
			if (startIndex > 19u)
			{
				errCode = ALSTOM_C_ERROR;
			}
			else
			{
				if (0u != startIndex)
				{
					fuseaddr = fuseaddr + 0xFF00u + ((startIndex - 1u) * 4u);
				}
	
				if (0u != fuseaddr)//���쳣����Ĵ������ڷ����Ա��
				{
					ret = FLASH_F_WriteDirect(2u, fuseaddr, ((INT8U *)pFusesValues), (4u * nbFuses));
	
					if (ALSTOM_C_NO_ERROR != ret)
					{
						errCode = ALSTOM_C_ERROR;
					}
				}
				else
				{
					errCode = ALSTOM_C_ERROR;
				}
			}
		}
		else
		{
			fuseaddr = GDF_C_SOFT_FUSE_FLASH_TOGGLE_ADDR;
	
			ret = FLASH_F_WriteDirect(2u, fuseaddr, ((INT8U *)pFusesValues), (4u * nbFuses));
	
			if (ALSTOM_C_NO_ERROR != ret)
			{
				errCode = ALSTOM_C_ERROR;
			}
		}
    }

    return errCode;
}

BOOLEAN GDF_F_FuseSwBurnArea(GDF_E_FuseAddrId fuseAddrId)
{
    BOOLEAN bRet = FALSE;
    ALSTOM_T_Status status = ALSTOM_C_NO_ERROR;
    INT32U fuseValue = 0u;

    status = GDF_F_FusesWrite(fuseAddrId, 0u, &fuseValue, 1u);

    if (ALSTOM_C_NO_ERROR == status)
    {
        bRet = TRUE;
    }

    return bRet;
}

void GDF_F_FuseSystemInSafety(GDF_T_EventId eventID,
                              INT32U context,
                              const char*pFileName,
                              INT16U line,
                              INT8U taskPrio)
{
    OSW_M_CRITICAL_VARIABLE;

    INT32U i = 0u;
    BOOLEAN ret = FALSE;
    GDF_E_FuseAddrId fuseState = GDF_C_FUSE_ADDR_ID_UNKNOWN;//�޸ı�������,zlp,20171219
    INT32U fileNameCrc = 0u;
    BOOLEAN bFaultFilterAllowed = FALSE;
    BOOLEAN bResetAllowed = FALSE;
    BOOLEAN BurnFuse = TRUE;
    ALSTOM_E_FunctionalMode funMode = ALSTOM_C_FUNCTIONAL_BOOT;
    INT16U dumpFlag = 0xAAAAu;

    static BOOLEAN bFirstGdfCall = TRUE;
    static BOOLEAN bInhibFilteringGdf = FALSE;//��ʼ��,zlp,20171222

#if 0 // v2, disable all interrupt,disable irq7
    MCF_INTC0_IMRL |= MCF_INTC_IMRL_MASKALL;
    MCF_EPORT_EPIER &= (INT8U)~MCF_EPORT_EPIER_EPIE7;
#else
    // v4,disable all interrupt,disable irq7
    MCF_INTC_SIMR(0) = MCF_INTC_SIMR_SALL;
    MCF_INTC_SIMR(1) = MCF_INTC_SIMR_SALL;
    MCF_INTC_SIMR(2) = MCF_INTC_SIMR_SALL;
    MCF_EPORT_EPIER &= ~(INT8U)MCF_EPORT_EPIER_EPIE7;
#endif
    /*������һ��ͨ�������ж�����,��Ӧ����һ��ͨ����7���ж�*/
    DEF_Drv_Kill_Distant_CPU();

    BIT_F_SysChkWdReset(); /*����ϵͳУ����*/
    
    funMode = SCHED_F_GetFunctionalMode();

    OS_ENTER_CRITICAL();

    Drv_Ext_WatchDog_F_Disable();//�����ⲿ���Ź�

    OS_EXIT_CRITICAL();

//    do
//    {
	fileNameCrc = GDF_F_FileNameCrcCompute(pFileName);

	if (TRUE == bFirstGdfCall)
	{
		bFirstGdfCall = FALSE;
		ret = GUEST_F_On_GDFSafe(eventID, context, fileNameCrc, line, taskPrio);
		PRINT_F_PrintfAuth(1u);
		bFaultFilterAllowed = BIT_F_IsFaultFilteringAllowed(&bInhibFilteringGdf);
#ifdef TestPlatform
		if(  Tst_Control_flags[FaultFifter] == TRUE )
		{
			bFaultFilterAllowed = TRUE;
		}
#endif
		if ((TRUE != ret) && (FALSE != bFaultFilterAllowed))
		{
			bResetAllowed = TRUE;
			bFaultFilterAllowed = TRUE;
			BurnFuse = FALSE;
//                break;
		}
	}
	else
	{
		PRINT_F_PrintfAuth(1u);
		bFaultFilterAllowed = BIT_F_IsFaultFilteringAllowed(&bInhibFilteringGdf);
	}
#ifdef TestPlatform
#ifndef VCU_BURN_FUSE_DISABLE 
	 if(Tst_Control_flags[NotBurnFuse] == TRUE)
	 {
		 BurnFuse = FALSE;//tst
	 }
	 else
	 {
		 //BurnFuse = TRUE; // �������ҲҪ����˿
	 }
	 
#else
	//	bFaultFilterAllowed = TRUE;  
	#ifdef ReprogBreakOnDump
	 if( ((OSW_F_TickGet()/1000) < ReprogBreakOnDump) && Tst_Control_flags[ReprogflagOnDump] == TRUE )
	 {
		 FLASH_F_SuspendDirect(2u);
		 Tst_F_BreakReporg(ReprogBreakOnDump);
		 BurnFuse = FALSE;
	 }
	 Tst_Control_flags[ReprogflagOnDump] == FALSE;
	#endif
#endif	 
#endif

	if(TRUE == BurnFuse)
	{
        FLASH_F_SuspendDirect(2u);

        /*��2����˿+��˿���ؽ����۶�,zlp,20210506*/
		/*20210803, change fuse check method to use one big group, CR:2915*/
#ifdef VCU_BURN_FUSE_DISABLE //��ʱע�͵��۶���˿ֵ,�������, by SXR

#else
        /*�۶���˿1*/
        ret = GDF_F_FuseSwBurnArea(GDF_C_FUSE_ADDR_ID_1);

        if (FALSE == ret)/*�۶���˿ʧ��*/
        {
            fuseState = GDF_C_FUSE_ADDR_ID_1;
        }
        else
        {
            fuseState = GDF_C_FUSE_ADDR_ID_UNKNOWN;
        }

        /*�۶���˿2*/
        ret = GDF_F_FuseSwBurnArea(GDF_C_FUSE_ADDR_ID_2);

        if (FALSE == ret)/*�۶���˿ʧ��*/
        {
            fuseState = GDF_C_FUSE_ADDR_ID_2;
        }

        /*�۶���˿����*/
        ret = GDF_F_FuseSwBurnArea(GDF_C_FUSE_ADDR_ID_TOGGLE);

        if (FALSE == ret) /*�۶���˿ʧ��*/
        {
            FLASH_F_ResumeDirect(2u);

            fuseState = GDF_C_FUSE_ADDR_ID_TOGGLE;

            do
            {
                ret = GDF_F_FuseSwBurnArea(fuseState);
            }
            while (FALSE == ret);
        }
        else /*�۶���˿�ɹ�*/
        {
            FLASH_F_ResumeDirect(2u);

            if (GDF_C_FUSE_ADDR_ID_UNKNOWN != fuseState)
            {
                do
                {
                    ret = GDF_F_FuseSwBurnArea(fuseState);
                }
                while (FALSE == ret);
            }
        }

#endif
	}
//    }
//    while (0);

    GDF_V_LastEvent.evntID = eventID;

    GDF_V_LastEvent.context = context;

    GDF_V_LastEvent.NameCrc = fileNameCrc;

    GDF_V_LastEvent.line = line;

    GDF_V_LastEvent.taskPriority = taskPrio;

    /*��һ���������кš��������ȼ����¼�ID�����4�ֽ��������и�λ2�ֽ�Ϊ�кţ���λ2�ֽڷֱ�Ϊ���ȼ����¼�ID��*/
    RECORD_F_EventDirectLog((((INT32U)line * 0x10000u) + ((INT32U)taskPrio * 0x100u) + (INT32U)eventID), fileNameCrc, context);
/*20210302,write dump segment info to SPI separately,CR:2606*/
	FLASH_F_WriteDirect(2u, 0x480010u, (INT8U*)&dumpFlag, 2u);  /*set 0xAAAA as a flag of dump*/
    FLASH_F_EraseDirect(0u, 0u, 0x10000u);
    FLASH_F_WriteDirect(0u, 0x0u, (INT8U*)&dumpFlag, 2u);/*set 0xAAAA as a flag of dump*/

    /*���ж�*/

    OSW_M_ENTER_CRITICAL();

#ifdef VCU_CONF_CPU_A
    DEF_Drv_Hmi_F_CPU_OK_Clear();/*Ϩ�������OK��*/

#else
    DEF_Drv_Hmi_F_CPU_OK_Clear();/*Ϩ�������OK��*/// by J.s.

#endif
/*20210302,In UPDATE and initial stage of INIT mode, MVCU can not switch to SAFE_STATE correctly,CR:2416*/
    if((ALSTOM_C_FUNCTIONAL_BOOT != funMode) && (ALSTOM_C_FUNCTIONAL_REPROG != funMode))
    {
    	SCHED_F_MasterLedClear(); /*20190718, add functions for controlling EXT LED and ETH LED, CR:1631*/
    }
    OSW_M_EXIT_CRITICAL();


    OSW_M_ENTER_CRITICAL();

    DEF_Drv_Hmi_F_CPU_ERR_Set();  /*�������Error��*/

    DEF_Drv_Hmi_F_CPU_OK_Set();/*���������OK��*/

    OSW_M_EXIT_CRITICAL();

    /*20180925, add Safe state print info, CR:1133*/
    PRINT_F_Printf("errCode %d, context %d, fileName %s, lineNumber %d, threadPrio %d\n",
                   eventID, context, pFileName, line, taskPrio);

    ret = GUEST_F_On_GDFEnding(eventID, context, fileNameCrc, line, taskPrio);

    GDF_DUMP_TOOLS_F_DumpMemory(ret, &GDF_V_LastEvent, bFaultFilterAllowed);

    DEF_Drv_Hmi_F_CPU_OK_Clear();/*Ϩ�������OK��*/

//  if ((0x5F66D3C1 == ($FFFB1600)) && (0x45BE9A6B == ($FFFB1604)))    do not use this! Ask GJL.
#if 0
    if (0)
    {
        STD_F_GenericPause(0x32);
        OSCpuReset();/*�����λ����*/
    }
    else
    {
#endif

    if (FALSE != bResetAllowed)
    {
        STD_F_GenericPause(0x32u);
        OSCpuReset();/*�����λ����*/
    }

    while (1)
    {
        for (i = 0u; i < 99999u; i++)
        {
            ; /*to do nothing*/
        }

        /*Ƭѡcs3, ������һ��ͨ�������ж����󣬶�Ӧ����һ��ͨ����7���ж�*/
        DEF_Drv_Kill_Distant_CPU();
    }
}


/*��Ӧ7���жϵ��жϴ�����*/
void GDF_F_FuseXirq(void)
{
    OSW_M_CRITICAL_VARIABLE;
    INT8U taskPri = 0u;
    INT32U i = 0u;
    INT32U count = 0u;
    BOOLEAN Irq6Detected = FALSE;
    INT16U distantPowerSupplyFailureCounter = 0u;
#ifndef OS_UCOS
    tcb_t *ptcb = NULL;
    ptcb = currentTcbGet();
#endif

    distantPowerSupplyFailureCounter = BIT_F_GetDistantPowerSupplyFailure();//�ѷſ�,zlp,20171212

    // V2 use IRQ3 , V4 use IRQ6;
    /*20180925, board goes into safe state when power off, CR:1133*/
    /*Ϊ�˽����ϵ�µ�ʱIRQ6��IRQ7���������¼�ⲻ��IRQ6��崻������⣬��IRQ6ѭ�����100������ж�*/

    do
    {
        count++;

        if (0u != (MCF_EPORT_EPFR & MCF_EPORT_EPFR_EPF6))
        {
            Irq6Detected = TRUE;
            break;
        }

    }
    while (count <= 1000u);

//    PRINT_F_Printf("GDF_F_FuseXirq irq6detected count %d \n", count);

    if ((FALSE == Irq6Detected) && (0u == distantPowerSupplyFailureCounter)) //0��ʾIRQ3δ�յ��ж�
    {
        GDF_DUMP_TOOLS_F_MemorizeModeBeforeGdf();
        /*20200228, reorganize code and add comments, CR:2006*/
#ifdef OS_UCOS

        if (NULL != OSTCBCur)
        {
            taskPri = OSTCBCur->OSTCBPrio;
            GDF_F_FuseSystemInSafety((GDF_T_EventId)0xFFu, 0xAAAAAAAAu, "./src/gdf.c", (INT16U)(__LINE__), ((INT8U)OS_LOWEST_PRIO - taskPri));
        }

#else
        if (NULL != ptcb)
        {
            taskPri = (INT8U)(ptcb->prio); //������,��ǿת,zlp,20171220

            GDF_F_FuseSystemInSafety((GDF_T_EventId)0xFFu, 0xAAAAAAAAu, "./src/gdf.c", (INT16U)(__LINE__), ((INT8U)OS_LOWEST_PRIO - taskPri));
        }

#endif
    }
    else
    {
        OS_ENTER_CRITICAL();
        Drv_Ext_WatchDog_F_Disable();// Disable watchdog
        OS_EXIT_CRITICAL();

        for (i = 0u; i < 10000u; i++)
        {
            /*���PD0Ϊ0����ͨ��PD0����һϵ��IRQ7�����ж�����*/
            DEF_Drv_Kill_Distant_CPU();

            STD_F_GenericPause(0x01u);
        }

        GDF_M_FAULT_EXT(EVT_C_HOT_REBOOT, (((INT32U)distantPowerSupplyFailureCounter * 0x10000u) + (MCF_GPIO_PODR_D&MCF_GPIO_PODR_D_PODR_D0)), "");
    }

}

void GDF_F_LogInternal(GDF_T_EventId eventID, INT32U context, const char *pFileName, INT16U line, INT8U taskPrio, GDF_T_EventInfoNotif callBackNotif)

{
    INT32U CRC = 0u;

    CRC = GDF_F_FileNameCrcCompute(pFileName);

    /*��һ���������кš��������ȼ����¼�ID�����4�ֽ��������и�λ2�ֽ�Ϊ�кţ���λ2�ֽڷֱ�Ϊ���ȼ����¼�ID��*/
    RECORD_F_EventLog((((INT32U)line * 0x10000u) + ((INT32U)taskPrio * 0x100u) + (INT32U)eventID), CRC, context, callBackNotif); //�ѷſ�,zlp,20171212
}

/******************************************************************
���ܣ���ӡ��ǰ����λ�ô�pStk�Ķ�ջ��Ϣ����СΪstkSize�ֽ�
����ֵ��Dump�����Ƿ�ɹ�
******************************************************************/
/*20181218, the print dump stack is wrong, CR:1251*/
void GDF_F_DumpStack(OS_STK * pStk, INT32U stkSize)
{
    INT32U num = 0u;
    INT32U DumpStk = 0u;

    DumpStk = (INT32U)pStk;

    if (NULL == pStk)
    {
        DumpStk = (INT32U) & pStk;/*���Ϊ�գ���dump��ǰ��ջ��Ϣ*/
    }

    if (0u < stkSize)
    {

        /*20190718, modify SDRAM_DATA size defined in LCF files from 8M to 16M, CR:1627*/
        while (1)
        {
            /*dump sram and data_sdram*/
            if ((((INT32U)__SRAM <= DumpStk) && ((INT32U)(__SRAM + SRAM_SIZE_64K) > DumpStk)) ||   \
                (((INT32U)__DATA_EXT_SDRAM_DATA_START <= DumpStk) && ((INT32U)(MSTEP_SDRAM_SIZE_16M + __DATA_EXT_SDRAM_DATA_START) > DumpStk)))
            {
                PRINT_F_Printf("Stack nb %d, address= 0x%08x, value= 0x%08x\n", num, DumpStk, *((INT32U*)DumpStk));
            }

            ++num;

            DumpStk += 4u;

            if (num >= stkSize)
            {
                break;
            }
        }
    }
    return;
}

void GDF_F_FaultInternal(GDF_T_EventId eventID, INT32U context, const char *pFileName, INT16U line, INT8U taskPrio)
{

	OSW_M_CRITICAL_VARIABLE;

    /*���ж�*/
    OSW_M_ENTER_CRITICAL();
    GDF_DUMP_TOOLS_F_MemorizeModeBeforeGdf();

#ifdef OS_UCOS
    /*�����������*/

    if (OSIntNesting != 0xFFu)
    {
        OSIntNesting += 1u;
    }

#else
    sched_lock();

#endif

    GDF_F_FuseSystemInSafety(eventID, context, pFileName, line, taskPrio);

    OSW_M_EXIT_CRITICAL();
}
#ifdef TestPlatform
BOOLEAN Tst_F_FuseReprog(GDF_E_FuseAddrId fuseAddrId)
{
	 BOOLEAN bRet = FALSE;
	 ALSTOM_T_Status status = ALSTOM_C_NO_ERROR;
	INT32U fuseValue = 0u;
	status = GDF_F_FusesWrite(fuseAddrId, 1u, &fuseValue, 1u);

	if (ALSTOM_C_NO_ERROR == status)
	{
		bRet = TRUE;
	}

	return bRet;
}
void Tst_F_BreakReporg(INT32U time)
{
	
	GDF_E_FuseAddrId fuseState = GDF_C_FUSE_ADDR_ID_UNKNOWN;
	BOOLEAN ret = FALSE;
	ret = Tst_F_FuseReprog(GDF_C_FUSE_ADDR_ID_2);

	if (FALSE == ret)/*�۶���˿ʧ��*/
	{
		fuseState = GDF_C_FUSE_ADDR_ID_2;
	}
	else
	{
		fuseState = GDF_C_FUSE_ADDR_ID_UNKNOWN;
	}

	/*�۶���˿1*/
	ret = Tst_F_FuseReprog(GDF_C_FUSE_ADDR_ID_1);

	if (FALSE == ret)/*�۶���˿ʧ��*/
	{
		fuseState = GDF_C_FUSE_ADDR_ID_1;
	}
	else /*�۶���˿�ɹ�*/
	{
		FLASH_F_ResumeDirect(2u);

		if (GDF_C_FUSE_ADDR_ID_UNKNOWN != fuseState)
		{
			do
			{
				ret = Tst_F_FuseReprog(fuseState);
			}
			while (FALSE == ret);
		}
	}
}
#endif
