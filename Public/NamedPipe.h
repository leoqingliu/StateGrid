#pragma once

#include "Pipe.h"
#include <vector>

class NamedPipe : public Pipe
{
public:
	NamedPipe(HANDLE hPipe, BOOL bServer = FALSE);
	~NamedPipe(void);

public:
	size_t Read(void *buffer, size_t len);
	size_t Write(const void *buffer, size_t len);
	void Close();

private:
	HANDLE m_hPipe;
	BOOL m_bServer;

public:
	static VOID Push(NamedPipe *pPipe);
	static VOID Check();
private:
	struct PIPE_CHECK_INFO
	{
		NamedPipe *pPipe;
		DWORD dwLastTimestamp;
	};
	static std::vector<PIPE_CHECK_INFO> m_vecPipes;
	static CCriticalSectionEx m_Lock;
};
