/**
* @file GM_Malloc.c
* @brief
* @author Hao Liming
* @date 2010-9-30 13:33:19
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: </b>haoliming@casco.com.cn</pre>
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/
#include "GM_Malloc.h"

void* GM_malloc(size_t size)
{
    void* rt = NULL;

	/*增加对size=0时候，返回null */
	if (size != (size_t)0)
	{
		rt = malloc(size);
	}

    return rt;
}

