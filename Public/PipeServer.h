#pragma once

#include "NamedPipe.h"

class PipeServer
{
public:
	PipeServer(LPCTSTR pszName, DWORD dwMilliseconds = INFINITE);
	~PipeServer(void);

public:
	BOOL Create();
	NamedPipe* Accept();
	VOID Close();

private:
	HANDLE m_hServerPipe;
	CString m_strName;
	DWORD m_dwMilliseconds;
};
