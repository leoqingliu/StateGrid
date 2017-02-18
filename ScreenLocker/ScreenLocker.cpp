
// ScreenLocker.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ScreenLocker.h"
#include "ScreenLockerDlg.h"
#include "Hook.h"
#include <assert.h>
#include "../PipeProtocol.h"
#include "../Public/Utils.h"
#include "../Public/DevUtils.h"
#include "../Public/SvchostServiceModule.h"
#include "../Public/NetworkAdapterUtils.h"
#include "../Public/SystemUtils.h"
#include "../Defines.h"
#include "../MacConfig.h"

#include <WinIoCtl.h>
#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else 
#include <guiddef.h>
#endif
#include <Bthdef.h>
#include <Ntddpar.h>
#include <Ntddmodm.h>
#include <Portabledevice.h>
#include <devguid.h>
#include <Ndisguid.h>
// 1394.h
DEFINE_GUID(BUS1394_CLASS_GUID, 0x6BDD1FC1, 0x810F, 0x11d0, 0xBE, 0xC7, 0x08, 0x00, 0x2B, 0xE2, 0x09, 0x2F);
DEFINE_GUID(GUID_DEVICE_CDROM, 0x4d36e965, 0xe325, 0x11ce, 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18);
DEFINE_GUID(GUID_DEVICE_PCMCIA, 0x4d36e977, 0xe325, 0x11ce, 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18);
DEFINE_GUID(GUID_DEVICE_MULTIFUNCTION, 0x4d36e971, 0xe325, 0x11ce, 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18);
DEFINE_GUID(GUID_DEVICE_IRDA, 0x6bdd1fc5, 0x810f, 0x11d0, 0xbe, 0xc7, 0x08, 0x00, 0x2b, 0xe2, 0x09, 0x2f);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CScreenLockerApp

BEGIN_MESSAGE_MAP(CScreenLockerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CScreenLockerApp construction

CScreenLockerApp::CScreenLockerApp()
	:m_PipeServer(PIPE_NAME)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_bDiskWaitForConfirm = FALSE;
	m_bDiskAllow = FALSE;
}


// The one and only CScreenLockerApp object

CScreenLockerApp theApp;

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

VOID CScreenLockerApp::ControlKey(BOOL bEnable)
{
	// task keys (Alt-TAB etc)
	if (NULL != _DisableTaskKeys)
	{
		_DisableTaskKeys(!bEnable);

		// task bar
		HWND hwnd = ::FindWindow(_T("Shell_traywnd"), NULL);
		::EnableWindow(hwnd, bEnable);
	}
}

// CScreenLockerApp initialization

#if 0
BOOL GetModulePath(LPVOID pAddr, LPTSTR pModuleName)
{
	MEMORY_BASIC_INFORMATION mbi = {0};
	if(::VirtualQuery(pAddr, &mbi, sizeof(mbi)))
	{
		UINT_PTR hModule = (UINT_PTR)mbi.AllocationBase;
		::GetModuleFileName((HMODULE)hModule, pModuleName, MAX_PATH);

		return TRUE;
	}
	return FALSE;
}

BOOL GetExceptionFileName(PEXCEPTION_POINTERS pExInfo, LPTSTR pModuleName)
{
	LPVOID pAddr = pExInfo->ExceptionRecord->ExceptionAddress;
	return GetModulePath(pAddr, pModuleName);
}

LONG WINAPI CustomUnhandledExceptionFilter(PEXCEPTION_POINTERS pExInfo)
{
#if 0
	//
	TCHAR szTemp[1024];
	static TCHAR szInforDis[1024 * 100] = {0};
	static TCHAR szInforWrite[1024 * 100] = {0};
	wsprintf(szInforDis, _T("\n程序发生异常，错误信息:\n\n"));

	// Set Exception Description
	TCHAR szExceptionDesc[512] = {0};
	switch(pExInfo->ExceptionRecord->ExceptionCode)
	{
	case EXCEPTION_ACCESS_VIOLATION          : lstrcpy(szExceptionDesc, _T("EXCEPTION_ACCESS_VIOLATION        ")); break;
	case EXCEPTION_DATATYPE_MISALIGNMENT     : lstrcpy(szExceptionDesc, _T("EXCEPTION_DATATYPE_MISALIGNMENT   ")); break;
	case EXCEPTION_BREAKPOINT                : lstrcpy(szExceptionDesc, _T("EXCEPTION_BREAKPOINT              ")); break;
	case EXCEPTION_SINGLE_STEP               : lstrcpy(szExceptionDesc, _T("EXCEPTION_SINGLE_STEP             ")); break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED     : lstrcpy(szExceptionDesc, _T("EXCEPTION_ARRAY_BOUNDS_EXCEEDED   ")); break;
	case EXCEPTION_FLT_DENORMAL_OPERAND      : lstrcpy(szExceptionDesc, _T("EXCEPTION_FLT_DENORMAL_OPERAND    ")); break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO        : lstrcpy(szExceptionDesc, _T("EXCEPTION_FLT_DIVIDE_BY_ZERO      ")); break;
	case EXCEPTION_FLT_INEXACT_RESULT        : lstrcpy(szExceptionDesc, _T("EXCEPTION_FLT_INEXACT_RESULT      ")); break;
	case EXCEPTION_FLT_INVALID_OPERATION     : lstrcpy(szExceptionDesc, _T("EXCEPTION_FLT_INVALID_OPERATION   ")); break;
	case EXCEPTION_FLT_OVERFLOW              : lstrcpy(szExceptionDesc, _T("EXCEPTION_FLT_OVERFLOW            ")); break;
	case EXCEPTION_FLT_STACK_CHECK           : lstrcpy(szExceptionDesc, _T("EXCEPTION_FLT_STACK_CHECK         ")); break;
	case EXCEPTION_FLT_UNDERFLOW             : lstrcpy(szExceptionDesc, _T("EXCEPTION_FLT_UNDERFLOW           ")); break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO        : lstrcpy(szExceptionDesc, _T("EXCEPTION_INT_DIVIDE_BY_ZERO      ")); break;
	case EXCEPTION_INT_OVERFLOW              : lstrcpy(szExceptionDesc, _T("EXCEPTION_INT_OVERFLOW            ")); break;
	case EXCEPTION_PRIV_INSTRUCTION          : lstrcpy(szExceptionDesc, _T("EXCEPTION_PRIV_INSTRUCTION        ")); break;
	case EXCEPTION_IN_PAGE_ERROR             : lstrcpy(szExceptionDesc, _T("EXCEPTION_IN_PAGE_ERROR           ")); break;
	case EXCEPTION_ILLEGAL_INSTRUCTION       : lstrcpy(szExceptionDesc, _T("EXCEPTION_ILLEGAL_INSTRUCTION     ")); break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION  : lstrcpy(szExceptionDesc, _T("EXCEPTION_NONCONTINUABLE_EXCEPTION")); break;
	case EXCEPTION_STACK_OVERFLOW            : lstrcpy(szExceptionDesc, _T("EXCEPTION_STACK_OVERFLOW          ")); break;
	case EXCEPTION_INVALID_DISPOSITION       : lstrcpy(szExceptionDesc, _T("EXCEPTION_INVALID_DISPOSITION     ")); break;
	case EXCEPTION_GUARD_PAGE                : lstrcpy(szExceptionDesc, _T("EXCEPTION_GUARD_PAGE              ")); break;
	case EXCEPTION_INVALID_HANDLE            : lstrcpy(szExceptionDesc, _T("EXCEPTION_INVALID_HANDLE          ")); break;
		//case EXCEPTION_POSSIBLE_DEADLOCK       : lstrcpy(szExceptionDesc, _T("EXCEPTION_POSSIBLE_DEADLOCK       ")); break;
	case CONTROL_C_EXIT                      : lstrcpy(szExceptionDesc, _T("CONTROL_C_EXIT                    ")); break;
	default									 : lstrcpy(szExceptionDesc, _T("EXCEPTION_UNKNOWN                 ")); break;
	}

	// Get Exception Module File Name
	TCHAR szExceptionPath[MAX_PATH] = {0};
	GetExceptionFileName(pExInfo, szExceptionPath);
	wsprintf(szTemp, _T("文件名:\t\t%s\n"), szExceptionPath);
	lstrcat(szInforDis, szTemp);

	// Address
	wsprintf(szTemp, _T("地址:\t\t0x%08X\n"), pExInfo->ExceptionRecord->ExceptionAddress);
	lstrcat(szInforDis, szTemp);

	// Exception Type
	wsprintf(szTemp, _T("错误类型:\t%s\n"), szExceptionDesc);
	lstrcat(szInforDis, szTemp);

	//
	// Show
	//
	if (AfxMessageBox(szInforDis, MB_ICONERROR))
	{
	}
#endif
	return EXCEPTION_EXECUTE_HANDLER;
}
#endif

struct ThreadApplyConfigParam
{
	CScreenLockerApp *pApp;
};
DWORD WINAPI ThreadApplyConfig(LPVOID lpThreadParameter)
{
	ThreadApplyConfigParam *pParam = (ThreadApplyConfigParam *)lpThreadParameter;
	CScreenLockerApp *pApp = pParam->pApp;
	delete pParam;

	pApp->ControlUDisk();
	pApp->ControlDevice();
	pApp->ControlNetwork();
	return 0;
}

struct ThreadPipeConnectionParam
{
	CScreenLockerApp *pApp;
	NamedPipe *pPipe;
};
DWORD WINAPI ThreadPipeServerConnection(LPVOID lpThreadParameter)
{
	ThreadPipeConnectionParam *pParam = (ThreadPipeConnectionParam *)lpThreadParameter;
	NamedPipe *pPipe = pParam->pPipe;
	CScreenLockerApp *pApp = pParam->pApp;
	delete pParam;

	LPBYTE pbBuffer = new BYTE[1024 * 200];
	while (TRUE)
	{
		ZeroMemory(pbBuffer, 1024 * 200);
		size_t iDataLen = pPipe->Read(pbBuffer, 1024 * 200);
		if (0 == iDataLen)
		{
			break;
		}
		
		/*
		if (iDataLen == sizeof(pipe_protocol_t))
		{
			pipe_protocol_t *pProto = (pipe_protocol_t *)pbBuffer;
			if (pProto->dwProtocolId == PIPE_PROTOCOL_ID)
			{
				int d = 0;
			}
		}
		*/

		Json::Reader reader;
		Json::Value root;
		if (reader.parse((LPSTR)pbBuffer, root, false))
		{
			int iType = GetJsonInt(root, "Type", 0);
			std::string strTmp = GET_JSON_STRING(root, "Content", "");
			if (PIPE_TYPE_USB == iType)
			{
				pApp->m_InfoCS.Lock();
				BOOL bParsed = pApp->m_Info.LoadConfig(strTmp.c_str());
				if (bParsed)
				{
					ThreadApplyConfigParam *pApplyParam = new ThreadApplyConfigParam;
					pApplyParam->pApp = pApp;
					DWORD dwThreadId = 0;
					CreateThread(NULL, 0, ThreadApplyConfig, pApplyParam, 0, &dwThreadId);
				}
				pApp->m_InfoCS.Unlock();
			}
			else if (PIPE_TYPE_LOGOFF == iType)
			{
				SystemUtils::SystemShutdown(SystemUtils::SYSTEM_OP_TYPE_LOGOFF);
			}
			else if (PIPE_TYPE_LOCK == iType)
			{
				pApp->m_dequeLockInfoLock.Lock();
				pApp->m_dequeLockInfo.push_back(iType);
				pApp->m_dequeLockInfoLock.Unlock();
			}
			else if (PIPE_TYPE_UNLOCK == iType)
			{
				pApp->m_dequeLockInfoLock.Lock();
				pApp->m_dequeLockInfo.push_back(iType);
				pApp->m_dequeLockInfoLock.Unlock();
			}
			else if (PIPE_TYPE_INFO == iType)
			{
				pApp->m_InfoCS.Lock();
				BOOL bParsed = pApp->m_Info.LoadConfig(strTmp.c_str());
				if (bParsed)
				{
					ThreadApplyConfigParam *pApplyParam = new ThreadApplyConfigParam;
					pApplyParam->pApp = pApp;
					DWORD dwThreadId = 0;
					CreateThread(NULL, 0, ThreadApplyConfig, pApplyParam, 0, &dwThreadId);
				}
				pApp->m_InfoCS.Unlock();
			}
		}
		else
		{
		}
	}
	pPipe->Close();
	delete[] pbBuffer;
	delete pPipe;
	return 0;
}

DWORD WINAPI ThreadPipeServer(LPVOID lpThreadParameter)
{
	CScreenLockerApp *pApp = (CScreenLockerApp *)lpThreadParameter;
	while (TRUE)
	{
		NamedPipe *pPipe = pApp->m_PipeServer.Accept();
		if (NULL == pPipe)
		{
			break;
		}
		DWORD dwThread;
		ThreadPipeConnectionParam *pParam = new ThreadPipeConnectionParam;
		pParam->pApp = pApp;
		pParam->pPipe = pPipe;
		::CreateThread(NULL, 0, ThreadPipeServerConnection, pParam, 0, &dwThread);
	}
	return 0;
}

BOOL CScreenLockerApp::InitInstance()
{
	HANDLE hMutex = CreateMutex(NULL, TRUE, _T("AUTO_SCREEN_LOCKER_INSTANCE_MUTEXT"));
	if (hMutex)
	{
		if (ERROR_ALREADY_EXISTS == GetLastError())
		{
			//AfxMessageBox(_T("只能启动一个程序，请关闭之前的程序再试!"), MB_ICONERROR);
			return FALSE;
		}
	}

	WNDCLASS wc;  
	::GetClassInfo(AfxGetInstanceHandle(), _T("#32770"), &wc);
	wc.lpszClassName = _T("{46D29001-2748-4859-B945-ADB6531FDD58}");
	AfxRegisterClass(&wc);

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
		AfxMessageBox(_T("获得临时目录失败!"), MB_ICONERROR);
		return FALSE;
	}

	TCHAR szTmpLongPath[MAX_PATH] = {0};
	GetLongPathName(szTmpPath, szTmpLongPath, MAX_PATH);
	lstrcpy(szTmpPath, szTmpLongPath);
	if (szTmpPath[lstrlen(szTmpPath) - 1] != _T('\\'))
	{
		lstrcat(szTmpPath, _T("\\"));
	}
	lstrcat(szTmpPath, _T("MacManager\\"));
	BOOL bCreateDirRet = TRUE;
	if (CreateDirectory(szTmpPath, NULL) == FALSE)
	{
		DWORD dwError = GetLastError();
		if (dwError != ERROR_FILE_EXISTS && dwError != ERROR_ALREADY_EXISTS)
		{
			bCreateDirRet = FALSE;
		}
	}
	if (FALSE == bCreateDirRet)
	{
		AfxMessageBox(_T("创建临时目录失败!"), MB_ICONERROR);
		return FALSE;
	}
	m_strTmpPath = szTmpPath;
	USES_CONVERSION;

	// UDisk Path
	CString strUDiskPackPath;
	strUDiskPackPath.Format(_T("%sUDisk.7z"), m_strTmpPath);
	m_strDriverUDiskDir.Format(_T("%sUDisk"), m_strTmpPath);

	INT iUDiskResId = IDR_BIN_UDISK;
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	OSVERSIONINFOEX os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if (GetVersionEx((OSVERSIONINFO *)&os))
	{
		// https://msdn.microsoft.com/en-us/library/ms724832.aspx
		// Vista/Win7
		if (os.dwMajorVersion == 6)
		{
			if (os.dwMinorVersion < 2)
			{
				iUDiskResId = IDR_BIN_UDISK_SHA1;
			}
		}
		// XP/Windows Server 2003
		else if (os.dwMajorVersion == 5)
		{
			iUDiskResId = IDR_BIN_UDISK_SHA1;
		}
	}
	// UDisk
	BOOL bUDiskRet = Utils::ExtractCompressedFile((HINSTANCE)m_hInstance, MAKEINTRESOURCE(iUDiskResId), _T("BIN"), strUDiskPackPath, m_strDriverUDiskDir);
	if (!bUDiskRet)
	{
		CString strError;
		strError.Format(_T("释放U盘保护文件失败(Error: %d)，请检查磁盘空间和杀毒软件相关配置!"), GetLastError());
		AfxMessageBox(strError, MB_ICONERROR);
		return FALSE;
	}
	
	BOOL bRet;
	CString strDllPath;
	strDllPath.Format(_T("%sTaskKeyHook.dll"), m_strTmpPath);
#ifdef _WIN64
	int iKeyResId = IDR_DLL_HOOK_X64;
#else
	int iKeyResId = IDR_DLL_HOOK;
#endif
	bRet = Extractfile(
		(HINSTANCE)AfxGetApp()->m_hInstance,
		MAKEINTRESOURCE(iKeyResId), _T("DLL"), strDllPath
		);
	if (!bRet)
	{
		CString strError;
		strError.Format(_T("释放文件失败(Error: %d)，请检查磁盘空间和杀毒软件相关配置!"), GetLastError());
		AfxMessageBox(strError, MB_ICONERROR);
		return FALSE;
	}

	bRet = InitApiHookLibrary(strDllPath);
	if (!bRet)
	{
		CString strError;
		strError.Format(_T("加载DLL失败(Error: %d)，请检查磁盘空间和杀毒软件相关配置!"), GetLastError());
		AfxMessageBox(strError, MB_ICONERROR);
		return FALSE;
	}

	bRet = m_PipeServer.Create();
	if (!bRet)
	{
		CString strError;
		strError.Format(_T("创建PIPE失败，请联系管理员!"), GetLastError());
		AfxMessageBox(strError, MB_ICONERROR);
		return FALSE;
	}
	DWORD dwThreadId;
	::CreateThread(NULL, 0, ThreadPipeServer, this, 0, &dwThreadId);

	//ControlKey(FALSE);

	CScreenLockerDlg dlg;
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

int CScreenLockerApp::ExitInstance()
{
	ControlKey(TRUE);

	return CWinApp::ExitInstance();
}

BOOL CScreenLockerApp::ControlUDisk()
{
	UDISK_TYPE eType = (UDISK_TYPE)m_Info.configInfo.m_uUDiskType;
	BOOL bRet = TRUE;
	switch (eType)
	{
	case UDISK_TYPE_DISABLE:
		{
			std::vector<CString> vecIds;
			CDevUtils::ControlUDisk(FALSE, FALSE, vecIds);
			if (CSvchostServiceModule::IsRunning(FLT_SERVICE_NAME))
			{
				bRet = bRet && theApp.m_uDiskUtils.Control(FLT_SERVICE_NAME, FALSE);
				bRet = bRet && theApp.m_uDiskUtils.Control(UGUARD_SERVICE_NAME, FALSE);
			}
		}
		break;
	case UDISK_TYPE_SPEC:
		{
			std::vector<CString> vecIds = m_Info.configInfo.m_ItemsUDiskIds.vecItems;
			CDevUtils::ControlUDisk(TRUE, TRUE, vecIds);
			if (CSvchostServiceModule::IsRunning(FLT_SERVICE_NAME))
			{
				bRet = bRet && theApp.m_uDiskUtils.Control(FLT_SERVICE_NAME, FALSE);
				bRet = bRet && theApp.m_uDiskUtils.Control(UGUARD_SERVICE_NAME, FALSE);
			}
		}
		break;
	case UDISK_TYPE_WRITEPROTECT:
	case UDISK_TYPE_PASSWD:
		{
			std::vector<CString> vecIds;
			CDevUtils::ControlUDisk(TRUE, FALSE, vecIds);
			int iOpenResult = theApp.m_uDiskUtils.Open();
			if (iOpenResult <= 0)
			{
				if (!CSvchostServiceModule::IsInstalled(FLT_SERVICE_NAME) ||
					!CSvchostServiceModule::IsInstalled(UGUARD_SERVICE_NAME))
				{
					bRet = bRet && theApp.m_uDiskUtils.Install(GetDesktopWindow(), theApp.m_strDriverUDiskDir);
				}

				if (bRet)
				{
					if (!CSvchostServiceModule::IsRunning(FLT_SERVICE_NAME))
					{
						BOOL bCheck = theApp.m_uDiskUtils.Control(FLT_SERVICE_NAME, TRUE);
						if (bCheck)
						{
							for (int i = 0; i < 10 * 5; i++)
							{
								if (CSvchostServiceModule::IsRunning(FLT_SERVICE_NAME))
								{
									break;
								}
								Sleep(100);
							}
						}
					}

					if (!CSvchostServiceModule::IsRunning(UGUARD_SERVICE_NAME))
					{
						BOOL bCheck = theApp.m_uDiskUtils.Control(UGUARD_SERVICE_NAME, TRUE);
						if (bCheck)
						{
							for (int i = 0; i < 10 * 5; i++)
							{
								if (CSvchostServiceModule::IsRunning(UGUARD_SERVICE_NAME))
								{
									break;
								}
								Sleep(100);
							}
						}
					}
					iOpenResult = theApp.m_uDiskUtils.Open();
					if (iOpenResult <= 0)
					{
						bRet = FALSE;
					}
				}
			}

			//bRet = bRet && theApp.m_uDiskUtils.Open();
			//bRet = theApp.m_uDiskUtils.SetForbidden(FALSE);
			BOOL bForbidden = (UDISK_TYPE_WRITEPROTECT == m_Info.configInfo.m_uUDiskType ? TRUE : FALSE);
			bRet = bRet && theApp.m_uDiskUtils.SetForbidden(bForbidden);
			if (!bRet)
			{
				AfxMessageBox(_T("设置U盘模式失败!"), MB_ICONERROR);
			}

			theApp.m_bDiskAllow = (UDISK_TYPE_WRITEPROTECT == m_Info.configInfo.m_uUDiskType ? FALSE : TRUE);
		}

		break;
	case UDISK_TYPE_COPYTOCOMPUTER:
		{
		}
		break;
	case UDISK_TYPE_ENABLE:
		{
			std::vector<CString> vecIds;
			CDevUtils::ControlUDisk(TRUE, FALSE, vecIds);
			if (CSvchostServiceModule::IsRunning(FLT_SERVICE_NAME))
			{
				bRet = bRet && theApp.m_uDiskUtils.Control(FLT_SERVICE_NAME, FALSE);
				bRet = bRet && theApp.m_uDiskUtils.Control(UGUARD_SERVICE_NAME, FALSE);
			}
		}
		break;
	default:
		break;
	}

	return bRet;
}

BOOL CScreenLockerApp::ControlDevice(BOOL bForceEnable /*= FALSE*/)
{
	BOOL bCheckCD = m_Info.configInfo.m_bCheckCD;
	BOOL bCheckFloppy = m_Info.configInfo.m_bCheckFloppy;
	BOOL bCheckBlueTooth = m_Info.configInfo.m_bCheckBlueTooth;
	BOOL bCheckSerial = m_Info.configInfo.m_bCheckSerial;
	BOOL bCheck1394 = m_Info.configInfo.m_bCheck1394;
	BOOL bCheckIrda = m_Info.configInfo.m_bCheckIrda;
	BOOL bCheckPcmcia = m_Info.configInfo.m_bCheckPcmcia;
	BOOL bCheckModem = m_Info.configInfo.m_bCheckModem;
	BOOL bCheckDeviceCell = m_Info.configInfo.m_bCheckDeviceCell;
	
	if (bForceEnable)
	{
		bCheckCD = FALSE;
		bCheckFloppy = FALSE;
		bCheckBlueTooth = FALSE;
		bCheckSerial = FALSE;
		bCheck1394 = FALSE;
		bCheckIrda = FALSE;
		bCheckPcmcia = FALSE;
		bCheckModem = FALSE;
		bCheckDeviceCell = FALSE;
	}
	
	//CDevUtils::ControlDevices(GUID_DEVINTERFACE_CDROM, bEnable);
	CDevUtils::ControlDevices(GUID_DEVICE_CDROM, !bCheckCD, FALSE);

	CDevUtils::ControlDevices(GUID_DEVINTERFACE_FLOPPY, !bCheckFloppy);

	CDevUtils::ControlDevices(GUID_BTHPORT_DEVICE_INTERFACE, !bCheckBlueTooth);

	CDevUtils::ControlDevices(GUID_DEVINTERFACE_COMPORT, !bCheckSerial);
	CDevUtils::ControlDevices(GUID_DEVINTERFACE_PARALLEL, !bCheckSerial);
	CDevUtils::ControlDevices(GUID_DEVINTERFACE_PARCLASS, !bCheckSerial);

	CDevUtils::ControlDevices(BUS1394_CLASS_GUID, !bCheck1394);

	CDevUtils::ControlDevices(GUID_DEVICE_IRDA, !bCheckIrda, FALSE);

	CDevUtils::ControlDevices(GUID_DEVICE_PCMCIA, !bCheckPcmcia, FALSE);
	CDevUtils::ControlDevices(GUID_DEVICE_MULTIFUNCTION, !bCheckPcmcia, FALSE);

	CDevUtils::ControlDevices(GUID_DEVINTERFACE_MODEM, !bCheckModem);

	CDevUtils::ControlDevices(GUID_DEVINTERFACE_WPD, !bCheckDeviceCell);
	CDevUtils::ControlDevices(GUID_DEVINTERFACE_WPD_PRIVATE, !bCheckDeviceCell);

	return TRUE;
}

BOOL CScreenLockerApp::ControlNetwork(BOOL bForceEnable /*= FALSE*/)
{
	CONFIG_INFO_ITEMS ItemsNetworkMini = m_Info.configInfo.m_ItemsNetworkMini;
	CONFIG_INFO_ITEMS ItemsNetworkWired = m_Info.configInfo.m_ItemsNetworkWired;
	CONFIG_INFO_ITEMS ItemsNetworkWireless = m_Info.configInfo.m_ItemsNetworkWireless;
	
	std::vector<CNetworkAdapterUtils::ADAPTER_TYPE_INFO> vecInfos;
	struct _INFO_
	{
		CONFIG_INFO_ITEMS *pItem;
		CNetworkAdapterUtils::ADAPTER_TYPE eType;
	};
	_INFO_ infos[] = 
	{
		{&ItemsNetworkMini, CNetworkAdapterUtils::ADAPTER_TYPE_MINI},
		{&ItemsNetworkWired, CNetworkAdapterUtils::ADAPTER_TYPE_WIRED},
		{&ItemsNetworkWireless, CNetworkAdapterUtils::ADAPTER_TYPE_WIRELESS}
	};
	for (int i = 0; i < _countof(infos); i++)
	{
		CONFIG_INFO_ITEMS *pItem = infos[i].pItem;
		for (int j = 0; j < (int)pItem->vecItems.size(); j++)
		{
			if (0 == lstrlen(pItem->vecItems[j].szDesc))
			{
				continue;
			}

			CNetworkAdapterUtils::ADAPTER_TYPE_INFO info;
			info.eType = infos[i].eType;
			info.strDesc = pItem->vecItems[j].szDesc;
			vecInfos.push_back(info);
		}
	}
	CNetworkAdapterUtils network;
	std::vector<CNetworkAdapterUtils::ADAPTER_INFO> vecInfo;
	BOOL bAdapter = network.GetNetworkAdapter(vecInfo, vecInfos);
	if (bAdapter)
	{
		//	IF_TYPE_IEEE80211;
		//	MIB_IF_TYPE_ETHERNET;
		for (int i = 0; i < (int)vecInfo.size(); i++)
		{
			if (CNetworkAdapterUtils::ADAPTER_TYPE_WIRED == vecInfo[i].eType)
			{
				BOOL bEnable = !ItemsNetworkWired.bDisable;
				if (bForceEnable)
				{
					bEnable = TRUE;
				}
				CDevUtils::ControlDevices(GUID_DEVINTERFACE_NET,	bEnable, TRUE, vecInfo[i].strDeviceId);
				CDevUtils::ControlDevices(UNSPECIFIED_NETWORK_GUID, bEnable, TRUE, vecInfo[i].strDeviceId);
				CDevUtils::ControlDevices(GUID_NDIS_LAN_CLASS,		bEnable, TRUE, vecInfo[i].strDeviceId);
			}
			else if (CNetworkAdapterUtils::ADAPTER_TYPE_WIRELESS == vecInfo[i].eType)
			{
				BOOL bEnable = !ItemsNetworkWireless.bDisable;
				if (bForceEnable)
				{
					bEnable = TRUE;
				}
				CDevUtils::ControlDevices(GUID_DEVINTERFACE_NET,	bEnable, TRUE, vecInfo[i].strDeviceId);
				CDevUtils::ControlDevices(UNSPECIFIED_NETWORK_GUID, bEnable, TRUE, vecInfo[i].strDeviceId);
				CDevUtils::ControlDevices(GUID_NDIS_LAN_CLASS,		bEnable, TRUE, vecInfo[i].strDeviceId);
			}
			else if (CNetworkAdapterUtils::ADAPTER_TYPE_MINI == vecInfo[i].eType)
			{
				BOOL bEnable = !ItemsNetworkMini.bDisable;
				if (bForceEnable)
				{
					bEnable = TRUE;
				}
				CDevUtils::ControlDevices(GUID_DEVINTERFACE_NET,	bEnable, TRUE, vecInfo[i].strDeviceId);
				CDevUtils::ControlDevices(UNSPECIFIED_NETWORK_GUID, bEnable, TRUE, vecInfo[i].strDeviceId);
				CDevUtils::ControlDevices(GUID_NDIS_LAN_CLASS,		bEnable, TRUE, vecInfo[i].strDeviceId);
			}
		}
	}

	return TRUE;
}
