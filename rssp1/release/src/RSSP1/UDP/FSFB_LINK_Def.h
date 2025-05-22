
/************************************************************************/
/* UDP, SCOM模块定义                               */
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

	#define GM_RSSP1_CFM_MAX_CONNECTIONS_FOR_PROCESS    100     /**< 一个线程处理的最大连接数*/
	#define GM_RSSP1_TASK_MAX_SOCK_NUM                  (2*GM_RSSP1_CFM_MAX_CONNECTIONS_FOR_PROCESS)    /* 一个CFM连接最多双通道 */
	#define GM_RSSP1_SOCKET_FD_SET_NUM                  (GM_RSSP1_TASK_MAX_SOCK_NUM/FD_SETSIZE + 1) /* 一个任务最多可一次select GM_RSSP1_TASK_MAX_SOCK_NUM个socket */
	#define GM_RSSP1_CFM_MAX_TASK_NUM                   20      /**< 最多启动的任务数 */

	#define GM_RSSP1_SOCKET_RECV_SELECT_DELAY       0      /**< 接收数据, select延时, ms @NOTE:在连接数多、周期数据量大时必须设为0, 防止socket异常但无法检测出时, 频繁延时导致阻塞! */
	#define GM_RSSP1_SOCKET_SEND_SELECT_DELAY       0      /**< 发送数据, select延时, ms @NOTE:在连接数多、周期数据量大时必须设为0, 防止socket异常但无法检测出时, 频繁延时导致阻塞!*/
	#define GM_RSSP1_SOCKET_SEND_RETRY_DELAY        1      /**<  发送数据, socket中途不可写时, 重试间隔延时 */

	#define GM_RSSP1_SOCKET_MAINTAIN_TASK_PRIORITY      200 /**< socket维护任务的优先级 */

	typedef enum
	{
		FSFB_COMM_TYPE_UDP           = 0x1111, /* 底层采用udp通信 */
		FSFB_COMM_TYPE_SCOM           = 0x2222, /* 底层采用串口通信 */
		FSFB_COMM_TYPE_UNKNOW           = 0xffff /* 底层采用串口通信 */
	} FSFB_LNK_Com_Type_Enum;

	/** 网络传输地址配置结构 */

	typedef struct
	{
		char                local_ip[20];     /**<  本地IP地址 */
		GM_RSSP1_UINT16         local_port;       /**<  本地端口 */

		struct sockaddr_in  local_addr;

		char                remote_ip[20];     /**<  远程IP地址 */
		GM_RSSP1_UINT16         remote_port;       /**<  远程端口 */

		struct sockaddr_in  remote_addr;
	} GM_RSSP1_PACKED FSFB_transport_config_struct;

	typedef struct
	{
		FSFB_transport_config_struct    addr_cfg;           /**< IP，port配置 */
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
		GM_RSSP1_UINT16                     packet_send_delay;      /**< 数据包之间发送间隔，防止流式传输时的粘包现象 RSSP1 3.6.1.2 */
		GM_RSSP1_UINT16                     maintain_cycle;         /**< 维护周期，以调用维护函数的任务周期值为单位。 */
	}GM_RSSP1_PACKED FSFB_SCOM_com_cfg_struct;

    typedef struct
    {
        char                            tsk_name[100];
        GM_RSSP1_UINT16                     sleep_time;             /**< 任务每周期休眠时间, ms.计算公式:500/每周期应用层发送的最大数据包数量 - 余量 */
        GM_RSSP1_UINT16                     priority;               /**< 任务优先级 */
        GM_RSSP1_UINT32                     stack_size;             /**< 任务堆栈大小 */
        GM_RSSP1_UINT16                     pkt_send_interval;      /**< 数据包发送间隔 ms */
        GM_RSSP1_UINT16                     connection_num;         /**< 本任务负责处理的连接数*/
        GM_RSSP1_UINT16                     connection_index[GM_RSSP1_CFM_MAX_CONNECTIONS_FOR_PROCESS]; /**< 本任务负责处理的连接下标，与配置中的各连接相匹配 */
#ifdef RSSP1_USE_OPTIMIZE_SELECT
        fd_set                          sock_read_set[GM_RSSP1_SOCKET_FD_SET_NUM];      /* 读集合 */
        fd_set                          sock_write_set[GM_RSSP1_SOCKET_FD_SET_NUM];     /* 写集合 */
#endif
    } GM_RSSP1_PACKED FSFB_LNK_task_cfg_struct;

	typedef struct
	{
		GM_RSSP1_UINT32                     sock_send_buf_size;
		GM_RSSP1_UINT32                     sock_recv_buf_size;
		GM_RSSP1_UINT16                     check_idle_cycle;
		GM_RSSP1_UINT16                     packet_send_delay;      /**< 数据包之间发送间隔，防止流式传输时的粘包现象 RSSP1 3.6.1.2 */
		GM_RSSP1_UINT16                     maintain_cycle;         /**< socket维护周期，以调用维护函数的任务周期值为单位。 */
	}GM_RSSP1_PACKED FSFB_UDP_sock_cfg_struct;

	typedef struct
	{
		GM_RSSP1_INT32                      com_port_no;               /**< 串口号 */
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
		GM_RSSP1_UINT16                     connection_nums;    /* 与SFM中的连接一一对应 */
		FSFB_LNK_connection_cfg_struct  connection_cfg[GM_RSSP1_MAX_SAFETY_CONNECTION_NUM];

		GM_RSSP1_UINT16                     net_task_num;
		FSFB_LNK_task_cfg_struct        net_task_cfg[GM_RSSP1_CFM_MAX_TASK_NUM];

	} GM_RSSP1_PACKED FSFB_LNK_cfg_struct;

#ifdef __cplusplus
}
#endif

#endif