#pragma once

#include <winsvc.h>

enum CALLBACK_TYPE
{
	CALLBACK_TYPE_START = 0,
	CALLBACK_TYPE_STOP,
	CALLBACK_TYPE_DEV,

	CALLBACK_TYPE_MAX
};

typedef DWORD (WINAPI *PCALLBACK_ROUTINE)(
	CALLBACK_TYPE uType, LPVOID pParam, LPVOID lpParameter, LPVOID lpParameter1
	);
typedef PCALLBACK_ROUTINE LPCALLBACK_ROUTINE;

class CSvchostServiceModule
{
public:
	CSvchostServiceModule();
	~CSvchostServiceModule();
	
public:
	static BOOL IsInstalled(LPCTSTR pszServiceName);
	static BOOL Install(LPCTSTR pszServiceName, LPTSTR lpDescription, LPCTSTR pszDllPath, LPCTSTR pszSvchost);
	static BOOL Uninstall(LPCTSTR pszServiceName, LPCTSTR pszSvchost);
	// How To Programmatically Stop a Service and Its Dependencies
	// https://support.microsoft.com/en-us/kb/245230
	static BOOL Control(LPCTSTR pszServiceName, BOOL bStart = TRUE, BOOL bStopDependencies = FALSE);
	static BOOL Query(LPCTSTR pszServiceName, SERVICE_STATUS& status);
	static BOOL IsRunning(LPCTSTR pszServiceName);
	static BOOL SetStartMode(LPCTSTR pszServiceName, BOOL bAuto);
	static BOOL GetStartMode(LPCTSTR pszServiceName, BOOL& bAuto);
	//static BOOL IsCallFromSCM();
	static VOID GetDllPath(LPTSTR pDllPath, LPCTSTR pDllName, DWORD tPid);

public:
	void Init(LPCTSTR pszServiceName, LPCALLBACK_ROUTINE pRoutine = NULL, LPVOID pParam = NULL);
	void _ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
	DWORD HandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData);
	void SetServiceStatus(DWORD dwState);
	BOOL Run();
	
public:
	TCHAR					m_szServiceName[256];
	SERVICE_STATUS_HANDLE	m_hServiceStatus;
	SERVICE_STATUS			m_status;
	DWORD					dwThreadID;

	LPCALLBACK_ROUTINE		m_pRoutine;
	LPVOID					m_pParam;
};

extern CSvchostServiceModule gSvchostServiceModule;
