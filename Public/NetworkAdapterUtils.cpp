#include "StdAfx.h"
#include "NetworkAdapterUtils.h"
#include "WMIUtils.h"
#include "Debug.h"
#include <SetupAPI.h>
#include <Ndisguid.h>
#include <cfgmgr32.h>
#include <regstr.h>

CNetworkAdapterUtils::CNetworkAdapterUtils(void)
{
}

CNetworkAdapterUtils::~CNetworkAdapterUtils(void)
{
}

#if 0
BOOL CNetworkAdapterUtils::GetWlan(WLAN_INFO& Info)
{
	HANDLE hClient = NULL;
	DWORD dwMaxClient = 2;
	DWORD dwCurVersion = 0;
	DWORD dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient); 
	if (dwResult != ERROR_SUCCESS)
	{
		OutputLog(_T("WlanOpenHandle failed with error: %u"), dwResult);
		return FALSE;
	}

	PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
	dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList); 
	if (dwResult != ERROR_SUCCESS)
	{
		OutputLog(_T("WlanEnumInterfaces failed with error: %u"), dwResult);
		WlanCloseHandle(hClient, NULL);
		return FALSE;
	}

	Info.dwCount = pIfList->dwNumberOfItems;
	Info.dwIndex = pIfList->dwIndex;
	Info.vecInfos.clear();

	for (int i = 0; i < (int) pIfList->dwNumberOfItems; i++)
	{
		PWLAN_INTERFACE_INFO pIfInfo = (WLAN_INTERFACE_INFO *) &pIfList->InterfaceInfo[i];
		Info.vecInfos.push_back(*pIfInfo);	
	}

	if (pIfList != NULL)
	{
		WlanFreeMemory(pIfList);
		pIfList = NULL;
	}
	WlanCloseHandle(hClient, NULL);

	return TRUE;
}

BOOL CNetworkAdapterUtils::GetNetworkAdapter(std::vector<ADAPTER_INFO>& vecInfo)
{
	CWMIUtils wmi;
	LRESULT lInit = wmi.Init();
	if (FAILED(lInit) || NULL == wmi.m_pSvc)
	{
		return FALSE;
	}

	// Get Serial Number
	BSTR className = L"Win32_NetworkAdapter";
	HRESULT lRet;
	IEnumWbemClassObject* pEnumAdapters = NULL;

	// Get the list of logical storage device instances.
	lRet = wmi.m_pSvc->CreateInstanceEnum(
		className,			// name of class
		0,
		NULL,
		&pEnumAdapters);	// pointer to enumerator
	if (FAILED(lRet))
	{
		OutputLog(_T("CreateInstanceEnum Failed, Error: 0x%08X"), lRet);
		wmi.UnInit();
		return FALSE;
	}

	BOOL bRet = FALSE;
	ULONG uReturned = 1;
	while(uReturned == 1)
	{
		IWbemClassObject *pStorageDev = NULL;

		//---------------------------
		// enumerate through the result-set.
		lRet = pEnumAdapters->Next(
			WBEM_INFINITE,	// timeout in two seconds
			1,				// return just one storage device
			&pStorageDev,	// pointer to storage device
			&uReturned);	// number obtained: one or zero

		if (SUCCEEDED(lRet) && 1 == uReturned)
		{
			ADAPTER_INFO info;
			wmi.GetFieldString(pStorageDev, L"GUID", info.strGuid);
			if (!info.strGuid.IsEmpty())
			{
				wmi.GetFieldString(pStorageDev, L"Description", info.strDesc);
				wmi.GetFieldString(pStorageDev, L"Name", info.strName);
				wmi.GetFieldString(pStorageDev, L"MACAddress", info.strMac);
				wmi.GetFieldString(pStorageDev, L"NetConnectionID", info.strDisplayName);
				wmi.GetFieldString(pStorageDev, L"PNPDeviceID", info.strDeviceId);
				wmi.GetFieldString(pStorageDev, L"Manufacturer", info.strManufacturer);

				// baidu:    64-D9-54
				//			 38-E3-C5
				//			 D0-0F-6D
				//			 D0-0E-D9
				// 360wifi3: B0-D5-9D
				//			 C8-D5-FE
				//			 70-B0-35
				// 360wifi2: 24-05-0F
				// xiaomi:   F8-A4-5F
				//           8C-BE-BE
				//           64-09-80
				//           98-FA-E3
				//           18-59-36
				//           9C-99-A0
				//           14-F6-5A
				//           0C-1D-AF
				//           28-E3-1F
				//           F0-B4-29
				//           D4-97-0B
				//           F4-8B-32
				//           AC-F7-F3
				//           00-9E-C8
				//           7C-1D-D9
				//           A0-86-C6
				//           58-44-98
				//           10-2A-B3
				//           FC-64-BA
				//           C4-6A-B7
				//           68-DF-DD
				//           64-B4-73
				//           74-51-BA
				//           34-80-B3
				//           20-82-C0

				LPCTSTR pMiniWifi[] =
				{
					// Baidu
					_T("64:D9:54"), _T("38:E3:C5"), _T("D0:0F:6D"), _T("D0:0E:D9"),
					// 360wifi3
					_T("B0:D5:9D"), _T("C8:D5:FE"), _T("70:B0:35"),
					// 360wifi2
					_T("24:05:0F"),
					// xiaomi
					_T("F8:A4:5F"), _T("8C:BE:BE"), _T("64:09:80"), _T("98:FA:E3"), _T("18:59:36"), _T("9C:99:A0"), _T("14:F6:5A"), _T("0C:1D:AF"), _T("28:E3:1F"), _T("F0:B4:29"), _T("D4:97:0B"), _T("F4:8B:32"), _T("AC:F7:F3"), _T("00:9E:C8"), _T("7C:1D:D9"), _T("A0:86:C6"), _T("58:44:98"), _T("10:2A:B3"), _T("FC:64:BA"), _T("C4:6A:B7"), _T("68:DF:DD"), _T("64:B4:73"), _T("74:51:BA"), _T("34:80:B3"), _T("20:82:C0")
				};
				int i;
				for (i = 0; i < _countof(pMiniWifi); i++)
				{
					if (0 == info.strMac.Left(8).CompareNoCase(pMiniWifi[i]))
					{
						break;
					}
				}
				if (i == _countof(pMiniWifi))
				{
					info.bIsMini = FALSE;
				}
				else
				{
					info.bIsMini = TRUE;
				}

				bRet = TRUE;
				vecInfo.push_back(info);
			}

			// Done with this object.
			pStorageDev->Release();
		}
	}

	if(pEnumAdapters)
	{
		pEnumAdapters->Release();
		pEnumAdapters = NULL;
	}

	wmi.UnInit();

	WLAN_INFO wlanInfo;
	BOOL bWlan = GetWlan(wlanInfo);
	if (!bWlan)
	{
		return FALSE;
	}

	for (int i = 0; i < (int)vecInfo.size(); i++)
	{
		int j;
		for (j = 0; j < (int)wlanInfo.vecInfos.size(); j++)
		{
			if (0 == vecInfo[i].strName.CompareNoCase(wlanInfo.vecInfos[j].strInterfaceDescription))
			{
				break;
			}
		}
		if (j != (int)wlanInfo.vecInfos.size())
		{
			vecInfo[i].bWired = FALSE;
			vecInfo[i].bWireless = TRUE;
		}
		else
		{
			vecInfo[i].bWired = TRUE;
			vecInfo[i].bWireless = FALSE;
		}
	}

	return bRet;
}

#endif

BOOL CNetworkAdapterUtils::IsClassHidden(const GUID *ClsGuid) const
{
	HKEY hKeyClass = ::SetupDiOpenClassRegKey(ClsGuid, KEY_READ);
	BOOL hidden = FALSE;
	if (INVALID_HANDLE_VALUE != hKeyClass)
	{
		hidden = (ERROR_SUCCESS == ::RegQueryValueEx(hKeyClass, REGSTR_VAL_NODISPLAYCLASS, NULL, NULL, NULL, NULL));
		::RegCloseKey(hKeyClass);
	}

	return hidden;
}

BOOL CNetworkAdapterUtils::GetNetworkAdapterInternal(CONST GUID *ClassGuid, std::vector<ADAPTER_INFO>& vecInfo, const std::vector<ADAPTER_TYPE_INFO>& adapterType)
{
	DWORD dwFlag = DIGCF_DEVICEINTERFACE;
	HDEVINFO hDevInfo = SetupDiGetClassDevs(ClassGuid, NULL, NULL, dwFlag);
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
		ULONG ulStatus = 0;//DN_MOVED;
		ULONG ulStatusRet = 0;
		status = CM_Get_DevNode_Status(&ulStatus, &ulStatusRet, DeviceInfoData.DevInst, 0);
		if (CR_SUCCESS != status)
			continue;
		//DEVPKEY_Device_ContainerId
		//ulStatus = DN_ROOT_ENUMERATED;
		//status = CM_Get_DevNode_Status(&ulStatus, &ulStatusRet, DeviceInfoData.DevInst, 0);
		//if (CR_SUCCESS != status)
		//	continue;
		//	ulStatus = 0;
		//	ulStatusRet = 0;
		//	status = CM_Get_DevNode_Status(&ulStatus, &ulStatusRet, DeviceInfoData.DevInst, 0);
		//	if (CR_SUCCESS != status)
		//		continue;
		//	if ((ulStatus & DN_NO_SHOW_IN_DM) || IsClassHidden(&DeviceInfoData.ClassGuid))
		//		continue;

		// 编程实现重起网卡等设备
		// http://www.cnblogs.com/flier/archive/2004/07/08/22299.html
		//
		// https://msdn.microsoft.com/en-us/library/windows/hardware/ff553955(v=vs.85).aspx

		if (!(!((ulStatus & DN_NO_SHOW_IN_DM) || IsClassHidden(&DeviceInfoData.ClassGuid))))   
			continue;  
		DWORD dwDataT;
		DWORD dwBuffersize = 0;
		SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_DEVICEDESC, &dwDataT, NULL, 0, &dwBuffersize);
		if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			continue;
		LPTSTR pBuffer = (LPTSTR)LocalAlloc(LPTR, dwBuffersize);
		if (NULL == pBuffer)
			continue;
		BOOL bProp = SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_DEVICEDESC, &dwDataT, (LPBYTE)pBuffer, dwBuffersize, &dwBuffersize);
		if (!bProp)
		{
			LocalFree(pBuffer);
			continue;
		}
		ADAPTER_INFO info;
		info.strDesc = pBuffer;
		LocalFree(pBuffer);
		info.strDeviceId = szDeviceInstanceID;
		info.eType = ADAPTER_TYPE_WIRED;
		for (std::vector<ADAPTER_TYPE_INFO>::const_iterator pIter = adapterType.begin(); pIter != adapterType.end(); pIter++)
		{
			if (-1 != info.strDesc.Find(pIter->strDesc))
			{
				info.eType = pIter->eType;
				break;
			}
		}
		vecInfo.push_back(info);
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);
	return TRUE;
}

BOOL CNetworkAdapterUtils::GetNetworkAdapter(std::vector<ADAPTER_INFO>& vecInfo, const std::vector<ADAPTER_TYPE_INFO>& adapterType)
{
	BOOL bRet = GetNetworkAdapterInternal(&GUID_DEVINTERFACE_NET, vecInfo, adapterType);
	BOOL bRet1 = GetNetworkAdapterInternal(&UNSPECIFIED_NETWORK_GUID, vecInfo, adapterType);
	BOOL bRet2 = GetNetworkAdapterInternal(&GUID_NDIS_LAN_CLASS, vecInfo, adapterType);
	if (bRet || bRet1 || bRet2)
	{
		return TRUE;
	}

	return FALSE;
}
