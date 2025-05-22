/**
* @file GM_Free.c
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
#include "GM_Free.h"

void GM_free(void* p)
{
    if (NULL != p)
    {
        free(p);
    }
    return;
}
