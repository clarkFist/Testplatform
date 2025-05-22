/**
* @file GM_MQ.c
* @brief
* @author Hao Liming
* @date 2010-4-17 12:57:43
* @version
* @modify by:Xavier
* @modify info:2013-7-29
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: </b>haoliming@casco.com.cn</pre>
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/
#include "GM_MQ.h"

#define GM_MQ_MSG_NUM_MAX 2048
#define GM_MQ_MSG_SIZE_MAX 10000

GM_BOOL GM_MQ_Read(GM_MQ_Struct* pMQ, GM_UINT8* pOutBuf, GM_UINT16* pSize)
{
    GM_BOOL rt = GM_FALSE;
    int temp = 0;

    if ((NULL == pMQ) || (NULL == pOutBuf) || (NULL == pSize)
        || (GM_TRUE != pMQ->initialized))
    {
        return GM_FALSE;
    }
	/*关于msgNum， front的读取和判断 调整到lock mutex之后。 CR1409*/
	rt = GM_Mutex_Lock(pMQ->mutex);

	if (GM_FALSE == rt)
	{
		return GM_FALSE;
	}	
	
	if ((GM_UINT16)0u == pMQ->msgNum)
	{
		GM_Mutex_Unlock(pMQ->mutex);
		return GM_FALSE;
	}

    temp = (pMQ->front + 1) % (pMQ->maxMsgNum);

    if (pMQ->pMQMsg[temp].msgSize > (*pSize))
    {
		GM_Mutex_Unlock(pMQ->mutex);
		return GM_FALSE;    
	}
    else
    {

        pMQ->front = temp;
        --(pMQ->msgNum);

        GM_memcpy((void*)pOutBuf, ((const void*)(pMQ->pMQMsg[pMQ->front].pMsg)), ((size_t)(pMQ->pMQMsg[pMQ->front].msgSize)));
        *pSize = pMQ->pMQMsg[pMQ->front].msgSize;

		/*增加清空已读消息内容的接口*/
		GM_memset(((void*)(pMQ->pMQMsg[pMQ->front].pMsg)),(GM_UINT8)0U, ((size_t)(pMQ->maxMsgSize)));
		pMQ->pMQMsg[pMQ->front].msgSize = 0U;

		rt = GM_Mutex_Unlock(pMQ->mutex);
    }

    return rt;
}

GM_UINT32 GM_MQ_Read_Result(GM_MQ_Struct* pMQ, GM_UINT8* pOutBuf, GM_UINT16* pSize)
{
	GM_BOOL rt = GM_FALSE;
	int temp = 0;

	if ((NULL == pMQ) || (NULL == pOutBuf) || (NULL == pSize))
	{
		return MQ_RD_PAR_NULL;
	}

	if (GM_TRUE != pMQ->initialized)
	{
		return MQ_RD_INIT_ERR;
	}
	
	/*关于msgNum， front的读取和判断 调整到lock mutex之后。 CR1409*/
	rt = GM_Mutex_Lock(pMQ->mutex);

	if (GM_FALSE == rt)
	{
		return MQ_RD_LOCK_ERR;
	}	

	if ((GM_UINT16)0u == pMQ->msgNum)
	{
		GM_Mutex_Unlock(pMQ->mutex);
		return MQ_RD_MQ_EMPTY;
	}

	temp = (pMQ->front + 1) % (pMQ->maxMsgNum);

	if (pMQ->pMQMsg[temp].msgSize > (*pSize))
	{
		GM_Mutex_Unlock(pMQ->mutex);
		return MQ_RD_MSGSIZE_ERR;    
	}
	else
	{

		pMQ->front = temp;
		--(pMQ->msgNum);

		GM_memcpy((void*)pOutBuf, ((const void*)(pMQ->pMQMsg[pMQ->front].pMsg)), ((size_t)(pMQ->pMQMsg[pMQ->front].msgSize)));
		*pSize = pMQ->pMQMsg[pMQ->front].msgSize;

		/*增加清空已读消息内容的接口*/
		GM_memset(((void*)(pMQ->pMQMsg[pMQ->front].pMsg)),(GM_UINT8)0U, ((size_t)(pMQ->maxMsgSize)));
		pMQ->pMQMsg[pMQ->front].msgSize = 0U;

		rt = GM_Mutex_Unlock(pMQ->mutex);
		if (GM_FALSE == rt)
		{
			return MQ_RD_UNLOCK_ERR;
		}
	}

	return MQ_RD_OK;
}

GM_BOOL GM_MQ_Get(GM_MQ_Struct* pMQ, GM_UINT16 index, GM_UINT8* pOutBuf, GM_UINT16* pSize)
{
    GM_BOOL rt = GM_FALSE;

    if ((NULL == pMQ) || (NULL == pOutBuf) || (NULL == pSize)
        || (GM_TRUE != pMQ->initialized))
    {
        return GM_FALSE;
    }

	/*关于msgNum， front的读取和判断 调整到lock mutex之后。 CR1409*/
	rt = GM_Mutex_Lock(pMQ->mutex);
	if (GM_FALSE == rt)
	{
		return GM_FALSE;
	}

	if (index >= pMQ->msgNum)
	{
		GM_Mutex_Unlock(pMQ->mutex);
		return GM_FALSE;
	}

	/*如果MQ中的消息 msgSize 大于 传入buff的大小pSize */
    if (pMQ->pMQMsg[(pMQ->front + 1 + index) % pMQ->maxMsgNum].msgSize > *pSize)
    {
		GM_Mutex_Unlock(pMQ->mutex);
		return GM_FALSE;
    }
    else
    {
        GM_memcpy((void*)pOutBuf, ((const void*)(pMQ->pMQMsg[(pMQ->front + 1 + index) % pMQ->maxMsgNum].pMsg)),
                  ((size_t)(pMQ->pMQMsg[(pMQ->front + 1 + index) % pMQ->maxMsgNum].msgSize)));
        *pSize = pMQ->pMQMsg[(pMQ->front + 1 + index) % pMQ->maxMsgNum].msgSize;

		rt = GM_Mutex_Unlock(pMQ->mutex);

    }

    return rt;
}

GM_UINT32 GM_MQ_Get_Result(GM_MQ_Struct* pMQ, GM_UINT16 index, GM_UINT8* pOutBuf, GM_UINT16* pSize)
{
	GM_BOOL rt = GM_FALSE;

	if ((NULL == pMQ) || (NULL == pOutBuf) || (NULL == pSize))
	{
		return MQ_GET_PAR_NULL;
	}

	if (GM_TRUE != pMQ->initialized)
	{
		return MQ_GET_INIT_ERR;
	}

	/*关于msgNum， front的读取和判断 调整到lock mutex之后。 CR1409*/
	rt = GM_Mutex_Lock(pMQ->mutex);
	if (GM_FALSE == rt)
	{
		return MQ_GET_LOCK_ERR;
	}

	if (index >= pMQ->msgNum)
	{
		GM_Mutex_Unlock(pMQ->mutex);
		return MQ_GET_INDEX_ERR;
	}

	/*如果MQ中的消息 msgSize 大于 传入buff的大小pSize */
	if (pMQ->pMQMsg[(pMQ->front + 1 + index) % pMQ->maxMsgNum].msgSize > *pSize)
	{
		GM_Mutex_Unlock(pMQ->mutex);
		return MQ_GET_MSGSIZE_ERR;
	}
	else
	{
		GM_memcpy((void*)pOutBuf, ((const void*)(pMQ->pMQMsg[(pMQ->front + 1 + index) % pMQ->maxMsgNum].pMsg)),
			((size_t)(pMQ->pMQMsg[(pMQ->front + 1 + index) % pMQ->maxMsgNum].msgSize)));
		*pSize = pMQ->pMQMsg[(pMQ->front + 1 + index) % pMQ->maxMsgNum].msgSize;

		rt = GM_Mutex_Unlock(pMQ->mutex);
		if (GM_FALSE == rt)
		{
			return MQ_GET_UNLOCK_ERR;
		}

	}

	return MQ_GET_OK;
}

GM_BOOL GM_MQ_Write(GM_MQ_Struct* pMQ, const GM_UINT8* pMsg, GM_UINT16 size)
{
    GM_BOOL rt = GM_FALSE;
    GM_UINT16 temp = 0U;

    if ((NULL == pMQ) || (NULL == pMsg) || (size > pMQ->maxMsgSize) || (size == 0U)
        || (GM_TRUE != pMQ->initialized))
    {
        return GM_FALSE;
    }

	/*关于msgNum， front的读取和判断 调整到lock mutex之后。 CR1409*/
	rt = GM_Mutex_Lock(pMQ->mutex);

	if (GM_FALSE == rt)
	{
		return GM_FALSE;
	}

	temp = pMQ->maxMsgNum - (GM_UINT16)1u;/*计算temp*/

	if (temp ==  pMQ->msgNum)/*如果当前消息数=max-1 表示满了*/
	{
		GM_Mutex_Unlock(pMQ->mutex);
		return GM_FALSE;
	}
	else
	{
	    pMQ->rear = (pMQ->rear + 1) % pMQ->maxMsgNum;/*rear前进一步*/
		GM_memcpy(((void*)(pMQ->pMQMsg[pMQ->rear].pMsg)), (const void*) pMsg, (size_t)size);/*复制消息进入rear位*/
		pMQ->pMQMsg[pMQ->rear].msgSize = size;
		++(pMQ->msgNum);/*当前消息数++*/

		rt = GM_Mutex_Unlock(pMQ->mutex);
	}

    return rt;
}

GM_UINT32 GM_MQ_Write_Result(GM_MQ_Struct* pMQ, const GM_UINT8* pMsg, GM_UINT16 size)
{
	GM_BOOL rt = GM_FALSE;
	GM_UINT16 temp = 0U;

	if ((NULL == pMQ) || (NULL == pMsg))
	{
		return MQ_WR_PAR_NULL;
	}

	if (GM_TRUE != pMQ->initialized)
	{
		return MQ_WR_INIT_ERR;
	}

	if ((size > pMQ->maxMsgSize) || (size == 0U))
	{
		return MQ_WR_MSGSIZE_ERR;
	}

	/*关于msgNum， front的读取和判断 调整到lock mutex之后。 CR1409*/
	rt = GM_Mutex_Lock(pMQ->mutex);

	if (GM_FALSE == rt)
	{
		return MQ_WR_LOCK_ERR;
	}

	temp = pMQ->maxMsgNum - (GM_UINT16)1u;/*计算temp*/

	if (temp ==  pMQ->msgNum)/*如果当前消息数=max-1 表示满了*/
	{
		GM_Mutex_Unlock(pMQ->mutex);
		return MQ_WR_MQ_FULL;
	}
	else
	{
		pMQ->rear = (pMQ->rear + 1) % pMQ->maxMsgNum;/*rear前进一步*/
		GM_memcpy(((void*)(pMQ->pMQMsg[pMQ->rear].pMsg)), (const void*) pMsg, (size_t)size);/*复制消息进入rear位*/
		pMQ->pMQMsg[pMQ->rear].msgSize = size;
		++(pMQ->msgNum);/*当前消息数++*/

		rt = GM_Mutex_Unlock(pMQ->mutex);
		if (GM_FALSE == rt)
		{
			return MQ_WR_UNLOCK_ERR;
		}
	}

	return MQ_WR_OK;
}

GM_BOOL GM_MQ_Init(GM_MQ_Struct* pMQ, GM_UINT16 maxMsgNum, GM_UINT16 maxMsgSize)
{
    GM_BOOL rt  = GM_FALSE;
    GM_UINT16 i = 0U;
    GM_INT16 j  = 0;

    /** @brief 如果最大消息个数或每条消息的最大字节数为0,则返回错误 */

    if ((NULL == pMQ) || (0U == maxMsgNum) || (0U == maxMsgSize)
        || (GM_MQ_MSG_NUM_MAX < maxMsgNum) || (GM_MQ_MSG_SIZE_MAX < maxMsgSize))
    {
        return GM_FALSE;
    }

    if (GM_TRUE == pMQ->initialized)
    {
        return GM_TRUE;
    }

    pMQ->pMQMsg     = NULL;

    pMQ->msgNum     = (GM_UINT16)0u;
    pMQ->front      = 0;
    pMQ->rear       = 0;
    pMQ->maxMsgNum  = maxMsgNum + 1U;/*改为maxMsgNum+1，使用户可以初始最大数与可放最大数相等 20120409*/
    pMQ->maxMsgSize = maxMsgSize;
    pMQ->pMQMsg     = (GM_MQ_Msg_Struct*)GM_malloc(sizeof(GM_MQ_Msg_Struct) * pMQ->maxMsgNum);

    if (NULL == pMQ->pMQMsg)
    {
        return GM_FALSE;
    }
	/*将mutex初始化 修改到malloc之前，避免mutex_ini出错时存在未释放内存 CR 1416*/
	rt = GM_Mutex_Init(&(pMQ->mutex));

	if (GM_FALSE == rt)
	{
		return GM_FALSE;
	}

    for (i = (GM_UINT16)0u; i < pMQ->maxMsgNum; ++i)
    {
        pMQ->pMQMsg[i].pMsg = (GM_UINT8*)GM_malloc(pMQ->maxMsgSize);

        /** @brief 如果初始化失败,则释放之前分配的资源 */

        if (NULL == pMQ->pMQMsg[i].pMsg)
        {
            for (j = ((GM_INT16)i - (GM_INT16)1); j >= (GM_INT16)0; --j)
            {
                GM_free(((void*)(pMQ->pMQMsg[j].pMsg)));
                pMQ->pMQMsg[j].pMsg = NULL;
            }

            GM_free((void*)(pMQ->pMQMsg));

            pMQ->pMQMsg = NULL;
            return GM_FALSE;
        }
        else
        {
            pMQ->pMQMsg[i].msgSize = (GM_UINT16)0u;
            GM_memset(((void*)(pMQ->pMQMsg[i].pMsg)), (GM_UINT8)0U, ((size_t)(pMQ->maxMsgSize)));
        }
    }



    pMQ->initialized = GM_TRUE;

    return GM_TRUE;
}

GM_UINT32 GM_MQ_Init_Result(GM_MQ_Struct* pMQ, GM_UINT16 maxMsgNum, GM_UINT16 maxMsgSize)
{
	GM_BOOL rt  = GM_FALSE;
	GM_UINT16 i = 0U;
	GM_INT16 j  = 0;

	/** @brief 如果最大消息个数或每条消息的最大字节数为0,则返回错误 */

	if ((NULL == pMQ) || (0U == maxMsgNum) || (0U == maxMsgSize)
		|| (GM_MQ_MSG_NUM_MAX < maxMsgNum) || (GM_MQ_MSG_SIZE_MAX < maxMsgSize))
	{
		return MQ_INIT_PAR_ERR;
	}

	if (GM_TRUE == pMQ->initialized)
	{
		return MQ_INIT_OK;
	}

	pMQ->pMQMsg     = NULL;

	pMQ->msgNum     = (GM_UINT16)0u;
	pMQ->front      = 0;
	pMQ->rear       = 0;
	pMQ->maxMsgNum  = maxMsgNum + 1U;/*改为maxMsgNum+1，使用户可以初始最大数与可放最大数相等 20120409*/
	pMQ->maxMsgSize = maxMsgSize;
	pMQ->pMQMsg     = (GM_MQ_Msg_Struct*)GM_malloc(sizeof(GM_MQ_Msg_Struct) * pMQ->maxMsgNum);

	if (NULL == pMQ->pMQMsg)
	{
		return MQ_INIT_MALLOC_ERR;
	}
	/*将mutex初始化 修改到malloc之前，避免mutex_ini出错时存在未释放内存 CR 1416*/
	rt = GM_Mutex_Init(&(pMQ->mutex));

	if (GM_FALSE == rt)
	{
		return MQ_INIT_MUTEX_ERR;
	}

	for (i = (GM_UINT16)0u; i < pMQ->maxMsgNum; ++i)
	{
		pMQ->pMQMsg[i].pMsg = (GM_UINT8*)GM_malloc(pMQ->maxMsgSize);

		/** @brief 如果初始化失败,则释放之前分配的资源 */

		if (NULL == pMQ->pMQMsg[i].pMsg)
		{
			for (j = ((GM_INT16)i - (GM_INT16)1); j >= (GM_INT16)0; --j)
			{
				GM_free(((void*)(pMQ->pMQMsg[j].pMsg)));
				pMQ->pMQMsg[j].pMsg = NULL;
			}

			GM_free((void*)(pMQ->pMQMsg));

			pMQ->pMQMsg = NULL;
			return MQ_INIT_MALLOC_ERR;
		}
		else
		{
			pMQ->pMQMsg[i].msgSize = (GM_UINT16)0u;
			GM_memset(((void*)(pMQ->pMQMsg[i].pMsg)), (GM_UINT8)0U, ((size_t)(pMQ->maxMsgSize)));
		}
	}



	pMQ->initialized = GM_TRUE;

	return MQ_INIT_OK;
}

GM_BOOL GM_MQ_Clear(GM_MQ_Struct* pMQ)
{
	GM_BOOL rt = GM_FALSE;

    if ((NULL == pMQ) || (GM_TRUE != pMQ->initialized))
    {
        return GM_FALSE;
    }

    rt = GM_Mutex_Lock(pMQ->mutex);

	if (GM_FALSE == rt)
	{
		return GM_FALSE;
	}
    pMQ->front  = 0;
    pMQ->rear   = 0;
    pMQ->msgNum = (GM_UINT16)0u;

    rt = GM_Mutex_Unlock(pMQ->mutex);

    return rt;
}

GM_INT32 GM_MQ_Get_Msg_Num(const GM_MQ_Struct* pMQ)
{
	GM_BOOL rt = GM_FALSE;
	GM_INT32 temp = -1;

    if ((NULL == pMQ) || (GM_TRUE != pMQ->initialized))
    {
        return -1;
    }
    else
    {
		/*获取msgNum， 增加lock mutex保护。 CR1409
		rt = GM_Mutex_Lock(pMQ->mutex);
		if (rt == GM_FALSE)
		{
			return -1;
		}*/

		temp = (GM_INT32)(pMQ->msgNum);

		/*rt = GM_Mutex_Unlock(pMQ->mutex);
		if (rt == GM_FALSE)
		{
			return -1;
		}*/

        return temp; 
    }
}


GM_BOOL GM_MQ_Flush(GM_MQ_Struct* pMQ)
{
	GM_BOOL rt = GM_FALSE;
	GM_UINT16 msgIndex = 0U;

	if ((NULL == pMQ) || (GM_TRUE != pMQ->initialized))
	{
		return GM_FALSE;
	}

	rt = GM_Mutex_Lock(pMQ->mutex);

	if (GM_FALSE == rt)
	{
		return GM_FALSE;
	}

	for(; msgIndex< (pMQ->maxMsgNum); ++msgIndex)
	{
		pMQ->pMQMsg[msgIndex].msgSize = 0u;
		GM_memset(((void*)(pMQ->pMQMsg[msgIndex].pMsg)), (GM_UINT8)0u, ((size_t)(pMQ->maxMsgSize)));
	}

	pMQ->front  = 0;
	pMQ->rear   = 0;
	pMQ->msgNum = (GM_UINT16)0u;

	rt = GM_Mutex_Unlock(pMQ->mutex);

	return rt;
}

