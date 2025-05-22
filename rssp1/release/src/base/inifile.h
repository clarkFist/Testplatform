/**
* @file IniFile.h
* @brief ini file operation wrapper class, platform independent.
* @author DuYuesheng
* @date 2005.11.23
* The modification will not write the ini file immediately when you set m_bFullBuf to true(the so called "full buffer mode"). 
* The modification will be saved to the ini file in the TIniFile's destructor. If you want to take effect immediately, you can call TIniFile::Flush.
* Warning:At "full buffer mode", when two TIniFile objects write the same ini file, it's easy to lose modification. Here is an example for this case:
*				TIniFile a("test.ini");
*				a.PutIniSetting(...);
*				TIniFile b("test.ini");
*				b.PutIniSetting(...);
*				...
* In the above example, both the TIniFile object a and b operate test.ini. As a result, only the one object's modification will be saved. The second
* TIniFile's destructor call will overlap the modification which is saved in the first TIniFile's destructor. There are serveral ways to resolve this
* problem. 
* 1.  you can add "a.Flush()" after "a.PutIniSetting(...)".
* 2. or you can set m_bFullBuf to FALSE.(the so called "non-full buffer mode")
* In a word, the "non-full buffer mode" is safer, but it will always write the ini file immediately when you modify the file. So in some cases, it will be 
* inefficient.
* Remarks: TIniFile will not modify the ini file's comment and blank line and etc. It behaves the same way with Microsoft's raw ini routines.
*/

#ifndef __INIFILE_H__
#define __INIFILE_H__

#ifdef _MSC_VER
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif

#include <assert.h>
#include <vector>
#include <list>
#include <string>
#include <fstream>
#include <algorithm>

class TIniFile
{
public:
	class SectionIterator
	{
	public:
		friend class TIniFile;

		SectionIterator()
			: m_pIniFile(NULL), m_pSection(NULL)
		{
		}			
		SectionIterator(const SectionIterator& other)
			: m_pIniFile(other.m_pIniFile), m_pSection(other.m_pSection), m_pos(other.m_pos)
		{
		}
		SectionIterator& operator=(const SectionIterator& other)
		{
			if (this == &other)
				return *this;
			this->m_pIniFile = other.m_pIniFile;
			this->m_pSection = other.m_pSection;
			this->m_pos = other.m_pos;
			return *this;
		}
		void Next() 
		{
			if (IsDone())
				throw std::out_of_range("out_of_range in TIniFile::SectionIterator::Next()");
			m_pos = std::find_if(++m_pos, ((const std::list<std::string>*)m_pSection)->end(), TIniFile::find_section_t(NULL, m_pIniFile));
		}
		bool IsDone() const 
		{ 
			return m_pos == m_pSection->end(); 
		}
		std::pair<std::string, std::string> Data() const 
		{
			std::string strKey;
			std::string strValue;
			if (!m_pIniFile->GetKeyValue(m_pos->c_str(), &strKey, &strValue))
				throw std::out_of_range("out_of_range in TIniFile::SectionIterator::Data()");
			return std::make_pair(strKey, strValue);
		}

	private:
		TIniFile* m_pIniFile;
		std::list<std::string>* m_pSection;
		std::list<std::string>::const_iterator m_pos;
	};
	friend class SectionIterator;

public:
	TIniFile(const char* lpszFileName, bool bFullBuf = true);
	TIniFile(std::iostream& stream, bool bFullBuf = true);
	~TIniFile();

	size_t GetIniSections(std::vector<std::string>& vSections) const;
	size_t GetIniSection(const char* pszSection, std::vector<std::string>& vKeys) const;
	bool GetIniSection(const char* pszSection, SectionIterator& iter) const;
	bool GetIniSetting(const char* pszSection, const char* pszKey, std::string& szValue) const;
	void PutIniSetting(const char* pszSection, const char* pszKey, const char* pszValue);
	void RemoveIniSetting(const char* pszSection, const char* pszKey);
	void Flush();

private:
	typedef std::list<std::string> INISection;
	typedef std::list<std::pair<std::string, INISection> > INIFile;
		
	/**
	* @name functor
	*/
	//@{
	class find_inifile_t
	{
	public:
		find_inifile_t(const char* pszSection, const TIniFile* pIniFile)
			: m_pIniFile(const_cast<TIniFile*>(pIniFile)), m_strFind(pszSection) { assert(pIniFile && pszSection); }
		bool operator()(const std::pair<std::string, INISection>& obj) const
		{
			std::string str;
			if (!m_pIniFile->GetSectionString(obj.first.c_str(), &str))
				return false;
			return _stricmp(str.c_str(), m_strFind.c_str()) == 0;
		}
		
	private:
		TIniFile* m_pIniFile;
		std::string m_strFind;
	};
	friend class find_inifile_t;

	class find_section_t
	{
	public:
		find_section_t(const char* pszKey, const TIniFile* pIniFile, std::string* pOutString = NULL)
			: m_pStrValue(pOutString), m_pIniFile(const_cast<TIniFile*>(pIniFile)), m_bAny(pszKey == NULL ? true : false), m_strFind(pszKey == NULL ? "" : pszKey) 
		{ 
			assert(pIniFile); 
		}
		find_section_t(const find_section_t& other)
			: m_pStrValue(other.m_pStrValue), m_pIniFile(other.m_pIniFile), m_bAny(other.m_bAny), m_strFind(other.m_strFind)
		{
		}
		bool operator()(const std::string& obj)
		{
			std::string str;
			if (!m_pIniFile->GetKeyValue(obj.c_str(), &str, m_pStrValue))
				return false;
			return m_bAny ? true : (_stricmp(str.c_str(), m_strFind.c_str()) == 0);
		}

	private:
		std::string* m_pStrValue; // [out]

		TIniFile* m_pIniFile;
		bool m_bAny;
		std::string m_strFind;
	};
	friend class find_section_t;
	//@}

#ifndef _MSC_VER
	static int stricmp(const char* pszOne, const char* pszTwo)
	{
		assert(pszOne && pszTwo);
		char* pOne = new char[strlen(pszOne) + 1];
		strcpy(pOne, pszOne);
		strupr(pOne);

		char* pTwo = new char[strlen(pszTwo) + 1];
		strcpy(pTwo, pszTwo);
		strupr(pTwo);

		int nRet = strcmp(pOne, pTwo);
		delete []pOne;
		delete []pTwo;
		return nRet;
	}
#endif

private:
	void LoadIni();
	bool GetSectionString(const char* pszLine, std::string* pStr = NULL) const;
	bool GetKeyValue(const char* pszLine, std::string* pStrKey = NULL, std::string* pStrValue = NULL) const;
	INIFile::iterator FindSection(const char* pszSection) const;
	INISection::iterator FindKey(const INISection& section,  const char* pszKey, std::string* pStrValue = NULL) const;
	std::string TrimSubStr(const char* pszSource) const;
	
	bool m_bModified;			///< modification flag
	bool m_bFullBuf;			///< "full buffer mode" flag
	INIFile m_iniFileMap;		///< the buffer storing the content of the ini file
	std::iostream* m_pStream;	///< the ini file that you want to operate
	bool m_bNeedRelease;
};

inline TIniFile::TIniFile(const char* lpszFileName, bool bFullBuf /*= true*/)
: m_bModified(false), m_bFullBuf(bFullBuf), m_bNeedRelease(true)
{
	assert(lpszFileName);
#if	_MSC_VER >= 1400 // >=VC2005
	std::locale old = std::locale::global(std::locale(""));
#endif
	m_pStream = new std::fstream(lpszFileName, std::ios::in | std::ios::out);
#if _MSC_VER >= 1400 // >=VC2005
	std::locale::global(std::locale(old));
#endif
	assert(m_pStream);
	assert(static_cast<std::fstream*>(m_pStream)->is_open());
	LoadIni();
}

inline TIniFile::TIniFile(std::iostream& stream, bool bFullBuf /* = true */)
: m_bModified(false), m_bFullBuf(bFullBuf), m_bNeedRelease(false)
{
	m_pStream = &stream;
	LoadIni();
}

inline TIniFile::~TIniFile()
{
	if (m_bModified)
		Flush();
	if (m_bNeedRelease)
		delete m_pStream;
}

/**
* Trim all leading and trailing whitespace and '\t' characters from the string.
*/
inline std::string TIniFile::TrimSubStr(const char* pszSource) const
{
	assert(pszSource);
	std::string strDummy = pszSource;
	std::string::size_type idxBegin;
	std::string::size_type idxEnd;
	if ((idxBegin = strDummy.find_first_not_of(" \t")) == strDummy.npos) // The string has nothing but whitespace or '\t'
		return "";
	idxEnd = strDummy.find_last_not_of(" \t");
	return strDummy.substr(idxBegin, 1 + idxEnd - idxBegin);
}

/**
* @param vSections [out] All section names in the ini file
* @return the count of vSection
*/
inline size_t TIniFile::GetIniSections(std::vector<std::string>& vSections) const
{
	vSections.clear();
	std::string strSection;
	INIFile::const_iterator pos;
	vSections.reserve(m_iniFileMap.size());
	for (pos = m_iniFileMap.begin(); pos != m_iniFileMap.end(); ++pos)
	{
		if (!GetSectionString(pos->first.c_str(), &strSection))
			continue;
		vSections.push_back(strSection);
	}
	std::vector<std::string>(vSections).swap(vSections);
	return vSections.size();
}

/**
* @param pszSection [in] Section name
* @param vKeys [out] All key name in the section
* @return the count of the vKeys
*/
inline size_t TIniFile::GetIniSection(const char* pszSection, std::vector<std::string>& vKeys) const
{
	assert(pszSection);
	
	std::string strDummy = TrimSubStr(pszSection);
	assert(!strDummy.empty());
	vKeys.clear();
	INIFile::const_iterator pos;
	if ((pos = FindSection(strDummy.c_str())) == m_iniFileMap.end())
		return false;
	const INISection& sec = pos->second;
	INISection::const_iterator posSec;
	std::string strKey;
	vKeys.reserve(sec.size());
	for (posSec = sec.begin(); posSec != sec.end(); ++posSec)
	{
		if (!GetKeyValue(posSec->c_str(), &strKey))
			continue;
		vKeys.push_back(strKey);
	}
	std::vector<std::string>(vKeys).swap(vKeys);
	return vKeys.size();
}

/**
* Note:please check the return value first. If false, no section exist and iter value will not be changed.
*/
inline bool TIniFile::GetIniSection(const char* pszSection, SectionIterator& iter) const
{
	assert(pszSection);

	std::string strDummy = TrimSubStr(pszSection);
	assert(!strDummy.empty());

	INIFile::const_iterator pos;
	if ((pos = FindSection(strDummy.c_str())) == m_iniFileMap.end())
		return false;
	iter.m_pIniFile = const_cast<TIniFile*>(this);
	iter.m_pSection = const_cast<INISection*>(&pos->second);
	iter.m_pos = std::find_if(pos->second.begin(), ((const INISection*)iter.m_pSection)->end(), find_section_t(NULL, this));
	return iter.m_pos != pos->second.end();
}

/**
* Note:please check the return value first. If false, no section-key exist and szValue will not be changed.
*/
inline bool TIniFile::GetIniSetting(const char* pszSection, const char* pszKey, std::string& szValue) const
{
	assert(pszSection && pszKey);
	std::string strDummySection = TrimSubStr(pszSection);
	assert(!strDummySection.empty());
	std::string strDummyKey = TrimSubStr(pszKey);
	assert(!strDummyKey.empty());

	INIFile::const_iterator pos;
	if ((pos = FindSection(strDummySection.c_str())) == m_iniFileMap.end())
		return false;
	// Some STL version implements operator!= as iterator's member method other than global function. (Awful ! The STL version VC6 used just like this!)
	return pos->second.end() != FindKey(pos->second, strDummyKey.c_str(), &szValue);
}

inline TIniFile::INIFile::iterator TIniFile::FindSection(const char* pszSection) const
{
	assert(pszSection);	
	TIniFile& rThis = const_cast<TIniFile&>(*this);
	return std::find_if(rThis.m_iniFileMap.begin(), rThis.m_iniFileMap.end(), find_inifile_t(pszSection, this));
}

inline TIniFile::INISection::iterator TIniFile::FindKey(const INISection& section, const char* pszKey, std::string* pStrValue /*= NULL*/) const
{
	assert(pszKey);

	INISection::iterator pos;
	INISection& rSection = const_cast<INISection&>(section);
	std::string strRet;
	find_section_t ftor(pszKey, this, &strRet);
	if ((pos = std::find_if<INISection::iterator, find_section_t&>(rSection.begin(), rSection.end(), ftor)) != rSection.end())
	{
		if (pStrValue)
			*pStrValue = strRet;
	}
	return pos;
}

inline void TIniFile::PutIniSetting(const char* pszSection, const char* pszKey, const char* pszValue)
{
	assert(pszSection && pszKey && pszValue);

	std::string strDummySection = TrimSubStr(pszSection);
	assert(!strDummySection.empty());
	std::string strDummyKey = TrimSubStr(pszKey);
	assert(!strDummyKey.empty());
	std::string strDummyValue = TrimSubStr(pszValue);

	INIFile::iterator posFile;
	INISection::iterator posSection;
	std::string strValue;
	std::string strLine(strDummyKey);
	strLine += "=";
	strLine += strDummyValue;
	if ((posFile = FindSection(strDummySection.c_str())) == m_iniFileMap.end()) // section doesn't exist
	{
		INISection tmp;
		tmp.push_back(strLine);
		m_iniFileMap.push_back(std::make_pair("[" + strDummySection + "]", tmp));
		m_bModified = true;
	}
	else if ((posSection = FindKey(posFile->second, strDummyKey.c_str(), &strValue)) == posFile->second.end()) // key doesn't exist
	{
		posFile->second.push_back(strLine);
		m_bModified = true;
	}
	else if (strValue != strDummyValue) // section and key already exist
	{
		*posSection = strLine;
		m_bModified = true;
	}

	if (m_bModified && !m_bFullBuf)
		Flush();
}

inline void TIniFile::RemoveIniSetting(const char* pszSection, const char* pszKey)
{
	assert(pszSection && pszKey);
	std::string strDummySection = TrimSubStr(pszSection);
	assert(!strDummySection.empty());
	std::string strDummyKey = TrimSubStr(pszKey);
	assert(!strDummyKey.empty());

	INIFile::iterator posFile;
	if ((posFile = FindSection(strDummySection.c_str())) == m_iniFileMap.end())
		return;
	INISection::iterator posSection;
	if ((posSection = FindKey(posFile->second, strDummyKey.c_str())) == posFile->second.end())
		return;
	posFile->second.erase(posSection);
	m_bModified = true;
	if (!m_bFullBuf)
		Flush();
}

inline void TIniFile::Flush()
{
	m_pStream->clear();
	m_pStream->seekp(0, std::ios_base::beg);
	
	bool bFirst; // The first line doesn't need to output endl and the others will output endl first.
	INIFile::const_iterator posFile;
	INISection::const_iterator posSection;
	for (posFile = m_iniFileMap.begin(); posFile != m_iniFileMap.end(); ++posFile)
	{
		bFirst = (posFile == m_iniFileMap.begin());
		const std::pair<std::string, INISection>& rNode = *posFile;
		if (!rNode.first.empty())
		{
			if (bFirst)
			{
				*m_pStream << rNode.first;
				bFirst = false;
			}
			else
				*m_pStream << std::endl << rNode.first;
		}
		for (posSection = rNode.second.begin(); posSection != rNode.second.end(); ++posSection)
		{
			if (bFirst)
			{
				*m_pStream << *posSection;
				bFirst = false;
			}
			else
				*m_pStream << std::endl << *posSection;
		}
	}
	m_pStream->flush();
	m_bModified = false;
}

inline void TIniFile::LoadIni()
{
	m_pStream->clear();
	m_pStream->seekg(0, std::ios_base::beg);
	while (m_pStream->good())
	{
		std::string strLine;
		std::getline(*m_pStream, strLine);
		if (GetSectionString(strLine.c_str()))
		{
			m_iniFileMap.push_back(std::make_pair(strLine, INISection()));
		}
		else
		{
			if (m_iniFileMap.empty())
			{
				INISection tmp;
				tmp.push_back(strLine);
				m_iniFileMap.push_back(std::make_pair(std::string(""), tmp));
				continue;
			}
			std::pair<std::string, INISection>& last = m_iniFileMap.back();
			last.second.push_back(strLine);
		}
	}
}

inline bool TIniFile::GetSectionString(const char* pszLine, std::string* pStr /* = NULL */) const
{
	assert(pszLine);
	std::string strDummy = pszLine;
	std::string::size_type idxBegin;
	std::string::size_type idxEnd;
	if ((idxBegin = strDummy.find_first_not_of(" \t")) == std::string::npos)
		return false;
	if (strDummy[idxBegin] != '[')
		return false;
	if ((idxEnd = strDummy.find_first_of("]")) == std::string::npos)
		return false;
	std::string::size_type idxTmp = strDummy.find_first_not_of(" \t", 1 + idxBegin);
	if (idxTmp >= idxEnd) // There is nothing or only whitespace and '\t' between '[' and ']'.
		return false;
	if (pStr)
		pStr->assign(strDummy, idxTmp, 1 + strDummy.find_last_not_of(" \t", idxEnd - 1) - idxTmp);
	return true;
}

inline bool TIniFile::GetKeyValue(const char* pszLine, std::string* pStrKey /* = NULL */, std::string* pStrValue /* = NULL */) const
{
	assert(pszLine);
	std::string strDummy = pszLine;
	std::string::size_type idxEqual = strDummy.find_first_of("=");
	if (idxEqual == std::string::npos)
		return false;
	std::string::size_type idxBegin = strDummy.find_first_not_of(" \t");
	if (idxBegin == idxEqual || strDummy[idxBegin] == ';' || strDummy[idxBegin] == '#')
		return false;
	if (pStrKey)
		pStrKey->assign(strDummy, idxBegin, 1 + strDummy.find_last_not_of(" \t", idxEqual - 1) - idxBegin);
	if (pStrValue)
	{
		std::string::size_type idxValueBegin;
		if ((idxValueBegin = strDummy.find_first_not_of(" \t", 1 + idxEqual)) == std::string::npos)
			*pStrValue = "";
		else
			pStrValue->assign(strDummy, idxValueBegin, 1 + strDummy.find_last_not_of(" \t") - idxValueBegin);
	}
	return true;
}
	
#endif