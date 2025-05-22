/**
* @file GM_RSSP1_CRC.h
* @brief
* @author Hao Liming CRC��װ,��̫����,֮���ٽ��з�װ
* @date 2009-12-22 15:35:29
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: </b>haoliming@casco.com.cn</pre>
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/
#ifndef _GM_RSSP1_CRC_H
#define _GM_RSSP1_CRC_H

#include "GM_RSSP1_Utils_Base.h"

#ifdef __cplusplus
extern "C"
{
#endif /**< __cplusplus */

    /** @brief ����ʽ0x11021������CRC�� */
    extern const GM_RSSP1_UINT16 GM_RSSP1_CRC16_Table_0x11021_MSB[];

    /** @brief ����ʽ0x11021������CRC��(0x18408) */
    extern const GM_RSSP1_UINT16 GM_RSSP1_CRC16_Table_0x11021_LSB[];

    /** @brief ����ʽ0x10811������CRC�� */
    extern const GM_RSSP1_UINT16 GM_RSSP1_CRC16_Table_0x10811_MSB[];

    /** @brief ����ʽ0x10811������CRC��, FSFB2�л��õ���CRC�� */
    extern const GM_RSSP1_UINT16 GM_RSSP1_CRC16_Table_0x10811_LSB[];

    /** @brief FSFB2ͨ��1�õ���CRC��0x1100D4E63 */
    extern const GM_RSSP1_UINT32 GM_RSSP1_CRC32_Channel1_Table[];

    /** @brief FSFB2ͨ��2�õ���CRC��0x18CE56011 */
    extern const GM_RSSP1_UINT32 GM_RSSP1_CRC32_Channel2_Table[];
	
    /** @brief ����ʽ0x04C11DB7��CRC32�ߵ��� */
    extern const GM_RSSP1_UINT32 GM_RSSP1_CRC32_Table_0x04C11DB7_L[];

    /**
    * @brief
    *
    * �������Ӻ�˳�����CRCУ����
    * @param[in] pBuffer ��У�������
    * @param[in] bufferLen ��У������ݳ���(�ֽ�)
    * @param[in] pCRC ����CRCУ����
    * @param[in] pCRCTable CRC��
    * @return GM_RSSP1_BOOL GM_CRC_Calculate_CRC16
    */
    GM_RSSP1_BOOL GM_RSSP1_CRC_Calculate_CRC16(const GM_RSSP1_UINT8 *pBuffer, GM_RSSP1_UINT32 bufferLen, GM_RSSP1_UINT16* pCRC, const GM_RSSP1_UINT16* pCRCTable);

    /**
    * @brief GM_RSSP1_CRC_Calculate_CRC32
    *
    * Detailed description.
    * @param[in] pBuffer ��У�������
    * @param[in] bufferLen ��У������ݳ���(�ֽ�)
    * @param[in] pCRC ����CRCУ����
    * @param[in] pCRCTable CRC��
    * @return GM_RSSP1_BOOL
    */
    GM_RSSP1_BOOL GM_RSSP1_CRC_Calculate_CRC32(const GM_RSSP1_UINT8* pBuffer, GM_RSSP1_UINT32 bufferLen, GM_RSSP1_UINT32* pCRC, const GM_RSSP1_UINT32* pCRCTable);


#ifdef __cplusplus
}

#endif /**< __cplusplus */

#endif /**< _GM_CRC_H */

