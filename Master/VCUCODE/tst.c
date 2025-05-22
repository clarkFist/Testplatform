#include "Tst.h"
#include "Tstswitch.h"
//#define TST_DEBUG

#ifdef  TST_DEBUG //����debug��,��ʱû��
//#define Tst_F_DebugReport(...)  DTXT_F_Report(__Tst_F_Va_argS__);
#define Tst_F_DebugReport  Tst_F_Report
#else
#define Tst_F_DebugReport(...)  ;
#endif

#define INTSIZEOF(n)  ((sizeof(n)+sizeof(int)-1)&~(sizeof(int) - 1))  //����������������ĺ�
#define va_arg(ap,t)    (*(t *)(((ap) += INTSIZEOF(t)) - INTSIZEOF(t)))
#define va_start(ap,v) ((ap) = (STD_T_VAList)&(v) + INTSIZEOF(v))
#define va_end(ap) ((ap) = (STD_T_VAList)0)

INT32U tst_cycle = 0u; //����������
INT8U localNodeID_A = 0u; //��ǰ�忨nodeID
INT8U localNodeID_B = 0u;
INT8U localNodeID = 0u;
INT32U TstServerIp = 0u; //��λ���ĵ�ַ
INT32S Tst_mode = Tst_C_FUNCTIONAL_BOOT;//��ʼ��Ϊbootģʽ
BOOLEAN Tst_Flag_UDPReport =  Tst_Flag_UDPReportDefault;  //����tst.h ���޸�
BOOLEAN Tst_Flag_SerialReport = Tst_Flag_SerialReportDefault;
BOOLEAN Tst_Flag_AllReport = Tst_Flag_AllReportDefault;
BOOLEAN Tst_Flag_AllReportFlag[2] = {TRUE,TRUE}; //��ʱ��ֹTst_Flag_AllReport ��Ч

INT8U Tst_Cycle_UDPReport = Tst_Cycle_UDPReportDefault;

BOOLEAN Tst_Flag_Task = TRUE;

INT8U isKernelCode = 0xFF; //��ֵ������,δ�õ�
//INT8U Tst_Cycle_SerialReport = Tst_Cycle_SerialReportDefault;



MSTEP_T_SocketID* Tst_V_pUdpSocket = NULL; //tst Socket ����

Tst_FrameBuf Tst_FrameBufSend;
Tst_FrameBuf Tst_FrameBufRecv;

Tst_ReportArgs Tst_ReportBuf[Tst_ReportBufMaxSize];

INT8U Tst_ReportBufType[Tst_ReportBufMaxSize];
INT8U Tst_ReportBufStart = 0;
INT8U Tst_ReportBufSize = 0;
BOOLEAN Tst_Control_flags[100] = {0};
BOOLEAN Tst_Maint_flags[100] = {0};
ALSTOM_T_Status Tst_F_UDPSend(INT32U destAddr ,Tst_UDPFrame* pData,const char* srcID);
ALSTOM_T_Status Tst_F_FrameStore(Tst_FrameBuf* FrameBuf, BOOLEAN prio, Tst_UDPFrame* data );
ALSTOM_T_Status Tst_F_FrameGet(Tst_FrameBuf* FrameBuf,Tst_UDPFrame* ret);



//void Tst_F_UDPReport(DTXT_E_ReportType    Type,
//                   const char*            locString,
//                   const char*            String,
//                   INT8U            Numbval,
//                   INT32U           Value1,
//                   INT32U           Value2,
//                   INT32U           Value3,
//                   INT32U           Value4,
//                   INT32U           Value5,
//                   INT32U           Value6);
//
//void Tst_F_SerialReport(DTXT_E_ReportType    Type,//store
//                   const char*            locString,
//                   const char*            String,
//                   INT8U            Numbval,
//                   INT32U           Value1,
//                   INT32U           Value2,
//                   INT32U           Value3,
//                   INT32U           Value4,
//                   INT32U           Value5,
//                   INT32U           Value6);


INT16U Tst_F_Crc16(const char *buf,int len)
{
	int count;
	INT16U crc = 0xFFFF ,temp,judge;
	len *=8;
	for(count =0;count<len;count++)
	{
		if(count % 8 == 0)
		{
			temp = *buf<<8;
			buf++;
		}
		judge = crc ^ temp;
		crc <<= 1;
		temp <<= 1;
		if(judge& 0x8000)
		{
			crc ^= 0x1021;
		}
	}
	return crc;
}

/*UDP ������ָ�����͵�ַ���������ݣ��˿ں�Ĭ����tst.h��ָ��*/
ALSTOM_T_Status Tst_F_UDPSend(INT32U destAddr ,Tst_UDPFrame* pData, const char* srcTips)
{	
	ALSTOM_T_Status status = ALSTOM_C_ERROR;
	const MSTEP_T_SocketID *pSocket = (const MSTEP_T_SocketID *)Tst_V_pUdpSocket;
	INT16U destPort = Tst_UDP_PORT;
	/*if( (pData->dataType &0xF0) == 0x10 && pData->srcNodeID == localNodeID)//�������͵���ظ���Ϣ���crc
	{
			if( pData->dataLen <=  sizeof(Tst_UDPFrame) - 2)
			{
			
				//STD_F_Memcpy((INT8U*)pData + pData->dataLen, sizeof(Tst_UDPFrame),(INT8U*)&p , 2);
				*(INT16U*)((INT8U*)pData + pData->dataLen) = Tst_F_Crc16((const char *)pData, pData->dataLen);
				pData->dataLen += 2;
			}
			else
			{
				Tst_F_PrintReport("send udp msg length too long");
				return ALSTOM_C_ERROR;
			}
		
	}*/
	status = Lib_F_UDPSend(pSocket, destAddr, destPort, (const INT8U * )pData, pData->dataLen);
	if(status != ALSTOM_C_NO_ERROR)
		Tst_F_PrintReport("udp send fail,srcTips:%s,status:%d\n",srcTips,status);
	
	return status;
}
/*�����跢�͵���Ϣ�ݴ棬��n�������ڷ��͹����ظ���Ϣ�����ᱻ���ͣ���ౣ��20����Ϣ
����ÿ���ڷ��Ͳ�ͬ����Ϣ����20��ʱ�����˽�ʧЧ
�����͹��໺������ʱ�����ӡstore faile udp  ����Ϣ����Ų�����ʾ��ʧ*/
ALSTOM_T_Status Tst_F_ReportStore(Tst_ReportType tst_type ,DTXT_E_ReportType    Type,
                   const char*            locString,
                   const char*            String,
                   INT8U            Numbval,
                   INT32U           Value1,
                   INT32U           Value2,
                   INT32U           Value3,
                   INT32U           Value4,
                   INT32U           Value5,
                   INT32U           Value6)
{	
	static Tst_ReportArgs args;
	BOOLEAN isStore = TRUE;
	ALSTOM_T_Status status = 0u;
	const INT8U rejectMaxSize = 20;
	static Tst_ReportArgs rejectReportTemp[20];
	static INT32U rejectFlag[20];
	static INT8U rejectStart = 0;
	static INT8U rejectSize =0;
	INT32U i = 0,j =0;
	static INT32U preCycle = 0;
	
	STD_F_Memset(&args,0, sizeof(Tst_ReportArgs));
	args.Type = Type;
	args.Numbval = Numbval;
	args.Value1 = Value1;
	args.Value2 = Value2;
	args.Value3 = Value3;
	args.Value4 = Value4;
	args.Value5 = Value5;
	args.Value6 = Value6;
	for(i=0; locString[i]!=0;i++);	//�����ӡ��Ϣ����ֹSTD_F_VSnprintf �����������
	;
	i++;
	j = i>30?30:i;  //30 Ϊ��ϢԴ��ʶ ��󳤶�
	STD_F_Memcpy(args.locString,30, locString + i - j , j);
	
	for(i=0; String[i] !=0;i++);
	;
	i++;
	STD_F_Memcpy(args.String,120, String, i>120?120:i);//���ӡ����Ϣ��ౣ��ǰ120�ֽ�
	
	if(Tst_mode  ==  Tst_C_FUNCTIONAL_BOOT)//boot or close
	{
		if(tst_type != Report_Udp) // boot ģʽ�£���Ϣ���������ڻ����̴�ӡ����udp ���ȱ��棬��normal ��ʼ��ӡ
		{
			Tst_F_SerialReport(Type, locString,String, Numbval, Value1, Value2,  Value3, Value4, Value5, Value6);
			if(tst_type == Report_Serial)
			{
				return 0;
			}
			else
			{
				tst_type = Report_Udp;
			}
		}
	}
	else if(Tst_mode ==  Tst_C_FUNCTIONAL_CLOSE)
	{
		if(tst_type != Report_Udp)
		{
			Tst_F_SerialReport(Type, locString,String, Numbval, Value1, Value2,  Value3, Value4, Value5, Value6);
		}
		return 0;
	}
	
	for(;0<rejectSize;)
	{
		if(Tst_F_GetCycle() >= rejectFlag[rejectStart] )
		{
			if(rejectStart == rejectMaxSize -1)
			{
				rejectStart = 0;
			}
			else
				rejectStart ++;
			
			rejectSize--;
		}
		else 
		{//ֻ�����������Ϣ����������cycle����֮ǰ����Ϣ����ǰ����������
			break;//������������int32u ���ֵ����ʱ����ʱ����cycle ���ܵ��³�ʱ�䲻�ܴ�ӡ
			//rejectStart ++;
		}
	}
	
	for(i =0 ; i<rejectSize; i++)//�ж���Ϣ�Ƿ���Ҫ����
	{
		if( 0==   STD_F_Memcmp(rejectReportTemp +(rejectStart +i)% rejectMaxSize , &args,sizeof(Tst_ReportArgs)) )
		{
			isStore = FALSE;
			break;
		}
	}
	if(isStore == TRUE)
	{
		if(Tst_ReportBufSize < Tst_ReportBufMaxSize) // ����log
		{
				STD_F_Memcpy(Tst_ReportBuf +(Tst_ReportBufStart + Tst_ReportBufSize)%Tst_ReportBufMaxSize, sizeof(Tst_ReportArgs), &args, sizeof(Tst_ReportArgs));
				Tst_ReportBufType[(Tst_ReportBufStart + Tst_ReportBufSize)%Tst_ReportBufMaxSize] = tst_type;

				Tst_ReportBufSize++;
		}
		else
		{
			status = ALSTOM_C_ERROR;
			if(preCycle != Tst_F_GetCycle()) //һ��tst������ֻ���ӡһ��
			{
				Tst_F_SerialReport(DTXT_REPORT_INFO, DTXT_C_LOC , "Report Store Fail ", 0u, 0u, 0u, 0u,0u, 0u, 0u);
				Tst_F_UDPReport(DTXT_REPORT_INFO, DTXT_C_LOC , "Report Store Fail ", 0u, 0u, 0u, 0u,0u, 0u, 0u);
			}
		}
		if(Tst_Cycle_UDPReport > 0)//Tst_Cycle_UDPReport=1 ʱÿ��tst����ֻ���ӡһ�Σ�Ϊ0 ʱ��������
		{
			if(rejectSize ==rejectMaxSize) //���ܾ���Ϣ�����Զ�����ɵ�����ɾ��
			{
				if(rejectStart == rejectMaxSize -1)
				{
					rejectStart = 0;
				}
				else
					rejectStart ++;
				
				rejectSize--;
			}
			STD_F_Memcpy(rejectReportTemp + (rejectStart + rejectSize)%rejectMaxSize , sizeof(Tst_ReportArgs), &args, sizeof(Tst_ReportArgs));
			rejectFlag[(rejectStart + rejectSize)%rejectMaxSize] = Tst_F_GetCycle() + Tst_Cycle_UDPReport;  //reject ����ʱ�������Ϣ���ͣ��������ڻ�udp�������
			rejectSize++;
		}
	}
	else
	{
		;
	}
	preCycle = Tst_F_GetCycle();
	return 0;
}

/*�˺���ÿ�ε��ö��ᷢ�ͣ����͵���Ϣ�����̴��ڷ��ͻ�������*/
void Tst_F_UDPReport(DTXT_E_ReportType    Type,//store
                   const char*            locString,
                   const char*            String,
                   INT8U            Numbval,
                   INT32U           Value1,
                   INT32U           Value2,
                   INT32U           Value3,
                   INT32U           Value4,
                   INT32U           Value5,
                   INT32U           Value6)
{
	INT8U pos = 0;
	static Tst_UDPFrame data;
	static INT16U number = 0;
	INT8U size_Str = 0;
	INT8U start_Str = 0;
	INT8U max_Str ;
	const INT8U size_val =sizeof(INT32U);//INT32U ���� 4
	const char *Str;
	INT8U max_Head = (INT8U)(Tst_UDPDataMaxLen - Numbval*size_val);//�ַ���Ϣ�ĳ��ȡ���sizeof(data.data) > 42

	if(Numbval <0 || Numbval > 6 || locString == NULL ||String == NULL )
	{
		Tst_F_PrintReport("at %s, Numbval is:%d\n",locString,Numbval);
		return;
	}
	if( Tst_Flag_UDPReport == FALSE )
	{
		return;
	}
	
	STD_F_Memcpy(data.data+pos, sizeof(number) , &number,sizeof(number) );
	pos += sizeof(number);//number Ϊ��Ϣ���
	
	Str = locString;//
	for(size_Str = 0 ;Str[size_Str] !=0 ;size_Str++);
	;
	max_Str = 15;//��ϢԴ��ʶ ֻ�ᱣ����15λ
	start_Str = size_Str>max_Str?size_Str-max_Str:0;//
	size_Str = size_Str>max_Str?max_Str:size_Str;
	STD_F_Memcpy(data.data+pos, size_Str , Str + start_Str, size_Str);
	pos += size_Str;
	
	Str = String;//
	for(size_Str = 0 ;Str[size_Str] !=0 ;size_Str++);
	;
	max_Str = max_Head - pos - 1;// ʣ��������ַ���String �������ĳ��ȡ� -1 ��������־
	start_Str = 0;//
	size_Str = size_Str>max_Str?max_Str:size_Str;
	STD_F_Memcpy(data.data+pos, size_Str , Str + start_Str, size_Str);
	pos += size_Str;
	
	data.data[pos] = 0;
	pos += 1;

	pos += (Numbval>0?Numbval-1:0)*size_val;//��ĩβ��ֵ���ӷ��� Numbval = 0��1ʱ ����ı�ָ��λ��
	if(pos + size_val > sizeof(data.data) && Numbval > 0 )
	{
		Tst_F_UDPReport(3u,DTXT_C_LOC,"udp pos err",0u,pos+4, sizeof(data.data),0,0,0,0);
		Tst_F_SerialReport(3u,DTXT_C_LOC,"udp pos err",2u,pos+4,Tst_UDPDataMaxLen,0,0,0,0);
		return;
	}
	switch(Numbval)//ÿ��ֵһλָ����������ƶ�4������pos ��ص���һ��pos ֮ǰ��ֵ
	{
		case 6:
			STD_F_Memcpy(data.data+pos, size_val , (void*)&Value6, size_val);
			pos -=size_val;
		case 5:
			STD_F_Memcpy(data.data+pos, size_val , (void*)&Value5, size_val);
			pos -=size_val;
		case 4:
			STD_F_Memcpy(data.data+pos, size_val , (void*)&Value4, size_val);
			pos -=size_val;
		case 3:
			STD_F_Memcpy(data.data+pos, size_val , (void*)&Value3, size_val);
			pos -=size_val;
		case 2:
			STD_F_Memcpy(data.data+pos, size_val , (void*)&Value2, size_val);
			pos -=size_val;
		case 1:
			STD_F_Memcpy(data.data+pos, size_val , (void*)&Value1, size_val);
		case 0:
			break;
		default:
			break;
	}
	data.srcNodeID = localNodeID;
	data.dstNodeID = ethNodeId; //������̫��
	data.dataType= Tst_UdpDataType_log; //0E
	data.dataLen = pos + Numbval*size_val; // ��Ϣ�ܳ���
	data.dataLen = (INT16U)Tst_F_UDPLenGet(data.dataLen); //����ͷ�ļ��ĳ���
	number++;
	if( Tst_mode == Tst_C_FUNCTIONAL_NORMAL)
		Tst_F_UDPSend(TstServerIp,&data,"UDP log send");
	else
		Tst_F_FrameStore(&Tst_FrameBufSend, 0u,&data);
}
/*Tst_FrameBuf ��ΪFrameBufRecv ��FrameBufSend��prio Ϊ1ʱ������������ͷ����Ϊ0ʱ������������β�� �����˻����������������κβ�������ʱ������λ�����udp��Ϣ����ж��Ƿ񶪰�*/
ALSTOM_T_Status Tst_F_FrameStore(Tst_FrameBuf* FrameBuf, BOOLEAN prio, Tst_UDPFrame* data )
{
	ALSTOM_T_Status status = 0u;
	if(FrameBuf->size < FrameBufSize)
	{
		if(TRUE == prio)//prio Ϊ1ʱ������������ͷ��
		{
			if(FrameBuf->size > 0)
			{
				if(FrameBuf->pos == 0)
				{
					FrameBuf->pos =  FrameBufSize;
				}
				else
				{
					FrameBuf->pos--;
				}
			}
			STD_F_Memcpy(&FrameBuf->data[FrameBuf->pos], sizeof(Tst_UDPFrame), data, sizeof(Tst_UDPFrame));
			//FrameBuf->data[FrameBuf->pos] = data;
			FrameBuf->size  ++;
		}
		else 
		{
			if(FALSE== prio)//Ϊ0ʱ������������β��
			{
				STD_F_Memcpy(&FrameBuf->data[(FrameBuf->pos + FrameBuf->size)%FrameBufSize], sizeof(Tst_UDPFrame), data, sizeof(Tst_UDPFrame));
				//FrameBuf->data[FrameBuf->pos + FrameBuf->size] = data;
				FrameBuf->size++;

			}
			else
			{
				;
			}
		}
	}
	else
	{
		status = ALSTOM_C_ERROR;
		
		//Tst_F_Report(DTXT_REPORT_INFO, DTXT_C_LOC , "Frame Store Fail ", 0u, 0u, 0u, 0u,0u, 0u, 0u);
	}

	return status	;
}
ALSTOM_T_Status Tst_F_FrameGet(Tst_FrameBuf* FrameBuf,Tst_UDPFrame* ret)
{
	ALSTOM_T_Status status = ALSTOM_C_ERROR;
	if(FrameBuf->size > 0)
	{
		status = ALSTOM_C_NO_ERROR;
		STD_F_Memcpy(ret, sizeof(Tst_UDPFrame), &FrameBuf->data[FrameBuf->pos], sizeof(Tst_UDPFrame));
		
		if(FrameBuf->pos == FrameBufSize -1)
		{
			FrameBuf->pos = 0;
		}
		else
		{
			FrameBuf->pos ++;
		}
		FrameBuf->size --;
	}
	else
	{
		status = ALSTOM_C_ERROR;
	}
	return status;
}

void Tst_F_SerialReport(DTXT_E_ReportType    Type,//store
                   const char*            locString,
                   const char*            String,
                   INT8U            Numbval,
                   INT32U           Value1,
                   INT32U           Value2,
                   INT32U           Value3,
                   INT32U           Value4,
                   INT32U           Value5,
                   INT32U           Value6)
{
	if(Tst_Flag_SerialReport == TRUE)
	{
		Tst_Flag_AllReportFlag[0] = FALSE;  
		DTXT_F_Report(Type, locString,String, Numbval, Value1, Value2,  Value3, Value4, Value5, Value6);
		//Tst_Flag_AllReportFlag = TRUE;
	}
}
/*����reportstore ��������Ϣ*/
void Tst_F_ReportProcess()
{
	INT8U recv_count = 0u;
	INT8U max_recv = 11u;
	static Tst_ReportArgs args;
	ALSTOM_T_Status status = 0;
	INT8U type = 0xFFU;
	do{
		if(Tst_ReportBufSize >0)
		{
			STD_F_Memcpy(&args, sizeof(Tst_ReportArgs), Tst_ReportBuf +Tst_ReportBufStart, sizeof(Tst_ReportArgs));
			type = Tst_ReportBufType[Tst_ReportBufStart];
			Tst_ReportBufSize--;
			if(Tst_ReportBufStart == Tst_ReportBufMaxSize -1 )
			{
				Tst_ReportBufStart = 0;
			}
			else
				Tst_ReportBufStart++;
			
			switch(type)
			{
				case Report_Both:
					Tst_F_SerialReport(args.Type,args.locString,args.String,args.Numbval,args.Value1,args.Value2,args.Value3,args.Value4,args.Value5,args.Value6);
					Tst_F_UDPReport(args.Type,args.locString,args.String,args.Numbval,args.Value1,args.Value2,args.Value3,args.Value4,args.Value5,args.Value6);

					break;
				case Report_Serial:
					Tst_F_SerialReport(args.Type,args.locString,args.String,args.Numbval,args.Value1,args.Value2,args.Value3,args.Value4,args.Value5,args.Value6);
					break;
				case Report_Udp:
					Tst_F_UDPReport(args.Type,args.locString,args.String,args.Numbval,args.Value1,args.Value2,args.Value3,args.Value4,args.Value5,args.Value6);
					break;
				default:
					break;
			}

		}
		else
		{
			status = ALSTOM_C_ERROR;
		}
		recv_count ++;
		if(recv_count >= max_recv)
			break;
	}while(status == 0u);
}
/*��print����ʽд����ӡ���������*/
void Tst_F_PrintReportStore(Tst_ReportType tst_type,const char* locString,const char* fmt,...)
{
	static char logStr[300];
	STD_T_VAList args;
    GDF_M_NULL_ASSERT(fmt);
		
	va_start(args,fmt);
	STD_F_VSnprintf(logStr, sizeof(logStr), fmt, args);
	va_end(args);
	Tst_F_ReportStore(tst_type,4u, locString, (const char*)logStr, 0u,0u,0u,0u,0u,0u,0u);
	return;
}
void Tst_F_UDPPrint(const char* locString,const char* fmt,...)
{
	static char logStr[300];
	STD_T_VAList args;
	GDF_M_NULL_ASSERT(fmt);
		
	va_start(args,fmt);
	STD_F_VSnprintf(logStr, sizeof(logStr), fmt, args);
	va_end(args);
	Tst_F_UDPReport(4u, locString, (const char*)logStr, 0u,0u,0u,0u,0u,0u,0u);
	return;
}
void Tst_F_EntrySafetyMode()
{
	INT8U test;
	Tst_Control_flags[ReprogflagOnDump] = FALSE;
	Tst_F_K2oo2CpuExchange(&test, &test, 1, 0x1f4);
	GDF_M_FAULT_EXT(EVT_C_SAFETY_CORRUPTED, 0, "Tst_F_EntrySafetyMode");
}
void Tst_F_TriggerFaultfifter()
{
	Tst_Control_flags[FaultFifter] = TRUE;
	Tst_F_EntrySafetyMode();
	
}

void Tst_F_OnFault(INT8U eventID, INT32U context, const char *pFileName, INT16U line, INT8U taskPrio)
{
	char logStr[200];
	STD_F_Snprintf(logStr, sizeof(logStr), "%s:%d>eventID:%d,taskPrio:%d,context:%x", pFileName,line,eventID,taskPrio,context);
	Tst_F_UDPReport(DTXT_REPORT_CRITICAL,"",logStr,0u,0,0,0,0,0,0);
}
INT32U Tst_F_GetCycle()
{
	return tst_cycle;
}
void Tst_F_SendMaintMsg(INT8U msgType, const char* fmt,...)
{
	static char logStr[Tst_UDPDataMaxLen - 2];
	static Tst_UDPFrame data;
	Tst_UDPMaintFrame *msg = (Tst_UDPMaintFrame *)data.data;
	STD_T_VAList args;
	GDF_M_NULL_ASSERT(fmt);
	
	STD_F_Memset(logStr, 0, sizeof(logStr));
	va_start(args,fmt);
	STD_F_VSnprintf(logStr, sizeof(logStr), fmt, args);
	va_end(args);
	msg->maint_type = msgType;
	msg->req_ack = 3;
	STD_F_Memcpy(msg->msg, Tst_UDPDataMaxLen - 2, logStr, Tst_UDPDataMaxLen - 2);
	
	data.srcNodeID = localNodeID;
	data.dstNodeID = ethNodeId; //������̫��
	data.dataType = Tst_UdpDataType_maint;
	data.dataLen = 2 + STD_F_Strlen(logStr); // ��Ϣ�ܳ���
	data.dataLen = (INT16U)Tst_F_UDPLenGet(data.dataLen); //����ͷ�ļ��ĳ���
	Tst_F_FrameStore(&Tst_FrameBufSend, 0u,&data);
}
INT8U Tst_F_SendEventLog()
{
	static int count = 0;
	Tst_Maint_flags[GetEventLog] = FALSE;
}
INT32U Tst_F_GetVCUTime(void)
{
	MSTEP_T_CanTimeOfDay  pTime;
	MSTEP_F_TimeGet(&pTime);
	if(pTime.day > 48)
	{
		pTime.day %= 49;
	}
	return 1000*60*60*24*pTime.day + pTime.ms;
}
