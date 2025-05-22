/**
* @file MyString.h
* @brief Routines about string operation
* @author DuYuesheng
* @date 2005.12.15
*/

#ifndef __MYSTRING_H__
#define __MYSTRING_H__

#include <assert.h>
#include <string>
#include <sstream>
#include <vector>

#ifdef _MSC_VER

inline CString I2STR(int num)
{
	CString str;
	str.Format("%d", num);
	return str;
}

inline CString I2STR(UINT num)
{
	CString str;
	str.Format("%u", num);
	return str;
}

#endif

/**
* @name powerful and platform independent. These functions can convert not only integer but also float etc.
*/
//@{
template<typename _T> 
inline std::string X2A(const _T& tNumber, int nWidth = 0) 
{ 
	std::ostringstream sOutStream; 
	sOutStream.width(nWidth); 
	sOutStream.fill('0'); 
	sOutStream << tNumber; 
	return sOutStream.str(); 
}

template<typename _T> 
inline _T A2X(const std::string& strNumber, _T def = (_T)0)
{
	_T tResult = def;
	std::stringstream sTranslation;
	sTranslation << strNumber;
	sTranslation >> tResult;
	return tResult;
}

template<typename _T>
inline _T A2X(const char* pszNumber, _T def = (_T)0)
{
	assert(pszNumber);
	std::string strTmp(pszNumber);
	return A2X<_T>(strTmp, def);
}
//@}

inline bool stristr(const char* pszStr, const char* pszSub)
{
	assert(pszStr && pszSub);

	char* pszStrDummy = new char[strlen(pszStr) + 1];
	strcpy(pszStrDummy, pszStr);
	_strupr(pszStrDummy);

	char* pszSubDummy = new char[strlen(pszSub) + 1];
	strcpy(pszSubDummy, pszSub);
	_strupr(pszSubDummy);

	char* p = strstr(pszStrDummy, pszSubDummy);
	delete []pszStrDummy;
	delete []pszSubDummy;
	return p != NULL;
}

inline size_t SpliteStr(const char* pszSource, const char* pszSeparator, std::vector<std::string>& vStr)
{
	assert(pszSource && pszSeparator);
	vStr.clear();
	std::string strDummy(pszSource);
	std::string::size_type old = 0;
	std::string::size_type idx;
	while (TRUE)
	{
		if ((idx = strDummy.find_first_of(pszSeparator, old)) == std::string::npos)
		{
			if (old < strDummy.size())
				vStr.push_back(strDummy.substr(old));
			break;
		}
		if (old < idx)
			vStr.push_back(strDummy.substr(old, idx - old));
		old = 1 + idx;
	}
	std::vector<std::string>(vStr).swap(vStr);
	return vStr.size();
}

/**
* "12A34F" ---> 0x12A34F
* Note:the caller must check the return value first. If return false,you should ingore the out param res!
*/
inline bool HexString2Int(const char* pszHexStr, unsigned int& res)
{
	assert(pszHexStr);
	const size_t len = strlen(pszHexStr);
	// avoid overflow
	if (len > 8)
		return false;
	res = 0; 
	unsigned int tmp = 0;
	for (size_t i = 0; i < len; i++)
	{
		if (pszHexStr[i] >= '0' && pszHexStr[i] <= '9')   
			tmp = pszHexStr[i] - '0'; 
		else if (pszHexStr[i] >= 'a' && pszHexStr[i] <= 'f') 
			tmp = pszHexStr[i] - 'a' + 10;   
		else if (pszHexStr[i] >= 'A' && pszHexStr[i] <= 'F')   
			tmp = pszHexStr[i] - 'A' + 10;   
		else 
			return false;
		tmp <<= ((len - i - 1) << 2);    
		res |= tmp;   
	}
	return true;
}

/**
* "10 00 00 00 3F 51 44 5A" ---> buff
* return the item count in pOut, the caller should release pOut!
*/
inline int HexMemory2Buff(const char* pszHexMemory, char*& pOut)
{
	assert(pszHexMemory);
	pOut = NULL;
	std::vector<std::string> vStr;
	SpliteStr(pszHexMemory, " ", vStr);
	if (vStr.empty())
		return 0;
	int idx = 0;
	pOut = new char[vStr.size()];
	std::vector<std::string>::iterator pos;
	for (pos = vStr.begin(); pos != vStr.end(); ++pos)
	{
		if (pos->size() > 2)
		{
			delete []pOut;
			pOut = NULL;
			return 0;
		}
		unsigned int res;
		if (!HexString2Int(pos->c_str(), res))
		{
			delete []pOut;
			pOut = NULL;
			return 0;
		}
		pOut[idx++] = res;
	}
	return idx;
}

#endif