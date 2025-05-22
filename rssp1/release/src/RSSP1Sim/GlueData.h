#pragma once

#ifndef __cplusplus

#ifndef WM_USER
#define WM_USER 0x0400
#endif

//#ifndef HWND
//typedef void* HWND;
//#endif

#ifndef BYTE
typedef unsigned char BYTE;
#endif

#ifndef UINT
typedef unsigned int UINT;
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef BOOL
#define BOOL int
#endif

#endif

#define TB_INTERFACE_DATA_MAX (2048)
#define TB_MAX_BYTES_COUNT (1024)

#define MAX_RSSP1_CON_NUM             (20)
#define MAX_RSSP1_MSG_NUM		      (2048)
#define MAX_RSSP1_MSG_LEN		      (1400)
#define FSFB_Q_TYPE_APP_RECV          (48)
#define FSFB_Q_TYPE_APP_SEND          (50)
#define CYCLE_500                     (500)

enum
{
	WM_INITRSSP1			= (WM_USER + 9),
	WM_RCVMSG				= (WM_USER + 10),
	WM_REFRESH_SEND_BUFF	= (WM_USER + 11),
};

enum
{
	FSFB2_VAR_COUNT_MAX = 8000
};

#ifdef __cplusplus
extern "C"
{
#endif

#include <PshPack1.h>

	typedef struct 
	{
		UINT SHB_NETRX_1[FSFB2_VAR_COUNT_MAX];  /**< 在chan1：用于储存一个FSFB2通道的接收的冗余码布尔量*/
		UINT SHB_NETRX_2[FSFB2_VAR_COUNT_MAX];  /**< 在chan2：用于储存一个FSFB2通道的接收的冗余码布尔量*/
	} RCV_MSG;

	typedef struct 
	{
		int count;
		BOOL b_TX_1[FSFB2_VAR_COUNT_MAX];
		BOOL b_TX_2[FSFB2_VAR_COUNT_MAX];
	} SND_MSG;

	typedef struct 
	{
		int m_nCrc1Offset;
		int m_nCrc2Offset;
		int m_nLenOffset;
		int m_nSNOffset;
		int m_nSrcIDOffset;
		int m_nDestIDOffset;
		int m_nDelayBSDOffset;
		int m_nDelaySSEOffset;
		int m_nDelaySSROffset;
		int m_nSYSCKW;
		int m_nCrc16Offset;
		/*int m_nFSFB2CKW;*/
		int m_RMSMsgsize;
		int m_RMSMsgversion;
		int m_RMSMsgsrcSS;
		int m_RMSMsgdestSS;
		int m_RMSMsgSN;
		int m_Cycle_Error_No;
		BOOL m_start;
	} BUG_INSERT;

	typedef struct  
	{
		int r1;
		int r2;
	}RELAY_XOR;

	typedef struct 
	{
		int size;
		int arr[FSFB2_VAR_COUNT_MAX];
	}RELAY_ADD;

#include <PopPack.h>

extern UINT g_nNumcdw;
extern UINT g_nNumcdwRecv;
extern UINT g_nNumcdwSend;

extern void* g_hFormView;

extern SND_MSG		g_sendMsg;
extern UINT m_SendLength;
extern BUG_INSERT	g_bugInsert;
extern BOOL	g_LongtimeRun;

#ifdef __cplusplus
};
#endif
