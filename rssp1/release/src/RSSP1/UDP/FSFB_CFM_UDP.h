/**
* @file FSFB_CFM_UDP.h
* @brief �ײ�UDP���紦��
* @author JiangHongjun
* @date 2010-2-7 16:25:56
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: </b>jianghongjun@casco.com.cn</pre>
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/

#ifndef _GM_RSSP1_UDP_MNG_H_
#define _GM_RSSP1_UDP_MNG_H_

#include "FSFB_UDP_Init.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
    * @brief ����socket
    *
    * Detailed description.
    * @param[in] type
    * @param[in] max_socket_value
    * @return GM_RSSP1_INT32
    */
    GM_RSSP1_INT32 FSFB_Create_Socket(GM_RSSP1_INT32 type, GM_SOCKET *max_socket_value);

    /**
    * @brief �ر�socket
    *
    * Detailed description.
    * @param[in] pSock
    * @return GM_RSSP1_INT32
    */
    GM_RSSP1_INT32 FSFB_Close_Socket(GM_SOCKET *pSock);

    /**
    * @brief ����socket����
    *
    * Detailed description.
    * @param[in] sock
    * @param[in] socket_send_buf_len
    * @param[in] socket_recv_buf_len
    * @return GM_RSSP1_INT32
    */
    GM_RSSP1_INT32 FSFB_Set_Socket_Option(GM_SOCKET sock, GM_RSSP1_INT32 socket_send_buf_len , GM_RSSP1_INT32 socket_recv_buf_len);

    /**
    * @brief ��ʼ��UDP socket
    *
    * Detailed description.
    * @param[in] pSock
    * @param[in] type
    * @param[in] pAddr
    * @param[in] socket_send_buf_len
    * @param[in] socket_recv_buf_len
    * @param[in] pMax_socket_value
    * @return GM_RSSP1_INT32
    */
    GM_RSSP1_INT32 FSFB_Socket_Init(GM_SOCKET *pSock , GM_RSSP1_INT32 type , struct sockaddr_in *pAddr , GM_RSSP1_INT32 socket_send_buf_len , GM_RSSP1_INT32 socket_recv_buf_len , GM_SOCKET *pMax_socket_value);

    /**
    * @brief �ж�socket�Ƿ�ɶ�
    *
    * @param[in] socket
    * @param[in] max_socket_value ϵͳsocket�����������ֵ
    * @param[in] time_Out �ȴ���ʱ, �ȴ����Բ��ɶ��ŷ���.��λ:ms,
    * @return GM_RSSP1_BOOL
    */
    GM_RSSP1_BOOL FSFB_Is_Socket_Readable(GM_SOCKET sock, GM_SOCKET max_socket_value, GM_RSSP1_INT32 time_Out);


    /**
    * @brief �ж�socket�Ƿ��д
    *
    * @param[in] socket
    * @param[in] max_socket_value ϵͳsocket�����������ֵ
    * @param[in] time_Out �ȴ���ʱ, �ȴ����Բ���д�ŷ���.��λ:ms,
    * @return GM_RSSP1_BOOL
    */
    GM_RSSP1_BOOL FSFB_Is_Socket_Writable(GM_SOCKET sock, GM_SOCKET max_socket_value, GM_RSSP1_INT32 time_Out);


	void FSFB_CFM_Con_UDP_Maintain(FSFB_LNK_connection_struct *pLNK_con , FSFB_LNK_object_struct * pLNK, GM_RSSP1_UINT8 index);

    /**
    * @brief ����CFM����ͨ���е�����
    *
    * ͬʱ�����ж�UDP�Ƿ�ʱ��δ�յ�����.
    * @param[in] pCFM_con
    * @param[in] pCFM
    */
    void FSFB_CFM_Con_UDP_Recv_Dat(FSFB_LNK_connection_struct *pLNK_con , FSFB_LNK_object_struct * pLNK, GM_RSSP1_UINT8 index);

    /**
    * @brief  ����CFM����ͨ���е�����
    *
    * Detailed description.
    * @param[in] pCFM_con
    * @param[in] pCFM
    */
    void FSFB_CFM_Con_UDP_Send_Dat(FSFB_LNK_connection_struct *pLNK_con , FSFB_LNK_object_struct * pLNK, GM_RSSP1_UINT8 index);

#ifdef __cplusplus
}
#endif

#endif