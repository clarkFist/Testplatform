#include "GM_Define.h"
#include "FSFB_UDP_Init.h"
#include "FSFB_CFM_UDP.h"


#ifdef LINK_Part

GM_RSSP1_UINT32 l_ip[GM_RSSP1_MAX_SAFETY_CONNECTION_NUM][GM_RSSP1_CFM_MAX_CHANNEL_NUM] = {0};

FSFB_LNK_object_struct g_LNK_obj;

GM_RSSP1_BOOL isRunning = GM_RSSP1_TRUE;

extern RSSP1_MQ_LINK_struct g_Link_MQ[GM_RSSP1_MAX_SAFETY_CONNECTION_NUM];

FSFB_LNK_object_struct *FSFB_Get_LNK_Object(void)
{
	return &g_LNK_obj;
}

GM_RSSP1_BOOL FSFB_LNK_Init(char *file_path)
{
	FSFB_LNK_object_struct *pLNK = NULL;
	GM_RSSP1_comm_cfg_struct fsfb_cfg;

	pLNK = FSFB_Get_LNK_Object();

	if (NULL == file_path)
	{
		return GM_RSSP1_FALSE;
	}

	if (GM_RSSP1_TRUE == FSFB_Init_Comm_Cfg_LNK(file_path, &fsfb_cfg))
	{
		if (GM_RSSP1_TRUE == FSFB_Link_Init_Lib(&fsfb_cfg , pLNK))
		{
			return GM_RSSP1_TRUE;
		}
	}
	return GM_RSSP1_FALSE;
}



GM_RSSP1_BOOL FSFB_Link_Init_Lib(GM_RSSP1_comm_cfg_struct *pcfg, FSFB_LNK_object_struct *pLNK)
{
	FSFB_LNK_connection_struct *pLNK_con        = NULL;
	FSFB_LNK_connection_cfg_struct *pConn_cfg   = NULL;
	GM_RSSP1_INT32 conn_index = 0;
	GM_RSSP1_INT32 chn_index = 0;
	GM_RSSP1_INT32 task_index = 0;
	char com_name[64] = {0};
#ifdef WIN32
	/** @brief 使用winsock必须加载WinSock DLL */
	WSADATA wsa;
	GM_RSSP1_UINT16 wVersionRequested;
	GM_RSSP1_INT32 err;

	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsa);

	if (err != 0)
	{
		WSACleanup();
		return GM_RSSP1_FALSE;
	}

#endif

	if ((NULL == pcfg) || (NULL == pLNK))
	{
		return GM_RSSP1_FALSE;
	}

	pLNK->connection_nums       = pcfg->LNK_cfg.connection_nums;

	/* init connections */

	for (conn_index = 0 ; conn_index < pLNK->connection_nums ; ++conn_index)
	{
		pConn_cfg = &(pcfg->LNK_cfg.connection_cfg[conn_index]);
		pLNK_con = &(pLNK->connection[conn_index]);

		pLNK_con->index        = conn_index;
		pLNK_con->com_type              = pConn_cfg->com_type;

		pLNK_con->UDP_chn_in_use       = 0; /* 默认主用0通道 */
		pLNK_con->chn_num          = pConn_cfg->chn_num;
		pLNK_con->UDP_sock_cfg         = pConn_cfg->UDP_sock_cfg;

		pLNK_con->chn_num          = pConn_cfg->chn_num;
		pLNK_con->scom_cfg          = pConn_cfg->scom_cfg;

		if(GM_RSSP1_CFM_COMM_TYPE_UDP == pLNK_con->com_type)
		{
			/* init UDP channel */
			for (chn_index = 0 ; chn_index < pLNK_con->chn_num ; ++chn_index)
			{
				/* init idle check flag */
				pLNK_con->com_chn[chn_index].b_connected    = GM_RSSP1_FALSE;
				pLNK_con->com_chn[chn_index].idle_counter   = 0;

				/* init ip,port */
				pLNK_con->com_chn[chn_index].addr_cfg  = pConn_cfg->UDP_chn_cfg[chn_index].addr_cfg;

				/* init socket */
				pLNK_con->com_chn[chn_index].sock  = GM_RSSP1_INVALID_SOCKET;


				pLNK_con->com_chn[chn_index].b_temp_send_pkt_valid      = GM_RSSP1_FALSE;

				pLNK_con->com_chn[chn_index].temp_send_pkt.bytes_count  = 0;
				GM_RSSP1_memset(pLNK_con->com_chn[chn_index].temp_send_pkt.byte , 0 , sizeof(pLNK_con->com_chn[chn_index].temp_send_pkt.byte));
			}
		}
	}

	/* TODO:init NET tasks */
	pLNK->net_task_num = pcfg->LNK_cfg.net_task_num;
	for (task_index = 0 ; task_index < pLNK->net_task_num ; ++task_index)
	{
		pLNK->net_task_cfg[task_index] = pcfg->LNK_cfg.net_task_cfg[task_index];
	}

	GM_RSSP1_memset(pLNK->task_args , 0 , sizeof(pLNK->task_args));

	if (GM_RSSP1_FALSE == FSFB_LNK_Create_Net_Task(pLNK))
	{
		return GM_RSSP1_FALSE;
	}

	return GM_RSSP1_TRUE;
}



GM_RSSP1_BOOL FSFB_Init_Comm_Cfg_LNK(char *init_file_path , GM_RSSP1_comm_cfg_struct *pCfg)
{
	GM_RSSP1_INT32 file_len                        = -1;
	char *pIniFileContent               = NULL; /**< 存储Ini文件内容, 动态分配, 使用完之后释放 */
	GM_RSSP1_BOOL rt                        = GM_RSSP1_FALSE;

	if ((NULL == init_file_path) || (NULL == pCfg))
	{
		return GM_RSSP1_FALSE;
	}

	/* 为读文件分配内存 */
	file_len = GM_RSSP1_Get_File_Size(init_file_path);

	if (-1 == file_len)
	{
		return GM_RSSP1_FALSE;
	}

	pIniFileContent = (char *)GM_RSSP1_malloc(file_len + 1);

	if (NULL == pIniFileContent)
	{
		return GM_RSSP1_FALSE;
	}

	GM_RSSP1_memset(pIniFileContent, 0, file_len + 1);

	/** @brief 初始化ini文件, 即读取其中的内容 */

	rt = GM_RSSP1_Ini_File_Load(init_file_path, pIniFileContent, file_len + 1);

	if (GM_RSSP1_FALSE == rt)
	{
		GM_RSSP1_free(pIniFileContent);
		pIniFileContent = NULL;

		return GM_RSSP1_FALSE;
	}

	/* 顺序不可变动。初始化各连接时需要从SFM cfg中获取本地连接通道共享的一些配置 */
	if (GM_RSSP1_FALSE == FSFB_Init_Comm_Global_LNK(pCfg, pIniFileContent))
	{
		return GM_RSSP1_FALSE;
	}

	if (GM_RSSP1_FALSE == FSFB_Init_Comm_Connection_LNK(pCfg, pIniFileContent))
	{
		return GM_RSSP1_FALSE;
	}

	if (GM_RSSP1_FALSE == FSFB_Init_Comm_Task_LNK(pCfg, pIniFileContent))
	{
		return GM_RSSP1_FALSE;
	}

	GM_RSSP1_free(pIniFileContent);

	pIniFileContent = NULL;

	return GM_RSSP1_TRUE;
}


GM_RSSP1_BOOL FSFB_Init_Comm_Global_LNK(GM_RSSP1_comm_cfg_struct *pCfg , char *pIniFileContent)
{
	GM_RSSP1_BOOL rt                        = GM_RSSP1_FALSE;
	GM_RSSP1_INT32 valueInt                        = -1;
	char valueString[256]               = {0};

	if ((NULL == pCfg) || (NULL == pIniFileContent))
	{
		return GM_RSSP1_FALSE;
	}

	/* RSSP1_GLOBAL 全局配置 */
	rt = GM_RSSP1_Ini_File_Read_Int("RSSP1_GLOBAL", "connection_num", &valueInt, pIniFileContent);

	if ((GM_RSSP1_FALSE == rt) || (valueInt < 0) || (valueInt > GM_RSSP1_MAX_SAFETY_CONNECTION_NUM))
	{
		return GM_RSSP1_FALSE;
	}
	else
	{
		pCfg->LNK_cfg.connection_nums = (GM_RSSP1_UINT16)valueInt;
	}

	rt = GM_RSSP1_Ini_File_Read_Int("RSSP1_GLOBAL", "net_task_num", &valueInt, pIniFileContent);

	if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
	{
		return GM_RSSP1_FALSE;
	}
	else
	{
		pCfg->LNK_cfg.net_task_num = (GM_RSSP1_UINT16)valueInt;
	}

	return GM_RSSP1_TRUE;
}


GM_RSSP1_BOOL FSFB_Init_Comm_Connection_LNK(GM_RSSP1_comm_cfg_struct *pCfg , char *pIniFileContent)
{
	GM_RSSP1_BOOL rt                             = GM_RSSP1_FALSE;
	GM_RSSP1_INT32 valueInt                        = -1;
	char valueString[256]               = {0};
	GM_RSSP1_BOOL valueBool                      = GM_RSSP1_FALSE;
	char section_key[256]               = {0};   /* 连接通道键名 */
	char sub_section_key[256]           = {0};   /* UDP通道键名 */
	GM_RSSP1_INT32 key_len                         = 0;
	GM_RSSP1_INT32 sub_key_len                     = 0;
	GM_RSSP1_INT32 conn_index                      = 0;
	GM_RSSP1_INT32 chn_index                   = 0;
	GM_RSSP1_INT32 fsfb_chn_index                  = 0;

	FSFB_LNK_connection_cfg_struct *pLNK_conn_cfg           = NULL;

	if ((NULL == pCfg) || (NULL == pIniFileContent))
	{
		return GM_RSSP1_FALSE;
	}

	/* 各个连接通道配置 */
	key_len = sizeof("CON_") - 1;

	GM_RSSP1_memcpy(section_key, "CON_", key_len + 1);

	for (conn_index = 0; conn_index < pCfg->LNK_cfg.connection_nums; ++conn_index)
	{
		pLNK_conn_cfg = &(pCfg->LNK_cfg.connection_cfg[conn_index]);

		GM_RSSP1_memset(section_key + key_len, 0, sizeof(section_key) - key_len);
		sprintf(section_key + key_len, "%d", conn_index);

		/* init com type */
		rt = GM_RSSP1_Ini_File_Read_Int(section_key, "com_type", &valueInt, pIniFileContent);

		if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
		{
			return GM_RSSP1_FALSE;
		}
		else
		{
			if(0 == valueInt)
			{
				pLNK_conn_cfg->com_type = GM_RSSP1_CFM_COMM_TYPE_UDP;
			}
			else if(1 == valueInt)
			{
				pLNK_conn_cfg->com_type = GM_RSSP1_CFM_COMM_TYPE_SCOM;
			}
			else
			{
				pLNK_conn_cfg->com_type = GM_RSSP1_CFM_COMM_TYPE_UNKNOW;
			}
		}

		if(GM_RSSP1_CFM_COMM_TYPE_UDP == pLNK_conn_cfg->com_type)
		{
			/* init UDP channel param */
			rt = GM_RSSP1_Ini_File_Read_Int(section_key, "UDP_channel_num", &valueInt, pIniFileContent);

			if ((GM_RSSP1_FALSE == rt) || (valueInt < 0) || (valueInt > GM_RSSP1_CFM_MAX_CHANNEL_NUM))
			{
				return GM_RSSP1_FALSE;
			}
			else
			{
				pLNK_conn_cfg->chn_num = (GM_RSSP1_UINT16)valueInt;
			}

			for (chn_index = 0 ; chn_index < pLNK_conn_cfg->chn_num ; ++chn_index)
			{
				sub_key_len = sizeof("local_ip_") - 1;
				GM_RSSP1_memcpy(sub_section_key, "local_ip_", sub_key_len + 1);
				GM_RSSP1_memset(sub_section_key + sub_key_len, 0, 2);
				sprintf(sub_section_key + sub_key_len, "%d", chn_index);
				rt = GM_RSSP1_Ini_File_Read_String(section_key, sub_section_key, valueString, sizeof(valueString), pIniFileContent);

				if (GM_RSSP1_FALSE == rt)
				{
					return GM_RSSP1_FALSE;
				}
				else
				{
					GM_RSSP1_memcpy(pLNK_conn_cfg->UDP_chn_cfg[chn_index].addr_cfg.local_ip, valueString, strlen(valueString) + 1);
					l_ip[conn_index][chn_index] = inet_addr(pLNK_conn_cfg->UDP_chn_cfg[chn_index].addr_cfg.local_ip);
				}

				sub_key_len = sizeof("local_port_") - 1;

				GM_RSSP1_memcpy(sub_section_key, "local_port_", sub_key_len + 1);
				GM_RSSP1_memset(sub_section_key + sub_key_len, 0, 2);
				sprintf(sub_section_key + sub_key_len, "%d", chn_index);
				rt = GM_RSSP1_Ini_File_Read_Int(section_key, sub_section_key, &valueInt, pIniFileContent);

				if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
				{
					return GM_RSSP1_FALSE;
				}
				else
				{
					pLNK_conn_cfg->UDP_chn_cfg[chn_index].addr_cfg.local_port = (GM_RSSP1_UINT16)valueInt;
				}

				GM_RSSP1_memset(&(pLNK_conn_cfg->UDP_chn_cfg[chn_index].addr_cfg.local_addr) , 0 , sizeof(pLNK_conn_cfg->UDP_chn_cfg[chn_index].addr_cfg.local_addr));

				pLNK_conn_cfg->UDP_chn_cfg[chn_index].addr_cfg.local_addr.sin_family = AF_INET;
				pLNK_conn_cfg->UDP_chn_cfg[chn_index].addr_cfg.local_addr.sin_addr.s_addr = inet_addr(pLNK_conn_cfg->UDP_chn_cfg[chn_index].addr_cfg.local_ip);

				if (-1 == pLNK_conn_cfg->UDP_chn_cfg[chn_index].addr_cfg.local_addr.sin_addr.s_addr)
				{
					return GM_RSSP1_FALSE;
				}

				pLNK_conn_cfg->UDP_chn_cfg[chn_index].addr_cfg.local_addr.sin_port = htons(pLNK_conn_cfg->UDP_chn_cfg[chn_index].addr_cfg.local_port);


				sub_key_len = sizeof("remote_ip_") - 1;
				GM_RSSP1_memcpy(sub_section_key, "remote_ip_", sub_key_len + 1);
				GM_RSSP1_memset(sub_section_key + sub_key_len, 0, 2);
				sprintf(sub_section_key + sub_key_len, "%d", chn_index);
				rt = GM_RSSP1_Ini_File_Read_String(section_key, sub_section_key, valueString, sizeof(valueString), pIniFileContent);

				if (GM_RSSP1_FALSE == rt)
				{
					return GM_RSSP1_FALSE;
				}
				else
				{
					GM_RSSP1_memcpy(pLNK_conn_cfg->UDP_chn_cfg[chn_index].addr_cfg.remote_ip, valueString, sizeof(pLNK_conn_cfg->UDP_chn_cfg[chn_index].addr_cfg.remote_ip));
				}

				sub_key_len = sizeof("remote_port_") - 1;

				GM_RSSP1_memcpy(sub_section_key, "remote_port_", sub_key_len + 1);
				GM_RSSP1_memset(sub_section_key + sub_key_len, 0, 2);
				sprintf(sub_section_key + sub_key_len, "%d", chn_index);
				rt = GM_RSSP1_Ini_File_Read_Int(section_key, sub_section_key, &valueInt, pIniFileContent);

				if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
				{
					return GM_RSSP1_FALSE;
				}
				else
				{
					pLNK_conn_cfg->UDP_chn_cfg[chn_index].addr_cfg.remote_port = (GM_RSSP1_UINT16)valueInt;
				}

				GM_RSSP1_memset(&(pLNK_conn_cfg->UDP_chn_cfg[chn_index].addr_cfg.remote_addr) , 0 , sizeof(pLNK_conn_cfg->UDP_chn_cfg[chn_index].addr_cfg.remote_addr));

				pLNK_conn_cfg->UDP_chn_cfg[chn_index].addr_cfg.remote_addr.sin_family = AF_INET;
				pLNK_conn_cfg->UDP_chn_cfg[chn_index].addr_cfg.remote_addr.sin_addr.s_addr = inet_addr(pLNK_conn_cfg->UDP_chn_cfg[chn_index].addr_cfg.remote_ip);

				if (-1 == pLNK_conn_cfg->UDP_chn_cfg[chn_index].addr_cfg.remote_addr.sin_addr.s_addr)
				{
					return GM_RSSP1_FALSE;
				}

				pLNK_conn_cfg->UDP_chn_cfg[chn_index].addr_cfg.remote_addr.sin_port = htons(pLNK_conn_cfg->UDP_chn_cfg[chn_index].addr_cfg.remote_port);
			}

			rt = GM_RSSP1_Ini_File_Read_Int(section_key, "check_idle_cycle", &valueInt, pIniFileContent);

			if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
			{
				return GM_RSSP1_FALSE;
			}
			else
			{
				pLNK_conn_cfg->UDP_sock_cfg.check_idle_cycle = (GM_RSSP1_UINT16)valueInt;
			}

			rt = GM_RSSP1_Ini_File_Read_Int(section_key, "maintain_cycle", &valueInt, pIniFileContent);

			if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
			{
				return GM_RSSP1_FALSE;
			}
			else
			{
				pLNK_conn_cfg->UDP_sock_cfg.maintain_cycle = (GM_RSSP1_UINT16)valueInt;
			}

			rt = GM_RSSP1_Ini_File_Read_Int(section_key, "packet_send_delay", &valueInt, pIniFileContent);

			if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
			{
				return GM_RSSP1_FALSE;
			}
			else
			{
				pLNK_conn_cfg->UDP_sock_cfg.packet_send_delay = (GM_RSSP1_UINT16)valueInt;
			}

			/* init socket buf. for setsockopt() */
			rt = GM_RSSP1_Ini_File_Read_Int(section_key, "sock_send_buf_size", &valueInt, pIniFileContent);

			if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
			{
				return GM_RSSP1_FALSE;
			}
			else
			{
				pLNK_conn_cfg->UDP_sock_cfg.sock_send_buf_size = (GM_RSSP1_UINT32)valueInt;
			}

			rt = GM_RSSP1_Ini_File_Read_Int(section_key, "sock_recv_buf_size", &valueInt, pIniFileContent);

			if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
			{
				return GM_RSSP1_FALSE;
			}
			else
			{
				pLNK_conn_cfg->UDP_sock_cfg.sock_recv_buf_size = (GM_RSSP1_UINT32)valueInt;
			}
		}
	}

	return GM_RSSP1_TRUE;
}



GM_RSSP1_BOOL FSFB_Init_Comm_Task_LNK(GM_RSSP1_comm_cfg_struct *pCfg , char *pIniFileContent)
{
	GM_RSSP1_BOOL rt                             = GM_RSSP1_FALSE;
	GM_RSSP1_INT32 valueInt                        = -1;
	char section_key[256]               = {0};   /* 任务键名 */
	char sub_section_key[256]           = {0};   /* 安全连接通道键名 */
	GM_RSSP1_INT32 key_len                         = 0;
	GM_RSSP1_INT32 sub_key_len                     = 0;
	GM_RSSP1_INT32 task_index                      = 0;
	GM_RSSP1_INT32 proc_conn_index                 = 0;

	FSFB_LNK_task_cfg_struct *pTaskCfg  = NULL;

	if ((NULL == pCfg) || (NULL == pIniFileContent))
	{
		return GM_RSSP1_FALSE;
	}

	/* 各个任务配置 */
	key_len = sizeof("TASK_") - 1;

	GM_RSSP1_memcpy(section_key, "TASK_", key_len + 1);

	for (task_index = 0; task_index < pCfg->LNK_cfg.net_task_num; ++task_index)
	{
		GM_RSSP1_memset(section_key + key_len, 0, sizeof(section_key) - key_len);
		sprintf(section_key + key_len, "%d", task_index);

		rt = GM_RSSP1_Ini_File_Read_Int(section_key, "sleep_time", &valueInt, pIniFileContent);

		if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
		{
			return GM_RSSP1_FALSE;
		}
		else
		{
			pCfg->LNK_cfg.net_task_cfg[task_index].sleep_time = (GM_RSSP1_UINT16)valueInt;
		}

		rt = GM_RSSP1_Ini_File_Read_Int(section_key, "stack_size", &valueInt, pIniFileContent);

		if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
		{
			return GM_RSSP1_FALSE;
		}
		else
		{
			pCfg->LNK_cfg.net_task_cfg[task_index].stack_size = (GM_RSSP1_UINT16)valueInt;
		}

		rt = GM_RSSP1_Ini_File_Read_Int(section_key, "priority", &valueInt, pIniFileContent);

		if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
		{
			return GM_RSSP1_FALSE;
		}
		else
		{
			pCfg->LNK_cfg.net_task_cfg[task_index].priority = (GM_RSSP1_UINT16)valueInt;
		}

		rt = GM_RSSP1_Ini_File_Read_Int(section_key, "connection_num", &valueInt, pIniFileContent);

		if ((GM_RSSP1_FALSE == rt) || (valueInt < 0))
		{
			return GM_RSSP1_FALSE;
		}
		else
		{
			pCfg->LNK_cfg.net_task_cfg[task_index].connection_num = (GM_RSSP1_UINT16)valueInt;
		}

		for (proc_conn_index = 0 ; proc_conn_index < pCfg->LNK_cfg.net_task_cfg[task_index].connection_num ; ++proc_conn_index)
		{
			sub_key_len = sizeof("connection_index_") - 1;
			GM_RSSP1_memcpy(sub_section_key, "connection_index_", sub_key_len + 1);
			GM_RSSP1_memset(sub_section_key + sub_key_len, 0, 2);
			sprintf(sub_section_key + sub_key_len, "%d", proc_conn_index);

			rt = GM_RSSP1_Ini_File_Read_Int(section_key, sub_section_key, &valueInt, pIniFileContent);

			if ((GM_RSSP1_FALSE == rt) || (valueInt < 0) || (valueInt >= pCfg->LNK_cfg.connection_nums))
			{
				return GM_RSSP1_FALSE;
			}
			else
			{
				pCfg->LNK_cfg.net_task_cfg[task_index].connection_index[proc_conn_index] = (GM_RSSP1_UINT16)valueInt;
			}

		}

	}

	return GM_RSSP1_TRUE;
}


GM_RSSP1_BOOL FSFB_LNK_Create_Net_Task(FSFB_LNK_object_struct *pLNK)
{
	GM_RSSP1_INT32 task_index                      = 0;
	FSFB_LNK_task_cfg_struct *ptask_cfg = NULL;
	char task_name[64]                 = {0};
	GM_RSSP1_INT32 tid                             = 0;
	FSFB_LNK_task_cfg_struct *p_rcv_task_cfg = NULL;
	FSFB_LNK_task_cfg_struct *p_snd_task_cfg = NULL;

	if (NULL == pLNK)
	{
		return GM_RSSP1_FALSE;
	}

	for (task_index = 0 ; task_index < pLNK->net_task_num ; ++task_index)
	{
		ptask_cfg = &(pLNK->net_task_cfg[task_index]);
		pLNK->task_args[task_index].args0   = (GM_RSSP1_INT32)(pLNK);
		pLNK->task_args[task_index].args1   = (GM_RSSP1_INT32)(ptask_cfg);

		memset(task_name, 0, sizeof(task_name));
		sprintf(task_name, "net_task_%d", task_index);
		GM_RSSP1_memcpy(ptask_cfg->tsk_name,task_name,sizeof(task_name));

		if (GM_RSSP1_TRUE != GM_Thread_Create((GM_THREAD *)(&tid), (GM_THREAD_FUNC)FSFB_LNK_Dat_Rcv_Send, task_name,
			ptask_cfg->stack_size, 0, ptask_cfg->priority, &(pLNK->task_args[task_index])))
		{
			pLNK->lib_state = GM_RSSP1_Lib_State_Failure;;
			return GM_RSSP1_FALSE;
		}
		else
		{
			pLNK->lib_state = GM_RSSP1_Lib_State_Operational;
//			GM_RSSP1_Log_Msg(0,"FSFB Create %s OK ! ID = 0x%x.\n", (GM_RSSP1_INT32)task_name, tid, 0, 0, 0, 0);
		}
	}

	return GM_RSSP1_TRUE;
}


GM_THREAD_FUNC_RETURN GM_THREAD_CALL FSFB_LNK_Dat_Rcv_Send(GM_Thread_Arg_Struct *pargs)
{
	FSFB_LNK_task_cfg_struct *ptask_cfg = NULL;
	FSFB_LNK_object_struct *pLNK        = NULL;
	FSFB_LNK_connection_struct *pLNK_con = NULL;
	GM_RSSP1_INT32 task_index = 0;
	GM_RSSP1_INT32 con_index = 0;
	GM_RSSP1_BOOL rd_ready  = GM_RSSP1_FALSE;
	GM_RSSP1_BOOL wt_ready  = GM_RSSP1_FALSE;

	if (NULL == pargs)
	{
		return GM_RSSP1_FALSE;
	}

	pLNK        = (FSFB_LNK_object_struct *)(pargs->args0);
	ptask_cfg   = (FSFB_LNK_task_cfg_struct *)(pargs->args1);

	if (NULL == pLNK)
	{
		return GM_RSSP1_FALSE;
	}

	if (GM_RSSP1_Lib_State_Failure == pLNK->lib_state)
	{
		return GM_RSSP1_FALSE;
	}

	if (NULL == ptask_cfg)
	{
		return GM_RSSP1_FALSE;
	}

	while (isRunning)
	{
		for (con_index = 0; con_index < ptask_cfg->connection_num ; ++con_index)
		{
			pLNK_con   = &(pLNK->connection[ptask_cfg->connection_index[con_index]]);

			if(GM_RSSP1_CFM_COMM_TYPE_UDP == pLNK_con->com_type)
			{
				FSFB_CFM_Con_UDP_Maintain(pLNK_con , pLNK, con_index);
				FSFB_CFM_Con_UDP_Recv_Dat(pLNK_con , pLNK, con_index);
				FSFB_CFM_Con_UDP_Send_Dat(pLNK_con , pLNK, con_index);
			}
		}

		GM_SLEEP(ptask_cfg->sleep_time);
	}
}
#endif
