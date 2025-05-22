#include "tst_tsk.h"
#include "tst.h"
#include "tstswitch.h"
extern ALSTOM_T_Status Tst_F_FrameStore(Tst_FrameBuf* FrameBuf, BOOLEAN prio, Tst_UDPFrame* data );
extern ALSTOM_T_Status Tst_F_FrameGet(Tst_FrameBuf* FrameBuf,Tst_UDPFrame* ret);
extern ALSTOM_T_Status Tst_F_UDPSend(INT32U destAddr ,Tst_UDPFrame* pData,const char* srcID);
extern void Tst_F_TstSwitchInit();

extern void Tst_F_ReportProcess();

extern MSTEP_T_SocketID* Tst_V_pUdpSocket;

extern INT32S Tst_mode ;
extern INT8U Tst_Cycle_UDPReport;
extern Tst_FrameBuf Tst_FrameBufSend;
extern Tst_FrameBuf Tst_FrameBufRecv;
extern INT32U tst_cycle;
int g_testcycleNo = 0;

void Tst_F_ReportFlagSet(Tst_UDPConfArgs *);
void Tst_F_ReportCycleSet(Tst_UDPConfArgs *);
void Tst_F_ControlFlagTask(ALSTOM_E_FunctionalMode functionalMode);
void Tst_F_MaintTask(ALSTOM_E_FunctionalMode functionalMode);
void Tst_F_ControlFlagSet(Tst_UDPConfArgs * args);
void Tst_F_OnMaint(Tst_UDPMaintFrame *Frame);
extern INT16U Tst_F_Crc16(const char *buf,int len);

#define  Tst_ConfPonterLen  30
void (*Tst_ConfPonter[Tst_ConfPonterLen]) (Tst_UDPConfArgs *); //函数指针数组,在creat 里赋值
INT8U  Tst_ConfPonterID[Tst_ConfPonterLen] = {0};

//#define  Tst_SyncBufLen  50
//INT8U Tst_SyncBuf[Tst_SyncBufLen] = {0};//该数组会在每个tst周期把CPUA的值给CPUB，如需使用该变量，使用Tst_F_ConfPonterAddSync
//INT8U Tst_SyncBufPos = 0;//Tst_SyncBuf 空闲内存的起始位置

/*调用函数指针，Tst_ConfPonterArgs ConfPonter的参数*/
INT8U Tst_F_ConfPonter(INT8U id, Tst_UDPConfArgs * args)
{
	if( id < Tst_ConfPonterLen &&Tst_ConfPonterID[id] != 0)  
	{
		Tst_ConfPonter[id](args);//目前只有两个，若更改Tst_ConfPonterArgs，该参数需更改
	}
	else
	{
		Tst_F_Report(DTXT_REPORT_WARNING, DTXT_C_LOC , "Tst_F_ConfPonter id mistake", 
						1u,id, 0u,0u,0u,0u,0u);
		
	}
} 

void Tst_F_UDPTask(); 
void Tst_F_FrameProcess();



OSW_T_pEVENT Tst_V_TaskInfo;
INT32U Tst_V_PointerQMsgTbl[0x20] = {0u};
/*Tst_ConfArgsType  INT32U 该函数会添加一个函数指针，并将Tst_SyncBuf的一段地址作为返回值*/
/*Tst_ConfArgsType* Tst_F_ConfPonterAddSync(INT8U index, void (*p)(Tst_ConfPonterArgs))
{
	Tst_ConfArgsType* SyncBuf;
	if(index < Tst_ConfPonterLen && Tst_ConfPonterID[index] == 0)
	{
		Tst_ConfPonter[index] = p;
		Tst_ConfPonterID[index] = 0x99;//非0表示被占用，值无意义
		
		SyncBuf = (Tst_ConfArgsType*)(Tst_SyncBuf + Tst_SyncBufPos);
		
		Tst_SyncBufPos += Tst_ConfArgsLen*sizeof(Tst_ConfArgsType);
		
		if(Tst_SyncBufPos > Tst_SyncBufLen)
		{
			GDF_M_FAULT_EXT(EVT_C_SAFETY_CORRUPTED,Tst_SyncBufPos, "");//err;
		}
	}
	else
	{
		GDF_M_FAULT_EXT(EVT_C_SAFETY_CORRUPTED,index, "");//err;
	}
	return SyncBuf;
}*/
/*该函数会添加一个函数指针，无返回值*/
void Tst_F_ConfPonterAdd(INT8U index,void (*p)(Tst_UDPConfArgs* args))
{
	if(index < Tst_ConfPonterLen && Tst_ConfPonterID[index] == 0)
	{
		Tst_ConfPonter[index] = p;
		Tst_ConfPonterID[index] = 0x99;
	}
	else
	{
		GDF_M_FAULT_EXT(EVT_C_SAFETY_CORRUPTED,index, "");//err;
	}
}
/*处理接收到，或将要发送的udp消息*/
void Tst_F_FrameProcess()
{
	ALSTOM_T_Status status = ALSTOM_C_ERROR;
	Tst_UDPFrame Frame;
	INT32U destAddr;
	INT8U max_process = 15;
	INT8U num = 0;
	status = Tst_F_FrameGet(&Tst_FrameBufSend, &Frame);
	
	while(status == ALSTOM_C_NO_ERROR) //发送
	{
		if(Frame.dstNodeID == ethNodeId)
		{
			destAddr = TstServerIp; //发往eth 路径的下一级，mvcu或eth
		}
		else
		{
			destAddr = Tst_F_canAddrGet(Frame.dstNodeID);//can
		}
		Frame.srcNodeID = localNodeID;
		Tst_F_UDPSend(destAddr,&Frame,"udp msg send");
		num ++;
		if(num >= max_process)
		{
			break;
		}
		status = Tst_F_FrameGet(&Tst_FrameBufSend, &Frame);
	}
	num =0;
	status = Tst_F_FrameGet(&Tst_FrameBufRecv,&Frame);
	#ifdef ALSTOM_CONFIG_CPU_A
		Tst_F_K2oo2CpuSend(&status, 1, 0x1f4);
	#else
		Tst_F_K2oo2CpuWait(&status, 1, 0x1f4);
	#endif
	if(status == ALSTOM_C_NO_ERROR) //处理接收消息
	{
		#ifdef ALSTOM_CONFIG_CPU_A
			Tst_F_K2oo2CpuSend(&TstServerIp, 4, 0x1f4);
		#else
			Tst_F_K2oo2CpuWait(&TstServerIp, 4, 0x1f4);
			if( ! Tst_F_isCanAddr(TstServerIp) )
			{
				TstServerIp = Tst_F_canAddrGet(localNodeID_A);
			}
		#endif
		Tst_F_K2oo2CpuExchange(&Frame, &Frame,Tst_F_UDPLenGet(10), 0x1f4); //最大VCU收到的消息最大为10
		
		switch(Frame.dataType&0x0F) //消息类型
		{
			case Tst_UdpDataType_tag & 0x0F:
				STD_F_Memcpy(&g_switchTemp, sizeof(Tst_TagType), Frame.data +1, sizeof(Tst_TagType));
			    GUEST_F_TST_UpdateTagTable(g_switchTemp,*(INT32U*)&Frame.data[5]); //???
				Tst_F_Report(DTXT_REPORT_INFO, DTXT_C_LOC , "tag recv", 1u, g_switchTemp,0u,0u,0u, 0u, 0u);
				break;
			case Tst_UdpDataType_conf & 0x0F:
				Tst_F_ConfPonter(Frame.data[0],(Tst_UDPConfArgs*)(&Frame.data[1]));
				Tst_F_Report(DTXT_REPORT_INFO, DTXT_C_LOC , "conf tst", 3u, Frame.data[0], *(INT32U*)&Frame.data[1], *(INT32U*)&Frame.data[5],0u, 0u, 0u);
				break;
			
			case Tst_UdpDataType_maint & 0x0F:
				Tst_F_OnMaint((Tst_UDPMaintFrame*)(Frame.data));
				Tst_F_Report(DTXT_REPORT_INFO, DTXT_C_LOC , "maint", 3u, Frame.data[0], Frame.data[1], 0u, 0u, 0u, 0u);
				break;
			default:
				break;
		}
		//num ++;
		//if(num >= max_process)
		//{
		//	break;
		//}
		//status = Tst_F_FrameGet(&Tst_FrameBufRecv,&Frame);
	}
}

void Tst_F_Creat()
{
	//tst_pFlagGrp = OSW_F_FlagCreate(0u, &tst_status);
	
	Tst_V_TaskInfo = OSW_F_QCreate((void**)&Tst_V_PointerQMsgTbl, 
								sizeof(Tst_V_PointerQMsgTbl) / sizeof(Tst_V_PointerQMsgTbl[0]));
#ifdef ReprogBreakOnDump
	Tst_Control_flags[ReprogflagOnDump] = TRUE;
#endif
	if(NULL == Tst_V_TaskInfo)
	{
		DTXT_F_Report(DTXT_REPORT_ERROR, DTXT_C_LOC , "tst", 0u, 0u,0u, 0u,0u, 0u, 0u);
	}
	Tst_F_TstSwitchInit();
	Tst_F_ConfPonterAdd(0,Tst_F_ReportFlagSet);
	Tst_F_ConfPonterAdd(1,Tst_F_ReportCycleSet);
	Tst_F_ConfPonterAdd(2,Tst_F_ControlFlagSet);
}



void Tst_F_Start()
{
	INT8S boardType[2] = {0};
	MSTEP_F_ConfigBoardPositionGet((INT8U* )boardType,(INT8U*)boardType+1);
	//boardType = CONF_F_GuessBoardType();
	TstServerIp = Tst_F_canAddrGet(0x79);//mvcu_a
	if (1u == *boardType) //机笼号
	{
		if(boardType[1] == 1) //槽道号
		{
			TstServerIp = 0x14020164u; //MVCU_P 默认IP 
#ifdef ALSTOM_CONFIG_CPU_B
			TstServerIp  = Tst_F_canAddrGet(0x02); // CPUB 通过CPUA 转发
#endif
			boardType[1] = -3;
		}
		else if(boardType[1] == 15)
		{
			TstServerIp = 0x14010164u;//MVCU_S 默认IP 
			boardType[1] = -2;
#ifdef ALSTOM_CONFIG_CPU_B
			TstServerIp  = Tst_F_canAddrGet(0x04);
#endif
		}
	}

	localNodeID_A = ( (boardType[0] -1)*16 + (boardType[1] +1 + 3 ))*2;
	localNodeID_B = localNodeID_A + 1;
	#ifdef ALSTOM_CONFIG_CPU_A
	localNodeID  = localNodeID_A;
	#endif
	
	#ifdef ALSTOM_CONFIG_CPU_B
	localNodeID  = localNodeID_B;
	
	#endif
	
    Tst_V_pUdpSocket = Lib_F_UDPOpen(Tst_UDP_PORT, Tst_V_TaskInfo, 1u);
	if (NULL== Tst_V_pUdpSocket)
	{
		//Tst_F_Report(DTXT_REPORT_WARNING, DTXT_C_LOC , "Tst: Can not open socket(port)", 1u, Tst_UDP_PORT, 0u,0u, 0u, 0u, 0u);
		Tst_F_PrintPrint("Tst: Can not open socket(port) %d\n", Tst_UDP_PORT);
		GDF_M_FAULT_EXT(0x12, Tst_UDP_PORT, "");
    }
	Lib_F_UDPFlush((const MSTEP_T_SocketID *)Tst_V_pUdpSocket);
    Tst_F_Report(DTXT_REPORT_INFO, DTXT_C_LOC , "Succeed to start Tst task.", 4u, TstServerIp,boardType[0], boardType[1], localNodeID_A , 0u, 0u);
	Tst_mode = Tst_C_FUNCTIONAL_NORMAL;
}
ALSTOM_T_Status Tst_F_Task(ALSTOM_E_FunctionalMode functionalMode)
{
	INT32U startTime=0;
	INT32U endTime=0;
	startTime = OSW_F_TickGet();

	tst_cycle++;
	g_testcycleNo++;
	
	if( Tst_C_FUNCTIONAL_NORMAL == Tst_mode )
	{
	       if(Tst_F_GetCycle()% 100 == 0)
	        {
	        	DTXT_REPORT(DTXT_REPORT_INFO,DTXT_ANSI_FG_BRIGHTRED"Abe-start");  
	       }
		Tst_F_ControlFlagTask(functionalMode);
		Tst_F_ReportProcess();
		Tst_F_UDPTask();
		Tst_F_FrameProcess();
		STD_F_Memset(&g_switchTemp, 0, sizeof(Tst_TagType));
		Tst_F_MaintTask(functionalMode);
	}
	else
	{
		;
	}
	
	endTime = OSW_F_TickGet();
	if (endTime-startTime >10)
		Tst_F_PrintReport("tst_task time:%d",endTime-startTime);
}
void Tst_F_UDPTask()
{
	INT16U srcPort = 0u;	
	INT16U datalength = Tst_UDP_MAX_DATA_LEN; 
	INT32U timeout = 0u;	
	INT32U srcAddr = 0u; 
	INT32U dstAddr = 0u; 
	static Tst_UDPFrame  TstBufferFrame;
	ALSTOM_T_Status status = ALSTOM_C_ERROR;
	static INT8U bufferUdp[Tst_UDP_MAX_DATA_LEN];
	INT8U recv_count = 0u;
	INT8U max_recv = 50u;
	status = ALSTOM_C_NO_ERROR;
	
	do{
		datalength = Tst_UDP_MAX_DATA_LEN; //MSTEP_F_UDPRecv 每次调用会修改length
		status = MSTEP_F_UDPRecv((const MSTEP_T_SocketID *)Tst_V_pUdpSocket, &srcAddr, &srcPort, bufferUdp, &datalength, 0u);//timeout 0
		if(status == ALSTOM_C_NO_ERROR) 
		{
			
			STD_F_Memcpy((void*)&TstBufferFrame, sizeof(Tst_UDPFrame),(void*) bufferUdp, sizeof(Tst_UDPFrame));
			if(TstBufferFrame.dstNodeID== localNodeID)
			{
				Tst_F_Report(4u, DTXT_C_LOC , "", 6u, srcAddr, TstBufferFrame.dstNodeID,TstBufferFrame.dataType,TstBufferFrame.data[0],TstBufferFrame.data[1],TstBufferFrame.data[2]);
				if(TstBufferFrame.srcNodeID == ethNodeId) //收到来自eth的消息
				{
					TstServerIp = srcAddr;
				}
				
				if( TstBufferFrame.dataLen>sizeof(Tst_UDPFrame))
				{
					Tst_F_PrintReport("recv udp msg length too long");
				}
				else
				{
					if( (TstBufferFrame.dataType&0xF0) == 0x10) //需要回复
					{
						//Tst_F_PrintReport("remote crc %x, local crc %x",*(INT16U*)((INT8U*)&TstBufferFrame + TstBufferFrame.dataLen -2 ),Tst_F_Crc16((const char *)&TstBufferFrame, TstBufferFrame.dataLen - 2));
						if(*(INT16U*)((INT8U*)&TstBufferFrame + TstBufferFrame.dataLen -2) == Tst_F_Crc16((const char *)&TstBufferFrame, TstBufferFrame.dataLen -2) ) //比较crc
						{
							TstBufferFrame.dataType |= 0x30;
							//TstBufferFrame.dataType +=1;
							TstBufferFrame.dstNodeID =  TstBufferFrame.srcNodeID;
							TstBufferFrame.srcNodeID =  localNodeID;
							//Tst_F_Report(4u, DTXT_C_LOC , "send ", 6u, srcAddr, TstBufferFrame.dstNodeID,TstBufferFrame.dataType,TstBufferFrame.data[0],TstBufferFrame.data[1],TstBufferFrame.data[2]);
							status = Tst_F_UDPSend( srcAddr,  &TstBufferFrame,"tst_reply");
							Tst_F_FrameStore(&Tst_FrameBufRecv,0u,&TstBufferFrame);

						}
						else
						{
							Tst_F_PrintReport("remote crc:%x,local crc:%x\n", *(INT16U*)((INT8U*)&TstBufferFrame + TstBufferFrame.dataLen -2),Tst_F_Crc16((const char *)&TstBufferFrame, TstBufferFrame.dataLen -2));	
						}
					}
					else
						Tst_F_FrameStore(&Tst_FrameBufRecv,0u,&TstBufferFrame);
				}
			}
			else
			{

				if( Tst_F_isCanAddr(srcAddr) )//不是eth ，即收到can 的消息
				{
					if(TstBufferFrame.dstNodeID == ethNodeId)
						status = Tst_F_UDPSend( TstServerIp,  &TstBufferFrame,"transmit msg to eth");
					else
					{
						Tst_F_Report(3u, DTXT_C_LOC , "recv from can buf not to eth ", 2u, srcAddr,TstBufferFrame.dstNodeID,0u,0u,0u,0u);//err
					}
				}
				else//来自以太网的消息，需转发
				{	
					
					//{
						
						TstServerIp = srcAddr;
						dstAddr = Tst_F_canAddrGet(TstBufferFrame.dstNodeID);
						Tst_F_Report(4u, DTXT_C_LOC , "send to ", 2u,dstAddr,0u,0u,0u,0u,0u);
						status = Tst_F_UDPSend(dstAddr, &TstBufferFrame,"transmit msg to can");
					/*}
					else
						{
						;
						}*/
				}
			}
			if (status == ALSTOM_C_NO_ERROR) 
			{
				;
			}
			else 
			{
				DTXT_F_Report(DTXT_REPORT_WARNING, DTXT_C_LOC , "tst_task: Failed to send Tst message(status)", 
						1u, (INT32U)status, 0u,0u,0u,0u,0u);
			}
		}
		else if(status == ALSTOM_C_TIMEOUT) 
		{
			break;
		}
		else
		{
			DTXT_F_Report(DTXT_REPORT_WARNING, DTXT_C_LOC , "tst_task: Failed to recv Tst message(status)", 
						1u, (INT32U)status, 0u,0u,0u,0u,0u);
			//GDF_M_FAULT_EXT(EVT_C_SAFETY_CORRUPTED, status, "");
			break;
		}
		
		recv_count++;
		if(recv_count >= max_recv)
		{
			break;
		}
		
	}while(1);
	
}
void Tst_F_ReportCycleSet(Tst_UDPConfArgs * args)
{
	INT32U val = args->val2;
	#ifdef ALSTOM_CONFIG_CPU_A
		val &= 0xFFFF;
	#else
		val >>= 16;
	#endif
	if(val == VALUE_DEFAULT_NONE)
		return;
	if(val >= 0 && val<100)
	{
		Tst_Cycle_UDPReport = val;
	}
	Tst_F_Report(4u, DTXT_C_LOC , "Tst_Cycle_Report ", 
					1u,Tst_Cycle_UDPReport,0u,0u,0u,0u,0u);
}
void Tst_F_ReportFlagSet(Tst_UDPConfArgs * args)
{
	INT32U val = args->val2;
	#ifdef ALSTOM_CONFIG_CPU_A
		val &= 0xFFFF;
	#else
		val >>= 16;
	#endif
	if(val == VALUE_DEFAULT_NONE)
		return;
				
	if(val == 0u|| val ==1u )
	{
		switch(args->val1)
		{
			case Report_Serial:
				Tst_Flag_SerialReport = val;
				break;
			case Report_Udp:
				Tst_Flag_UDPReport = val;
				break;
			case Report_Both:
				Tst_Flag_SerialReport = val;
				Tst_Flag_UDPReport = val;
				break;
			case Report_All:
				Tst_Flag_AllReport = val;
				Tst_Flag_UDPReport = val;
				break;
			case Tst_Task:
				if (val  == FALSE)
				{
					Tst_Flag_AllReport = val;
					Tst_Flag_UDPReport = val;
					Tst_mode = Tst_C_FUNCTIONAL_CLOSE;
				}
				break;
		}
	}
	Tst_F_Report(4u, DTXT_C_LOC , "Tst_Flag_SerialReport Tst_Flag_UDPReport", 
						2u, Tst_Flag_SerialReport, Tst_Flag_UDPReport,0u,0u,0u,0u);
}
void Tst_F_ControlFlagSet(Tst_UDPConfArgs * args)
{
	INT32U type = args->val1;
	INT32U val = args->val2;
#ifdef ALSTOM_CONFIG_CPU_A
		val &= 0xFFFF;
	#else
		val >>= 16;
#endif
	if(val == VALUE_DEFAULT_NONE)
		return;
	if( (val == 0u|| val ==1u) && type < 100 )
	{
		Tst_Control_flags[type] = val;
	}
}
void Tst_F_ControlFlagTask(ALSTOM_E_FunctionalMode functionalMode)
{
	if(Tst_Control_flags[Shutdown] == TRUE)
	{
		Tst_F_EntrySafetyMode();
	}
	if(Tst_Control_flags[TriggerFaultFifter] == TRUE)
	{
		Tst_F_TriggerFaultfifter();
	}
#ifdef EVT_C_SWITCH_TO_SDANDBY
	if(Tst_Control_flags[ToStandby] == TRUE )
	{
		if(functionalMode == ALSTOM_C_FUNCTIONAL_NORMAL)
			MSTEP_F_SwitchStandbyModeRequest(0x1FF);
		Tst_Control_flags[ToStandby] = FALSE;
	}
#endif
}
void Tst_F_OnMaint(Tst_UDPMaintFrame *Frame)
{
	INT8U val = Frame->req_ack;
#ifdef ALSTOM_CONFIG_CPU_A
		val &= 0x0F;
	#else
		val >>= 4;
#endif
	if(val == 0xF)
		return;
	
	if(val == 1)
	{
		if(Frame->msg[0] == 0 || Frame->msg[0] == 1)
			Tst_Maint_flags[Frame->maint_type] = Frame->msg[0];
	}
	Tst_F_Report(4u, DTXT_C_LOC , "Tst_F_OnMaint", 
							3u, val, Frame->msg[0],Frame->maint_type,0u,0u,0u);
}
void Tst_F_MaintTask(ALSTOM_E_FunctionalMode functionalMode)
{
//	if(Tst_Maint_flags[GetEventLog] == TRUE)
//	{
//		//Tst_F_SendEventLog();
//	}
}
