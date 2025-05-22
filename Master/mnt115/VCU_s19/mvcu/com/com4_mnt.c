/******************************************************************************
* COPYRIGHT (C) CASCO Signal 2015. VIOP Project. All rights reserved.
*****************************************************************************/

/**
* @file COM4_mnt.c
* @brief
* @author Song Kaixuan
* @date 2019-5-20 
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: fuliyu@casco.com.cn
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/

#include "com4_mnt.h"
#include "com.h"


/* ���MNT_moni*/
COM4_T_MNT_LOOPBUF    g_MNT_Buf={0} ;
COM4_T_MNT_TFTP_ServerRcvMsg  	g_MNT_MoniTftpMsg[COM4_C_MONI_RCVBUFFER_MAXNUM];  //moni_01����tftp

static ALSTOM_T_Status COM4_F_MNT_GetSndMsg(INT8U *buf, INT16U *msgLen);

ALSTOM_T_Status GetRackNumandSlotNumFromIpAddr ( INT32U ipAddr , INT8U*pRackNumAndSlotNum )
{
	ALSTOM_T_Status Status = ALSTOM_C_NO_ERROR;	
	INT8U slotId = 0u;
	INT8U mouduleId = 0u;
	INT8U nodeIdChA = 0u;
	INT8U rackId = 0u;

	GDF_M_NULL_ASSERT(pRackNumAndSlotNum);

	nodeIdChA = (INT8U)(ipAddr & 0x000000FFu);
	if(0!=(nodeIdChA%2))
	{
		nodeIdChA = nodeIdChA-1;
	}
	mouduleId = nodeIdChA/2;    //cpuA���㹫ʽ

	for (rackId=1; rackId<=3u; ++rackId)
	{	
		for(slotId=0u;slotId<=16u;++slotId )
		{
			if( (mouduleId ==( (rackId-1u)*16u +slotId+3u ) ) )   //�����Ų۵���������ɵ�moduleId��һһ��Ӧ�ģ�
			{
				*pRackNumAndSlotNum  = (INT8U)(( rackId<<4)|(slotId-1u ));    //�۵�����0-15��ʾ1-16		 
				// PRINT_F_Report(PRINT_REPORT_ERROR, PRINT_C_LOC , "mouduleId is", 1u, *pRackNumAndSlotNum, 0u, 0u, 0u, 0u, 0u);
				return Status;
			}
		}
	}
	Status = ALSTOM_C_ERROR;   //��moduleID�Ǵ����
	return Status;
}

//ע�⵽�ܰ�ͷ�����Ϣ���Ͳ�һ��
ALSTOM_T_Status AddTotalHeaderToMNTBuf (INT8U* buf , INT16U TotalMsgBodyLen, INT16U msgType)
{
	ALSTOM_T_Status Status = ALSTOM_C_NO_ERROR;
	INT8U currentSate = 0x00u;
	COM4_T_MNT_TotalHead *totalHead = NULL;
	totalHead = (COM4_T_MNT_TotalHead *)buf;

	GDF_M_NULL_ASSERT(buf);

	if(g_MNT_CurrentFunctionalMode >1u )   //����ģʽ������
	{
		currentSate = 0x03;
	}
	else
	{
		if(g_MNT_CurrentFunctionalMode==ALSTOM_C_FUNCTIONAL_NORMAL)
		{
			currentSate = 0x01;
		}
		else
		{
			currentSate = 0x02;
		}
	}

	totalHead->ProtocolVer = 0x01U;
	totalHead->MsgType = msgType;
	totalHead->Sate = currentSate;
	totalHead->Reserved = 0x00000000;
	totalHead->Size = TotalMsgBodyLen;

	return Status;
}

void COM4_F_MNT_Init(void) 
{
	INT32U i = 0u;
	
	STD_F_Memset((void*)&COM4_V_Connection, 0x0u, sizeof(COM4_V_Connection) ); 
	STD_F_Memset((void*)&g_MNT_MoniTftpMsg, 0x0u, sizeof(g_MNT_MoniTftpMsg) );  
	STD_F_Memset((void*)&g_MNT_Buf, 0x0u, sizeof(g_MNT_Buf) );

	
	for(i = 0u; i < COM4_C_MONI_RCVBUFFER_MAXNUM; i++)
	{
		g_MNT_MoniTftpMsg[i].bUsed = FALSE;	
	}
	

	return;
}

//mnt_01��ȡ���л�����
ALSTOM_T_Status COM4_F_MNT_TFTPGetFreeMoniTftpBuf(INT8U* index)
{	
	ALSTOM_T_Status Status = ALSTOM_C_ERROR; 
	GDF_M_NULL_ASSERT(index);


	for ( (*index) = 0u; (*index)  < COM4_C_MONI_RCVBUFFER_MAXNUM; (*index) ++)
	{
		if( (FALSE == g_MNT_MoniTftpMsg[(*index) ].bUsed) )
		{			
			g_MNT_MoniTftpMsg[(*index) ].bUsed = TRUE;
			g_MNT_MoniTftpMsg[(*index) ].moniRecBufferId=(*index) ;
			Status=ALSTOM_C_NO_ERROR;

			break;
		}
	}
	//(void) DTXT_F_Printf("\nCOM4_F_MNT_TFTPGetFreeMoniTftpBuf_index %d \n",(*index) );
	return Status;
}

//mnt_01RCVTFTP�ͷŻ�����
ALSTOM_T_Status COM4_F_MNT_TFTPReleaseMoniTftpBuf(INT32U index)
{
	ALSTOM_T_Status Status = ALSTOM_C_ERROR;  

	/*GDF_M_RANGE_ASSERT(index >= COM4_C_MONI_RCVBUFFER_MAXNUM, index);*/

	STD_F_Memset((void*)&(g_MNT_MoniTftpMsg[index]), 0x0u, sizeof(COM4_T_MNT_TFTP_ServerRcvMsg)); 

	Status = ALSTOM_C_NO_ERROR;
	return Status;
}

//mnt�������,���ظ�TFTP�������buf��ַ��װ���Ӱ�ñ��
ALSTOM_T_Status COM4_F_MNT_ServiceOpen(const CHAR *const connectionName,
									   INT32U remoteIpAddr,
									   MSTEP_E_TftpConnectionType type,
									   MSTEP_T_TftpServiceConnectionId *pId,
									   INT8U **pBuffer,
									   INT32U *pMaxSize)
{
	ALSTOM_T_Status Status = ALSTOM_C_NO_ERROR;   
	INT8U  i = 0;	

	GDF_M_NULL_ASSERT(connectionName);
	GDF_M_NULL_ASSERT(pId);
	GDF_M_NULL_ASSERT(pBuffer);
	GDF_M_NULL_ASSERT(pMaxSize);
	
	/*CR:3827,3.��mnt��Ϣ��tftp�ص������е�PID����ʼ����ֵ��������δ��ʼ����ֵ���ں˷������ֵ�����;*/
	*pId = COM4_C_MONI_RCVBUFFER_MAXNUM;
	    
	if ((g_MNT_CurrentFunctionalMode == ALSTOM_C_FUNCTIONAL_NORMAL)||(g_MNT_CurrentFunctionalMode == ALSTOM_C_FUNCTIONAL_STANDBY))
	{	
		/* �ļ����Ƚ�һ�� MNT_XXXX */
		if (0 == STD_F_Strncmp(&connectionName[0], COM4_C_MNT_SERVICE_NAME,
			(INT32S)(COM4_C_MNT_SUBSERVICE_NAME_OFFSET - 1)))
		{
			if (TFTP_C_CONNECTION_WRITE == type)
			{                                
				/*"mnt_01"*/
				if (0 == STD_F_Strncmp(&connectionName[COM4_C_MNT_SUBSERVICE_NAME_OFFSET],
					COM4_C_MNT_SUBSERVICE_MONI_NAME,
					(INT32S)STD_F_Strlen(COM4_C_MNT_SUBSERVICE_MONI_NAME)))
				{
					Status = COM4_F_MNT_TFTPGetFreeMoniTftpBuf(&i);
					if(Status == ALSTOM_C_NO_ERROR)
					{
						//song �����ߵĻ����۵���					 
						GetRackNumandSlotNumFromIpAddr(remoteIpAddr, &(g_MNT_MoniTftpMsg[i].moniSubBag.rackAndSlot));
						*pBuffer = g_MNT_MoniTftpMsg[i].buffer;
						*pMaxSize = COM4_C_MNT_TFTPRec_MAXSIZE;
						*pId = g_MNT_MoniTftpMsg[i].moniRecBufferId;
					}
					else
					{
						/*CR:3827,3.��mnt��Ϣ��tftp�ص������е�PID����ʼ����ֵ��������δ��ʼ����ֵ���ں˷������ֵ�����;*/
                        ; /* do nothing */
					}
				}           
				else
				{
					/*(void) DTXT_F_Printf("MNTOpen_SUBSERVICEname error \n");*/
					Status = ALSTOM_C_ERROR;
				}				
			}            
			else
			{
				/*(void) DTXT_F_Printf("MNTOpen_type error \n");*/
				Status = ALSTOM_C_ERROR;
			}
		}
		else
		{
			/*(void) DTXT_F_Printf("MNTOpen_SERVICENAME error \n");*/
			Status = ALSTOM_C_ERROR;
		}	
	}
	return Status;
}

//CR:VIOP00003491
//SONG ����size��ʾ�ͻ����յ�TFTP���ĵ�data�γ��ȣ�
//�ú��������κ�һ�����֮�䣨������ԭ�����䣩ִ�ж��п��ܱ�UDP�ϴ���ϣ�
void COM4_F_MNT_ServiceClose(MSTEP_T_TftpServiceConnectionId id, 
							 ALSTOM_T_Status status, INT32U size)
{	
	MSTEP_T_CanTimeOfDay time={0x0U};
	INT32U  DataLen =0 ; 	
	INT8U  errNO = 0;
	
	OSW_F_SemPend(g_Sem.MNT_mnt_01TftpBUF_EVENT, 20, &errNO, TRUE);
	if (id >= COM4_C_MONI_RCVBUFFER_MAXNUM)
	{
		return;
	}

	if ((g_MNT_CurrentFunctionalMode == ALSTOM_C_FUNCTIONAL_NORMAL)||(g_MNT_CurrentFunctionalMode == ALSTOM_C_FUNCTIONAL_STANDBY))
	{
		/*if (status == ALSTOM_C_ERROR)
		{
			DTXT_REPORT1(DTXT_REPORT_INFO, "Return error id", id);
		}*/

		if(status == ALSTOM_C_NO_ERROR)
		{
			//���ʱ��
			MSTEP_F_TimeGet(&time); 
			g_MNT_MoniTftpMsg[id].moniSubBag.TimeMs=(INT32U)time.ms; 	
			g_MNT_MoniTftpMsg[id].moniSubBag.TimeDay=(INT32U)time.day;    
			DataLen = (size + sizeof(COM4_T_MNT_SUB_BAG));  //song ÿ���Ӱ��������յ��ĳ��ȼ���ñ�ӳ���

			//(void) DTXT_F_Printf("A_Slot %x, Wt %d, Rt %d \n", g_MNT_MoniTftpMsg[id].moniSubBag.rackAndSlot, g_MNT_Buf.WT_Offset,  g_MNT_Buf.RD_Offset);//�����֧׮
			//(void) DTXT_F_Printf("MNTClose slot  %x \n",g_MNT_MoniTftpMsg[id].moniSubBag.rackAndSlot);
			/*Bufferβ��ʣ��Ŀռ���дһ�����ݣ�ÿ��TFTPǰ���������ֽ��Ǹð����ݵĴ�С*/
			if ( ( (g_MNT_Buf.WT_Offset)+DataLen+2u) <= (INT32U)MNT_LOOPARRAY_MAXSIZE )
			{
				/*���дָ����ǰ����ֱ��д����������ں���д��ð����ݺ�дָ��ҪС�ڶ�ָ�룬��������ˣ������������ص�*/
				if ( (g_MNT_Buf.WT_Offset == g_MNT_Buf.RD_Offset)||(g_MNT_Buf.WT_Offset > g_MNT_Buf.RD_Offset) ||(  (g_MNT_Buf.WT_Offset +DataLen+2u) <g_MNT_Buf.RD_Offset)   )
				{
					*(INT16U*)(g_MNT_Buf.LOOPARRAY+g_MNT_Buf.WT_Offset) = (INT16U)DataLen;			
					STD_F_Memcpy(g_MNT_Buf.LOOPARRAY+g_MNT_Buf.WT_Offset +2u , DataLen, &(g_MNT_MoniTftpMsg[id].moniSubBag), DataLen);
					g_MNT_Buf.WT_Offset = g_MNT_Buf.WT_Offset+ (INT16U) DataLen+2u;
					if( (INT32U)MNT_LOOPARRAY_MAXSIZE==g_MNT_Buf.WT_Offset)
					{
						g_MNT_Buf.WT_Offset = 0u;
					}
				}
				else
				{
					DTXT_REPORT4(DTXT_REPORT_WARNING, "MNT1 buf is full, Slot , WT_Offset , RD_Offset , RCVid ,", g_MNT_MoniTftpMsg[id].moniSubBag.rackAndSlot,g_MNT_Buf.WT_Offset,g_MNT_Buf.RD_Offset,g_MNT_MoniTftpMsg[id].buffer[0]);
				}
			}
			else
			{
				/*Bufferβ��ʣ��Ŀռ䲻��дһ�����ݣ����buffer����ʼλ�ÿ�ʼд��
				��RΪ0ʱ��ֱ��д�룬��R��Ϊ0ʱ��ֻ����R��ǰ��д�룬����R��Rָ������ݾͲ������ݰ��Ĵ�С�ˡ�*/
				if (((g_MNT_Buf.RD_Offset)==0u) ||(( (g_MNT_Buf.RD_Offset)!=0u) && ( (DataLen+2u)<(g_MNT_Buf.RD_Offset)) ) )
				{
					g_MNT_Buf.WT_Offset = 0u;
					*(INT16U*)(g_MNT_Buf.LOOPARRAY+g_MNT_Buf.WT_Offset) = (INT16U)DataLen;						
					STD_F_Memcpy(g_MNT_Buf.LOOPARRAY+g_MNT_Buf.WT_Offset+ 2u,  DataLen, &(g_MNT_MoniTftpMsg[id].moniSubBag), DataLen);
					g_MNT_Buf.WT_Offset = g_MNT_Buf.WT_Offset+ (INT16U) DataLen+2u;
				}
				else
				{
					DTXT_REPORT4(DTXT_REPORT_WARNING, "MNT2 buf is full, Slot , WT_Offset , RD_Offset , RCVid ,", g_MNT_MoniTftpMsg[id].moniSubBag.rackAndSlot,g_MNT_Buf.WT_Offset,g_MNT_Buf.RD_Offset,g_MNT_MoniTftpMsg[id].buffer[0]);
				}
			}            
		}
		else
		{
			DTXT_REPORT3(DTXT_REPORT_ERROR, "ServiceClose_MNTtftp connectionErr Slot, ErrorNum, Rcvsize: ", g_MNT_MoniTftpMsg[id].moniSubBag.rackAndSlot,status,size);
		}
	}
	
	COM4_F_MNT_TFTPReleaseMoniTftpBuf((INT32U)id);
	OSW_F_SemPost(g_Sem.MNT_mnt_01TftpBUF_EVENT);
}
//һ��ѭ������6���룬���ϴ�ӡ,
static ALSTOM_T_Status COM4_F_MNT_GetSndMsg(INT8U *buf, INT16U *msgLen)
{
	ALSTOM_T_Status Status = ALSTOM_C_ERROR;
	/*�����ܰ�ͷ�Ĵ洢�ռ�*/
	INT16U bufOffset = sizeof(COM4_T_MNT_TotalHead);
	INT16U MSGSize = 0u;
	INT16U  wtOffset = g_MNT_Buf.WT_Offset;

	GDF_M_NULL_ASSERT(buf);
	GDF_M_NULL_ASSERT(msgLen);

	do 
	{
		/*���buf������Ϣ�����ܶ�ȡ*/

		//(void) DTXT_F_Printf("GetSndMsg1_Wt %d, Rt %d bufOffset %d MSGSize %d msgSize %d	\n", g_MNT_Buf.WT_Offset,  g_MNT_Buf.RD_Offset ,   bufOffset  ,MSGSize,(*(INT16U*)(g_MNT_Buf.LOOPARRAY+g_MNT_Buf.RD_Offset)));//�����֧׮
		if ((g_MNT_Buf.RD_Offset)!=wtOffset)   
		{
			MSGSize = *(INT16U*)(g_MNT_Buf.LOOPARRAY+g_MNT_Buf.RD_Offset);
			/*bufʣ��Ŀռ���д��һ������*/
			if ((bufOffset+MSGSize)<=COM4_C_MNT_UDPSend_MAXSIZE)
			{
				/*�����ݿ��������緢�͵�buf*/
				STD_F_Memcpy(buf+bufOffset,MSGSize,(g_MNT_Buf.LOOPARRAY+g_MNT_Buf.RD_Offset+2u),MSGSize);
				/*���м�buf�Ĵ洢�����*/
				STD_F_Memset(g_MNT_Buf.LOOPARRAY+g_MNT_Buf.RD_Offset, 0x00u,MSGSize+2u);
				bufOffset = bufOffset+MSGSize;
				g_MNT_Buf.RD_Offset = g_MNT_Buf.RD_Offset+MSGSize+2u;

				//(void) DTXT_F_Printf("GetSndMsg2_Wt %d, Rt %d bufOffset %d MSGSize %d msgSize %d  \n", g_MNT_Buf.WT_Offset,  g_MNT_Buf.RD_Offset ,   bufOffset  ,MSGSize,(*(INT16U*)(g_MNT_Buf.LOOPARRAY+g_MNT_Buf.RD_Offset)));//�����֧׮
				/*�°����ݴ�СΪ0����R/D���غϣ�˵��ʣ��Ŀռ䲻����дһ�����ݣ�RҪ�Ƶ���ʼ����
				����R���ڵ�λ�ó�����LOOPARRAY�����ֵ��RҲҪ�Ƶ���ʼ����*/
				if (((0u==(*(INT16U*)(g_MNT_Buf.LOOPARRAY+g_MNT_Buf.RD_Offset)))&&((g_MNT_Buf.RD_Offset)!=wtOffset))||(g_MNT_Buf.RD_Offset >=MNT_LOOPARRAY_MAXSIZE))
				{
					g_MNT_Buf.RD_Offset = 0u;
				}
				//(void) DTXT_F_Printf("GetSndMsg3_Wt %d, Rt %d bufOffset %d MSGSize %d msgSize %d  \n", g_MNT_Buf.WT_Offset,  g_MNT_Buf.RD_Offset ,   bufOffset  ,MSGSize,(*(INT16U*)(g_MNT_Buf.LOOPARRAY+g_MNT_Buf.RD_Offset)));//�����֧׮
			}
			else
			{
				/*���緢�͵�buf��д��������*/
				Status = ALSTOM_C_NO_ERROR;
				break;
			}
		}
		else
		{
			/*�м�buf�Ķ�дָ����ȣ�û�пɶ�������*/
			Status = ALSTOM_C_ERROR;
			break;
		}	

		// (void) DTXT_F_Printf("GetSndMsg4_Wt %d, Rt %d bufOffset %d MSGSize %d msgSize %d	\n", g_MNT_Buf.WT_Offset,  g_MNT_Buf.RD_Offset ,   bufOffset  ,MSGSize,(*(INT16U*)(g_MNT_Buf.LOOPARRAY+g_MNT_Buf.RD_Offset)));//�����֧׮
	} while (1);
	*msgLen = bufOffset-sizeof(COM4_T_MNT_TotalHead);	


	//(void) DTXT_F_Printf("GetSndMsg_Wt %d, Rt %d \n", g_MNT_Buf.WT_Offset,  g_MNT_Buf.RD_Offset);
	return Status;
}

//��ʱ�ϴ�ftp_moni�ܱ��Ĳ�ⷢ��
ALSTOM_T_Status COM4_F_MNT_UploadMoniByUdp(MSTEP_T_SocketID* pSocket, INT32U destAddr,INT16U destPort)
{
	ALSTOM_T_Status Status = ALSTOM_C_ERROR;
	ALSTOM_T_Status SNDStatus = ALSTOM_C_NO_ERROR;
	static INT8U  buf[COM4_C_MNT_UDPSend_MAXSIZE];
	INT16U msgLen = 0u;	
#ifdef ALSTOM_CONFIG_CPU_A  
	static CHAR defaultContext[MSTEPINC_C_FAILURE_CONTEXT_SIZE];
#endif	
	INT8U  errNO = 0;

	GDF_M_NULL_ASSERT(pSocket);
	OSW_F_SemPend(g_Sem.MNT_mnt_01TftpBUF_EVENT, 20, &errNO, TRUE); 
	do 
	{
		msgLen = 0u;
		STD_F_Memset(buf, 0x00u,COM4_C_MNT_UDPSend_MAXSIZE);

		//oid) DTXT_F_Printf("UploadMoniByUdp_Wt %d, Rt %d \n", g_MNT_Buf.WT_Offset,g_MNT_Buf.RD_Offset);  //��׮
		Status = COM4_F_MNT_GetSndMsg(buf, &msgLen);
		if (msgLen>0)
		{
			AddTotalHeaderToMNTBuf(buf,msgLen, (INT16U)COM4_C_MONI_MSGTYPE );
			
			SNDStatus = MSTEP_F_UDPSend(pSocket,destAddr,destPort,buf,msgLen+sizeof(COM4_T_MNT_TotalHead));
			if(ALSTOM_C_NO_ERROR !=SNDStatus)	//0820�޸�
			{
				DTXT_REPORT1(DTXT_REPORT_ERROR, "MNT_MONI upload  ErrNum: ",SNDStatus);
				/* дMIB���� */
                ;
			}
			else
			{
				/* ��FailureCode */
                ;
			}			
		}
		else
		{
			break;
		}
	} while (ALSTOM_C_NO_ERROR==Status);
	OSW_F_SemPost(g_Sem.MNT_mnt_01TftpBUF_EVENT);
	return SNDStatus;
}

//UDP���������
ALSTOM_T_Status COM4_F_MNT_UdpReqRcv(MSTEP_T_SocketID* pSocket)
{
	static COM4_T_MNT_HB_DATA HBRcvMsg;
	INT16U size = sizeof(COM4_T_MNT_HB_DATA);   //д����󳤶ȴ���ӿڣ��ӿڷ����������յ�����;����Ǿ�̬�ᷢ����UDP�ӿڸ������
	INT16U srcPort = 0u;
	INT32U srcAddr = 0u;
	ALSTOM_T_Status rx_status = ALSTOM_C_NO_ERROR;
	ALSTOM_T_Status tx_status = ALSTOM_C_NO_ERROR;	


	GDF_M_NULL_ASSERT(pSocket);

	rx_status = MSTEP_F_UDPRecv(pSocket,&srcAddr, &srcPort,(INT8U*)&HBRcvMsg,&size, 0u); 	
	//ֻ��UdpRcv��������ֵΪ0���ű�ʾ���յ������ݶ������ݴ�С�����ڴ����size
	if(ALSTOM_C_NO_ERROR == rx_status)
	{
		if ( (HBRcvMsg.Head.MsgType == (INT16U)COM4_C_HB_MSGTYPE) &&(size ==sizeof(COM4_T_MNT_HB_DATA) ) )  //��Ϣ�����ֶμ������б�
		{			  
			COM4_V_Connection.destAddr = srcAddr;
			COM4_V_Connection.destPort = srcPort;
			COM4_V_Connection.isConnect = TRUE;
		}
		else
		{
			/*(void) DTXT_F_Printf("MNT_HB messge type or length error \n");*/
		}
	}
	else//�ο�GM_TFTP_F_RecvMessage
	{		
	    ;
	}
	STD_F_Memset((INT8U*)&HBRcvMsg,0x0u,sizeof(HBRcvMsg));
	return tx_status;
}


