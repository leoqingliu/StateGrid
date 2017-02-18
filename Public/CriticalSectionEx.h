#pragma once
//
#include <windows.h>

class CCriticalSectionEx
{
public:
	CCriticalSectionEx(void);
	~CCriticalSectionEx(void);
public:
	void UnInit();
	void Lock();
	BOOL TryLock();
	void Unlock();

private:
	CRITICAL_SECTION m_CriticalSection;
};