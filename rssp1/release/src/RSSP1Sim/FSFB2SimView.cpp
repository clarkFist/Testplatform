
// FSFB2SimView.cpp : CFSFB2SimView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "FSFB2Sim.h"
#endif

#include "AppUtility.h"
#include "FSFB2SimDoc.h"
#include "FSFB2SimView.h"
#include "RSSP1_Control.h"

#include "../base/mystring.h"
#include "../GridCtrl/NewCellTypes/GridCellCombo.h"
#include "../base/LogMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
extern char* InitValueGet();

// CFSFB2SimView

IMPLEMENT_DYNCREATE(CFSFB2SimView, CFormView)

BEGIN_MESSAGE_MAP(CFSFB2SimView, CFormView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_MESSAGE(WM_INITRSSP1, OnInitRSSP1)
	ON_MESSAGE(WM_RCVMSG, OnRcvMsg)
	ON_MESSAGE(WM_REFRESH_SEND_BUFF, OnRefreshSendBuff)
	ON_COMMAND(ID_DEBUG_BUG_INSERT, &CFSFB2SimView::OnDebugBugInsert)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_BUG_INSERT, &CFSFB2SimView::OnUpdateDebugBugInsert)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_CUSTOM_BUG_INSERT, OnGridEndEdit)
	ON_COMMAND(ID_DEBUG_SHOW_RECV, &CFSFB2SimView::OnDebugShowRecv)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_SHOW_RECV, &CFSFB2SimView::OnUpdateDebugShowRecv)
	ON_COMMAND(ID_HELP_TEXT, &CFSFB2SimView::OnHelpText)
	ON_BN_CLICKED(IDC_BUTTON_START, &CFSFB2SimView::OnBnClickedButtonStart)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CFSFB2SimView::OnBnClickedButtonStop)
	ON_NOTIFY(GVN_SELCHANGEDUP, IDC_CUSTOM_SND_CH1, OnSendGridChangedUp)
	ON_NOTIFY(GVN_SELCHANGEDDOWN, IDC_CUSTOM_SND_CH1, OnSendGridChangedDown)
	ON_BN_CLICKED(IDC_BUTTON2, &CFSFB2SimView::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON4, &CFSFB2SimView::OnBnClickedButton4)
	ON_COMMAND(ID_DEBUG_32777, &CFSFB2SimView::OnDebug32777)
END_MESSAGE_MAP()

// CFSFB2SimView 构造/析构

CFSFB2SimView::CFSFB2SimView()
	: CFormView(CFSFB2SimView::IDD)
	, m_bBugInsert(FALSE),m_bShowRecv(FALSE)
{
	memset(&m_rcvMsg, 0, sizeof(m_rcvMsg));
}

CFSFB2SimView::~CFSFB2SimView()
{
}

void CFSFB2SimView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUSTOM_RCV_CH1, m_ctrlRcvCh1);
	DDX_Control(pDX, IDC_CUSTOM_RCV_CH2, m_ctrlRcvCh2);
	DDX_Control(pDX, IDC_CUSTOM_SND_CH1, m_ctrlSndCh1);
	DDX_Control(pDX, IDC_CUSTOM_SND_CH2, m_ctrlSndCh2);
	DDX_Control(pDX, IDC_CUSTOM_BUG_INSERT, m_ctrlBugInsert);
	DDX_Control(pDX, IDC_Error_cycle, m_ctrlErrorCycle);
}

BOOL CFSFB2SimView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CFormView::PreCreateWindow(cs);
}

void CFSFB2SimView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	g_hFormView = m_hWnd;
	g_bugInsert.m_start = false;
	
	InitBugInsert();

	InitRelay();

	InitCondition();

	InvokeRSSP1SimTask();
}

LRESULT CFSFB2SimView::OnInitRSSP1(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	InitRcvCh1();
	InitRcvCh2();
	InitSndCh1();
	InitSndCh2();

	SetDlgItemText(IDC_EDIT_TX_CDW, "8000");

	return 0;
}

void CFSFB2SimView::InitRcvCh1()
{
	m_ctrlRcvCh1.SetFixedRowCount(1);
	m_ctrlRcvCh1.SetFixedColumnCount(1);
	m_ctrlRcvCh1.SetRowCount(1 + g_nNumcdwRecv);
	m_ctrlRcvCh1.SetColumnCount(1 + 1);

	// fixed row
	m_ctrlRcvCh1.SetItemText(0, 1, "bool");

	for (int nRow = 1; nRow < m_ctrlRcvCh1.GetRowCount(); nRow++)
	{
		m_ctrlRcvCh1.SetItemState(nRow, 1, m_ctrlRcvCh1.GetItemState(nRow, 1) | GVIS_READONLY);
		m_ctrlRcvCh1.SetItemText(nRow, 0, I2STR(nRow));
	}

	m_ctrlRcvCh1.AutoSizeColumns();
	m_ctrlRcvCh1.AutoSizeRows();
}

void CFSFB2SimView::InitRcvCh2()
{
	m_ctrlRcvCh2.SetFixedRowCount(1);
	m_ctrlRcvCh2.SetFixedColumnCount(1);
	m_ctrlRcvCh2.SetRowCount(1 + g_nNumcdwRecv);
	m_ctrlRcvCh2.SetColumnCount(1 + 1);

	// fixed row
	m_ctrlRcvCh2.SetItemText(0, 1, "bool");

	for (int nRow = 1; nRow < m_ctrlRcvCh2.GetRowCount(); nRow++)
	{
		m_ctrlRcvCh2.SetItemState(nRow, 1, m_ctrlRcvCh2.GetItemState(nRow, 1) | GVIS_READONLY);
		m_ctrlRcvCh2.SetItemText(nRow, 0, I2STR(nRow));
	}

	m_ctrlRcvCh2.AutoSizeColumns();
	m_ctrlRcvCh2.AutoSizeRows();
}

void CFSFB2SimView::InitSndCh1()
{
	m_ctrlSndCh1.SetFixedRowCount(1);
	m_ctrlSndCh1.SetFixedColumnCount(1);
	m_ctrlSndCh1.SetRowCount(1 + g_nNumcdw);
	m_ctrlSndCh1.SetColumnCount(1 + 2);

	// fixed row
	m_ctrlSndCh1.SetItemText(0, 1, "名称");
	m_ctrlSndCh1.SetItemText(0, 2, "继电器");

	for (int nRow = 1; nRow < m_ctrlSndCh1.GetRowCount(); nRow++)
	{
		m_ctrlSndCh1.SetItemText(nRow, 0, I2STR(nRow));

		// col 1
		std::map<int, CString>::iterator name_iter = m_relayName.find(nRow);
		if (name_iter != m_relayName.end())
			m_ctrlSndCh1.SetItemText(nRow, 1, name_iter->second);

		// col 2
		m_ctrlSndCh1.SetCellType(nRow, 2, RUNTIME_CLASS(CGridCellCombo));
		if (nRow<225 && InitValueGet()[nRow] == 1)
		{
			m_ctrlSndCh1.SetItemText(nRow, 2, "吸起");
			TriggerXORCondition(nRow, "落下");
			TriggerADDCondition(nRow, "吸起");
		}
		else
		{
			m_ctrlSndCh1.SetItemText(nRow, 2, "落下");
			TriggerXORCondition(nRow, "吸起");
			TriggerADDCondition(nRow, "落下");
		}

		CStringArray options;
		options.Add("落下");
		options.Add("吸起");

		CGridCellCombo* pCell = (CGridCellCombo*)m_ctrlSndCh1.GetCell(nRow, 2);
		pCell->SetOptions(options);
		pCell->SetStyle(CBS_DROPDOWNLIST);
		//m_ctrlSndCh1.SetItemText(nRow, 1, "0");

		//CGridCell* pCell = dynamic_cast<CGridCell*>(m_ctrlSndCh1.GetCell(nRow, 1));
		//ASSERT(pCell);
		//pCell->SetLimitText(1);
		//pCell->SetCharFilterFn([](UINT nChar)->BOOL { return nChar == '0' || nChar == '1'; });
	}

	m_ctrlSndCh1.AutoSizeColumns();
	m_ctrlSndCh1.AutoSizeRows();
}

void CFSFB2SimView::InitSndCh2()
{
	m_ctrlSndCh2.SetFixedRowCount(1);
	m_ctrlSndCh2.SetFixedColumnCount(1);
	m_ctrlSndCh2.SetRowCount(1 + g_nNumcdw);
	m_ctrlSndCh2.SetColumnCount(1 + 1);

	// fixed row
	m_ctrlSndCh2.SetItemText(0, 1, "bool");

	for (int nRow = 1; nRow < m_ctrlSndCh2.GetRowCount(); nRow++)
	{
		m_ctrlSndCh2.SetItemText(nRow, 0, I2STR(nRow));
		m_ctrlSndCh2.SetItemText(nRow, 1, "0");

		CGridCell* pCell = dynamic_cast<CGridCell*>(m_ctrlSndCh2.GetCell(nRow, 1));
		ASSERT(pCell);
		pCell->SetLimitText(1);
		pCell->SetCharFilterFn([](UINT nChar)->BOOL { return nChar == '0' || nChar == '1'; });
	}

	m_ctrlSndCh2.AutoSizeColumns();
	m_ctrlSndCh2.AutoSizeRows();
}

void CFSFB2SimView::InitBugInsert()
{
	m_ctrlBugInsert.SetTextColor(RGB(255, 0, 0));

	m_ctrlBugInsert.SetFixedRowCount(1);
	m_ctrlBugInsert.SetFixedColumnCount(1);
	m_ctrlBugInsert.SetRowCount(1 + 10);
	m_ctrlBugInsert.SetColumnCount(1 + 1);

	m_ctrlErrorCycle.SetTextColor(RGB(255, 0, 0));

	m_ctrlErrorCycle.SetFixedRowCount(0);
	m_ctrlErrorCycle.SetFixedColumnCount(1);
	m_ctrlErrorCycle.SetRowCount(1+1 );
	m_ctrlErrorCycle.SetColumnCount(1 + 1);
	
	//Error main cycle
	m_ctrlErrorCycle.SetItemText(1, 0, "周期数");
    m_ctrlErrorCycle.SetItemText(0, 1, "注入故障周期数");
	m_ctrlErrorCycle.SetItemText(1, 1, "0");
	SetCellOffsetType(m_ctrlErrorCycle, 1, 1);

	m_ctrlErrorCycle.AutoSizeColumns();
	m_ctrlErrorCycle.AutoSizeRows();
	// fixed row
	m_ctrlBugInsert.SetItemText(0, 1, "应用层故障注入");
	
	for (int nRow = 1; nRow < m_ctrlBugInsert.GetRowCount(); nRow++)
	{
		m_ctrlBugInsert.SetItemText(nRow, 1, "0");

		switch (nRow)
		{
		case 1:
			m_ctrlBugInsert.SetItemText(nRow, 0, "CRCM1");
			SetCellOffsetType(m_ctrlBugInsert, nRow, 1);
			break;
		case 2:
			m_ctrlBugInsert.SetItemText(nRow, 0, "CRCM2");
			SetCellOffsetType(m_ctrlBugInsert, nRow, 1);
			break;
		case 3:
			m_ctrlBugInsert.SetItemText(nRow, 0, "长度");
			SetCellOffsetType(m_ctrlBugInsert, nRow, 1);
			break;
		case 4:
			m_ctrlBugInsert.SetItemText(nRow, 0, "序号");
			SetCellOffsetType(m_ctrlBugInsert, nRow, 1);
			break;
		case 5:
			m_ctrlBugInsert.SetItemText(nRow, 0, "Src-ID");
			SetCellOffsetType(m_ctrlBugInsert, nRow, 1);
			break;
		case 6:
			m_ctrlBugInsert.SetItemText(nRow, 0, "Dest-ID");
			SetCellOffsetType(m_ctrlBugInsert, nRow, 1);
			break;
		case 7:
			m_ctrlBugInsert.SetItemText(nRow, 0, "BSD延迟");
			SetCellOffsetType(m_ctrlBugInsert, nRow, 1);
			break;
		case 8:
			m_ctrlBugInsert.SetItemText(nRow, 0, "数据丢失");
			SetCellOffsetType(m_ctrlBugInsert, nRow, 1);
			break;
		case 9:
			m_ctrlBugInsert.SetItemText(nRow, 0, "CRC16");
			SetCellOffsetType(m_ctrlBugInsert, nRow, 1);
			break;
		case 10:
			m_ctrlBugInsert.SetItemText(nRow, 0, "重复故障");
			SetCellOffsetType(m_ctrlBugInsert, nRow, 1);
			break;
/*		case 9:
			m_ctrlBugInsert.SetItemText(nRow, 0, "不一致");

			m_ctrlBugInsert.SetCellType(nRow, 1, RUNTIME_CLASS(CGridCellCombo));

			CStringArray options;
			options.Add("0");
			options.Add("1");

			CGridCellCombo* pCell = (CGridCellCombo*)m_ctrlBugInsert.GetCell(nRow, 1);
			pCell->SetOptions(options);
			pCell->SetStyle(CBS_DROPDOWNLIST);
			break;*/
		}
	}
	/*m_ctrlBugInsert.SetTextColor(RGB(0, 0, 0));
	m_ctrlBugInsert.SetItemText(10, 1, "冗余层故障注入");
	for (int nRow = 11; nRow < m_ctrlBugInsert.GetRowCount(); nRow++)
	{
		m_ctrlBugInsert.SetItemText(nRow, 1, "0");

		switch (nRow)
		{
		case 11:
			m_ctrlBugInsert.SetItemText(nRow, 0, "RMSMsg.size");
			SetCellOffsetType(m_ctrlBugInsert, nRow, 1);
			break;
		case 12:
			m_ctrlBugInsert.SetItemText(nRow, 0, "RMSMsg.version");
			SetCellOffsetType(m_ctrlBugInsert, nRow, 1);
			break;
		case 13:
			m_ctrlBugInsert.SetItemText(nRow, 0, "RMSMsg.srcSS");
			SetCellOffsetType(m_ctrlBugInsert, nRow, 1);
			break;
		case 14:
			m_ctrlBugInsert.SetItemText(nRow, 0, "RMSMsg.destSS");
			SetCellOffsetType(m_ctrlBugInsert, nRow, 1);
			break;
		case 15:
			m_ctrlBugInsert.SetItemText(nRow, 0, "RMSMsg.SN");
			SetCellOffsetType(m_ctrlBugInsert, nRow, 1);
			break;

		case 16:
			m_ctrlBugInsert.SetItemText(nRow, 0, "SSE延迟");
			SetCellOffsetType(m_ctrlBugInsert, nRow, 1);
			break;

		case 17:
			m_ctrlBugInsert.SetItemText(nRow, 0, "SSR延迟");
			SetCellOffsetType(m_ctrlBugInsert, nRow, 1);
			break;
		
		}
	}*/
	m_ctrlBugInsert.AutoSizeColumns();
	m_ctrlBugInsert.AutoSizeRows();
}

void CFSFB2SimView::InitRelay()
{
	CString strPath;
	GetModuleFilePath(strPath);
	CStdioFile cfgFile;
	if (!cfgFile.Open(strPath + "\\rssp1.vsl", CFile::modeRead))
	{
		LOG(("读取配置文件rssp1.vsl失败"));
		return;
	}
	BOOL bOther = FALSE;
	CString oneLine;
	m_relayName.clear();
	int next = cfgFile.ReadString(oneLine);
	while (next)
	{
		if (oneLine.Find("SOURCE") != -1 && oneLine.Find("OTHER") != -1)
			bOther = TRUE;

		if (bOther)
		{
			int pos = oneLine.Find("=");
			if (pos != -1)
			{
				CString strIndex = oneLine.Left(pos);
				int index = atoi(strIndex);
				if (index)
				{
					CString strName = oneLine.Right(oneLine.GetLength()-pos-1);
					strName.TrimLeft();
					strName.TrimRight();
					m_relayName.insert(std::pair<int, CString>(index, strName));
				}
			}
		}
		next = cfgFile.ReadString(oneLine);
	}
}

void CFSFB2SimView::ParseADDCondition(CString strCondition)
{
	int _pos = 0;
	int _rID = 0;
	int _count = 0;
	RELAY_ADD ADDRelay = {0};
	ADDRelay.size = strCondition.Replace(",", ",")+1;
	
	do 
	{
		_pos = strCondition.Find(",");
		if (_pos != -1)
		{
			CString strLeft = strCondition.Left(_pos);
			strLeft.Trim();
			_rID = atoi(strLeft.Right(strLeft.GetLength()-1));
			ADDRelay.arr[_count] = _rID;
			_count++;
			strCondition = strCondition.Right(strCondition.GetLength()-_pos-1);
		}
		else
		{
			strCondition.Trim();
			_rID = atoi(strCondition.Right(strCondition.GetLength()-1));
			ADDRelay.arr[_count] = _rID;
			_count++;
		}
	}while (_pos != -1);

	m_relayAdd.push_back(ADDRelay);
}

void CFSFB2SimView::ParseXORCondition(CString strCondition)
{
	RELAY_XOR XORRelay = {0};
	int numSeperator = strCondition.Replace(",", ",");
	if (numSeperator != 1)
		LOG(("Parse ADD condition error: %s", strCondition));

	int pos = strCondition.Find(",");
	if (pos != -1)
	{
		CString strLeft = strCondition.Left(pos);
		strLeft.Trim();
		XORRelay.r1 = atoi(strLeft.Right(strLeft.GetLength()-1));

		CString strRight = strCondition.Right(strCondition.GetLength()-pos-1);
		strRight.Trim();
		XORRelay.r2 = atoi(strRight.Right(strRight.GetLength()-1));
	};

	m_relayXor.push_back(XORRelay);
}

void CFSFB2SimView::AddCondition(int cond, CString strCondition)
{
	switch (cond)
	{
	case COND_ADD:
		ParseADDCondition(strCondition);
		break;

	case COND_XOR:
		ParseXORCondition(strCondition);
		break;

	case COND_OTHER:
	default:
		break;
	}
}

void CFSFB2SimView::InitCondition()
{
	CString strPath;
	GetModuleFilePath(strPath);
	CStdioFile cfgFile;
	if (!cfgFile.Open(strPath + "\\condition.ini", CFile::modeRead))
	{
		//LOG(("读取配置文件condition.ini失败"));
		return;
	}

	CString oneLine = "";
	int cond = COND_OTHER;
	m_relayAdd.clear();
	m_relayXor.clear();
	int next = cfgFile.ReadString(oneLine);
	while (next)
	{
		if (oneLine.Find("[ADD]") != -1)
			cond = COND_ADD;

		if (oneLine.Find("[XOR]") != -1)
			cond = COND_XOR;

		int pos = oneLine.Find("=");
		if (pos != -1)
		{
			CString strIndex = oneLine.Left(pos);
			int index = atoi(strIndex);
			if (index)
			{
				CString strCondition = oneLine.Right(oneLine.GetLength()-pos-1);
				AddCondition(cond, strCondition);
			}
		}
		next = cfgFile.ReadString(oneLine);
	}

	LOG(("[Init Condition] XOR:%d, ADD:%d", m_relayXor.size(), m_relayAdd.size()));
}

void CFSFB2SimView::SetCellOffsetType(CGridCtrl& grid, int nRow, int nCol)
{
	grid.SetItemBkColour(nRow, nCol, RGB(255, 255, 128));
	grid.SetCellType(nRow, nCol, RUNTIME_CLASS(CGridCell));
	CGridCell* pCell = dynamic_cast<CGridCell*>(grid.GetCell(nRow, nCol));
	ASSERT(pCell);
	pCell->SetCharFilterFn([](UINT nChar){ return nChar == '-' || nChar >= '0' && nChar <= '9'; });
}

void CFSFB2SimView::OnGridEndEdit(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	CString strItem;
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	if (pItem->hdr.hwndFrom == m_ctrlBugInsert.m_hWnd)
	{
		strItem = m_ctrlBugInsert.GetItemText(pItem->iRow, pItem->iColumn);
		if ( strItem.Find('-') > 0 || strItem == "-")
		{
			LOG(("输入的负数格式不正确，重置为0！"));
			m_ctrlBugInsert.SetItemText(pItem->iRow, pItem->iColumn, "0");
		}
	}
}

void CFSFB2SimView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CFSFB2SimView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CFSFB2SimView 诊断

#ifdef _DEBUG
void CFSFB2SimView::AssertValid() const
{
	CFormView::AssertValid();
}

void CFSFB2SimView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CFSFB2SimDoc* CFSFB2SimView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFSFB2SimDoc)));
	return (CFSFB2SimDoc*)m_pDocument;
}
#endif //_DEBUG


// CFSFB2SimView 消息处理程序
LRESULT CFSFB2SimView::OnRcvMsg(WPARAM wParam, LPARAM lParam)
{
	char* pMsg = (char*)wParam;
	ASSERT(pMsg);

	//if (memcmp(pMsg, &m_rcvMsg, MAX_RSSP1_MSG_LEN) != 0)
	{
		memcpy(&m_rcvMsg, pMsg, MAX_RSSP1_MSG_LEN);
		UpdateRcvCtrls();
	}

	return 0;
}

void CFSFB2SimView::UpdateRcvCtrls()
{
	UpdateRcvCh1();
	//UpdateRcvCh2();
}

void CFSFB2SimView::UpdateRcvCh1()
{
	/*for(int i= 0; i<(MAX_RSSP1_MSG_LEN-6);i++)
		{
	   m_rcvMsg[i] = ((m_rcvMsg[i] & 0x01) << 7) | ((m_rcvMsg[i] & 0x02) << 5)| ((m_rcvMsg[i] & 0x04) << 3)| ((m_rcvMsg[i] & 0x08) << 1) | ((m_rcvMsg[i] & 0x10) >> 1) | ((m_rcvMsg[i] & 0x20) >> 3)| ((m_rcvMsg[i] & 0x40) >> 5)| ((m_rcvMsg[i] & 0x80) >> 7);
		}*/
	for (int nRow = 1; nRow < m_ctrlRcvCh1.GetRowCount(); nRow++)
	{
		if (m_rcvMsg[(nRow-1) / 8] & (0x01<<((nRow - 1)%8)))
		{
			m_ctrlRcvCh1.SetItemText(nRow, 1, "1"); 
		}
		else
		{
			m_ctrlRcvCh1.SetItemText(nRow, 1, "0");
		}
		m_ctrlRcvCh1.Refresh();
	}
}

void CFSFB2SimView::UpdateRcvCh2()
{
}

LRESULT CFSFB2SimView::OnRefreshSendBuff(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	CString strTxCdw = "";
	GetDlgItemText(IDC_EDIT_TX_CDW, strTxCdw);
	g_sendMsg.count = atoi(strTxCdw.GetBuffer());
	g_sendMsg.count=m_SendLength;
	strTxCdw.ReleaseBuffer();
	FillSendMsg();
	return 0;
}

void CFSFB2SimView::FillSendMsg()
{
	memset(g_sendMsg.b_TX_1, 0, g_nNumcdw);
	memset(g_sendMsg.b_TX_2, 0, g_nNumcdw);
	for (int nRow = 1; nRow < m_ctrlSndCh1.GetRowCount(); nRow++)
	{
		CString strRelay = m_ctrlSndCh1.GetItemText(nRow, 2);
		if ("吸起" == strRelay)
		{
			g_sendMsg.b_TX_1[nRow - 1] = 1;
			g_sendMsg.b_TX_2[nRow - 1] = 1;
		}
		else
		{
			g_sendMsg.b_TX_1[nRow - 1] = 0;
			g_sendMsg.b_TX_2[nRow - 1] = 0;
		}
	}

	FillBugInsert();
}

void CFSFB2SimView::FillBugInsert()
{
	g_bugInsert.m_Cycle_Error_No= atoi(m_ctrlErrorCycle.GetItemText(1, 1));
	/*if (g_bugInsert.m_start && g_bugInsert.m_Cycle_Error_No !=01)*/
	if(1)
	{
		g_bugInsert.m_nCrc1Offset	= atoi(m_ctrlBugInsert.GetItemText(1, 1));
		g_bugInsert.m_nCrc2Offset	= atoi(m_ctrlBugInsert.GetItemText(2, 1));
		g_bugInsert.m_nLenOffset	= atoi(m_ctrlBugInsert.GetItemText(3, 1));
		g_bugInsert.m_nSNOffset		= atoi(m_ctrlBugInsert.GetItemText(4, 1));
		g_bugInsert.m_nSrcIDOffset	= atoi(m_ctrlBugInsert.GetItemText(5, 1));
		g_bugInsert.m_nDestIDOffset	= atoi(m_ctrlBugInsert.GetItemText(6, 1));
		g_bugInsert.m_nDelayBSDOffset	= atoi(m_ctrlBugInsert.GetItemText(7, 1));
		g_bugInsert.m_nSYSCKW		= atoi(m_ctrlBugInsert.GetItemText(8, 1));
		g_bugInsert.m_nCrc16Offset  = atoi(m_ctrlBugInsert.GetItemText(9, 1));
		g_bugInsert.m_RMSMsgsize  = atoi(m_ctrlBugInsert.GetItemText(10, 1));
		/*
		g_bugInsert.m_RMSMsgsize  = atoi(m_ctrlBugInsert.GetItemText(11, 1));
		g_bugInsert.m_RMSMsgversion  = atoi(m_ctrlBugInsert.GetItemText(12, 1));
		g_bugInsert.m_RMSMsgsrcSS  = atoi(m_ctrlBugInsert.GetItemText(13, 1));
		g_bugInsert.m_RMSMsgdestSS  = atoi(m_ctrlBugInsert.GetItemText(14, 1));
		g_bugInsert.m_RMSMsgSN  = atoi(m_ctrlBugInsert.GetItemText(15, 1));
		g_bugInsert.m_nDelaySSEOffset  = atoi(m_ctrlBugInsert.GetItemText(16, 1));
		g_bugInsert.m_nDelaySSROffset  = atoi(m_ctrlBugInsert.GetItemText(17, 1));*/
		m_ctrlErrorCycle.SetItemText(1, 1, I2STR(max(0, g_bugInsert.m_Cycle_Error_No - 1)));
	} 
	else
	{
		g_bugInsert.m_nCrc1Offset	= 0;
		g_bugInsert.m_nCrc2Offset	= 0;
		g_bugInsert.m_nLenOffset	= 0;
		g_bugInsert.m_nSNOffset		= 0;
		g_bugInsert.m_nSrcIDOffset	= 0;
		g_bugInsert.m_nDestIDOffset	= 0;
		g_bugInsert.m_nDelayBSDOffset = 0;
		g_bugInsert.m_nSYSCKW		= 0;
		g_bugInsert.m_nCrc16Offset  = 0;
		g_bugInsert.m_RMSMsgsize  = 0;
		g_bugInsert.m_RMSMsgversion  = 0;
		g_bugInsert.m_RMSMsgsrcSS  = 0;
		g_bugInsert.m_RMSMsgdestSS  = 0;
		g_bugInsert.m_RMSMsgSN  = 0;
		g_bugInsert.m_nDelaySSEOffset = 0;
		g_bugInsert.m_nDelaySSROffset = 0;
	}
	m_ctrlErrorCycle.Refresh();
}

void CFSFB2SimView::OnDebugBugInsert()
{
	switch (m_bBugInsert)
	{
	case FALSE:
		m_bBugInsert = TRUE;
		m_ctrlBugInsert.ShowWindow(SW_SHOW);
		break;
	case TRUE:
		m_bBugInsert = FALSE;
		m_ctrlBugInsert.ShowWindow(SW_HIDE);
		break;
	default:
		ASSERT(0);
		break;
	}
}

void CFSFB2SimView::OnUpdateDebugBugInsert(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bBugInsert);
}


void CFSFB2SimView::OnDebugShowRecv()
{
	// TODO: Add your command handler code here
	/*switch (m_bShowRecv)
	{
	case FALSE:
		m_bShowRecv = TRUE;
		m_ctrlRcvCh1.ShowWindow(SW_SHOW);
		m_ctrlRcvCh2.ShowWindow(SW_SHOW);
		break;
	case TRUE:
		m_bShowRecv = FALSE;
		m_ctrlRcvCh1.ShowWindow(SW_HIDE);
		m_ctrlRcvCh2.ShowWindow(SW_HIDE);
		break;
	default:
		ASSERT(0);
		break;
	}*/
}


void CFSFB2SimView::OnUpdateDebugShowRecv(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_bShowRecv);
}


void CFSFB2SimView::OnHelpText()
{
	// TODO: Add your command handler code here
	WinExec("notepad readme.txt", SW_SHOW);
}

void CFSFB2SimView::OnBnClickedButtonStart()
{
	// TODO: 在此添加控件通知处理程序代
	//SetTimer(1,5000,NULL);
	g_bugInsert.m_start = TRUE;
}


void CFSFB2SimView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int i=(int)rand()%2;
	UpdateChannel(i);
	CFormView::OnTimer(nIDEvent);
}


void CFSFB2SimView::UpdateChannel(int flag)
{
	int i=0;
	switch(flag)
	{
	case 0:
		for (i=1;i<m_ctrlSndCh1.GetRowCount();i++)
		{
			m_ctrlSndCh1.SetItemText(i, 2, "落下");
			m_ctrlSndCh1.Refresh();
		}
		break;
	case 1:
		for (i=1;i<m_ctrlSndCh1.GetRowCount();i++)
		{
			m_ctrlSndCh1.SetItemText(i, 2, "吸起");
			m_ctrlSndCh1.Refresh();
		}
		break;	
	default:
		break;
	}
}


void CFSFB2SimView::OnBnClickedButtonStop()
{
	// TODO: 在此添加控件通知处理程序代码
	g_bugInsert.m_start = FALSE;
}

RELAY_XOR* CFSFB2SimView::GetXORRelay(int iRelay)
{
	std::vector<RELAY_XOR>::iterator xor_iter = m_relayXor.begin();
	while(xor_iter != m_relayXor.end())
	{
		if (xor_iter->r1 == iRelay || xor_iter->r2 == iRelay)
			return &(*xor_iter);
		xor_iter++;
	}

	return NULL;
}

RELAY_ADD* CFSFB2SimView::GetADDRelay(int iRelay)
{
	std::vector<RELAY_ADD>::iterator add_iter = m_relayAdd.begin();
	while(add_iter != m_relayAdd.end())
	{
		int size = add_iter->size;
		for (int i=0; i<size; i++)
		{
			int rID = add_iter->arr[i];
			if (rID == iRelay)
			{
				return &(*add_iter);
			}
		}
		add_iter++;
	}

	return NULL;
}

void CFSFB2SimView::TriggerADDCondition(int iRelay, CString strStatus)
{
	RELAY_ADD* pRelayADD = GetADDRelay(iRelay);
	if (NULL == pRelayADD)
		return;

	for (int i=0; i<pRelayADD->size; i++)
	{
		if (pRelayADD->arr[i] != iRelay)
		{
			m_ctrlSndCh1.SetItemText(pRelayADD->arr[i], 2, strStatus);
		}
	}
	m_ctrlSndCh1.Refresh();
}

void CFSFB2SimView::TriggerXORCondition(int iRelay, CString strStatus)
{
	RELAY_XOR* pRelayXOR = GetXORRelay(iRelay);
	if (NULL == pRelayXOR)
		return;

	if (pRelayXOR->r1 == iRelay)
		m_ctrlSndCh1.SetItemText(pRelayXOR->r2, 2, strStatus);
	else
		m_ctrlSndCh1.SetItemText(pRelayXOR->r1, 2, strStatus);
	m_ctrlSndCh1.Refresh();
}

void CFSFB2SimView::OnSendGridChangedUp(NMHDR* pNMHDR, LRESULT* pResult)
{
	GV_DISPINFO *pgvDispInfo = (GV_DISPINFO *)pNMHDR;  
	GV_ITEM *pgvItem = &pgvDispInfo->item;  

	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNMHDR;  
	CGridCtrl *pGrid = &m_ctrlSndCh1;  
	if (pGrid == NULL || pGrid->GetSafeHwnd() == NULL) 
		return;  
	CGridCell *pCell = (CGridCell*)pGrid->GetCell(pItem->iRow, pItem->iColumn);  
	if (pCell == NULL) 
		return;  

	if (pCell != NULL && pCell->IsKindOf(RUNTIME_CLASS(CGridCellCombo))) 
	{  
		TriggerXORCondition(pItem->iRow, "落下");
		TriggerADDCondition(pItem->iRow, "吸起");
	}

	pGrid->Refresh();
}

void CFSFB2SimView::OnSendGridChangedDown(NMHDR* pNMHDR, LRESULT* pResult)
{
	GV_DISPINFO *pgvDispInfo = (GV_DISPINFO *)pNMHDR;  
	GV_ITEM *pgvItem = &pgvDispInfo->item;  

	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNMHDR;  
	CGridCtrl *pGrid = &m_ctrlSndCh1;  
	if (pGrid == NULL || pGrid->GetSafeHwnd() == NULL) 
		return;  
	CGridCell *pCell = (CGridCell*)pGrid->GetCell(pItem->iRow, pItem->iColumn);  
	if (pCell == NULL) 
		return;  

	if (pCell != NULL && pCell->IsKindOf(RUNTIME_CLASS(CGridCellCombo))) 
	{  
		TriggerXORCondition(pItem->iRow, "吸起");
		TriggerADDCondition(pItem->iRow, "落下");
	}

	pGrid->Refresh();
}

void CFSFB2SimView::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	SetTimer(1,5000,NULL);
}


void CFSFB2SimView::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
    KillTimer(1);
}


void CFSFB2SimView::OnDebug32777()
{
	SetTimer(1,5000,NULL);
	g_LongtimeRun = TRUE;
}
