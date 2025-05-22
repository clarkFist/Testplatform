/**
* @file GM_RSSP1_Msg_Define.h
* @brief RSSP1中定义的消息结构。参考RSSP-I V0.1
* @author JiangHongjun
* @date 2010-2-7 16:24:25
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: </b>jianghongjun@casco.com.cn</pre>
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/

/* RSSP-I */

#ifndef _GM_RSSP1_MSG_DEFINE_H
#define _GM_RSSP1_MSG_DEFINE_H

#ifdef __cplusplus
extern "C"
{
#endif

	#define RSSP1_DAT_VERSION           ((GM_RSSP1_UINT8)0x01U)
		/*** SSE ***/
	#define RSSP1_SSE_PROTOCOL_TYPE     ((GM_RSSP1_UINT8)0x01U)
	#define RSSP1_SSE_FRAME_TYPE        ((GM_RSSP1_UINT8)0x90U)


		/*** SSR ***/
	#define RSSP1_SSR_PROTOCOL_TYPE     ((GM_RSSP1_UINT8)0x01U)
	#define RSSP1_SSR_FRAME_TYPE        ((GM_RSSP1_UINT8)0x91U)


		/*** RSD ***/
	#define RSSP1_RSD_PROTOCOL_TYPE_ACTIVE      ((GM_RSSP1_UINT8)0x01U)
	#define RSSP1_RSD_PROTOCOL_TYPE_STANDBY     ((GM_RSSP1_UINT8)0x02U)
	#define RSSP1_RSD_FRAME_TYPE_FROM_A         ((GM_RSSP1_UINT8)0x80U)
	#define RSSP1_RSD_FRAME_TYPE_FROM_B         ((GM_RSSP1_UINT8)0x81U)


	#define RSSP1_SSE_FRAME_LEN_WITH_CRC    20
	#define RSSP1_SSR_FRAME_LEN_WITH_CRC    25
	#define RSSP1_RSD_FRAME_LEN_WITHOUT_USERDAT 22  /* RSD长度 = RSSP1_RSD_FRAME_LEN_WITHOUT_USERDAT + USERDAT */
	#define RSSP1_MAX_RSD_LEN   545

	//#define RSSP1_FAULT_INJECT    /* RSSP1协议的故障注入*/
	#define RSSP1_FAULT_NUM  10   /* RSSP1协议的故障注入*/
	#define RSSP1_FAULT_CRC1  0   /* RSD消息的CRC1错误  */ 
	#define RSSP1_FAULT_CRC2  1   /* RSD消息的CRC2错误  */
	#define RSSP1_FAULT_SEQ   2   /* RSD消息的序号错误  */
	#define RSSP1_FAULT_DELAY 3   /* RSD消息的超时错误  */
	#define RSSP1_FAULT_LOSS  4   /* RSD消息的丢失错误  */

	extern int* pFaults;

#ifdef __cplusplus
}

#endif /* extern "C" */

#endif
