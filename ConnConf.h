#ifndef _CONN_CONF_H__
#define _CONN_CONF_H__

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <Windows.h>
#pragma comment(lib, "Ws2_32.lib")
#include "MacConfig.h"
#include <json/json.h>
#include "Public/crypt.h"

namespace NInformation
{
#if 0
	class CConnectInfo
	{
	public:
		CConnectInfo()
		{
			strcpy(szName, "SafeNet");
			dwVersion = 100;
			dwId = 1;
		//	memset(szPasswd, 0, sizeof(szPasswd));
			strncpy(szPasswd, "213456", 64 - 1);
			szPasswd[64 - 1] = 0;
			bIsUrl = FALSE;
			memset(szUrl, 0, sizeof(szUrl));
			dwIPAddress = inet_addr("127.0.0.1");
			uPort = 10015;
		}

	public:
		// Agent Name
		CHAR	szName[32];
		// Agent Version
		DWORD	dwVersion;
		// Agent ID
		DWORD	dwId;
		// Agent Password
		CHAR	szPasswd[64];
		
		// Dynamic Connect
		BOOL	bIsUrl;
		// URL
		CHAR	szUrl[32];
		// Static Network IP
		DWORD	dwIPAddress;

		// Port
		USHORT	uPort;

	public:

		/*
		DWORD ConvertAddr(DWORD Addr) const
		{   
			DWORD Old = MAKELONG(
				MAKEWORD(HIBYTE(HIWORD(Addr)), LOBYTE(HIWORD(Addr))),
				MAKEWORD(HIBYTE(LOWORD(Addr)),	LOBYTE(LOWORD(Addr))));
			return Old;
		}
		*/

		DWORD GetAddress() const
		{
			if (bIsUrl)
			{
				HOSTENT* hst = gethostbyname(szUrl);
				if(hst == NULL)
				{
					return 0;
				}
				
				//
				DWORD dwUrlAddress = 0;
				int iCount = 0;
				for(int i = 0; hst->h_addr_list[i]; i++)
				{
					iCount++;
					if(iCount > 1)
					{
						return 0;
					}

					memcpy(&dwUrlAddress, hst->h_addr_list[i], sizeof(DWORD));
				}
				return dwUrlAddress;
			}
			else
			{
				return dwIPAddress;
			}
		}
	};

	typedef CConnectInfo* PCConnectInfo;
#endif
}

#pragma warning(push)
#pragma warning(disable:4996)
struct CommunicationInfo
{
	CommunicationInfo()
	{
		ZeroMemory(this, sizeof(CommunicationInfo));
		strcpy(szKey, "6dNfg8Upn5fBzGgj8licQHblQvLnUY19z5zcNKNFdsDhUzuI8otEsBODrzFCqCKr");
		strcpy(szHost, "127.0.0.1");
		uPort = 80;
		strcpy(szPage, "/safe/record.php");
		dwId = 1;
		strcpy(szComment, "123456");
	}
	char   szKey[128];
	CHAR  szHost[128];
	USHORT uPort;
	CHAR  szPage[128];
	DWORD  dwId;
	CHAR  szComment[128];
};
#pragma warning(pop)

struct RelayFileInfo
{
	RelayFileInfo()
	{
		bUpdated = FALSE;
	}
	BOOL bUpdated;
	BOOL  bFileTrans;
	TCHAR szUpalodScript[128];
	TCHAR szUpalodDir[128];
};

#pragma pack(push)
#pragma pack(1)
typedef struct _TIME_RANGE
{
	DWORD uTimeStart;
	DWORD uTimeEnd;
	_TIME_RANGE()
	{
		uTimeStart = 4102329600;
		uTimeEnd = 4102415999;
	}
}
TIME_RANGE;

typedef struct _CLIENT_INFO
{
	TCHAR strUserName[MAX_PATH];
	TCHAR strCompany[MAX_PATH];
	TCHAR strNumber[MAX_PATH];
	TCHAR strId[MAX_PATH];
	TCHAR strUnlockPassword[MAX_PATH];
	std::vector<_TIME_RANGE> vecTimeRange;
	TCHAR szTimeDesc[MAX_PATH];
	BOOL bWhiteMode;
	DWORD iWinCount;
	TCHAR vecWin[32][MAX_PATH];
	TCHAR vecClass[32][MAX_PATH];
	DWORD iWhiteWinCount;
	TCHAR vecWhiteWin[32][MAX_PATH];
	TCHAR vecWhiteClass[32][MAX_PATH];
	UINT uScreenTime;
	CONFIG_INFO configInfo;
	TCHAR szCustomComment[MAX_PATH];
	BOOL bLoaded;
	DWORD dwIdleTime;
	_CLIENT_INFO()
	{
		ZeroMemory(strUserName, sizeof(strUserName));
		ZeroMemory(strCompany, sizeof(strCompany));
		ZeroMemory(strNumber, sizeof(strNumber));
		ZeroMemory(strId, sizeof(strId));
		lstrcpy(strUnlockPassword, _T("685768op"));
		//uTimeStart = 0;
		//uTimeEnd = 86399;
		//uTimeEnd = 0;
		//uTimeStart = 4102329600;
		//uTimeEnd = 4102415999;
		ZeroMemory(szTimeDesc, sizeof(szTimeDesc));
		bWhiteMode = FALSE;
		iWinCount = 0;
		ZeroMemory(vecWin, sizeof(vecWin));
		ZeroMemory(vecClass, sizeof(vecClass));
		iWhiteWinCount = 0;
		ZeroMemory(vecWhiteWin, sizeof(vecWhiteWin));
		ZeroMemory(vecWhiteClass, sizeof(vecWhiteClass));
		uScreenTime = 0;
		ZeroMemory(szCustomComment, sizeof(szCustomComment));
		//_TIME_RANGE range;
		//range.uTimeStart = 0;
		//range.uTimeEnd = 4102415999;
		//vecTimeRange.push_back(range);
		bLoaded = FALSE;
		dwIdleTime = 0;
	}

	BOOL LoadConfig(const Json::Value& root)
	{
		USES_CONVERSION;
		
		std::string strVal = GET_JSON_STRING(root, "strUserName", "");	lstrcpy(strUserName, CA2T(strVal.c_str()).m_psz);
		strVal = GET_JSON_STRING(root, "strCompany", "");				lstrcpy(strCompany, CA2T(strVal.c_str()).m_psz);
		strVal = GET_JSON_STRING(root, "strNumber", "");				lstrcpy(strNumber, CA2T(strVal.c_str()).m_psz);
		strVal = GET_JSON_STRING(root, "strId", "");					lstrcpy(strId, CA2T(strVal.c_str()).m_psz);
		strVal = GET_JSON_STRING(root, "strUnlockPassword", "");		lstrcpy(strUnlockPassword, CA2T(strVal.c_str()).m_psz);

		//uTimeStart	= GetJsonInt(root, "uTimeStart", 0);
		//uTimeEnd	= GetJsonInt(root, "uTimeEnd", 86399);
		//uTimeEnd	= GetJsonInt(root, "uTimeEnd", 0);
		//uTimeStart	= GetJsonInt(root, "uTimeStart", 4102329600);
		//uTimeEnd	= GetJsonInt(root, "uTimeEnd", 4102415999);

		vecTimeRange.clear();
		if (root.isMember("TimeRange") && root["TimeRange"].isArray())
		{
			for (int i = 0; i < (int)root["TimeRange"].size(); i++)
			{
				Json::Value jsonItem = root["TimeRange"][i];
				_TIME_RANGE range;
				range.uTimeStart = GetJsonInt(jsonItem, "uTimeStart", 4102329600);
				range.uTimeEnd = GetJsonInt(jsonItem, "uTimeEnd", 4102415999);
				vecTimeRange.push_back(range);
			}
		}
		
		strVal = GET_JSON_STRING(root, "szTimeDesc", "");				lstrcpy(szTimeDesc, CA2T(strVal.c_str()).m_psz);
		bWhiteMode	= GetJsonInt(root, "bWhiteMode", FALSE);
		
		iWinCount	= GetJsonInt(root, "iWinCount", 0);
		for (int i = 0; i < (int)iWinCount; i++)
		{
			if (root.isMember("vecWin") && root["vecWin"].isArray())
			{
				for (int i = 0; i < (int)root["vecWin"].size(); i++)
				{
					Json::Value jsonItem = root["vecWin"][i];
					if (jsonItem.isString())
					{
						lstrcpy(vecWin[i], CA2T(jsonItem.asString().c_str()).m_psz);
					}
				}
			}
			if (root.isMember("vecClass") && root["vecClass"].isArray())
			{
				for (int i = 0; i < (int)root["vecClass"].size(); i++)
				{
					Json::Value jsonItem = root["vecClass"][i];
					if (jsonItem.isString())
					{
						lstrcpy(vecClass[i], CA2T(jsonItem.asString().c_str()).m_psz);
					}
				}
			}
		}

		iWhiteWinCount	= GetJsonInt(root, "iWhiteWinCount", 0);
		for (int i = 0; i < (int)iWhiteWinCount; i++)
		{
			if (root.isMember("vecWhiteWin") && root["vecWhiteWin"].isArray())
			{
				for (int i = 0; i < (int)root["vecWhiteWin"].size(); i++)
				{
					Json::Value jsonItem = root["vecWhiteWin"][i];
					if (jsonItem.isString())
					{
						lstrcpy(vecWhiteWin[i], CA2T(jsonItem.asString().c_str()).m_psz);
					}
				}
			}
			if (root.isMember("vecWhiteClass") && root["vecWhiteClass"].isArray())
			{
				for (int i = 0; i < (int)root["vecWhiteClass"].size(); i++)
				{
					Json::Value jsonItem = root["vecWhiteClass"][i];
					if (jsonItem.isString())
					{
						lstrcpy(vecWhiteClass[i], CA2T(jsonItem.asString().c_str()).m_psz);
					}
				}
			}
		}

		uScreenTime	= GetJsonInt(root, "uScreenTime", 0);

		if (root.isMember("CONFIG"))
		{
			configInfo.LoadConfig(root["CONFIG"]);
		}

		strVal = GET_JSON_STRING(root, "szCustomComment", "");			lstrcpy(szCustomComment, CA2T(strVal.c_str()).m_psz);
		
		dwIdleTime	= GetJsonInt(root, "IdleTime", 0);

		bLoaded = TRUE;
		return TRUE;
	}

	BOOL LoadConfig(LPCSTR pBuf)
	{
		Json::Reader reader;
		Json::Value root;
		if (!reader.parse(pBuf, root, false))
		{
			return FALSE;
		}

		BOOL bRet = LoadConfig(root);
		return bRet;
	}
	BOOL SaveConfig(Json::Value& jsonItem)
	{
		USES_CONVERSION;
		jsonItem["strUserName"]			= CT2A(strUserName).m_psz;
		jsonItem["strCompany"]			= CT2A(strCompany).m_psz;
		jsonItem["strNumber"]			= CT2A(strNumber).m_psz;
		jsonItem["strId"]				= CT2A(strId).m_psz;
		jsonItem["strUnlockPassword"]	= CT2A(strUnlockPassword).m_psz;
		//jsonItem["uTimeStart"]			= (Json::UInt)uTimeStart;
		//jsonItem["uTimeEnd"]			= (Json::UInt)uTimeEnd;
		jsonItem["szTimeDesc"]			= CT2A(szTimeDesc).m_psz;
		jsonItem["bWhiteMode"]			= (Json::UInt)bWhiteMode;

		Json::Value jsonTime(Json::arrayValue);
		for (int i = 0; i < (int)vecTimeRange.size(); i++)
		{
			Json::Value jsonTimeItem;
			jsonTimeItem["uTimeStart"] = (Json::UInt)vecTimeRange[i].uTimeStart;
			jsonTimeItem["uTimeEnd"] = (Json::UInt)vecTimeRange[i].uTimeEnd;
			jsonTime.append(jsonTimeItem);
		}
		jsonItem["TimeRange"] = jsonTime;

		jsonItem["iWinCount"] = (Json::UInt)iWinCount;
		Json::Value jsonWin(Json::arrayValue);
		Json::Value jsonClass(Json::arrayValue);
		for (int i = 0; i < (int)iWinCount; i++)
		{
			std::string strBuf = CT2A(vecWin[i]).m_psz;
			jsonWin.append(strBuf);
			strBuf = CT2A(vecClass[i]).m_psz;
			jsonClass.append(strBuf);
		}
		jsonItem["vecWin"] = jsonWin;
		jsonItem["vecClass"] = jsonClass;

		jsonItem["iWhiteWinCount"] = (Json::UInt)iWhiteWinCount;
		Json::Value jsonWhiteWin(Json::arrayValue);
		Json::Value jsonWhiteClass(Json::arrayValue);
		for (int i = 0; i < (int)iWhiteWinCount; i++)
		{
			std::string strBuf = CT2A(vecWhiteWin[i]).m_psz;
			jsonWhiteWin.append(strBuf);
			strBuf = CT2A(vecWhiteClass[i]).m_psz;
			jsonWhiteClass.append(strBuf);
		}
		jsonItem["vecWhiteWin"] = jsonWhiteWin;
		jsonItem["vecWhiteClass"] = jsonWhiteClass;

		jsonItem["uScreenTime"] = (Json::UInt)uScreenTime;

		Json::Value configValue;
		configInfo.SaveConfig(configValue);
		jsonItem["CONFIG"] = configValue;

		jsonItem["szCustomComment"] = CT2A(szCustomComment).m_psz;
		jsonItem["IdleTime"] = (Json::UInt)dwIdleTime;

		return TRUE;
	}

	BOOL SaveConfig(std::string& strBuf)
	{
		Json::Value jsonItem;
		BOOL bRet = SaveConfig(jsonItem);
		if (bRet)
		{
			Json::FastWriter writer;
			strBuf = writer.write(jsonItem);
		}
		return bRet;
	}

	CString strExtraConfigMd5;
	CString CalcSign()
	{
		std::string strJson;
		SaveConfig(strJson);
		BYTE md5[16] = {0};
		Crypt::_md5Hash(md5, (LPVOID)strJson.c_str(), strJson.size());
		CString strMd5;
		for (int j = 0; j < _countof(md5); j++)
		{
			CString strTmp;
			strTmp.Format(_T("%02x"), md5[j]);
			strMd5 += strTmp;
		}
		return strMd5;
	}
	VOID UpdateSign()
	{
		strExtraConfigMd5 = CalcSign();
	}
}
CLIENT_INFO;
#pragma pack(pop)

#endif
