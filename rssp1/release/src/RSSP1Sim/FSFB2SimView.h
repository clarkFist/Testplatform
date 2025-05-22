
// FSFB2SimView.h : CFSFB2SimView ��Ľӿ�
//

#pragma once

#include "resource.h"
#include "../GridCtrl/GridCtrl_src/GridCtrl.h"
#include "GlueData.h"

#include <map>
#include <vector>

class CFSFB2SimView : public CFormView
{
protected: // �������л�����
	CFSFB2SimView();
	DECLARE_DYNCREATE(CFSFB2SimView)

public:
	enum{ IDD = IDD_FSFB2SIM_FORM };

	enum{ COND_XOR, COND_ADD, COND_OTHER};
// ����
public:
	CFSFB2SimDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual void OnInitialUpdate(); // ������һ�ε���

// ʵ��
public:
	virtual ~CFSFB2SimView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg LRESULT OnInitRSSP1(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRcvMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRefreshSendBuff(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDebugBugInsert();
	afx_msg void OnUpdateDebugBugInsert(CCmdUI *pCmdUI);
	afx_msg void OnGridEndEdit(NMHDR* pNotifyStruct, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

private:
	void UpdateRcvCtrls();
	void UpdateRcvCh1();
	void UpdateRcvCh2();

	void InitRcvCh1();
	void InitRcvCh2();
	void InitSndCh1();
	void InitSndCh2();
	void InitBugInsert();
	void InitRelay();
	void InitCondition();

	void FillSendMsg();
	void FillBugInsert();

	void UpdateChannel(int flag);

	void AddCondition(int cond, CString strCondition);

	void ParseADDCondition(CString strCondition);
	void ParseXORCondition(CString strCondition);

	void TriggerADDCondition(int iRelay, CString strStatus);
	void TriggerXORCondition(int iRelay, CString strStatus);

	RELAY_ADD* GetADDRelay(int iRelay);
	RELAY_XOR* GetXORRelay(int iRelay);

	// ����Ԫ����Ϊ��ƫ��ֵ������
	void SetCellOffsetType(CGridCtrl& grid, int nRow, int nCol);

private:
	char m_rcvMsg[MAX_RSSP1_MSG_LEN];

	CGridCtrl m_ctrlRcvCh1;
	CGridCtrl m_ctrlRcvCh2;
	CGridCtrl m_ctrlSndCh1;
	CGridCtrl m_ctrlSndCh2;
	CGridCtrl m_ctrlBugInsert;
	CGridCtrl m_ctrlErrorCycle;

	BOOL m_bBugInsert;
	BOOL m_bShowRecv;

	std::map<int, CString> m_relayName;
	std::vector<RELAY_ADD> m_relayAdd;
	std::vector<RELAY_XOR> m_relayXor;
public:
	afx_msg void OnDebugShowRecv();
	afx_msg void OnUpdateDebugShowRecv(CCmdUI *pCmdUI);
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnHelpText();
	afx_msg void OnSendGridChangedUp(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSendGridChangedDown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnDebug32777();
};

#ifndef _DEBUG  // FSFB2SimView.cpp �еĵ��԰汾
inline CFSFB2SimDoc* CFSFB2SimView::GetDocument() const
   { return reinterpret_cast<CFSFB2SimDoc*>(m_pDocument); }
#endif

