#include "StdAfx.h"
#include "Pipe.h"

Pipe::Pipe(void)
{
}

Pipe::~Pipe(void)
{
}

size_t Pipe::WriteByHandle(const void *buffer, size_t len, HANDLE hPipeHandle)
{
	if (INVALID_HANDLE_VALUE == hPipeHandle) {
		return 0;
	}

	DWORD result = 0;
	OVERLAPPED overlapped;
	memset(&overlapped, 0, sizeof(OVERLAPPED));
	overlapped.hEvent = CreateEvent(0, FALSE, FALSE, NULL);

	m_Lock.Lock();
	bool success = WriteFile(
		hPipeHandle,	// pipe handle
		buffer,			// message
		(DWORD)len,		// message length
		&result,		// bytes written
		&overlapped)    // overlapped
		!= 0;
	m_Lock.Unlock();
	if (!success) {
		int errCode = GetLastError();
		if (errCode == ERROR_IO_PENDING) {
			WaitForSingleObject(overlapped.hEvent, INFINITE);
			DWORD cbRet;
			m_Lock.Lock();
			if (GetOverlappedResult(hPipeHandle, &overlapped, &cbRet, FALSE) || cbRet == 0) {
				result = cbRet;
				m_Lock.Unlock();
			}
			else {
				m_Lock.Unlock();
				return 0;
			}
		}
	}
	return result;
}

size_t Pipe::ReadByHandle(void *buffer, size_t len, HANDLE hPipeHandle)
{
	if (INVALID_HANDLE_VALUE == hPipeHandle) {
		return 0;
	}
	
	DWORD result = 0;
	OVERLAPPED overlapped;
	memset(&overlapped, 0, sizeof(OVERLAPPED));
	overlapped.hEvent = CreateEvent(0, FALSE, FALSE, NULL);

	m_Lock.Lock();
	bool success = ReadFile(
		hPipeHandle,    // pipe handle
		buffer,         // message
		(DWORD)len,     // message length
		&result,        // bytes read
		&overlapped)	// overlapped
		!= 0;
	m_Lock.Unlock();
	if (!success) {
		DWORD errCode = GetLastError();
		if (errCode == ERROR_IO_PENDING) {
			WaitForSingleObject(overlapped.hEvent, INFINITE);
			DWORD cbRet = 0;
			m_Lock.Lock();
			if (GetOverlappedResult(hPipeHandle, &overlapped, &cbRet, FALSE) && cbRet != 0) {
				m_Lock.Unlock();
				result = cbRet;
			}
			else {
				m_Lock.Unlock();
				return 0;
			}
		}
	}
	return result;
}
