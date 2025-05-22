#include "stdafx.h"
#include "GlueData.h"
#include "RSSP1_Control.h"

#include "../base/LogMsg.h"
#include "../RSSP1/GM_Utils/GM_Condition.h"
#include "../RSSP1/GM_Define.h"
#include "../RSSP1/GM_Utils/GM_Mutex.h"
#include "../RSSP1/GM_Utils/GM_MQ.h"


#include "TB_RSSP1.h"

#include <MMSystem.h>
#pragma comment(lib, "Winmm.lib")

static GM_MUTEX mutex    = 0;
static GM_CONDITION cond = 0;
static GM_BOOL RSSP1_Initialized = GM_FALSE;
static MMRESULT RSSP1_Timer_ID = 0; 

void AddMsgOutputWndPack(const char* pszMsg)
{
	char tmp[200] = {0};
	sprintf(tmp, "%s", pszMsg);
	LOG((tmp));
}

void FSFB2_Sim_Main_Signal(void)
{
	if (GM_TRUE == RSSP1_Initialized)
	{
		GM_Condition_Signal(cond);
	}

	return;
}

void CALLBACK FSFB2_Sim_Timer_Fun(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	FSFB2_Sim_Main_Signal();
}

GM_BOOL RSSP1_Sim_Timer_Init(GM_UINT32 cyc)
{
	RSSP1_Timer_ID = timeSetEvent(cyc, 1, (LPTIMECALLBACK)FSFB2_Sim_Timer_Fun, 1, TIME_PERIODIC);

	if (0 == RSSP1_Timer_ID)
	{
		return GM_FALSE;
	}

	return GM_TRUE;
}

void TB_RSSP1_Get_Cfg_Path(char* cfgPath)
{
	char strSec[256] = {0};
	char strKey[256] = {0}; 
	char strVal[256] = {0};
	sprintf(strSec, "%s", "RSSP1_GLOBAL");
	sprintf(strKey, "%s", "main_cycle");
	int n = GetModuleFileName(0, cfgPath, 1024);
	*::strrchr(cfgPath, '\\') = 0;
	strcat(cfgPath, "\\fsfb.ini");
}

GM_UINT32 TB_RSSP1_Get_Main_Cycle()
{
	char strSec[256] = {0};
	char strKey[256] = {0}; 
	char strVal[256] = {0};
	char strPath[1024] = {0};
	TB_RSSP1_Get_Cfg_Path(strPath);
	sprintf(strSec, "%s", "RSSP1_GLOBAL");
	sprintf(strKey, "%s", "main_cycle");
	GetPrivateProfileString(strSec, strKey, "500", strVal, 256, strPath);

	return strtoul(strVal, NULL, 0);
}

UINT RSSP1_Sim_Run(LPVOID /*p*/)
{
	GM_BOOL rt = GM_FALSE;
	GM_UINT32 cycle = 0;
	char strCyc[32] = {0};
	char strPath[1024] = {0};
	
	rt = GM_Mutex_Init(&mutex);
	if (GM_FALSE == rt)
	{
		AddMsgOutputWndPack("GM_Mutex_Init error!");
		return GM_FALSE;
	}

	rt = GM_Condition_Init(&cond);
	if (GM_FALSE == rt)
	{
		AddMsgOutputWndPack("GM_Cond_Init error!");
		return GM_FALSE;
	}

	cycle = TB_RSSP1_Get_Main_Cycle();
	sprintf(strCyc, "main cycle:%d", cycle);
	AddMsgOutputWndPack(strCyc);

	rt = RSSP1_Sim_Timer_Init(cycle);
	if (GM_FALSE == rt)
	{
		AddMsgOutputWndPack("RSSP1_Timer_Init error!");
		return GM_FALSE;
	}

	TB_RSSP1_Get_Cfg_Path(strPath);
	rt = TB_RSSP1_Init(strPath);
	if (GM_FALSE == rt)
	{
		AddMsgOutputWndPack("RSSP1_Init error!");
		return GM_FALSE;
	}

	RSSP1_Initialized = GM_TRUE;
	SendMessage((HWND)g_hFormView, WM_INITRSSP1, 0, 0);

	AddMsgOutputWndPack("RSSP1 Init OK.");

	while(1)
	{
		GM_Mutex_Lock(&mutex);
		GM_Condition_Wait(cond, mutex);

		TB_RSSP1_Recv();

		TB_RSSP1_Send();
		
		GM_Mutex_Unlock(&mutex);
	}
	return 0;
}

void InvokeRSSP1SimTask()
{
	VERIFY(AfxBeginThread(RSSP1_Sim_Run, NULL));
}
