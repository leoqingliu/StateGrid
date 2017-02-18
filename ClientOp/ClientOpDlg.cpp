
// ClientOpDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ClientOp.h"
#include "ClientOpDlg.h"
#include "../Public/SvchostServiceModule.h"
#include "../Public/Utils.h"
#include "../Defines.h"
#include "ClientFunc.h"
#include "ConnectConfigDlg.h"
#include <TlHelp32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CClientOpDlg dialog



BOOL Extractfile(HMODULE hInstance, LPCTSTR lpType, LPCTSTR lpName, LPCTSTR lpPath)
{
	// Find and Load the resource containing file(s) data
	HRSRC hResLoad = FindResource(hInstance, lpType, lpName);
	HGLOBAL hResData = LoadResource(hInstance, hResLoad);
	LPCSTR data = (LPCSTR)LockResource(hResData);

	// We should have properly loaded the resource
	assert(data != NULL && hResData != NULL);

	DWORD dwFileLen = SizeofResource(hInstance, hResLoad);
	HANDLE hFileHandle = CreateFile(lpPath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	if(hFileHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	SetFilePointer(hFileHandle, 0, 0, FILE_BEGIN);   
	DWORD dwBytesWrite;
	if(!WriteFile(hFileHandle, data, dwFileLen, &dwBytesWrite, 0))
	{
		CloseHandle(hFileHandle);
		return FALSE;
	}
	CloseHandle(hFileHandle);

	// Set Attributes
	//SetFileAttributes(lpPath, FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_HIDDEN);

	// Perform cleanup
	//UnlockResource(hResData);
	FreeResource(hResData);

	return TRUE;
}

BOOL MyFileExists(LPCTSTR lpszFileName)
{
	DWORD dwAttributes = ::GetFileAttributes(lpszFileName);
	return (dwAttributes != ((DWORD)-1)) && ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

BOOL MyDirectoryExists(LPCTSTR lpszDirectoryName)
{
	DWORD dwAttributes = ::GetFileAttributes(lpszDirectoryName);
	return (dwAttributes != ((DWORD)-1)) && ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

CClientOpDlg::CClientOpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClientOpDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	// Start up
	WSADATA		wsd;
	if(WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		return;
	}
}

void CClientOpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CClientOpDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_INSTALL, &CClientOpDlg::OnBnClickedButtonInstall)
	ON_BN_CLICKED(IDC_BUTTON_UNINSTALL, &CClientOpDlg::OnBnClickedButtonUninstall)
	ON_BN_CLICKED(IDC_BUTTON_START, &CClientOpDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CClientOpDlg::OnBnClickedButtonStop)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_GEN, &CClientOpDlg::OnBnClickedButtonGen)
END_MESSAGE_MAP()


// CClientOpDlg message handlers

BOOL CClientOpDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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

	UpdateDisplay();
	SetTimer(1, 1000, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CClientOpDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CClientOpDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CClientOpDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CClientOpDlg::OnBnClickedButtonInstall()
{
	INT iResId = IDR_DLL_SERVICE;
	BOOL bRet = Extractfile(
		AfxGetApp()->m_hInstance,
		MAKEINTRESOURCE(iResId), _T("DLL"), theApp.m_strDllPath
		);
	if (!bRet)
	{
		CString strError;
		strError.Format(_T("释放DLL文件失败(Error: %d)，请检查磁盘空间和杀毒软件相关配置!"), GetLastError());
		SetDlgItemText(IDC_STATIC_TIP, strError);
		return;
	}

	CConnectConfigDlg dlg;
	dlg.m_strTagerPath = theApp.m_strDllPath;
	if (IDOK != dlg.DoModal())
	{
		return;
	}

	bRet = InitClientLibrary(theApp.m_strDllPath);
	if (bRet)
	{
		bRet = _InstallService();
		if (bRet)
		{
			SetDlgItemText(IDC_STATIC_TIP, _T("安装成功"));
		}
		else
		{
			SetDlgItemText(IDC_STATIC_TIP, _T("安装失败!"));
		}

		UnInitClientLibrary();
	}
	else
	{
		SetDlgItemText(IDC_STATIC_TIP, _T("加载DLL文件失败!"));
	}
}

void CClientOpDlg::OnBnClickedButtonUninstall()
{
	if (!MyFileExists(theApp.m_strDllPath))
	{
		BOOL bRet = Extractfile(
			AfxGetApp()->m_hInstance,
			MAKEINTRESOURCE(IDR_DLL_SERVICE), _T("DLL"), theApp.m_strDllPath
			);
		if (!bRet)
		{
			CString strError;
			strError.Format(_T("释放DLL文件失败(Error: %d)，请检查磁盘空间和杀毒软件相关配置!"), GetLastError());
			SetDlgItemText(IDC_STATIC_TIP, strError);
			return;
		}
	}

	BOOL bRet = InitClientLibrary(theApp.m_strDllPath);
	if (bRet)
	{
		bRet = _UnInstallService();
		if (bRet)
		{
			SetDlgItemText(IDC_STATIC_TIP, _T("卸载成功"));
		}
		else
		{
			SetDlgItemText(IDC_STATIC_TIP, _T("卸载失败!"));
		}

		UnInitClientLibrary();

		CSvchostServiceModule::Control(_T("tvnserver"), FALSE);
		CSvchostServiceModule::Uninstall(_T("tvnserver"), _T("MacMan"));

		Sleep(1000);

		const LPCTSTR TASK_EXE = _T("ScreenLocker.exe");
		PROCESSENTRY32 pe32 = {0};
		HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);    
		if (hProcessSnap != (HANDLE)-1)    
		{
			pe32.dwSize = sizeof(PROCESSENTRY32);    
			if (Process32First(hProcessSnap, &pe32))    
			{  
				while(1)  
				{
					if (0 == _tcsicmp(pe32.szExeFile, TASK_EXE))
					{
						HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
						if (hProc != 0)
						{
							TerminateProcess(hProc, -1);
						}
					}

					if (!Process32Next(hProcessSnap, &pe32))
					{
						break;
					}
				}
			}
			CloseHandle(hProcessSnap);
		}
	}
	else
	{
		SetDlgItemText(IDC_STATIC_TIP, _T("加载DLL文件失败!"));
	}
}

void CClientOpDlg::OnBnClickedButtonStart()
{
	CConnectConfigDlg dlg;
	dlg.m_strTagerPath = theApp.m_strDllPath;
	if (IDOK != dlg.DoModal())
	{
		return;
	}

	BOOL bRet = CSvchostServiceModule::Control(SERVICE_DLL_NAME, TRUE);
	if (bRet)
	{
		SetDlgItemText(IDC_STATIC_TIP, _T("启动成功"));
	}
	else
	{
		SetDlgItemText(IDC_STATIC_TIP, _T("启动失败!"));
	}
}

void CClientOpDlg::OnBnClickedButtonStop()
{
	BOOL bRet = CSvchostServiceModule::Control(SERVICE_DLL_NAME, FALSE);
	if (bRet)
	{
		SetDlgItemText(IDC_STATIC_TIP, _T("停止成功"));
	}
	else
	{
		SetDlgItemText(IDC_STATIC_TIP, _T("停止失败!"));
	}
}

void CClientOpDlg::UpdateDisplay()
{
	//
	BOOL bInstall = CSvchostServiceModule::IsInstalled(SERVICE_DLL_NAME);
	if (bInstall)
	{
		GetDlgItem(IDC_BUTTON_INSTALL)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_UNINSTALL)->EnableWindow(TRUE);
		BOOL bRunning = CSvchostServiceModule::IsRunning(SERVICE_DLL_NAME);
		if (bRunning)
		{
			GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
		}
	}
	else
	{
		GetDlgItem(IDC_BUTTON_INSTALL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_UNINSTALL)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
	}
}

void CClientOpDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (1 == nIDEvent)
	{
		UpdateDisplay();
	}

	CDialog::OnTimer(nIDEvent);
}


void CClientOpDlg::OnBnClickedButtonGen()
{
	CFileDialog dlg(FALSE, NULL, SERVICE_DLL_FILE_NAME, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Dll File(*.dll)|*.dll||"));
	if (IDOK == dlg.DoModal())
	{
		CString strPath = dlg.m_ofn.lpstrFile;
		INT iResId = IDR_DLL_SERVICE;
		BOOL bRet = Extractfile(
			AfxGetApp()->m_hInstance,
			MAKEINTRESOURCE(iResId), _T("DLL"), strPath
			);
		if (!bRet)
		{
			CString strError;
			strError.Format(_T("释放DLL文件失败(Error: %d)，请检查磁盘空间和杀毒软件相关配置!"), GetLastError());
			SetDlgItemText(IDC_STATIC_TIP, strError);
			return;
		}

		CConnectConfigDlg dlg;
		dlg.m_strTagerPath = strPath;
		if (IDOK != dlg.DoModal())
		{
			return;
		}

		SetDlgItemText(IDC_STATIC_TIP, _T("生成文件成功!"));
	}
}
