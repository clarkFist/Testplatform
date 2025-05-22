/**
* @file AppUtility.h
* @brief Tools for application
* @author DuYuesheng
* @date 2008.01.09
*/

#pragma once

/**
* Get the  path of the application
*/
inline DWORD GetModuleFilePath(CString &strPath)
{
	char dir[MAX_PATH] = "\0";
	DWORD dwRet = ::GetModuleFileName(NULL, dir, MAX_PATH);
	*::strrchr(dir, '\\') = 0;
	strPath = dir;
	return dwRet;
}

/**
* 根据文件名得到不含文件名得路径名，如："SVA\CFG\DIGIT.INI" ---> "SVA\CFG"
*/
inline BOOL GetDirectoryFromFile(LPCSTR pszFileName, CString& strDir)
{
	ASSERT(pszFileName);
	const char* p = strrchr(pszFileName, '\\');
	if (p == NULL)
		return FALSE;
	strDir = CString(pszFileName, p - pszFileName);
	return TRUE;
}

/**
* @param lpszPath [in] 欲创建的目录
* @param cDelimiter [in] 目录分界符号：\ 或 /
* @return 如果创建目录返回TRUE，否则返回FALSE。如果目录已经存在，函数不会改变目录内容，返回TRUE
* @remarks 一个更好的创建目录的函数，比CreateDirectory强的是，它可以直接创建不存在的多级目录
*/
inline BOOL CreateDirectoryRecursive(LPCSTR lpszPath, char cDelimiter = '\\')
{
	ASSERT(lpszPath);
	ASSERT(cDelimiter == '\\' || cDelimiter == '/');
	const int nLen = strlen(lpszPath);
	if (nLen < 1)
		return FALSE;
	CString strPath = lpszPath;
	if (strPath.GetAt(nLen - 1) != cDelimiter)
		strPath.Insert(nLen, cDelimiter);
	int idx = 0;
	int nRet = -1;
	DWORD dwRet;
	CString strTemp;
	while ((nRet = strPath.Find(cDelimiter, idx)) != -1)
	{
		strTemp = strPath.Left(nRet + 1);
		dwRet = ::GetFileAttributes(strTemp); // 此函数在strTemp不存在的情况下返回0xffffffff
		if (dwRet == 0xffffffff || !(dwRet & FILE_ATTRIBUTE_DIRECTORY)) // 不存在 or 虽存在，但不是目录
		{
			if (!::CreateDirectory(strTemp, NULL))
			{
				CString strError;
				LPSTR lpMsgBuf;
				::FormatMessage( 
					FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					FORMAT_MESSAGE_FROM_SYSTEM | 
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					::GetLastError(),
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
					(LPTSTR) &lpMsgBuf,
					0, NULL);
				strError.Format("创建目录%s失败：%s", strTemp, lpMsgBuf);
				TRACE(strError);
				return FALSE;
			}
		}
		idx = nRet + 1;
	}
	return TRUE;
}

inline CString GetAppVersion(UINT nVerID)
{
	CString strVer;
	strVer.LoadString(nVerID);
	return strVer;
}

inline CString GetBuildString()
{
	char dir[MAX_PATH] = "\0";
	DWORD dwRet = ::GetModuleFileName(NULL, dir, MAX_PATH);
	CFileStatus status;
	CFile::GetStatus(dir, status);
	return status.m_mtime.Format("%y%m%d-%H%M%S");
}

inline CString GetBuildString(time_t t)
{
	CTime tmp(t);
	return tmp.Format("%y%m%d-%H%M%S");
}