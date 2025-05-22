/**
* @file GM_RSSP1_Utils.h
* @brief �ṩ��FSFBЭ����ʹ�õĹ�������
* @author JiangHongjun
* @date 2010-2-7 16:27:30
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: </b>jianghongjun@casco.com.cn</pre>
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/
#ifndef _GM_RSSP1_UTILS_H
#define _GM_RSSP1_UTILS_H

#include "GM_RSSP1_Utils_Base.h"
#include "GM_RSSP1_Utils_Other.h"
#include "GM_RSSP1_Memory.h"
#include "GM_RSSP1_CRC.h"
#include "GM_RSSP1_LFSR.h"
#include "GM_RSSP1_Mutex.h"
#include "GM_RSSP1_File.h"
#include "GM_RSSP1_VSN.h"

#ifdef WIN32
#include <winsock2.h>
#include "process.h"
#pragma comment(lib, "Ws2_32.lib")
#else
#include <netinet/in.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

	#define GM_RSSP1_OK 1
	#define GM_RSSP1_ERROR (-1)
	#define GM_RSSP1_RETURN_RESERVED  (-2)



	/** �������ݴ�С���ƶ��� */
#ifndef GM_RSSP1_CHECK_CHN_NUM
	#define GM_RSSP1_CHECK_CHN_NUM          2U       /**< FSFB˫ͨ��У�� */
#endif

#ifndef GM_RSSP1_MAX_USER_DAT_LEN
	#define GM_RSSP1_MAX_USER_DAT_LEN       (480U)     /**< Ϊ������Ϣ */
#endif

#ifndef GM_RSSP1_MAX_SAFETY_CONNECTION_NUM
	#define GM_RSSP1_MAX_SAFETY_CONNECTION_NUM      128U   /**< ���֧�ֵ�FSFB��ȫ������ */
#endif

#ifndef GM_RSSP1_MAX_DELTATIME_ARRAY_SIZE
	#define GM_RSSP1_MAX_DELTATIME_ARRAY_SIZE       20U    /**< ΪPREC_SINIT[]��POST_RXDATA[]�����������������ʵ��ʹ��ʱ�Ը�ͨ�����õ�deltaTimeΪ���� */
#endif

#ifndef GM_RSSP1_MAX_TOLERATE_CYCLE
	#define GM_RSSP1_MAX_TOLERATE_CYCLE             20U  /**< FSFB��ˡ��������ܳ���10�롣*/
#endif


	/*    #define GM_RSSP1_MAX_USER_DAT_LEN       478  */  /**< ��ȫ��ɽ��ܵ�����Ӧ�ò��������ݳ���RSSP-I p10. ����Ϊż���ֽ� */
	#define GM_RSSP1_MAX_SAFETY_DAT_LEN     (GM_RSSP1_MAX_USER_DAT_LEN+22)     /**< ͨ�Ų�ɽ��ܵ����԰�ȫ���������ݳ���RSSP-I p10. */
	#define GM_RSSP1_MAX_SND_DAT_LEN        GM_RSSP1_MAX_SAFETY_DAT_LEN
	#define GM_RSSP1_CFM_MAX_CHANNEL_NUM        GM_RSSP1_CHECK_CHN_NUM      /**< �ײ�����ͨ�������ֵ */
	#define GM_RSSP1_MAX_PKT_NUM_PER_CYCLE          5U     /**< ÿ���ڿ��ܲ�����ÿ�����ݰ��������������RSD��SSE��SSR */

	/*fulintai*/
	#define GM_RSSP1_MAX_LINK_CHANNEL        GM_RSSP1_CFM_MAX_CHANNEL_NUM

    	/*VSN_Get �ص��������Ͷ���added by huang, 20151219*/
	typedef void(*GM_RSSP1_VSN_GET_CALLBACK_FUN)(GM_RSSP1_UINT32* pVSN0, GM_RSSP1_UINT32* pVSN1, GM_RSSP1_UINT32* pVSN2);

#ifdef __cplusplus
}
#endif

#endif

