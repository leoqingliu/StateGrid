#pragma once

#define FLT_SERVICE_NAME	_T("DszProtect")
#define UGUARD_SERVICE_NAME	_T("UDiskGuard")

typedef ULONGLONG DeviceHandle;
class CUDiskUtils
{
public:
	CUDiskUtils(void);
	~CUDiskUtils(void);

public:
	BOOL IsInstalled();
	BOOL Install(HWND hWnd, LPCTSTR pDir);
	BOOL Uninstall(HWND hWnd, LPCTSTR pDir);
	BOOL Control(LPCTSTR pName, BOOL bStart);
	INT Open();
	VOID Close();
	BOOL SetForbidden(BOOL bEnable);
	BOOL SetAuthResult(BOOL bAllow);
	BOOL GetDeviceProcInfo();
private:
	BOOL DeviceCtrl(LPVOID inBuf, DWORD inBufLen, LPVOID outBuf, DWORD outBufLen);
	BOOL SendFltPortMessage(LPVOID inBuf, DWORD inBufLen, LPVOID outBuf, DWORD outBufLen);
	BOOL SetDeviceAuthResult(DeviceHandle devHandle, BOOL fSuccess);

private:
	HANDLE m_hDiskDevice;
	HANDLE m_hDiskFltPort;
	DeviceHandle m_dwCurHandle;
	DWORD m_dwCurDeviceType;
	enum ClientOSVer
	{
		COSV_2K_BEFORE	= 0,
		COSV_2K,
		COSV_XP,
		COSV_VISTA,
		COSV_WIN7_HOME,
		COSV_WIN7,
		COSV_WIN8,
	};
	ClientOSVer GetClientWindwosVer();
};
