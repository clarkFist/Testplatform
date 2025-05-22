/**
* @file LogMsg.h
* @brief 
* @author DuYuesheng
* @date 2012.03.22
*
* This class is simple,flexible and THREAD-SAFE. By default, Log method will TRACE the msg, but you can modify 
* the behavier by TLogMsg::Init. You can call the mothods of TLogMsg directly, but it is strongly recommended
* that you call Macro such as LOG,LOG_ERR,etc. Maybe TLogMsg::Init is the only method you should call directly.
* Here is an example:
* // At the beginning of your app. 
* TSingleton<TLogMsg>::Get()->Init([](LPCSTR str){cout << str << endl;});
* // After Init,you can use it anywhere.
* LOG(("Hello world"));
* LOG(("Name:%s,Age:%d", "Tom", 18));
* LOG_ERR(("File write failed"), GetLastError());
* LOG_ERR(("Communication with %s disconnect", "192.100.100.11"), ACE_OS::last_error());
* LOG_ERR_FUNC(("TCP port %d open error", 40000), ACE_OS::last_error());
* // Remember to release memory before your app end.
* TSingleton<TLogMsg>::Free();
*/

#pragma once

#include <functional>
#include "Singleton.h"

class TLogMsg
{
public:
	SINGLETON(TLogMsg);

	void Init(const std::function<void(LPCSTR)>& fnProcessMsg);
	void SetConditionalValues(bool bIsSet, int nErrnum = -1, LPCSTR pszFunc = NULL, LPCSTR pszFile = NULL, int nLine = -1);
	void Log(LPCSTR pszFormat, ...);

private:
	TLogMsg()
		: m_fnProcessMsg( [](LPCSTR pszText){ TRACE(pszText); TRACE("\n"); } )
	{
	}

	struct ConditionalValues
	{
		bool m_bIsSet;
		int m_nErrnum;
		LPCSTR m_pszFunc;
		LPCSTR m_pszFile;
		int m_nLine;
	};

	ConditionalValues* GetConditionalValues();

	std::function<void(LPCSTR)> m_fnProcessMsg;
};

inline void TLogMsg::SetConditionalValues(bool bIsSet, int nErrnum /* = -1 */, LPCSTR pszFunc /* = NULL */, LPCSTR pszFile /* = NULL */, int nLine /* = -1 */)
{
	ConditionalValues* pCV = GetConditionalValues();
	pCV->m_bIsSet	= bIsSet;
	pCV->m_nErrnum	= nErrnum;
	pCV->m_pszFunc	= pszFunc;
	pCV->m_pszFile	= pszFile;
	pCV->m_nLine	= nLine;
}

inline TLogMsg::ConditionalValues* TLogMsg::GetConditionalValues()
{
	__declspec(thread) static ConditionalValues s_ConditionalValues;
	return &s_ConditionalValues;
}

inline void TLogMsg::Init(const std::function<void(LPCSTR)>& fnProcessMsg)
{
	m_fnProcessMsg = fnProcessMsg;
}

inline void TLogMsg::Log(LPCSTR pszFormat, ...)
{
	ASSERT(pszFormat);
	if (m_fnProcessMsg == NULL)
		return;

	va_list args;
	va_start(args, pszFormat);
	int nLen = _vscprintf(pszFormat, args) + 1; // terminating '\0'
	char* pBuffer = new char[nLen];
	vsprintf_s(pBuffer, nLen, pszFormat, args);
	va_end(args);

	CString strMsg;
	strMsg.Format("%s  %s", CTime::GetCurrentTime().Format("%y-%m-%d %H:%M:%S"), pBuffer);
	delete []pBuffer;
	const ConditionalValues* pCV = GetConditionalValues();
	if (pCV->m_bIsSet)
	{
		strMsg += " --> ";
		CString str;
		if (pCV->m_nErrnum >= 0)
		{
			str.Format("errno:%d ", pCV->m_nErrnum);
			strMsg += str;
		}
		if (pCV->m_pszFunc)
		{
			str.Format("fn:%s ", pCV->m_pszFunc);
			strMsg += str;
		}
		if (pCV->m_pszFile)
		{
			str.Format("file:%s ", pCV->m_pszFile);
			strMsg += str;
		}
		if (pCV->m_nLine >= 0)
		{
			str.Format("line:%d ", pCV->m_nLine);
			strMsg += str;
		}
	}
	m_fnProcessMsg(strMsg);
}

#define LOG(X)  \
	do { \
	TLogMsg* pObj = TSingleton<TLogMsg>::Get(); \
	pObj->SetConditionalValues(false); \
	pObj->Log X; \
	} while (0)

#define LOG_ERR(X, err) \
	do { \
	TLogMsg* pObj = TSingleton<TLogMsg>::Get(); \
	pObj->SetConditionalValues(true, err); \
	pObj->Log X; \
	} while (0)

#define LOG_FUNC(X) \
	do { \
	TLogMsg* pObj = TSingleton<TLogMsg>::Get(); \
	pObj->SetConditionalValues(true, -1, __FUNCTION__); \
	pObj->Log X; \
	} while (0)

#define LOG_FILELINE(X) \
	do { \
	TLogMsg* pObj = TSingleton<TLogMsg>::Get(); \
	pObj->SetConditionalValues(true, -1, NULL, __FILE__, __LINE__); \
	pObj->Log X; \
	} while (0)

#define LOG_ERR_FUNC(X, err) \
	do { \
	TLogMsg* pObj = TSingleton<TLogMsg>::Get(); \
	pObj->SetConditionalValues(true, err, __FUNCTION__); \
	pObj->Log X; \
	} while (0)
