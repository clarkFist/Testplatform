/**
* @file GM_MultiThread.c
* @brief 实现了跨平台多线程、互斥、信号功能.Linux下的线程优先级问题需要考虑
* @author Hao Liming
* @date 2010-3-16 12:51:42
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: </b>haoliming@casco.com.cn</pre>
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/
#include "GM_MultiThread.h"
#include "GM_Error.h"

#ifdef WIN32
GM_RSSP1_BOOL GM_Thread_Create(GM_THREAD* thread, GM_THREAD_FUNC myfunc, const char* threadName, int stackSize, int guardSize, int priority, const GM_Thread_Arg_Struct* pargs)
{
	GM_THREAD tmpThread = 0u;
	GM_RSSP1_BOOL rt          = GM_RSSP1_FALSE;
	unsigned int ThrAddr = 0u;

	if (stackSize < 0)
	{
		GM_RSSP1_Log_Usr(GM_THREAD_CREATE_WIN32_STACKSIZE_ERR, (int)stackSize, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	if ((priority != THREAD_PRIORITY_HIGHEST)&&(priority != THREAD_PRIORITY_ABOVE_NORMAL)
		&&(priority != THREAD_PRIORITY_NORMAL)&&(priority != THREAD_PRIORITY_BELOW_NORMAL)
		&&(priority != THREAD_PRIORITY_LOWEST)&&(priority != THREAD_PRIORITY_IDLE))
	{
		GM_RSSP1_Log_Usr(GM_THREAD_CREATE_WIN32_PRIORITY_ERR, (int)priority, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	tmpThread = (GM_THREAD)_beginthreadex(NULL, stackSize, myfunc, pargs, 0, &ThrAddr);

	if (tmpThread > 0u)
	{
		rt = SetThreadPriority(tmpThread, priority);

		if ((rt == GM_RSSP1_TRUE)&&(thread != NULL))
		{
			*thread = tmpThread;
		}
		if (GM_RSSP1_FALSE == rt)
		{
			GM_RSSP1_Log_Usr(GM_THREAD_CREATE_WIN32_SETPRIORITY_ERR, (int)tmpThread, 0, 0, 0, 0, 0);
		}
		return rt;
	}
	else
	{
		GM_RSSP1_Log_Usr(GM_THREAD_CREATE_WIN32_BEGINTHREADEX_ERR, (int)tmpThread, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
}

GM_THREAD GM_Thread_Self(void)
{
	GM_THREAD td = GetCurrentThreadId();

	return td;
}
#endif
#ifdef QNX
GM_RSSP1_BOOL GM_Thread_Create(GM_THREAD* thread, GM_THREAD_FUNC myfunc, const char* threadName, int stackSize, int guardSize, int priority, const GM_Thread_Arg_Struct* args)
{
	GM_THREAD tmpThread = 0;
	GM_THREAD rt        = 0;
	GM_RSSP1_UINT32 pageSize	= 0U;
	pthread_attr_t attr = { 0 };
	struct sched_param params;
	/*优先级63为最高，1为最低*/
	if ((priority > GM_TASK_PRIORITY_63) || (priority < GM_TASK_PRIORITY_1) || (stackSize <= 0))
	{
		GM_RSSP1_Log_Usr(GM_THREAD_CREATE_QNX_PAR_ERR, priority, stackSize, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	/*start: GM151 add qnx stack guard*/
	pageSize = sysconf(_SC_PAGESIZE);
	if ((guardSize < 0) || ((guardSize % (int)pageSize) != 0))
	{
		GM_RSSP1_Log_Usr(GM_THREAD_CREATE_QNX_PAR_GUARDSIZE_ERR, guardSize, (int)pageSize, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	/*end: GM151 add qnx stack guard*/
	pthread_attr_init(&attr);

	rt = pthread_attr_setinheritsched(&attr,PTHREAD_EXPLICIT_SCHED);
	if(rt != 0)
	{
		GM_RSSP1_Log_Usr(GM_THREAD_CREATE_QNX_SETINHERITSCHED_ERR, (int)rt, 0, 0, 0, 0, 0);/*CR1871*/
		return GM_RSSP1_FALSE;
	}
	params.sched_priority = priority;
	rt = pthread_attr_setschedparam(&attr, &params);
	if(rt != 0)
	{
		GM_RSSP1_Log_Usr(GM_THREAD_CREATE_QNX_SETSCHEDPARAM_ERR, (int)rt, 0, 0, 0, 0, 0);/*CR1871*/
		return GM_RSSP1_FALSE;
	}

	rt = pthread_attr_setstacksize(&attr,(size_t)stackSize );
	if(rt != 0)
	{
		GM_RSSP1_Log_Usr(GM_THREAD_CREATE_QNX_SETSTACKSIZE_ERR, stackSize, (int)rt, 0, 0, 0, 0);/*CR1871*/
		return GM_RSSP1_FALSE;
	}

	/*start: GM151 add qnx stack guard*/
	rt = pthread_attr_setguardsize(&attr,(size_t)guardSize);
	if(rt != 0)
	{
		GM_RSSP1_Log_Usr(GM_THREAD_CREATE_QNX_SETGUARDSIZE_ERR, guardSize, (int)rt, 0, 0, 0, 0);/*CR1871*/
		return GM_RSSP1_FALSE;
	}
	/*end: GM151 add qnx stack guard*/

	if (NULL == thread)
	{
		rt = pthread_create(&tmpThread, &attr, myfunc, args);
	}
	else
	{
		rt = pthread_create(thread, &attr, myfunc, args);
	}

	if (0 == rt)
	{
		return GM_RSSP1_TRUE;
	}
	else
	{
		GM_RSSP1_Log_Usr(GM_THREAD_CREATE_QNX_PTHREADCREATE_ERR, (int)rt, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
}

GM_THREAD GM_Thread_Self(void)
{
	GM_THREAD rt = pthread_self();

	return rt;
}
#endif 
#ifdef VXWORKS
GM_RSSP1_BOOL GM_Thread_Create(GM_THREAD* thread, GM_THREAD_FUNC myfunc, const char* threadName, int stackSize, int guardSize, int priority, const GM_Thread_Arg_Struct * pargs)
{/*传入的参数指针 不能赋值给局部变量*/
	GM_THREAD tmpThread = 0;

	/*GM_TASK_PRIORITY_99为最低优先级，99； GM_TASK_PRIORITY_0为最高优先级，0*/
	if ((priority > GM_TASK_PRIORITY_99) || (priority < GM_TASK_PRIORITY_0) || (stackSize <= 0))
	{
		GM_RSSP1_Log_Usr(GM_THREAD_CREATE_VXWORKS_PAR_ERR, priority, stackSize, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	/*GM_RSSP1_Log_Msg(0, "参数地址：[%d] %d %d %d",pargs, pargs->args1, pargs->args2, pargs->args3,0,0);*/
	if (NULL != pargs)
	{
		tmpThread = taskSpawn(threadName, priority, VX_FP_TASK, stackSize, (FUNCPTR)myfunc,
			pargs->args0, pargs->args1, pargs->args2, pargs->args3, pargs->args4, 
			pargs->args5, pargs->args6, pargs->args7, pargs->args8, pargs->args9);
	}
	else
	{
		tmpThread = taskSpawn(threadName, priority, VX_FP_TASK, stackSize, (FUNCPTR)myfunc,
			0, 0, 0, 0, 0,0, 0, 0, 0,0);	
	}

	if (thread != NULL)
	{/*给指针赋值，修改0223bug，*/
		*thread = tmpThread;
	}

	if (ERROR == tmpThread)
	{
		GM_RSSP1_Log_Usr(GM_THREAD_CREATE_VXWORKS_TASKSPAWN_ERR, (int)tmpThread, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	else
	{
		return GM_RSSP1_TRUE;
	}
}

GM_THREAD GM_Thread_Self(void)
{
	GM_THREAD td = taskIdSelf();

	return td;
}
#endif


