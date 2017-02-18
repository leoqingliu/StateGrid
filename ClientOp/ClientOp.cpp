
// ClientOp.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ClientOp.h"
#include "ClientOpDlg.h"
#include "PasswordDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClientOpApp

BEGIN_MESSAGE_MAP(CClientOpApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CClientOpApp construction

CClientOpApp::CClientOpApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CClientOpApp object

CClientOpApp theApp;

// CClientOpApp initialization

BOOL CClientOpApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("grabsun"));

	TCHAR szTmpPath[MAX_PATH] = {0};
	//	GetTempPath(MAX_PATH, szTmpPath);
	LPITEMIDLIST pidl;
	LPMALLOC pShellMalloc;
	if (SUCCEEDED(SHGetMalloc(&pShellMalloc)))
	{
		if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_APPDATA, &pidl)))
		{
			SHGetPathFromIDList(pidl, szTmpPath);
			pShellMalloc->Free(pidl);
		}
		pShellMalloc->Release();
	}
	if (0 == lstrlen(szTmpPath))
	{
		MessageBox(NULL, _T("获得临时目录失败!"), _T("ERROR"), MB_ICONERROR);
		return FALSE;
	}

	TCHAR szTmpLongPath[MAX_PATH] = {0};
	GetLongPathName(szTmpPath, szTmpLongPath, MAX_PATH);
	lstrcpy(szTmpPath, szTmpLongPath);
	if (szTmpPath[lstrlen(szTmpPath) - 1] != _T('\\'))
	{
		lstrcat(szTmpPath, _T("\\"));
	}
	lstrcat(szTmpPath, _T("MacMan\\"));
	BOOL bCreateDirRet = TRUE;
	if(CreateDirectory(szTmpPath, NULL) == FALSE)
	{
		DWORD dwError = GetLastError();
		if(dwError != ERROR_FILE_EXISTS && dwError != ERROR_ALREADY_EXISTS)
		{
			bCreateDirRet = FALSE;
		}
	}
	if (FALSE == bCreateDirRet)
	{
		MessageBox(NULL, _T("创建临时目录失败!"), _T("ERROR"), MB_ICONERROR);
		return FALSE;
	}
	m_strTmpPath = szTmpPath;

	m_strDllPath = m_strTmpPath + _T("CommuExt.dll");
	
	CPasswordDlg dlgPass;
	if (IDOK == dlgPass.DoModal())
	{
		if (dlgPass.m_strPassword != _T("685768op"))
		{
			AfxMessageBox(_T("密码错误!"), MB_ICONERROR);
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
	
	CClientOpDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
