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
#define GM_UINT32_MAX (4294967295U) /**< GM_UINT32���ֵ */
#define GM_TIME_OS_SET_WIN32_PAR_ERR					0xA00B0300U			/*WIN32�£�GM_Time_OS_Set�����������*/
#define GM_TIME_BIOS_GET_DEFINE_ERR						0xA00B0107U			/*GM_Time_BIOS_Getģʽ�궨�����*/
#define GM_TIME_BIOS_SET_QNX_RTCINIT_ERR				0xA00B0206U			/*QNX�£�rtcInit()����ʧ��*/
#define GM_TIME_OS_SET_WIN32_SETLOCALTIME_ERR			0xA00B0301U			/*WIN32�£�SetLocalTime����ʧ��*/
#define GM_TIME_GET_UTC_WIN32_PAR_ERR					0xA00B0400U				/*WIN32�£�GM_Time_Get_UTC�����������*/

#ifndef SNTP_UNIX_OFFSET
#define SNTP_UNIX_OFFSET  0x83AA7E80    /** @brief 1970 - 1900 in seconds 2208988800.0 */
#endif
#define UTC_SNTP_TIME(time) (GM_RSSP1_UINT32)((time) + SNTP_UNIX_OFFSET)

#ifdef WIN32
    /** @brief
    * ��Windows�¶�����Linux��VxWorks�е�struct timespecһ���Ľṹ
    * tv_sec����Ϊ��λ���������1970��7��1������������ʱ��. tv_nsec��¼������һ�뿪ʼ������������
    */

    typedef struct
    {
        time_t tv_sec; /* ��   */
        long tv_nsec;  /* ����(10E-9��) 0-1000,000,000 */
    }GM_Time_Spec_Struct;

#else/*��������ϵͳ*/

    typedef struct timespec GM_Time_Spec_Struct;
#endif

    /* tm�ṹ��˵��
    struct tm
    {
        int tm_sec;   �� �C ȡֵ����Ϊ[0,59]
        int tm_min;   �� - ȡֵ����Ϊ[0,59]
        int tm_hour;  ʱ - ȡֵ����Ϊ[0,23]
        int tm_mday;  һ�����е����� - ȡֵ����Ϊ[1,31]
        int tm_mon;   �·ݣ���һ�¿�ʼ��0����һ�£� - ȡֵ����Ϊ[0,11]
        int tm_year;  ��ݣ���ֵ����ʵ����ݼ�ȥ1900
        int tm_wday;  ���� �C ȡֵ����Ϊ[0,6]������0���������죬1��������һ���Դ�����
        int tm_yday;  ��ÿ���1��1�տ�ʼ������ �C ȡֵ����Ϊ[0,365]������0����1��1�գ�1����1��2�գ��Դ�����
        int tm_isdst; ����ʱ��ʶ����ʵ������ʱ��ʱ��tm_isdstΪ������ʵ������ʱ�Ľ���tm_isdstΪ0�����˽����ʱ��tm_isdst()Ϊ��
    };
    */

    /**
    * @brief GM_Time_Sleep
    *
    * ��ʱ���Ժ���Ϊ��λ.
	* Windows�ϣ����뼶����ʱ����׼
    * @param[in] iSleepTime
    */
    void GM_Time_Sleep(GM_RSSP1_UINT32 iSleepTime_ms);

    /**
    * @brief GM_Time_Get_Tick_Count
    *
    * ���ϵͳ���������ĵ�ǰʱ���tick��.
	* Windows,Linux: ��ȡ��ǰʱ��
	* VxWorks��tv_sec=0����tv_nsec����ʾ����������tick��
    * @return GM_Time_Spec_Struct
    */
    GM_Time_Spec_Struct GM_Time_Get_Tick_Count(void);

    /**
    * @brief GM_Time_Get_Tick_Count_Interval
    *
    * ���ʱ��A��ʱ��B֮��ļ��.��λ��������
    * @param[in] newTime ��ʱ�� GM_Time_Get_Tick_Count��ȡ��ʱ�䷵��ֵ
    * @param[in] oldTime ��ʱ�� GM_Time_Get_Tick_Count��ȡ��ʱ�䷵��ֵ
    * @return GM_RSSP1_UINT32 ��ʱ��-��ʱ��(ms) ��ʱ��Ӧ�Ǿ�ʱ��֮���ʱ��ڵ� ����1ms�Ĳ�������
	* ��󷵻� GM_UINT32_MAX ��ms���������������ʱ�� ��������˷�Χ���򷵻� GM_UINT32_MAX ��ms��
    */
    GM_RSSP1_UINT32 GM_Time_Get_Tick_Count_Interval(GM_Time_Spec_Struct newTime, GM_Time_Spec_Struct oldTime);

    /**
    * @brief GM_Time_BIOS_Get
    * ��˾Ŀǰֻ��RC���ṩBIOSʱ��,BIOSʱ���������ֵ�仯
    * ȡBIOS�е�ʱ��. VxWorks����ϵͳʱ����Զ��1970,�������޸�
    * @param[out] sysDateTime ������ʱ����
    * @return time_t ��1970�������
    */
    GM_RSSP1_BOOL GM_Time_BIOS_Get(struct tm* sysDateTime);

    /**
    * @brief GM_Time_BIOS_Set
    * ��˾Ŀǰֻ��RC���ṩBIOSʱ��
    * ����BIOSʱ��.
    * @param[out] sysDateTime ������ʱ����
    */
    void GM_Time_BIOS_Set(struct tm* sysDateTime);

    /**
    * @brief GM_Time_OS_Get
    *
    * ȡϵͳʱ�� VxWorks����ʱ��ϵͳʱ������1900 01-01 0��0��0����BIOSʱ���޹�
    * @param[out] dayTime ������ʱ����
    * @return time_t ��1900�������
    */
    time_t GM_Time_OS_Get(struct tm* dayTime);

    /**
    * @brief GM_Time_OS_Set
    *
    * ����ϵͳʱ��.
    * @param[out] dayTime ������ʱ����
    * @return GM_RSSP1_BOOL
    */
    GM_RSSP1_BOOL GM_Time_OS_Set(struct tm* dayTime);

    /**
    * @brief GM_Time_Get_UTC
    *
    * Detailed description.
    * @param[out] secondsFrom1900  ���ʾ��ʱ��.
    * @param[out] fraction  Ƥ���ʾ��ʱ��.
    * @return GM_RSSP1_BOOL
    */
    GM_RSSP1_BOOL GM_Time_Get_UTC(GM_RSSP1_UINT32 *secondsFrom1900, GM_RSSP1_UINT32 *fraction);

#ifdef __cplusplus
}

#endif /**< __cplusplus */

#endif /**< _GM_TIME_H */

