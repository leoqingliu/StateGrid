// ResEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ResEdit.h"
#include "ResEditDlg.h"

#include <Shlwapi.h>
#pragma comment(lib, "ShlWapi.lib")
#include <atlconv.h>
#include "../Defines.h"
#include "../Public/crypt.h"
#include "../Public/FileEx.h"
using namespace NGlobalUtils;

#define DEFAULT_PORT 80

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResEditDlg dialog

CResEditDlg::CResEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CResEditDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CResEditDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//
	GetModuleFileName(NULL, m_szFile, MAX_PATH);
	*StrRChr(m_szFile, m_szFile + lstrlen(m_szFile), _T('\\')) = 0;
	lstrcat(m_szFile, _T("\\Config.ini"));
	
	//
	//memset(&m_ConnectInfo, 0, sizeof(CConnectInfo));
}

void CResEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResEditDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CResEditDlg, CDialog)
	//{{AFX_MSG_MAP(CResEditDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_OK, OnBtnOk)
	ON_BN_CLICKED(IDC_RADIO_PORT_DEFAULT, OnRadioPortDefault)
	ON_BN_CLICKED(IDC_RADIO_PORT_USER, OnRadioPortUser)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResEditDlg message handlers

BOOL CResEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	/* 
	IN_ADDR AddrIn;
	ULONG uAddr;
	AddrIn.s_addr	= inet_addr("127.0.0.1");
	uAddr			= AddrIn.s_addr;	//ntohl(AddrIn.s_addr);
	this->m_IPCtrl.SetAddress(uAddr);
	*/

	//
	if (ReadConfig() == FALSE)
	{
		AfxMessageBox(_T("ReadConfig Failed"), MB_ICONERROR);
		PostQuitMessage(1);
		return FALSE;
	}

	//
	if(m_ConnectInfo.uPort == DEFAULT_PORT)
	{
		OnRadioPortDefault();
	}
	else
	{
		SetDlgItemInt(IDC_EDIT_PORT, m_ConnectInfo.uPort);
		OnRadioPortUser();
	}

	//
//	SetDlgItemInt(IDC_EDIT_ID, m_ConnectInfo.dwId);
	USES_CONVERSION;
	SetDlgItemText(IDC_EDIT_PASSWD, A2T(m_ConnectInfo.szKey));
	SetDlgItemText(IDC_EDIT_PAGE, A2T(m_ConnectInfo.szPage));
	SetDlgItemText(IDC_EDIT_DYNAMIC, A2T(m_ConnectInfo.szHost));
	SetDlgItemText(IDC_EDIT_COMMENT, A2T(m_ConnectInfo.szComment));
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CResEditDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CResEditDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CResEditDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


BOOL CResEditDlg::ReadConfig()
{
	CFileEx fileEx;
	CFileEx::CFileStruct fileStruct;
	memset(&fileStruct, 0, sizeof(CFileEx::CFileStruct));
	if (fileEx.OpenFile(m_szFile, &fileStruct, TRUE) == FALSE)
	{
		if (fileEx.OpenFile(m_szFile, &fileStruct, TRUE, TRUE) == FALSE)
		{
			return FALSE;
		}

		if (FALSE == WriteConfig())
		{
			return FALSE;
		}

		return TRUE;
	}
	else
	{
		DWORD dwRead = 0;
		LPBYTE pBuf = new BYTE[sizeof(CommunicationInfo)];
		BOOL bOK = ReadFile(fileStruct.hFile, pBuf, sizeof(CommunicationInfo), &dwRead, NULL);
		Crypt::_rc4Full(CONFIG_KEY, strlen(CONFIG_KEY), pBuf, sizeof(CommunicationInfo));
		memcpy(&m_ConnectInfo, pBuf, sizeof(CommunicationInfo));
		delete[] pBuf;
		return sizeof(CommunicationInfo) == dwRead;
	}
	
	/*
	DWORD dwSize = m_File.GetFileSize(m_FileStruct.hFile, NULL);
	if(dwSize == 0 || dwSize != sizeof(CONNECT_INFO))
	{
		DWORD dwWrite = 0;
		BOOL bOK = WriteFile(m_FileStruct.hFile, &m_ConnectInfo, sizeof(m_ConnectInfo), &dwWrite, NULL);
	}
	
	if(m_File.MapFile(&m_FileStruct) == FALSE)
	{
		AfxMessageBox(_T("Map 文件错误"), MB_ICONERROR);
		return FALSE;
	}
	//
	return TRUE;
	*/
}

BOOL CResEditDlg::WriteConfig()
{
	CFileEx fileEx;
	CFileEx::CFileStruct fileStruct;
	memset(&fileStruct, 0, sizeof(CFileEx::CFileStruct));
	if (fileEx.OpenFile(m_szFile, &fileStruct, TRUE, TRUE) == FALSE)
	{
		return FALSE;
	}
	DWORD dwWrite = 0;
	SetFilePointer(fileStruct.hFile, 0, 0, FILE_BEGIN);
	LPBYTE pBuf = new BYTE[sizeof(CommunicationInfo)];
	memcpy(pBuf, &m_ConnectInfo, sizeof(CommunicationInfo));
	Crypt::_rc4Full(CONFIG_KEY, strlen(CONFIG_KEY), pBuf, sizeof(CommunicationInfo));
	BOOL bOK = WriteFile(fileStruct.hFile, pBuf, sizeof(m_ConnectInfo), &dwWrite, NULL);
	delete[] pBuf;
	return bOK;
}

void CResEditDlg::OnRadioPortDefault() 
{
	CheckDlgButton(IDC_RADIO_PORT_DEFAULT, BST_CHECKED);
	GetDlgItem(IDC_EDIT_PORT)->EnableWindow(FALSE);
}

void CResEditDlg::OnRadioPortUser() 
{
	CheckDlgButton(IDC_RADIO_PORT_USER, BST_CHECKED);
	GetDlgItem(IDC_EDIT_PORT)->EnableWindow(TRUE);
}

void CResEditDlg::OnBtnOk() 
{
	// Dynamic
	BOOL bTrans = FALSE;
	TCHAR szBuffer[128] = {0};
	::GetDlgItemText(m_hWnd, IDC_EDIT_DYNAMIC, szBuffer, 128);
	if(lstrlen(szBuffer) == 0)
	{
		AfxMessageBox(_T("请输入动态域名!"), MB_ICONERROR);
		return;
	}
	USES_CONVERSION;
	strcpy(m_ConnectInfo.szHost, T2A(szBuffer));
	
	// Page
	::GetDlgItemText(m_hWnd, IDC_EDIT_PAGE, szBuffer, 128);
	if(lstrlen(szBuffer) == 0)
	{
		AfxMessageBox(_T("请输入页面!"), MB_ICONERROR);
		return;
	}
	strcpy(m_ConnectInfo.szPage, T2A(szBuffer));

	// Comment
	::GetDlgItemText(m_hWnd, IDC_EDIT_COMMENT, szBuffer, 128);
	if(lstrlen(szBuffer) == 0)
	{
		AfxMessageBox(_T("请输入备注!"), MB_ICONERROR);
		return;
	}
	strcpy(m_ConnectInfo.szComment, T2A(szBuffer));

	// Port
	if(BST_CHECKED == IsDlgButtonChecked(IDC_RADIO_PORT_DEFAULT))
	{
		m_ConnectInfo.uPort = DEFAULT_PORT;
	}
	else
	{
		m_ConnectInfo.uPort = GetDlgItemInt(IDC_EDIT_PORT, &bTrans, FALSE);
	}

	// Id
	//m_ConnectInfo.dwId = GetDlgItemInt(IDC_EDIT_ID, &bTrans, FALSE);
	m_ConnectInfo.dwId = 0;

	// Password
	szBuffer[0] = 0;
	::GetDlgItemText(m_hWnd, IDC_EDIT_PASSWD, szBuffer, 128);
	if(lstrlen(szBuffer) == 0)
	{
		AfxMessageBox(_T("请输入密码!"), MB_ICONERROR);
		return;
	}
	strcpy(m_ConnectInfo.szKey, T2A(szBuffer));
	
	//
	if(!WriteConfig())
	{
		AfxMessageBox(_T("写文件失败"), MB_ICONERROR);
	}
	else
	{
		AfxMessageBox(_T("写文件成功"), MB_ICONINFORMATION);
	}
	
	//
	//m_File.CloseFile(&m_FileStruct);
	//PostQuitMessage(0);
}
