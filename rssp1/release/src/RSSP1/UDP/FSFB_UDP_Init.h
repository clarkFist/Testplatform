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
	/** 一个UDP通道的结构定义 */
	#ifdef WIN32
		typedef GM_RSSP1_INT16 GM_SOCKET;
	#endif

	#ifdef WIN32
			/* 字节对齐 */
	#pragma pack(1) /* 以单字节对齐 */
	#define FSFB_PACKED

			/*socket*/
	#define FSFB_SOCKET                 GM_INT16
	#define FSFB_SOCKET_OK              0
	#define FSFB_SOCKET_ERROR           (-1)
	#define FSFB_MSG_NOSIGNAL           0               /**< send函数最后一个参数, windows下一般设置为0  */
	#define FSFB_MSG_DONTWAIT           0

	#define FSFB_SOCKET_ERR_WOULDBLOCK WSAEWOULDBLOCK
	#define FSFB_SOCKET_ERR_AGAIN      WSAEWOULDBLOCK
	#define FSFB_SOCKET_ERR_INPROGRESS WSAEINPROGRESS   /* Operation now in progress. */
	#define FSFB_SOCKET_ERR_ISCONN     WSAEISCONN      /* Socket is already connected */
	#define FSFB_SOCKET_ERR_INVAL       WSAEINVAL  /* socket已经绑定 */
	#define FSFB_SOCKET_ERR_NOBUFS      WSAENOBUFS
	#define FSFB_SOKC_ERR_EADDRINUSE    WSAEADDRINUSE
	#define FSFB_SOCK_STREAM           SOCK_STREAM
	#define FSFB_SOCK_DGRAM            SOCK_DGRAM
	#define FSFB_SOCK_RAW              SOCK_RAW

	#define FSFB_GET_LAST_ERRNO      WSAGetLastError()
	#endif /*WIN32*/



	typedef struct
	{
		GM_RSSP1_BOOL                       b_connected;        /**< 已经连接上(UDP无连接，规定在宽恕周期check_idle_cycle内没有收到任何数据时，则视为断开) */
		GM_RSSP1_INT32                      idle_counter;       /**< 未收到数据计数 */

		GM_SOCKET                     sock;

		FSFB_transport_config_struct    addr_cfg;           /**< IP，port配置 */

		GM_RSSP1_com_pri_struct    temp_send_pkt;      /**< 从发送队列中取出一包，暂存在此，防止取出后socket不满足发送条件导致丢包 */
		GM_RSSP1_BOOL                       b_temp_send_pkt_valid;

		GM_RSSP1_Msg_Queue_struct           send_udp_dat_Q;     /**< 待发送的UDP数据包队列，其他任务放入，由收发任务发送 */
		GM_RSSP1_Msg_Queue_struct           recv_udp_dat_Q;     /**< UDP socket接收到的数据包队列，收发任务从网络接收到数据后存入，由其他任务处理 */
	}GM_RSSP1_PACKED FSFB_UDP_chn_mng_struct;

	/** 一个SCOM通道的结构定义 */

	typedef struct
	{
		GM_RSSP1_BOOL                       b_connected;        /**< 已经连接上(串口无连接，规定在宽恕周期check_idle_cycle内没有收到任何数据时，则视为断开) */
		GM_RSSP1_INT32                             idle_counter;       /**< 未收到数据计数 */

		GM_RSSP1_INT32                             com_handle;             /**< 串口资源句柄 */

		GM_RSSP1_INT32                      com_port_no;               /**< 串口号 */
		GM_RSSP1_UINT32                     Baud_rate;

		GM_RSSP1_com_pri_struct    temp_send_pkt;      /**< 从发送队列中取出一包，暂存在此，防止取出后串口不满足发送条件导致丢包 */
		GM_RSSP1_BOOL                       b_temp_send_pkt_valid;

		GM_RSSP1_Msg_Queue_struct           send_scom_dat_Q;     /**< 待发送的数据包队列，其他任务放入，由收发任务发送 */
		GM_RSSP1_Msg_Queue_struct           recv_scom_dat_Q;     /**< 接收到的数据包队列，收发任务从串口接收到数据后存入，由其他任务处理 */
		unsigned char                   *p_recv_buf;        /**< 按流数据提取 */
		GM_RSSP1_UINT16                     recv_buf_size;      /**< 为串口接收分配的缓冲区长度，通过配置文件设定 */
		GM_RSSP1_UINT32                     buf_head_pos;       /**< 当前有效数据部分相对于缓冲区头的偏移量 */
		GM_RSSP1_UINT32                     valid_dat_len;      /**< 当前有效数据长度 */

	}GM_RSSP1_PACKED FSFB_SCOM_chn_mng_struct;


	typedef struct
	{
		GM_RSSP1_UINT8                 index;
		FSFB_LNK_Com_Type_Enum          com_type;
		GM_RSSP1_UINT8                 UDP_chn_in_use;         /**< 当前主用的UDP通道，主用通道不可用时切换到其他可用的UDP通道 */
		GM_RSSP1_UINT16                     chn_num;                            /**< 冗余UDP通道数，不大于FSFB_CFM_UDP_CHANNEL_NUM */
		FSFB_UDP_chn_mng_struct     com_chn[GM_RSSP1_CFM_MAX_CHANNEL_NUM];  /**< 所有冗余UDP通道 */
		FSFB_UDP_sock_cfg_struct    UDP_sock_cfg;

		FSFB_SCOM_chn_mng_struct    scom_chn[GM_RSSP1_CFM_MAX_CHANNEL_NUM];
		FSFB_SCOM_com_cfg_struct    scom_cfg;

	}GM_RSSP1_PACKED FSFB_LNK_connection_struct;


	typedef struct
	{
		GM_RSSP1_library_state_enum         lib_state;
		GM_RSSP1_UINT16                     connection_nums;

		FSFB_LNK_connection_struct      connection[GM_RSSP1_MAX_SAFETY_CONNECTION_NUM];


		GM_SOCKET                     max_socket_value;       /**< 系统启动的socket的最大值, 初始化为0 */
		GM_RSSP1_UINT32                     max_scom_fd_value;      /**< 系统中串口描述符最大值 */

		GM_RSSP1_UINT16                     net_task_num;
		FSFB_LNK_task_cfg_struct        net_task_cfg[GM_RSSP1_CFM_MAX_TASK_NUM];
		GM_Thread_Arg_Struct            task_args[GM_RSSP1_CFM_MAX_TASK_NUM];   /**< 存储所有任务参数 */

		struct sockaddr_in              local_addrs[GM_RSSP1_MAX_SAFETY_CONNECTION_NUM*GM_RSSP1_CFM_MAX_CHANNEL_NUM]; 
		GM_SOCKET                     local_socks[GM_RSSP1_MAX_SAFETY_CONNECTION_NUM*GM_RSSP1_CFM_MAX_CHANNEL_NUM];
		GM_RSSP1_INT32 vld_addr_num;

		GM_Thread_Arg_Struct            nonblk_task_arg;   /**< 存储任务参数 */
		GM_Thread_Arg_Struct            blk_maintain_arg;
	} GM_RSSP1_PACKED FSFB_LNK_object_struct;

	/**
	* @brief CFM模块创建网络收发任务
	*
	* 由CFM初始化过程调用.
	* @param[in] pCFM
	* @return GM_RSSP1_BOOL
	*/
	GM_RSSP1_BOOL FSFB_LNK_Create_Net_Task(FSFB_LNK_object_struct *pCFM);

	/**
	* @brief CFM模块收发UDP数据函数
	*
	* 本函数同时负责UDP socket的维护.
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