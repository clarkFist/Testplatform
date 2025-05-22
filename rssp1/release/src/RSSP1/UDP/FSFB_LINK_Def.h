
/************************************************************************/
/* UDP, SCOMģ�鶨��                               */
/************************************************************************/

#ifndef _GM_RSSP1_LINK_DEF_H_
#define _GM_RSSP1_LINK_DEF_H_

#include "GM_RSSP1_Utils.h"

#ifdef __cplusplus
extern "C"
{
#endif
	#define GM_RSSP1_INVALID_SOCKET  (-1)
	#define GM_RSSP1_WAIT_FOREVER    (-1)
	#define GM_RSSP1_NO_WAIT         0

	#define GM_RSSP1_CFM_MAX_CONNECTIONS_FOR_PROCESS    100     /**< һ���̴߳�������������*/
	#define GM_RSSP1_TASK_MAX_SOCK_NUM                  (2*GM_RSSP1_CFM_MAX_CONNECTIONS_FOR_PROCESS)    /* һ��CFM�������˫ͨ�� */
	#define GM_RSSP1_SOCKET_FD_SET_NUM                  (GM_RSSP1_TASK_MAX_SOCK_NUM/FD_SETSIZE + 1) /* һ����������һ��select GM_RSSP1_TASK_MAX_SOCK_NUM��socket */
	#define GM_RSSP1_CFM_MAX_TASK_NUM                   20      /**< ��������������� */

	#define GM_RSSP1_SOCKET_RECV_SELECT_DELAY       0      /**< ��������, select��ʱ, ms @NOTE:���������ࡢ������������ʱ������Ϊ0, ��ֹsocket�쳣���޷�����ʱ, Ƶ����ʱ��������! */
	#define GM_RSSP1_SOCKET_SEND_SELECT_DELAY       0      /**< ��������, select��ʱ, ms @NOTE:���������ࡢ������������ʱ������Ϊ0, ��ֹsocket�쳣���޷�����ʱ, Ƶ����ʱ��������!*/
	#define GM_RSSP1_SOCKET_SEND_RETRY_DELAY        1      /**<  ��������, socket��;����дʱ, ���Լ����ʱ */

	#define GM_RSSP1_SOCKET_MAINTAIN_TASK_PRIORITY      200 /**< socketά����������ȼ� */

	typedef enum
	{
		FSFB_COMM_TYPE_UDP           = 0x1111, /* �ײ����udpͨ�� */
		FSFB_COMM_TYPE_SCOM           = 0x2222, /* �ײ���ô���ͨ�� */
		FSFB_COMM_TYPE_UNKNOW           = 0xffff /* �ײ���ô���ͨ�� */
	} FSFB_LNK_Com_Type_Enum;

	/** ���紫���ַ���ýṹ */

	typedef struct
	{
		char                local_ip[20];     /**<  ����IP��ַ */
		GM_RSSP1_UINT16         local_port;       /**<  ���ض˿� */

		struct sockaddr_in  local_addr;

		char                remote_ip[20];     /**<  Զ��IP��ַ */
		GM_RSSP1_UINT16         remote_port;       /**<  Զ�̶˿� */

		struct sockaddr_in  remote_addr;
	} GM_RSSP1_PACKED FSFB_transport_config_struct;

	typedef struct
	{
		FSFB_transport_config_struct    addr_cfg;           /**< IP��port���� */
		GM_RSSP1_UINT32                     loc_ip;
		GM_RSSP1_UINT32                     rem_ip;
		GM_RSSP1_UINT32                     loc_port;
		GM_RSSP1_UINT32                     rem_port;
		GM_RSSP1_UINT32                     send_Q_size;
		GM_RSSP1_UINT32                     send_Q_offset;
		GM_RSSP1_UINT32                     recv_Q_size;
		GM_RSSP1_UINT32                     recv_Q_offset;
	} GM_RSSP1_PACKED FSFB_UDP_chn_cfg_struct;


	typedef struct
	{
		GM_RSSP1_UINT32                     Baud_rate;
		GM_RSSP1_UINT16                     check_idle_cycle;
		GM_RSSP1_UINT16                     packet_send_delay;      /**< ���ݰ�֮�䷢�ͼ������ֹ��ʽ����ʱ��ճ������ RSSP1 3.6.1.2 */
		GM_RSSP1_UINT16                     maintain_cycle;         /**< ά�����ڣ��Ե���ά����������������ֵΪ��λ�� */
	}GM_RSSP1_PACKED FSFB_SCOM_com_cfg_struct;

    typedef struct
    {
        char                            tsk_name[100];
        GM_RSSP1_UINT16                     sleep_time;             /**< ����ÿ��������ʱ��, ms.���㹫ʽ:500/ÿ����Ӧ�ò㷢�͵�������ݰ����� - ���� */
        GM_RSSP1_UINT16                     priority;               /**< �������ȼ� */
        GM_RSSP1_UINT32                     stack_size;             /**< �����ջ��С */
        GM_RSSP1_UINT16                     pkt_send_interval;      /**< ���ݰ����ͼ�� ms */
        GM_RSSP1_UINT16                     connection_num;         /**< �����������������*/
        GM_RSSP1_UINT16                     connection_index[GM_RSSP1_CFM_MAX_CONNECTIONS_FOR_PROCESS]; /**< ����������������±꣬�������еĸ�������ƥ�� */
#ifdef RSSP1_USE_OPTIMIZE_SELECT
        fd_set                          sock_read_set[GM_RSSP1_SOCKET_FD_SET_NUM];      /* ������ */
        fd_set                          sock_write_set[GM_RSSP1_SOCKET_FD_SET_NUM];     /* д���� */
#endif
    } GM_RSSP1_PACKED FSFB_LNK_task_cfg_struct;

	typedef struct
	{
		GM_RSSP1_UINT32                     sock_send_buf_size;
		GM_RSSP1_UINT32                     sock_recv_buf_size;
		GM_RSSP1_UINT16                     check_idle_cycle;
		GM_RSSP1_UINT16                     packet_send_delay;      /**< ���ݰ�֮�䷢�ͼ������ֹ��ʽ����ʱ��ճ������ RSSP1 3.6.1.2 */
		GM_RSSP1_UINT16                     maintain_cycle;         /**< socketά�����ڣ��Ե���ά����������������ֵΪ��λ�� */
	}GM_RSSP1_PACKED FSFB_UDP_sock_cfg_struct;

	typedef struct
	{
		GM_RSSP1_INT32                      com_port_no;               /**< ���ں� */
	}GM_RSSP1_PACKED FSFB_SCOM_cfg_struct;

	typedef struct
	{
		FSFB_LNK_Com_Type_Enum          com_type;

		GM_RSSP1_UINT16                      chn_num;
		FSFB_UDP_chn_cfg_struct      UDP_chn_cfg[GM_RSSP1_CFM_MAX_CHANNEL_NUM];
		FSFB_UDP_sock_cfg_struct     UDP_sock_cfg;

		FSFB_SCOM_cfg_struct        scom_chn_cfg[GM_RSSP1_CFM_MAX_CHANNEL_NUM];
		FSFB_SCOM_com_cfg_struct    scom_cfg;

	}GM_RSSP1_PACKED FSFB_LNK_connection_cfg_struct;

	typedef struct
	{
		GM_RSSP1_UINT16                     connection_nums;    /* ��SFM�е�����һһ��Ӧ */
		FSFB_LNK_connection_cfg_struct  connection_cfg[GM_RSSP1_MAX_SAFETY_CONNECTION_NUM];

		GM_RSSP1_UINT16                     net_task_num;
		FSFB_LNK_task_cfg_struct        net_task_cfg[GM_RSSP1_CFM_MAX_TASK_NUM];

	} GM_RSSP1_PACKED FSFB_LNK_cfg_struct;

#ifdef __cplusplus
}
#endif

#endif