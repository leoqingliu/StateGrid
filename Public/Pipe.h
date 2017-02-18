#pragma once

#include "CriticalSectionEx.h"
class Pipe
{
public:
	Pipe(void);
	~Pipe(void);

public:
	size_t WriteByHandle(const void *buffer, size_t len, HANDLE pipeHandle);
	size_t ReadByHandle(void *buffer, size_t len, HANDLE pipeHandle);

protected:
	CCriticalSectionEx m_Lock;
};
