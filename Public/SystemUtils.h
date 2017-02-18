#pragma once

class SystemUtils
{
public:
	SystemUtils(void);
	~SystemUtils(void);
	static DWORD FindSessionPid(LPCTSTR lpProcessName, DWORD dwSessionId);
	static BOOL CreateNormalProcessInSystem(LPCTSTR lpCommandLine, BOOL bHide = TRUE, BOOL bUseAuc = TRUE, BOOL bWaitFinish = FALSE);
	static BOOL CreateNormalProcess(LPCTSTR lpCommandLine, BOOL bHide = TRUE, BOOL bWaitFinish = FALSE);
	enum SYSTEM_OP_TYPE
	{
		SYSTEM_OP_TYPE_SHUTDOWN = 0,
		SYSTEM_OP_TYPE_RESTART,
		SYSTEM_OP_TYPE_LOGOFF,

		SYSTEM_OP_TYPE_MAX
	};
	static BOOL SystemShutdown(SYSTEM_OP_TYPE eType);
	static BOOL Extractfile(HMODULE hInstance, LPCTSTR lpType, LPCTSTR lpName, LPCTSTR lpPath);
};
