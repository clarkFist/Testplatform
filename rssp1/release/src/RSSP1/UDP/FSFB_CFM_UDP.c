
#include "GM_Define.h"
#include "FSFB_CFM_UDP.h"
#include "GM_RSSP1_Msg_Queue.h"
#include "GM_RSSP1_MQ_Interface.h"
#include "GlueData.h"

#ifdef LINK_Part
extern RSSP1_MQ_LINK_struct g_Link_MQ[GM_RSSP1_MAX_SAFETY_CONNECTION_NUM];
extern GM_RSSP1_UINT32 l_ip[GM_RSSP1_MAX_SAFETY_CONNECTION_NUM][GM_RSSP1_CFM_MAX_CHANNEL_NUM];
GM_RSSP1_UINT32 g_con = 0;
GM_RSSP1_UINT32 g_chn = 0;

GM_RSSP1_UINT8               Copy_byte[GM_RSSP1_MAX_SAFETY_DAT_LEN];     /**复制*/

GM_RSSP1_INT32 FSFB_Create_Socket(GM_RSSP1_INT32 type, GM_SOCKET *max_socket_value)
{
    GM_RSSP1_INT32 new_socket = GM_RSSP1_INVALID_SOCKET;

    if (NULL == max_socket_value)
    {
        return GM_RSSP1_INVALID_SOCKET;
    }

    new_socket = socket(AF_INET, type, 0);

    if (new_socket < 0)
    {
        return GM_RSSP1_INVALID_SOCKET;
    }

    if (*max_socket_value < (GM_SOCKET)new_socket)
    {
        *max_socket_value = (GM_SOCKET)new_socket;
    }

    return new_socket;
}

GM_RSSP1_INT32 FSFB_Close_Socket(GM_SOCKET *pSock)
{
    GM_RSSP1_INT32 rt = -1;

    if (NULL == pSock)
    {
        return -1;
    }

    if (0 > (*pSock))
    {
        return -1;
    }

#ifdef WIN32
    shutdown(*pSock, 2);

    rt = closesocket(*pSock);

#else
    shutdown(*pSock, 2);

    rt = close(*pSock);

#endif

    *pSock = GM_RSSP1_INVALID_SOCKET;

    return rt;
}

GM_RSSP1_INT32 FSFB_Set_Socket_Option(GM_SOCKET sock, GM_RSSP1_INT32 socket_send_buf_len , GM_RSSP1_INT32 socket_recv_buf_len)
{
    GM_RSSP1_UINT32 arg                      = 1;
    GM_RSSP1_INT32 bReuseaddr                  = 1; /* 0: 不允许端口重用 .1 允许 */
    GM_RSSP1_INT32 bReuseport                  = 1;
    GM_RSSP1_INT32 len                         = 0;
    GM_RSSP1_INT32 opt_send_buf_len            = 0;
    GM_RSSP1_INT32 opt_recv_buf_len            = 0;
    GM_RSSP1_INT32 opt_tcp_send_no_delay       = 1;
    struct timeval opt_send_timeout;
    struct timeval opt_recv_timeout;
	GM_RSSP1_BOOL bNewBehavior			= GM_RSSP1_FALSE;
	GM_RSSP1_INT32 dwBytesReturned				= 0;

    if (0 > sock)
    {
        return GM_RSSP1_ERROR;
    }
    /* 公共属性设置 TODO:vxworks下需要设置最大文件描述符数目,默认为50.可通过iosFdShow查看. */
    /* 设置非阻塞 */
#ifdef WIN32
    if (GM_RSSP1_ERROR == ioctlsocket(sock, FIONBIO, &arg))
    {
        return GM_RSSP1_ERROR;
    }
	WSAIoctl(sock, _WSAIOW(IOC_VENDOR, 12), &bNewBehavior, sizeof(bNewBehavior), NULL, 0, &dwBytesReturned, NULL, NULL);
#endif
#ifdef OS_VXWORKS
    if (GM_RSSP1_ERROR == ioctl(sock, FIONBIO, (GM_RSSP1_INT32)&arg))
    {
        return GM_RSSP1_ERROR;
    }

#endif
#ifdef LINUX
    if (GM_RSSP1_ERROR == fcntl(sock, F_SETFL, O_NONBLOCK))
    {
        return GM_RSSP1_ERROR;
    }
#endif

    /*  注意：在Linux系统中,如果一个socket绑定了某个端口,该socket正常关闭或程序退出后,在一段时间内该端口依然保持被绑定的状态,其他程序（或者重新启动的原程序）无法绑定该端口.可以通过调用以下语句避免该问题  */
    if (GM_RSSP1_ERROR == setsockopt(sock, SOL_SOCKET , SO_REUSEADDR, (char *)&bReuseaddr, sizeof(bReuseaddr)))
    {
//        GM_RSSP1_Log_Msg(0,"Fatal GM_RSSP1_ERROR! set server socket SO_REUSEADDR option fail! errno:%d.\n", FSFB_GET_LAST_ERRNO, 0, 0, 0, 0, 0);
        return GM_RSSP1_ERROR;
    }
#ifndef WIN32    
    /* 允许重复绑定端口 jianghongjun mod 2010-10-15 */
    if (GM_RSSP1_ERROR == setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (char *)&bReuseport, sizeof(bReuseport)))
    {
        GM_RSSP1_Log_Msg(0,"Fatal GM_RSSP1_ERROR! set server socket SO_REUSEPORT option fail! errno:%d.\n", FSFB_GET_LAST_ERRNO, 0, 0, 0, 0, 0);
        return GM_RSSP1_ERROR;
    }
#endif

    /* 设定发送缓冲区大小 = 1k左右,约等于MTU */
    len = sizeof(len);
    opt_send_buf_len = socket_send_buf_len;
    opt_recv_buf_len = socket_recv_buf_len;
    if (GM_RSSP1_ERROR == setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (const char *)&opt_send_buf_len, sizeof(len)))
    {
//        GM_RSSP1_Log_Msg(0,"Fatal GM_RSSP1_ERROR! set server socket SO_SNDBUF option fail!errno:%d.\n", FSFB_GET_LAST_ERRNO, 0, 0, 0, 0 , 0);

        return GM_RSSP1_ERROR;
    }

    if (GM_RSSP1_ERROR == setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (const char *)&opt_recv_buf_len, sizeof(len)))
    {
        GM_RSSP1_Log_Msg(0,"Fatal GM_RSSP1_ERROR! set server socket SO_RCVBUF option fail!errno:%d.\n", FSFB_GET_LAST_ERRNO , 0, 0, 0, 0, 0);

        return GM_RSSP1_ERROR;
    }

    /* set send/recv timeout */
    opt_send_timeout.tv_sec     = 0;
    opt_send_timeout.tv_usec    = 0;
    if (GM_RSSP1_ERROR == setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&opt_send_timeout, sizeof(opt_send_timeout)))
    {
//        GM_RSSP1_Log_Msg(0,"Fatal GM_RSSP1_ERROR! set server socket SO_SNDTIMEO option fail! errno:%d\n", FSFB_GET_LAST_ERRNO, 0, 0, 0, 0, 0);
        return GM_RSSP1_ERROR;
    }

    opt_recv_timeout.tv_sec     = 0;
    opt_recv_timeout.tv_usec    = 0;
    if (GM_RSSP1_ERROR == setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&opt_recv_timeout, sizeof(opt_recv_timeout)))
    {
        GM_RSSP1_Log_Msg(0,"Fatal GM_RSSP1_ERROR! set server socket SO_RCVTIMEO option fail! errno:%d\n", FSFB_GET_LAST_ERRNO, 0, 0, 0, 0, 0);
        return GM_RSSP1_ERROR;
    }

    return GM_RSSP1_OK;
}


GM_RSSP1_INT32 FSFB_Socket_Init(GM_SOCKET *pSock , GM_RSSP1_INT32 type , struct sockaddr_in *pAddr , GM_RSSP1_INT32 socket_send_buf_len , GM_RSSP1_INT32 socket_recv_buf_len , GM_SOCKET *pMax_socket_value)
{
    struct sockaddr_in addr;
    
    if ((NULL == pSock) || (NULL == pAddr) || (NULL == pMax_socket_value))
    {
        return GM_RSSP1_ERROR;
    }

    if (0 <= *pSock)
    {
        FSFB_Close_Socket(pSock);
    }

    *pSock = FSFB_Create_Socket(type , pMax_socket_value);

    if (0 > *pSock)
    {
        return GM_RSSP1_ERROR;
    }

    if (GM_RSSP1_ERROR == FSFB_Set_Socket_Option(*pSock , socket_send_buf_len , socket_recv_buf_len))
    {
        FSFB_Close_Socket(pSock);
        return GM_RSSP1_ERROR;
    }

    /* bind socket */
    memset(&addr , 0 , sizeof(addr));
    addr.sin_family = pAddr->sin_family;
    addr.sin_addr.s_addr = l_ip[g_con][g_chn];
    addr.sin_port = pAddr->sin_port;
    if (0 > bind(*pSock , (struct sockaddr *)&addr , sizeof(addr)))
    {
#ifndef WIN32
		inet_ntoa_b(addr.sin_addr,str);
#endif
	/*
        GM_RSSP1_Log_Msg(0,"bind fail.erno:%d.ip:%s.prt:%d.\n",FSFB_GET_LAST_ERRNO,inet_ntoa_b(pAddr->sin_addr，str),ntohs(pAddr->sin_port),0,0,0);
        */
        
        if(FSFB_SOKC_ERR_EADDRINUSE != FSFB_GET_LAST_ERRNO)
        {
            FSFB_Close_Socket(pSock);
            return GM_RSSP1_ERROR;
        }
    }
    else
    {
#ifndef WIN32
		inet_ntoa_b(addr.sin_addr,str);
#endif
//        GM_RSSP1_Log_Msg(0,"FSFB sock %d bind OK. prt:%d.\n",*pSock,ntohs(pAddr->sin_port),0,0,0,0);
    }

    return GM_RSSP1_OK;
}

GM_RSSP1_BOOL FSFB_Is_Socket_Readable(GM_SOCKET sock, GM_SOCKET max_socket_value, GM_RSSP1_INT32 time_Out)
{
    fd_set read_Set;/* 读集合 */

    struct timeval select_Time_Out;
    GM_RSSP1_INT32 ret = -1;

    if (0 > sock)
    {
        return GM_RSSP1_FALSE;
    }

    FD_ZERO(&read_Set);

    FD_SET(sock, &read_Set);

    if (time_Out >= 1000)
    {
        select_Time_Out.tv_sec  = time_Out / 1000;
        select_Time_Out.tv_usec = (time_Out % 1000) * 1000;
    }
    else
    {
        select_Time_Out.tv_sec  = 0;
        select_Time_Out.tv_usec = time_Out * 1000;
    }

    ret = select(sock + 1, &read_Set, NULL, NULL, &select_Time_Out);

    if (0 >= ret)
    {
        return GM_RSSP1_FALSE;
    }
    else
    {
        if (0 != FD_ISSET(sock, &read_Set))
        {
            return GM_RSSP1_TRUE;
        }
        else
        {
            return GM_RSSP1_FALSE;
        }
    }
}

GM_RSSP1_BOOL FSFB_Is_Socket_Writable(GM_SOCKET sock, GM_SOCKET max_socket_value, GM_RSSP1_INT32 time_Out)
{
    fd_set write_Set;/* 读集合, 写集合 */

    struct timeval select_Time_Out;
    GM_RSSP1_INT32 ret = -1;

    if (0 > sock)
    {
        return GM_RSSP1_FALSE;
    }

    FD_ZERO(&write_Set);

    FD_SET(sock, &write_Set);

    if (time_Out >= 1000)
    {
        select_Time_Out.tv_sec  = time_Out / 1000;
        select_Time_Out.tv_usec = (time_Out % 1000) * 1000;
    }
    else
    {
        select_Time_Out.tv_sec  = 0;
        select_Time_Out.tv_usec = time_Out * 1000;
    }

    ret = select(sock + 1, NULL, &write_Set, NULL, &select_Time_Out);

    if (0 >= ret)
    {
        return GM_RSSP1_FALSE;
    }
    else
    {
        if (0 != FD_ISSET(sock, &write_Set))
        {
            return GM_RSSP1_TRUE;
        }
        else
        {
            return GM_RSSP1_FALSE;
        }
    }
}

void FSFB_CFM_Con_UDP_Recv_Dat(FSFB_LNK_connection_struct *pLNK_con , FSFB_LNK_object_struct * pLNK, GM_RSSP1_UINT8 index)
{
    struct sockaddr_in remote_add;
    GM_RSSP1_INT32 remote_addr_len                 = 0;
    GM_RSSP1_INT32 udp_chn_index                   = 0;
    GM_RSSP1_INT32 i                               = 0;
    GM_RSSP1_INT32 j                               = 0;
    GM_RSSP1_INT32 recv_len                        = 0;
	GM_RSSP1_BOOL recv_chk                  = GM_RSSP1_FALSE;
    char buf[GM_RSSP1_MAX_SAFETY_DAT_LEN * 2]  = {0};  /* 预留空间，防止越界 */
    GM_RSSP1_INT32 rt                              = GM_RSSP1_ERROR;
    GM_RSSP1_com_pri_struct pri;
    GM_RSSP1_Write_Q_Return_Enum write_q_rt = Q_WRITE_FAIL;
    GM_RSSP1_UINT32 rcvd_addr = 0;
	GM_RSSP1_UINT32 rcvd_port = 0;
	GM_RSSP1_UINT32 cfg_addr = 0;
	GM_RSSP1_UINT32 cfg_port = 0;

    if ((NULL == pLNK_con) || (NULL == pLNK))
    {
        return;
    }

    if (GM_RSSP1_Lib_State_Operational != pLNK->lib_state)
    {
        return;
    }

    remote_addr_len = sizeof(struct sockaddr);

    for (udp_chn_index = 0 ; udp_chn_index < pLNK_con->chn_num ; ++udp_chn_index)
    {
        if (GM_RSSP1_TRUE == FSFB_Is_Socket_Readable(pLNK_con->com_chn[udp_chn_index].sock , pLNK->max_socket_value , GM_RSSP1_SOCKET_RECV_SELECT_DELAY))
        {
            memset(&remote_add , 0 , sizeof(remote_add));
            recv_len = recvfrom(pLNK_con->com_chn[udp_chn_index].sock , buf , sizeof(buf) , 0 , (struct sockaddr *)(&remote_add) , &remote_addr_len);

            if (0 > recv_len)
            {
                /* recv error close socket */
                FSFB_Close_Socket(&(pLNK_con->com_chn[udp_chn_index].sock));

                /* switch UDP chn in use */
                pLNK_con->com_chn[udp_chn_index].b_connected = GM_RSSP1_FALSE;

            }
            else if (0 == recv_len)
            {
                /* 不作任何处理 The  return  value  will be 0 when the peer has performed an orderly shutdown*/
                /* switch UDP chn in use */
                pLNK_con->com_chn[udp_chn_index].b_connected = GM_RSSP1_FALSE;
            }
            else if (0 < recv_len)
            {
                if (recv_len > GM_RSSP1_MAX_SAFETY_DAT_LEN)
                {
                }
                else
                {

					GM_RSSP1_Log_Msg(0,"--------idx %d ,UDP recv %d bytes.\n",pLNK_con->index,recv_len,0,0,0,0);
                    recv_chk = GM_RSSP1_FALSE;
					for (i=0; i<pLNK->connection_nums;++i)
					{
						for (j=0;j<pLNK->connection[i].chn_num;++j)
						{
					rcvd_addr = remote_add.sin_addr.s_addr;
					rcvd_port = remote_add.sin_port;
					cfg_addr = pLNK->connection[i].com_chn[j].addr_cfg.remote_addr.sin_addr.s_addr;
					cfg_port = pLNK->connection[i].com_chn[j].addr_cfg.remote_addr.sin_port;
					if ((rcvd_addr == cfg_addr)&&(rcvd_port == cfg_port))
							{
								GM_RSSP1_Log_Msg(4,"--------idx %d ,UDP recv %d bytes.\n",i,recv_len,0,0,0,0);
								recv_chk = GM_RSSP1_TRUE;
								break;
							}
						}
						if (GM_RSSP1_TRUE == recv_chk)
						{
							break;
						}
					}
                   
                    if(GM_RSSP1_TRUE == recv_chk)
                    {
                        
//                        GM_RSSP1_Log_Msg(0,"--------idx %d ,UDP recv %d bytes.\n",i,recv_len,0,0,0,0);
                        
                        /*根据收到数据包的IP和端口，搜索真实的通道，解决在同一端口收到不同IP的情况 .和利时调试*/
                        /* retset check idle flag */
                        pLNK->connection[i].com_chn[j].b_connected = GM_RSSP1_TRUE;
                        pLNK->connection[i].com_chn[j].idle_counter = 0;
                        
                        /* save dat to msg queue */
                        pri.bytes_count = recv_len;
                        GM_RSSP1_memcpy(pri.byte , buf , recv_len);
                        
                        write_q_rt = FSFB_Msg_Queue_Write(&(g_Link_MQ[i].RCV_SND_MQ[j].RCV_MQ/*pLNK->connection[i].com_chn[j].recv_udp_dat_Q*/) , &pri);
                        if (Q_WRITE_OK_FULL == write_q_rt)
                        {
						}
                        else if (Q_WRITE_FAIL == write_q_rt)
                        {
                            pLNK->lib_state = GM_RSSP1_Lib_State_Failure;
                        }
                    }
                }
            }
        }

        /* 连接维护。判断UDP是否长时间未收到数据 */
        ++pLNK_con->com_chn[udp_chn_index].idle_counter;
        if (pLNK_con->com_chn[udp_chn_index].idle_counter > pLNK_con->UDP_sock_cfg.check_idle_cycle)
        {
            pLNK_con->com_chn[udp_chn_index].b_connected = GM_RSSP1_FALSE;
            pLNK_con->com_chn[udp_chn_index].idle_counter  = 0;

            /* 清空已超时通道的接收队列。 */
            FSFB_Msg_Queue_Clear(&(pLNK_con->com_chn[udp_chn_index].recv_udp_dat_Q));
        }
    }

    return;
}

void FSFB_CFM_Con_UDP_Send_Dat(FSFB_LNK_connection_struct *pLNK_con , FSFB_LNK_object_struct * pLNK, GM_RSSP1_UINT8 index)
{
    GM_RSSP1_INT32 udp_chn_index                   = 0;
    GM_RSSP1_INT32 rt                              = GM_RSSP1_ERROR;
    GM_RSSP1_INT32 send_rt                         = -1;
	char aa[1024]={0};

    if ((NULL == pLNK_con) || (NULL == pLNK))
    {
        return;
    }

    if (GM_RSSP1_Lib_State_Operational != pLNK->lib_state)
    {
        return;
    }

    for (udp_chn_index = 0 ; udp_chn_index < pLNK_con->chn_num ; ++udp_chn_index)
    {
        if (GM_RSSP1_TRUE == FSFB_Is_Msg_Queue_Empty(&(g_Link_MQ[index].RCV_SND_MQ[udp_chn_index].SND_MQ)/*&(pLNK_con->com_chn[udp_chn_index].send_udp_dat_Q)*/))
        {
            continue;
        }


        if (GM_RSSP1_FALSE == FSFB_Is_Socket_Writable(pLNK_con->com_chn[udp_chn_index].sock , pLNK->max_socket_value , GM_RSSP1_SOCKET_SEND_SELECT_DELAY))
        {
            continue;
        }

        if (GM_RSSP1_FALSE == pLNK_con->com_chn[udp_chn_index].b_temp_send_pkt_valid)
        {
            /* socket 可写，发送暂存数据包或从队列中取数据包暂存后发送 */
            if (GM_RSSP1_FALSE == FSFB_Msg_Queue_Get(&(g_Link_MQ[index].RCV_SND_MQ[udp_chn_index].SND_MQ)/*&(pLNK_con->com_chn[udp_chn_index].send_udp_dat_Q)*/, &(pLNK_con->com_chn[udp_chn_index].temp_send_pkt)))
            {
                continue;
            }

            if (pLNK_con->com_chn[udp_chn_index].temp_send_pkt.bytes_count > GM_RSSP1_MAX_SAFETY_DAT_LEN)
            {
                continue;
            }

            pLNK_con->com_chn[udp_chn_index].b_temp_send_pkt_valid = GM_RSSP1_TRUE;
        }

		if(g_bugInsert.m_RMSMsgsize ==0 )
		{
			GM_RSSP1_memcpy(Copy_byte , pLNK_con->com_chn[udp_chn_index].temp_send_pkt.byte , pLNK_con->com_chn[udp_chn_index].temp_send_pkt.bytes_count);
		}

        /* 发送数据包延时，防止粘包 */
        GM_SLEEP(pLNK_con->UDP_sock_cfg.packet_send_delay);
		if(g_bugInsert.m_RMSMsgsize !=0 )
		{
        send_rt = sendto(
                      pLNK_con->com_chn[udp_chn_index].sock ,
                      Copy_byte ,
                      pLNK_con->com_chn[udp_chn_index].temp_send_pkt.bytes_count ,
                      0 ,
                      (struct sockaddr *) & (pLNK_con->com_chn[udp_chn_index].addr_cfg.remote_addr) ,
                      sizeof(pLNK_con->com_chn[udp_chn_index].addr_cfg.remote_addr)
                  );
		}
		else
		{
        send_rt = sendto(
                      pLNK_con->com_chn[udp_chn_index].sock ,
                      pLNK_con->com_chn[udp_chn_index].temp_send_pkt.byte ,
                      pLNK_con->com_chn[udp_chn_index].temp_send_pkt.bytes_count ,
                      0 ,
                      (struct sockaddr *) & (pLNK_con->com_chn[udp_chn_index].addr_cfg.remote_addr) ,
                      sizeof(pLNK_con->com_chn[udp_chn_index].addr_cfg.remote_addr)
                  );
		}
        if (send_rt != pLNK_con->com_chn[udp_chn_index].temp_send_pkt.bytes_count)
        {
            /* clear send msg queue */
            FSFB_Msg_Queue_Clear(&(g_Link_MQ[index].RCV_SND_MQ[udp_chn_index].SND_MQ)/*&(pLNK_con->com_chn[udp_chn_index].send_udp_dat_Q)*/);

            /* switch UDP chn in use */
            pLNK_con->com_chn[udp_chn_index].b_connected = GM_RSSP1_FALSE;

            FSFB_Close_Socket(&(pLNK_con->com_chn[udp_chn_index].sock));
        }
        else
        {
            /* 成功发送，清除暂存包标志 */
            pLNK_con->com_chn[udp_chn_index].b_temp_send_pkt_valid = GM_RSSP1_FALSE;
            
           /* GM_RSSP1_Log_Msg(0,"FSFB index %d UDP send %d bytes from %s (%d) to %s (%d).\n",pCFM_con->index,send_rt,pCFM_con->com_chn[udp_chn_index].addr_cfg.local_ip,pCFM_con->com_chn[udp_chn_index].addr_cfg.local_port,pCFM_con->com_chn[udp_chn_index].addr_cfg.remote_ip,pCFM_con->com_chn[udp_chn_index].addr_cfg.remote_port);*/
            
        }

    }

    return;
}

void FSFB_CFM_Con_UDP_Maintain(FSFB_LNK_connection_struct *pLNK_con , FSFB_LNK_object_struct * pLNK, GM_RSSP1_UINT8 index)
{
    GM_RSSP1_INT32 udp_chn_index                   = 0;
    GM_RSSP1_INT32 rt                              = GM_RSSP1_ERROR;

    if ((NULL == pLNK_con) || (NULL == pLNK))
    {
        return;
    }

    if (GM_RSSP1_Lib_State_Operational != pLNK->lib_state)
    {
        return;
    }

	g_con = pLNK_con->index;

    for (udp_chn_index = 0 ; udp_chn_index < pLNK_con->chn_num ; ++udp_chn_index)
    {
		g_chn = udp_chn_index;
        if (0 > pLNK_con->com_chn[udp_chn_index].sock)
        {

            rt = FSFB_Socket_Init(

                     &(pLNK_con->com_chn[udp_chn_index].sock) ,
                     SOCK_DGRAM ,
                     &(pLNK_con->com_chn[udp_chn_index].addr_cfg.local_addr) ,
                     pLNK_con->UDP_sock_cfg.sock_send_buf_size ,
                     pLNK_con->UDP_sock_cfg.sock_recv_buf_size ,
                     &(pLNK->max_socket_value)
                 );

            if (GM_RSSP1_ERROR == rt)
            {
                /*
                                GM_RSSP1_Log_Msg(0,"FSFB CFM ID:0x%X UDP socket %d init error! errno: %d.\n",pCFM_con->remote_CASCO_ID ,udp_chn_index ,FSFB_GET_LAST_ERRNO,0,0,0);
                */
            }

        }
    }

    return;
}

#endif