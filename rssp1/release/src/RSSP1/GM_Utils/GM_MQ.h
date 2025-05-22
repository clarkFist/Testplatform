/**
* @file GM_MQ.h
* @brief 实现了线程安全消息队列的封装
* @author Hao Liming
* @date 2010-4-17 12:57:57
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
#ifndef _GM_MQ_H
#define _GM_MQ_H

#include "GM_Mutex.h"
#include "GM_Memset.h"
#include "GM_Memcpy.h"
#include "GM_Malloc.h"
#include "GM_Free.h"
#include "../GM_Error.h"

#ifdef __cplusplus
extern "C"
{
#endif /**< __cplusplus */

    typedef struct
    {
        GM_UINT16 msgSize;
        GM_UINT8* pMsg;
    }GM_PACKED GM_MQ_Msg_Struct;

    typedef struct
    {
        GM_BOOL initialized; /**< 是否初始化了         */
        GM_UINT16 maxMsgNum;   /**< MQ中最大消息个数     */
        GM_UINT16 maxMsgSize;  /**< 每条消息的最大字节数 */
        int front;              /**< 头指针               */
        int rear;               /**< 尾指针               */
        GM_UINT16 msgNum;      /**< 消息队列中消息个数   */
        GM_MUTEX mutex;        /**< 保护消息队列的互斥器 */
        GM_MQ_Msg_Struct* pMQMsg; /**< 指向第一条消息    */
    }GM_PACKED GM_MQ_Struct;

    /**
    * @brief GM_MQ_Init
    *
    * Detailed description.
    * @param[in] pMQ 要初始化的消息队列
    * @param[in] maxMsgNum 消息队列可存放的消息数目
    * @param[in] maxMsgSize 每条消息的大小
    * @return GM_BOOL
    */
    GM_BOOL GM_MQ_Init(GM_MQ_Struct* pMQ, GM_UINT16 maxMsgNum, GM_UINT16 maxMsgSize);

	/**
    * @brief GM_MQ_Init_Result
    *
    * Detailed description.
    * @param[in] pMQ 要初始化的消息队列
    * @param[in] maxMsgNum 消息队列可存放的消息数目
    * @param[in] maxMsgSize 每条消息的大小
    * @return GM_UINT32 若成功则返回MQ_INIT_OK，否则返回错误号
    */
    GM_UINT32 GM_MQ_Init_Result(GM_MQ_Struct* pMQ, GM_UINT16 maxMsgNum, GM_UINT16 maxMsgSize);

    /**
    * @brief GM_MQ_Get_Msg_Num
    *
    * Detailed description.
    * @param[in] pMQ 要获得消息个数的消息队列
    * @return GM_INT32 若失败则返回-1,否则返回消息个数
    */
    GM_INT32 GM_MQ_Get_Msg_Num(const GM_MQ_Struct* pMQ);

    /**
    * @brief GM_MQ_Write
    *
    * Detailed description.
    * @param[in] pMQ 要写的消息队列
    * @param[in] pMsg 要写的消息
    * @param[in] size 要写的消息大小
    * @return GM_MQ_Write_Result_Enum 
    */
    GM_BOOL GM_MQ_Write(GM_MQ_Struct* pMQ, const GM_UINT8* pMsg, GM_UINT16 size);

	/**
    * @brief GM_MQ_Write_Result
    *
    * Detailed description.
    * @param[in] pMQ 要写的消息队列
    * @param[in] pMsg 要写的消息
    * @param[in] size 要写的消息大小
    * @return GM_UINT32 若成功则返回MQ_WR_OK，否则返回错误号
    */
    GM_UINT32 GM_MQ_Write_Result(GM_MQ_Struct* pMQ, const GM_UINT8* pMsg, GM_UINT16 size);

    /**
    * @brief GM_MQ_Read
    *
    * Detailed description.
    * @param[in] pMQ 要读的消息队列
    * @param[in] pOutBuf 要输出消息的缓冲区
    * @param[in/out] pSize 作为输入参数时为输出的缓冲区消息大小，作为输出参数为读到的消息大小
    * @return GM_BOOL
    */
    GM_BOOL GM_MQ_Read(GM_MQ_Struct* pMQ, GM_UINT8* pOutBuf, GM_UINT16* pSize);

	/**
    * @brief GM_MQ_Read_Result
    *
    * Detailed description.
    * @param[in] pMQ 要读的消息队列
    * @param[in] pOutBuf 要输出消息的缓冲区
    * @param[in/out] pSize 作为输入参数时为输出的缓冲区消息大小，作为输出参数为读到的消息大小
    * @return GM_UINT32 若成功则返回MQ_RD_OK，否则返回错误号
    */
    GM_UINT32 GM_MQ_Read_Result(GM_MQ_Struct* pMQ, GM_UINT8* pOutBuf, GM_UINT16* pSize);

    /**
    * @brief GM_MQ_Get
    *
    * 仅读取数据，而不弹出.
    * @param[in] pMQ
    * @param[in] index
    * @param[in] pOutBuf
    * @param[in] pSize
    * @return GM_BOOL
    */
    GM_BOOL GM_MQ_Get(GM_MQ_Struct* pMQ, GM_UINT16 index, GM_UINT8* pOutBuf, GM_UINT16* pSize);

	/**
    * @brief GM_MQ_Get_Result
    *
    * 仅读取数据，而不弹出.
    * @param[in] pMQ
    * @param[in] index
    * @param[in] pOutBuf
    * @param[in] pSize
    * @return GM_UINT32 若成功则返回MQ_GET_OK，否则返回错误号
    */
    GM_UINT32 GM_MQ_Get_Result(GM_MQ_Struct* pMQ, GM_UINT16 index, GM_UINT8* pOutBuf, GM_UINT16* pSize);

    /**
    * @brief GM_MQ_Clear
    *
    * Detailed description.
    * @param[in] pMQ 要清空的消息队列
    * @return GM_BOOL
    */
    GM_BOOL GM_MQ_Clear(GM_MQ_Struct* pMQ);

    /**
    * @brief GM_MQ_Flush
    *
    * Detailed description.
    * @param[in] pMQ 要清零的消息队列内存
    * @return GM_BOOL
    */
	GM_BOOL GM_MQ_Flush(GM_MQ_Struct* pMQ);


#ifdef __cplusplus
}

#endif /**< __cplusplus */

#endif /**< _GM_MQ_H */

