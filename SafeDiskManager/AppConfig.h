#pragma once

#include <map>
#include "../ConnConf.h"

class CAppConfig
{
public:
	CAppConfig(void);
	~CAppConfig(void);

public:
	RelayInfo_t m_Info;
	std::map<DWORD, _CLIENT_INFO> m_mapClient;
	struct GROUP_INFO
	{
		DWORD dwGroupId;
		CString strName;
		std::vector<DWORD> vecIds;
		std::vector<DWORD>::iterator FindId(DWORD dwClientId)
		{
			for (std::vector<DWORD>::iterator pIter = vecIds.begin(); pIter != vecIds.end(); pIter++)
			{
				if (*pIter == dwClientId)
				{
					return pIter;
				}
			}
			return vecIds.end();
		}
		GROUP_INFO()
		{
			dwGroupId = 0;
		}
	};
	std::vector<GROUP_INFO> m_vecGroup;
	std::vector<GROUP_INFO>::iterator FindGroup(CString strName)
	{
		for (std::vector<GROUP_INFO>::iterator pIter = m_vecGroup.begin(); pIter != m_vecGroup.end(); pIter++)
		{
			if (strName == pIter->strName)
			{
				return pIter;
			}
		}
		return m_vecGroup.end();
	}
	std::vector<GROUP_INFO>::iterator FindGroup(DWORD dwId)
	{
		for (std::vector<GROUP_INFO>::iterator pIter = m_vecGroup.begin(); pIter != m_vecGroup.end(); pIter++)
		{
			std::vector<DWORD>::iterator pIdIter = pIter->FindId(dwId);
			if (pIdIter != pIter->vecIds.end())
			{
				return pIter;
			}
		}
		return m_vecGroup.end();
	}
	CCriticalSection m_GroupLock;

public:
	VOID Load(LPCTSTR pszAppPath, LPCTSTR pszConfigPath);
	VOID Save();

protected:
	CString m_strAppPath;
	CString m_strConfigPath;
};

extern CAppConfig theConfig;