
// FSFB2Sim.h : FSFB2Sim 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号


// CFSFB2SimApp:
// 有关此类的实现，请参阅 FSFB2Sim.cpp
//

class CFSFB2SimApp : public CWinAppEx
{
public:
	CFSFB2SimApp();


// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	void LoadTBVersion();

// 实现
	BOOL  m_bHiColorIcons;

	CString m_strTBVersion;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CFSFB2SimApp theApp;
