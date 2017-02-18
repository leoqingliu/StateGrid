#pragma once

#include <vector>
class CUserConfig
{
public:
	CUserConfig(void);
	~CUserConfig(void);

	typedef struct _USER_INFO
	{
		CString strUser;
		CString strPassword;
		BOOL bAdmin;
		_USER_INFO()
		{
			strUser = _T("");
			strPassword = _T("");
			bAdmin = FALSE;
		}
	}
	USER_INFO;
	std::vector<USER_INFO> m_vecClient;

public:
	VOID Load(LPCTSTR pFile);
	VOID Save();

private:
	CString m_strConfigPath;
};
