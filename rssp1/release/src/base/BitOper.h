/**
* @file BitOper.h
* @brief bit operation
* @author DuYuesheng
* @date 2011.12.14
*/

#pragma once

#include <bitset>
#include <algorithm>

class CBitOper
{
public:
	static bool IsBitStr(const char* pStr)
	{
		ASSERT(pStr);
		return std::all_of(pStr, pStr + strlen(pStr), [](unsigned char c) { return c == '0' || c == '1'; });
	}

	static bool IsEqual(unsigned char val, size_t nBitBegin, size_t nBitCount, const char* pStr)
	{
		ASSERT(pStr);
		ASSERT(nBitBegin < 8 && nBitBegin + nBitCount <= 8);
		val <<= 8 - (nBitBegin + nBitCount);
		val >>= 8 - nBitCount;
		return std::bitset<8>(val) == std::bitset<8>(pStr);
	}

	static std::string ToStr(unsigned char val)
	{
		return ToStr<0, 8>(val);
	}

	template<size_t nBitBegin, size_t nBitCount>
	static std::string ToStr(unsigned char val)
	{
		ASSERT(nBitBegin < 8 && nBitBegin + nBitCount <= 8);
		val <<= 8 - (nBitBegin + nBitCount);
		val >>= 8 - nBitCount;
		return std::bitset<nBitCount>(val).to_string();
	}

	static unsigned char ToNum(unsigned char val, size_t nBitBegin, size_t nBitCount)
	{
		ASSERT(nBitBegin < 8 && nBitBegin + nBitCount <= 8);
		unsigned char nRet = val;
		nRet <<= 8 - (nBitBegin + nBitCount);
		nRet >>= 8 - nBitCount;
		return nRet;
	}

	static void SetByte(unsigned char& val, const char* pStr)
	{
		ASSERT(pStr);
		ASSERT(strlen(pStr) <= 8 && IsBitStr(pStr));
		val = (unsigned char)std::bitset<8>(pStr).to_ulong();
	}

	static void SetBits(unsigned char& val, size_t nBitBegin, const char* pStr)
	{
		ASSERT(pStr);
		ASSERT(strlen(pStr) + nBitBegin <= 8 && IsBitStr(pStr));
		std::bitset<8> dest(val);
		for (size_t i = 0; i < strlen(pStr); i++)
			dest.set(nBitBegin + i, false);
		std::bitset<8> source(pStr);
		source <<= nBitBegin;
		dest |= source;
		val = (unsigned char)dest.to_ulong();
	}
};