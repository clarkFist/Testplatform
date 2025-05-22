#ifndef _GM_RSSP1_UDP_INIT_H
#define _GM_RSSP1_UDP_INIT_H

#include "FSFB_LINK_Def.h"
#include "../GM_RSSP1_Msg_Queue.h"
#include "../GM_RSSP1_MQ_Interface.h"
#include "GM_MultiThread.h"
#ifdef __cplusplus
extern "C"
{
#endif

	#ifdef WIN32
	#define GM_SLEEP(x) Sleep(x)
	#endif
	/** һ��UDPͨ���Ľṹ���� */
	#ifdef WIN32
		typedef GM_RSSP1_INT16 GM_SOCKET;
	#endif

	#ifdef WIN32
			/* �ֽڶ��� */
	#pragma pack(1) /* �Ե��ֽڶ��� */
	#define FSFB_PACKED

			/*socket*/
	#define FSFB_SOCKET                 GM_INT16
	#define FSFB_SOCKET_OK              0
	#define FSFB_SOCKET_ERROR           (-1)
	#define FSFB_MSG_NOSIGNAL           0               /**< send�������һ������, windows��һ������Ϊ0  */
	#define FSFB_MSG_DONTWAIT           0

	#define FSFB_SOCKET_ERR_WOULDBLOCK WSAEWOULDBLOCK
	#define FSFB_SOCKET_ERR_AGAIN      WSAEWOULDBLOCK
	#define FSFB_SOCKET_ERR_INPROGRESS WSAEINPROGRESS   /* Operation now in progress. */
	#define FSFB_SOCKET_ERR_ISCONN     WSAEISCONN      /* Socket is already connected */
	#define FSFB_SOCKET_ERR_INVAL       WSAEINVAL  /* socket�Ѿ��� */
	#define FSFB_SOCKET_ERR_NOBUFS      WSAENOBUFS
	#define FSFB_SOKC_ERR_EADDRINUSE    WSAEADDRINUSE
	#define FSFB_SOCK_STREAM           SOCK_STREAM
	#define FSFB_SOCK_DGRAM            SOCK_DGRAM
	#define FSFB_SOCK_RAW              SOCK_RAW

	#define FSFB_GET_LAST_ERRNO      WSAGetLastError()
	#endif /*WIN32*/



	typedef struct
	{
		GM_RSSP1_BOOL                       b_connected;        /**< �Ѿ�������(UDP�����ӣ��涨�ڿ�ˡ����check_idle_cycle��û���յ��κ�����ʱ������Ϊ�Ͽ�) */
		GM_RSSP1_INT32                      idle_counter;       /**< δ�յ����ݼ��� */

		GM_SOCKET                     sock;

		FSFB_transport_config_struct    addr_cfg;           /**< IP��port���� */

		GM_RSSP1_com_pri_struct    temp_send_pkt;      /**< �ӷ��Ͷ�����ȡ��һ�����ݴ��ڴˣ���ֹȡ����socket�����㷢���������¶��� */
		GM_RSSP1_BOOL                       b_temp_send_pkt_valid;

		GM_RSSP1_Msg_Queue_struct           send_udp_dat_Q;     /**< �����͵�UDP���ݰ����У�����������룬���շ������� */
		GM_RSSP1_Msg_Queue_struct           recv_udp_dat_Q;     /**< UDP socket���յ������ݰ����У��շ������������յ����ݺ���룬������������ */
	}GM_RSSP1_PACKED FSFB_UDP_chn_mng_struct;

	/** һ��SCOMͨ���Ľṹ���� */

	typedef struct
	{
		GM_RSSP1_BOOL                       b_connected;        /**< �Ѿ�������(���������ӣ��涨�ڿ�ˡ����check_idle_cycle��û���յ��κ�����ʱ������Ϊ�Ͽ�) */
		GM_RSSP1_INT32                             idle_counter;       /**< δ�յ����ݼ��� */

		GM_RSSP1_INT32                             com_handle;             /**< ������Դ��� */

		GM_RSSP1_INT32                      com_port_no;               /**< ���ں� */
		GM_RSSP1_UINT32                     Baud_rate;

		GM_RSSP1_com_pri_struct    temp_send_pkt;      /**< �ӷ��Ͷ�����ȡ��һ�����ݴ��ڴˣ���ֹȡ���󴮿ڲ����㷢���������¶��� */
		GM_RSSP1_BOOL                       b_temp_send_pkt_valid;

		GM_RSSP1_Msg_Queue_struct           send_scom_dat_Q;     /**< �����͵����ݰ����У�����������룬���շ������� */
		GM_RSSP1_Msg_Queue_struct           recv_scom_dat_Q;     /**< ���յ������ݰ����У��շ�����Ӵ��ڽ��յ����ݺ���룬������������ */
		unsigned char                   *p_recv_buf;        /**< ����������ȡ */
		GM_RSSP1_UINT16                     recv_buf_size;      /**< Ϊ���ڽ��շ���Ļ��������ȣ�ͨ�������ļ��趨 */
		GM_RSSP1_UINT32                     buf_head_pos;       /**< ��ǰ��Ч���ݲ�������ڻ�����ͷ��ƫ���� */
		GM_RSSP1_UINT32                     valid_dat_len;      /**< ��ǰ��Ч���ݳ��� */

	}GM_RSSP1_PACKED FSFB_SCOM_chn_mng_struct;


	typedef struct
	{
		GM_RSSP1_UINT8                 index;
		FSFB_LNK_Com_Type_Enum          com_type;
		GM_RSSP1_UINT8                 UDP_chn_in_use;         /**< ��ǰ���õ�UDPͨ��������ͨ��������ʱ�л����������õ�UDPͨ�� */
		GM_RSSP1_UINT16                     chn_num;                            /**< ����UDPͨ������������FSFB_CFM_UDP_CHANNEL_NUM */
		FSFB_UDP_chn_mng_struct     com_chn[GM_RSSP1_CFM_MAX_CHANNEL_NUM];  /**< ��������UDPͨ�� */
		FSFB_UDP_sock_cfg_struct    UDP_sock_cfg;

		FSFB_SCOM_chn_mng_struct    scom_chn[GM_RSSP1_CFM_MAX_CHANNEL_NUM];
		FSFB_SCOM_com_cfg_struct    scom_cfg;

	}GM_RSSP1_PACKED FSFB_LNK_connection_struct;


	typedef struct
	{
		GM_RSSP1_library_state_enum         lib_state;
		GM_RSSP1_UINT16                     connection_nums;

		FSFB_LNK_connection_struct      connection[GM_RSSP1_MAX_SAFETY_CONNECTION_NUM];


		GM_SOCKET                     max_socket_value;       /**< ϵͳ������socket�����ֵ, ��ʼ��Ϊ0 */
		GM_RSSP1_UINT32                     max_scom_fd_value;      /**< ϵͳ�д������������ֵ */

		GM_RSSP1_UINT16                     net_task_num;
		FSFB_LNK_task_cfg_struct        net_task_cfg[GM_RSSP1_CFM_MAX_TASK_NUM];
		GM_Thread_Arg_Struct            task_args[GM_RSSP1_CFM_MAX_TASK_NUM];   /**< �洢����������� */

		struct sockaddr_in              local_addrs[GM_RSSP1_MAX_SAFETY_CONNECTION_NUM*GM_RSSP1_CFM_MAX_CHANNEL_NUM]; 
		GM_SOCKET                     local_socks[GM_RSSP1_MAX_SAFETY_CONNECTION_NUM*GM_RSSP1_CFM_MAX_CHANNEL_NUM];
		GM_RSSP1_INT32 vld_addr_num;

		GM_Thread_Arg_Struct            nonblk_task_arg;   /**< �洢������� */
		GM_Thread_Arg_Struct            blk_maintain_arg;
	} GM_RSSP1_PACKED FSFB_LNK_object_struct;

	/**
	* @brief CFMģ�鴴�������շ�����
	*
	* ��CFM��ʼ�����̵���.
	* @param[in] pCFM
	* @return GM_RSSP1_BOOL
	*/
	GM_RSSP1_BOOL FSFB_LNK_Create_Net_Task(FSFB_LNK_object_struct *pCFM);

	/**
	* @brief CFMģ���շ�UDP���ݺ���
	*
	* ������ͬʱ����UDP socket��ά��.
	* @param[in] pargs
	* @return FSFB_THREAD_FUNC_RETURN FSFB_THREAD_CALL
	*/
	GM_THREAD_FUNC_RETURN GM_THREAD_CALL FSFB_LNK_Dat_Rcv_Send(GM_Thread_Arg_Struct *pargs);
	GM_THREAD_FUNC_RETURN GM_THREAD_CALL FSFB_LNK_Dat_Rcv(GM_Thread_Arg_Struct *pargs);
	GM_THREAD_FUNC_RETURN GM_THREAD_CALL FSFB_LNK_Dat_Snd(GM_Thread_Arg_Struct *pargs);

	GM_RSSP1_BOOL FSFB_LNK_Init(char *file_path);

	GM_RSSP1_BOOL FSFB_Init_Comm_Cfg_LNK(char *init_file_path , GM_RSSP1_comm_cfg_struct *pCfg);

	GM_RSSP1_BOOL FSFB_Link_Init_Lib(GM_RSSP1_comm_cfg_struct *pcfg, FSFB_LNK_object_struct *pLNK);

	GM_RSSP1_BOOL FSFB_Init_Comm_Global_LNK(GM_RSSP1_comm_cfg_struct *pCfg , char *pIniFileContent);

	GM_RSSP1_BOOL FSFB_Init_Comm_Connection_LNK(GM_RSSP1_comm_cfg_struct *pCfg , char *pIniFileContent);

	GM_RSSP1_BOOL FSFB_Init_Comm_Task_LNK(GM_RSSP1_comm_cfg_struct *pCfg , char *pIniFileContent);

	extern GM_RSSP1_BOOL isRunning;

#ifdef __cplusplus
}
#endif

#endif