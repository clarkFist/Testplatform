/**
* @file GM_Memset.c
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
#include "GM_Memset.h"

void GM_memset(void* destination, GM_UINT8 value, size_t size)
{
    if ((NULL == destination) || (size == (size_t)0))
    {
        return;
    }

    memset(destination, (int)value, size);

    return;
}
