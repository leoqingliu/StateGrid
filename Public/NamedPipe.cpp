#include "StdAfx.h"
#include "NamedPipe.h"
#include "Utils.h"

std::vector<NamedPipe::PIPE_CHECK_INFO> NamedPipe::m_vecPipes;
CCriticalSectionEx NamedPipe::m_Lock;

NamedPipe::NamedPipe(HANDLE hPipe, BOOL bServer /*= FALSE*/)
	:m_hPipe(hPipe)
	,m_bServer(bServer)
{
}

NamedPipe::~NamedPipe(void)
{
	Close();
}

size_t NamedPipe::Read(void *buffer, size_t len)
{
	return ReadByHandle(buffer, len, m_hPipe);
}

size_t NamedPipe::Write(const void *buffer, size_t len)
{
	return WriteByHandle(buffer, len, m_hPipe);
}

void NamedPipe::Close()
{
	m_Lock.Lock();
	if (m_bServer) {
		if (DisconnectNamedPipe(m_hPipe) == 0) {
		}
	}

	if (m_hPipe != INVALID_HANDLE_VALUE) {
		CloseHandle(m_hPipe);
		m_hPipe = INVALID_HANDLE_VALUE;
	}
	m_Lock.Unlock();
}

VOID NamedPipe::Push(NamedPipe *pPipe)
{
	m_Lock.Lock();
	PIPE_CHECK_INFO info;
	info.pPipe = pPipe;
	info.dwLastTimestamp = Utils::GetCurrentTimestamp();
	m_vecPipes.push_back(info);
	m_Lock.Unlock();
}

VOID NamedPipe::Check()
{
	m_Lock.Lock();
	DWORD dwTime = Utils::GetCurrentTimestamp();
	for (std::vector<NamedPipe::PIPE_CHECK_INFO>::iterator pIter = m_vecPipes.begin(); pIter != m_vecPipes.end();)
	{
		if (pIter->dwLastTimestamp + 10 < dwTime)
		{
			delete pIter->pPipe;
			pIter = m_vecPipes.erase(pIter);
		}
		else
		{
			pIter++;
		}
	}
	m_Lock.Unlock();
}
