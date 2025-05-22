/**
* @file GM_RSSP1_LFSR.h
* @brief ����С�˱���
* @author Hao Liming
* @date 2010-7-29 15:28:03
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: </b>haoliming@casco.com.cn</pre>
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/

#ifndef _GM_RSSP1_LFSR_H
#define _GM_RSSP1_LFSR_H

#include "GM_RSSP1_Utils_Base.h"

#ifdef __cplusplus
extern "C"
{
#endif /**< __cplusplus */

    /**
    * @brief FSFB2ͨ��1 PD�����õ�������LFSR��0x10FC22F87
    *
    */
    extern const GM_RSSP1_UINT32 GM_RSSP1_LFSR_LEFT_CHN1_TABLE[];

    /**
    * @brief FSFB2ͨ��1 PD�����õ�������LFSR��0x10FC22F87
    *
    */
    extern const GM_RSSP1_UINT32 GM_RSSP1_LFSR_RIGHT_CHN1_TABLE[];

    /**
    * @brief FSFB2ͨ��2 PD�����õ�������LFSR��0x1C3E887E1
    *
    */
    extern const GM_RSSP1_UINT32 GM_RSSP1_LFSR_LEFT_CHN2_TABLE[];

    /**
    * @brief FSFB2ͨ��2 PD�����õ�������LFSR��0x1C3E887E1
    *
    */
    extern const GM_RSSP1_UINT32 GM_RSSP1_LFSR_RIGHT_CHN2_TABLE[];

    typedef struct
    {
        GM_RSSP1_UINT32 reg;
        const GM_RSSP1_UINT32 *leftTable;
        const GM_RSSP1_UINT32 *rightTable;
    }GM_RSSP1_PACKED GM_RSSP1_LFSR;

    /**
    * @brief GM_RSSP1_LFSR_Read
    *
    * ��ȡ������.
    * @param[in] This LFSR�����м����ݽṹ
    * @param[out] pData �Ĵ����е�ֵ
    * @return GM_RSSP1_BOOL
    */
    GM_RSSP1_BOOL GM_RSSP1_LFSR_Read(GM_RSSP1_LFSR* This, GM_RSSP1_UINT32* pData);

    /**
    * @brief GM_RSSP1_LFSR_Init
    *
    * ��ʼ��LFSR�����м����ݽṹ.
    * @param[in] This LFSR�����м����ݽṹ
    * @param[in] pLeftTable ���������
    * @param[in] pRightTable ���������
    * @return GM_RSSP1_BOOL
    */
    GM_RSSP1_BOOL GM_RSSP1_LFSR_Init(GM_RSSP1_LFSR* This, const GM_RSSP1_UINT32* pLeftTable, const GM_RSSP1_UINT32* pRightTable);

    /**
    * @brief GM_RSSP1_LFSR_Load
    *
    * ���ؽ���LFSR���������.
    * @param[in] This LFSR�����м����ݽṹ
    * @param[in] data ��������
    * @return GM_RSSP1_BOOL
    */
    GM_RSSP1_BOOL GM_RSSP1_LFSR_Load(GM_RSSP1_LFSR* This, GM_RSSP1_UINT32 data);

    /**
    * @brief GM_RSSP1_LFSR_Add
    *
    * ����Add����.
    * @param[in] This LFSR�����м����ݽṹ
    * @param[in] data ��������
    * @return GM_RSSP1_BOOL
    */
    GM_RSSP1_BOOL GM_RSSP1_LFSR_Add(GM_RSSP1_LFSR* This, GM_RSSP1_UINT32 data);

    /**
    * @brief GM_RSSP1_LFSR_Post
    *
    * ����Post����.
    * @param[in] This LFSR�����м����ݽṹ
    * @param[in] data ��������
    * @return GM_RSSP1_BOOL
    */
    GM_RSSP1_BOOL GM_RSSP1_LFSR_Post(GM_RSSP1_LFSR* This, GM_RSSP1_UINT32 data);

#ifdef __cplusplus
}

#endif /**< __cplusplus */

#endif /**< _GM_RSSP1_LFSR_H */



