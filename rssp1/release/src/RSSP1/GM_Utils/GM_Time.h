/**
* @file GM_Time.h
* @brief
* @author Hao Liming
* @date 2010-3-17 10:29:05
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: </b>haoliming@casco.com.cn</pre>
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/
#ifndef _GM_TIME_H
#define _GM_TIME_H

//#include "GM_RSSP1_memset.h"
//#include "GM_Code_Convert.h"
//#include "GM_Utils_Other.h"
#include "GM_RSSP1_Utils_Other.h"
#include "GM_RSSP1_Utils_Base.h"

#ifdef WIN32
#include <sys/timeb.h>
#include <time.h>
#endif
#ifdef VXWORKS
#include <time.h>
#endif
#ifdef QNX
#include <sys/time.h>
#include <time.h>
#endif
#ifdef __cplusplus
extern "C"
{
#endif /**< __cplusplus */
#define GM_UINT32_MAX (4294967295U) /**< GM_UINT32最大值 */
#define GM_TIME_OS_SET_WIN32_PAR_ERR					0xA00B0300U			/*WIN32下，GM_Time_OS_Set传入参数有误*/
#define GM_TIME_BIOS_GET_DEFINE_ERR						0xA00B0107U			/*GM_Time_BIOS_Get模式宏定义错误*/
#define GM_TIME_BIOS_SET_QNX_RTCINIT_ERR				0xA00B0206U			/*QNX下，rtcInit()返回失败*/
#define GM_TIME_OS_SET_WIN32_SETLOCALTIME_ERR			0xA00B0301U			/*WIN32下，SetLocalTime返回失败*/
#define GM_TIME_GET_UTC_WIN32_PAR_ERR					0xA00B0400U				/*WIN32下，GM_Time_Get_UTC传入参数有误*/

#ifndef SNTP_UNIX_OFFSET
#define SNTP_UNIX_OFFSET  0x83AA7E80    /** @brief 1970 - 1900 in seconds 2208988800.0 */
#endif
#define UTC_SNTP_TIME(time) (GM_RSSP1_UINT32)((time) + SNTP_UNIX_OFFSET)

#ifdef WIN32
    /** @brief
    * 在Windows下定义与Linux、VxWorks中的struct timespec一样的结构
    * tv_sec以秒为单位，存放着自1970年7月1日以来经过的时间. tv_nsec记录了自上一秒开始经过的纳秒数
    */

    typedef struct
    {
        time_t tv_sec; /* 秒   */
        long tv_nsec;  /* 纳秒(10E-9秒) 0-1000,000,000 */
    }GM_Time_Spec_Struct;

#else/*其它操作系统*/

    typedef struct timespec GM_Time_Spec_Struct;
#endif

    /* tm结构的说明
    struct tm
    {
        int tm_sec;   秒 – 取值区间为[0,59]
        int tm_min;   分 - 取值区间为[0,59]
        int tm_hour;  时 - 取值区间为[0,23]
        int tm_mday;  一个月中的日期 - 取值区间为[1,31]
        int tm_mon;   月份（从一月开始，0代表一月） - 取值区间为[0,11]
        int tm_year;  年份，其值等于实际年份减去1900
        int tm_wday;  星期 – 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推
        int tm_yday;  从每年的1月1日开始的天数 – 取值区间为[0,365]，其中0代表1月1日，1代表1月2日，以此类推
        int tm_isdst; 夏令时标识符，实行夏令时的时候，tm_isdst为正。不实行夏令时的进候，tm_isdst为0；不了解情况时，tm_isdst()为负
    };
    */

    /**
    * @brief GM_Time_Sleep
    *
    * 延时。以毫秒为单位.
	* Windows上，毫秒级的延时不精准
    * @param[in] iSleepTime
    */
    void GM_Time_Sleep(GM_RSSP1_UINT32 iSleepTime_ms);

    /**
    * @brief GM_Time_Get_Tick_Count
    *
    * 获得系统距离启动的当前时间或tick数.
	* Windows,Linux: 获取当前时间
	* VxWorks：tv_sec=0，用tv_nsec来表示距离启动的tick数
    * @return GM_Time_Spec_Struct
    */
    GM_Time_Spec_Struct GM_Time_Get_Tick_Count(void);

    /**
    * @brief GM_Time_Get_Tick_Count_Interval
    *
    * 获得时间A与时间B之间的间隔.单位：毫秒数
    * @param[in] newTime 新时间 GM_Time_Get_Tick_Count获取的时间返回值
    * @param[in] oldTime 旧时间 GM_Time_Get_Tick_Count获取的时间返回值
    * @return GM_RSSP1_UINT32 新时间-旧时间(ms) 新时间应是旧时间之后的时间节点 不足1ms的部分舍弃
	* 最大返回 GM_UINT32_MAX （ms），若传入的两个时间 间隔超过此范围，则返回 GM_UINT32_MAX （ms）
    */
    GM_RSSP1_UINT32 GM_Time_Get_Tick_Count_Interval(GM_Time_Spec_Struct newTime, GM_Time_Spec_Struct oldTime);

    /**
    * @brief GM_Time_BIOS_Get
    * 公司目前只有RC板提供BIOS时间,BIOS时间根据设置值变化
    * 取BIOS中的时间. VxWorks操作系统时间永远是1970,除非做修改
    * @param[out] sysDateTime 年月日时分秒
    * @return time_t 据1970年的秒数
    */
    GM_RSSP1_BOOL GM_Time_BIOS_Get(struct tm* sysDateTime);

    /**
    * @brief GM_Time_BIOS_Set
    * 公司目前只有RC板提供BIOS时间
    * 设置BIOS时间.
    * @param[out] sysDateTime 年月日时分秒
    */
    void GM_Time_BIOS_Set(struct tm* sysDateTime);

    /**
    * @brief GM_Time_OS_Get
    *
    * 取系统时间 VxWorks其中时的系统时间总是1900 01-01 0：0：0，与BIOS时间无关
    * @param[out] dayTime 年月日时分秒
    * @return time_t 据1900年的秒数
    */
    time_t GM_Time_OS_Get(struct tm* dayTime);

    /**
    * @brief GM_Time_OS_Set
    *
    * 设置系统时间.
    * @param[out] dayTime 年月日时分秒
    * @return GM_RSSP1_BOOL
    */
    GM_RSSP1_BOOL GM_Time_OS_Set(struct tm* dayTime);

    /**
    * @brief GM_Time_Get_UTC
    *
    * Detailed description.
    * @param[out] secondsFrom1900  秒表示的时间.
    * @param[out] fraction  皮秒表示的时间.
    * @return GM_RSSP1_BOOL
    */
    GM_RSSP1_BOOL GM_Time_Get_UTC(GM_RSSP1_UINT32 *secondsFrom1900, GM_RSSP1_UINT32 *fraction);

#ifdef __cplusplus
}

#endif /**< __cplusplus */

#endif /**< _GM_TIME_H */

