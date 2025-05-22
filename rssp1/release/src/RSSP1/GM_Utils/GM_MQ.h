/**
* @file GM_MQ.h
* @brief ʵ�����̰߳�ȫ��Ϣ���еķ�װ
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
        GM_BOOL initialized; /**< �Ƿ��ʼ����         */
        GM_UINT16 maxMsgNum;   /**< MQ�������Ϣ����     */
        GM_UINT16 maxMsgSize;  /**< ÿ����Ϣ������ֽ��� */
        int front;              /**< ͷָ��               */
        int rear;               /**< βָ��               */
        GM_UINT16 msgNum;      /**< ��Ϣ��������Ϣ����   */
        GM_MUTEX mutex;        /**< ������Ϣ���еĻ����� */
        GM_MQ_Msg_Struct* pMQMsg; /**< ָ���һ����Ϣ    */
    }GM_PACKED GM_MQ_Struct;

    /**
    * @brief GM_MQ_Init
    *
    * Detailed description.
    * @param[in] pMQ Ҫ��ʼ������Ϣ����
    * @param[in] maxMsgNum ��Ϣ���пɴ�ŵ���Ϣ��Ŀ
    * @param[in] maxMsgSize ÿ����Ϣ�Ĵ�С
    * @return GM_BOOL
    */
    GM_BOOL GM_MQ_Init(GM_MQ_Struct* pMQ, GM_UINT16 maxMsgNum, GM_UINT16 maxMsgSize);

	/**
    * @brief GM_MQ_Init_Result
    *
    * Detailed description.
    * @param[in] pMQ Ҫ��ʼ������Ϣ����
    * @param[in] maxMsgNum ��Ϣ���пɴ�ŵ���Ϣ��Ŀ
    * @param[in] maxMsgSize ÿ����Ϣ�Ĵ�С
    * @return GM_UINT32 ���ɹ��򷵻�MQ_INIT_OK�����򷵻ش����
    */
    GM_UINT32 GM_MQ_Init_Result(GM_MQ_Struct* pMQ, GM_UINT16 maxMsgNum, GM_UINT16 maxMsgSize);

    /**
    * @brief GM_MQ_Get_Msg_Num
    *
    * Detailed description.
    * @param[in] pMQ Ҫ�����Ϣ��������Ϣ����
    * @return GM_INT32 ��ʧ���򷵻�-1,���򷵻���Ϣ����
    */
    GM_INT32 GM_MQ_Get_Msg_Num(const GM_MQ_Struct* pMQ);

    /**
    * @brief GM_MQ_Write
    *
    * Detailed description.
    * @param[in] pMQ Ҫд����Ϣ����
    * @param[in] pMsg Ҫд����Ϣ
    * @param[in] size Ҫд����Ϣ��С
    * @return GM_MQ_Write_Result_Enum 
    */
    GM_BOOL GM_MQ_Write(GM_MQ_Struct* pMQ, const GM_UINT8* pMsg, GM_UINT16 size);

	/**
    * @brief GM_MQ_Write_Result
    *
    * Detailed description.
    * @param[in] pMQ Ҫд����Ϣ����
    * @param[in] pMsg Ҫд����Ϣ
    * @param[in] size Ҫд����Ϣ��С
    * @return GM_UINT32 ���ɹ��򷵻�MQ_WR_OK�����򷵻ش����
    */
    GM_UINT32 GM_MQ_Write_Result(GM_MQ_Struct* pMQ, const GM_UINT8* pMsg, GM_UINT16 size);

    /**
    * @brief GM_MQ_Read
    *
    * Detailed description.
    * @param[in] pMQ Ҫ������Ϣ����
    * @param[in] pOutBuf Ҫ�����Ϣ�Ļ�����
    * @param[in/out] pSize ��Ϊ�������ʱΪ����Ļ�������Ϣ��С����Ϊ�������Ϊ��������Ϣ��С
    * @return GM_BOOL
    */
    GM_BOOL GM_MQ_Read(GM_MQ_Struct* pMQ, GM_UINT8* pOutBuf, GM_UINT16* pSize);

	/**
    * @brief GM_MQ_Read_Result
    *
    * Detailed description.
    * @param[in] pMQ Ҫ������Ϣ����
    * @param[in] pOutBuf Ҫ�����Ϣ�Ļ�����
    * @param[in/out] pSize ��Ϊ�������ʱΪ����Ļ�������Ϣ��С����Ϊ�������Ϊ��������Ϣ��С
    * @return GM_UINT32 ���ɹ��򷵻�MQ_RD_OK�����򷵻ش����
    */
    GM_UINT32 GM_MQ_Read_Result(GM_MQ_Struct* pMQ, GM_UINT8* pOutBuf, GM_UINT16* pSize);

    /**
    * @brief GM_MQ_Get
    *
    * ����ȡ���ݣ���������.
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
    * ����ȡ���ݣ���������.
    * @param[in] pMQ
    * @param[in] index
    * @param[in] pOutBuf
    * @param[in] pSize
    * @return GM_UINT32 ���ɹ��򷵻�MQ_GET_OK�����򷵻ش����
    */
    GM_UINT32 GM_MQ_Get_Result(GM_MQ_Struct* pMQ, GM_UINT16 index, GM_UINT8* pOutBuf, GM_UINT16* pSize);

    /**
    * @brief GM_MQ_Clear
    *
    * Detailed description.
    * @param[in] pMQ Ҫ��յ���Ϣ����
    * @return GM_BOOL
    */
    GM_BOOL GM_MQ_Clear(GM_MQ_Struct* pMQ);

    /**
    * @brief GM_MQ_Flush
    *
    * Detailed description.
    * @param[in] pMQ Ҫ�������Ϣ�����ڴ�
    * @return GM_BOOL
    */
	GM_BOOL GM_MQ_Flush(GM_MQ_Struct* pMQ);


#ifdef __cplusplus
}

#endif /**< __cplusplus */

#endif /**< _GM_MQ_H */

