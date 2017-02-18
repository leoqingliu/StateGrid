
// ScreenLockerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ScreenLocker.h"
#include "ScreenLockerDlg.h"
#include "AboutDlg.h"
#include "Hook.h"
#include "../Public/IPC.h"
#include "../Defines.h"
#include "../Public/Utils.h"
#include "../Public/SystemUtils.h"
#include "../Public/PipeClient.h"
#include "../Public/SvchostServiceModule.h"
#include "../PipeProtocol.h"
#include <TlHelp32.h>
#include <Shlwapi.h>
#include "DialogTip.h"
#include "UDiskLoginDlg.h"

//#define TEST_MODE

#define HOTKEY_ID					1000

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMER_CHECK_TASKMGR		1
#define TIMER_CHECK_BLACK_WIN	2
//#define TIMER_UPDATE_INFO		3
#define TIMER_CHECK_LOCK		4
#define TIMER_CHECK_WHITE_WIN	5
#define TIMER_CHECK_LOCK_TIME	6

#define TIMER_CHECK_WIN					10
#define TIMER_CHECK_UPDATE_UDISK_LETTER	11
#define TIMER_CHECK_UDISK_PASSWD		12

#define TIMER_CHECK_INPUT_INFO			13

#include <Portabledevice.h>
/* A5DCBF10-6530-11D2-901F-00C04FB951ED */
DEFINE_GUID(GUID_DEVINTERFACE_USB_DEVICE, 0xA5DCBF10L, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, \
			0xC0, 0x4F, 0xB9, 0x51, 0xED);
#include <ndisguid.h>
#include <WinIoCtl.h>

// CScreenLockerDlg dialog

CScreenLockerDlg::CScreenLockerDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CScreenLockerDlg::IDD, pParent)
	, m_strPassword(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pPopUp = NULL;
	m_strCurrentUser = CA2T(getenv("USERNAME")).m_psz;
	m_bFullScreen = FALSE;
	m_hAccel = NULL;
}

void CScreenLockerDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_EXIT, m_strPassword);
}

BEGIN_MESSAGE_MAP(CScreenLockerDlg, CResizableDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DEVICECHANGE()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CScreenLockerDlg::OnBnClickedButtonExit)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_COPYDATA()
	ON_REGISTERED_MESSAGE(AFX_WM_ON_CLOSEPOPUPWINDOW, onAlertMessage)
	ON_WM_HOTKEY()
	ON_BN_CLICKED(IDC_BUTTON_SHOW, &CScreenLockerDlg::OnBnClickedButtonShow)
END_MESSAGE_MAP()


// CScreenLockerDlg message handlers

BOOL CScreenLockerDlg::OnInitDialog()
{
	CResizableDialog::OnInitDialog();

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

	m_DeviceUtils.RegisterWnd(m_hWnd);

	//
	AddAnchor(IDC_STATIC_INFO, MIDDLE_CENTER);
	AddAnchor(IDC_BUTTON_EXIT, BOTTOM_RIGHT);
	AddAnchor(IDC_STATIC_EXIT, BOTTOM_RIGHT);
	AddAnchor(IDC_EDIT_EXIT, BOTTOM_RIGHT);

#ifndef TEST_MODE
//#if 1
	theApp.m_bLocked = FALSE;
	//ShowWindow(SW_HIDE);
#else
	theApp.m_bLocked = TRUE;
#endif

	//SetTimer(TIMER_CHECK_TASKMGR, 100, NULL);

 	SetTimer(TIMER_CHECK_BLACK_WIN, 100, NULL);
// 	SetTimer(TIMER_UPDATE_INFO, 100, NULL);
 	SetTimer(TIMER_CHECK_LOCK, 100, NULL);
 	SetTimer(TIMER_CHECK_WHITE_WIN, 200, NULL);
 	SetTimer(TIMER_CHECK_LOCK_TIME, 1000, NULL);
	SetTimer(TIMER_CHECK_UDISK_PASSWD, 100, NULL);
	
	SetTimer(TIMER_CHECK_WIN, 2000, NULL);
	SetTimer(TIMER_CHECK_UPDATE_UDISK_LETTER, 1000, NULL);
//	theApp.m_bLocked = TRUE;
	SetTimer(TIMER_CHECK_INPUT_INFO, 1000, NULL);

	/*
	CMFCDesktopAlertWnd *m_pPopUp;
	m_pPopUp = new CMFCDesktopAlertWnd();
	m_pPopUp->SetAnimationType(CMFCPopupMenu::FADE);
	m_pPopUp->SetAnimationSpeed(100);
	m_pPopUp->SetTransparency(255);
	m_pPopUp->SetAutoCloseTime(1000 * 10);
	CMFCDesktopAlertWndInfo params;
	params.m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	params.m_strText = _T("\n\n\n您的上网时间还剩5分钟，到期电脑将自动锁定，请准备下机\n\n\n");
	params.m_strURL = _T("");
	params.m_nURLCmdID = 0;
	m_pPopUp->Create(this, params, NULL, CPoint(-1, -1));
	*/

	/*
	BOOL bRegister = RegisterHotKey(m_hWnd, HOTKEY_ID, MOD_CONTROL | MOD_ALT, VK_F3);
	if (!bRegister)
	{
		bRegister = RegisterHotKey(m_hWnd, HOTKEY_ID, MOD_CONTROL | MOD_ALT, VK_F2);
		if (!bRegister)
		{
			bRegister = RegisterHotKey(m_hWnd, HOTKEY_ID, MOD_CONTROL | MOD_ALT, VK_F4);
			if (!bRegister)
			{
				AfxMessageBox(_T("注册热键失败!"), MB_ICONERROR);
			}
		}
	}
	else
	{
	}
	*/
	m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CScreenLockerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CResizableDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CScreenLockerDlg::OnPaint()
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
		CResizableDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CScreenLockerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CScreenLockerDlg::ChangeFullScreen()
{
	LONG style = GetWindowLong(m_hWnd, GWL_STYLE);
	::ShowWindow(m_hWnd, SW_MAXIMIZE);
	style = GetWindowLong(m_hWnd, GWL_STYLE);
	style &= ~(WS_DLGFRAME | WS_THICKFRAME);
	SetWindowLong(m_hWnd, GWL_STYLE, style);
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);
	::SetWindowPos(m_hWnd, HWND_TOPMOST, -1, -1, cx + 3, cy + 3, SWP_FRAMECHANGED);
	m_bFullScreen = TRUE;
	::InvalidateRect(m_hWnd, NULL, TRUE);
}

BOOL CScreenLockerDlg::SetAlpha(BYTE dwAlpha)
{
	BOOL bRet = FALSE;
	SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE)^0x80000);
	HINSTANCE hInst = LoadLibrary(_T("User32.DLL"));
	if (hInst)
	{
		typedef BOOL (WINAPI *MYFUNC)(HWND, COLORREF, BYTE, DWORD);
		MYFUNC fun = NULL;
		fun = (MYFUNC)GetProcAddress(hInst, "SetLayeredWindowAttributes");
		if (fun)
		{
			bRet = fun(this->GetSafeHwnd(), 0, dwAlpha, 2);
		}
		FreeLibrary(hInst);
	}

	return bRet;
}

void CScreenLockerDlg::OnBnClickedButtonExit()
{
	UpdateData();
	CString strPassword;
	theApp.m_InfoCS.Lock();
	strPassword = theApp.m_Info.strUnlockPassword;
	theApp.m_InfoCS.Unlock();

	if (m_strPassword != strPassword)
	{
		AfxMessageBox(_T("密码错误！"), MB_ICONERROR);
		return;
	}

	m_strPassword = _T("");
	UpdateData(FALSE);

// 	if (NULL != m_pPopUp)
// 	{
// 		delete m_pPopUp;
// 		m_pPopUp = NULL;
// 	}

//	OnOK();

	Json::Value jsonItem;
	jsonItem["Type"]	= PIPE_TYPE_UNLOCK;
	jsonItem["Content"]	= "";
	Json::FastWriter writer;
	std::string strPipeContent = writer.write(jsonItem);
	PipeClient client;
	NamedPipe *pPipe = client.Connect(PIPE_SERVICE_NAME);
	if (NULL != pPipe)
	{
		pPipe->Write(strPipeContent.c_str(), strPipeContent.size());
		NamedPipe::Push(pPipe);
	}

#if 0
	PipeClient client;
	NamedPipe *pPipe = client.Connect(_T("SCREEN_LOCKER_PIPE_SERVER"));
	std::auto_ptr<NamedPipe> ptr(pPipe);
	if (NULL != pPipe)
	{
		pipe_protocol_t proto;
		ZeroMemory(&proto, sizeof(proto));
		proto.dwProtocolId = PIPE_PROTOCOL_ID;
		proto.uType = 100;
		for (int i = 0; i < 100; i++)
		{
			proto.uParams[i] = i;
		}
		pPipe->Write(&proto, sizeof(proto));		
	}
#endif
}

void CScreenLockerDlg::OnClose()
{
	/*
	if (NULL != m_pPopUp)
	{
		delete m_pPopUp;
		m_pPopUp = NULL;
	}

	CResizableDialog::OnClose();
	*/
}

CString GetProcessName(DWORD dwProcessId)
{
	PROCESSENTRY32 pe32 = {0};
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);    
	if (hProcessSnap != (HANDLE)-1)    
	{
		pe32.dwSize = sizeof(PROCESSENTRY32);    
		if (Process32First(hProcessSnap, &pe32))    
		{  
			while(1)  
			{
				if (pe32.th32ProcessID == dwProcessId)
				{
					CloseHandle(hProcessSnap);
					return pe32.szExeFile;
				}
				
				if (!Process32Next(hProcessSnap, &pe32))
				{
					break;
				}
			}
		}
		CloseHandle(hProcessSnap);
	}
	return _T("");
}


VOID KillWindowProc(HWND hWnd)
{
	if (IsWindow(hWnd) && NULL == GetParent(hWnd))
	{
		OutputLog(_T("Find Windows !!: %X\n"), hWnd);
		DWORD dwProcessId;
		DWORD dwThreadId = GetWindowThreadProcessId(hWnd, &dwProcessId);
		CString strProcName = GetProcessName(dwProcessId);
		if (0 == _tcsicmp(strProcName, _T("explorer.exe")) || 
			0 == _tcsicmp(strProcName, _T("ScreenLocker.exe")) ||
			0 == _tcsicmp(strProcName, _T("tvnserver.exe")) ||
			0 == _tcsicmp(strProcName, _T("ClientOp.exe")))
		{
			//OutputLog(_T("Find Explorer Windows"));
			//::SendMessage(hWnd, WM_CLOSE, NULL, NULL);
		}
		else
		{
			//OutputLog(_T("Find Other Windows: %d"), dwProcessId);
			HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessId);
			if (hProc != 0)
			{
				TerminateProcess(hProc, -1);
			}
		}
	}
}

BOOL CALLBACK EnumWndWhiteProc(HWND hWnd, LPARAM parameter)
{
	_CLIENT_INFO *pInfo = (_CLIENT_INFO *)parameter;
	if (NULL != GetParent(hWnd) || !IsWindow(hWnd) || !IsWindowVisible(hWnd))
	{
		return TRUE;
	}
	
	DWORD dwProcessId = 0;
	DWORD dwThreadId = GetWindowThreadProcessId(hWnd, &dwProcessId);
	TCHAR szWinName[MAX_PATH] = {0};
	GetWindowText(hWnd, szWinName, MAX_PATH);
	TCHAR szClassName[MAX_PATH] = {0};
	GetClassName(hWnd, szClassName, MAX_PATH);
	OutputLog(_T("Win: %s, Class: %s, ProcId: %d, Rule: %d\n"), szWinName, szClassName, dwProcessId, pInfo->iWhiteWinCount);

	BOOL bKill = TRUE;
	for (int i = 0; i < (int)pInfo->iWhiteWinCount; i++)
	{
		LPCTSTR pWin = 0 == lstrlen(pInfo->vecWhiteWin[i]) ? NULL : pInfo->vecWhiteWin[i];
		LPCTSTR pClass = 0 == lstrlen(pInfo->vecWhiteClass[i]) ? NULL : pInfo->vecWhiteClass[i];
		if (NULL == pWin && NULL == pClass)
		{
			continue;
		}
		//OutputLog(_T(" --Win: %s, Class: %s"), pWin, pClass);

		if (NULL != pWin)
		{
			//if (0 != lstrcmp(pWin, szWinName))
			if (NULL != StrStrI(szWinName, pWin))
			{
				bKill = FALSE;
				break;
				//OutputLog(_T("Window: Matched"));
				//KillWindowProc(hWnd);
				/*
				if (0 == lstrcmp(pWin, _T("注册")) && 0 == lstrcmp(szWinName, _T("注册表编辑器")))
				{
					OutputLog(_T("Find Windows: %X"), hWnd);
					OutputLog(_T("Window: %s"), szWinName);
					//KillWindowProc(hWnd);
				}
				*/
			}
		}
		if (NULL != pClass)
		{
			//if (0 != lstrcmp(pClass, szClassName))
			if (NULL != StrStrI(szClassName, pClass))
			{
				//KillWindowProc(hWnd);
				bKill = FALSE;
				break;
			}
		}
	}
	if (bKill)
	{
		KillWindowProc(hWnd);
	}

	return TRUE;
}


BOOL CALLBACK EnumWndBlackProc(HWND hWnd, LPARAM parameter)
{
	_CLIENT_INFO *pInfo = (_CLIENT_INFO *)parameter;

	TCHAR szWinName[MAX_PATH] = {0};
	GetWindowText(hWnd, szWinName, MAX_PATH);
	TCHAR szClassName[MAX_PATH] = {0};
	GetClassName(hWnd, szClassName, MAX_PATH);

	for (int i = 0; i < (int)pInfo->iWinCount; i++)
	{
		LPCTSTR pWin = 0 == lstrlen(pInfo->vecWin[i]) ? NULL : pInfo->vecWin[i];
		LPCTSTR pClass = 0 == lstrlen(pInfo->vecClass[i]) ? NULL : pInfo->vecClass[i];
		if (NULL == pWin && NULL == pClass)
		{
			continue;
		}

		if (NULL != pWin)
		{
			//if (0 != lstrcmp(pWin, szWinName))
			if (NULL != StrStrI(szWinName, pWin))
			{
				KillWindowProc(hWnd);
			}
		}
		if (NULL != pClass)
		{
			//if (0 != lstrcmp(pClass, szClassName))
			if (NULL != StrStrI(szClassName, pClass))
			{
				KillWindowProc(hWnd);
			}
		}
	}

	return TRUE;
}

BOOL CALLBACK EnumWndProc(HWND hWnd, LPARAM parameter)
{
	//if (NULL != GetParent(hWnd) || !IsWindow(hWnd) || !IsWindowVisible(hWnd))
	if (!IsWindow(hWnd) || !IsWindowVisible(hWnd))
	{
		return TRUE;
	}

	std::vector<CScreenLockerDlg::ENUM_WIN_INFO> *vecEnumInfo = (std::vector<CScreenLockerDlg::ENUM_WIN_INFO> *)parameter;
	CScreenLockerDlg::ENUM_WIN_INFO info;
	info.hWnd = hWnd;
	DWORD dwProcessId;
	DWORD dwThreadId = GetWindowThreadProcessId(hWnd, &dwProcessId);
	info.dwProcessId = dwProcessId;
	TCHAR szTmp[MAX_PATH];
	GetWindowText(hWnd, szTmp, MAX_PATH);
	info.strWinName = szTmp;
	GetClassName(hWnd, szTmp, MAX_PATH);
	info.strClassName = szTmp;
	//	OutputLog(_T("Win: %s, Class: %s, ProcId: %d\n"), szWinName, szClassName, dwProcessId);
	vecEnumInfo->push_back(info);
	return TRUE;
}

void CScreenLockerDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMER_CHECK_TASKMGR == nIDEvent)
	{
		HWND hFind = ::FindWindow(NULL, _T("Windows 任务管理器"));
		if (hFind)
		{
			::SendMessage(hFind, WM_CLOSE, NULL, NULL);
		}

		hFind = ::FindWindow(NULL, _T("任务管理器"));
		if (hFind)
		{
			::SendMessage(hFind, WM_CLOSE, NULL, NULL);
		}

#ifndef TEST_MODE
		//if (!m_bFullScreen)
		{
			int cx = GetSystemMetrics(SM_CXSCREEN);
			CRect winRect;
			GetWindowRect(&winRect);
			int iWidth = winRect.Width();
			if (iWidth < cx - 100)
			{
				ChangeFullScreen();
			}
		}
#endif

		SetForegroundWindow();
	}
	else if (TIMER_CHECK_BLACK_WIN == nIDEvent)
	{
		_CLIENT_INFO info;
		theApp.m_InfoCS.Lock();
		info = theApp.m_Info;
		theApp.m_InfoCS.Unlock();
		//CString strInfo;
		//strInfo.Format(_T("-- WhiteMode: %d"), info.bWhiteMode);
		//OutputDebugString(strInfo);
		if (!info.bWhiteMode)
		{
			if (0 != info.iWinCount)
			{
				EnumWindows(EnumWndBlackProc, (LPARAM)&info);
			}
			/*
			CString strError;
			//strError.Format(_T("-- Find: %d"), info.iWinCount);
			for (int i = 0; i < (int)info.iWinCount; i++)
			{
				LPCTSTR pWin = 0 == lstrlen(info.vecWin[i]) ? NULL : info.vecWin[i];
				LPCTSTR pClass = 0 == lstrlen(info.vecClass[i]) ? NULL : info.vecClass[i];
				if (NULL == pWin && NULL == pClass)
				{
					continue;
				}

				HWND hTarget = ::FindWindow(pClass, pWin);
				//strError.Format(_T("-- Find Wnd: %x"), hTarget);
				if (hTarget)
				{
					//::SendMessage(hTarget, WM_CLOSE, NULL, NULL);

					DWORD dwProcessId;
					DWORD dwThreadId = GetWindowThreadProcessId(hTarget, &dwProcessId);
					CString strProcName = GetProcessName(dwProcessId);
					if (0 == _tcsicmp(strProcName, _T("explorer.exe")) || 
						0 == _tcsicmp(strProcName, _T("ScreenLocker.exe")) ||
						0 == _tcsicmp(strProcName, _T("tvnserver.exe")) ||
						0 == _tcsicmp(strProcName, _T("ClientOp.exe")))
					{
						//::SendMessage(hWnd, WM_CLOSE, NULL, NULL);
					}
					else
					{
						HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessId);
						if (hProc != 0)
						{
							TerminateProcess(hProc, -1);
						}
					}
				}
			}
			if (!strError.IsEmpty())
			{
				OutputDebugString(strError);
			}
			*/
		}		
	}
#if 0
	else if (TIMER_UPDATE_INFO == nIDEvent)
	{
		IPC ipc;
		BOOL bOpen = ipc.Open(IPC_SHARED_MMF_INFO);
		if (bOpen)
		{
			_CLIENT_INFO info;
			DWORD dwSize = sizeof(_CLIENT_INFO);
			BOOL bRead = ipc.Read((LPBYTE)&info, dwSize);
			if (bRead && dwSize == sizeof(_CLIENT_INFO))
			{
				//OutputDebugString(_T("READ OK"));
				m_LockInfoCS.Lock();
				if (0 != memcmp(&info, &m_LockInfo, sizeof(_CLIENT_INFO)))
				{
					OutputDebugString(_T("READ OK UPDATE"));
					memcpy(&m_LockInfo, &info, sizeof(_CLIENT_INFO));		
				}
				else
				{
					//OutputDebugString(_T("READ OK NOT UPDATE"));
				}
				m_LockInfoCS.Unlock();
			}
			else
			{
				//OutputDebugString(_T("READ FAIL"));
			}
		}
		else
		{
			//OutputDebugString(_T("OPEN FAILED"));
		}
	}
#endif
	else if (TIMER_CHECK_LOCK == nIDEvent)
	{
#if 0
		IPC ipc;
		BOOL bOpen = ipc.Open(IPC_SHARED_MMF_LOCK);
		if (bOpen)
		{
			DWORD dwType;
			DWORD dwSize = sizeof(DWORD);
			BOOL bRead = ipc.Read((LPBYTE)&dwType, dwSize);
			if (bRead && dwSize == sizeof(DWORD))
			{
				DWORD dwTargetType = 0;
				BOOL bWrite = ipc.Write((LPBYTE)&dwTargetType, sizeof(DWORD));
				if (bWrite)
				{
					CString strError;
					if (1 == dwType)
					{
						strError.Format(_T("----- Msg Matched 1"));
						m_bFullScreen = FALSE;
						ChangeFullScreen();
						SetAlpha(230);
						SetTimer(TIMER_CHECK_TASKMGR, 50, NULL);
						theApp.ControlKey(FALSE);
						m_bLocked = TRUE;
						ShowWindow(SW_SHOW);
						::InvalidateRect(m_hWnd, NULL, TRUE);
					}
					else if (2 == dwType)
					{
						strError.Format(_T("----- Msg Matched 2"));
						KillTimer(1);
						theApp.ControlKey(TRUE);
						m_bLocked = FALSE;
						ShowWindow(SW_HIDE);
						::InvalidateRect(m_hWnd, NULL, TRUE);
					}
					
					OutputDebugString(strError);
				}
			}
		}
		else
		{
			OutputDebugString(_T("OPEN FAILED"));
		}
#else
		theApp.m_dequeLockInfoLock.Lock();
		int iType = -1;
		if (theApp.m_dequeLockInfo.size() > 0)
		{
			iType = theApp.m_dequeLockInfo[0];
			theApp.m_dequeLockInfo.pop_front();
		}
		theApp.m_dequeLockInfoLock.Unlock();
		if (-1 != iType)
		{
			CString strError;
			if (PIPE_TYPE_LOCK == iType)
			{
				strError.Format(_T("----- Msg Matched 1"));
				//m_bFullScreen = FALSE;
				//SetAlpha(230);
				SetTimer(TIMER_CHECK_TASKMGR, 50, NULL);
#ifndef TEST_MODE
				theApp.ControlKey(FALSE);
#endif
				GetDlgItem(IDC_BUTTON_EXIT)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_EXIT)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_EXIT)->ShowWindow(SW_HIDE);

				theApp.m_bLocked = TRUE;
				ShowWindow(SW_SHOW);
				::InvalidateRect(m_hWnd, NULL, TRUE);
			}
			else if (PIPE_TYPE_UNLOCK == iType)
			{
				strError.Format(_T("----- Msg Matched 2"));
				//SetAlpha(0);
				KillTimer(TIMER_CHECK_TASKMGR);
#ifndef TEST_MODE
				theApp.ControlKey(TRUE);
#endif
				theApp.m_bLocked = FALSE;
				ShowWindow(SW_HIDE);
				::InvalidateRect(m_hWnd, NULL, TRUE);
			}

			OutputDebugString(strError);
		}
#endif
	}
	else if (TIMER_CHECK_WHITE_WIN == nIDEvent)
	{
		_CLIENT_INFO info;
		theApp.m_InfoCS.Lock();
		info = theApp.m_Info;
		theApp.m_InfoCS.Unlock();
		
		if (info.bWhiteMode)
		{
			//if (0 != info.iWhiteWinCount)
			{
				EnumWindows(EnumWndWhiteProc, (LPARAM)&info);
			}
		}
	}
	else if (TIMER_CHECK_LOCK_TIME == nIDEvent)
	{
		_CLIENT_INFO info;
		theApp.m_InfoCS.Lock();
		info = theApp.m_Info;
		theApp.m_InfoCS.Unlock();

#if 0
		COleDateTime oleTime = COleDateTime::GetCurrentTime();
		DWORD dwHour = oleTime.GetHour();
		DWORD dwMin = oleTime.GetMinute();
		DWORD dwSec = oleTime.GetSecond();
		//DWORD dwTime = (((DWORD)dwHour) << 16) + (((DWORD)dwMin) << 8) + dwSec;
		DWORD dwTime = dwHour * 3600 + dwMin * 60 + dwSec;
		OutputLog(_T("Check Lock Time: Time[%d], Start[%d], End[%d]"), dwTime, info.uTimeStart, info.uTimeEnd);
		if (dwTime + 60 * 5 > info.uTimeEnd && dwTime < info.uTimeEnd)
#endif

		DWORD dwMaxEnd = 0;
		for (int i = 0; i < (int)info.vecTimeRange.size(); i++)
		{
			if (info.vecTimeRange[i].uTimeEnd > dwMaxEnd)
			{
				dwMaxEnd = info.vecTimeRange[i].uTimeEnd;
			}
		}
		DWORD dwTime = Utils::GetCurrentTimestamp();
		if (dwTime + 60 * 5 > dwMaxEnd && dwTime < dwMaxEnd)
		//if (TRUE)
		{
			OutputLog(_T("Check Lock Time OK\n"));
			if (NULL == m_pPopUp)
			{
				/*
				m_pPopUp = new CMFCDesktopAlertNoCloseWnd();
				m_pPopUp->SetAnimationType(CMFCPopupMenu::FADE);
				m_pPopUp->SetAnimationSpeed(100);
				m_pPopUp->SetTransparency(255);
				m_pPopUp->SetAutoCloseTime(1000 * 10);
				CMFCDesktopAlertWndInfo params;
				params.m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
				params.m_strText = _T("\n\n\n您的上网时间还剩5分钟，到期电脑将自动锁定，请准备下机\n\n\n");
				params.m_strURL = _T("");
				params.m_nURLCmdID = 0;
				m_pPopUp->Create(this, params, NULL, CPoint(-1, -1));
				*/
				m_pPopUp = new CDialogTip();
				if (m_pPopUp->Create(CDialogTip::IDD))
				{
					RECT rectWin;
					m_pPopUp->GetWindowRect(&rectWin);
					RECT rectDesktop;
					//GetDesktopWindow()->GetWindowRect(&rectDesktop);
					SystemParametersInfo(SPI_GETWORKAREA, 0, (PVOID)&rectDesktop, 0);
					m_pPopUp->MoveWindow(
						rectDesktop.right - (rectWin.right - rectWin.left), rectDesktop.bottom - (rectWin.bottom - rectWin.top),
						rectWin.right - rectWin.left,
						rectWin.bottom - rectWin.top
						);
					m_pPopUp->ShowWindow(SW_SHOW);
					m_pPopUp->SetForegroundWindow();
				}
			}
		}
		else
		{
			OutputLog(_T("Check Lock Time Not OK\n"));
			if (NULL != m_pPopUp)
			{
				delete m_pPopUp;
				m_pPopUp = NULL;
			}
		}
	}
	else if (TIMER_CHECK_WIN == nIDEvent)
	{
		std::vector<ENUM_WIN_INFO> vecEnumInfo;
		EnumWindows(EnumWndProc, (LPARAM)&vecEnumInfo);
		std::vector<DWORD> vecIds;
		std::vector<CString> vecNames;
		//Utils::GetProcessName(vecNames, vecIds);
		std::vector<Utils::ProcessInfo> vecInfos;
		Utils::GetProcessNames(vecInfos);
		for (int j = 0; j < (int)vecInfos.size(); j++)
		{
			if (m_strCurrentUser.MakeLower() == vecInfos[j].strUserName.MakeLower())
			{
				vecNames.push_back(vecInfos[j].strProcessName);
				vecIds.push_back(vecInfos[j].dwProcessId);
			}
		}
		std::map<DWORD, ENUM_PROC_INFO> mapProcInfo;
		for (int j = 0; j < (int)vecNames.size(); j++)
		{
			DWORD dwProcessId = vecIds[j];
			ENUM_PROC_INFO info;
			CString strFileDesc;
			CString strProductDesc;
			CString strFileVersion;
			BOOL bRet = Utils::GetPidInfo(dwProcessId, strFileDesc, strProductDesc, strFileVersion);
			if (bRet)
			{
				if (!strFileDesc.IsEmpty())
				{
					info.strFileDesc = strFileDesc;
				}
				if (!strProductDesc.IsEmpty())
				{
					info.strProductDesc = strProductDesc;
				}
			}
			info.strProcessName = vecNames[j];

			std::map<DWORD, ENUM_PROC_INFO>::iterator pIter = mapProcInfo.find(dwProcessId);
			if (pIter == mapProcInfo.end())
			{
				mapProcInfo.insert(std::map<DWORD, ENUM_PROC_INFO>::value_type(dwProcessId, info));
			}
		}
		// Update Window Process Info
		std::set<DWORD> setEnumInfoPid;
		for (int i = 0; i < (int)vecEnumInfo.size(); i++)
		{
			setEnumInfoPid.insert(vecEnumInfo[i].dwProcessId);

			std::map<DWORD, ENUM_PROC_INFO>::iterator pIter = mapProcInfo.find(vecEnumInfo[i].dwProcessId);
			if (pIter != mapProcInfo.end())
			{
				vecEnumInfo[i].strProcessName = pIter->second.strProcessName;
				vecEnumInfo[i].strFileDesc	  = pIter->second.strFileDesc;
				vecEnumInfo[i].strProductDesc = pIter->second.strProductDesc;
			}
		}
		// Update No-Window Process Info
		for (std::map<DWORD, ENUM_PROC_INFO>::iterator pIter = mapProcInfo.begin(); pIter != mapProcInfo.end(); pIter++)
		{
			std::set<DWORD>::iterator pIterFound = setEnumInfoPid.find(pIter->first);
			if (pIterFound == setEnumInfoPid.end())
			{
				ENUM_WIN_INFO winInfo;
				winInfo.dwProcessId = pIter->first;
				winInfo.strProcessName = pIter->second.strProcessName;
				winInfo.strProductDesc = pIter->second.strProductDesc;
				winInfo.strFileDesc = pIter->second.strFileDesc;
				vecEnumInfo.push_back(winInfo);

				setEnumInfoPid.insert(pIter->first);
			}
		}
		FilterWnds(vecEnumInfo);

		#define SHARE_SERVICE _T("LanmanServer")
		if (theApp.m_Info.configInfo.m_bCheckShare)
		{
			if (CSvchostServiceModule::IsRunning(SHARE_SERVICE))
			{
				CSvchostServiceModule::Control(SHARE_SERVICE, FALSE, TRUE);
			}
		}
		else
		{
			if (!CSvchostServiceModule::IsRunning(SHARE_SERVICE))
			{
				CSvchostServiceModule::Control(SHARE_SERVICE, TRUE);
			}
		}
	}
	else if (TIMER_CHECK_UPDATE_UDISK_LETTER == nIDEvent)
	{
		UpdateExplorerId();
	}
	else if (TIMER_CHECK_UDISK_PASSWD == nIDEvent)
	{
		BOOL bGot = theApp.m_uDiskUtils.GetDeviceProcInfo();
		if (bGot && !theApp.m_bDiskWaitForConfirm)
		{
			theApp.m_bDiskWaitForConfirm = TRUE;
			if (theApp.m_bDiskAllow)
			{
				BOOL bCheckSuccess = FALSE;
				CUDiskLoginDlg dlg;
				if (dlg.Create(CUDiskLoginDlg::IDD))
				{
					//dlg.ShowWindow(SW_HIDE);
					int iDlgRet = dlg.RunModalLoop();
					if (IDOK == iDlgRet)
					{
						CString strUDiskPasswd = theApp.m_Info.configInfo.m_szUDiskPasswd;
						if (0 == dlg.m_strPassword.Compare(strUDiskPasswd))
						{
							bCheckSuccess = TRUE;
						}
						else
						{
							AfxMessageBox(_T("密码错误！"), MB_ICONERROR);
						}
					}
				}
				//theApp.AddLog(_T("UDiskPasswd"), bCheckSuccess ? _T("OK") : _T("FAIL"), _T(""), _T(""), _T(""), _T(""));
				theApp.m_uDiskUtils.SetAuthResult(bCheckSuccess);
			}
			else
			{
				theApp.m_uDiskUtils.SetAuthResult(FALSE);
			}
			theApp.m_bDiskWaitForConfirm = FALSE;
		}
	}
	else if (TIMER_CHECK_INPUT_INFO == nIDEvent)
	{
		NamedPipe::Check();

		LASTINPUTINFO lpi;
		lpi.cbSize = sizeof(LASTINPUTINFO);
		BOOL bInput = GetLastInputInfo(&lpi);
		if (bInput)
		{
			DWORD dwTick = GetTickCount();
			DWORD dwSecond = (dwTick - lpi.dwTime) / 1000;
			OutputLog(_T("Check Input Info Success: Time[%d], Second[%d]\n"), lpi.dwTime, dwSecond);
			if (0 != theApp.m_Info.dwIdleTime && dwSecond >= theApp.m_Info.dwIdleTime)
			{
				Json::Value jsonItem;
				jsonItem["Type"]	= PIPE_TYPE_LOCK;
				jsonItem["Content"]	= "";
				Json::FastWriter writer;
				std::string strPipeContent = writer.write(jsonItem);
				PipeClient client;
				NamedPipe *pPipe = client.Connect(PIPE_SERVICE_NAME);
				if (NULL != pPipe)
				{
					pPipe->Write(strPipeContent.c_str(), strPipeContent.size());
					NamedPipe::Push(pPipe);
				}
			}
		}
		else
		{
			OutputLog(_T("Check Input Info Failed\n"));
		}
	}

	CResizableDialog::OnTimer(nIDEvent);
}

BOOL CScreenLockerDlg::PreTranslateMessage(MSG* pMsg)
{
	if (::TranslateAccelerator(GetSafeHwnd(), m_hAccel, pMsg))
		return TRUE;

	if (pMsg->message == WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
		case VK_RETURN:
			return TRUE;
		case VK_ESCAPE:
			return TRUE;
		}
	}
	
	return CResizableDialog::PreTranslateMessage(pMsg);
}

void CScreenLockerDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	if (!theApp.m_bLocked)
	{
		lpwndpos->flags &= ~SWP_SHOWWINDOW;
	}
	else
	{
		lpwndpos->flags |= SWP_SHOWWINDOW;
	}
	
	CResizableDialog::OnWindowPosChanging(lpwndpos);
}

BOOL CScreenLockerDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	/*
	CString strError;
	if (pCopyDataStruct->cbData == sizeof(DWORD))
	{
		strError.Format(_T("----- Msg Matched"));
		DWORD dwType = *((PDWORD)pCopyDataStruct->lpData);
		if (1 == dwType)
		{
			strError.Format(_T("----- Msg Matched 1"));
			m_bFullScreen = FALSE;
			ChangeFullScreen();
			SetAlpha(230);
			SetTimer(1, 50, NULL);
			theApp.ControlKey(FALSE);
			m_bLocked = TRUE;
			ShowWindow(SW_SHOW);
			::InvalidateRect(m_hWnd, NULL, TRUE);		
		}
		else if (2 == dwType)
		{
			strError.Format(_T("----- Msg Matched 2"));
			KillTimer(1);
			theApp.ControlKey(TRUE);
			m_bLocked = FALSE;
			ShowWindow(SW_HIDE);
			::InvalidateRect(m_hWnd, NULL, TRUE);
		}
		else
		{
			strError.Format(_T("----- Msg Matched Other"));
		}
	}
	*/
	/*
	else if (pCopyDataStruct->cbData == sizeof(_CLIENT_INFO))
	{
		strError.Format(_T("----- Msg Matched Update"));
		memcpy(&m_LockInfo, pCopyDataStruct->lpData, sizeof(_CLIENT_INFO));		
		CString strError;
		strError.Format(_T("-- Find --: %d"), m_LockInfo.iWinCount);
		OutputDebugString(strError);
	}
	
	else
	{
		strError.Format(_T("----- Msg Not Matched"));
	}
	OutputDebugString(strError);
	*/

	return CResizableDialog::OnCopyData(pWnd, pCopyDataStruct);
}

LRESULT CScreenLockerDlg::onAlertMessage(WPARAM wparam, LPARAM lparam)
{
//	if (NULL != m_pPopUp)
//	{
//		delete m_pPopUp;
//		m_pPopUp = NULL;
//	}

	return 0;
}

BOOL gExplorerChildFound = FALSE;
BOOL CALLBACK ExplorerChildWndProc(HWND hWnd, LPARAM parameter)
{
	const std::vector<CString> &vecLetter = *(std::vector<CString> *)parameter;
	TCHAR szClassName[MAX_PATH] = {0};
	GetClassName(hWnd, szClassName, MAX_PATH);
	TCHAR szWinName[MAX_PATH] = {0};
	GetWindowText(hWnd, szWinName, MAX_PATH);
	if (NULL != StrStrI(szClassName, _T("ToolbarWindow32")))
	{
		for (int i = 0; i < vecLetter.size(); i++)
		{
			if (NULL != StrStrI(szWinName, vecLetter[i]))
			{
				gExplorerChildFound = TRUE;
				return FALSE;
			}
		}
	}

	return TRUE;
}
VOID CScreenLockerDlg::FilterWnds(const std::vector<ENUM_WIN_INFO>& vecInfo)
{
	CONFIG_INFO ruleConfig;
	theApp.m_ConfigLock.Lock();
	ruleConfig = theApp.m_Info.configInfo;
	theApp.m_ConfigLock.Unlock();

	CString strType;
	for (std::vector<ENUM_WIN_INFO>::const_iterator pIter = vecInfo.begin(); pIter != vecInfo.end(); pIter++)
	{
// 		std::set<DWORD>::iterator pSetIter = setExceptPids.find(pIter->dwProcessId);
// 		if (pSetIter != setExceptPids.end())
// 		{
// 			continue;
// 		}
		
		// Check Normal
		BOOL bSendCloseMsg = FALSE;
		BOOL bKill = FALSE;
		if (!bKill && ruleConfig.m_bCheckRegEdit && (NULL != StrStrI(pIter->strWinName, _T("注册表编辑器"))))
		{
			bKill = TRUE;
			strType = _T("RegEdit");
		}
		if (!bKill && ruleConfig.m_bCheckDevMgr && (NULL != StrStrI(pIter->strWinName, _T("设备管理器"))))
		{
			bKill = TRUE;
			strType = _T("DevMgr");
		}
		if (!bKill && ruleConfig.m_bCheckGpedit && (NULL != StrStrI(pIter->strWinName, _T("组策略编辑器"))))
		{
			bKill = TRUE;
			strType = _T("Gpedit");
		}
		if (!bKill && ruleConfig.m_bCheckComputermgr && (NULL != StrStrI(pIter->strWinName, _T("计算机管理"))))
		{
			bKill = TRUE;
			strType = _T("Computermgr");
		}
		if (!bKill && ruleConfig.m_bCheckTaskmgr && (NULL != StrStrI(pIter->strWinName, _T("任务管理器"))))
		{
			bKill = TRUE;
			strType = _T("Taskmgr");
		}
		if (!bKill && ruleConfig.m_bCheckMsconfig && (NULL != StrStrI(pIter->strWinName, _T("系统配置"))))
		{
			bKill = TRUE;
			strType = _T("Msconfig");
		}
		if (!bKill && ruleConfig.m_bCheckDos && (NULL != StrStrI(pIter->strClassName, _T("ConsoleWindowClass"))))
		{
			bKill = TRUE;
			strType = _T("Dos");
		}
		if (!bKill && ruleConfig.m_bCheckFormat && (NULL != StrStrI(pIter->strWinName, _T("ghost"))))
		{
			bKill = TRUE;
			strType = _T("Format");
		}
		if (!bKill && ruleConfig.m_bCheckFormat && (NULL != StrStrI(pIter->strWinName, _T("格式化"))))
		{
			bSendCloseMsg = TRUE;
			strType = _T("Format");
		}
		if (!bKill && ruleConfig.m_bCheckIp && (NULL != StrStrI(pIter->strWinName, _T("TCP/IP"))))
		{
			bSendCloseMsg = TRUE;
			strType = _T("Ip");
		}
		if (!bKill && ruleConfig.m_bCheckIp && (NULL != StrStrI(pIter->strProcessName, _T("rundll"))))
		{
			bKill = TRUE;
			strType = _T("Mac");
		}
		
		if (!bKill && ruleConfig.m_bCheckTelnet && (NULL != StrStrI(pIter->strProcessName, _T("telnet"))))
		{
			bKill = TRUE;
			strType = _T("Telnet");
		}

		if (!bKill && ruleConfig.m_bCheckVir && (
			NULL != StrStrI(pIter->strWinName, _T("vmware")) ||
			NULL != StrStrI(pIter->strWinName, _T("虚拟机")) ||
			NULL != StrStrI(pIter->strWinName, _T("Virtual Box")) ||
			NULL != StrStrI(pIter->strWinName, _T("Virtual Machine")) ||
			NULL != StrStrI(pIter->strWinName, _T("Virtual PC"))
			))
		{
			bKill = TRUE;
			strType = _T("Vir");
		}
		if (!bKill && ruleConfig.m_bCheckCreateUser && (
			NULL != StrStrI(pIter->strWinName, _T("管理帐户")) ||
			NULL != StrStrI(pIter->strWinName, _T("创建新帐户")) ||
			NULL != StrStrI(pIter->strWinName, _T("新用户"))
			))
		{
			bSendCloseMsg = TRUE;
			strType = _T("CreateUser");
		}
		if (!bKill && ruleConfig.m_uUDiskType == UDISK_TYPE_COPYTOCOMPUTER)
		{
			m_DataLock.Lock();
			DWORD dwExplorerId = m_dwExplorerId;
			std::vector<CString> vecUDiskLetter = m_vecUDiskLetter;
			m_DataLock.Unlock();
			if (pIter->dwProcessId == dwExplorerId)
			{
				gExplorerChildFound = FALSE;
				EnumChildWindows(pIter->hWnd, ExplorerChildWndProc, (LPARAM)&vecUDiskLetter);
				if (gExplorerChildFound)
				{
					bSendCloseMsg = TRUE;
					strType = _T("UDisk CopyToComputer");
				}
			}
		}

		if (0 != pIter->strProcessName.CompareNoCase(_T("devenv.exe")))
		{
			if (bKill)
			{
				BOOL bCheck = Utils::CheckPid(pIter->dwProcessId);
				if (!bCheck)
				{
					/*
					DWORD dwCount = 1;
					std::map<DWORD, DWORD>::iterator pIterProc = m_mapPidKillCount.find(pIter->dwProcessId);
					if (pIterProc == m_mapPidKillCount.end())
					{
						m_mapPidKillCount.insert(std::map<DWORD, DWORD>::value_type(pIter->dwProcessId, 1));
					}
					else
					{
						pIterProc->second += 1;
						dwCount = pIterProc->second;
					}
					if (dwCount < 3)
					{
						Utils::KillPid(pIter->dwProcessId);
						theApp.AddLog(strType, pIter->strWinName, pIter->strClassName, pIter->procInfo.strProcessName, pIter->procInfo.strFileDesc, pIter->procInfo.strProductDesc);
					}
					*/
					Utils::KillPid(pIter->dwProcessId);
					BOOL bKilled = FALSE;
					for (int i = 0; i < 20; i++)
					{
						CString strProcessName = Utils::GetProcessName(pIter->dwProcessId);
						if (strProcessName.IsEmpty())
						{
							bKilled = TRUE;
							break;
						}
						Sleep(100);
					}
					//if (bKilled)
					//{
					//	theApp.AddLog(strType, pIter->strWinName, pIter->strClassName, pIter->strProcessName, pIter->strFileDesc, pIter->strProductDesc);
					//}
				}
			}
			if (bSendCloseMsg)
			{
				/*
				DWORD dwCount = 1;
				std::map<DWORD, DWORD>::iterator pIterProc = m_mapPidKillCount.find(pIter->dwProcessId);
				if (pIterProc == m_mapPidKillCount.end())
				{
					m_mapPidKillCount.insert(std::map<DWORD, DWORD>::value_type(pIter->dwProcessId, 1));
				}
				else
				{
					pIterProc->second += 1;
					dwCount = pIterProc->second;
				}
				if (dwCount < 3)
				{
					::PostMessage(pIter->hWnd, WM_CLOSE, NULL, NULL);
					theApp.AddLog(strType, pIter->strWinName, pIter->strClassName, pIter->procInfo.strProcessName, pIter->procInfo.strFileDesc, pIter->procInfo.strProductDesc);
				}
				*/
				::PostMessage(pIter->hWnd, WM_CLOSE, NULL, NULL);
				BOOL bClosed = FALSE;
				for (int i = 0; i < 20; i++)
				{
					if (!::IsWindow(pIter->hWnd))
					{
						bClosed = TRUE;
						break;
					}

					Sleep(100);
				}
				//if (bClosed)
				//{
				//	theApp.AddLog(strType, pIter->strWinName, pIter->strClassName, pIter->strProcessName, pIter->strFileDesc, pIter->strProductDesc);
				//}
			}
		}
	}
}

BOOL CScreenLockerDlg::OnDeviceChange(UINT nEventType, DWORD_PTR dwData)
{
	if (DBT_DEVICEARRIVAL == nEventType || DBT_DEVICEREMOVECOMPLETE == nEventType)
	{
		PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)dwData;
		switch (pHdr->dbch_devicetype)
		{
		case DBT_DEVTYP_DEVICEINTERFACE:
			{
				PDEV_BROADCAST_DEVICEINTERFACE pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)pHdr;
				if (DBT_DEVICEARRIVAL == nEventType)
				{
					if (IsWindow(m_hWnd))
					{
						//if (!theApp.m_bIgnoreDeviceChange)
						{
							if (GUID_DEVINTERFACE_DISK == pDevInf->dbcc_classguid ||
								GUID_DEVINTERFACE_WPD == pDevInf->dbcc_classguid ||
								GUID_DEVINTERFACE_USB_DEVICE == pDevInf->dbcc_classguid)
							{
								theApp.ControlUDisk();
							}
							else if (GUID_DEVINTERFACE_NET == pDevInf->dbcc_classguid)
							{
								theApp.ControlNetwork();
							}
							else
							{
								theApp.ControlDevice();
							}
						}
					}
				}
				else if (DBT_DEVICEREMOVECOMPLETE == nEventType)
				{
				}
			}
			break;
		case DBT_DEVTYP_HANDLE:
			{
				PDEV_BROADCAST_HANDLE pDevHnd = (PDEV_BROADCAST_HANDLE)pHdr;
			}
			break;
		case DBT_DEVTYP_OEM:
			{
				PDEV_BROADCAST_OEM pDevOem = (PDEV_BROADCAST_OEM)pHdr;
			}
			break;
		case DBT_DEVTYP_PORT:
			{
				PDEV_BROADCAST_PORT pDevPort = (PDEV_BROADCAST_PORT)pHdr;
			}
			break;
		case DBT_DEVTYP_VOLUME:
			{
				PDEV_BROADCAST_VOLUME pDevVolume = (PDEV_BROADCAST_VOLUME)pHdr;
			}
			break;
		default:
			break;
		}
	}
	return TRUE;
}

void CScreenLockerDlg::UpdateExplorerId()
{
	m_DataLock.Lock();
	std::vector<DWORD> vecPids;
	Utils::GetProcessId(_T("explorer.exe"), vecPids);
	for (int i = 0; i < (int)vecPids.size(); i++)
	{
		m_dwExplorerId = vecPids[i];
	}

	m_vecUDiskLetter.clear();
	TCHAR szDrive[MAX_PATH] = _T("A:\\");
	for (int i = 26 - 1; i >= 0; i--)
	{
		szDrive[0] = _T('A') + i;
		UINT uType = GetDriveType(szDrive);
		if (DRIVE_REMOVABLE == uType)
		{
			m_vecUDiskLetter.push_back(szDrive);
		}
	}
	m_DataLock.Unlock();
}
void CScreenLockerDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	CResizableDialog::OnHotKey(nHotKeyId, nKey1, nKey2);
}

void CScreenLockerDlg::OnBnClickedButtonShow()
{
	GetDlgItem(IDC_BUTTON_EXIT)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_EXIT)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT_EXIT)->ShowWindow(SW_SHOW);
}
