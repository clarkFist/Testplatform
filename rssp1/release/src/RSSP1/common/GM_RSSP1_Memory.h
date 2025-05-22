/**
* @file GM_RSSP1_Memory.h
* @brief
* @author Hao Liming
* @date 2010-9-30 13:33:06
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: </b>haoliming@casco.com.cn</pre>
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/
#ifndef _GM_RSSP1_MEMORY_H
#define _GM_RSSP1_MEMORY_H

#include "../GM_RSSP1_Utils_Base.h"

#ifdef WIN32
#include <malloc.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif /**< __cplusplus */

     /**
    * @brief
    *
    * 封装了malloc.
    * @param[in] size
    * @return void* GM_RSSP1_malloc
    */
    void* GM_RSSP1_malloc(size_t size);

    /**
    * @brief
    *
    * 封装了memcpy.
    * @param[in] destination
    * @param[in] source
    * @param[in] dim
    * @return void GM_RSSP1_memcpy
    */
    void GM_RSSP1_memcpy(void* destination, const void* source, size_t dim);

    /**
    * @brief
    *
    * 封装了memset.
    * @param[in] destination
    * @param[in] value
    * @param[in] dim
    * @return void GM_RSSP1_memset
    */
    void GM_RSSP1_memset(void* destination, GM_RSSP1_UINT8 value, size_t dim);

    /**
    * @brief GM_RSSP1_free
    *
    * 释放指针所指向的内存,是对free的封装.
    * @param[in] p 指针
    */
    void GM_RSSP1_free(void* p);


	void* GM_RSSP1_Simple_Malloc(size_t size);


	GM_RSSP1_BOOL GM_RSSP1_Memory_Init(void *pStartAddr, GM_RSSP1_UINT64 size);
#ifdef __cplusplus
}

#endif /**< __cplusplus */

#endif /**< _GM_MALLOCY_H */

