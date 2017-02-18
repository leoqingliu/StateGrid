#include "StdAfx.h"
#include "CriticalSectionEx.h"


CCriticalSectionEx::CCriticalSectionEx(void)
{
	InitializeCriticalSection(&m_CriticalSection);
}

CCriticalSectionEx::~CCriticalSectionEx(void)
{
	DeleteCriticalSection(&m_CriticalSection);
}

void CCriticalSectionEx::Lock()
{
	EnterCriticalSection(&m_CriticalSection);
}

BOOL CCriticalSectionEx::TryLock()
{
#if _MSC_VER > 1200
	return TryEnterCriticalSection(&m_CriticalSection);
#else
	return FALSE;
#endif
}

void CCriticalSectionEx::Unlock()
{
	LeaveCriticalSection(&m_CriticalSection);
}