
// FSFB2Sim.h : FSFB2Sim Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CFSFB2SimApp:
// �йش����ʵ�֣������ FSFB2Sim.cpp
//

class CFSFB2SimApp : public CWinAppEx
{
public:
	CFSFB2SimApp();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	void LoadTBVersion();

// ʵ��
	BOOL  m_bHiColorIcons;

	CString m_strTBVersion;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CFSFB2SimApp theApp;
