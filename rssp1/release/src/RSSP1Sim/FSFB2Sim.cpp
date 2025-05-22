
// FSFB2Sim.cpp : 定义应用程序的类行为。
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
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()


// CFSFB2SimApp 构造

CFSFB2SimApp::CFSFB2SimApp()
{
	m_bHiColorIcons = TRUE;

	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则:
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("FSFB2Sim.AppID.NoVersion"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CFSFB2SimApp 对象

CFSFB2SimApp theApp;


// CFSFB2SimApp 初始化

BOOL CFSFB2SimApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	LoadTBVersion();

	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// 使用 RichEdit 控件需要  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(0);  // 加载标准 INI 文件选项(包括 MRU)

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CFSFB2SimDoc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(CFSFB2SimView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// 调度在命令行中指定的命令。如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	m_pMainWnd->SetWindowText(m_strTBVersion);
	m_pMainWnd->ShowWindow(SW_SHOW);

/*#ifdef _DEBUG
	m_pMainWnd->SetWindowText("FSFB2模拟程序，v1.0 debug版");
#else
	m_pMainWnd->SetWindowText("FSFB2模拟程序，v1.0 release版");
#endif*/

	m_pMainWnd->UpdateWindow();
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 SDI 应用程序中，这应在 ProcessShellCommand 之后发生
	return TRUE;
}

int CFSFB2SimApp::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CFSFB2SimApp 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

public:
	BOOL OnInitDialog();

	CString m_strVersion;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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

// 用于运行对话框的应用程序命令
void CFSFB2SimApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.m_strVersion = m_strTBVersion;
	aboutDlg.DoModal();
}

// CFSFB2SimApp 自定义加载/保存方法

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
		AfxMessageBox("没找到Version.ini文件！");
	}
}

// CFSFB2SimApp 消息处理程序





void CAboutDlg::OnStnClickedStaticVersion()
{
	// TODO: Add your control notification handler code here
}
