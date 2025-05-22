/**
* @file GM_RSSP1_VSN.h
* @brief VSN�㷨ʵ��,AlgorithmA��AlgorithmB�����㷨���ڴ��ļ���ʵ��
* @author Hao Liming
* @date 2010-4-4 11:11:55
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: </b>haoliming@casco.com.cn</pre>
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/
#ifndef _GM_RSSP1_VSN_H
#define _GM_RSSP1_VSN_H

#include "../GM_RSSP1_Utils_Base.h"
#include "GM_RSSP1_LFSR.h"
#include "GM_RSSP1_Memory.h"
#include "GM_RSSP1_Utils_Other.h"

#ifdef __cplusplus
extern "C"
{
#endif /**< __cplusplus */

#define GM_RSSP1_VSN_M01 (0x12340234U) /**< VSN1������  */
#define GM_RSSP1_VSN_M02 (0x43211328U) /**< VSN2������ */

#define GM_RSSP1_VSN_SEED1 (0x0B7A7124U) /**< VSN1�ĳ�ʼֵ*/
#define GM_RSSP1_VSN_SEED2 (0x1C4269F4U) /**< VSN2�ĳ�ʼֵ*/

    /**
    * @brief GM_RSSP1_VSN_Init
    *
    * ��ʼ��VSN.  ע�⣺���������VSNֻ�ܳ�ʼ��һ�Σ��ڶ��ε��ó�ʼ����������ʹVSN��ֵ������Ϊ��ʼ̬��
    */
    GM_RSSP1_BOOL GM_RSSP1_VSN_Init(void);

    /**
    * @brief GM_RSSP1_VSN_Update
    *
    * ÿ�����ڸ���һ��VSN.
    */
    void GM_RSSP1_VSN_Update(void);

    /**
    * @brief GM_VSN_Get
    *
    * ��õ�ǰ���ڵ�VSN.
    * @param[out] pVSN0
    * @param[out] pVSN1
    * @param[out] pVSN2
    */
    void GM_RSSP1_VSN_Get(GM_RSSP1_UINT32* pVSN0, GM_RSSP1_UINT32* pVSN1, GM_RSSP1_UINT32* pVSN2);

    /**
    * @brief GM_RSSP1_VSN_Set
    *
    * ���õ�ǰ���ڵ�VSN.
    * @param[in] VSN0
    * @param[in] VSN1
    * @param[in] VSN2
    */
    void GM_RSSP1_VSN_Set(GM_RSSP1_UINT32 VSN0, GM_RSSP1_UINT32 VSN1, GM_RSSP1_UINT32 VSN2);

#ifdef __cplusplus
}

#endif /**< __cplusplus */

#endif /**< _GM_VSN_H */

