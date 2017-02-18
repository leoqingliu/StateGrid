#include "StdAfx.h"
#include "UDiskUtils.h"
#include "SvchostServiceModule.h"
#include <SetupAPI.h>
#include <FltUser.h>
#include <WinIoCtl.h>
#pragma comment(lib, "fltLib.lib")
//#pragma comment(lib, "fltMgr.lib")

#define FLT_PORT_NAME		L"\\DszMiniPort"
#define DRIVER_NAME			_T("PDCtrlDev-6761FCBC-40EB-9905-B7D0-7F3535256A1F")

enum UDISK_CMD
{
	CMD_QUERY = 0,
	CMD_ALLOW,
	CMD_DENY,
	CMD_FORBIDDEN,	// 完全禁止
	CMD_PWD,		// 需要密码
};
#define IOCTL_CDO_COMMAND CTL_CODE(0x9000,0x900,METHOD_BUFFERED,FILE_READ_ACCESS|FILE_WRITE_ACCESS)

#pragma warning(push)
#pragma warning(disable:4200) // disable warnings for structures with zero length arrays.

typedef struct __declspec(align(16)) _COMMAND_MESSAGE
{
	ULONGLONG command;
	ULONGLONG data;
	UCHAR Data[];
}
COMMAND_MESSAGE, *PCOMMAND_MESSAGE;

#pragma warning(pop)

CUDiskUtils::CUDiskUtils(void)
{
	m_hDiskDevice = INVALID_HANDLE_VALUE;
	m_hDiskFltPort = NULL;
}

CUDiskUtils::~CUDiskUtils(void)
{
}

BOOL CUDiskUtils::Install(HWND hWnd, LPCTSTR pDir)
{
	BOOL bInstalled = CSvchostServiceModule::IsInstalled(FLT_SERVICE_NAME);
	BOOL bGuardInstalled = CSvchostServiceModule::IsInstalled(UGUARD_SERVICE_NAME);
	if (bInstalled && bGuardInstalled)
	{
		return TRUE;
	}

	TCHAR szCurDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurDir);
	SetCurrentDirectory(pDir);
	//InstallHinfSection(hWnd, NULL, _T("DefaultInstall 128 ./DszProtect.inf"), 0);
	InstallHinfSection(hWnd, NULL, _T("DefaultInstall 128 ./DszProtect.inf"), 0);
	InstallHinfSection(hWnd, NULL, _T("DefaultInstall 128 ./UDiskGuard.inf"), 0);
	SetCurrentDirectory(szCurDir);
	bInstalled = CSvchostServiceModule::IsInstalled(FLT_SERVICE_NAME);
	bGuardInstalled = CSvchostServiceModule::IsInstalled(UGUARD_SERVICE_NAME);
	if (!bInstalled || !bGuardInstalled)
	{
		return FALSE;
	}

// 	BOOL bStarted = Control(FLT_SERVICE_NAME, TRUE);
// 	bStarted = bStarted && Control(UGUARD_SERVICE_NAME, TRUE);
// 	if (!bStarted)
// 	{
// 		return FALSE;
// 	}
		
	return TRUE;
}

BOOL CUDiskUtils::Uninstall(HWND hWnd, LPCTSTR pDir)
{
//	BOOL fOK = Control(FALSE);
	TCHAR szCurDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurDir);
	SetCurrentDirectory(pDir);
	//InstallHinfSection(hWnd, NULL, _T("DefaultUninstall 128 ./DszProtect.inf"), 0);
	BOOL bStarted = Control(FLT_SERVICE_NAME, FALSE);
	bStarted = bStarted && Control(UGUARD_SERVICE_NAME, FALSE);
	InstallHinfSection(NULL, NULL, _T("DefaultUninstall 128 ./DszProtect.inf"), 0);
	InstallHinfSection(NULL, NULL, _T("DefaultUninstall 128 ./UDiskGuard.inf"), 0);
	SetCurrentDirectory(szCurDir);
	return TRUE;
}

BOOL CUDiskUtils::Control(LPCTSTR pName, BOOL bStart)
{
	BOOL bRunning = CSvchostServiceModule::IsRunning(pName);
	if (bStart && !bRunning)
	{
		return CSvchostServiceModule::Control(pName, bStart);
	}
	else if (!bStart && bRunning)
	{
		return CSvchostServiceModule::Control(pName, bStart);
	}

	return TRUE;
}

INT CUDiskUtils::Open()
{
	Close();

	int iResultCount = 0;
	HRESULT hr = FilterConnectCommunicationPort(FLT_PORT_NAME, 0, NULL, 0, NULL, &m_hDiskFltPort);
	if (S_OK == hr)
	{
		iResultCount++;
	}
	else
	{
		iResultCount--;
	}

	TCHAR szCompleteDeviceName[MAX_PATH];
	if ((::GetVersion() & 0xFF) >= 5)
	{
		//
		// We reference the global name so that the application can
		// be executed in Terminal Services sessions on Win2K
		//
		wsprintf(szCompleteDeviceName, TEXT("\\\\.\\Global\\%s"), DRIVER_NAME);
	}
	else
	{
		wsprintf(szCompleteDeviceName, TEXT("\\\\.\\%s"), DRIVER_NAME);
	}

	m_hDiskDevice = CreateFile(
		szCompleteDeviceName,
		GENERIC_READ | GENERIC_WRITE,
		0, 
		NULL, 
		OPEN_EXISTING, 
		0,
		NULL);
	if (INVALID_HANDLE_VALUE != m_hDiskDevice)
	{
		iResultCount++;
	}
	else
	{
		ClientOSVer OSVer = GetClientWindwosVer();
		if (OSVer >= COSV_WIN7)
		{
			iResultCount--;
		}
	}
	
	return iResultCount;
}

VOID CUDiskUtils::Close()
{
	if (NULL != m_hDiskFltPort)
	{
		::CloseHandle(m_hDiskFltPort);
		m_hDiskFltPort = NULL;

	}

	if (INVALID_HANDLE_VALUE != m_hDiskDevice)
	{
		::CloseHandle(m_hDiskDevice);
		m_hDiskDevice = INVALID_HANDLE_VALUE;
	}
}

BOOL CUDiskUtils::DeviceCtrl(LPVOID inBuf, DWORD inBufLen, LPVOID outBuf, DWORD outBufLen)
{
	if (INVALID_HANDLE_VALUE == m_hDiskDevice)
	{
		return FALSE;
	}

	DWORD bytesRetuned;
	BOOL bResult = DeviceIoControl(
		m_hDiskDevice,
		IOCTL_CDO_COMMAND, 
		inBuf, 
		inBufLen, 
		outBuf, 
		outBufLen, 
		&bytesRetuned, 
		NULL);

	return bResult;
}

BOOL CUDiskUtils::SendFltPortMessage(LPVOID inBuf, DWORD inBufLen, LPVOID outBuf, DWORD outBufLen)
{
	DWORD bytesReturned;
	HRESULT hr = FilterSendMessage(
		m_hDiskFltPort, inBuf, inBufLen, 
		outBuf, outBufLen, &bytesReturned);
	return SUCCEEDED(hr);
}

BOOL CUDiskUtils::SetDeviceAuthResult(DeviceHandle devHandle, BOOL fSuccess)
{
	__declspec(align(8)) ULONGLONG inBuf[2] = { fSuccess ? CMD_ALLOW : CMD_DENY, devHandle };
	if (m_dwCurDeviceType == 1)
	{
		return SendFltPortMessage(inBuf, sizeof(inBuf), inBuf, sizeof(inBuf));
	}

	ASSERT(m_dwCurDeviceType == 2);
	return DeviceCtrl(inBuf, sizeof(inBuf), inBuf, sizeof(inBuf));
}

BOOL CUDiskUtils::SetForbidden(BOOL bEnable)
{
	__declspec(align(8)) ULONGLONG inBuf[2] = {CMD_FORBIDDEN, bEnable};
	if (NULL != m_hDiskFltPort)
	{
		return SendFltPortMessage(inBuf, sizeof(inBuf), inBuf, sizeof(inBuf));
	}

	if (INVALID_HANDLE_VALUE != m_hDiskDevice)
	{
		return DeviceCtrl(inBuf, sizeof(inBuf), inBuf, sizeof(inBuf));
	}

	return FALSE;
}

BOOL CUDiskUtils::SetAuthResult(BOOL bAllow)
{
	BOOL fOK = SetDeviceAuthResult(m_dwCurHandle, bAllow);
	return fOK;
}

BOOL CUDiskUtils::GetDeviceProcInfo()
{
	BOOL fRet;
	COMMAND_MESSAGE cmdMsg = { CMD_QUERY };
	__declspec(align(8)) ULONGLONG outBuf[16] = { 0 };

	fRet = SendFltPortMessage(&cmdMsg, sizeof(cmdMsg), outBuf, sizeof(outBuf));
	if (fRet && outBuf[0] > 0)
	{
		m_dwCurHandle = outBuf[1];
		m_dwCurDeviceType = 1;
		return TRUE;
	}

	fRet = DeviceCtrl(&cmdMsg, sizeof(cmdMsg), outBuf, sizeof(outBuf));
	if (fRet && outBuf[0] > 0)
	{
		m_dwCurHandle = outBuf[1];
		m_dwCurDeviceType = 2;
		return TRUE;
	}

	return FALSE;
}

CUDiskUtils::ClientOSVer CUDiskUtils::GetClientWindwosVer()
{
	typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
	typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);

	OSVERSIONINFOEX osvi;
	SYSTEM_INFO si;
	PGNSI pGNSI;
	PGPI pGPI;
	DWORD dwType;

	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx ((OSVERSIONINFO *) &osvi);

	pGNSI = (PGNSI) GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo");
	if (NULL != pGNSI)
	{
		pGNSI(&si);
	}
	else 
	{
		GetSystemInfo(&si);
	}

	if ( VER_PLATFORM_WIN32_NT == osvi.dwPlatformId && osvi.dwMajorVersion > 4 )
	{
		if ( osvi.dwMajorVersion == 6 )
		{
			if (osvi.dwMinorVersion == 1)
			{
				pGPI = (PGPI)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetProductInfo");
				pGPI( osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &dwType);
				if (dwType == PRODUCT_HOME_BASIC || dwType == PRODUCT_HOME_PREMIUM)
				{
					return COSV_WIN7_HOME;
				}

				return COSV_WIN7;
			}
			else if (osvi.dwMinorVersion == 0)
			{
				return COSV_VISTA;
			}
		}
		else if (osvi.dwMajorVersion == 5 )
		{
			if (osvi.dwMinorVersion > 0)
			{
				return COSV_XP;
			}
			else
			{
				return COSV_2K;
			}
		}

		if (osvi.dwMajorVersion >= 6 && osvi.dwMinorVersion > 1)
		{
			return COSV_WIN8;
		}
	}


	return COSV_2K_BEFORE;
}
