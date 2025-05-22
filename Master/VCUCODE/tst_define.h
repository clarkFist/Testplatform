#ifndef __tst_define__
#define __tst_define__
#include "step_includes.h"
#include "step_guest.h"
#include "osw.h"
#include "mstep.h"
#include "gdf.h"
#include "std.h"
#include "dtxt_print_on.h"
#include "dtxt.h"
#include "datatypes.h"


#define VCU_VERSION 112
 
#if VCU_VERSION == 102
	//#include "GM_PROSTACK_Udp.h"
	//#define Lib_F_UDPFlush(a) MSTEP_F_UDPFlush(a) 
	//#define Lib_F_UDPOpen(a,b,c) (MSTEP_T_SocketID*)GM_PROSTACK_F_UdpOpen(a, b, 0u, 1)
	//#define Lib_F_UDPSend(pSocket, destAddr, destPort, pData, dataLength) GM_PROSTACK_F_UdpSend((const GM_PROS_UDP_Socket*)pSocket, destAddr, destPort, pData, dataLength)
	#define Lib_F_UDPFlush(a) MSTEP_F_UDPFlush(a) 
	#define Lib_F_UDPOpen(a,b,c) MSTEP_F_UDPOpen(a,b)	
	#define Lib_F_UDPSend(pSocket, destAddr, destPort, pData, dataLength) MSTEP_F_UDPSend((const MSTEP_T_SocketID*)pSocket, destAddr, destPort, pData, dataLength)	

#else
	#define Lib_F_UDPFlush(a) MSTEP_F_UDPFlush(a) 
	#define Lib_F_UDPOpen(a,b,c) MSTEP_F_UDPOpen(a,b,c)
	#define Lib_F_UDPSend(pSocket, destAddr, destPort, pData, dataLength) MSTEP_F_UDPSend((const MSTEP_T_SocketID*)pSocket, destAddr, destPort, pData, dataLength)
#endif

#define Tst_F_K2oo2CpuExchange(pBufIn, pBufOut, size, timeout) (MSTEP_F_K2oo2CpuExchangeExt((INT8U* const)(pBufIn), (const INT8U*)(pBufOut), (const INT32U)(size), (const INT16U)(timeout), (INT8U)(MSTEP_C_FUNCTION_ID_K2OO2_CPU_EXCHANGE), (INT16U)(__LINE__)))
/** \see MSTEP_F_K2oo2CpuSendExt */
#define Tst_F_K2oo2CpuSend(pBuf, size, timeout) (MSTEP_F_K2oo2CpuSendExt((const INT8U* const)(pBuf), (const INT32U)(size), (const INT16U)(timeout), (INT8U)(MSTEP_C_FUNCTION_ID_K2OO2_CPU_SEND), (INT16U)(__LINE__)))
/** \see MSTEP_F_K2oo2CpuWaitExt */
#define Tst_F_K2oo2CpuWait(pBuf, size, timeout) (MSTEP_F_K2oo2CpuWaitExt((INT8U* const)(pBuf), (const INT32U)(size), (const INT16U)(timeout), (INT8U)(MSTEP_C_FUNCTION_ID_K2OO2_CPU_WAIT), (INT16U)(__LINE__)))

#endif
