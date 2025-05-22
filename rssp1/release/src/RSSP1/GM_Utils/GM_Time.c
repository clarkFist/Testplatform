/**
* @file GM_Time.c
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
#include "GM_Time.h"
#include "common\GM_RSSP1_Memory.h"

#define Convert_To_Tm(x) (((x) & 0x0FU) + (((x) & 0xF0U)>>4U)*10U)
#define Convert_Fm_Tm(x) (((x)%10U)|(((x)/10U)<<4U))/*规则检查加U */
/*VLE_6U_RC版 使用硬件rtc接口 需要程序定义的结构*/
typedef struct TimeMode
{
	unsigned int Year;
	unsigned int Month;
	unsigned int DayOfMonth;
	unsigned int DayOfWeek;
	unsigned int Hour;
	unsigned int Minute;
	unsigned int Second;
}TimeMode;

typedef struct
{
	unsigned char dotsec;
	unsigned char sec;
	unsigned char min;
	unsigned char hour;
	unsigned char day;
	unsigned char date;
	unsigned char month;
	unsigned char year;
}time_define;

/*需兼容板卡BSP  API的函数*/
GM_RSSP1_BOOL GM_Time_BIOS_Get(struct tm* dayTime)
{
#ifdef _VLE_6U_RC
	TimeMode rtc_read={0};
	GM_RSSP1_memset((void*)(&rtc_read),(GM_RSSP1_UINT8)0U,(size_t)(sizeof(TimeMode)));
	if (NULL == dayTime)
	{
		GM_RSSP1_Log_Usr(GM_TIME_BIOS_GET_6U_PAR_ERR, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	Drv_Rtc_Update();
	if(ERROR == Drv_Rtc_Time_Get (&rtc_read))
	{
		GM_RSSP1_Log_Usr(GM_TIME_BIOS_GET_6U_TIME_GET_ERR, (int)rtc_read.Year, (int)rtc_read.Month, (int)rtc_read.DayOfMonth, (int)rtc_read.Hour, (int)rtc_read.Minute, (int)rtc_read.Second);
		return GM_RSSP1_FALSE;
	}
	/*logMsg("inBIOSget: [%d]-[%d]-[%d] [%d]:[%d]:[%d]\n",
		rtc_read.Year, rtc_read.Month,rtc_read.DayOfMonth,rtc_read.Hour,rtc_read.Minute ,rtc_read.Second);
	还有赋值给传入参数*/
	dayTime->tm_year = rtc_read.Year - 1900U;
	dayTime->tm_mon = rtc_read.Month - 1U;
	dayTime->tm_mday = rtc_read.DayOfMonth;
	dayTime->tm_wday = rtc_read.DayOfWeek -1U;
	dayTime->tm_hour = rtc_read.Hour;
	dayTime->tm_min = rtc_read.Minute;
	dayTime->tm_sec = rtc_read.Second;

	return GM_RSSP1_TRUE;
#else/*除VLE_6U板*/

#ifdef VXWORKS6_6 /*EMERSON板VXWORKS6_6*/
	if (NULL == dayTime)
	{
		GM_RSSP1_Log_Usr(GM_TIME_BIOS_GET_VXWORKS6_6_PAR_ERR, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	if(ERROR == sysRtcGet(dayTime))
	{
		GM_RSSP1_Log_Usr(GM_TIME_BIOS_GET_VXWORKS6_6_SYSRTCGET_ERR, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	return GM_RSSP1_TRUE;
#else

#ifdef QNX /*EMERSON板QNX*/
	time_define myRtcTime;

	if (NULL == dayTime)
	{
		GM_RSSP1_Log_Usr(GM_TIME_BIOS_GET_QNX_PAR_ERR, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	memset(&myRtcTime,0x0,sizeof(time_define));

	if(rtcInit() == -1)
	{
		/*printf("rtcInit error\n");*/
		GM_RSSP1_Log_Usr(GM_TIME_BIOS_GET_QNX_RTCINIT_ERR, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	if( rtcRead(&myRtcTime) ==-1 )
	{
		/*printf("rtcRead error\n");*/
		GM_RSSP1_Log_Usr(GM_TIME_BIOS_GET_QNX_RTCREAD_ERR, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	dayTime->tm_year = Convert_To_Tm(myRtcTime.year) + 100U;
	dayTime->tm_mon = Convert_To_Tm(myRtcTime.month);
	dayTime->tm_mday = Convert_To_Tm(myRtcTime.date);
	dayTime->tm_wday = Convert_To_Tm(myRtcTime.day);
	dayTime->tm_hour = Convert_To_Tm(myRtcTime.hour);
	dayTime->tm_min = Convert_To_Tm(myRtcTime.min);
	dayTime->tm_sec = Convert_To_Tm(myRtcTime.sec);
	return GM_RSSP1_TRUE;
#else
	GM_RSSP1_Log_Usr(GM_TIME_BIOS_GET_DEFINE_ERR, 0, 0, 0, 0, 0, 0);
	return GM_RSSP1_FALSE;
#endif

#endif

#endif



}
/*需兼容板卡BSP  API的函数*/
void GM_Time_BIOS_Set(struct tm* dayTime)
{
#ifdef _VLE_6U_RC
	TimeMode rtc={0} ;
	GM_RSSP1_memset((void*)(&rtc),(GM_RSSP1_UINT8)0U,(size_t)(sizeof(TimeMode)));
	if (NULL == dayTime)
	{
		GM_RSSP1_Log_Usr(GM_TIME_BIOS_SET_6U_PAR_ERR, 0, 0, 0, 0, 0, 0);
		logMsg("daytime is null\n",0,0,0,0,0,0);
		return;
	}
	if ((dayTime->tm_sec < 0)||(dayTime->tm_sec > 59)
		||(dayTime->tm_min < 0)||(dayTime->tm_min > 59)
		||(dayTime->tm_hour < 0)||(dayTime->tm_hour > 23)
		||(dayTime->tm_mday < 1)||(dayTime->tm_mday > 31)
		||(dayTime->tm_mon < 0)||(dayTime->tm_mon > 11)
		||(dayTime->tm_year < 0))

	{
		GM_RSSP1_Log_Usr(GM_TIME_BIOS_SET_6U_DAYTIME_ERR, dayTime->tm_year, dayTime->tm_mon, dayTime->tm_mday, dayTime->tm_hour, dayTime->tm_min, dayTime->tm_sec);
		logMsg("daytime error: [%d]-[%d]-[%d] [%d]:[%d]:[%d]\n",
				dayTime->tm_year, dayTime->tm_mon, dayTime->tm_mday, dayTime->tm_hour,dayTime->tm_min ,dayTime->tm_sec);
		return;
	}
	rtc.Year = dayTime->tm_year + 1900;
	rtc.Month = dayTime->tm_mon + 1;
	rtc.DayOfMonth= dayTime->tm_mday;
	rtc.DayOfWeek = dayTime->tm_wday + 1;/*根据宋志坚意见增加week设置 2012-11-12*/
	rtc.Hour = dayTime->tm_hour;
	rtc.Minute = dayTime->tm_min;
	rtc.Second = dayTime->tm_sec;

	if(ERROR == Drv_Rtc_Time_Set (&rtc))
	{
		GM_RSSP1_Log_Usr(GM_TIME_BIOS_SET_6U_TIME_SET_ERR, 0, 0, 0, 0, 0, 0);
		logMsg("Drv_Rtc_Time_Set error!\n");
		return;
	}
#endif

#ifdef VXWORKS6_6
	if (NULL == dayTime)
	{
		GM_RSSP1_Log_Usr(GM_TIME_BIOS_SET_VXWORKS6_6_PAR_ERR, 0, 0, 0, 0, 0, 0);
		logMsg("daytime is null\n",0,0,0,0,0,0);
		return;
	}
	if ((dayTime->tm_sec < 0)||(dayTime->tm_sec > 59)
		||(dayTime->tm_min < 0)||(dayTime->tm_min > 59)
		||(dayTime->tm_hour < 0)||(dayTime->tm_hour > 23)
		||(dayTime->tm_mday < 1)||(dayTime->tm_mday > 31)
		||(dayTime->tm_mon < 0)||(dayTime->tm_mon > 11)
		||(dayTime->tm_year < 0))

	{
		GM_RSSP1_Log_Usr(GM_TIME_BIOS_SET_VXWORKS6_6_DAYTIME_ERR, dayTime->tm_year, dayTime->tm_mon, dayTime->tm_mday, dayTime->tm_hour, dayTime->tm_min, dayTime->tm_sec);
		logMsg("daytime error: [%d]-[%d]-[%d] [%d]:[%d]:[%d]\n",
			dayTime->tm_year, dayTime->tm_mon, dayTime->tm_mday, dayTime->tm_hour,dayTime->tm_min ,dayTime->tm_sec);
		return;
	}
	sysRtcSet(dayTime);
#endif

#ifdef QNX
	time_define myRtcTime;
	if (NULL == dayTime)
	{
		GM_RSSP1_Log_Usr(GM_TIME_BIOS_SET_QNX_PAR_ERR, 0, 0, 0, 0, 0, 0);
		return;
	}
	memset(&myRtcTime,0x0,sizeof(time_define));

	if(rtcInit() == -1)
	{
		/*printf("rtcInit error\n");*/
		GM_RSSP1_Log_Usr(GM_TIME_BIOS_SET_QNX_RTCINIT_ERR, 0, 0, 0, 0, 0, 0);
		return;
	}
	myRtcTime.year = (unsigned char)(Convert_Fm_Tm((dayTime->tm_year - 100)));
	myRtcTime.month = (unsigned char)(Convert_Fm_Tm(dayTime->tm_mon));
	myRtcTime.date = (unsigned char)(Convert_Fm_Tm(dayTime->tm_mday));
	myRtcTime.day = (unsigned char)(Convert_Fm_Tm(dayTime->tm_wday));
	myRtcTime.hour = (unsigned char)(Convert_Fm_Tm(dayTime->tm_hour));
	myRtcTime.min = (unsigned char)(Convert_Fm_Tm(dayTime->tm_min));
	myRtcTime.sec = (unsigned char)(Convert_Fm_Tm(dayTime->tm_sec));

	rtcWrite(&myRtcTime);
#endif
    return;
}

/* 以下为 需兼容操作系统API的函数*/
void GM_Time_Sleep(GM_RSSP1_UINT32 iSleepTime_ms)
{
#ifdef WIN32
   Sleep(iSleepTime_ms);
#endif
#ifdef QNX
    usleep(iSleepTime_ms * 1000U);
#endif
#ifdef VXWORKS
    GM_Time_Spec_Struct req={0};

    GM_Time_Spec_Struct rem={0};

    GM_RSSP1_memset((void*)(&req), (GM_RSSP1_UINT8)0U, (size_t)(sizeof(req)));
    GM_RSSP1_memset((void*)(&rem), (GM_RSSP1_UINT8)0U, (size_t)(sizeof(rem)));

    if ((GM_RSSP1_UINT32)1000U > (GM_RSSP1_UINT32)iSleepTime_ms)
    {
        req.tv_sec = (time_t) 0;
        req.tv_nsec = iSleepTime_ms * 1000000U;
        nanosleep(&req, &rem);
    }
    else
    {
        taskDelay(iSleepTime_ms * sysClkRateGet() / 1000);
    }

#endif
    return;
}
/*需兼容操作系统API的函数*/
GM_Time_Spec_Struct GM_Time_Get_Tick_Count(void)
{
    GM_Time_Spec_Struct t={0};

#ifdef WIN32
    SYSTEMTIME sys;
    time_t now_time = time(NULL);

	GM_RSSP1_memset((void*)(&sys), (GM_RSSP1_UINT8)0U, (size_t)(sizeof(SYSTEMTIME)));
	GM_RSSP1_memset((void*)(&t), (GM_RSSP1_UINT8)0U, (size_t)(sizeof(GM_Time_Spec_Struct)));

    GetLocalTime(&sys);
    t.tv_sec = (GM_RSSP1_UINT32)now_time;
    t.tv_nsec = sys.wMilliseconds * 1000000;
#endif
#ifdef QNX

    struct timeval current; /**< 该结构包括自UNIX纪元起的秒数tv_sec和毫秒数tv_usec */

	GM_RSSP1_memset((void*)(&t), (GM_RSSP1_UINT8)0U, (size_t)(sizeof(GM_Time_Spec_Struct)));
	GM_RSSP1_memset((void*)(&current), (GM_RSSP1_UINT8)0U, (size_t)(sizeof(current)));
    gettimeofday(&current, NULL);
    t.tv_sec = current.tv_sec;
    t.tv_nsec = current.tv_usec * 1000;
#endif
#ifdef VXWORKS
	GM_RSSP1_memset((void*)(&t), (GM_RSSP1_UINT8)0U, (size_t)(sizeof(GM_Time_Spec_Struct)));
    t.tv_sec = 0;
    t.tv_nsec = tickGet();
#endif
    return t;
}
/*需兼容操作系统API的函数*/
GM_RSSP1_UINT32 GM_Time_Get_Tick_Count_Interval(GM_Time_Spec_Struct newTime, GM_Time_Spec_Struct oldTime)
{
    GM_RSSP1_UINT32 temp = 0U;

#ifdef VXWORKS
	GM_RSSP1_UINT32 tickps = 0U;/*sysClkRateGet()值由sysClkRateSet()决定，一般60，不要大于1000; 每秒包含的tick数*/
	GM_RSSP1_UINT32 tickcoutdif = 0U; /*新时间-旧时间的 tick数差*/
	GM_RSSP1_UINT32 tick_remain = 0U; /*少于1s内的tick 数*/

	tickps = (GM_RSSP1_UINT32)sysClkRateGet();

	/*VXWORKS下 对于TICK数是有最大值的，当在最大值附近时， newTime包含的TICK数值就会小于oldTime包含的TICK数值*/
	if (newTime.tv_nsec < oldTime.tv_nsec)
	{
		tickcoutdif = GM_UINT32_MAX - oldTime.tv_nsec + newTime.tv_nsec;
	}
	else
	{
		tickcoutdif = (GM_RSSP1_UINT32)(newTime.tv_nsec - oldTime.tv_nsec);
	}

	/*如果tick数差 / tick数每秒 = 时间(s) 超过 可表示最大毫秒数/1000  */
	if (tickcoutdif/ tickps > GM_UINT32_MAX / 1000U)
	{
		 temp = GM_UINT32_MAX;/*返回可表示的最大毫秒数 32MAX*/
	}
	else
	{
		/*tick数差 / tick数每秒 = 秒数 再×1000 = 毫秒数*/
		temp = tickcoutdif / tickps * 1000U;
		tick_remain = tickcoutdif % tickps;
		/*处理tick数差 中 不足1秒的部分*/
		if (tick_remain != (GM_RSSP1_UINT32)0U)
		{
			if (GM_UINT32_MAX - temp <= tick_remain * 1000U / tickps)
			{
				temp = GM_UINT32_MAX;
			}
			else
			{
				temp += tick_remain * 1000U / tickps;
			}
		}
	}
#else/*其它操作系统*/

	if ((GM_RSSP1_UINT32)(newTime.tv_sec - oldTime.tv_sec) > (GM_UINT32_MAX / 1000U))
	{/*当秒数差超出最大范围*/
		temp = GM_UINT32_MAX;
	}
	else
	{/*秒数差满足范围*/
		if (newTime.tv_nsec > oldTime.tv_nsec)
		{/*纳秒数 是新时间更大时，也可能超出范围*/
			if ((GM_UINT32_MAX - (GM_RSSP1_UINT32)(newTime.tv_sec - oldTime.tv_sec) * 1000U) < ((GM_RSSP1_UINT32)(newTime.tv_nsec - oldTime.tv_nsec)) / 1000000U)
			{
				temp = GM_UINT32_MAX;
				return temp;
			}
		}
		temp = (GM_RSSP1_UINT32)((long)(newTime.tv_sec - oldTime.tv_sec) * 1000 + (newTime.tv_nsec - oldTime.tv_nsec)/ 1000000);
	}

#endif

    return temp;
}

/*需兼容操作系统API的函数*/
time_t GM_Time_OS_Get(struct tm* dayTime)
{
#ifndef VXWORKS
    time_t now_time = time(NULL);
    *dayTime = *localtime(&now_time); /*将日历时间转换为本地时间*/
    return now_time;
#else/*其它操作系统*/
    GM_Time_Spec_Struct ttt={0};

	GM_RSSP1_memset((void*)(&ttt), (GM_RSSP1_UINT8)0U, (size_t)(sizeof(GM_Time_Spec_Struct)));

	clock_gettime(CLOCK_REALTIME, &ttt);

    *dayTime = *localtime(&ttt.tv_sec);
    return mktime(dayTime);
#endif
}
/*需兼容操作系统API的函数*/
GM_RSSP1_BOOL GM_Time_OS_Set(struct tm* dayTime)
{
    GM_RSSP1_BOOL rt = GM_RSSP1_FALSE;

#ifdef WIN32
    SYSTEMTIME st;
	GM_RSSP1_memset((void*)(&st), (GM_RSSP1_UINT8)0U, (size_t)(sizeof(SYSTEMTIME)));
	if (NULL == dayTime)
	{
		GM_RSSP1_Log_Usr(GM_TIME_OS_SET_WIN32_PAR_ERR, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
    st.wYear         = dayTime->tm_year + 1900;
    st.wMonth        = dayTime->tm_mon + 1;
    st.wDay          = dayTime->tm_mday;
    st.wHour         = dayTime->tm_hour;
    st.wMinute       = dayTime->tm_min;
    st.wSecond       = dayTime->tm_sec;
    st.wMilliseconds = 0;

    rt = SetLocalTime(&st);

    if (GM_RSSP1_TRUE == rt)
    {
        return GM_RSSP1_TRUE;
    }
    else
    {
       /* GM_RSSP1_Log_Msg(0,"Set system time error %d\n",
						(int)GM_Get_Error_No(),0,0,0,0,0);*/
		GM_RSSP1_Log_Usr(GM_TIME_OS_SET_WIN32_SETLOCALTIME_ERR, 0, 0, 0, 0, 0, 0);
        return GM_RSSP1_FALSE;
    }
#endif
#ifdef QNX
    struct timeval time_set;
    time_t tmp = 0;

    if (NULL == dayTime)
    {
		GM_RSSP1_Log_Usr(GM_TIME_OS_SET_QNX_PAR_ERR, 0, 0, 0, 0, 0, 0);
        return rt;
    }
    memset(&time_set,0,sizeof(time_set));
    tmp = mktime(dayTime);

    if(tmp == (time_t)(-1))
    {
		GM_RSSP1_Log_Usr(GM_TIME_OS_SET_QNX_MKTIME_ERR, 0, 0, 0, 0, 0, 0);
    	return GM_RSSP1_FALSE;
    }
    time_set.tv_sec = tmp;
    if (0 > settimeofday(&time_set, NULL))
    {
		GM_RSSP1_Log_Usr(GM_TIME_OS_SET_QNX_SETTIMEOFDAY_ERR, 0, 0, 0, 0, 0, 0);
        return GM_RSSP1_FALSE;
    }
    else
    {
    	return GM_RSSP1_TRUE;
    }
#endif
#ifdef VXWORKS
	GM_Time_Spec_Struct tset={0};

	GM_RSSP1_memset((void*)(&tset), (GM_RSSP1_UINT8)0U, (size_t)(sizeof(GM_Time_Spec_Struct)));
	if (NULL == dayTime)
	{
		logMsg("daytime is null\n",0,0,0,0,0,0);
		GM_RSSP1_Log_Usr(GM_TIME_OS_SET_VXWORKS_PAR_ERR, 0, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	tset.tv_nsec = 0;
	tset.tv_sec = mktime(dayTime);

    rt = clock_settime(CLOCK_REALTIME, &tset);

    if (0 == rt)
    {
        return GM_RSSP1_TRUE;
    }
    else
    {
		logMsg("SET OS TIME ERROR\n",0,0,0,0,0,0);
		GM_RSSP1_Log_Usr(GM_TIME_OS_SET_VXWORKS_CLOCKSETTIME_ERR, 0, 0, 0, 0, 0, 0);
        return GM_RSSP1_FALSE;
    }

#endif
}

/**
* @brief GM_Time_Microseconds_2_NTP_Fraction
* 入参为x 返回值为 y， 则x/10^6 = y/2^32, y = 4294*x + 1981*x / 2^11
* 微秒转换为NTP时间.  y是比微秒更 小的 tick单位。2^32 tick = 1s 1tick = 0.23ns.
* @param[in] x 微秒表示的时间.
* @return GM_RSSP1_UINT32 NTP时间
*/
static GM_RSSP1_UINT32 GM_Time_Microseconds_2_NTP_Fraction(GM_RSSP1_UINT32 x)
{
    if (x > (GM_RSSP1_UINT32)1000000u)
    {
        x = x % (GM_RSSP1_UINT32)1000000;
    }

    return (GM_RSSP1_UINT32)(4294U * x + ((1981U * x) >> 11U));
}
/*需兼容操作系统API的函数*/
GM_RSSP1_BOOL GM_Time_Get_UTC(GM_RSSP1_UINT32 *secondsFrom1900, GM_RSSP1_UINT32 *fraction)
{
#ifdef WIN32
#if _MSC_VER > 1200

    struct __timeb32 n_time_now={0};

	GM_RSSP1_memset((void*)(&n_time_now), (GM_RSSP1_UINT8)0U, (size_t)(sizeof(n_time_now)));

	if ((NULL == secondsFrom1900) || (NULL == fraction))
    {
		GM_RSSP1_Log_Usr(GM_TIME_GET_UTC_WIN32_PAR_ERR, 0, 0, 0, 0, 0, 0);
        return GM_RSSP1_FALSE;
    }

    _ftime32(&n_time_now);

    *secondsFrom1900 = UTC_SNTP_TIME(n_time_now.time);
    *fraction = GM_Time_Microseconds_2_NTP_Fraction((GM_RSSP1_UINT32)(1000 * n_time_now.millitm));
#else
    time_t n_time_now = 0;

    if ((NULL == secondsFrom1900) || (NULL == fraction))
    {
		GM_RSSP1_Log_Usr(GM_TIME_GET_UTC_WIN32_PAR_ERR, 0, 0, 0, 0, 0, 0);
        return GM_RSSP1_FALSE;
    }

    n_time_now = time(NULL);

    *secondsFrom1900 = UTC_SNTP_TIME(n_time_now);
    *fraction = (GM_RSSP1_UINT32)0U;
#endif
#else/*其它操作系统*/
    GM_Time_Spec_Struct DateTime={0};

	GM_RSSP1_memset((void*)(&DateTime), (GM_RSSP1_UINT8)0U, (size_t)(sizeof(GM_Time_Spec_Struct)));
    if ((NULL == secondsFrom1900) || (NULL == fraction))
    {
		GM_RSSP1_Log_Usr(GM_TIME_GET_UTC_OTHER_PAR_ERR, 0, 0, 0, 0, 0, 0);
        return GM_RSSP1_FALSE;
    }

    clock_gettime(CLOCK_REALTIME, &DateTime);    /*  gcc -lrt  */

    DateTime.tv_sec = UTC_SNTP_TIME(DateTime.tv_sec);
    *secondsFrom1900 = (GM_RSSP1_UINT32)DateTime.tv_sec ;
    *fraction = (GM_RSSP1_UINT32)DateTime.tv_nsec ;    /* nanoseconds :ns = 1/1000000000 s*/
#endif

    return GM_RSSP1_TRUE;
}

