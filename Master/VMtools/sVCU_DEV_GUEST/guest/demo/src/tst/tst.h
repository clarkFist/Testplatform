#ifndef TST_H_
#define TST_H_


#include "tst_define.h"

//#define ReprogBreakOnDump 180 //若定义该宏则允许在上电180s 前宕机重启后进Reprog

#define Tst_Flag_UDPReportDefault FALSE //UDP LOG 是否允许（初值）
#define Tst_Flag_SerialReportDefault TRUE//Tst 串口 LOG 是否允许（初值）
#define Tst_Flag_AllReportDefault FALSE //来自非测试打印函数的  LOG 是否允许转发至udp（初值）
#define VALUE_DEFAULT_NONE 0xFFFF

#define Tst_Cycle_UDPReportDefault  10  // udp and serial 打印过滤周期
//#define Tst_Cycle_SerialReportDefault 10
#define Tst_UDP_PORT  18125u//收发双端端口号
 
typedef enum{
	Tst_C_FUNCTIONAL_BOOT = 0,
	Tst_C_FUNCTIONAL_NORMAL = 1,
	Tst_C_FUNCTIONAL_CLOSE = 2,
}Tst_C_FUNCTIONAL_MODE;


extern BOOLEAN Tst_Flag_AllReport;
extern BOOLEAN Tst_Flag_AllReportFlag[2];
extern BOOLEAN Tst_Flag_UDPReport;
extern BOOLEAN Tst_Flag_SerialReport;
extern BOOLEAN Tst_Control_flags[100];
extern BOOLEAN Tst_Maint_flags[100];

enum Tst_UdpDataType{
	Tst_UdpDataType_tag = 0x00u,
	Tst_UdpDataType_conf = 0x02u,
	Tst_UdpDataType_log = 0x0Eu,
	Tst_UdpDataType_maint = 0x04,
};
enum Tst_ConfType{
	Tst_ConfType_Flag = 0x00u,
	Tst_ConfType_Cycle = 0x01u,
};

#define Tst_UDPDataMaxLen 110 //  取值范围 (42,120)
typedef struct{
	INT8U dstNodeID;
	INT8U srcNodeID;
	INT8U dataType;//
	INT8U dataLen;
	INT8U data[Tst_UDPDataMaxLen];
}Tst_UDPFrame;

typedef struct{
	INT8U maint_type;
	INT8U req_ack;
	INT8U msg[Tst_UDPDataMaxLen - 2];
}Tst_UDPMaintFrame;
#define Tst_F_UDPLenGet(dataLen) (dataLen + sizeof(Tst_UDPFrame) - Tst_UDPDataMaxLen)
#define ethNodeId 0xFFu
#define Tst_UDP_MAX_DATA_LEN 0x150u

#define FrameBufSize 60u
typedef struct{
	Tst_UDPFrame data[FrameBufSize];
	INT8U pos;
	INT8U size;
}Tst_FrameBuf;

#define Tst_ReportBufMaxSize 30


extern INT32U TstServerIp;
extern INT8U localNodeID_B;
extern INT8U localNodeID_A;
extern INT8U localNodeID;
extern INT32U tst_cycle;

#define Tst_F_canAddrGet(id)	(0x7f000000u|id|(GUEST_C_LOW_PRIO<<16))
#define Tst_F_isCanAddr(addr) ((addr>>24) == 0x7f)

typedef enum{
Report_Serial = 0u,
Report_Udp = 1u,
Report_Both= 2u ,
Report_All = 3u
}Tst_ReportType;
#define Tst_Task 4u

typedef enum{
Shutdown,
FaultFifter,
TriggerFaultFifter,
ReprogflagOnDump,
ToStandby,
}Tst_ControlFlag;

typedef enum{
	GetEventLog,
}Tst_MaintItems;

typedef struct{
	INT32U val1;
	INT32U val2;
}Tst_UDPConfArgs;

extern void Tst_F_ConfPonterAdd(INT8U index,void (*p)(Tst_UDPConfArgs* args));
//extern Tst_ConfArgsType* Tst_F_ConfPonterAddSync(INT8U index,void (*p)(Tst_ConfPonterArgs));

#define Tst_PrintType Report_Both
//和PRINT_F_Report 参数相同，打印会同时发往串口和udp udp打印会被过滤
#define Tst_F_Report(...)  do{Tst_F_ReportStore(Tst_PrintType,__VA_ARGS__);}while(0)
//和PRINT_F_Printf 参数相同， 串口打印走中断, udp打印不会被过滤
#define Tst_F_PrintPrint(...) do{if(Tst_Flag_SerialReport == TRUE)\
							{ Tst_Flag_AllReportFlag[1] = FALSE;DTXT_F_Printf(__VA_ARGS__);} \
							Tst_F_UDPPrint(DTXT_C_LOC " ",__VA_ARGS__);\
											}while(0)
//和PRINT_F_Printf 参数相同， 串口打印走任务调度，udp打印会被过滤
#define Tst_F_PrintReport(...)  do{Tst_F_PrintReportStore(Report_Both,DTXT_C_LOC " ",__VA_ARGS__);}while(0)
//获取测试任务周期数
INT32U Tst_F_GetCycle();

void Tst_F_TriggerFaultfifter();
void Tst_F_EntrySafetyMode();
ALSTOM_T_Status Tst_F_ReportStore(Tst_ReportType tst_type,DTXT_E_ReportType    Type,//store
                   const char*            locString,
                   const char*            String,
                   INT8U            Numbval,
                   INT32U           Value1,
                   INT32U           Value2,
                   INT32U           Value3,
                   INT32U           Value4,
                   INT32U           Value5,
                   INT32U           Value6);
void Tst_F_UDPReport(DTXT_E_ReportType    Type,
                   const char*            locString,
                   const char*            String,
                   INT8U            Numbval,
                   INT32U           Value1,
                   INT32U           Value2,
                   INT32U           Value3,
                   INT32U           Value4,
                   INT32U           Value5,
                   INT32U           Value6);
void Tst_F_SerialReport(DTXT_E_ReportType    Type,
                   const char*            locString,
                   const char*            String,
                   INT8U            Numbval,
                   INT32U           Value1,
                   INT32U           Value2,
                   INT32U           Value3,
                   INT32U           Value4,
                   INT32U           Value5,
                   INT32U           Value6);
void Tst_F_PrintReportStore(Tst_ReportType tst_type,const char* locString,const char* fmt,...);
void Tst_F_UDPPrint(const char* locString,const char* fmt,...);
INT32U Tst_F_GetVCUTime(void);
typedef struct{
DTXT_E_ReportType    Type;//store
                   char            locString[30];
                   char            String[120];
                   INT8U            Numbval;
                   INT32U           Value1;
                   INT32U           Value2;
                   INT32U           Value3;
                   INT32U           Value4;
                   INT32U           Value5;
                   INT32U           Value6;
}Tst_ReportArgs;
#endif
