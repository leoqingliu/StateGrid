#include "StdAfx.h"
#include "SvchostServiceModule.h"
#include <wincred.h>
#include <wincrypt.h>
#include <fcntl.h>
#include <tlhelp32.h>
#include <io.h>
#include <stdio.h>
#include <fstream>
using namespace std;
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Advapi32.lib")
#include "Debug.h"

//LPCTSTR SZ_DESKTOP_WINLOGON	= _T("Winlogon");
//LPCTSTR SZ_DESKTOP_DEFAULT	= _T("default");
//LPCTSTR SZ_SVCHOST_GROUP		= _T("MacMan");

CSvchostServiceModule gSvchostServiceModule;

#if _MSC_VER <= 1200
#pragma warning(push)
#pragma warning(disable:4996)
#pragma warning(disable:4786)
#define SERVICE_ACCEPT_SESSIONCHANGE	0x00000080
#define SERVICE_CONTROL_SESSIONCHANGE	0x0000000E
#define WINSTA_ALL_ACCESS           (WINSTA_ENUMDESKTOPS  | WINSTA_READATTRIBUTES  | WINSTA_ACCESSCLIPBOARD | \
	WINSTA_CREATEDESKTOP | WINSTA_WRITEATTRIBUTES | WINSTA_ACCESSGLOBALATOMS | \
	WINSTA_EXITWINDOWS   | WINSTA_ENUMERATE       | WINSTA_READSCREEN)
#endif

void ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
//	MessageBox(NULL, _T("ServiceMain"), _T(""), MB_ICONINFORMATION);
	gSvchostServiceModule._ServiceMain(dwArgc, lpszArgv);
}

static DWORD WINAPI _HandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
	return gSvchostServiceModule.HandlerEx(dwControl, dwEventType, lpEventData);
}

CSvchostServiceModule::CSvchostServiceModule()
	:m_pRoutine(NULL)
	,m_pParam(NULL)
{
}

CSvchostServiceModule::~CSvchostServiceModule()
{
}

BOOL CSvchostServiceModule::IsInstalled(LPCTSTR pszServiceName)
{
	BOOL bResult = FALSE;
	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, STANDARD_RIGHTS_READ);
	if (hSCM != NULL)
	{
		SC_HANDLE hService = ::OpenService(hSCM, pszServiceName, SERVICE_QUERY_CONFIG);
		if (hService != NULL)
		{
			bResult = TRUE;
			::CloseServiceHandle(hService);
		}
		::CloseServiceHandle(hSCM);
	}
	return bResult;
}

BOOL CSvchostServiceModule::Install(LPCTSTR pszServiceName, LPTSTR lpDescription, LPCTSTR pszDllPath, LPCTSTR pszSvchost)
{
	if (IsInstalled(pszServiceName))
		return TRUE;

	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCM == NULL)
	{
		OutputLog(_T("Couldn't open service manager, %d"), GetLastError());
		return FALSE;
	}

	// Get the executable file path
	TCHAR szFilePath[_MAX_PATH];
	//::GetModuleFileName(NULL, szFilePath, _MAX_PATH);
	lstrcpy(szFilePath, _T("%SystemRoot%\\System32\\svchost.exe -k "));
	lstrcat(szFilePath, pszSvchost);

	SC_HANDLE hService = ::CreateService(
		hSCM, pszServiceName, pszServiceName,
		SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
		SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
		szFilePath, NULL, NULL, NULL, NULL, NULL);
	if (hService == NULL)
	{
		::CloseServiceHandle(hSCM);
		OutputLog(_T("Couldn't create service"));
		return FALSE;
	}

	SERVICE_DESCRIPTION	seDescription;
	seDescription.lpDescription = lpDescription;
	ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, (LPVOID)&seDescription);

	::CloseServiceHandle(hService);
	::CloseServiceHandle(hSCM);


	HKEY hRootKey;
	BOOL bOK = FALSE;
	while(1)
	{
		//
		// Create Another SVCHOST Group
		//
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\SvcHost"),
			0, KEY_ALL_ACCESS, &hRootKey) != ERROR_SUCCESS)
		{
			OutputLog(_T("Install:RegOpenKeyEx Failed"));
			break;
		}

		TCHAR szSafeBox[MAX_PATH] = {0};
		lstrcpy(szSafeBox, pszSvchost);

		TCHAR szValue[MAX_PATH] = {0};
		DWORD dwValueLen = MAX_PATH;
		DWORD dwType = REG_MULTI_SZ;
		if (RegQueryValueEx(hRootKey, szSafeBox, 0, &dwType, (LPBYTE)szValue, &dwValueLen) != ERROR_SUCCESS ||
			lstrcmpi(szValue, pszServiceName) != 0)
		{
			OutputLog(_T("Install:Don't Exist, Set Value"));
			if (RegSetValueEx(hRootKey, szSafeBox, 0, REG_MULTI_SZ,
				(CONST BYTE*)pszServiceName,
				lstrlen(pszServiceName) * sizeof(TCHAR)) != ERROR_SUCCESS)
			{
				OutputLog(_T("Install:Set Value Failed"));
				break;
			}
		}
		else
		{
			OutputLog(_T("Install:Exist"));
		}
		RegCloseKey(hRootKey);

		//
		// Set DLL
		//
		HKEY hCreateKey;
		TCHAR szServicePath[MAX_PATH] = {0};
		wsprintf(szServicePath, _T("SYSTEM\\CurrentControlSet\\Services\\%s"), pszServiceName);

		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szServicePath, 0, KEY_ALL_ACCESS, &hRootKey) != ERROR_SUCCESS)
		{
			break;
		}

		if (RegCreateKey(hRootKey, _T("Parameters"), &hCreateKey) != ERROR_SUCCESS)
		{
			break;
		}

		if (RegSetValueEx(hCreateKey, _T("ServiceDll"), 0, REG_EXPAND_SZ,
			(unsigned char*)pszDllPath, lstrlen(pszDllPath) * sizeof(TCHAR)) != ERROR_SUCCESS)
		{
			break;
		}

		//
		DWORD dwServiceDllUnloadOnStop = 0;
		if (RegSetValueEx(hCreateKey, _T("ServiceDllUnloadOnStop"), 0, REG_DWORD,
			(LPBYTE)&dwServiceDllUnloadOnStop, sizeof(DWORD)) != ERROR_SUCCESS)
		{
			break;
		}


		RegCloseKey(hCreateKey);
		RegCloseKey(hRootKey);
		bOK = TRUE;
		break;
	}
	return bOK;
}

BOOL CSvchostServiceModule::Uninstall(LPCTSTR pszServiceName, LPCTSTR pszSvchost)
{
	if (!IsInstalled(pszServiceName))
		return TRUE;

	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCM == NULL)
	{
		OutputLog(_T("Couldn't open service manager"));
		return FALSE;
	}

	SC_HANDLE hService = ::OpenService(hSCM, pszServiceName, SERVICE_STOP | DELETE);
	if (hService == NULL)
	{
		::CloseServiceHandle(hSCM);
		OutputLog(_T("Couldn't open service"));
		return FALSE;
	}
	SERVICE_STATUS status;
	::ControlService(hService, SERVICE_CONTROL_STOP, &status);

	BOOL bDelete = ::DeleteService(hService);
	::CloseServiceHandle(hService);
	::CloseServiceHandle(hSCM);

	//
	// Create Another SVCHOST Group
	//
	HKEY hRootKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\SvcHost"),
		0, KEY_ALL_ACCESS, &hRootKey) != ERROR_SUCCESS)
	{
		if (RegDeleteValue(hRootKey, pszSvchost) != ERROR_SUCCESS)
		{
		}
		else
		{
		}
		RegCloseKey(hRootKey);
	}

	if (bDelete)
	{
		OutputLog(_T("Service could has deleted"));
		return TRUE;
	}

	OutputLog(_T("Service could not be deleted"));
	return FALSE;
}

BOOL CSvchostServiceModule::Control(LPCTSTR pszServiceName, BOOL bStart /*= TRUE*/, BOOL bStopDependencies /*= FALSE*/)
{
	//Try the following codes to stop/start a service
	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	DWORD dwAccess = SERVICE_STOP|SERVICE_START;
	if (bStopDependencies)
	{
		dwAccess |= SERVICE_ENUMERATE_DEPENDENTS;
	}
	SC_HANDLE hService = ::OpenService(hSCM, pszServiceName, dwAccess);
	SERVICE_STATUS status;
	BOOL bRet;
	if (bStart)
	{
		if (IsRunning(pszServiceName))
		{
			bRet = TRUE;
		}
		else
		{
			bRet = ::StartService(hService, 0, NULL);
		}
	}
	else
	{
		if (IsRunning(pszServiceName))
		{
			if (bStopDependencies)
			{
				DWORD dwBytesNeeded;
				DWORD dwCount;
				LPENUM_SERVICE_STATUS   lpDependencies = NULL;
				// Pass a zero-length buffer to get the required buffer size
				if (EnumDependentServices(hService, SERVICE_ACTIVE, lpDependencies, 0, &dwBytesNeeded, &dwCount))
				{
					// If the Enum call succeeds, then there are no dependent
					// services so do nothing
				}
				else
				{
					if (GetLastError() != ERROR_MORE_DATA)
						return FALSE;

					// Allocate a buffer for the dependencies
					lpDependencies = (LPENUM_SERVICE_STATUS) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytesNeeded);
					if (!lpDependencies)
						return FALSE;

					SC_HANDLE               hDepService;
					__try
					{
						// Enumerate the dependencies
						if (!EnumDependentServices(hService, SERVICE_ACTIVE, lpDependencies, dwBytesNeeded, &dwBytesNeeded, &dwCount))
							return FALSE;

						DWORD i;
						for ( i = 0; i < dwCount; i++ )
						{
							ENUM_SERVICE_STATUS ess = *(lpDependencies + i);

							// Open the service
							hDepService = OpenService(hSCM, ess.lpServiceName, SERVICE_STOP | SERVICE_QUERY_STATUS);
							if (!hDepService)
								return FALSE;

							__try {

								// Send a stop code
								bRet = ControlService( hDepService, SERVICE_CONTROL_STOP, &status);
							}
							__finally
							{
								// Always release the service handle
								CloseServiceHandle(hDepService);
							}
						}

					}
					__finally
					{
						// Always free the enumeration buffer
						HeapFree( GetProcessHeap(), 0, lpDependencies );
					}
				}
			}
			bRet = ::ControlService(hService, SERVICE_CONTROL_STOP, &status);
		}
		else
		{
			bRet = TRUE;
		}
	}
	::CloseServiceHandle(hService);
	::CloseServiceHandle(hSCM);
	return bRet;
}

BOOL CSvchostServiceModule::Query(LPCTSTR pszServiceName, SERVICE_STATUS& status)
{
	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, STANDARD_RIGHTS_READ);
	if (NULL == hSCM)
	{
		return FALSE;
	}

	SC_HANDLE hService = ::OpenService(hSCM, pszServiceName, SERVICE_QUERY_STATUS);
	if (NULL == hService)
	{
		CloseServiceHandle(hSCM);
		return FALSE;
	}

	BOOL bQuery = QueryServiceStatus(hService, &status);
	CloseServiceHandle(hSCM);
	CloseServiceHandle(hService);
	if (!bQuery)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CSvchostServiceModule::IsRunning(LPCTSTR pszServiceName)
{
	SERVICE_STATUS status;
	BOOL bRunning = FALSE;
	if (Query(pszServiceName, status))
	{
		if (SERVICE_RUNNING == status.dwCurrentState)
		{
			bRunning = TRUE;
		}
	}
	return bRunning;
}

BOOL CSvchostServiceModule::SetStartMode(LPCTSTR pszServiceName, BOOL bAuto)
{
	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (NULL == hSCM)
	{
		return FALSE;
	}

	SC_HANDLE hService = ::OpenService(hSCM, pszServiceName, SERVICE_QUERY_STATUS | SERVICE_CHANGE_CONFIG);
	if (NULL == hService)
	{
		CloseServiceHandle(hSCM);
		return FALSE;
	}

	BOOL bSet = ChangeServiceConfig(
		hService,				// handle of service
		SERVICE_NO_CHANGE,		// service type: no change
		bAuto ? SERVICE_AUTO_START : SERVICE_DEMAND_START,
								// service start type
		SERVICE_NO_CHANGE,		// error control: no change
		NULL,					// binary path: no change
		NULL,					// load order group: no change
		NULL,					// tag ID: no change
		NULL,					// dependencies: no change
		NULL,					// account name: no change
		NULL,					// password: no change
		NULL					// display name: no change
		);

	CloseServiceHandle(hSCM);
	CloseServiceHandle(hService);
	if (!bSet)
	{
		return FALSE;
	}

	return bSet;
}

BOOL CSvchostServiceModule::GetStartMode(LPCTSTR pszServiceName, BOOL& bAuto)
{
	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, STANDARD_RIGHTS_READ);
	if (NULL == hSCM)
	{
		return FALSE;
	}

	SC_HANDLE hService = ::OpenService(hSCM, pszServiceName, SERVICE_QUERY_STATUS | SERVICE_QUERY_CONFIG);
	if (NULL == hService)
	{
		CloseServiceHandle(hSCM);
		return FALSE;
	}

	BOOL bFetch = FALSE;
	DWORD dwBytes = 0;
	QueryServiceConfig(hService, NULL, 0, &dwBytes);
	LPQUERY_SERVICE_CONFIG pConfig = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LPTR, dwBytes);
	if (NULL != pConfig)
	{
		bFetch = QueryServiceConfig(hService, pConfig, dwBytes, &dwBytes);
		if (bFetch)
		{
			if (SERVICE_AUTO_START == pConfig->dwStartType)
			{
				bAuto = TRUE;
			}
			else if (SERVICE_DEMAND_START == pConfig->dwStartType)
			{
				bAuto = FALSE;
			}
			else
			{
				bFetch = FALSE;
			}
		}
		LocalFree(pConfig);
	}

	CloseServiceHandle(hSCM);
	CloseServiceHandle(hService);
	if (!bFetch)
	{
		return FALSE;
	}

	return bFetch;
}

#if 0
BOOL CSvchostServiceModule::IsCallFromSCM()
{
	HANDLE hProcessSnap = NULL;
	PROCESSENTRY32 pe32 = {0};

	//
	// Take a snapshot of all processes in the system.
	//
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hProcessSnap == (HANDLE)-1)
	{
		return 0;
	}

	//
	// Fill in the size of the structure before using it.
	//
	pe32.dwSize = sizeof(PROCESSENTRY32);

	//
	// Walk the snapshot of the processes, and for each process,
	// display information.
	//
	DWORD dwServices = 0;
	DWORD dwParent = 1;
	if(Process32First(hProcessSnap, &pe32))
	{
		while(1)
		{
			if(lstrcmpi(pe32.szExeFile, _T("services.exe")) == 0)
			{
				dwServices = pe32.th32ProcessID;
			}

			if(pe32.th32ProcessID == GetCurrentProcessId())
			{
				dwParent = pe32.th32ParentProcessID;
			}

			if(!Process32Next(hProcessSnap, &pe32))
			{
				break;
			}
		}

	}
	return dwServices == dwParent;
}
#endif

#include <TlHelp32.h>
VOID CSvchostServiceModule::GetDllPath(LPTSTR pDllPath, LPCTSTR pDllName, DWORD tPid)
{
	MODULEENTRY32 me32;
	HANDLE snapMod = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, tPid);
	me32.dwSize = sizeof(MODULEENTRY32);
	if (Module32First(snapMod, &me32))
	{
		do
		{
			if (lstrcmpi(pDllName, me32.szModule) == 0)
			{
				lstrcpy(pDllPath, me32.szExePath);
				break;
			}
		}
		while(Module32Next(snapMod,&me32));
	}
	CloseHandle(snapMod);
}

//////////////////////////////////////////////////////////////////////////

void CSvchostServiceModule::Init(LPCTSTR pszServiceName,
								 LPCALLBACK_ROUTINE pRoutine /*= NULL*/, LPVOID pParam /*= NULL*/)
{
	lstrcpy(m_szServiceName, pszServiceName);

	// set up the initial service status
	m_hServiceStatus = NULL;
	m_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	m_status.dwCurrentState = SERVICE_STOPPED;
	m_status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SESSIONCHANGE | SERVICE_ACCEPT_SHUTDOWN;
	m_status.dwWin32ExitCode = 0;
	m_status.dwServiceSpecificExitCode = 0;
	m_status.dwCheckPoint = 0;
	m_status.dwWaitHint = 0;

	m_pRoutine = pRoutine;
	m_pParam = pParam;
}

void CSvchostServiceModule::_ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
//	MessageBox(NULL, _T("_ServiceMain"), _T(""), MB_ICONINFORMATION);
	OutputLog(_T("_ServiceMain Start"));
	// Register the control request handler
	m_status.dwCurrentState = SERVICE_START_PENDING;
//	OutputLog(_T("00");
#if _MSC_VER > 1200
	m_hServiceStatus = ::RegisterServiceCtrlHandlerEx(m_szServiceName, _HandlerEx, NULL);
//	OutputLog(_T("000");
#else
	HMODULE hInstAdvapi32 = LoadLibrary(_T("Advapi32.dll"));
	if(!hInstAdvapi32)
	{
		return;
	}

	typedef SERVICE_STATUS_HANDLE (WINAPI *RegisterServiceCtrlHandlerExPROC)(
		LPCTSTR lpServiceName,
		LPHANDLER_FUNCTION_EX lpHandlerProc,
		LPVOID lpContext
		);
#ifdef UNICODE
	LPSTR pFunctionName = "RegisterServiceCtrlHandlerExW";
#else
	LPSTR pFunctionName = "RegisterServiceCtrlHandlerExA";
#endif

	RegisterServiceCtrlHandlerExPROC RegisterServiceCtrlHandlerEx_Custom_Proc = NULL;
	RegisterServiceCtrlHandlerEx_Custom_Proc = (RegisterServiceCtrlHandlerExPROC)GetProcAddress(hInstAdvapi32, pFunctionName);
	if (!RegisterServiceCtrlHandlerEx_Custom_Proc)
	{
		FreeLibrary(hInstAdvapi32);
		return;
	}
	m_hServiceStatus = RegisterServiceCtrlHandlerEx_Custom_Proc(m_szServiceName, _HandlerEx, NULL);
	FreeLibrary(hInstAdvapi32);
#endif
//	OutputLog(_T("1");
	if (m_hServiceStatus == NULL)
	{
		OutputLog(_T("Handler not installed"));
		return;
	}
	SetServiceStatus(SERVICE_START_PENDING);

//	OutputLog(_T("2");
	m_status.dwWin32ExitCode = S_OK;
	m_status.dwCheckPoint = 0;
	m_status.dwWaitHint = 0;

	// When the Run function returns, the service has stopped.
//	OutputLog(_T("3");
	SetServiceStatus(SERVICE_RUNNING);
	Run();
//	OutputLog(_T("4");
	SetServiceStatus(SERVICE_STOPPED);
}

DWORD CSvchostServiceModule::HandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData)
{
	#if _MSC_VER <= 1200
	#define SERVICE_CONTROL_PRESHUTDOWN 0x0000000F
	#endif
	DWORD dwRet = ERROR_CALL_NOT_IMPLEMENTED;
	switch (dwControl)
	{
	case SERVICE_CONTROL_PRESHUTDOWN:
	case SERVICE_CONTROL_SHUTDOWN:
	case SERVICE_CONTROL_STOP:
		SetServiceStatus(SERVICE_STOP_PENDING);
		if (m_pRoutine)
		{
			m_pRoutine(CALLBACK_TYPE_STOP, m_pParam, NULL, NULL);
		}
		dwRet = NO_ERROR;
		break;
	case SERVICE_CONTROL_PAUSE:
		break;
	case SERVICE_CONTROL_CONTINUE:
		break;
	case SERVICE_CONTROL_INTERROGATE:
		dwRet = NO_ERROR;
		break;
	case SERVICE_CONTROL_SESSIONCHANGE:
		dwRet = NO_ERROR;
		break;
	case SERVICE_CONTROL_POWEREVENT:
		break;
	case SERVICE_CONTROL_DEVICEEVENT:
		if (m_pRoutine)
		{
			m_pRoutine(CALLBACK_TYPE_DEV, m_pParam, (LPVOID)dwEventType, lpEventData);
		}
		break;
	default:
		break;
	}
	::SetServiceStatus(m_hServiceStatus, &m_status);
	return dwRet;
}


void CSvchostServiceModule::SetServiceStatus(DWORD dwState)
{
	m_status.dwCurrentState = dwState;
	::SetServiceStatus(m_hServiceStatus, &m_status);
}

BOOL CSvchostServiceModule::Run()
{
	dwThreadID = ::GetCurrentThreadId();
	OutputLog(_T("Run Start..."));

#if 0
	BOOL bRet = FALSE;
	HWINSTA hwinstaCurrent = NULL;
	HDESK hdeskCurrent = NULL;
	HWINSTA hwinsta = NULL;
	HDESK hdesk = NULL;
	do
	{
		hwinstaCurrent = GetProcessWindowStation();
		if (hwinstaCurrent == NULL)
		{
			OutputLog(_T("Get Window Station Error");
			break;
		}

		hdeskCurrent = GetThreadDesktop(GetCurrentThreadId());
		if (hdeskCurrent == NULL)
		{
			OutputLog(_T("Get Window Desktop Error");
			break;
		}

		hwinsta = OpenWindowStation(TEXT("winsta0"), FALSE, WINSTA_ALL_ACCESS);
		if (hwinsta == NULL)
		{
			OutputLog(_T("Open Window Station Error");
			break;
		}

		if (!SetProcessWindowStation(hwinsta))
		{
			OutputLog(_T("Set Window Station Error");
			break;
		}

#define DESKTOP_OPTION		DESKTOP_CREATEMENU| DESKTOP_CREATEWINDOW |\
		DESKTOP_ENUMERATE			| DESKTOP_HOOKCONTROL		|\
		DESKTOP_JOURNALPLAYBACK		| DESKTOP_JOURNALRECORD		|\
		DESKTOP_READOBJECTS			| DESKTOP_SWITCHDESKTOP		|\
		DESKTOP_WRITEOBJECTS

		hdesk = OpenDesktop(SZ_DESKTOP_WINLOGON, 0, FALSE, MAXIMUM_ALLOWED);
		if (hdesk == NULL)
		{
			OutputLog(_T("Open Desktop Error");
			break;
		}

		if(!SetThreadDesktop(hdesk))
		{
			OutputLog(_T("Can't Set Thread Desktop");
			break;
		}

		if (m_pRoutine)
			m_pRoutine(0, m_pParam);

		bRet = TRUE;
	}
	while (FALSE);

	if (hwinstaCurrent)
	{
		SetProcessWindowStation(hwinstaCurrent);
		hwinstaCurrent = NULL;
	}

	if (hdeskCurrent)
	{
		SetThreadDesktop(hdeskCurrent);
		hdeskCurrent = NULL;
	}

	if (hwinsta)
	{
		CloseWindowStation(hwinsta);
		hwinsta = NULL;
	}

	if (hdesk)
	{
		CloseDesktop(hdesk);
		hdesk = NULL;
	}
	return bRet;
#else
	if (m_pRoutine)
	{
		m_pRoutine(CALLBACK_TYPE_START, m_pParam, NULL, NULL);
	}
	OutputLog(_T("Run End..."));
	return TRUE;
#endif
}

#if _MSC_VER <= 1200
#pragma warning(pop)
#endif

