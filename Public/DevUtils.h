#pragma once

#include <Dbt.h>
//#include "../MacConfig.h"
#include <WinSvc.h>
#include <vector>
#include <WinUser.h>

class CDevUtils
{
public:
	CDevUtils(void);
	~CDevUtils(void);

public:
	BOOL RegisterHandle(SERVICE_STATUS_HANDLE hServiceStatus);
	BOOL RegisterWnd(HWND hWnd);
	VOID UnRegister();
//	BOOL OnDeviceChange(CONFIG_INFO *pInfo, UINT nEventType, DWORD_PTR dwData);
	static BOOL ControlDevices(const GUID guid, BOOL bEnable = TRUE, BOOL bDevInterface = TRUE, LPCTSTR pszDeviceInstId = NULL);
//	VOID CheckDevice(CONFIG_INFO *pInfo);
	static BOOL ParseUDiskIds_Usb(CString strPnpDevice, CString& strId);
	static BOOL ParseUDiskIds_UsbDisk(CString strPnpDevice, CString& strId);
	static BOOL GetUDisk(std::vector<CString>& vecIds);
	static VOID ControlUDisk_Usb(BOOL bEnable, BOOL bCheckId, const std::vector<CString>& vecEnableIds);
	static VOID ControlUDisk_Disk(BOOL bEnable, BOOL bCheckId, const std::vector<CString>& vecEnableIds);
	static VOID ControlUDisk_Wpd(BOOL bEnable, BOOL bCheckId, const std::vector<CString>& vecEnableIds);
	static VOID ControlUDisk(BOOL bEnable, BOOL bCheckId, const std::vector<CString>& vecEnableIds);
	
protected:
	HDEVNOTIFY m_hDevNotify;
};

