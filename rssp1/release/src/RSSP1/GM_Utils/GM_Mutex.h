/**
* @file GM_Mutex.h
* @brief
* @author Hao Liming
* @date 2010-11-26 22:25:31
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: </b>haoliming@casco.com.cn</pre>
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/
#ifndef _GM_MUTEX_H
#define _GM_MUTEX_H

#include "GM_Utils_Base.h"
#include "GM_Malloc.h"
#include "GM_Free.h"

#ifdef VXWORKS
#include <semLib.h>
#endif

#ifdef QNX
#include <pthread.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif /**< __cplusplus */

#ifdef WIN32
	typedef HANDLE          GM_MUTEX;
#endif
#ifdef QNX
	typedef pthread_mutex_t* GM_MUTEX;
#endif
#ifdef VXWORKS
	typedef SEM_ID          GM_MUTEX;
#endif

    /**
    * @brief GM_Mutex_Init
    *
    * ��ʼ��������.
    * @param[out] mutex
    * @return GM_BOOL
    */
    GM_BOOL GM_Mutex_Init(GM_MUTEX* pMutex);

    /**
    * @brief GM_Mutex_Destroy
    *
    * ���ٻ�����.
    * @param[in] mutex
    * @return GM_BOOL
    */
    GM_BOOL GM_Mutex_Destroy(GM_MUTEX mutex);

    /**
    * @brief GM_Mutex_Lock
    *
    * ��û�������Դ.
    * @param[in] mutex
    * @return GM_BOOL
    */
    GM_BOOL GM_Mutex_Lock(GM_MUTEX mutex);

    /**
    * @brief GM_Mutex_Unlock
    *
    * �ͷŻ�����.
    * @param[in] mutex
    * @return GM_BOOL
    */
    GM_BOOL GM_Mutex_Unlock(GM_MUTEX mutex);

#ifdef __cplusplus
}

#endif /**< __cplusplus */

#endif /**< _GM_MUTEX_H */
