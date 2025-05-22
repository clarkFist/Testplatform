/**
 * Copyright(C) 2012,CASCO Company ��������Ȩ����( All rights reserved. )
 * 
 * @file:GM_RSSP1_Utils_Other.c
 * @brief:
 * @auther:Hao Liming
 * @create date:2010��10��30��
 * @modify by:wtt
 * @Modify info:2012��12��6��
 */

#include "GM_RSSP1_Utils_Other.h"
#include "io.h"
GM_RSSP1_LOG_CALL_BACK_FUN GM_RSSP1_Log_Usr = GM_RSSP1_Log_Err;

GM_RSSP1_BOOL GM_RSSP1_atoui(const char* pBuf, GM_RSSP1_UINT32 bufSize, unsigned int* pVal)
{
	GM_RSSP1_UINT32 i = 0U;
	int cmpRt = 0;

	if ((NULL == pVal) || (NULL == pBuf) || ((GM_RSSP1_UINT32)0U == (GM_RSSP1_UINT32)bufSize))
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_ATOUI_PAR_ERR, (int)bufSize, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	*pVal = 0U;
	for (i = (GM_RSSP1_UINT32)0u; i < bufSize; ++i)
	{
		if ('\0' == *(pBuf + i))
		{
			bufSize = i;
		}
	}

	if ((GM_RSSP1_UINT32)0U == (GM_RSSP1_UINT32)bufSize)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_ATOUI_BUFSIZE_ZERO_ERR, (int)bufSize, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}

	if ((GM_RSSP1_UINT32)bufSize > (GM_RSSP1_UINT32)10U)
	{/*ֻ�з���λ�� λ������ 4294967295*/
		GM_RSSP1_Log_Usr(GM_RSSP1_ATOUI_BUFSIZE_BITTOMAX_ERR, (int)bufSize, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
	if ((GM_RSSP1_UINT32)bufSize == (GM_RSSP1_UINT32)10U)
	{
		cmpRt = strncmp(pBuf, "4294967295", 10);
	}

	for (i = (GM_RSSP1_UINT32)0u; i < bufSize; ++i)
	{
		if ((*(pBuf + i) < '0') || (*(pBuf + i) > '9'))
		{
			GM_RSSP1_Log_Usr(GM_RSSP1_ATOUI_BUFSIZE_CHAR_ERR, (int)(*(pBuf + i)), (int)i, 0, 0, 0, 0);
			return GM_RSSP1_FALSE;
		}
	}

	if (cmpRt > 0)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_ATOUI_BUFSIZE_RTTOMAX_ERR, (int)(*(pBuf + i)), (int)i, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
#ifdef Platform_2oo3
	GM_RSSP1_Log_Usr(GM_RSSP1_ATOUI_BUFSIZE_2OO3_ERR, 0, 0, 0, 0, 0, 0);
	return GM_RSSP1_FALSE;
#else
	*pVal = (unsigned int)strtoul(pBuf, NULL, 10);
	return GM_RSSP1_TRUE;
#endif

}
/*�����ַ������жϣ����ų�*/
GM_RSSP1_BOOL GM_RSSP1_atoi(const char* pBuf, GM_RSSP1_UINT32 bufSize, int* pVal)
{
    GM_RSSP1_UINT32 i = 0U;
    GM_RSSP1_BOOL Bnegtive = GM_RSSP1_FALSE;
    int cmpRt = 0;

    if ((NULL == pVal) || (NULL == pBuf) || ((GM_RSSP1_UINT32)0U == (GM_RSSP1_UINT32)bufSize))
    {
        GM_RSSP1_Log_Usr(GM_RSSP1_ATOI_PAR_ERR, (int)bufSize, 0, 0, 0, 0, 0);
        return GM_RSSP1_FALSE;
    }

    *pVal = 0;
    for (i = (GM_RSSP1_UINT32)0u; i < bufSize; ++i)
    {
        if ('\0' == *(pBuf + i))
        {
            bufSize = i;
        }
    }

    if ((GM_RSSP1_UINT32)0U == (GM_RSSP1_UINT32)bufSize)
    {
        GM_RSSP1_Log_Usr(GM_RSSP1_ATOI_BUFSIZE_ZERO_ERR, (int)bufSize, 0, 0, 0, 0, 0);
        return GM_RSSP1_FALSE;
    }
    /** @brief �ж����������Ǹ��� */
    if ('-' == *pBuf)
    {
        Bnegtive = GM_RSSP1_TRUE;
    }

    if (GM_RSSP1_TRUE == Bnegtive)/*�Ǹ���*/
    {
        if (((GM_RSSP1_UINT32)1U == (GM_RSSP1_UINT32)bufSize)||((GM_RSSP1_UINT32)bufSize > (GM_RSSP1_UINT32)11U))
        {/*ֻ�з���λ�� λ������ -2147483648*/
            GM_RSSP1_Log_Usr(GM_RSSP1_ATOI_BUFSIZE_ERR, (int)bufSize, 0, 0, 0, 0, 0);
            return GM_RSSP1_FALSE;
        }
        if ((GM_RSSP1_UINT32)bufSize == (GM_RSSP1_UINT32)11U)
        {
            cmpRt = strncmp(pBuf, "-2147483648", 11);
        }


        for (i = (GM_RSSP1_UINT32)1u; i < bufSize; ++i)
        {
            if ((*(pBuf + i) < '0') || (*(pBuf + i) > '9'))
            {
                GM_RSSP1_Log_Usr(GM_RSSP1_ATOI_BUFSIZE_CHAR_ERR, (int)(*(pBuf + i)), (int)i, (int)Bnegtive, 0, 0, 0);
                return GM_RSSP1_FALSE;
            }
        }
    }
    else
    {
        if ((GM_RSSP1_UINT32)bufSize > (GM_RSSP1_UINT32)10U)
        {/*λ������ 2147483647*/
            GM_RSSP1_Log_Usr(GM_RSSP1_ATOI_BUFSIZE_ERR, (int)bufSize, 0, 0, 0, 0, 0);
            return GM_RSSP1_FALSE;
        }
        if ((GM_RSSP1_UINT32)bufSize == (GM_RSSP1_UINT32)10U)
        {/*��2147483647���Ƚϣ�wtt�޸�2011-11-30*/
            cmpRt = strncmp(pBuf, "2147483647", 10);
        }

        for (i = (GM_RSSP1_UINT32)0u; i < bufSize; ++i)
        {
            if ((*(pBuf + i) < '0') || (*(pBuf + i) > '9'))
            {
                GM_RSSP1_Log_Usr(GM_RSSP1_ATOI_BUFSIZE_CHAR_ERR, (int)(*(pBuf + i)), (int)i, (int)Bnegtive, 0, 0, 0);
                return GM_RSSP1_FALSE;
            }
        }
    }

	if (cmpRt > 0)
	{
		GM_RSSP1_Log_Usr(GM_RSSP1_ATOI_BUFSIZE_RTTOMAX_ERR, (int)bufSize, 0, 0, 0, 0, 0);
		return GM_RSSP1_FALSE;
	}
#ifdef Platform_2oo3
	GM_RSSP1_Log_Usr(GM_RSSP1_ATOI_BUFSIZE_2OO3_ERR, 0, 0, 0, 0, 0, 0);
	return GM_RSSP1_FALSE;
#else
	*pVal = (int)strtol(pBuf, NULL, 10);
	/*	*pVal = atoi(pBuf); �����Ϊstrtol������atoi���ݲ�ͬ����ϵͳ�����������ڳ�����Χʱ��õ���ͬ��� wtt 20111116*/
	return GM_RSSP1_TRUE;
#endif
}

#if(RSSP1_LOG_LEVEL >= -10)/*��-10��-1��ɿɶ����level�ȼ���GM������С��0���û��������*/
void GM_RSSP1_Log_Msg(
    int level,
    const char* fmt,  /* format string for print */
    int    arg1, /* first of six required args for fmt */
    int    arg2,
    int    arg3,
    int    arg4,
    int    arg5,
    int    arg6
)
{
    if ( access("a.txt", 0) == 0 && access("a.txt", 4) != 0)
    {
        return;
    }
	if(level == -6)
	{
	FILE* fp = fopen("a.txt", "a+");
	fprintf(fp, fmt, arg1, arg2, arg3, arg4, arg5, arg6);
	fclose(fp);
	}
	if (level <= RSSP1_LOG_LEVEL)
    {
#ifdef VXWORKS
        logMsg(
            fmt,
            arg1,
            arg2,
            arg3,
            arg4,
            arg5,
            arg6
        );
#endif

#ifdef WIN32
#ifndef NO_PRINTF
		printf(
			fmt,
			arg1,
			arg2,
			arg3,
			arg4,
			arg5,
			arg6
			);
		printf("\n");
#endif
#endif
#ifdef QNX
#ifndef NO_PRINTF
        printf(
            fmt,
            arg1,
            arg2,
            arg3,
            arg4,
            arg5,
            arg6
        );
#endif
#endif
    }

    return;
}
#endif


void GM_RSSP1_Log_Err(unsigned int errNo, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6)
{
	return;
}

GM_RSSP1_BOOL GM_RSSP1_Log_Usr_Callback_Init(GM_RSSP1_LOG_CALL_BACK_FUN log_err_fun)
{
	if (NULL == log_err_fun)
	{
		return GM_RSSP1_FALSE;
	}

	GM_RSSP1_Log_Usr = log_err_fun;

	return GM_RSSP1_TRUE;
}

GM_RSSP1_BOOL GM_RSSP1_Code_Is_Big_Endian(void)
{
    GM_RSSP1_BOOL isBigEndian		   = GM_RSSP1_FALSE;
    GM_RSSP1_UINT16 value            = 0x55AAU;

    if (0x55U == *((GM_RSSP1_UINT8*)(&value)))
    {
        isBigEndian = GM_RSSP1_TRUE;
    }
    else
    {
        isBigEndian = GM_RSSP1_FALSE;
    }

    return isBigEndian;
}

GM_RSSP1_UINT16 GM_RSSP1_Code_Big_Little_Endian16(GM_RSSP1_UINT16 x)
{
    GM_RSSP1_UINT16 y = 0U;
    GM_RSSP1_UINT16 z = 0U;

    z = x & 0x00FFU;
    y = z << 8U;
    z = x & 0xFF00U;
    y += (z >> 8U);
    return y;
}

GM_RSSP1_UINT32 GM_RSSP1_Code_Big_Little_Endian32(GM_RSSP1_UINT32 x)
{
    GM_RSSP1_UINT32 y = 0U;
    GM_RSSP1_UINT32 z = 0U;

    z = x & 0x000000FFU;
    y = z << 24U;
    z = x & 0x0000FF00U;
    y += (z << 8U);
    z = x & 0x00FF0000U;
    y += (z >> 8U);
    z = x & 0xFF000000U;
    y += (z >> 24U);
    return y;
}

