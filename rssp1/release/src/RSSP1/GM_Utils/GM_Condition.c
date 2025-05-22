/**
* @file GM_Condition.c
* @brief
* @author Hao Liming
* @date 2010-11-26 22:26:40
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: </b>haoliming@casco.com.cn</pre>
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/
#include "GM_Condition.h"
#ifdef WIN32
GM_RSSP1_BOOL GM_Condition_Init(GM_CONDITION* pCond)
{
    if (NULL == pCond)
    {
        return GM_RSSP1_FALSE;
    }

    *pCond = CreateEvent(NULL, GM_RSSP1_FALSE, GM_RSSP1_FALSE, NULL);

    if (NULL == (*pCond))
    {
        return GM_RSSP1_FALSE;
    }
    else
    {
        return GM_RSSP1_TRUE;
    }
}

GM_RSSP1_BOOL GM_Condition_Destroy(GM_CONDITION cond)
{
    int rt = 0;

    rt = CloseHandle(cond);

    if (0 == rt)
    {
        return GM_RSSP1_FALSE;
    }
    else
    {
        return GM_RSSP1_TRUE;
    }
}

GM_RSSP1_BOOL GM_Condition_Signal(GM_CONDITION cond)
{
    int rt = 0;

    rt = SetEvent(cond);

    if (0 == rt)
    {
        return GM_RSSP1_FALSE;
    }
    else
    {
        return GM_RSSP1_TRUE;
    }
}

GM_RSSP1_BOOL GM_Condition_Wait(GM_CONDITION cond, GM_RSSP1_MUTEX mutex)
{
    GM_RSSP1_BOOL rt = GM_RSSP1_TRUE;

    /*rt = GM_Mutex_Unlock(mutex);*/

    /*if (GM_RSSP1_FALSE == rt)
    {
        return GM_RSSP1_FALSE;
    }windows下面并不需要 mutex*/

    rt = WaitForSingleObject(cond, INFINITE);

    if (WAIT_OBJECT_0 != rt)
    {
        return GM_RSSP1_FALSE;
    }
    else
    {
    	return GM_RSSP1_TRUE;
    }

    /*rt = GM_Mutex_Lock(mutex);*/
}
#endif
#ifdef QNX
GM_RSSP1_BOOL GM_Condition_Init(GM_CONDITION* pCond)
{
    int rt = 0;

    if (NULL == pCond)
    {
        return GM_RSSP1_FALSE;
    }
    *pCond = (pthread_cond_t*)GM_RSSP1_malloc(sizeof(pthread_cond_t));
    rt = pthread_cond_init(*pCond, NULL);

    if (0 == rt)
    {
        return GM_RSSP1_TRUE;
    }
    else
    {
        return GM_RSSP1_FALSE;
    }
}

GM_RSSP1_BOOL GM_Condition_Destroy(GM_CONDITION cond)
{
    int rt = 0;

    rt = pthread_cond_destroy(cond);
    GM_RSSP1_free((void*)cond);/*规则检查增加（void*）*/
    if (0 == rt)
    {
        return GM_RSSP1_TRUE;
    }
    else
    {
        return GM_RSSP1_FALSE;
    }
}

GM_RSSP1_BOOL GM_Condition_Signal(GM_CONDITION cond)
{
    int rt = 0;

    rt = pthread_cond_signal(cond);
    if (0 == rt)
    {
        return GM_RSSP1_TRUE;
    }
    else
    {
        return GM_RSSP1_FALSE;
    }
}

GM_RSSP1_BOOL GM_Condition_Wait(GM_CONDITION cond, GM_RSSP1_MUTEX mutex)
{
    int rt = 0;
   
    rt = pthread_cond_wait(cond, mutex);
  
    if (0 == rt)
    {
        return GM_RSSP1_TRUE;
    }
    else
    {
        return GM_RSSP1_FALSE;
    }
}
#endif
#ifdef VXWORKS
GM_RSSP1_BOOL GM_Condition_Init(GM_CONDITION* pCond)
{
    if (NULL == pCond)
    {
        return GM_RSSP1_FALSE;
    }

    *pCond = semBCreate(SEM_Q_FIFO, SEM_EMPTY);

    if (NULL == (*pCond))
    {
        return GM_RSSP1_FALSE;
    }
    else
    {
        return GM_RSSP1_TRUE;
    }
}

GM_RSSP1_BOOL GM_Condition_Destroy(GM_CONDITION cond)
{
    STATUS rt = OK;

    rt = semTake(cond, WAIT_FOREVER);

    if (OK == rt)
    {
        semDelete(cond);

        return GM_RSSP1_TRUE;
    }
    else
    {
        return GM_RSSP1_FALSE;
    }
}

GM_RSSP1_BOOL GM_Condition_Signal(GM_CONDITION cond)
{
    STATUS rt = OK;

    if (NULL == cond)
    {
        return GM_RSSP1_FALSE;
    }

    rt = semGive(cond);

    if (OK == rt)
    {
        return GM_RSSP1_TRUE;
    }
    else
    {
        return GM_RSSP1_FALSE;
    }
}

GM_RSSP1_BOOL GM_Condition_Wait(GM_CONDITION cond, GM_RSSP1_MUTEX mutex)
{
    STATUS rt = OK;

    /*rt = GM_Mutex_Unlock(mutex);

    if (GM_RSSP1_FALSE == rt)
    {
        return GM_RSSP1_FALSE;
    } VXWORKS下面并不需要 mutex*/

    rt = semTake(cond, WAIT_FOREVER);

    if (OK == rt)
    {
        /*rt =  GM_Mutex_Lock(mutex);*/

        return GM_RSSP1_TRUE;
    }
    else
    {
        return GM_RSSP1_FALSE;
    }
}

#endif
