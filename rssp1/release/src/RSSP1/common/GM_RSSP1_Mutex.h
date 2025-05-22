/**
* @file GM_RSSP1_Mutex.h
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
#ifndef _GM_RSSP1_MUTEX_H
#define _GM_RSSP1_MUTEX_H

#include "../GM_RSSP1_Utils_Base.h"
#include "GM_RSSP1_Memory.h"

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
	typedef HANDLE          GM_RSSP1_MUTEX;
#endif
#ifdef QNX
	typedef pthread_mutex_t* GM_RSSP1_MUTEX;
#endif
#ifdef VXWORKS
	typedef SEM_ID          GM_RSSP1_MUTEX;
#endif

    /**
    * @brief GM_RSSP1_Mutex_Init
    *
    * ³õÊ¼»¯»¥³âÆ÷.
    * @param[out] mutex
    * @return GM_RSSP1_BOOL
    */
    GM_RSSP1_BOOL GM_RSSP1_Mutex_Init(GM_RSSP1_MUTEX* pMutex);

    /**
    * @brief GM_RSSP1_Mutex_Lock
    *
    * »ñµÃ»¥³âÆ÷×ÊÔ´.
    * @param[in] mutex
    * @return GM_RSSP1_BOOL
    */
    GM_RSSP1_BOOL GM_RSSP1_Mutex_Lock(GM_RSSP1_MUTEX mutex);

    /**
    * @brief GM_RSSP1_Mutex_Unlock
    *
    * ÊÍ·Å»¥³âÆ÷.
    * @param[in] mutex
    * @return GM_RSSP1_BOOL
    */
    GM_RSSP1_BOOL GM_RSSP1_Mutex_Unlock(GM_RSSP1_MUTEX mutex);

#ifdef __cplusplus
}

#endif /**< __cplusplus */

#endif /**< _GM_RSSP1_MUTEX_H */
