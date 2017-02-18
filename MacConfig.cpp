#include "StdAfx.h"
#include "MacConfig.h"
#include "Utils.h"

CONFIG_INFO_ITEM::CONFIG_INFO_ITEM()
{
	ZeroMemory(this, sizeof(CONFIG_INFO_ITEM));
}

//////////////////////////////////////////////////////////////////////////

CONFIG_INFO_ITEMS::CONFIG_INFO_ITEMS()
{
	bDisable = FALSE;
}

VOID CONFIG_INFO_ITEMS::Load(const Json::Value& val)
{
	bDisable = GetJsonInt(val, "bDisable", FALSE);
	USES_CONVERSION;
	vecItems.clear();
	if (val.isMember("Items") && val["Items"].isArray())
	{
		for (int i = 0; i < (int)val["Items"].size(); i++)
		{
			Json::Value jsonItem = val["Items"][i];
			CONFIG_INFO_ITEM subItem;
			subItem.iType = GetJsonInt(jsonItem, "iType", 0);
			std::string strTmp = GET_JSON_STRING(jsonItem, "szWindow", "");
			lstrcpy(subItem.szWindow, CA2T(strTmp.c_str()).m_psz);
			strTmp = GET_JSON_STRING(jsonItem, "szClass", "");
			lstrcpy(subItem.szClass, CA2T(strTmp.c_str()).m_psz);
			strTmp = GET_JSON_STRING(jsonItem, "szProcess", "");
			lstrcpy(subItem.szProcess, CA2T(strTmp.c_str()).m_psz);
			strTmp = GET_JSON_STRING(jsonItem, "szDesc", "");
			lstrcpy(subItem.szDesc, CA2T(strTmp.c_str()).m_psz);
			vecItems.push_back(subItem);
		}
	}
}

VOID CONFIG_INFO_ITEMS::Save(Json::Value& valItem)
{
	USES_CONVERSION;
	valItem["bDisable"] = bDisable;
	Json::Value jsonItems(Json::arrayValue);
	for (int i = 0; i < (int)vecItems.size(); i++)
	{
		const CONFIG_INFO_ITEM& subItem = vecItems[i];
		Json::Value jsonItem;
		jsonItem["iType"]	= subItem.iType;
		jsonItem["szWindow"] = CT2A(subItem.szWindow).m_psz;
		jsonItem["szClass"]	= CT2A(subItem.szClass).m_psz;
		jsonItem["szProcess"]= CT2A(subItem.szProcess).m_psz;
		jsonItem["szDesc"]	= CT2A(subItem.szDesc).m_psz;
		jsonItems.append(jsonItem);
	}
	valItem["Items"] = jsonItems;
}

//////////////////////////////////////////////////////////////////////////

CONFIG_INFO_URL_ITEMS::CONFIG_INFO_URL_ITEMS()
{
	bDisable = FALSE;
}

VOID CONFIG_INFO_URL_ITEMS::Load(const Json::Value& val)
{
	bDisable = GetJsonInt(val, "bDisable", FALSE);
	USES_CONVERSION;
	vecItems.clear();
	if (val.isMember("Items") && val["Items"].isArray())
	{
		for (int i = 0; i < (int)val["Items"].size(); i++)
		{
			Json::Value jsonItem = val["Items"][i];
			if (jsonItem.isString())
			{
				vecItems.push_back(CA2T(jsonItem.asString().c_str()).m_psz);
			}
		}
	}
}

VOID CONFIG_INFO_URL_ITEMS::Save(Json::Value& valItem)
{
	USES_CONVERSION;
	valItem["bDisable"] = bDisable;
	Json::Value jsonItems(Json::arrayValue);
	for (int i = 0; i < (int)vecItems.size(); i++)
	{
		std::string strBuf = CT2A(vecItems[i]).m_psz;
		jsonItems.append(strBuf);
	}
	valItem["Items"] = jsonItems;
}

//////////////////////////////////////////////////////////////////////////

CONFIG_INFO::CONFIG_INFO()
{
	//::InitializeCriticalSection(&m_Cs);
	m_uUDiskType = UDISK_TYPE_ENABLE;
	lstrcpy(m_szUDiskPasswd, _T("123"));
	m_bCheckFileTrans		= FALSE;
	m_bCheckLog				= FALSE;
	// System
	m_bCheckRegEdit			= FALSE;
	m_bCheckDevMgr			= FALSE;
	m_bCheckGpedit			= FALSE;
	m_bCheckTaskmgr			= FALSE;
	m_bCheckMsconfig		= FALSE;
	m_bCheckComputermgr		= FALSE;
	m_bCheckFormat			= FALSE;
	m_bCheckDos				= FALSE;
	m_bCheckIp				= FALSE;
	m_bCheckKeyPrtScn		= FALSE;
	m_bCheckKeyEsc			= FALSE;
	m_bCheckKeyWin			= FALSE;
	m_bCheckKeyCtrlAltA		= FALSE;
	m_bCheckTelnet			= FALSE;
	m_bCheckVir				= FALSE;
	m_bCheckClipboard		= FALSE;
	m_ItemsLan.bDisable		= FALSE;
	m_bCheckShare			= FALSE;
	m_bCheckCreateUser		= FALSE;
	//
	m_bCheckCD				= FALSE;
	m_bCheckFloppy			= FALSE;
	m_ItemsCDBurn.bDisable	= FALSE;
	//
	m_bCheckDeviceCell		= FALSE;
	m_bCheckBlueTooth		= FALSE;
	m_bCheckSerial			= FALSE;
	m_bCheck1394			= FALSE;
	m_bCheckIrda			= FALSE;
	m_bCheckPcmcia			= FALSE;
	m_bCheckModem			= FALSE;
	m_ItemsPhoneAssist.bDisable  = FALSE;
//	m_ItemsProcessBlack.bDisable = FALSE;
// 	m_bCheckQQGroupFile		= FALSE;
// 	m_bCheckQQFile			= FALSE;
// 	m_bCheckHttps			= FALSE;
	
//	if (Utils::IsTaskMgrBootExist())
// 	{
// 		m_bCheckTaskmgr = FALSE;
// 	}

	
	
	bUpdateKeyInfo			= FALSE;
	bUpdateDevice			= FALSE;
	bUpdateNetwork			= FALSE;
	bUpdateUrl				= FALSE;
	bUpdateUDisk			= FALSE;
}
/*
CONFIG_INFO::CONFIG_INFO(const CONFIG_INFO& rhs)
{
	*this = rhs;
	::InitializeCriticalSection(&m_Cs);
}
*/
CONFIG_INFO::~CONFIG_INFO()
{
	//::DeleteCriticalSection(&m_Cs);
}

BOOL CONFIG_INFO::LoadConfig(const Json::Value& root, BOOL bEnableOp /*= FALSE*/)
{
//	Lock();
	USES_CONVERSION;
	m_uUDiskType			= (UDISK_TYPE)(GetJsonInt(root, "uUDiskType", UDISK_TYPE_ENABLE));
	std::string strTmp = GET_JSON_STRING(root, "szUDiskPasswd", "123");
	lstrcpy(m_szUDiskPasswd, CA2T(strTmp.c_str()).m_psz);
	m_bCheckFileTrans		= GetJsonInt(root, "bCheckFileTrans",		FALSE);
	m_bCheckLog				= GetJsonInt(root, "bCheckLog",				FALSE);
	m_bCheckRegEdit			= GetJsonInt(root, "bCheckRegEdit",			FALSE);
	m_bCheckDevMgr			= GetJsonInt(root, "bCheckDevMgr",			FALSE);
	m_bCheckGpedit			= GetJsonInt(root, "bCheckGpedit",			FALSE);
	m_bCheckTaskmgr			= GetJsonInt(root, "bCheckTaskmgr",			FALSE);
	m_bCheckMsconfig		= GetJsonInt(root, "bCheckMsconfig",		FALSE);
	m_bCheckComputermgr		= GetJsonInt(root, "bCheckComputermgr",		FALSE);
	m_bCheckFormat			= GetJsonInt(root, "bCheckFormat",			FALSE);
	m_bCheckDos				= GetJsonInt(root, "bCheckDos",				FALSE);
	m_bCheckIp				= GetJsonInt(root, "bCheckIp",				FALSE);
	m_bCheckKeyPrtScn		= GetJsonInt(root, "bCheckKeyPrtScn",		FALSE);
	m_bCheckKeyEsc			= GetJsonInt(root, "bCheckKeyEsc",			FALSE);
	m_bCheckKeyWin			= GetJsonInt(root, "bCheckKeyWin",			FALSE);
	m_bCheckKeyCtrlAltA		= GetJsonInt(root, "bCheckKeyCtrlAltA",		FALSE);
	m_bCheckTelnet			= GetJsonInt(root, "bCheckTelnet",			FALSE);
	m_bCheckVir				= GetJsonInt(root, "bCheckVir",				FALSE);
	m_bCheckClipboard		= GetJsonInt(root, "bCheckClipboard",		FALSE);
	m_bCheckBlueTooth		= GetJsonInt(root, "bCheckBlueTooth",		FALSE);
	m_bCheckSerial			= GetJsonInt(root, "bCheckSerial",			FALSE);
	m_bCheck1394			= GetJsonInt(root, "bCheck1394",			FALSE);
	m_bCheckIrda			= GetJsonInt(root, "bCheckIrda",			FALSE);
	m_bCheckPcmcia			= GetJsonInt(root, "bCheckPcmcia",			FALSE);
	m_bCheckModem			= GetJsonInt(root, "bCheckModem",			FALSE);
	m_bCheckCD				= GetJsonInt(root, "bCheckCD",				FALSE);
	m_bCheckFloppy			= GetJsonInt(root, "bCheckFloppy",			FALSE);
	m_bCheckDeviceCell		= GetJsonInt(root, "bCheckDeviceCell",		FALSE);
//	m_bCheckQQGroupFile		= GetJsonInt(root, "bCheckQQGroupFile",		FALSE);
//	m_bCheckQQFile			= GetJsonInt(root, "bCheckQQFile",			FALSE);
	m_bCheckHttps			= GetJsonInt(root, "bCheckHttps",			FALSE);
	m_bCheckShare			= GetJsonInt(root, "bCheckShare",			FALSE);
	m_bCheckCreateUser		= GetJsonInt(root, "bCheckCreateUser",		FALSE);
// 	if (Utils::IsTaskMgrBootExist())
// 	{
// 		m_bCheckTaskmgr = FALSE;
// 	}
	if (root.isMember("ItemsCDBurn"))
		m_ItemsCDBurn.Load(root["ItemsCDBurn"]);
	else
		m_ItemsCDBurn.bDisable = FALSE;
	if (root.isMember("ItemsPhoneAssist"))
		m_ItemsPhoneAssist.Load(root["ItemsPhoneAssist"]);
	else
		m_ItemsPhoneAssist.bDisable = FALSE;
// 	if (root.isMember("ItemsMail"))
// 		m_ItemsMail.Load(root["ItemsMail"]);
// 	if (root.isMember("ItemsDisk"))
// 		m_ItemsDisk.Load(root["ItemsDisk"]);
// 	if (root.isMember("ItemsFtp"))
// 		m_ItemsFtp.Load(root["ItemsFtp"]);
// 	if (root.isMember("ItemsChat"))
// 		m_ItemsChat.Load(root["ItemsChat"]);
// 	if (root.isMember("ItemsBlog"))
// 		m_ItemsBlog.Load(root["ItemsBlog"]);
// 	if (root.isMember("ItemsProcessWhite"))
// 		m_ItemsProcessWhite.Load(root["ItemsProcessWhite"]);
// 	if (root.isMember("ItemsProcessBlack"))
// 		m_ItemsProcessBlack.Load(root["ItemsProcessBlack"]);
// 	else
// 		m_ItemsProcessBlack.bDisable = FALSE;
	if (root.isMember("ItemsNetworkWired"))
		m_ItemsNetworkWired.Load(root["ItemsNetworkWired"]);
	if (root.isMember("ItemsNetworkWireless"))
		m_ItemsNetworkWireless.Load(root["ItemsNetworkWireless"]);
	if (root.isMember("ItemsNetworkMini"))
		m_ItemsNetworkMini.Load(root["ItemsNetworkMini"]);
	if (root.isMember("ItemsLan"))
		m_ItemsLan.Load(root["ItemsLan"]);
	else
		m_ItemsLan.bDisable = FALSE;
	if (root.isMember("ItemsUDiskIds"))
		m_ItemsUDiskIds.Load(root["ItemsUDiskIds"]);
	if (root.isMember("ItemsUrlWhite"))
		m_ItemsUrlWhite.Load(root["ItemsUrlWhite"]);
	if (root.isMember("ItemsUrlBlack"))
		m_ItemsUrlBlack.Load(root["ItemsUrlBlack"]);
// 	if (root.isMember("ItemsQQ"))
// 		m_ItemsQQ.Load(root["ItemsQQ"]);
// 	if (root.isMember("ItemsWanwang"))
// 		m_ItemsWanwang.Load(root["ItemsWanwang"]);

	if (bEnableOp)
	{
		bUpdateKeyInfo	= GetJsonInt(root, "bUpdateKeyInfo",	FALSE);
		bUpdateDevice	= GetJsonInt(root, "bUpdateDevice",		FALSE);
		bUpdateNetwork	= GetJsonInt(root, "bUpdateNetwork",	FALSE);
		bUpdateUrl		= GetJsonInt(root, "bUpdateUrl",		FALSE);
		bUpdateUDisk	= GetJsonInt(root, "bUpdateUDisk",		FALSE);
	}

//	UnLock();
	return TRUE;
}

BOOL CONFIG_INFO::LoadConfig(LPCSTR pBuf, BOOL bEnableOp /*= FALSE*/)
{
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(pBuf, root, false))
	{
		return FALSE;
	}
	
	BOOL bRet = LoadConfig(root, bEnableOp);
	return bRet;
}

BOOL CONFIG_INFO::SaveConfig(Json::Value& jsonItem, BOOL bEnableOp /*= FALSE*/, BOOL bCalcSign /*= FALSE*/)
{
//	Lock();
	USES_CONVERSION;
	jsonItem["uUDiskType"]				= (INT)m_uUDiskType;
	jsonItem["szUDiskPasswd"]			= CT2A(m_szUDiskPasswd).m_psz;
	jsonItem["bCheckFileTrans"]			= m_bCheckFileTrans;
	jsonItem["bCheckLog"]				= m_bCheckLog;
	jsonItem["bCheckRegEdit"]			= m_bCheckRegEdit;
	jsonItem["bCheckDevMgr"]			= m_bCheckDevMgr;
	jsonItem["bCheckGpedit"]			= m_bCheckGpedit;
	jsonItem["bCheckTaskmgr"]			= m_bCheckTaskmgr;
	jsonItem["bCheckMsconfig"]			= m_bCheckMsconfig;
	jsonItem["bCheckComputermgr"]		= m_bCheckComputermgr;
	jsonItem["bCheckFormat"]			= m_bCheckFormat;
	jsonItem["bCheckDos"]				= m_bCheckDos;
	jsonItem["bCheckIp"]				= m_bCheckIp;
	jsonItem["bCheckKeyPrtScn"]			= m_bCheckKeyPrtScn;
	jsonItem["bCheckKeyEsc"]			= m_bCheckKeyEsc;
	jsonItem["bCheckKeyWin"]			= m_bCheckKeyWin;
	jsonItem["bCheckKeyCtrlAltA"]		= m_bCheckKeyCtrlAltA;
	jsonItem["bCheckTelnet"]			= m_bCheckTelnet;
	jsonItem["bCheckVir"]				= m_bCheckVir;
	jsonItem["bCheckClipboard"]			= m_bCheckClipboard;
	jsonItem["bCheckBlueTooth"]			= m_bCheckBlueTooth;
	jsonItem["bCheckSerial"]			= m_bCheckSerial;
	jsonItem["bCheck1394"]				= m_bCheck1394;
	jsonItem["bCheckIrda"]				= m_bCheckIrda;
	jsonItem["bCheckPcmcia"]			= m_bCheckPcmcia;
	jsonItem["bCheckModem"]				= m_bCheckModem;
	jsonItem["bCheckCD"]				= m_bCheckCD;
	jsonItem["bCheckFloppy"]			= m_bCheckFloppy;
	jsonItem["bCheckDeviceCell"]		= m_bCheckDeviceCell;
// 	jsonItem["bCheckQQGroupFile"]		= m_bCheckQQGroupFile;
// 	jsonItem["bCheckQQFile"]			= m_bCheckQQFile;
	jsonItem["bCheckHttps"]				= m_bCheckHttps;
	jsonItem["bCheckShare"]				= m_bCheckShare;
	jsonItem["bCheckCreateUser"]		= m_bCheckCreateUser;
	Json::Value valItemsCDBurn;
	m_ItemsCDBurn.Save(valItemsCDBurn);
	jsonItem["ItemsCDBurn"] = valItemsCDBurn;

	Json::Value valItemsPhoneAssist;
	m_ItemsPhoneAssist.Save(valItemsPhoneAssist);
	jsonItem["ItemsPhoneAssist"] = valItemsPhoneAssist;

// 	Json::Value valItemsMail;
// 	m_ItemsMail.Save(valItemsMail);
// 	jsonItem["ItemsMail"] = valItemsMail;
// 
// 	Json::Value valItemsDisk;
// 	m_ItemsDisk.Save(valItemsDisk);
// 	jsonItem["ItemsDisk"] = valItemsDisk;
// 
// 	Json::Value valItemsFtp;
// 	m_ItemsFtp.Save(valItemsFtp);
// 	jsonItem["ItemsFtp"] = valItemsFtp;
// 
// 	Json::Value valItemsChat;
// 	m_ItemsChat.Save(valItemsChat);
// 	jsonItem["ItemsChat"] = valItemsChat;
// 
// 	Json::Value valItemsBlog;
// 	m_ItemsBlog.Save(valItemsBlog);
// 	jsonItem["ItemsBlog"] = valItemsBlog;
//
// 	Json::Value valItemsProcessWhite;
// 	m_ItemsProcessWhite.Save(valItemsProcessWhite);
// 	jsonItem["ItemsProcessWhite"] = valItemsProcessWhite;
// 
// 	Json::Value valItemsProcessBlack;
// 	m_ItemsProcessBlack.Save(valItemsProcessBlack);
// 	jsonItem["ItemsProcessBlack"] = valItemsProcessBlack;

	Json::Value valItemsNetworkWired;
	m_ItemsNetworkWired.Save(valItemsNetworkWired);
	jsonItem["ItemsNetworkWired"] = valItemsNetworkWired;

	Json::Value valItemsNetworkWireless;
	m_ItemsNetworkWireless.Save(valItemsNetworkWireless);
	jsonItem["ItemsNetworkWireless"] = valItemsNetworkWireless;

	Json::Value valItemsNetworkMini;
	m_ItemsNetworkMini.Save(valItemsNetworkMini);
	jsonItem["ItemsNetworkMini"] = valItemsNetworkMini;

	Json::Value valItemsLan;
	m_ItemsLan.Save(valItemsLan);
	jsonItem["ItemsLan"] = valItemsLan;

	Json::Value valItemsUDiskIds;
	m_ItemsUDiskIds.Save(valItemsUDiskIds);
	jsonItem["ItemsUDiskIds"] = valItemsUDiskIds;

	Json::Value valItemsUrlWhite;
	m_ItemsUrlWhite.Save(valItemsUrlWhite);
	jsonItem["ItemsUrlWhite"] = valItemsUrlWhite;

	Json::Value valItemsUrlBlack;
	m_ItemsUrlBlack.Save(valItemsUrlBlack);
	jsonItem["ItemsUrlBlack"] = valItemsUrlBlack;

// 	Json::Value valItemsQQ;
// 	m_ItemsQQ.Save(valItemsQQ);
// 	jsonItem["ItemsQQ"] = valItemsQQ;
// 
// 	Json::Value valItemsWanwang;
// 	m_ItemsWanwang.Save(valItemsWanwang);
// 	jsonItem["ItemsWanwang"] = valItemsWanwang;

	if (bEnableOp)
	{
		jsonItem["bUpdateKeyInfo"]	= bUpdateKeyInfo;
		jsonItem["bUpdateDevice"]	= bUpdateDevice;
		jsonItem["bUpdateNetwork"]	= bUpdateNetwork;
		jsonItem["bUpdateUrl"]		= bUpdateUrl;
		jsonItem["bUpdateUDisk"]	= bUpdateUDisk;
	}

//	UnLock();
	return TRUE;
}

BOOL CONFIG_INFO::SaveConfig(std::string& strBuf, BOOL bEnableOp /*= FALSE*/, BOOL bCalcSign /*= FALSE*/)
{
	Json::Value jsonItem;
	BOOL bRet = SaveConfig(jsonItem, bEnableOp, bCalcSign);
	if (bRet)
	{
//		strBuf = jsonItem.toStyledString();
		Json::FastWriter writer;
		strBuf = writer.write(jsonItem);
	}
	return bRet;
}

BOOL CONFIG_INFO::LoadIni(LPCTSTR pFile)
{
	struct GET_PROFILE
	{
		CString strAppName;
		CONFIG_INFO_ITEMS *pItems;
	};
	GET_PROFILE profiles[] =
	{
		{_T("CDBlur"), &m_ItemsCDBurn}
		,{_T("PhoneAssist"), &m_ItemsPhoneAssist}
//		,{_T("ChatSoftware"), &m_ItemsChat}
// 		,{_T("Mail"), &m_ItemsMail}
// 		,{_T("NetDisk"), &m_ItemsDisk}
// 		,{_T("Ftp"), &m_ItemsFtp}
// 		,{_T("Blog"), &m_ItemsBlog}
		,{_T("NetworkWired"), &m_ItemsNetworkWired}
		,{_T("NetworkWireless"), &m_ItemsNetworkWireless}
		,{_T("NetworkMini"), &m_ItemsNetworkMini}
		,{_T("Lan"), &m_ItemsLan}
//		,{_T("ProcessBlack"), &m_ItemsProcessBlack}
	};

	for (int i = 0; i < _countof(profiles); i++)
	{
		UINT iCount = GetPrivateProfileInt(profiles[i].strAppName, _T("Num"), 0, pFile);
		for (UINT j = 0; j < iCount; j++)
		{
			CString strIndex;
			strIndex.Format(_T("%d"), j + 1);
			TCHAR szBuf[MAX_PATH] = {0};
			GetPrivateProfileString(profiles[i].strAppName, strIndex, _T(""), szBuf, MAX_PATH, pFile);
			CONFIG_INFO_ITEM item;
			if (i < 7 || i > 9)
			{
				lstrcpy(item.szWindow, szBuf);
			}
			else
			{
				item.iType = 2;
			}
			lstrcpy(item.szDesc, szBuf);
			lstrcpy(item.szProcess, szBuf);
			profiles[i].pItems->vecItems.push_back(item);
		}
	}
	
	return TRUE;
}

/*
VOID CONFIG_INFO::Lock()
{
//	m_Lock.Lock();
	::EnterCriticalSection(&m_Cs);
}

VOID CONFIG_INFO::UnLock()
{
//	m_Lock.Unlock();
	::LeaveCriticalSection(&m_Cs);
}
*/

DWORD GetJsonInt(Json::Value _root, LPCSTR _key, INT _default)
{
	if (_root.isMember(_key))
	{
		if (_root[_key].isUInt())
			return _root[_key].asUInt();
		if (_root[_key].isInt())
			return _root[_key].asInt();
	}
	return _default;
}
