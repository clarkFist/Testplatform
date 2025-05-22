
// FSFB2Sim.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "FSFB2Sim.h"
#include "MainFrm.h"

#include "FSFB2SimDoc.h"
#include "FSFB2SimView.h"
#include "GlueData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

char InitValue[225] = {0};
void InitValueSet(CString strVer)
{
	char temp;
	char tempStr[500];
	strncpy(tempStr, (LPCTSTR)strVer, strVer.GetLength());
	int size = strVer.GetLength() > 56 ? 56:strVer.GetLength();
	for (int i = 0;i < size ;i++)
	{
		if (tempStr[i] >= 'A' && tempStr[i] <= 'Z')
		{
			temp = 10 + tempStr[i] - 'A';
		}
		else if(tempStr[i] >= '0' && tempStr[i] <= '9')
		{
			temp = tempStr[i] - '0';
		}
		else
		{
			AfxMessageBox(strVer);
			break;
		}
		for (int j = 0;j < 4;j++)
		{
			InitValue[i * 4 + j + 1] = (temp & 0x08) >> 3;
			temp = temp<<1;
		}
	}
}
char* InitValueGet()
{
	return InitValue;
}

// CFSFB2SimApp

BEGIN_MESSAGE_MAP(CFSFB2SimApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CFSFB2SimApp::OnAppAbout)
	// �����ļ��ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()


// CFSFB2SimApp ����

CFSFB2SimApp::CFSFB2SimApp()
{
	m_bHiColorIcons = TRUE;

	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// ���Ӧ�ó��������ù�����������ʱ֧��(/clr)�����ģ���:
	//     1) �����д˸������ã�������������������֧�ֲ�������������
	//     2) ��������Ŀ�У������밴������˳���� System.Windows.Forms ������á�
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: ������Ӧ�ó��� ID �ַ����滻ΪΨһ�� ID �ַ�����������ַ�����ʽ
	//Ϊ CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("FSFB2Sim.AppID.NoVersion"));

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

// Ψһ��һ�� CFSFB2SimApp ����

CFSFB2SimApp theApp;


// CFSFB2SimApp ��ʼ��

BOOL CFSFB2SimApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	LoadTBVersion();

	// ��ʼ�� OLE ��
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// ʹ�� RichEdit �ؼ���Ҫ  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	LoadStdProfileSettings(0);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// ע��Ӧ�ó�����ĵ�ģ�塣�ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CFSFB2SimDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ��ܴ���
		RUNTIME_CLASS(CFSFB2SimView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// ������׼ shell ���DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// ��������������ָ����������
	// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
	m_pMainWnd->SetWindowText(m_strTBVersion);
	m_pMainWnd->ShowWindow(SW_SHOW);

/*#ifdef _DEBUG
	m_pMainWnd->SetWindowText("FSFB2ģ�����v1.0 debug��");
#else
	m_pMainWnd->SetWindowText("FSFB2ģ�����v1.0 release��");
#endif*/

	m_pMainWnd->UpdateWindow();
	// �������к�׺ʱ�ŵ��� DragAcceptFiles
	//  �� SDI Ӧ�ó����У���Ӧ�� ProcessShellCommand ֮����
	return TRUE;
}

int CFSFB2SimApp::ExitInstance()
{
	//TODO: �����������ӵĸ�����Դ
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CFSFB2SimApp ��Ϣ�������


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

public:
	BOOL OnInitDialog();

	CString m_strVersion;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedStaticVersion();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetDlgItemText(IDC_STATIC_VERSION, m_strVersion);

	return TRUE;
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_STN_CLICKED(IDC_STATIC_VERSION, &CAboutDlg::OnStnClickedStaticVersion)
END_MESSAGE_MAP()

// �������жԻ����Ӧ�ó�������
void CFSFB2SimApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.m_strVersion = m_strTBVersion;
	aboutDlg.DoModal();
}

// CFSFB2SimApp �Զ������/���淽��

void CFSFB2SimApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CFSFB2SimApp::LoadCustomState()
{
}

void CFSFB2SimApp::SaveCustomState()
{
}

void CFSFB2SimApp::LoadTBVersion()
{
	CString strVer;
	CString strPath;
	CStdioFile verFile;
	CString initValuePath;
	CString appPath;
	m_strTBVersion = "";
	GetModuleFileName(NULL, appPath.GetBuffer(MAX_PATH), MAX_PATH);      
	appPath.ReleaseBuffer(); 
	int nPos = appPath.ReverseFind('\\');   
	appPath = appPath.Left(nPos+1);
	strPath = appPath + "version.ini";
	initValuePath = appPath + "..\\initValue.ini";
	if (verFile.Open(strPath, CFile::modeRead))
	{
		int iRead = verFile.ReadString(strVer);
		while (iRead)
		{
			strVer.MakeUpper();
			strVer.Replace(" ", "");
			if (strVer.Replace("VERSION=", ""))
			{
				m_strTBVersion = strVer;
			}
			else if (strVer.Replace("SENDLENGTH=", ""))
			{
				m_SendLength = atoi(strVer);
				g_nNumcdw = m_SendLength;
				g_nNumcdwRecv = m_SendLength;
				g_nNumcdwSend = m_SendLength
					;
			}
			else
			{
				m_SendLength = 1344;
			}
			iRead = verFile.ReadString(strVer);
		}
	}
	verFile.Close();
	if (verFile.Open(initValuePath, CFile::modeRead))
	{
		int iRead = verFile.ReadString(strVer);
		while (iRead)
		{
			strVer.MakeUpper();
			strVer.Replace(" ", "");
			if (strVer.Replace(m_strTBVersion +":", ""))
			{
				InitValueSet(strVer);
				break;
			}
			iRead = verFile.ReadString(strVer);
		}
		verFile.Close();
	}
	if (m_strTBVersion == "")
	{
		AfxMessageBox("û�ҵ�Version.ini�ļ���");
	}
}

// CFSFB2SimApp ��Ϣ�������





void CAboutDlg::OnStnClickedStaticVersion()
{
	// TODO: Add your control notification handler code here
}
