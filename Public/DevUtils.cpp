#include "StdAfx.h"
#include "DevUtils.h"
#include <SetupAPI.h>
#include <WinIoCtl.h>
#include <initguid.h>
#include "Debug.h"
#pragma comment(lib, "SetupAPI.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Rpcrt4.lib")
#include <cfgmgr32.h>
#include <cfg.h>
#include <vector>
#include "Utils.h"
#include <devguid.h>
#include <Portabledevice.h>

/* A5DCBF10-6530-11D2-901F-00C04FB951ED */
DEFINE_GUID(GUID_DEVINTERFACE_USB_DEVICE, 0xA5DCBF10L, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, \
			0xC0, 0x4F, 0xB9, 0x51, 0xED);

/*
//DEFINE_GUID(GUID_PORTS,      0x4D36E978, 0xE325, 0x11CE, 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18);   // Serial and parallel ports

DEFINE_GUID(GUID_DEVINTERFACE_PARALLEL,		0x97F76EF0, 0xF883, 0x11D0, 0xAF, 0x1F, 0x00, 0x00, 0xF8, 0x00, 0x84, 0x5C);
GUID gArrGuidDisable[] =
{
	//GUID_PORTS
	GUID_DEVINTERFACE_COMPORT,
	GUID_DEVINTERFACE_PARALLEL
};


#include <initguid.h>

DEFINE_GUID(_GUID_DEVINTERFACE_USB_DEVICE, 0xA5DCBF10L, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, \
			0xC0, 0x4F, 0xB9, 0x51, 0xED);
*/

CDevUtils::CDevUtils(void)
{
}

CDevUtils::~CDevUtils(void)
{
	if (NULL != m_hDevNotify)
	{
		UnRegister();
	}
}

BOOL CDevUtils::RegisterHandle(SERVICE_STATUS_HANDLE hServiceStatus)
{
	DEV_BROADCAST_DEVICEINTERFACE DevInt;
	memset(&DevInt,0,sizeof(DEV_BROADCAST_DEVICEINTERFACE));
	DevInt.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	DevInt.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	m_hDevNotify = RegisterDeviceNotification(hServiceStatus, &DevInt, DEVICE_NOTIFY_SERVICE_HANDLE | DEVICE_NOTIFY_ALL_INTERFACE_CLASSES);
	if (NULL == m_hDevNotify)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CDevUtils::RegisterWnd(HWND hWnd)
{
	DEV_BROADCAST_DEVICEINTERFACE DevInt;
	memset(&DevInt,0,sizeof(DEV_BROADCAST_DEVICEINTERFACE));
	DevInt.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	DevInt.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	m_hDevNotify = RegisterDeviceNotification(hWnd, &DevInt, DEVICE_NOTIFY_WINDOW_HANDLE | DEVICE_NOTIFY_ALL_INTERFACE_CLASSES);
	if (NULL == m_hDevNotify)
	{
		return FALSE;
	}

	return TRUE;
}

VOID CDevUtils::UnRegister()
{
	BOOL bRet = UnregisterDeviceNotification(m_hDevNotify);
	if (bRet)
	{
		m_hDevNotify = NULL;
	}
}

#if 0
BOOL CDevUtils::OnDeviceChange(CONFIG_INFO *pInfo, UINT nEventType, DWORD_PTR dwData)
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
					OutputLog(_T("-- OnDeviceChange:DBT_DEVICEARRIVAL --\n"));
					/*
					BOOL bDisable = FALSE;
					for (int i = 0; i < _countof(gArrGuidDisable); i++)
					{
						if (gArrGuidDisable[i] == pDevInf->dbcc_classguid)
						{
							bDisable = TRUE;
						}
					}
					if (bDisable)
					{
						TCHAR szGuid[MAX_PATH] = {0};
						StringFromGUID2(pDevInf->dbcc_classguid, szGuid, MAX_PATH);
						OutputLog(_T("-- OnDeviceChange:DBT_DEVICEARRIVAL Disable[%s] --\n"), szGuid);
						ControlDevices(pDevInf->dbcc_classguid, FALSE);
					}
					*/

					/*
					if (pInfo->m_bCheckBlueTooth)
					{
					}
					else if (pInfo->m_bCheckSerial && (pDevInf->dbcc_classguid == GUID_DEVINTERFACE_COMPORT || pDevInf->dbcc_classguid == GUID_DEVINTERFACE_PARALLEL))
					{
						ControlDevices(pDevInf->dbcc_classguid, FALSE);
					}
					else if (pInfo->m_bCheck1394)
					{
					}
					else if (pInfo->m_bCheckIrda)
					{
					}
					else if (pInfo->m_bCheckPcmcia)
					{
					}
					else if (pInfo->m_bCheckModem)
					{
					}
					*/
				}
				else if (DBT_DEVICEREMOVECOMPLETE == nEventType)
				{
					OutputLog(_T("-- OnDeviceChange:DBT_DEVICEREMOVECOMPLETE --\n"));
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
#endif

BOOL CDevUtils::ControlDevices(const GUID guid, BOOL bEnable /*= TRUE*/, BOOL bDevInterface /*= TRUE*/, LPCTSTR pszDeviceInstId /*= NULL*/)
{
	DWORD dwFlag = 0;
	if (bDevInterface)
	{
		dwFlag |= DIGCF_DEVICEINTERFACE;
	}
	if (!bEnable)
	{
		dwFlag |= DIGCF_PRESENT;
	}
	HDEVINFO hDevInfo = SetupDiGetClassDevs(&guid, NULL, NULL, dwFlag);
	if (INVALID_HANDLE_VALUE == hDevInfo)
	{
		OutputLog(_T("SetupDiGetClassDevs Failed: %08X\n"), GetLastError());
		return FALSE;
	}
	OutputLog(_T("SetupDiGetClassDevs Success\n"));

	DWORD dwSuccessCount = 0;
	SP_DEVINFO_DATA DeviceInfoData;
	ZeroMemory(&DeviceInfoData, sizeof(SP_DEVINFO_DATA));
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	DWORD i;
	for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); ++i)
	{
		OutputLog(_T("SetupDiEnumDeviceInfo: %d\n"), i);
		TCHAR szDeviceInstanceID [MAX_DEVICE_ID_LEN];
		CONFIGRET status = CM_Get_Device_ID(DeviceInfoData.DevInst, szDeviceInstanceID , MAX_PATH, 0);
		if (status != CR_SUCCESS)
			continue;
		if (NULL != pszDeviceInstId)
		{
			if (NULL == StrStrI(szDeviceInstanceID, pszDeviceInstId))
			{
				continue;
			}
		}

		ULONG ulStatus = 0;
		ULONG ulProblemNumber = 0;
		status = CM_Get_DevNode_Status(&ulStatus, &ulProblemNumber, DeviceInfoData.DevInst, 0);
		if (CR_SUCCESS != status)
			continue;

		DWORD dwStateChange = (bEnable ? DICS_ENABLE : DICS_DISABLE);
		if ((DICS_ENABLE == dwStateChange && DN_STARTED != (ulStatus & DN_STARTED)) ||
			(DICS_DISABLE == dwStateChange && DN_STARTED == (ulStatus & DN_STARTED)))
		{
			SP_PROPCHANGE_PARAMS spPropChangeParams;
			spPropChangeParams.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
			spPropChangeParams.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
			spPropChangeParams.Scope = DICS_FLAG_GLOBAL;
			spPropChangeParams.HwProfile = 0;
			spPropChangeParams.StateChange = (bEnable ? DICS_ENABLE : DICS_DISABLE);

			// note we pass spPropChangeParams as SP_CLASSINSTALL_HEADER
			// but set the size as sizeof(SP_PROPCHANGE_PARAMS)
			BOOL bSetParams = SetupDiSetClassInstallParams(
				hDevInfo,
				&DeviceInfoData,
				(SP_CLASSINSTALL_HEADER*)&spPropChangeParams,
				sizeof(SP_PROPCHANGE_PARAMS));
			if (bSetParams)
			{
				BOOL bCall = SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, hDevInfo, &DeviceInfoData);
				if (bCall)
				{
					dwSuccessCount++;
					OutputLog(_T("SetupDiCallClassInstaller Success\n"));
				}
				else
				{
					OutputLog(_T("SetupDiCallClassInstaller Failed: %08X\n"), GetLastError());
				}
			}
			else
			{
				OutputLog(_T("SetupDiSetClassInstallParams Failed: %08X\n"), GetLastError());
			}
		}
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);
	BOOL bRet = (i == dwSuccessCount);
	return bRet;
}

/*
VOID CDevUtils::CheckDevice(CONFIG_INFO *pInfo)
{
	if (pInfo->m_bCheckBlueTooth)
	{
	}
	else if (pInfo->m_bCheckSerial)
	{
//		ControlDevices(GUID_DEVINTERFACE_COMPORT, FALSE);
//		ControlDevices(GUID_DEVINTERFACE_PARALLEL, FALSE);
	}
	else if (pInfo->m_bCheck1394)
	{
	}
	else if (pInfo->m_bCheckIrda)
	{
	}
	else if (pInfo->m_bCheckPcmcia)
	{
	}
	else if (pInfo->m_bCheckModem)
	{
	}
}
*/

BOOL CDevUtils::ParseUDiskIds_Usb(CString strPnpDevice, CString& strId)
{
	strPnpDevice = strPnpDevice.Trim(_T(""));
	/*
	CString strNewSerialNumber;
	for (int i = 0; i < usbInfo.strSerialNumber.GetLength(); i++)
	{
		TCHAR ch = usbInfo.strSerialNumber[i];
		if (isprint(ch))
		{
			strNewSerialNumber.AppendChar(ch);
		}
	}
	*/

	std::vector<CString> vecFields = Utils::Split(strPnpDevice, _T("\\"));
	if (vecFields.size() >= 1)
	{
		strId = vecFields[vecFields.size() - 1];
		return TRUE;
	}
	return FALSE;
}

BOOL CDevUtils::ParseUDiskIds_UsbDisk(CString strPnpDevice, CString& strId)
{
	strPnpDevice = strPnpDevice.Trim(_T(""));
	std::vector<CString> vecFields = Utils::Split(strPnpDevice, _T("&"));
	if (vecFields.size() >= 2)
	{
		CString strTmp = vecFields[vecFields.size() - 2];
		vecFields = Utils::Split(strTmp, _T("\\"));
		if (vecFields.size() >= 1)
		{
			strId = vecFields[vecFields.size() - 1];
			//strId = vecFields[0];
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CDevUtils::GetUDisk(std::vector<CString>& vecIds)
{
	DWORD dwFlag = DIGCF_DEVICEINTERFACE;
	//HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_DISK, NULL, NULL, dwFlag);
	HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_USB_DEVICE, NULL, NULL, dwFlag);
	//HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_USB, NULL, NULL, dwFlag);
	if (INVALID_HANDLE_VALUE == hDevInfo)
	{
		OutputLog(_T("SetupDiGetClassDevs Failed: %08X\n"), GetLastError());
		return FALSE;
	}
	OutputLog(_T("SetupDiGetClassDevs Success\n"));

	DWORD dwSuccessCount = 0;
	SP_DEVINFO_DATA DeviceInfoData;
	ZeroMemory(&DeviceInfoData, sizeof(SP_DEVINFO_DATA));
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	DWORD i;
	for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); ++i)
	{
		OutputLog(_T("SetupDiEnumDeviceInfo: %d\n"), i);
		TCHAR szDeviceInstanceID[MAX_DEVICE_ID_LEN] = {0};
		CONFIGRET status = CM_Get_Device_ID(DeviceInfoData.DevInst, szDeviceInstanceID , MAX_PATH, 0);
		if (CR_SUCCESS != status)
			continue;
		ULONG ulStatus = DN_MOVED;
		ULONG ulProblemNumber = 0;
		status = CM_Get_DevNode_Status(&ulStatus, &ulProblemNumber, DeviceInfoData.DevInst, 0);
		if (CR_SUCCESS != status)
			continue;

		DWORD dwDataT;
		DWORD dwBuffersize = 0;
		SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_SERVICE, &dwDataT, NULL, 0, &dwBuffersize);
		if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			continue;
		LPTSTR pBuffer = (LPTSTR)LocalAlloc(LPTR, dwBuffersize);
		if (NULL == pBuffer)
			continue;
		BOOL bProp = SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_SERVICE, &dwDataT, (LPBYTE)pBuffer, dwBuffersize, &dwBuffersize);
		if (!bProp)
		{
			LocalFree(pBuffer);
			continue;
		}
		CString strService = pBuffer;
		LocalFree(pBuffer);

		//DEVPKEY_Device_ContainerId
		//BOOL bMatched = (0 == StrCmpN(szDeviceInstanceID, _T("USBSTOR\\"), lstrlen(_T("USBSTOR\\"))) ? TRUE : FALSE);
		BOOL bMatched = (0 == strService.CompareNoCase(_T("USBSTOR")));
		if (bMatched)
		{
			CString strId;
			BOOL bParsed = ParseUDiskIds_Usb(szDeviceInstanceID, strId);
			if (!bParsed)
				continue;
			vecIds.push_back(strId);
		}
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);
	return TRUE;
}

VOID CDevUtils::ControlUDisk_Usb(BOOL bEnable, BOOL bCheckId, const std::vector<CString>& vecEnableIds)
{
	DWORD dwFlag = DIGCF_DEVICEINTERFACE;
	//HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_DISK, NULL, NULL, dwFlag);
	HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_USB_DEVICE, NULL, NULL, dwFlag);
	if (INVALID_HANDLE_VALUE == hDevInfo)
	{
		OutputLog(_T("SetupDiGetClassDevs Failed: %08X\n"), GetLastError());
		return;
	}
	OutputLog(_T("SetupDiGetClassDevs Success\n"));

	DWORD dwSuccessCount = 0;
	SP_DEVINFO_DATA DeviceInfoData;
	ZeroMemory(&DeviceInfoData, sizeof(SP_DEVINFO_DATA));
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	DWORD i;
	for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); ++i)
	{
		OutputLog(_T("SetupDiEnumDeviceInfo: %d\n"), i);
		TCHAR szDeviceInstanceID[MAX_DEVICE_ID_LEN] = {0};
		CONFIGRET status = CM_Get_Device_ID(DeviceInfoData.DevInst, szDeviceInstanceID , MAX_PATH, 0);
		if (CR_SUCCESS != status)
			continue;
		ULONG ulStatus = DN_MOVED;
		ULONG ulProblemNumber = 0;
		status = CM_Get_DevNode_Status(&ulStatus, &ulProblemNumber, DeviceInfoData.DevInst, 0);
		if (CR_SUCCESS != status)
			continue;

		DWORD dwDataT;
		DWORD dwBuffersize = 0;
		SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_SERVICE, &dwDataT, NULL, 0, &dwBuffersize);
		if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			continue;
		LPTSTR pBuffer = (LPTSTR)LocalAlloc(LPTR, dwBuffersize);
		if (NULL == pBuffer)
			continue;
		BOOL bProp = SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_SERVICE, &dwDataT, (LPBYTE)pBuffer, dwBuffersize, &dwBuffersize);
		if (!bProp)
		{
			LocalFree(pBuffer);
			continue;
		}
		CString strService = pBuffer;
		LocalFree(pBuffer);

		//DEVPKEY_Device_ContainerId
		//BOOL bMatched = (0 == StrCmpN(szDeviceInstanceID, _T("USBSTOR\\"), lstrlen(_T("USBSTOR\\"))) ? TRUE : FALSE);
		BOOL bMatched = (0 == strService.CompareNoCase(_T("USBSTOR")));
		if (bMatched)
		{
			DWORD dwStateChange = (bEnable ? DICS_ENABLE : DICS_DISABLE);
			CString strId;
			BOOL bParsed = ParseUDiskIds_Usb(szDeviceInstanceID, strId);
			if (!bParsed)
				continue;
			if (bCheckId && bEnable)
			{
				bMatched = FALSE;
				for (int j = 0; j < (int)vecEnableIds.size(); j++)
				{
					if (0 == strId.CompareNoCase(vecEnableIds[j]))
					{
						bMatched = TRUE;
						break;
					}
				}
				if (!bMatched)
				{
					dwStateChange = DICS_DISABLE;
				}
			}

			if ((DICS_ENABLE == dwStateChange && DN_STARTED != (ulStatus & DN_STARTED)) ||
				(DICS_DISABLE == dwStateChange && DN_STARTED == (ulStatus & DN_STARTED)))
			{
				SP_PROPCHANGE_PARAMS spPropChangeParams;
				spPropChangeParams.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
				spPropChangeParams.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
				spPropChangeParams.Scope = DICS_FLAG_GLOBAL;
				spPropChangeParams.HwProfile = 0;
				spPropChangeParams.StateChange = dwStateChange;

				// note we pass spPropChangeParams as SP_CLASSINSTALL_HEADER
				// but set the size as sizeof(SP_PROPCHANGE_PARAMS)
				BOOL bSetParams = SetupDiSetClassInstallParams(
					hDevInfo,
					&DeviceInfoData,
					(SP_CLASSINSTALL_HEADER*)&spPropChangeParams,
					sizeof(SP_PROPCHANGE_PARAMS));
				if (bSetParams)
				{
					BOOL bCall = SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, hDevInfo, &DeviceInfoData);
					if (bCall)
					{
						dwSuccessCount++;
						OutputLog(_T("SetupDiCallClassInstaller Success\n"));
					}
					else
					{
						OutputLog(_T("SetupDiCallClassInstaller Failed: %08X\n"), GetLastError());
					}
				}
				else
				{
					OutputLog(_T("SetupDiSetClassInstallParams Failed: %08X\n"), GetLastError());
				}
			}
		}
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);
}


VOID CDevUtils::ControlUDisk_Disk(BOOL bEnable, BOOL bCheckId, const std::vector<CString>& vecEnableIds)
{
	DWORD dwFlag = DIGCF_DEVICEINTERFACE;
	HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_DISK, NULL, NULL, dwFlag);
	if (INVALID_HANDLE_VALUE == hDevInfo)
	{
		OutputLog(_T("SetupDiGetClassDevs Failed: %08X\n"), GetLastError());
		return;
	}
	OutputLog(_T("SetupDiGetClassDevs Success\n"));

	DWORD dwSuccessCount = 0;
	SP_DEVINFO_DATA DeviceInfoData;
	ZeroMemory(&DeviceInfoData, sizeof(SP_DEVINFO_DATA));
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	DWORD i;
	for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); ++i)
	{
		OutputLog(_T("SetupDiEnumDeviceInfo: %d\n"), i);
		TCHAR szDeviceInstanceID[MAX_DEVICE_ID_LEN] = {0};
		CONFIGRET status = CM_Get_Device_ID(DeviceInfoData.DevInst, szDeviceInstanceID , MAX_PATH, 0);
		if (CR_SUCCESS != status)
			continue;
		ULONG ulStatus = DN_MOVED;
		ULONG ulProblemNumber = 0;
		status = CM_Get_DevNode_Status(&ulStatus, &ulProblemNumber, DeviceInfoData.DevInst, 0);
		if (CR_SUCCESS != status)
			continue;

		DWORD dwDataT;
		DWORD dwBuffersize = 0;
		SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_SERVICE, &dwDataT, NULL, 0, &dwBuffersize);
		if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			continue;
		LPTSTR pBuffer = (LPTSTR)LocalAlloc(LPTR, dwBuffersize);
		if (NULL == pBuffer)
			continue;
		BOOL bProp = SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_SERVICE, &dwDataT, (LPBYTE)pBuffer, dwBuffersize, &dwBuffersize);
		if (!bProp)
		{
			LocalFree(pBuffer);
			continue;
		}
		CString strService = pBuffer;
		LocalFree(pBuffer);

		//DEVPKEY_Device_ContainerId
		BOOL bMatched = (0 == StrCmpN(szDeviceInstanceID, _T("USBSTOR\\"), lstrlen(_T("USBSTOR\\"))) ? TRUE : FALSE);
		if (bMatched)
		{
			DWORD dwStateChange = (bEnable ? DICS_ENABLE : DICS_DISABLE);
			CString strId;
			BOOL bParsed = ParseUDiskIds_UsbDisk(szDeviceInstanceID, strId);
			if (!bParsed)
				continue;
			if (bCheckId && bEnable)
			{
				bMatched = FALSE;
				for (int j = 0; j < (int)vecEnableIds.size(); j++)
				{
					if (0 == strId.CompareNoCase(vecEnableIds[j]))
					{
						bMatched = TRUE;
						break;
					}
				}
				if (!bMatched)
				{
					dwStateChange = DICS_DISABLE;
				}
			}

			if ((DICS_ENABLE == dwStateChange && DN_STARTED != (ulStatus & DN_STARTED)) ||
				(DICS_DISABLE == dwStateChange && DN_STARTED == (ulStatus & DN_STARTED)))
			{
				SP_PROPCHANGE_PARAMS spPropChangeParams;
				spPropChangeParams.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
				spPropChangeParams.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
				spPropChangeParams.Scope = DICS_FLAG_GLOBAL;
				spPropChangeParams.HwProfile = 0;
				spPropChangeParams.StateChange = dwStateChange;

				// note we pass spPropChangeParams as SP_CLASSINSTALL_HEADER
				// but set the size as sizeof(SP_PROPCHANGE_PARAMS)
				BOOL bSetParams = SetupDiSetClassInstallParams(
					hDevInfo,
					&DeviceInfoData,
					(SP_CLASSINSTALL_HEADER*)&spPropChangeParams,
					sizeof(SP_PROPCHANGE_PARAMS));
				if (bSetParams)
				{
					BOOL bCall = SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, hDevInfo, &DeviceInfoData);
					if (bCall)
					{
						dwSuccessCount++;
						OutputLog(_T("SetupDiCallClassInstaller Success\n"));
					}
					else
					{
						OutputLog(_T("SetupDiCallClassInstaller Failed: %08X\n"), GetLastError());
					}
				}
				else
				{
					OutputLog(_T("SetupDiSetClassInstallParams Failed: %08X\n"), GetLastError());
				}
			}
		}
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);
}

VOID CDevUtils::ControlUDisk_Wpd(BOOL bEnable, BOOL bCheckId, const std::vector<CString>& vecEnableIds)
{
	DWORD dwFlag = DIGCF_DEVICEINTERFACE;
	//HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_DISK, NULL, NULL, dwFlag);
	HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_WPD, NULL, NULL, dwFlag);
	if (INVALID_HANDLE_VALUE == hDevInfo)
	{
		OutputLog(_T("SetupDiGetClassDevs Failed: %08X\n"), GetLastError());
		return;
	}
	OutputLog(_T("SetupDiGetClassDevs Success\n"));

	DWORD dwSuccessCount = 0;
	SP_DEVINFO_DATA DeviceInfoData;
	ZeroMemory(&DeviceInfoData, sizeof(SP_DEVINFO_DATA));
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	DWORD i;
	for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); ++i)
	{
		OutputLog(_T("SetupDiEnumDeviceInfo: %d\n"), i);
		TCHAR szDeviceInstanceID[MAX_DEVICE_ID_LEN] = {0};
		CONFIGRET status = CM_Get_Device_ID(DeviceInfoData.DevInst, szDeviceInstanceID , MAX_PATH, 0);
		if (CR_SUCCESS != status)
			continue;
		ULONG ulStatus = 0;
		ULONG ulProblemNumber = 0;
		status = CM_Get_DevNode_Status(&ulStatus, &ulProblemNumber, DeviceInfoData.DevInst, 0);
		if (CR_SUCCESS != status)
			continue;

		DWORD dwStateChange = (bEnable ? DICS_ENABLE : DICS_DISABLE);
		if ((DICS_ENABLE == dwStateChange && DN_STARTED != (ulStatus & DN_STARTED)) ||
			(DICS_DISABLE == dwStateChange && DN_STARTED == (ulStatus & DN_STARTED)))
		{
			SP_PROPCHANGE_PARAMS spPropChangeParams;
			spPropChangeParams.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
			spPropChangeParams.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
			spPropChangeParams.Scope = DICS_FLAG_GLOBAL;
			spPropChangeParams.HwProfile = 0;
			spPropChangeParams.StateChange = dwStateChange;

			// note we pass spPropChangeParams as SP_CLASSINSTALL_HEADER
			// but set the size as sizeof(SP_PROPCHANGE_PARAMS)
			BOOL bSetParams = SetupDiSetClassInstallParams(
				hDevInfo,
				&DeviceInfoData,
				(SP_CLASSINSTALL_HEADER*)&spPropChangeParams,
				sizeof(SP_PROPCHANGE_PARAMS));
			if (bSetParams)
			{
				BOOL bCall = SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, hDevInfo, &DeviceInfoData);
				if (bCall)
				{
					dwSuccessCount++;
					OutputLog(_T("SetupDiCallClassInstaller Success\n"));
				}
				else
				{
					OutputLog(_T("SetupDiCallClassInstaller Failed: %08X\n"), GetLastError());
				}
			}
			else
			{
				OutputLog(_T("SetupDiSetClassInstallParams Failed: %08X\n"), GetLastError());
			}
		}
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);
}

VOID CDevUtils::ControlUDisk(BOOL bEnable, BOOL bCheckId, const std::vector<CString>& vecEnableIds)
{	
	if (bEnable)
	{
		ControlUDisk_Usb(bEnable, bCheckId, vecEnableIds);
		ControlUDisk_Disk(bEnable, bCheckId, vecEnableIds);
		ControlUDisk_Wpd(bEnable, bCheckId, vecEnableIds);
	}
	else
	{
		ControlUDisk_Wpd(bEnable, bCheckId, vecEnableIds);
		ControlUDisk_Disk(bEnable, bCheckId, vecEnableIds);
		ControlUDisk_Usb(bEnable, bCheckId, vecEnableIds);
	}
}
