#pragma once

#include <vector>
#include <map>
#include <wlanapi.h>
#pragma comment(lib, "wlanapi.lib")

class CNetworkAdapterUtils
{
public:
	CNetworkAdapterUtils(void);
	~CNetworkAdapterUtils(void);

#if 0
	struct WLAN_INFO
	{
		DWORD dwCount;
		DWORD dwIndex;
		std::vector<WLAN_INTERFACE_INFO> vecInfos;
	};
	static BOOL GetWlan(WLAN_INFO& Info);
#endif

	enum ADAPTER_TYPE
	{
		ADAPTER_TYPE_WIRED = 0,
		ADAPTER_TYPE_WIRELESS,
		ADAPTER_TYPE_MINI,

		ADAPTER_TYPE_MAX
	};
	struct ADAPTER_INFO
	{
		ADAPTER_INFO()
		{
			eType = ADAPTER_TYPE_MAX;
		}
		CString strDesc;
		CString strName;
		CString strGuid;
		CString strMac;
		CString strDisplayName;
		CString strDeviceId;
		CString strManufacturer;
		ADAPTER_TYPE eType;
	};
	BOOL IsClassHidden(const GUID *ClsGuid) const;

	struct ADAPTER_TYPE_INFO
	{
		CString strDesc;
		ADAPTER_TYPE eType;
	};
	BOOL GetNetworkAdapterInternal(CONST GUID *ClassGuid, std::vector<ADAPTER_INFO>& vecInfo, const std::vector<ADAPTER_TYPE_INFO>& adapterType);
	BOOL GetNetworkAdapter(std::vector<ADAPTER_INFO>& vecInfo, const std::vector<ADAPTER_TYPE_INFO>& adapterType);
};
