/**
* @file GM_RSSP1_Memory.c
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
#include "GM_RSSP1_Memory.h"
GM_RSSP1_UINT8 *g_pMemBase_P1 = NULL;/*ָ���ڴ��׵�ַ*/
GM_RSSP1_UINT64 g_MemSize_P1 = 0U;/*�ڴ��ܴ�С*/
GM_RSSP1_UINT64 g_MemUsedSize_P1 = 0U;/*��ʹ�ô�С*/

void* GM_RSSP1_malloc(size_t size)
{
    void* rt = NULL;

	/*���Ӷ�size=0ʱ�򣬷���null */
	if (size != (size_t)0)
	{
	#ifndef GM_RSSP1_MEMORY_WITH_STATIC
		rt = malloc(size);
	#else
		rt = GM_RSSP1_Simple_Malloc(size);
	#endif
	}

    return rt;
}

void GM_RSSP1_memcpy(void* destination, const void* source, size_t size)
{
    if ((NULL == destination) || (NULL == source) || (size == (size_t)0))
    {
        return;
    }

    memcpy(destination, source, size);

    return;
}

void GM_RSSP1_memset(void* destination, GM_RSSP1_UINT8 value, size_t size)
{
    if ((NULL == destination) || (size == (size_t)0))
    {
        return;
    }

    memset(destination, (int)value, size);

    return;
}

void GM_RSSP1_free(void* p)
{
#ifndef GM_RSSP1_MEMORY_WITH_STATIC
    if (NULL != p)
    {
        free(p);
    }
#endif
    return;
}


GM_RSSP1_BOOL GM_RSSP1_Memory_Init(void *pStartAddr, GM_RSSP1_UINT64 size)
{
	if ((pStartAddr == NULL)||(size == 0U))
	{
		return GM_RSSP1_FALSE;
	}

	if ((g_pMemBase_P1 != NULL)&&(g_MemSize_P1 != 0U))
	{
	 /*has already initialized
	 �������ظ����ã�����false��ʾ�û��������ô���
	 ���ʧЧ�ٴε��øú��� ϵͳʧЧ ���д��� ���øú������� ����false��ϵͳ�ݴ˽��뵼��ȫ�Ĳ���
	 */
		return GM_RSSP1_FALSE;
	}

	g_pMemBase_P1 = (GM_RSSP1_UINT8*)pStartAddr;
	g_MemSize_P1 = size;

	return GM_RSSP1_TRUE;
}

void* GM_RSSP1_Simple_Malloc(size_t size)
{
	void* p = NULL;
	GM_RSSP1_UINT64 tempSize = (GM_RSSP1_UINT64)size;
	GM_RSSP1_UINT32 tmpVal = 0U;

	if ((tempSize == 0U)||(g_pMemBase_P1 == NULL))
	{
		return NULL;
	}

	tmpVal = (GM_RSSP1_UINT32)(g_pMemBase_P1 + g_MemUsedSize_P1) % 4U ;

	if (tmpVal != 0)
	{
		g_MemUsedSize_P1 += 4 - tmpVal;
	}
	
	if (tempSize > (g_MemSize_P1 - g_MemUsedSize_P1))/*����ʱ���պ÷�����;�ü�������ֹ���*/
	{
		return NULL;
	}
	
	/*g_pMemBase�趨��ΪGM_UINT8*,����ͨ����+���֡�����ǰ��*/
	p = (void*)(g_pMemBase_P1 + g_MemUsedSize_P1);
	g_MemUsedSize_P1 += tempSize;
    printf("111GM_RSSP1_Simple_Malloc g_MemUsedSize_P1:%lld\n", g_MemUsedSize_P1);
    printf("222GM_RSSP1_Simple_Malloc g_MemUsedSize_P1:%zd\n", size);
	return p;
}