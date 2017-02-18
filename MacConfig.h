#pragma once

#include <json/json.h>
#define GET_JSON_STRING(_root, _key, _default)	(_root.isMember(_key) && _root[_key].isString()) ? _root[_key].asString() : _default
//#define GET_JSON_INT(_root, _key, _default)		(_root.isMember(_key) && (_root[_key].isInt() ? _root[_key].asInt() : (_root[_key].isUInt() ? _root[_key].asUInt() : _default)))

DWORD GetJsonInt(Json::Value _root, LPCSTR _key, INT _default);

class CONFIG_INFO_ITEM
{
public:
	INT iType;
	TCHAR szWindow[MAX_PATH];
	TCHAR szClass[MAX_PATH];
	TCHAR szProcess[MAX_PATH];
	TCHAR szDesc[MAX_PATH];
	CONFIG_INFO_ITEM();

	static CString GetTypeName(int i)
	{
		if (0 == i)
		{
			return _T("∆’Õ®");
		}

		if (1 == i)
		{
			return _T("¿˝Õ‚");
		}
		
		if (2 == i)
		{
			return _T("√Ë ˆ");
		}

		return _T("");
	}

	CString GetTypeName()
	{
		return GetTypeName(iType);
	}
};

class CONFIG_INFO_ITEMS
{
public:
	BOOL bDisable;
	std::vector<CONFIG_INFO_ITEM> vecItems;
	CONFIG_INFO_ITEMS();

	VOID Load(const Json::Value& val);
	VOID Save(Json::Value& valItem);
};

class CONFIG_INFO_URL_ITEMS
{
public:
	BOOL bDisable;
	std::vector<CString> vecItems;
	CONFIG_INFO_URL_ITEMS();

	VOID Load(const Json::Value& val);
	VOID Save(Json::Value& valItem);
};

enum UDISK_TYPE
{
	UDISK_TYPE_DISABLE = 0,
	UDISK_TYPE_SPEC,
	UDISK_TYPE_WRITEPROTECT,
	UDISK_TYPE_COPYTOCOMPUTER,
	UDISK_TYPE_PASSWD,
	UDISK_TYPE_ENABLE,

	UDISK_TYPE_MAX
};

#define MAN_USER_TYPE_ADMIN	1
#define MAN_USER_TYPE_USER	2
typedef struct _MAN_USER
{
	int iType;
	CString strUser;
	CString strPassword;
}
MAN_USER;

class CONFIG_INFO
{
public:
	BOOL m_bCheckLog;
	BOOL m_bCheckFileTrans;
	// System
	BOOL m_bCheckRegEdit;
	BOOL m_bCheckDevMgr;
	BOOL m_bCheckGpedit;
	BOOL m_bCheckTaskmgr;
	BOOL m_bCheckMsconfig;
	BOOL m_bCheckComputermgr;
	BOOL m_bCheckFormat;
	BOOL m_bCheckDos;
	BOOL m_bCheckIp;
	BOOL m_bCheckKeyPrtScn;
	BOOL m_bCheckKeyEsc;
	BOOL m_bCheckKeyWin;
	BOOL m_bCheckKeyCtrlAltA;
	BOOL m_bCheckClipboard;
	BOOL m_bCheckTelnet;
	BOOL m_bCheckShare;
	CONFIG_INFO_ITEMS m_ItemsLan;
	BOOL m_bCheckVir;
	BOOL m_bCheckCreateUser;
	// Storage
	UDISK_TYPE m_uUDiskType;
	TCHAR m_szUDiskPasswd[MAX_PATH];
	CONFIG_INFO_URL_ITEMS m_ItemsUDiskIds;
	BOOL m_bCheckFloppy;
	CONFIG_INFO_ITEMS m_ItemsCDBurn;
	BOOL m_bCheckCD;
	// Dev
	BOOL m_bCheckDeviceCell;
	CONFIG_INFO_ITEMS m_ItemsPhoneAssist;
	CONFIG_INFO_ITEMS m_ItemsNetworkWired;
	CONFIG_INFO_ITEMS m_ItemsNetworkWireless;
	CONFIG_INFO_ITEMS m_ItemsNetworkMini;
	BOOL m_bCheckBlueTooth;
	BOOL m_bCheckSerial;
	BOOL m_bCheck1394;
	BOOL m_bCheckIrda;
	BOOL m_bCheckPcmcia;
	BOOL m_bCheckModem;

	//COMMUNICATE_INFO_ITEM items[MAX_INFO_ITEM_COUNT];
	//INT iItemCount;
//	BOOL m_bCheckQQGroupFile;
//	BOOL m_bCheckQQFile;
	BOOL m_bCheckHttps;
// 	CONFIG_INFO_ITEMS m_ItemsMail;
// 	CONFIG_INFO_ITEMS m_ItemsDisk;
// 	CONFIG_INFO_ITEMS m_ItemsFtp;
// 	CONFIG_INFO_ITEMS m_ItemsChat;
// 	CONFIG_INFO_ITEMS m_ItemsBlog;
//	CONFIG_INFO_ITEMS m_ItemsProcessWhite;
//	CONFIG_INFO_ITEMS m_ItemsProcessBlack;
	CONFIG_INFO_URL_ITEMS m_ItemsUrlWhite;
	CONFIG_INFO_URL_ITEMS m_ItemsUrlBlack;
//	CONFIG_INFO_URL_ITEMS m_ItemsQQ;
//	CONFIG_INFO_URL_ITEMS m_ItemsWanwang;
	CONFIG_INFO();
	//CONFIG_INFO(const CONFIG_INFO& rhs);
	~CONFIG_INFO();

	BOOL LoadConfig(const Json::Value& root, BOOL bEnableOp = FALSE);
	BOOL LoadConfig(LPCSTR pBuf, BOOL bEnableOp = FALSE);
	BOOL SaveConfig(Json::Value& jsonItem, BOOL bEnableOp = FALSE, BOOL bCalcSign = FALSE);
	BOOL SaveConfig(std::string& strBuf, BOOL bEnableOp = FALSE, BOOL bCalcSign = FALSE);
	BOOL LoadIni(LPCTSTR pFile);
	//VOID Lock();
	//VOID UnLock();
	BOOL bUpdateKeyInfo;
	BOOL bUpdateDevice;
	BOOL bUpdateNetwork;
	BOOL bUpdateUrl;
	BOOL bUpdateUDisk;

public:
	//CCriticalSection m_Lock;
	//CRITICAL_SECTION m_Cs;
};

#if 0
#define SET_OPTION_FLAG	\
	theConfig->Lock();														\
	theConfig->m_bCheckAutoRun					= m_bCheckAutoRun;			\
	theConfig->m_bCheckHotkey					= m_bCheckHotkey;			\
	theConfig->m_bAutoLogin						= m_bAutoLogin;				\
	theConfig->m_bStartHide						= m_bStartHide;				\
	theConfig->m_bCheckFileTrans				= m_bCheckFileTrans;		\
	theConfig->m_bCheckLog						= m_bCheckLog;				\
	theConfig->m_bCheckRegEdit					= m_bCheckRegEdit;			\
	theConfig->m_bCheckDevMgr					= m_bCheckDevMgr;			\
	theConfig->m_bCheckGpedit					= m_bCheckGpedit;			\
	theConfig->m_bCheckTaskmgr					= m_bCheckTaskmgr;			\
	theConfig->m_bCheckMsconfig					= m_bCheckMsconfig;			\
	theConfig->m_bCheckComputermgr 				= m_bCheckComputermgr;		\
	theConfig->m_bCheckDos						= m_bCheckDos;				\
	theConfig->m_bCheckIp						= m_bCheckIp;				\
	theConfig->m_bCheckFormat					= m_bCheckFormat;			\
	theConfig->m_bCheckKeyPrtScn				= m_bCheckKeyPrtScn;		\
	theConfig->m_bCheckKeyEsc					= m_bCheckKeyEsc;			\
	theConfig->m_bCheckKeyWin					= m_bCheckKeyWin;			\
	theConfig->m_bCheckKeyCtrlAltA				= m_bCheckKeyCtrlAltA;		\
	theConfig->m_bCheckTelnet					= m_bCheckTelnet;			\
	theConfig->m_bCheckVir						= m_bCheckVir;				\
	theConfig->m_bCheckClipboard				= m_bCheckClipboard;		\
	theConfig->m_bCheckBlueTooth				= m_bCheckBlueTooth;		\
	theConfig->m_bCheckSerial					= m_bCheckSerial;			\
	theConfig->m_bCheck1394						= m_bCheck1394;				\
	theConfig->m_bCheckIrda						= m_bCheckIrda;				\
	theConfig->m_bCheckPcmcia					= m_bCheckPcmcia;			\
	theConfig->m_bCheckModem					= m_bCheckModem;			\
	theConfig->m_bCheckCD						= m_bCheckCD;				\
	theConfig->m_bCheckFloppy					= m_bCheckFloppy;			\
	theConfig->m_bCheckDeviceCell				= m_bCheckDeviceCell;		\
	theConfig->m_ItemsNetworkWired.bDisable		= m_bCheckNetworkWire;		\
	theConfig->m_ItemsNetworkWireless.bDisable	= m_bCheckNetworkWireless;	\
	theConfig->m_ItemsNetworkMini.bDisable		= m_bCheckNetworkMini;		\
	theConfig->m_ItemsLan.bDisable				= m_bCheckLan;				\
	theConfig->m_ItemsCDBurn.bDisable			= m_bCheckCDBurn;			\
	theConfig->m_ItemsPhoneAssist.bDisable 		= m_bCheckPhoneAssist;		\
	theConfig->m_ItemsMail.bDisable				= m_bCheckMail;				\
	theConfig->m_ItemsDisk.bDisable				= m_bCheckDisk;				\
	theConfig->m_ItemsFtp.bDisable				= m_bCheckFtp;				\
	theConfig->m_ItemsChat.bDisable				= m_bCheckChat;				\
	theConfig->m_ItemsBlog.bDisable				= m_bCheckBlog;				\
	theConfig->m_ItemsProcessBlack.bDisable		= m_bCheckProcessBlack;		\
	theConfig->m_ItemsProcessWhite.bDisable		= m_bCheckProcessWhite;		\
	theConfig->m_ItemsUrlBlack.bDisable			= m_bCheckUrlBlackList;		\
	theConfig->m_ItemsUrlWhite.bDisable			= m_bCheckUrlWhiteList;		\
	theConfig->m_uUDiskType						= (UDISK_TYPE)m_uUDiskType;	\
	theConfig->m_bCheckQQGroupFile				= m_bCheckQQGroupFile;		\
	theConfig->m_bCheckQQFile					= m_bCheckQQFile;			\
	theConfig->m_bCheckHttps					= m_bCheckHttps;			\
	theConfig->m_bCheckShare					= m_bCheckShare;			\
	theConfig->m_bCheckCreateUser				= m_bCheckCreateUser;		\
	theConfig->m_ItemsQQ.bDisable				= m_bCheckQQ;				\
	theConfig->m_ItemsWanwang.bDisable			= m_bCheckWangWang;			\
	theConfig->m_dwIpStart						= ntohl(m_dwIpStart);		\
	theConfig->m_dwIpEnd						= ntohl(m_dwIpEnd);			\
	theConfig->UnLock();

#define GET_OPTION_FLAG	\
	theConfig->Lock();														\
	m_bCheckAutoRun			= theConfig->m_bCheckAutoRun;					\
	m_bCheckHotkey			= theConfig->m_bCheckHotkey;					\
	m_bAutoLogin			= theConfig->m_bAutoLogin;						\
	m_bStartHide			= theConfig->m_bStartHide;						\
	m_bCheckFileTrans		= theConfig->m_bCheckFileTrans;					\
	m_bCheckLog				= theConfig->m_bCheckLog;						\
	m_bCheckRegEdit			= theConfig->m_bCheckRegEdit;					\
	m_bCheckDevMgr			= theConfig->m_bCheckDevMgr;					\
	m_bCheckGpedit			= theConfig->m_bCheckGpedit;					\
	m_bCheckTaskmgr			= theConfig->m_bCheckTaskmgr;					\
	m_bCheckMsconfig		= theConfig->m_bCheckMsconfig;					\
	m_bCheckComputermgr		= theConfig->m_bCheckComputermgr;				\
	m_bCheckFormat			= theConfig->m_bCheckFormat;					\
	m_bCheckDos				= theConfig->m_bCheckDos;						\
	m_bCheckIp				= theConfig->m_bCheckIp;						\
	m_bCheckKeyPrtScn		= theConfig->m_bCheckKeyPrtScn;					\
	m_bCheckKeyEsc			= theConfig->m_bCheckKeyEsc;					\
	m_bCheckKeyWin			= theConfig->m_bCheckKeyWin;					\
	m_bCheckKeyCtrlAltA		= theConfig->m_bCheckKeyCtrlAltA;				\
	m_bCheckTelnet			= theConfig->m_bCheckTelnet;					\
	m_bCheckVir				= theConfig->m_bCheckVir;						\
	m_bCheckClipboard		= theConfig->m_bCheckClipboard;					\
	m_bCheckBlueTooth		= theConfig->m_bCheckBlueTooth;					\
	m_bCheckSerial			= theConfig->m_bCheckSerial;					\
	m_bCheck1394			= theConfig->m_bCheck1394;						\
	m_bCheckIrda			= theConfig->m_bCheckIrda;						\
	m_bCheckPcmcia			= theConfig->m_bCheckPcmcia;					\
	m_bCheckModem			= theConfig->m_bCheckModem;						\
	m_bCheckCD				= theConfig->m_bCheckCD;						\
	m_bCheckFloppy			= theConfig->m_bCheckFloppy;					\
	m_bCheckDeviceCell		= theConfig->m_bCheckDeviceCell;				\
	m_bCheckNetworkWire		= theConfig->m_ItemsNetworkWired.bDisable;		\
	m_bCheckNetworkWireless	= theConfig->m_ItemsNetworkWireless.bDisable;	\
	m_bCheckNetworkMini		= theConfig->m_ItemsNetworkMini.bDisable;		\
	m_bCheckLan				= theConfig->m_ItemsLan.bDisable;				\
	m_bCheckCDBurn			= theConfig->m_ItemsCDBurn.bDisable;			\
	m_bCheckPhoneAssist		= theConfig->m_ItemsPhoneAssist.bDisable;		\
	m_bCheckMail			= theConfig->m_ItemsMail.bDisable;				\
	m_bCheckDisk			= theConfig->m_ItemsDisk.bDisable;				\
	m_bCheckFtp				= theConfig->m_ItemsFtp.bDisable;				\
	m_bCheckChat			= theConfig->m_ItemsChat.bDisable;				\
	m_bCheckBlog			= theConfig->m_ItemsBlog.bDisable;				\
	m_bCheckProcessBlack	= theConfig->m_ItemsProcessBlack.bDisable;		\
	m_bCheckProcessWhite	= theConfig->m_ItemsProcessWhite.bDisable;		\
	m_bCheckUrlBlackList	= theConfig->m_ItemsUrlBlack.bDisable;			\
	m_bCheckUrlWhiteList	= theConfig->m_ItemsUrlWhite.bDisable;			\
	m_uUDiskType			= (int)theConfig->m_uUDiskType;					\
	m_bCheckQQGroupFile		= theConfig->m_bCheckQQGroupFile;				\
	m_bCheckQQFile			= theConfig->m_bCheckQQFile;					\
	m_bCheckHttps			= theConfig->m_bCheckHttps;						\
	m_bCheckShare			= theConfig->m_bCheckShare;						\
	m_bCheckCreateUser		= theConfig->m_bCheckCreateUser;				\
	m_bCheckQQ				= theConfig->m_ItemsQQ.bDisable;				\
	m_bCheckWangWang		= theConfig->m_ItemsWanwang.bDisable;			\
	m_dwIpStart				= ntohl(theConfig->m_dwIpStart);				\
	m_dwIpEnd				= ntohl(theConfig->m_dwIpEnd);					\
	theConfig->UnLock();
#endif