#include "tstswitch.h"

fault_insert_item g_switchTable[MAX_OPEN_STUB];
//fault_insert_item g_SyncTagBuf[MAX_OPEN_STUB];
/*fault_insert_item g_SingleTagBuf[MAX_OPEN_STUB];*/
Tst_TagType g_switchTemp = 0;
INT32U	l_OpenedStubIdx = 0;
INT32U	l_SyncTagBufIdx = 0;
/*INT32U	l_SingleTagBufIdx = 0;*/
BOOLEAN tst_Bol = FALSE;
INT32U	g_tst_flag = 0;
INT32U	g_RIT_flag = 0;
//
typedef struct{
	const char* name;
	INT32U openCount;
}StringTag;
StringTag StringTagTable[MAX_OPEN_STUB];
INT32U l_OpenedStringIdx = 0;
#define Tst_F_FindStringTable(name) Tst_F_FindStringTableWithName(#name)

static char thisTagName[50] = {0};
//

void GUEST_F_TST_SetTagOpen( TAG_NAME tagName, INT32U );
void GUEST_F_TST_SetTagClose( TAG_NAME tagName, INT32U );
void GUEST_F_TST_DeleteTag( fault_insert_item* pTagBuf, INT32U* idx, TAG_NAME tagName );

void Tst_F_TstSwitchInit()
{
	int i;
	for (i = 0; i < MAX_OPEN_STUB; i++)
	{
		StringTagTable[i].name = NULL;
		g_switchTable[i].m_CloseCycle = -1;
	}
}
INT32S Tst_F_FindTagTable(TAG_NAME tagName)
{
	int i = -1;
	for (i = 0; i < l_OpenedStubIdx; i++)
	{
		if ( tagName == g_switchTable[i].m_tagName)
		{
			return i;
		}
	}
	return -1;
}
INT32S Tst_F_FindStringTableWithName(const char* name)
{
	int i = -1;
	for (i = 0; i < l_OpenedStringIdx; i++)
	{
		if (name  == StringTagTable[i].name )
		{
			return i;
		}
	}
	return -1;
}
void Tst_F_OnOpen(const char* name)
{
	int i = 0;
	for(i = 0;name[i] != 0;i++)
	{
		thisTagName[i] = name[i];
		if(i == sizeof(thisTagName) - 1)
		{
			break;
		}
	}
	thisTagName[i] = 0;
	i = Tst_F_FindStringTableWithName(name);
	if(i < 0)
	{
		if(l_OpenedStringIdx < MAX_OPEN_STUB)
		{
			StringTagTable[l_OpenedStringIdx].name = name;
			StringTagTable[l_OpenedStringIdx].openCount = 0;
			l_OpenedStringIdx ++;
		}
		else
			Tst_F_PrintReport("Tag open too much %d\n",l_OpenedStringIdx);
	}
	else
	{
		StringTagTable[i].openCount ++;
	}
}
INT32U Tst_F_GetOpenCountWithName(const char* name)
{
	int i = 0 ;
	i = Tst_F_FindStringTableWithName(name);
	if(i < 0)
	{
		return -1;
	}
	else
	{
		return StringTagTable[i].openCount;
	}
}
#ifndef TST_SWITCH_DEBUG
TB_BOOL IsTagOpen(TAG_NAME tagName)
{
	int i;
	if( tagName < 0x100)
	{
		Tst_F_PrintReport("Tag Value Err :0x%x\n",tagName);
		return FALSE;
	}
	i = Tst_F_FindTagTable(tagName);
	if(i >= 0)
	{
		if((g_switchTable[i].m_bOpen && (g_switchTable[i].m_OpenCycle <= Tst_F_GetCycle())))
		{
			//Tst_F_OnOpen(name);
			return TRUE;
		}
	}
	return FALSE;
	
}
TB_BOOL IsTagOpenInit(INT32U tagName)
{
	//int i;
	
	if(tagName == TRUE)	
	{
		//Tst_F_OnOpen(name);
		return TRUE;
	}
	if( tagName > 1)
	{
		Tst_F_PrintReport("Init Tag Value Err :0x%x\n",tagName);
	}
	return FALSE;
}
#else
TB_BOOL IsTagOpenWithName(TAG_NAME tagName,const char* name,const char * locString)
{
	int i;
	if( tagName < 0x100)
	{
		Tst_F_PrintReport("Tag Value Err :0x%x,%s,%s\n",tagName,name,locString);
		return FALSE;
	}
	i = Tst_F_FindTagTable(tagName);
	if(i >= 0)
	{
		if((g_switchTable[i].m_bOpen && (g_switchTable[i].m_OpenCycle <= Tst_F_GetCycle())))
		{
			//Tst_F_OnOpen(name);
			return TRUE;
		}
	}
	return FALSE;
	
}
TB_BOOL IsTagOpenInitWithName(INT32U tagName,const char* name,const char * locString)
{
	//int i;
	
	if(tagName == TRUE)	
	{
		//Tst_F_OnOpen(name);
		return TRUE;
	}
	if( tagName > 1)
	{
		Tst_F_PrintReport("Tag Value Err :0x%x,%s,%s\n",tagName,name,locString);
	}
	return FALSE;
}
#endif
const char* Tst_F_GetTagName()
{
	return thisTagName;
}
void SetTagCloseAfter(TAG_NAME tagName, INT32U cycle)
{
	int i;
	i = Tst_F_FindTagTable(tagName);
	if( i <0)
	{
		Tst_F_PrintReport("Tag: 0x%x Not Open!\n",tagName);
		return;
	}
	if( IsTagOpen(tagName))
	{
		if(g_switchTable[i].m_CloseCycle == -1)
		{
			g_switchTable[i].m_CloseCycle = cycle;//
		}
	}
	else
	{
		if(g_switchTable[i].m_CloseCycle != -1)
		{
			Tst_F_PrintReport("Tag: 0x%x Close by other\n",tagName);
			g_switchTable[i].m_CloseCycle = -1;
		}
		return;//err
	}
	if(g_switchTable[i].m_CloseCycle == 0)
	{
		g_switchTable[i].m_bOpen = FALSE;
		g_switchTable[i].m_CloseCycle = -1;
		Tst_F_PrintReport("Tag: 0x%x Close\n",tagName);
	}
	if(g_switchTable[i].m_CloseCycle > 0)
	{
		g_switchTable[i].m_CloseCycle --;
	}
}


void GUEST_F_TST_UpdateTagTable(Tst_TagType tagTemp, INT32U cycle)
{

 #ifdef ALSTOM_CONFIG_CPU_A	
	TB_BOOL bOpen = (tagTemp&0xF0)>>4;
	TB_BOOL bSync = (tagTemp&0x0F);
#else
	TB_BOOL bSync = (tagTemp&0xF0)>>4;
	TB_BOOL bOpen = (tagTemp&0x0F);
#endif
	INT32U tagName = tagTemp>>8;;
	if (bOpen == bSync)
	{
	   Tst_F_K2oo2CpuExchange(&bOpen , &bSync, sizeof(TB_BOOL), 0x1F4u);
	   if (bOpen != bSync)
	   {
	   	
	   	Tst_F_UDPReport(4u,DTXT_C_LOC,"cpu A,B g_switchTemp sync fail",2u,bOpen,bSync,3u,4u,5u,6u);
		Tst_F_SerialReport(4u,DTXT_C_LOC,"cpu A,B g_switchTemp sync fail",2u,bOpen,bSync,3u,4u,5u,6u);
		return;
	   }
	}	
	if (TRUE == bOpen) 
	{
		if (l_OpenedStubIdx < MAX_OPEN_STUB)
		{
			GUEST_F_TST_SetTagOpen(tagName, cycle);
		}
		else
		{
			Tst_F_Report(4u,DTXT_C_LOC,"Too much tags open ",3u,tagName,l_OpenedStubIdx, bSync,0u,5u,6u);

			return;
		}
	}
	else 
	{
		GUEST_F_TST_SetTagClose(tagName, 0);
	}
	return;	
}

unsigned long TB_GetTimeInterval(INT32U sTime, INT32U eTime)
{
    INT32U timeInterval = 0;
	if(eTime < sTime)
	{
		timeInterval = 0xFFFFFFFF - sTime + eTime;
	}
	else
	{
		timeInterval = eTime - sTime;
	}
	return timeInterval;
}


void GUEST_F_TST_SetTagOpen( TAG_NAME tagName, INT32U cycle )
{
	//GUEST_F_TST_DeleteTag(g_switchTable, &l_OpenedStubIdx, tagName);
	int i = Tst_F_FindTagTable(tagName);
	if( i < 0 )
	{
		g_switchTable[l_OpenedStubIdx].m_tagName	= tagName;
		g_switchTable[l_OpenedStubIdx].m_bOpen		= TRUE;
		g_switchTable[l_OpenedStubIdx].m_OpenCycle		= cycle;
		g_switchTable[l_OpenedStubIdx].m_CloseCycle = -1;
		i = l_OpenedStubIdx;
		l_OpenedStubIdx++;
	}
	else
	{
		g_switchTable[i].m_bOpen		= TRUE;
		g_switchTable[i].m_OpenCycle		= cycle;
	}

	//DTXT_REPORT3(DTXT_REPORT_INFO,"[OPEN] STUB, idx, Sync: ",tagName,l_OpenedStubIdx, bSync);
	Tst_F_Report(4u,DTXT_C_LOC,"[OPEN] STUB, idx, cycle: ",3u,tagName,i, g_switchTable[i].m_OpenCycle,0u,5u,6u);
}
void GUEST_F_TST_SetTagClose( TAG_NAME tagName, INT32U cycle )
{
	//GUEST_F_TST_DeleteTag(g_switchTable, &l_OpenedStubIdx, tagName);
	g_tst_flag = 0;
	g_RIT_flag = 0;
	tst_Bol = FALSE;                                      
	//DTXT_REPORT4(DTXT_REPORT_INFO,"[CLOSE] STUB, idx, Sync,g_tst_flag: ",tagName,l_OpenedStubIdx, bSync,0u);//g_tst_flag);
	Tst_F_Report(4u,DTXT_C_LOC,"[CLOSE] STUB, idx: ",3u,tagName,l_OpenedStubIdx, 0,0u,5u,6u);
	/*l_OpenedStubIdx--;*/
	GUEST_F_TST_DeleteTag(g_switchTable, &l_OpenedStubIdx, tagName);
}

void GUEST_F_TST_DeleteTag( fault_insert_item* pTagBuf, INT32U* idx, TAG_NAME tagName )
{
	unsigned int i;
	TB_BOOL	isFind = FALSE;

	if ((* idx) > 0)
	{

		for (i = 0; i< *idx; i++)
		{
			if (pTagBuf[i].m_tagName == tagName)
			{
				isFind = TRUE;
				break;
			}

		}
		if (TRUE == isFind)
		{
			if(i != *idx -1)
				STD_F_Memcpy((void*)(pTagBuf + i),sizeof(fault_insert_item),(void*)(pTagBuf + *idx -1),sizeof(fault_insert_item));
			pTagBuf[*idx - 1].m_tagName = (TAG_NAME)0u;
			pTagBuf[*idx - 1].m_bOpen = FALSE;
			pTagBuf[*idx - 1].m_OpenCycle = 0;
			pTagBuf[*idx - 1].m_CloseCycle = -1;
			(*idx)--;
		}

	}
	/*else
	{
		DTXT_REPORT(DTXT_REPORT_INFO, "[Delete] Delete Tag Error: Idx equals to 0");
	}*/
}
