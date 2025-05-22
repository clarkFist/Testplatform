/**
* @file GM_Memcpy.h
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
#ifndef _GM_MEMCPY_H
#define _GM_MEMCPY_H

#include "GM_Utils_Base.h"

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
    * ·â×°ÁËmemcpy.
    * @param[in] destination
    * @param[in] source
    * @param[in] dim
    * @return void GM_memcpy
    */
    void GM_memcpy(void* destination, const void* source, size_t dim);

#ifdef __cplusplus
}

#endif /**< __cplusplus */

#endif /**< _GM_MEMCPY_H */

