/**
* @file GM_RSSP1_File.h
* @brief ��ȡ�����ļ�
* -'='֮���ֵ����Ϊ��char
* -֧��';'ע��ǰ׺
* -��֧�ֿ���
* @author Hao Liming
* @date 2009-9-18 16:22:00
* @version 0.3
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: </b>haoliming@casco.com.cn</pre>
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>�������޸ĳ�ֻ��ʼ��һ��, ����˶�ȡ����ֵ��ʮ�����Ƶķ���, ������keyǰ׺��ͬ��ȡ�����bug.</pre>
*/

#ifndef _GM_RSSP1_FILE_H
#define _GM_RSSP1_FILE_H

#include "GM_RSSP1_Utils_Other.h"
#include<ctype.h>

#ifdef __cplusplus
extern "C"
{
#endif /**< __cplusplus */

    /**
    * @brief GM_RSSP1_Ini_File_Load
    *
    * ���������ļ���fileContent��.
    * @param[in] filePathAndName �����ļ�·��
    * @param[out] fileContent �����ļ�������
    * @param[in] fileContent ��������ļ����ݵĴ�С
    * @return GM_RSSP1_BOOL
    */
    GM_RSSP1_BOOL GM_RSSP1_Ini_File_Load(const char* filePathAndName, char* fileContent, int fileContentSize);


    /**
    * @brief GM_RSSP1_Ini_File_Read_String
    *
    * ��section�ж�ȡ����key��ֵ��value��.
    * @param[in] section
    * @param[in] key
    * @param[out] value
    * @param[in] size
    * @param[in] iniFileContent
    * @return GM_RSSP1_BOOL
    */
    GM_RSSP1_BOOL GM_RSSP1_Ini_File_Read_String(const char *section, const char *key, char *value, int size, const char* iniFileContent);

    /**
    * @brief GM_RSSP1_Ini_File_Read_Int
    *
    * ��section�ж�ȡ����key��ֵ��value��.
    * @param[in] section
    * @param[in] key
    * @param[out] value
    * @param[in] iniFileContent
    * @return GM_RSSP1_BOOL
    */
	
    GM_RSSP1_BOOL GM_RSSP1_Ini_File_Read_Int(const char *section, const char *key, int* value, const char *iniFileContent);

    /**
    * @brief GM_RSSP1_Ini_File_Read_BOOL
    *
    * ��section�ж�ȡ����key��ֵ��value��.
    * @param[in] section
    * @param[in] key
    * @param[out] value
    * @param[in] iniFileContent
    * @return GM_RSSP1_BOOL
    */
    GM_RSSP1_BOOL GM_RSSP1_Ini_File_Read_BOOL(const char *section, const char *key, GM_RSSP1_BOOL* value, const char *iniFileContent);

    /**
    * @brief GM_RSSP1_Get_File_Size
    *
    * �����ļ�������ļ���С.
    * @param[in] filePathAndName
    * @return int
    */
    /*sjz: ��Windows��Ƕ��ʽ����ϵͳ��VxworksǶ��ʽ����ϵͳ�£���ȡ���ļ���С����ͬ�ģ����س���������ϵͳ�У�����"\n" */
    long GM_RSSP1_Get_File_Size(const char *filePathAndName);

	    /**
    * @brief �Ӷ������ļ���ȡ�����ļ�.
    * @param[in] fileName �ļ�����.
    * @param[out] pBuf ��Ŷ����Ķ��������ݵĻ�����.
    * @param[in] bufSize ��������С
    * @return GM_RSSP1_BOOL
    */
    GM_RSSP1_BOOL GM_RSSP1_Bin_File_Load(const char* fileName, GM_RSSP1_UINT8* pBuf, GM_RSSP1_UINT32 bufSize);



#ifdef __cplusplus
}

#endif /**< __cplusplus */

#endif /**< _GM_INI_FILE_H */

