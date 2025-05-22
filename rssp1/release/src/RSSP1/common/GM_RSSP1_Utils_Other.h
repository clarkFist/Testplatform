/**
 * Copyright(C) 2012,CASCO Company ��������Ȩ����( All rights reserved. )
 * 
 * @file:GM_RSSP1_Utils_Other.h
 * @brief:
 * @auther:Hao Liming
 * @create date:2010��10��30��
 * @modify by:wtt
 * @Modify info:2012��12��6��
 */


#ifndef _GM_RSSP1_UTILS_OTHER_H
#define _GM_RSSP1_UTILS_OTHER_H

#include "../GM_RSSP1_Utils_Base.h"

#ifdef __cplusplus
extern "C"
{
#endif /**< __cplusplus */

    /**
    * @brief GM_RSSP1_atoi
    *
    * �ַ���ת���������ķ�װ����ת�������ַ�Χ[-2147483648��2147483647]
    * @param[in] pBuf Ҫת�����ַ���
    * @param[in] bufSize Ҫת�����ַ�������
    * @param[out] pVal ת�����ֵ
    * @return GM_RSSP1_BOOL ת���Ƿ�ɹ�
    */
    GM_RSSP1_BOOL GM_RSSP1_atoi(const char* pBuf, GM_RSSP1_UINT32 bufSize, int* pVal);

	/**
    * @brief GM_RSSP1_atoui
    *
    * �ַ���ת���������ķ�װ����ת�������ַ�Χ[0��4294967295]
    * @param[in] pBuf Ҫת�����ַ���
    * @param[in] bufSize Ҫת�����ַ�������
    * @param[out] pVal ת�����ֵ
    * @return GM_RSSP1_BOOL ת���Ƿ�ɹ�
    */
    GM_RSSP1_BOOL GM_RSSP1_atoui(const char* pBuf, GM_RSSP1_UINT32 bufSize, unsigned int* pVal);

    /**
    * @brief GM_RSSP1_Log_Msg
    *
    * ��װ��logMsg.
    * @param[in] fmt
    * @param[in] arg1
    * @param[in] arg2
    * @param[in] arg3
    * @param[in] arg4
    * @param[in] arg5
    * @param[in] arg6
    */
#if(RSSP1_LOG_LEVEL < -10)/*��-10��-1��ɿɶ����level�ȼ���GM������С��0���û��������*/
#define GM_RSSP1_Log_Msg(X,Y,Z1,Z2,Z3,Z4,Z5,Z6) NULL
#else
    void GM_RSSP1_Log_Msg(int level, const char* fmt, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6);
#endif
    
	
	/**
    * @brief
	*
	* Ӧ�ô���ص�����.
	* @param[in] errNo
	* @param[in] fmt
	* @param[in] arg1
	* @param[in] arg2
	* @param[in] arg3
	* @param[in] arg4
	* @param[in] arg5
	* @param[in] arg6
	*/
    typedef void (*GM_RSSP1_LOG_CALL_BACK_FUN)(unsigned int errNo, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6);


	/**
    * @brief GM_RSSP1_Log_Err
    *
    * GM�ڲ�����ص�����.
	* @param[in] errNo
    * @param[in] fmt
    * @param[in] arg1
    * @param[in] arg2
	* @param[in] arg3
	* @param[in] arg4
	* @param[in] arg5
	* @param[in] arg6
	*/
	void GM_RSSP1_Log_Err(unsigned int errNo, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6);

			/**
    * @brief GM_FSFB2_Log_Usr_Callback_Init
    *
    * GM_FSFB2����ص�������ʼ��
	* @param[in] Log_err_fun
	*/
	GM_RSSP1_BOOL GM_RSSP1_Log_Usr_Callback_Init(GM_RSSP1_LOG_CALL_BACK_FUN Log_err_fun);


	    /**
    * @brief GM_RSSP1_Code_Is_Big_Endian
    *
    * �жϱ����������ֽ����������ֽ���.
    * @return GM_RSSP1_BOOL
    */
    GM_RSSP1_BOOL GM_RSSP1_Code_Is_Big_Endian(void);

    /**
    * @brief GM_RSSP1_Code_Big_Little_Endian16
    *
    * ��С��֮���ת��.
    * @param[in] x
    * @return GM_RSSP1_UINT16
    */
    GM_RSSP1_UINT16 GM_RSSP1_Code_Big_Little_Endian16(GM_RSSP1_UINT16 x);

    /**
    * @brief GM_RSSP1_Code_Big_Little_Endian32
    *
    * ��С��֮���ת��.
    * @param[in] x
    * @return GM_RSSP1_UINT32
    */
    GM_RSSP1_UINT32 GM_RSSP1_Code_Big_Little_Endian32(GM_RSSP1_UINT32 x);
	
    extern GM_RSSP1_LOG_CALL_BACK_FUN GM_RSSP1_Log_Usr;

#ifdef __cplusplus
}

#endif /**< __cplusplus */

#endif /**< _GM_UTILS_BASE_H */

