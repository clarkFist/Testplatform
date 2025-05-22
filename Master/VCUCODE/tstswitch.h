#ifndef _TB_SWITCH_H_
#define _TB_SWITCH_H_

#include "tst.h"


#include "tst_vcu_stubs.h"

#define TST_SWITCH_DEBUG


#define VCU_SwRT

#define MAX_OPEN_STUB				10
#define GUEST_C_TST_SYNC_TIMEOUT	50u
#define GUEST_C_TST_STUB_RETRY		3
#define GUEST_C_TST_SYNC_MAX_SIZE	100
typedef unsigned char TB_BOOL;
#define TST_TIME_VALID(x,y,z)	\
	((x) >= (y)&&(x) <= (z))
	
#define TST_StartOpenTagCYCLE(x,y)	\
	(x >= y)

extern INT32U  g_tst_flag ;
extern INT32U  g_RIT_flag;
extern BOOLEAN tst_Bol;


typedef struct 
{
	TAG_NAME m_tagName;
	TB_BOOL	 m_bOpen;
	INT32U	 m_OpenCycle;
	INT32S	 m_CloseCycle;
} fault_insert_item;
typedef INT32U Tst_TagType;
extern Tst_TagType g_switchTemp;

/*extern fault_insert_item g_SingleTagBuf[];*/
extern void SetTagCloseAfter(TAG_NAME tagName, INT32U cycle);
extern const char* Tst_F_GetTagName();
#define Tst_F_GetOpenCount(tagName) Tst_F_GetOpenCountWithName(#tagName)

INT32U Tst_F_GetOpenCountWithName(const char* name);

#ifdef TST_SWITCH_DEBUG
  TB_BOOL IsTagOpenInitWithName(INT32U tagName,const char* name,const char * locString);
  TB_BOOL IsTagOpenWithName(TAG_NAME tagName,const char* name,const char * locString);
  #define IsTagOpen(tagName) IsTagOpenWithName(tagName, #tagName,DTXT_C_LOC)
  #define IsTagOpenInit(tagName) IsTagOpenInitWithName(tagName, #tagName,DTXT_C_LOC)
#else
 TB_BOOL IsTagOpen(TAG_NAME tagName);
 TB_BOOL IsTagOpenInit(INT32U tagName);
#endif
extern unsigned long TB_GetTimeInterVal(unsigned long sTime, unsigned long eTime);
//extern TB_BOOL GUEST_F_TST_UpdateTestPlatform(INT8U ** pBuffer);
extern void GUEST_F_TST_UpdateTagTable(Tst_TagType tagname,INT32U cycle);
void GUEST_F_TST_SetTagOpen( TAG_NAME tagName, INT32U cycle);
void GUEST_F_TST_SetTagClose( TAG_NAME tagName, INT32U cycle );
void GUEST_F_TST_DeleteTag( fault_insert_item* pTagBuf, INT32U* idx, TAG_NAME tagName );
//extern void GUEST_F_TST_UpdateTagBuf(int g_CycleNO);
//extern void GUEST_F_TST_SetTagBuf(TAG_NAME tagName, TB_BOOL bOpen, TB_BOOL bSync);

/*extern TB_BOOL TST_OpenTagCycleRegion(unsigned int x,unsigned int y);
extern TB_BOOL TST_StartOpenTagCYCLE(unsigned int x);*/

//yyy
//#undef GDF_M_FAULT_EXT
//#define GDF_M_FAULT_EXT(A,B, exp) DTXT_F_Printf("!!!!!!!!!!!!ENTER INTO SAFETY MODE!!!!!!!!!!!\n")

#endif
