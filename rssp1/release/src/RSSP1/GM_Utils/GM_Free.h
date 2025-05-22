/**
* @file GM_Free.h
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
#ifndef _GM_FREE_H
#define _GM_FREE_H

#include "GM_Utils_Base.h"

#ifdef WIN32
#include <malloc.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif /**< __cplusplus */

    /**
    * @brief GM_free
    *
    * 释放指针所指向的内存,是对free的封装.
    * @param[in] p 指针
    */
    void GM_free(void* p);

 #ifdef __cplusplus
}

#endif /**< __cplusplus */

#endif /**< _GM_FREE_H */

