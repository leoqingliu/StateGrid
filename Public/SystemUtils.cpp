#include "stdafx.h"
#include "SystemUtils.h"
#include <Windows.h>
#include <Winbase.h>
#include <Wtsapi32.h>
#include <TlHelp32.h>
#include <Wtsapi32.h>
#pragma comment(lib, "Wtsapi32.lib")
#include <Userenv.h>
#pragma comment(lib, "Userenv.lib")

SystemUtils::SystemUtils(void)
{
}

SystemUtils::~SystemUtils(void)
{
}

DWORD SystemUtils::FindSessionPid(LPCTSTR lpProcessName, DWORD dwSessionId)
{
	DWORD res = 0;

	PROCESSENTRY32 procEntry;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap == INVALID_HANDLE_VALUE)
	{
		return res;
	}

	procEntry.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hSnap, &procEntry))
	{
		goto _end;
	}

	do
	{
		if (_tcsicmp(procEntry.szExeFile, lpProcessName) == 0)
		{
			DWORD winlogonSessId = 0;
			if (ProcessIdToSessionId(procEntry.th32ProcessID, &winlogonSessId) && winlogonSessId == dwSessionId)
			{
				res = procEntry.th32ProcessID;
				break;
			}
		}

	}
	while (Process32Next(hSnap, &procEntry));

_end:
	CloseHandle(hSnap);
	return res;
}

BOOL SystemUtils::CreateNormalProcessInSystem(LPCTSTR lpCommandLine, BOOL bHide /*= TRUE*/, BOOL bUseAuc /*= TRUE*/, BOOL bWaitFinish /*= FALSE*/)
{
	BOOL bOK = FALSE;

	// Get Active User's Session ID
	//#if _MSC_VER > 1200
#if 0
	DWORD ConsoleSessionId = WTSGetActiveConsoleSessionId();
#else
	typedef DWORD (WINAPI *WTSGetActiveConsoleSessionIdPROC)();
	WTSGetActiveConsoleSessionIdPROC WTSGetActiveConsoleSessionId = NULL;
	HMODULE hInstKernel32 = LoadLibrary(_T("Kernel32.dll"));
	if(!hInstKernel32)
	{
		return FALSE;
	}
	WTSGetActiveConsoleSessionId = (WTSGetActiveConsoleSessionIdPROC)GetProcAddress(hInstKernel32,
		"WTSGetActiveConsoleSessionId");
	if (!WTSGetActiveConsoleSessionId)
	{
		FreeLibrary(hInstKernel32);
		return FALSE;
	}
	DWORD dwConsoleSessionId = WTSGetActiveConsoleSessionId();
	FreeLibrary(hInstKernel32);
#endif
	HANDLE hTokenUser = NULL;
	while(1)
	{
		// Get Active User's Token
		bOK = WTSQueryUserToken(dwConsoleSessionId, &hTokenUser);
		if(bOK == FALSE)
		{
			break;
		}
		//HANDLE hPToken = hTokenUser;

		HANDLE hUserTokenDup = NULL;
		LPVOID pEnv = NULL;
		DWORD dwCreationFlags;
		if (!bUseAuc)
		{
			// Imitate Active User
			bOK = ImpersonateLoggedOnUser(hTokenUser);
			if(bOK == FALSE)
			{
				break;
			}

			hUserTokenDup = hTokenUser;
			dwCreationFlags = CREATE_NEW_CONSOLE;	
		}
		else
		{
			BOOL bTokenRet = FALSE;
			DWORD dwWinlogonPid = FindSessionPid(_T("winlogon.exe"), dwConsoleSessionId);
			OutputLog(_T("Logon Id: %d"), dwWinlogonPid);
			if (0 != dwWinlogonPid)
			{
				TOKEN_PRIVILEGES tp;
				HANDLE hPToken;
				HANDLE hProcess = OpenProcess(MAXIMUM_ALLOWED, FALSE, dwWinlogonPid);
				if (::OpenProcessToken(
					hProcess,
					TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY | TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_ADJUST_SESSIONID | TOKEN_READ | TOKEN_WRITE,
					&hPToken))
				{
					LUID luid;
					if (LookupPrivilegeValue(NULL,SE_DEBUG_NAME, &luid))
					{
						tp.PrivilegeCount =1;
						tp.Privileges[0].Luid = luid;
						tp.Privileges[0].Attributes =SE_PRIVILEGE_ENABLED;
						DuplicateTokenEx(
							hPToken,
							MAXIMUM_ALLOWED,
							NULL,
							SecurityIdentification,
							TokenPrimary,
							&hUserTokenDup);
					}
				}

				if (NULL != hUserTokenDup)
				{
					//Adjust Token privilege
					SetTokenInformation(
						hUserTokenDup,
						TokenSessionId, (void*)dwConsoleSessionId, sizeof(DWORD));

					if (AdjustTokenPrivileges(
						hUserTokenDup,
						FALSE,
						&tp,
						sizeof(TOKEN_PRIVILEGES),
						(PTOKEN_PRIVILEGES)NULL,
						NULL))
					{
						bTokenRet = TRUE;
					}
				}
			}

			if (!bTokenRet)
			{
				OutputLog(_T("Fetch Token Failed: %d"), GetLastError());
				break;
			}

			dwCreationFlags = NORMAL_PRIORITY_CLASS|CREATE_NEW_CONSOLE;
			if (CreateEnvironmentBlock(&pEnv, hUserTokenDup, TRUE))
			{
				dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
			}
			else
			{
				pEnv = NULL;
			}
		}

		// Create Process
		STARTUPINFO si;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		if (bHide)
		{
			si.wShowWindow	= SW_HIDE;
			si.dwFlags		= STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
		}
		else
		{
			si.wShowWindow	= SW_NORMAL;
			//	si.dwFlags		= STARTF_USEPOSITION|STARTF_USESIZE;
		}
		si.lpDesktop = _T("winsta0\\default");
		PROCESS_INFORMATION pi;
		ZeroMemory(&pi, sizeof(pi));

		// Start the child process.
		TCHAR lpCommand[512] = {0};
		wsprintf(lpCommand, _T("%s"), lpCommandLine);

		bOK = ::CreateProcessAsUser(
			hUserTokenDup,
			NULL,
			lpCommand,
			NULL,
			NULL,
			FALSE,
			dwCreationFlags,
			pEnv,
			NULL,
			&si,
			&pi);
		if (bOK == FALSE)
		{
			OutputLog(_T("Create Process Failed: %d"), GetLastError());
			if (!bUseAuc)
			{
				RevertToSelf();
			}
			break;
		}
		OutputLog(_T("Create Process Success"));
		if (bWaitFinish)
		{
			WaitForSingleObject(pi.hProcess, INFINITE);
		}

		if (!bUseAuc)
		{
			// Close Imitate
			bOK = RevertToSelf();
			if(bOK == FALSE)
			{
				break;
			}
		}

		//
		bOK = TRUE;
		//
		break;
	}

	if(hTokenUser != NULL)
	{
		CloseHandle(hTokenUser);
		hTokenUser = NULL;
	}
	return bOK;
}


BOOL SystemUtils::CreateNormalProcess(LPCTSTR lpCommandLine, BOOL bHide /*= TRUE*/, BOOL bWaitFinish /*= FALSE*/)
{
	// Create Process
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb			= sizeof(si);
	if(bHide)
	{
		si.wShowWindow	= SW_HIDE;
		si.dwFlags		= STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
	}
	else
	{
		si.wShowWindow	= SW_NORMAL;
		si.dwFlags		= STARTF_USEPOSITION|STARTF_USESIZE;
	}
	/*
	#ifndef _DEBUG
	si.lpDesktop	= _T("Winsta0\\Winlogon");
	#endif
	*/

	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));

	// Start the child process.
	TCHAR lpCommand[512] = {0};
	wsprintf(lpCommand, _T("%s"), lpCommandLine);
	BOOL bOK = ::CreateProcess(
		NULL, lpCommand, NULL, NULL, FALSE, CREATE_NEW_CONSOLE,
		NULL, NULL, &si, &pi);
	if (bOK)
	{
		if (bWaitFinish)
		{
			WaitForSingleObject(pi.hProcess, INFINITE);
		}
	}
	
	return bOK;
}

BOOL SystemUtils::SystemShutdown(SYSTEM_OP_TYPE eType)
{
	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp; 

	// Get a token for this process. 

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		OutputLog(_T("SystemShutdown: OpenProcessToken Failed: %d"), GetLastError());
		return FALSE;
	}

	// Get the LUID for the shutdown privilege.
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

	tkp.PrivilegeCount = 1;  // one privilege to set
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Get the shutdown privilege for this process. 
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
	if (GetLastError() != ERROR_SUCCESS)
		return FALSE; 

#ifndef SHTDN_REASON_MAJOR_OPERATINGSYSTEM
#define SHTDN_REASON_MAJOR_OPERATINGSYSTEM 0x00020000
#endif
#ifndef SHTDN_REASON_MINOR_UPGRADE
#define SHTDN_REASON_MINOR_UPGRADE 0x00000003
#endif
#ifndef SHTDN_REASON_FLAG_PLANNED
#define SHTDN_REASON_FLAG_PLANNED 0x80000000
#endif

	// Shut down the system and force all applications to close.
	if (SYSTEM_OP_TYPE_SHUTDOWN == eType)
	{
		if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE,
			SHTDN_REASON_MAJOR_OPERATINGSYSTEM |
			SHTDN_REASON_MINOR_UPGRADE |
			SHTDN_REASON_FLAG_PLANNED))
			return FALSE;
	}
	else if (SYSTEM_OP_TYPE_RESTART == eType)
	{
		if (!ExitWindowsEx(EWX_REBOOT | EWX_FORCE,
			SHTDN_REASON_MAJOR_OPERATINGSYSTEM|
			SHTDN_REASON_MINOR_UPGRADE))
			return FALSE;
	}
	else if (SYSTEM_OP_TYPE_LOGOFF == eType)
	{
		if (!ExitWindowsEx(EWX_LOGOFF | EWX_FORCE,
			SHTDN_REASON_MAJOR_OPERATINGSYSTEM|
			SHTDN_REASON_MINOR_UPGRADE))
			return FALSE;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL SystemUtils::Extractfile(HMODULE hInstance, LPCTSTR lpType, LPCTSTR lpName, LPCTSTR lpPath)
{
	// Find and Load the resource containing file(s) data
	HRSRC hResLoad = FindResource(hInstance, lpType, lpName);
	HGLOBAL hResData = LoadResource(hInstance, hResLoad);
	LPCSTR data = (LPCSTR)LockResource(hResData);

	// We should have properly loaded the resource
	assert(data != NULL && hResData != NULL);

	DWORD dwFileLen = SizeofResource(hInstance, hResLoad);
	HANDLE hFileHandle = CreateFile(lpPath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);
	if (hFileHandle == INVALID_HANDLE_VALUE)
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
