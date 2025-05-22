#include "GM_MQ.h"
#include "TB_RSSP1.h"
#include "FSFB_UDP_Init.h"
#include "GM_RSSP1_Syn.h"
#include "GM_RSSP1_MQ_Interface.h"
#include "GM_RSSP1_APP_Interface.h"

#pragma pack(push, 1)
typedef struct _Delay_Msg_
{
	GM_RSSP1_UINT8 data[MAX_RSSP1_MSG_LEN];
	GM_RSSP1_UINT16 datasize;
	GM_RSSP1_UINT32 loophour;
} DelayStruct;

typedef struct
{
	GM_RSSP1_UINT32 srcPid;
	GM_RSSP1_UINT32 dstPid;
} TPID;

#pragma pack(pop)

static GM_RSSP1_UINT32 g_loophour = 0U;
static GM_RSSP1_UINT8 g_syn_dat[480U] = {0U};
static GM_RSSP1_UINT16 g_syn_data_leng = 0U;
static GM_VLE_ABAS_Type_enum ABAS_Staus = ABAS_Type_A_AS;
GM_MQ_Struct delayMQ;
DelayStruct delayMsg;

char rxMsg[MAX_RSSP1_MSG_LEN];
char txMsg[MAX_RSSP1_MSG_LEN];
char rxMsg_p[MAX_RSSP1_MSG_LEN];

int faults[RSSP1_FAULT_NUM] = {0};
TPID tpid[MAX_RSSP1_CON_NUM] = {0};
GM_RSSP1_UINT32 Last_SN = 0;

/* 获取所有源和目的地址唯一的连接
 * 当向外设发送应用数据的时候
 * 每个唯一的连接只发送一次数据 */
GM_RSSP1_BOOL TB_RSSP1_TPID_Init()
{
	GM_RSSP1_UINT16 i = 0;
	GM_RSSP1_UINT16 j = 0;
	GM_RSSP1_SFM_object_struct *pSFM = GM_RSSP1_Get_SFM_Object();
	memset(tpid, 0, sizeof(TPID) * MAX_RSSP1_CON_NUM);
	for (i = 0 ; i < pSFM->connection_nums ; ++i)
	{
		tpid[i].srcPid = pSFM->source_addr;
		tpid[i].dstPid = pSFM->connection[i].dest_addr;
		for (j = 0; j < i; j++)
		{
			if (tpid[i].srcPid == tpid[j].srcPid && tpid[i].dstPid == tpid[j].dstPid)
			{
				tpid[i].srcPid = 0;
				tpid[i].dstPid = 0;
				break;
			}
		}
	}

	return GM_RSSP1_TRUE;
}


void TB_Log_Err(unsigned int errNo, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6)
{
	GM_RSSP1_Log_Msg(0,"Error Number = 0x%X,%d,%d,%d,%d,%d,%d,",errNo,arg1,arg2,arg3,arg4,arg5);
	return;
}

void TB_RSSP1_Close()
{
	isRunning = GM_RSSP1_FALSE;
}

GM_RSSP1_BOOL TB_RSSP1_Queue_Init(GM_MQ_Struct* rxMQ, GM_MQ_Struct* txMQ)
{
	GM_RSSP1_BOOL rt = GM_RSSP1_FALSE;

	rt = GM_MQ_Init(rxMQ, MAX_RSSP1_MSG_NUM, MAX_RSSP1_MSG_LEN);
	if (GM_RSSP1_FALSE == rt)
	{
		return GM_RSSP1_FALSE;
	}

	rt = GM_MQ_Init(txMQ, MAX_RSSP1_MSG_NUM, MAX_RSSP1_MSG_LEN);
	if (GM_RSSP1_FALSE == rt)
	{
		return GM_RSSP1_FALSE;
	}

	rt = GM_MQ_Init(&delayMQ, MAX_RSSP1_MSG_NUM, MAX_RSSP1_MSG_LEN);
	if (GM_RSSP1_FALSE == rt)
	{
		return GM_RSSP1_FALSE;
	}

	return GM_RSSP1_TRUE;
}

GM_RSSP1_UINT16 TB_Resource_Get_ABAS(void)
{
	return (GM_RSSP1_UINT16)ABAS_Staus;
}

void TB_Resource_Set_ABAS(int nr, int ab)
{
	if (nr)
	{
		if (ab)
		{
			ABAS_Staus = ABAS_Type_A_AS;
		}
		else
		{
			ABAS_Staus = ABAS_Type_B_AS;
		}
	}
	else
	{
		if (ab)
		{
			ABAS_Staus = ABAS_Type_A_SA;
		}
		else
		{
			ABAS_Staus = ABAS_Type_B_SA;
		}
	}
}

GM_RSSP1_BOOL TB_RSSP1_Clear_Msg_Queue(void)
{
	GM_RSSP1_BOOL rt = GM_RSSP1_FALSE;
	rt = GM_RSSP1_APP_Clear_Msg_Queue();
	return rt;
}

void TB_Indication_Recv_Test(GM_RSSP1_UINT8* msg)
{
	memset(msg, 0, TB_MAX_BYTES_COUNT);
	*(msg) = INTERFACE_TYPE_RSSP1;
	*(msg + 1) = INTERFACE_DAT_TYPE_RSSP1_DAT;
	*(GM_RSSP1_UINT16 *)(msg + 35) = 0x6c00;
	*(GM_RSSP1_UINT16 *)(msg + 37) = 0x6a00;
	*(GM_RSSP1_UINT16 *)(msg + 39) = 0x0102;
	*(GM_RSSP1_UINT16 *)(msg + 43) = 0x6400;
	*(msg + 45) = 0x55;
	*(msg + 144) = 0xAA;
}

void TB_Send_App_Data(GM_RSSP1_UINT8* data, GM_RSSP1_UINT16 len)
{
	int i = 0;
	GM_RSSP1_UINT16 src_addr = 0U;
	GM_RSSP1_UINT16 dest_addr = 0U;
	GM_RSSP1_UINT8 tx_dat[MAX_RSSP1_MSG_LEN] = {0U};
	GM_RSSP1_SFM_object_struct *pFSFB_SFM = GM_RSSP1_Get_SFM_Object();
	int nr = *(faults + RSSP1_FAULT_NUM - 2);
	int ab = *(faults + RSSP1_FAULT_NUM - 1);
	//TB_Resource_Set_ABAS(nr, ab);

	for (i = 0; i < pFSFB_SFM->connection_nums; i++)
	{
		if (0xFFFF == pFSFB_SFM->connection[i].dest_addr)
		{
			continue;
		}
		if (tpid[i].srcPid && tpid[i].dstPid)
		{
			src_addr = pFSFB_SFM->source_addr;
			dest_addr = pFSFB_SFM->connection[i].dest_addr;
			tx_dat[0] = ((src_addr & 0xff00) >> 8);
			tx_dat[1] = (src_addr & 0x00ff);
			tx_dat[2] = ((dest_addr & 0xff00) >> 8);
			tx_dat[3] = (dest_addr & 0x00ff);
			memcpy(tx_dat + 4, data, len);

			GM_RSSP1_Send_App_Dat(tx_dat , len + 4);
		}
	}
}

GM_RSSP1_BOOL TB_Proc_Main_App_Tx_RSSP1()
{
	int i = 0;
	GM_BOOL rt = GM_FALSE;
	GM_RSSP1_UINT32 ret = 0U;
	GM_RSSP1_UINT16 app_dat_len = 0U;
	GM_RSSP1_INT32 delay_msg_num = 0U;
	GM_RSSP1_UINT16 read_msg_size = 0U;
	GM_RSSP1_UINT8 app_dat[MAX_RSSP1_MSG_LEN] = {0U};

	g_loophour++;

	app_dat_len = MAX_RSSP1_MSG_LEN;	
	memset(app_dat, 0, MAX_RSSP1_MSG_LEN);
		
	/* 从界面读取要发送的码位状态 */
	SendMessage((HWND)g_hFormView, WM_REFRESH_SEND_BUFF, (WPARAM)0, 0);
	app_dat_len = g_sendMsg.count/8 + !!(g_sendMsg.count % 8);
	/*if (app_dat_len > 168)
		app_dat_len = 168;*/
	if (app_dat_len > 1000)
		app_dat_len = 1000;
	for (i = 0; i < g_sendMsg.count; i++)
	{
		if (g_sendMsg.b_TX_1[i])
		{
			//app_dat[i / 8] |= (0x01 << (7-(i%8)));
			app_dat[i / 8] |= (0x01 << (i%8));
		}

	}
	if(g_LongtimeRun == TRUE)
	{
	   GM_RSSP1_Log_Msg(-6,"<<<<<<<<<<<<<RSD: app_dat[0],app_dat[1],app_dat[2],app_dat[3]=0x%x,0x%x,0x%x,0x%x\n",app_dat[0],app_dat[1],app_dat[2],app_dat[3],0,0);
	}
	/*从延迟队列中查询是否有待发送消息*/
	delay_msg_num = GM_MQ_Get_Msg_Num(&delayMQ);
	if (delay_msg_num != -1)
	{
		for (GM_UINT8 index = 0; index < delay_msg_num; index++)
		{
			rt = GM_MQ_Read(&delayMQ,(GM_UINT8 *)&delayMsg, &read_msg_size);
			if (GM_FALSE == rt)
				break;
			/*消息需要发送则发送出去否则重新放回队列*/
			if (delayMsg.loophour <= g_loophour )
			{
				TB_Send_App_Data(delayMsg.data, delayMsg.datasize);
			}
			else
			{
				GM_MQ_Write(&delayMQ, (GM_UINT8*)&delayMsg, read_msg_size);
			}
		}
	}

	/* 如果有延迟故障，则写入延迟队列，否则直接发送 */
	if (*(faults + RSSP1_FAULT_DELAY) != 0)
	{
		GM_memset(&delayMsg, 0, sizeof(delayMsg));
		delayMsg.datasize = app_dat_len;
		delayMsg.loophour = g_loophour + *(faults + RSSP1_FAULT_DELAY);
		GM_memcpy(delayMsg.data, app_dat, app_dat_len);
		/*延迟队列增加消息*/
		GM_MQ_Write(&delayMQ,(GM_UINT8 *)&delayMsg,sizeof(DelayStruct));
	}
	else
	{
		TB_Send_App_Data(app_dat, app_dat_len);
	}

	return GM_RSSP1_TRUE;
}

GM_RSSP1_BOOL TB_Proc_Main_App_Rx_RSSP1()
{
	GM_RSSP1_INT8 ret = -1;
	GM_RSSP1_UINT32 addr = 0;
	GM_RSSP1_UINT32 datlen = 0;
	GM_RSSP1_INT32  count = -1;
	GM_RSSP1_UINT8  dat[MAX_RSSP1_MSG_LEN] = {0};
	GM_RSSP1_UINT32 SN = 0;

	memset(rxMsg, 0, MAX_RSSP1_MSG_LEN);

	do
	{
		GM_RSSP1_APP_Interface_Rcv_App_Dat(dat, &addr, &datlen, &count);
		if (0 == datlen || datlen > 65535)
		{
			SendMessage((HWND)g_hFormView, WM_RCVMSG, (WPARAM)rxMsg, 0);
			continue;
		}

		switch (*(dat + 1))
		{
		case INTERFACE_DAT_TYPE_RSSP1_DAT:
			memcpy(rxMsg, (dat+6), MAX_RSSP1_MSG_LEN-6);
			SendMessage((HWND)g_hFormView, WM_RCVMSG, (WPARAM)rxMsg, 0);
			memcpy(rxMsg_p, (dat+6), MAX_RSSP1_MSG_LEN-6);
			SN = (0XFFFFFF | (dat[9] << 24)) & (0XFF00FFFF | (dat[8] << 16)) & (0XFFFF00FF | (dat[7] << 8))& (0XFFFFFF00 | dat[6]) ;
			if(SN - Last_SN !=1)
			Last_SN = SN;
		/*for(int i= 0; i<(MAX_RSSP1_MSG_LEN-6);i++)
		{
	   rxMsg_p[i] = ((rxMsg_p[i] & 0x01) << 7) | ((rxMsg_p[i] & 0x02) << 5)| ((rxMsg_p[i] & 0x04) << 3)| ((rxMsg_p[i] & 0x08) << 1) | ((rxMsg_p[i] & 0x10) >> 1) | ((rxMsg_p[i] & 0x20) >> 3)| ((rxMsg_p[i] & 0x40) >> 5)| ((rxMsg_p[i] & 0x80) >> 7);
		}*/
			if(g_LongtimeRun == TRUE)
			{
			GM_RSSP1_Log_Msg(-6,"<<<<<<<<<<<<<RSD: data=0x%x,0x%x,0x%x,0x%x,0x%x,0x%x\n",rxMsg_p[0], rxMsg_p[1],rxMsg_p[2],dat[3],rxMsg_p[4],rxMsg_p[5]);
			}
			else
			{
			GM_RSSP1_Log_Msg(-6,"<<<<<<<<<<<<<RSD: data=0x%x,0x%x,0x%x,0x%x,0x%x,0x%x\n",rxMsg_p[0], rxMsg_p[1],rxMsg_p[2],dat[3],rxMsg_p[4],rxMsg_p[5]);
			}
			break;

		case INTERFACE_DAT_TYPE_RSSP1_COM_STATE:
			printf("recv conn status, len:%d\n", datlen, 0,0,0,0,0);
			break;
		}

	}while(count > 0);

	return GM_RSSP1_TRUE;
}


GM_RSSP1_BOOL TB_Proc_Main_SYN_Creat(void)
{
	GM_RSSP1_BOOL rt =GM_RSSP1_FALSE;
	GM_RSSP1_SFM_object_struct *pFSFB_SFM = NULL;
	GM_RSSP1_UINT8 dat[GM_MAX_QUEUE_ELE_LEN-50];   /* 预留，防止VLE端对SDM打包P1状态时，增加包头长度后超过GM_MAX_QUEUE_ELE_LEN */
	GM_RSSP1_UINT16 i = 0;
	GM_RSSP1_UINT16 source_addr = 0;
	GM_RSSP1_UINT16 dest_addr = 0;
	GM_RSSP1_INT16 dat_len = 0;
	GM_RSSP1_UINT8 Msg[TB_MAX_BYTES_COUNT] = {0};

	for (i=0;i<1;i++)
	{
		Msg[i] = 0xBE;
	}

	pFSFB_SFM = GM_RSSP1_Get_SFM_Object();

	for (i=0; i< pFSFB_SFM->connection_nums; i++)
	{
		source_addr = pFSFB_SFM->source_addr;
		dest_addr = pFSFB_SFM->connection[i].dest_addr;
		dat[0]   = ((source_addr & 0xff00) >> 8);
		dat[1]   = (source_addr & 0x00ff) ;
		dat[2]   = ((dest_addr & 0xff00) >> 8);
		dat[3]   = (dest_addr & 0x00ff) ;
		GM_RSSP1_memcpy(dat + 4 , Msg , 1);

		dat_len = 4 + 1;

		rt = GM_RSSP1_Send_App_Dat(dat , dat_len);
		if (rt == GM_RSSP1_FALSE)
		{
			printf("Create Msg error.\n");
			return GM_RSSP1_FALSE;
		}

		printf("Create Msg OK.\n");	
	}

	return GM_RSSP1_TRUE;
}

GM_RSSP1_BOOL TB_Proc_Main_App_Syn_RSSP1(GM_RSSP1_UINT32 cyc)
{
	if (TB_Resource_Get_ABAS() == ABAS_Type_A_AS)
	{
		GM_RSSP1_Syn_Send_Fsfb_State_To_Peer(GM_RSSP1_Local_Sys_A, GM_RSSP1_TRUE);
	}
	else if (TB_Resource_Get_ABAS() == ABAS_Type_B_AS)
	{
		GM_RSSP1_Syn_Send_Fsfb_State_To_Peer(GM_RSSP1_Local_Sys_B, GM_RSSP1_TRUE);
	}

	/*打包加头， 接收端可识别*/
	if(g_syn_data_leng != 0)
	{
		GM_RSSP1_Syn_Send_App_To_Peer(g_syn_dat,g_syn_data_leng);
		g_syn_dat[0] = 0;
	}

	return GM_RSSP1_TRUE;
}

void TB_RSSP1_Recv()
{
	GM_RSSP1_CFM_Interface_Proc_Recv();
	GM_RSSP1_SFM_Interface_Proc_Recv();

	TB_Proc_Main_App_Rx_RSSP1();
}

void TB_RSSP1_Send()
{
	TB_Proc_Main_App_Tx_RSSP1();

	GM_RSSP1_SFM_Interface_Proc_Send();
	GM_RSSP1_CFM_Interface_Proc_Send();
}

bool TB_RSSP1_Init(char* fileName)
{
	char name[256] = {0U};
	void* handle = NULL;
	GM_RSSP1_BOOL rt = GM_RSSP1_FALSE;
	sprintf(name, "Init file name: %s\n", fileName);
	GM_RSSP1_Log_Msg(0, name, 0, 0, 0, 0, 0, 0);
	GM_RSSP1_APP_Interface_VSN_Init();

	rt = GM_RSSP1_Log_Usr_Callback_Init(TB_Log_Err);
	if (GM_RSSP1_FALSE == rt)
	{
		GM_RSSP1_Log_Msg(0, "GM_RSSP1_Log_Usr_Callback_Init failed!\n",0,0,0,0,0,0);
		return false;
	}

	rt = FSFB_LNK_Init(fileName);
	if (GM_RSSP1_FALSE == rt)
	{
		GM_RSSP1_Log_Msg(0, "FSFB_LNK_Init failed!\n",0,0,0,0,0,0);
		return false;
	}

	rt = GM_RSSP1_APP_Interface_Init(TB_Resource_Get_ABAS, fileName, GM_RSSP1_TRUE, NULL);
	if (GM_RSSP1_FALSE == rt)
	{
		GM_RSSP1_Log_Msg(0, "GM_RSSP1_APP_Interface_Init failed!\n",0,0,0,0,0,0);
		return false;
	}

	rt = TB_RSSP1_TPID_Init();

	return true;
}