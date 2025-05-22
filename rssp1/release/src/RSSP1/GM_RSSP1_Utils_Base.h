/**
* @file GM_RSSP1_Utils_Base.h
* @brief
* @author Hao Liming
* @date 2010-7-16 19:08:31
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: </b>haoliming@casco.com.cn</pre>
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/

#ifndef _GM_RSSP1_UTILS_BASE_H
#define _GM_RSSP1_UTILS_BASE_H

#include "GM_RSSP1_Define.h"
#include "GM_RSSP1_Error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef WIN32
#define OS_WINDOWS WIN32
#include <winsock2.h>
#endif

#ifdef VXWORKS
#include <vxworks.h>
#ifdef VXWORKS6_6
#include <ioLib.h>
#include <sioLibCommon.h>
#endif

#ifdef VXWORKS5_5
#endif
#endif
#ifdef QNX
#include <errno.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif /**< __cplusplus */
    /**
    * @BasicType
    * @brief 基本类型定义
    *
    * Detailed description.
    * @{
    */

#ifndef NULL
#define NULL 0
#endif


    typedef unsigned char      	GM_RSSP1_UINT8;
    typedef unsigned short     	GM_RSSP1_UINT16;
    typedef unsigned long      	GM_RSSP1_UINT32;
    typedef signed char        	GM_RSSP1_INT8;
    typedef signed short int   	GM_RSSP1_INT16;
    typedef signed long int    	GM_RSSP1_INT32;
    typedef int                	GM_RSSP1_BOOL;

#define GM_RSSP1_TRUE   (GM_RSSP1_BOOL)1
#define GM_RSSP1_FALSE  (GM_RSSP1_BOOL)0
    
#ifdef WIN32
    /*Start modify by xw for CR1975 2014-09-02*/
    typedef unsigned __int64  GM_RSSP1_UINT64; 
    typedef __int64           GM_RSSP1_INT64;
    /*End modify by xw for CR1975 2014-09-02*/
#else/*其它操作系统*/
    typedef unsigned long long GM_RSSP1_UINT64;
    typedef long long          GM_RSSP1_INT64;
#endif

#define GM_RSSP1_UINT32_MAX (4294967295U) /**< GM_UINT32最大值 */
#define GM_RSSP1_UINT16_MAX (65535U)        /**< GM_UINT16最大值 */
#define GM_RSSP1_UINT8_MAX  (255)          /**< GM_UINT8最大值 */
#define GM_RSSP1_LONG_MIN   (-2147483647L - 1)
#define GM_RSSP1_LONG_MAX   (2147483647L)
    /** @} */

    /** @brief 设置字节对齐 */
#ifdef WIN32
	#pragma pack(1) /**< 以单字节对齐 */
	#define GM_RSSP1_PACKED
#else/*2003和其它操作系统*/
	#ifdef Platform_2oo3
	/*2003中没有操作系统*/
		#ifdef __ghs__
			#pragma pack(push)
			#pragma pack(1)
			#define GM_RSSP1_PACKED
		#elif defined(_DIAB_TOOL)
			#pragma pack(1)
			#define GM_RSSP1_PACKED
		#else
			#define GM_RSSP1_PACKED  __attribute__((__packed__))
		#endif
	#else/*除win32以外，其它为此种方式*/
		#define GM_RSSP1_PACKED  __attribute__((__packed__))
	#endif
#endif

#ifdef __cplusplus
}

#endif /**< __cplusplus */

#endif /**< _GM_UTILS_BASE_H */

