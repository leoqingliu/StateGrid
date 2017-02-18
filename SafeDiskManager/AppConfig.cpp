#include "StdAfx.h"
#include "AppConfig.h"
#include "../Public/crypt.h"
#include "../Public/Utils.h"

#define INI_APPNAME _T("REMOTE")

CAppConfig::CAppConfig(void)
{
// 	TCHAR szPath[MAX_PATH] = {0};
// 	GetModuleFileName(NULL, szPath, MAX_PATH);
// 	*StrRChr(szPath, szPath + lstrlen(szPath), _T('\\')) = 0;
// 	lstrcat(szPath, _T("\\App.ini"));
// 	m_strAppPath = szPath;
}

CAppConfig::~CAppConfig(void)
{
}

VOID CAppConfig::Load(LPCTSTR pszAppPath, LPCTSTR pszConfigPath)
{
	m_strAppPath = pszAppPath;
	m_strConfigPath = pszConfigPath;

	/*
	strcpy(m_Info.szKey, "6dNfg8Upn5fBzGgj8licQHblQvLnUY19z5zcNKNFdsDhUzuI8otEsBODrzFCqCKr");
	lstrcpy(m_Info.szHost, _T("db-sf-herring01.db01.baidu.com"));
	m_Info.uPort = 8888;
	lstrcpy(m_Info.szPage, _T("/safe/record.php"));
	*/

	USES_CONVERSION;
	GetPrivateProfileString(
		INI_APPNAME, _T("HOST"), _T("127.0.0.1"), m_Info.szHost, _countof(m_Info.szHost), m_strAppPath);
	TCHAR szKey[128];
	GetPrivateProfileString(
		INI_APPNAME, _T("KEY"), _T("6dNfg8Upn5fBzGgj8licQHblQvLnUY19z5zcNKNFdsDhUzuI8otEsBODrzFCqCKr"), szKey, _countof(szKey), m_strAppPath);
	StrCpyA(m_Info.szKey, T2A(szKey));
	m_Info.uPort = GetPrivateProfileInt(
		INI_APPNAME, _T("PORT"), 8900, m_strAppPath);
	GetPrivateProfileString(
		INI_APPNAME, _T("PAGE"), _T("/safe/record.php"), m_Info.szPage, _countof(m_Info.szPage), m_strAppPath);
	m_Info.uTimeout = GetPrivateProfileInt(
		INI_APPNAME, _T("TIMEOUT"), 15, m_strAppPath);
	GetPrivateProfileString(
		INI_APPNAME, _T("LOCAL"), _T("C:\\MacManager"), m_Info.szLocalDir, _countof(m_Info.szLocalDir), m_strAppPath);

	m_mapClient.clear();
	int iBufferSize = 1024 * 1024 * 1;
	LPTSTR pBuffer = new TCHAR[iBufferSize];
	INT iCount = GetPrivateProfileInt(INI_APPNAME, _T("CLIENT_INFO_COUNT"), 0, m_strAppPath);
	for (INT i = 0; i < iCount; i++)
	{
		CString strIndex;
		strIndex.Format(_T("CLIENT_INFO_%d"), i);
		DWORD dwId = GetPrivateProfileInt(strIndex, _T("CLIENT_ID"), 0, m_strAppPath);
		_CLIENT_INFO info;
		//TCHAR szBuf[MAX_PATH];
		GetPrivateProfileString(strIndex, _T("USER"), _T(""), info.strUserName, MAX_PATH, m_strAppPath);
		GetPrivateProfileString(strIndex, _T("COMPANY"), _T(""), info.strCompany, MAX_PATH, m_strAppPath);
		GetPrivateProfileString(strIndex, _T("NUMBER"), _T(""), info.strNumber, MAX_PATH, m_strAppPath);
		GetPrivateProfileString(strIndex, _T("CARD_ID"), _T(""), info.strId, MAX_PATH, m_strAppPath);
		//info.uTimeStart = GetPrivateProfileInt(strIndex, _T("TIME_START"), 0, m_strAppPath);
		//info.uTimeEnd = GetPrivateProfileInt(strIndex, _T("TIME_END"), 86400, m_strAppPath);
		info.vecTimeRange.clear();
		int iTimeCount = GetPrivateProfileInt(strIndex, _T("TIME_COUNT"), 0, m_strAppPath);
		for (int j = 0; j < iTimeCount; j++)
		{
			_TIME_RANGE range;
			TCHAR szTmp[MAX_PATH];
			CString strSubIndex;
			strSubIndex.Format(_T("TIME_START_%d"), j);
			GetPrivateProfileString(strIndex, strSubIndex, _T("4102329600"), szTmp, MAX_PATH, m_strAppPath);
			range.uTimeStart = _tstol(szTmp);
			strSubIndex.Format(_T("TIME_END_%d"), j);
			GetPrivateProfileString(strIndex, strSubIndex, _T("4102415999"), szTmp, MAX_PATH, m_strAppPath);
			range.uTimeEnd = _tstol(szTmp);
			info.vecTimeRange.push_back(range);
		}

		GetPrivateProfileString(strIndex, _T("PASSWORD"), _T("685768op"), info.strUnlockPassword, MAX_PATH, m_strAppPath);
		info.uScreenTime = GetPrivateProfileInt(strIndex, _T("SCREEN_TIME"), 0, m_strAppPath);
		info.bWhiteMode = GetPrivateProfileInt(strIndex, _T("WHITE_MODE"), FALSE, m_strAppPath);
		GetPrivateProfileString(strIndex, _T("CUSTOMCOMMENT"), _T(""), info.szCustomComment, MAX_PATH, m_strAppPath);
		
		info.iWinCount = 0;
		int iWinCount = GetPrivateProfileInt(strIndex, _T("WIN_COUNT"), 0, m_strAppPath);
		for (int j = 0; j < iWinCount; j++)
		{
			CString strSubIndex;
			strSubIndex.Format(_T("WIN_%d"), j);
			GetPrivateProfileString(strIndex, strSubIndex, _T(""), info.vecWin[info.iWinCount], MAX_PATH, m_strAppPath);
			strSubIndex.Format(_T("CLASS_%d"), j);
			GetPrivateProfileString(strIndex, strSubIndex, _T(""), info.vecClass[info.iWinCount], MAX_PATH, m_strAppPath);
			info.iWinCount++;
		}

		info.iWhiteWinCount = 0;
		iWinCount = GetPrivateProfileInt(strIndex, _T("WHITE_WIN_COUNT"), 0, m_strAppPath);
		for (int j = 0; j < iWinCount; j++)
		{
			CString strSubIndex;
			strSubIndex.Format(_T("WHITE_WIN_%d"), j);
			GetPrivateProfileString(strIndex, strSubIndex, _T(""), info.vecWhiteWin[info.iWhiteWinCount], MAX_PATH, m_strAppPath);
			strSubIndex.Format(_T("WHITE_CLASS_%d"), j);
			GetPrivateProfileString(strIndex, strSubIndex, _T(""), info.vecWhiteClass[info.iWhiteWinCount], MAX_PATH, m_strAppPath);
			info.iWhiteWinCount++;
		}

		ZeroMemory(pBuffer, iBufferSize * sizeof(TCHAR));
		DWORD dwRet = GetPrivateProfileString(strIndex, _T("CONFIG"), _T(""), pBuffer, iBufferSize, m_strAppPath);
		if (0 == dwRet)
		{
			info.configInfo.LoadIni(m_strConfigPath);

			std::string strBuffer;
			info.configInfo.SaveConfig(strBuffer);

			size_t iCompressedDataSize = 0;
			LPBYTE pCompressedData = Utils::CompressData((LPBYTE)strBuffer.c_str(), strBuffer.size(), iCompressedDataSize);
			LPSTR pBuf = (LPSTR)Crypt::_base64Encode(pCompressedData, iCompressedDataSize, NULL);
			CString strContent = CA2T(pBuf).m_psz;
			free(pBuf);
			free(pCompressedData);
			WritePrivateProfileString(strIndex, _T("CONFIG"), strContent, m_strAppPath);
		}
		else
		{
			std::string strBuffer = CT2A(pBuffer).m_psz;
			SIZE_T iBufLen = 0;
			LPSTR pBuf = (LPSTR)Crypt::_base64Decode(strBuffer.c_str(), strBuffer.size(), &iBufLen);
			size_t iDeCompressedSize = 0;
			LPBYTE pDecompressedData = Utils::DeCompressData((LPBYTE)pBuf, iBufLen, iDeCompressedSize);
			info.configInfo.LoadConfig((LPSTR)pDecompressedData);
			free(pBuf);
			free(pDecompressedData);
		}

		if (0 != dwId)
		{
			std::map<DWORD, _CLIENT_INFO>::iterator pIter = m_mapClient.find(dwId);
			if (pIter == m_mapClient.end())
			{
				// Calc Sign
				info.UpdateSign();

				// New
				m_mapClient.insert(std::map<DWORD, _CLIENT_INFO>::value_type(dwId, info));
			}
		}
	}
	
	m_vecGroup.clear();
	CString strTmp;
	strTmp.Format(_T("%d"), m_vecGroup.size());
	iCount = GetPrivateProfileInt(_T("GROUP"), _T("GROUP_INFO_COUNT"), 0, m_strAppPath);
	int i = 0;
	for (int i = 0; i < iCount; i++)
	{
		CString strIndex;
		strIndex.Format(_T("GROUP_INFO_%d"), i);
		GetPrivateProfileString(strIndex, _T("NAME"), _T(""), pBuffer, iBufferSize, m_strAppPath);
		CString strName = pBuffer;
		if (strName.IsEmpty())
			continue;
		GetPrivateProfileString(strIndex, _T("CLIENT_ID"), _T(""), pBuffer, iBufferSize, m_strAppPath);
		DWORD dwGroupId = GetPrivateProfileInt(strIndex, _T("GROUP_ID"), 0, m_strAppPath);
		CString strVal = pBuffer;
		std::vector<DWORD> vecIds;
		std::vector<CString> vecTokens = Utils::Split(strVal, _T(","));
		for (int j = 0; j < (int)vecTokens.size(); j++)
		{
			DWORD dwClientId = _tstol(vecTokens[j]);
			if (0 != dwClientId)
				vecIds.push_back(dwClientId);
		}
		std::vector<GROUP_INFO>::iterator pIter = FindGroup(strName);
		if (pIter == m_vecGroup.end())
		{
			GROUP_INFO groupInfo;
			groupInfo.strName = strName;
			groupInfo.dwGroupId = dwGroupId;
			groupInfo.vecIds = vecIds;
			m_vecGroup.push_back(groupInfo);
		}
		else
		{
			pIter->vecIds = vecIds;
		}
	}

	delete[] pBuffer;
}

VOID CAppConfig::Save()
{
	USES_CONVERSION;
	WritePrivateProfileString(INI_APPNAME, _T("HOST"), m_Info.szHost, m_strAppPath);
	WritePrivateProfileString(INI_APPNAME, _T("KEY"), A2T(m_Info.szKey), m_strAppPath);
	CString strInfo;
	strInfo.Format(_T("%d"), m_Info.uPort);
	WritePrivateProfileString(INI_APPNAME, _T("PORT"), strInfo, m_strAppPath);
	WritePrivateProfileString(INI_APPNAME, _T("PAGE"), m_Info.szPage, m_strAppPath);
	strInfo.Format(_T("%d"), m_Info.uTimeout);
	WritePrivateProfileString(INI_APPNAME, _T("TIMEOUT"), strInfo, m_strAppPath);
	WritePrivateProfileString(INI_APPNAME, _T("LOCAL"), m_Info.szLocalDir, m_strAppPath);

	CString strTmp;
	strTmp.Format(_T("%d"), m_mapClient.size());
	WritePrivateProfileString(INI_APPNAME, _T("CLIENT_INFO_COUNT"), strTmp, m_strAppPath);
	int i = 0;
	for (std::map<DWORD, _CLIENT_INFO>::iterator pIter = m_mapClient.begin(); pIter != m_mapClient.end(); pIter++, i++)
	{
		// Calc Sign
		pIter->second.UpdateSign();

		// Items
		CString strIndex;
		strIndex.Format(_T("CLIENT_INFO_%d"), i);
		strTmp.Format(_T("%d"), pIter->first);
		WritePrivateProfileString(strIndex, _T("CLIENT_ID"), strTmp, m_strAppPath);
		WritePrivateProfileString(strIndex, _T("USER"), pIter->second.strUserName, m_strAppPath);
		WritePrivateProfileString(strIndex, _T("COMPANY"), pIter->second.strCompany, m_strAppPath);
		WritePrivateProfileString(strIndex, _T("NUMBER"), pIter->second.strNumber, m_strAppPath);
		WritePrivateProfileString(strIndex, _T("CARD_ID"), pIter->second.strId, m_strAppPath);
		//strTmp.Format(_T("%u"), pIter->second.uTimeStart);
		//WritePrivateProfileString(strIndex, _T("TIME_START"), strTmp, m_strAppPath);
		//strTmp.Format(_T("%u"), pIter->second.uTimeEnd);
		//WritePrivateProfileString(strIndex, _T("TIME_END"), strTmp, m_strAppPath);
		strTmp.Format(_T("%u"), pIter->second.vecTimeRange.size());
		WritePrivateProfileString(strIndex, _T("TIME_COUNT"), strTmp, m_strAppPath);
		for (int j = 0; j < (int)pIter->second.vecTimeRange.size(); j++)
		{
			const _TIME_RANGE &range = pIter->second.vecTimeRange[j];
			CString strSubIndex;
			strSubIndex.Format(_T("TIME_START_%d"), j);
			strTmp.Format(_T("%u"), range.uTimeStart);
			WritePrivateProfileString(strIndex, strSubIndex, strTmp, m_strAppPath);
			strSubIndex.Format(_T("TIME_END_%d"), j);
			strTmp.Format(_T("%u"), range.uTimeEnd);
			WritePrivateProfileString(strIndex, strSubIndex, strTmp, m_strAppPath);
		}


		WritePrivateProfileString(strIndex, _T("PASSWORD"), pIter->second.strUnlockPassword, m_strAppPath);
		strTmp.Format(_T("%u"), pIter->second.uScreenTime);
		WritePrivateProfileString(strIndex, _T("SCREEN_TIME"), strTmp, m_strAppPath);
		strTmp.Format(_T("%u"), pIter->second.bWhiteMode);
		WritePrivateProfileString(strIndex, _T("WHITE_MODE"), strTmp, m_strAppPath);
		WritePrivateProfileString(strIndex, _T("CUSTOMCOMMENT"), pIter->second.szCustomComment, m_strAppPath);

		strTmp.Format(_T("%u"), pIter->second.iWinCount);
		WritePrivateProfileString(strIndex, _T("WIN_COUNT"), strTmp, m_strAppPath);
		for (int j = 0; j < (int)pIter->second.iWinCount; j++)
		{
			CString strSubIndex;
			strSubIndex.Format(_T("WIN_%d"), j);
			WritePrivateProfileString(strIndex, strSubIndex, pIter->second.vecWin[j], m_strAppPath);
			strSubIndex.Format(_T("CLASS_%d"), j);
			WritePrivateProfileString(strIndex, strSubIndex, pIter->second.vecClass[j], m_strAppPath);
		}

		strTmp.Format(_T("%u"), pIter->second.iWhiteWinCount);
		WritePrivateProfileString(strIndex, _T("WHITE_WIN_COUNT"), strTmp, m_strAppPath);
		for (int j = 0; j < (int)pIter->second.iWhiteWinCount; j++)
		{
			CString strSubIndex;
			strSubIndex.Format(_T("WHITE_WIN_%d"), j);
			WritePrivateProfileString(strIndex, strSubIndex, pIter->second.vecWhiteWin[j], m_strAppPath);
			strSubIndex.Format(_T("WHITE_CLASS_%d"), j);
			WritePrivateProfileString(strIndex, strSubIndex, pIter->second.vecWhiteClass[j], m_strAppPath);
		}

		strTmp.Format(_T("%u"), pIter->second.iWhiteWinCount);
		WritePrivateProfileString(strIndex, _T("WHITE_WIN_COUNT"), strTmp, m_strAppPath);

		std::string strBuffer;
		pIter->second.configInfo.SaveConfig(strBuffer);
		size_t iCompressedDataSize = 0;
		LPBYTE pCompressedData = Utils::CompressData((LPBYTE)strBuffer.c_str(), strBuffer.size(), iCompressedDataSize);
		LPSTR pBuf = (LPSTR)Crypt::_base64Encode(pCompressedData, iCompressedDataSize, NULL);
		CString strContent = CA2T(pBuf).m_psz;
		free(pBuf);
		free(pCompressedData);
		WritePrivateProfileString(strIndex, _T("CONFIG"), strContent, m_strAppPath);
	}

	strTmp.Format(_T("%d"), m_vecGroup.size());
	WritePrivateProfileString(_T("GROUP"), _T("GROUP_INFO_COUNT"), strTmp, m_strAppPath);
	i = 0;
	for (std::vector<GROUP_INFO>::iterator pIter = m_vecGroup.begin(); pIter != m_vecGroup.end(); pIter++, i++)
	{
		CString strIndex;
		strIndex.Format(_T("GROUP_INFO_%d"), i);
		WritePrivateProfileString(strIndex, _T("NAME"), pIter->strName, m_strAppPath);
		strTmp = _T("");
		for (int j = 0; j < (int)pIter->vecIds.size(); j++)
		{
			if (!strTmp.IsEmpty())
				strTmp += _T(",");
			CString strClientTmp;
			strClientTmp.Format(_T("%d"), pIter->vecIds[j]);
			strTmp += strClientTmp;
		}
		WritePrivateProfileString(strIndex, _T("CLIENT_ID"), strTmp, m_strAppPath);
		strTmp.Format(_T("%d"), pIter->dwGroupId);
		WritePrivateProfileString(strIndex, _T("GROUP_ID"), strTmp, m_strAppPath);
	}
}

CAppConfig theConfig;