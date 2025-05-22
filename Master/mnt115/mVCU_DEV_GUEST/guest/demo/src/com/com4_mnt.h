/******************************************************************************
* COPYRIGHT (C) CASCO Signal 2015. VIOP Project. All rights reserved.
*****************************************************************************/

/**
* @file COM4_public.h
* @brief
* @author 62058
* @date 2013-5-22
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: fuliyu@casco.com.cn
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/

#ifndef __COM4_MNT_H_
#define __COM4_MNT_H_

#include "com.h"

#define COM4_C_MNT_UDPSend_MAXSIZE                    566u  //传入UDP的数据大小限制；由传入IP层数据大小限制574减8
//#define EIOCOM4
#define MNT_LOOPARRAY_MAXSIZE  	(COM4_C_MONI_RCVBUFFER_MAXNUM*COM4_C_MNT_UDPSend_MAXSIZE)			/* 566*70 equal 39620	*/
#define COM4_C_INTERFACE_VER                      0x01    //接口版本号
#define COM4_C_HB_MSGTYPE                     0xFFF0    //hb消息类型
#define COM4_C_IOVAR_MSGTYPE                     0x0003    //IOVAR消息类型
#define COM4_C_MONI_MSGTYPE                     0x0001    //MONI消息类型
#define COM4_C_MONI_RCVBUFFER_MAXNUM                      70u                 /*moni_01并行缓冲区数量*/
#define COM4_C_MONI_BUF_MAXNUM                      4u                 /*moni_01总buf并行缓冲区数量，解决UDP上传时，被TFTP接收打断后的缓冲区等地址被重用*/
#define COM4_C_MNT_TFTPRec_MAXSIZE        (COM4_C_MNT_UDPSend_MAXSIZE-(sizeof(COM4_T_MNT_TotalHead)+sizeof(COM4_T_MNT_SUB_BAG)) )  //tftp接收长度限制,
#define COM4_C_MNT_BUF_MAXSIZE        ((COM4_C_MNT_TFTPRec_MAXSIZE)*100u)  //MNT结构体中转BUF最大,暂定？？
#define COM4_C_MNT_BACKBOARD_GID_ADDR                       (126u * 0x10000)        /* 背板key身份ID地址 */
#define MNT_IOVAR_TIMEOUT_TIME                     30000u   //输入输出码位定时30s上传
#define COM4_C_MSEC_OF_ONE_HOUR                       ((60u * 60u) * 1000ul)  /* msec of 1 hour -- ms */
/** TFT service Name */
#define COM4_C_MNT_SERVICE_NAME                  "mnt"
/*TFTP Failure Code*/
#define COM4_C_MNT_SUBSERVICE_MONI_NAME      "01"
/** TFT Sub Service Name : dmo_subservicename*/
#define COM4_C_MNT_SUBSERVICE_NAME_OFFSET        4u

typedef struct
{
	INT32U BackboardGidA;
	INT32U BackboardGidB;
} COM4_T_BACK_BOARD_GID;

#pragma pack(1)
typedef struct 
{	
	INT32U TimeMs;
	INT32U TimeDay;
	INT8U rackAndSlot;
} COM4_T_MNT_SUB_BAG;    //子包帽子
#pragma pack()

#pragma pack(1)
typedef struct 
{
	/*协议版本号. */
	INT8U						ProtocolVer;
	/*消息类型 */
	INT16U 						MsgType;
	/* COM4板卡的主备标识*/
	INT8U 	                    Sate;
	//保留字段
	INT32U 	                    Reserved;	
	/*消息体的长度 */
	INT16U 						Size;
}COM4_T_MNT_TotalHead;
#pragma pack()

#pragma pack(1)
typedef struct 
{
	COM4_T_MNT_TotalHead Head;
	INT8U HBsn;	//ECID 与 SDM 的心跳序号
	INT32U ReservedSeg;	
} COM4_T_MNT_HB_DATA;    //心跳回应
#pragma pack()

#pragma pack(1)
typedef struct 
{	
	BOOLEAN bUsed;	//应该是TFTP一个连接在用,z则TFTP另一个连接不能用
	INT32U  moniRecBufferId;//用于TFTP的pid指向	
	COM4_T_MNT_SUB_BAG moniSubBag;  //要针对每条TFTp定制
	INT8U buffer[COM4_C_MNT_TFTPRec_MAXSIZE];  //中转存放戴帽子的TFTP报文
} COM4_T_MNT_TFTP_ServerRcvMsg;//TFTP服务端接收buffer
#pragma pack()


#pragma pack(1)
typedef struct 
{
	INT16U	 RD_Offset;
	INT16U 	WT_Offset;
	INT8U  LOOPARRAY[MNT_LOOPARRAY_MAXSIZE];   	//
} COM4_T_MNT_LOOPBUF;
#pragma pack()

/** Definition of the run mode of the COM4 board */
typedef enum
{
	COM4_C_STATE_UNDEFINED      = 0u,             /**<Undefined state          */
	COM4_C_STATE_INITIAL        = 1u,             /**<Initial state     */
	COM4_C_STATE_PREOPERATIONAL = 2u,             /**<Pre-Operational state     */
	COM4_C_STATE_OPERATIONAL    = 3u,             /**<Operational state     */
	COM4_C_STATE_INITFAILED     = 4u,             /**<Init Failed state     */
	COM4_C_STATE_PS_ISOLATION   = 5u,             /**<24V MONIor Set Isolation state       */
	COM4_C_STATE_OPT_ISOLATION  = 6u,             /**<Output Port Test MONIor Set Isolation state       */
	COM4_C_STATE_RST_ISOLATION  = 7u,             /**<Restrictive Set Isolation state       */
	COM4_C_STATE_SAFETY         = 8u             /**<Safety state          */

} GUST_E_RUN_STATE;

void COM4_F_MNT_Init(void);
ALSTOM_T_Status COM4_F_MNT_CompareBackAndBottomBoardGID(void);
ALSTOM_T_Status COM4_F_MNT_K2oo2CompareGIDConsistency(void);
ALSTOM_T_Status COM4_F_MNT_CheckBackAndBottomBoardGID(void);
ALSTOM_T_Status GetRackNumandSlotNumFromIpAddr ( INT32U ipAddr , INT8U*pRackNumAndSlotNum );
ALSTOM_T_Status AddTotalHeaderToMNTBuf (INT8U* buf , INT16U TotalMsgBodyLen, INT16U msgType);
ALSTOM_T_Status COM4_F_MNT_TFTPGetFreeMoniTftpBuf(INT8U* index);
ALSTOM_T_Status COM4_F_MNT_TFTPReleaseMoniTftpBuf(INT32U index);
ALSTOM_T_Status COM4_F_MNT_UdpReqRcv(MSTEP_T_SocketID* pSocket);
ALSTOM_T_Status COM4_F_MNT_ServiceOpen(const CHAR *const connectionName,
									   INT32U remoteIpAddr,
									   MSTEP_E_TftpConnectionType type,
									   MSTEP_T_TftpServiceConnectionId *pId,
									   INT8U **pBuffer,
									   INT32U *pMaxSize);
void COM4_F_MNT_ServiceClose(MSTEP_T_TftpServiceConnectionId id,
							 ALSTOM_T_Status status, INT32U size);
ALSTOM_T_Status COM4_F_MNT_UploadMoniByUdp(MSTEP_T_SocketID* pSocket, INT32U destAddr,INT16U destPort);

#endif 
/* EOF */
