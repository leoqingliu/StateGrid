#include "StdAfx.h"
#include "UserConfig.h"
#include "../Public/crypt.h"

#define INI_APPNAME _T("USER")

CUserConfig::CUserConfig(void)
{

}

CUserConfig::~CUserConfig(void)
{
}

VOID CUserConfig::Load(LPCTSTR pFile)
{
	USES_CONVERSION;
	m_strConfigPath = pFile;
	INT iCount = GetPrivateProfileInt(INI_APPNAME, _T("COUNT"), 0, pFile);
	for (INT i = 0; i < iCount; i++)
	{
		CString strIndex;
		strIndex.Format(_T("%d"), i);
		USER_INFO userInfo;
		TCHAR szTmp[MAX_PATH];
		GetPrivateProfileString(strIndex, _T("USER"), _T(""), szTmp, MAX_PATH, pFile);
		userInfo.strUser = szTmp;
		GetPrivateProfileString(strIndex, _T("PASSWORD"), _T(""), szTmp, MAX_PATH, pFile);
		LPSTR pPassword = NULL;
		SIZE_T iPasswordSize = 0;
		Crypt::ContentDec(szTmp, "PaO83OBlbLCpVjHBg8b30hNpM2JUuEH9", pPassword, iPasswordSize);
		userInfo.strPassword = A2T(pPassword);
		delete[] pPassword;
		userInfo.bAdmin = GetPrivateProfileInt(strIndex, _T("ADMIN"), FALSE, pFile);
		m_vecClient.push_back(userInfo);
	}
}

VOID CUserConfig::Save()
{
	USES_CONVERSION;
	CString strTmp;
	strTmp.Format(_T("%d"), m_vecClient.size());
	INT iCount = WritePrivateProfileString(INI_APPNAME, _T("COUNT"), strTmp, m_strConfigPath);
	int i = 0;
	for (std::vector<USER_INFO>::iterator pIter = m_vecClient.begin(); pIter != m_vecClient.end(); pIter++, i++)
	{
		CString strIndex;
		strIndex.Format(_T("%d"), i);
		WritePrivateProfileString(strIndex, _T("USER"), pIter->strUser, m_strConfigPath);
		LPSTR pPassword = T2A(pIter->strPassword);
		CString strPasswordEnc;
		Crypt::ContentEnc(pPassword, pIter->strPassword.GetLength(), strPasswordEnc, "PaO83OBlbLCpVjHBg8b30hNpM2JUuEH9");
		WritePrivateProfileString(strIndex, _T("PASSWORD"), strPasswordEnc, m_strConfigPath);
		strTmp.Format(_T("%d"), pIter->bAdmin);
		WritePrivateProfileString(strIndex, _T("ADMIN"), strTmp, m_strConfigPath);
	}
}
