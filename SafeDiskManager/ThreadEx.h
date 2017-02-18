#pragma once

class CThreadEx
{
public:
	CThreadEx(LPTHREAD_START_ROUTINE pRoutine, LPVOID pParam = NULL);
	~CThreadEx(void);

public:
	VOID Initialize();

private:
	HANDLE m_hExited;
	bool m_bWantExit;
	static DWORD WINAPI _Thread(LPVOID lpThreadParameter);
	VOID Thread();

	LPTHREAD_START_ROUTINE m_pRoutine;
	LPVOID m_pParam;
};
