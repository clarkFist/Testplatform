/**
* @file GM_MultiThread.h
* @brief 实现了跨平台(Windows, Linux, VxWorks, ReWorks)线程、互斥、信号，sleep
* @author Hao Liming
* @date 2010-3-16 12:49:30
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: </b>haoliming@casco.com.cn</pre>
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/

#ifndef _GM_MULTITHREAD_H
#define _GM_MULTITHREAD_H

#include "GM_MultiThread_Priority.h"



#ifdef WIN32
#include <process.h>
#endif

#ifdef VXWORKS
#include <taskLib.h>
#endif

#ifdef QNX
#include <pthread.h>
#include <unistd.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif /**< __cplusplus */

#ifdef WIN32
    typedef unsigned        GM_THREAD;
    typedef unsigned        GM_THREAD_FUNC_RETURN;
    typedef GM_THREAD_FUNC_RETURN(__stdcall* GM_THREAD_FUNC)(void* args);
#define GM_THREAD_CALL __stdcall
#endif
#ifdef QNX
    typedef pthread_t       GM_THREAD;
    typedef void*           GM_THREAD_FUNC_RETURN;
    typedef GM_THREAD_FUNC_RETURN(* GM_THREAD_FUNC)(void* args);
#define GM_THREAD_CALL
#endif
#ifdef VXWORKS
    typedef int             GM_THREAD;
    typedef int             GM_THREAD_FUNC_RETURN;
    typedef FUNCPTR         GM_THREAD_FUNC;
#define GM_THREAD_CALL
#endif

#define GM_THREAD_RETRUN static GM_THREAD_FUNC_RETURN GM_THREAD_CALL

	/** 新任务属性结构体  */
	typedef struct
	{
		int	priority;	/*新线程优先级*/
		int	StackSize;	/*堆栈大小*/
		GM_THREAD	Thread;	/*新建线程句柄*/
	}GM_RSSP1_PACKED GM_Thread_Property_Struct;

    /** 任务入口参数结构体  */
    typedef struct
    {
        int args0;
        int args1;
        int args2;
        int args3;
        int args4;
        int args5;
        int args6;
        int args7;
        int args8;
        int args9;
    }GM_RSSP1_PACKED GM_Thread_Arg_Struct;

    /**
    * @brief GM_Thread_Create
    *
    * @param[out] thread 线程句柄(若为NULL则不输出线程句柄)
    * @param[in] myfunc 线程函数
    * @param[in] threadName 线程名(仅VxWorks用到)
    * @param[in] stackSize 线程堆栈大小(VxWorks和Windows可用)
	* @param[in] guardSize 栈溢出保护区大小(仅QNX用到，且数值必须为页大小的整数倍或0)
    * @param[in] priority 优先级(VxWorks和Windows可用) GM_TASK_PRIORITY_0 到 GM_TASK_PRIORITY_99
    * @param[in] args 参数
    * @return GM_RSSP1_BOOL
    */
    GM_RSSP1_BOOL GM_Thread_Create(GM_THREAD* thread, GM_THREAD_FUNC myfunc, const char* threadName, int stackSize, int guardSize, int priority, const GM_Thread_Arg_Struct* pArgs);

    /**
    * @brief GM_Thread_Self
    *
    * @return GM_THREAD
    */
    GM_THREAD GM_Thread_Self(void);

#ifdef __cplusplus
}

#endif /**< __cplusplus */

#endif /**< _GM_MULTITHREAD_H */

