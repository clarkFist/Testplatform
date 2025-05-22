/**
* @file GM_Condition.h
* @brief
* @author Hao Liming
* @date 2010-11-26 22:26:22
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: </b>haoliming@casco.com.cn</pre>
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/
#ifndef _GM_CONDITION_H
#define _GM_CONDITION_H

//#include "GM_Mutex.h"
//#include "GM_Malloc.h"
//#include "GM_Free.h"
#include "../common\GM_RSSP1_Mutex.h"

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
    typedef HANDLE          GM_CONDITION;
#endif
#ifdef QNX
    typedef pthread_cond_t*  GM_CONDITION;
#endif
#ifdef VXWORKS
    typedef SEM_ID          GM_CONDITION;
#endif

    /**
    * @brief GM_Condition_Init
    *
    * 初始化Condition.
    * @param[out] cond
    * @return GM_RSSP1_BOOL
    */
    GM_RSSP1_BOOL GM_Condition_Init(GM_CONDITION* pCond);

    /**
    * @brief GM_Condition_Destroy
    *
    * 销毁Condition.
    * @param[in] cond
    * @return GM_RSSP1_BOOL
    */
    GM_RSSP1_BOOL GM_Condition_Destroy(GM_CONDITION cond);

    /**
    * @brief GM_Condition_Wait
    *
    * 等待Condition.
    * @param[in] cond
    * @param[in] mutex
    * @return GM_RSSP1_BOOL
    */
    GM_RSSP1_BOOL GM_Condition_Wait(GM_CONDITION cond, GM_RSSP1_MUTEX mutex);

    /**
    * @brief GM_Condition_Signal
    *
    * 通知Condition.
    * @param[in] cond
    * @return GM_RSSP1_BOOL
    */
    GM_RSSP1_BOOL GM_Condition_Signal(GM_CONDITION cond);

#ifdef __cplusplus
}

#endif /**< __cplusplus */

#endif /**< _GM_CONDITION_H */

