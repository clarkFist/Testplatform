# VCU

> 20211014

## 一. 功能

### 1.判断桩打开/关闭

```c
//可检测非法格式，但无法定位
BOOLEAN IsTagOpen(TagName name) // TagName 枚举 判断枚举桩是否打开
BOOLEAN isTagOpenINIT(BOOLEAN name) // 判断宏定义桩是否打开
void SetTagCloseAfter(TagName name, INT32U count)//仅可用于枚举桩，当调用count后桩会被关闭
for(int i = 0; i < 10;i++)
 SetTagCloseAfter(VCU_SyRTC_0001_01, 10);
//for 循环执行n次后VCU_SyRTC_0001_01将会被关闭
//若在执行n次前被关闭则会打印一条信息
```

可通过设置宏 TST_SWITCH_DEBUG（tstswitch.h)  检测出非法格式

```c
//使用方法与非debug 相同，但通过宏额外传递了几个参数，可定位非法格式位置
#ifdef Tst_SWITCH_DEBUG
  TB_BOOL IsTagOpenInitWithName(INT32U  tagName,const char* name,const char * locString);
  TB_BOOL IsTagOpenWithName(TAG_NAME tagName,const char* name,const char * locString);
  #define IsTagOpen(tagName) IsTagOpenWithName(tagName, #tagName,DTXT_C_LOC)
  #define IsTagOpenInit(tagName) IsTagOpenInitWithName(tagName, #tagName,DTXT_C_LOC)
#else
 TB_BOOL IsTagOpen(TAG_NAME tagName);
 TB_BOOL IsTagOpenInit(INT32U tagName);  
#endif
```

### 2.与测试平台通信实现自定义功能

目前可用的配置项为

```c
//更改VCU的打印flag
Tst_Flag_AllReport = FALSE;
//更改VCU的打印过滤周期
Tst_Cycle_UDPReport = 20;
//让VCU宕机
Tst_Control_flags[Shutdown] = TRUE;//不会进入Reprog
//主备切换
Tst_Control_flags[ToStandby] = TRUE;
//触发错误过滤
Tst_Control_flags[TriggerFaultFifter] = TRUE//不会进入Reprog
//允许错误过滤
Tst_Control_flags[FaultFifter] == TRUE
//禁止宕机后进入Reprog
Tst_Control_flags[ReprogflagOnDump] == FALSE
```

### 3. 发送UDP LOG

可通过配置print.c 将其他打印转发到udp

```c
//和PRINT_F_Report 参数相同，打印会同时发往串口和udp udp打印会被过滤
#define Tst_F_Report(...)  do{Tst_F_ReportStore(Tst_PrintType,__VA_ARGS__);}while(0)
//和PRINT_F_Printf 参数相同， 串口打印走任务调度，udp打印会被过滤
#define Tst_F_PrintReport(...)  do{Tst_F_PrintReportStore(Report_Both,DTXT_C_LOC " ",__VA_ARGS__);}while(0)
//和PRINT_F_Printf 参数相同， 串口打印走中断, udp打印不会被过滤
#define Tst_F_PrintPrint(...) do{if(Tst_Flag_SerialReport == TRUE)\
                            { Tst_Flag_AllReportFlag[1] = FALSE;DTXT_F_Printf(__VA_ARGS__);} \
                            Tst_F_UDPPrint(DTXT_C_LOC " ",__VA_ARGS__);\
                                            }while(0)


/*将所需发送的消息暂存，在n个周期内发送过的重复消息将不会被发送，最多保存20个消息
，当每周期发送不同的消息超过20条时，过滤将失效
当发送过多缓冲区满时，会打印store faile udp  。消息的序号不会显示丢失*/
ALSTOM_T_Status Tst_F_ReportStore(Tst_ReportType tst_type ,DTXT_E_ReportType    Type,
                   const char*            locString,
                   const char*            String,
                   INT8U            Numbval,
                   INT32U           Value1,
                   INT32U           Value2,
                   INT32U           Value3,
                   INT32U           Value4,
                   INT32U           Value5,
                   INT32U           Value6);

/*此函数每次调用都会发送，发送的消息会立刻发送或储存在发送缓冲区内*/
void Tst_F_UDPReport(DTXT_E_ReportType    Type,//store
                   const char*            locString,
                   const char*            String,
                   INT8U            Numbval,
                   INT32U           Value1,
                   INT32U           Value2,
                   INT32U           Value3,
                   INT32U           Value4,
                   INT32U           Value5,
                   INT32U           Value6);

void Tst_F_SerialReport(...)//同PRINT_F_Report，不会向 UDP转发
```

### 4. 宕机进update模式

  若定义#define ReprogBreakOnDump 180 且 gdf.c 已添加测试代码

  若VCU在上电时间小于110s 宕机 && 宕机不烧熔丝（熔丝宏未定义 或 满足错误过滤），则VCU会破坏Reprog 安全标志 使 重启后进入update模式

```c
#ifdef TestPlatform
    //    bFaultFilterAllowed = TRUE;  
#ifdef ReprogBreakOnDump
#ifndef VCU_BURN_FUSE_DISABLE 
     if(FALSE == BurnFuse)
#endif
         if( ((OSW_F_TickGet()/1000) < ReprogBreakOnDump) && Tst_Control_flags[ReprogflagOnDump] == TRUE )
         {
             FLASH_F_SuspendDirect(2u);
             Tst_F_BreakReporg(ReprogBreakOnDump);
             BurnFuse = FALSE;
         }

#endif

#endif
```

### 5. 自定义功能

```c
typedef struct{
    INT32U val1;
    INT32U val2;
}Tst_UDPConfArgs;

void Tst_F_WiriteTempSet( Tst_UDPConfArgs* args)
{
    INT32U val1 = args->val1, val2 = args->val2;
    dst = val1 - val2;//dst 为全局变量
    if(val1 == 0)
        func(val2);
}
Tst_F_ConfPonterAdd(3,Tst_F_WiriteTempSet);//下位机使用3 参数调用该函数并为val1,val2 赋值
```

PC 端可通过Conf 命令调用Tst_F_WiriteTempSet 函数实现自定义功能

val1 、val2 为PC端指定的参数

当前仅支持两个32位 的无符号整形

## 二. 配置

### 安装

   测试代码在文件夹VCUCODE 内

   将 tst.c, tst.h ,tst_tsk.c tst_tsk.h, tst_define.h  tstswitch.h tstswitch.c 复制到tst 文件夹

   将 原有的桩 移动到 tst_vcu_stubs.h 文件内 并将该文件放进tst 文件夹

   将print.c,  gdf.c 中的 宏#define TestPlatflam 的内容添加到 源代码对应文件

   修改com.c or guest.c

```c
#ifdef TestPlatform
#include "tst.h"
#include "tstswitch.h"
#include "tst_tsk.h"
#endif 
//以上添加到com.c or guest.c 头部

#ifdef TestPlatform
    Tst_F_Task(functionalMode);
#endif
//以上添加到 mainloop 函数尾部
#ifdef TestPlatform
    Tst_F_Creat();
#endif
//以上添加到 com or guest 任务 的Creat 函数内
#ifdef TestPlatform
    Tst_F_Start();
#endif
//以上添加到 com or guest 任务 的Start 函数内
```

### 配置

```c
//tst.h
#define ReprogBreakOnDump 180 //若定义该宏则允许在上电110s 前宕机重启后进Reprog
#define Tst_Flag_UDPReportDefault FALSE //UDP LOG 是否允许（初值）
#define Tst_Flag_SerialReportDefault TRUE//Tst 串口 LOG 是否允许（初值）
#define Tst_Flag_AllReportDefault FALSE //来自非测试打印函数的  LOG 是否允许转发至udp（初值）
#define Tst_Cycle_UDPReportDefault  10  // udp and serial 打印过滤周期
#define Tst_UDP_PORT  18125u //收发双端端口号
#define Tst_UDPDataMaxLen 90 //  取值范围 (42,120)
typedef enum{
Shutdown,
FaultFifter,
TriggerFaultFifter,
ReprogflagOnDump,
ToStandby,
}Tst_ControlFlag;
//tst_define.h
#define VCU_VERSION 112 //两网口，四网口不兼容
//tst_tst.c
void Tst_F_Creat()
{
    //tst_pFlagGrp = OSW_F_FlagCreate(0u, &tst_status);
    Tst_V_TaskInfo = OSW_F_QCreate((void**)&Tst_V_PointerQMsgTbl, 0x20u);
    if(NULL == Tst_V_TaskInfo)
    {
        DTXT_F_Report(DTXT_REPORT_ERROR, DTXT_C_LOC , "tst", 0u, 0u,0u, 0u,0u, 0u, 0u);
    }
    Tst_F_TstSwitchInit();
    Tst_F_ConfPonterAdd(0,Tst_F_ReportFlagSet);//函数指针定义
    Tst_F_ConfPonterAdd(1,Tst_F_ReportCycleSet);
    Tst_F_ConfPonterAdd(2,Tst_F_ControlFlagSet);
}
```



