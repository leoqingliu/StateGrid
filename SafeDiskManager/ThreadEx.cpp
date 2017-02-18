#include "StdAfx.h"
#include "ThreadEx.h"

CThreadEx::CThreadEx(LPTHREAD_START_ROUTINE pRoutine, LPVOID pParam /*= NULL*/)
{
	m_pRoutine = pRoutine;
	m_pParam = pParam;
	m_bWantExit = false;
	m_hExited = ::CreateEvent(NULL, TRUE, TRUE, NULL);
}

CThreadEx::~CThreadEx(void)
{
	m_bWantExit = true;
	WaitForSingleObject(m_hExited, INFINITE);
}

DWORD WINAPI CThreadEx::_Thread(LPVOID lpThreadParameter)
{
	CThreadEx *pThread = (CThreadEx*)lpThreadParameter;
	pThread->Thread();
	return 0;
}

VOID CThreadEx::Initialize()
{
	DWORD dwThreadId;
	CreateThread(NULL, 0, _Thread, this, 0, &dwThreadId);
}

VOID CThreadEx::Thread()
{
	ResetEvent(m_hExited);
	while (!m_bWantExit)
	{
		BOOL bExit = m_pRoutine(m_pParam);
		if (bExit != 0)
		{
			break;
		}
	}
	SetEvent(m_hExited);
}
