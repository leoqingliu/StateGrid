#include "StdAfx.h"
#include "PipeServer.h"

#define BUFSIZE 512 * 1024

PipeServer::PipeServer(LPCTSTR pszName, DWORD dwMilliseconds /*= INFINITE*/)
	:m_hServerPipe(INVALID_HANDLE_VALUE)
{
	m_strName.Format(_T("\\\\.\\pipe\\%s"), pszName);
	m_dwMilliseconds = dwMilliseconds;
}

PipeServer::~PipeServer(void)
{
	Close();
}

BOOL PipeServer::Create()
{
	SECURITY_ATTRIBUTES sa; 
	SECURITY_DESCRIPTOR sd; 
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = &sd;
	sa.bInheritHandle = FALSE;

	m_hServerPipe = CreateNamedPipe(
		m_strName,				  // pipe name
		PIPE_ACCESS_DUPLEX |      // read/write access
		FILE_FLAG_OVERLAPPED,     // overlapped mode
		PIPE_TYPE_MESSAGE |       // message type pipe
		PIPE_READMODE_MESSAGE |   // message-read mode
		PIPE_WAIT,                // blocking mode
		PIPE_UNLIMITED_INSTANCES, // max. instances
		BUFSIZE,                  // output buffer size
		BUFSIZE,                  // input buffer size
		0,                        // client time-out
		&sa						  // security attributes
		);
	if (m_hServerPipe == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	return TRUE;
}

NamedPipe* PipeServer::Accept()
{
	if (m_hServerPipe == INVALID_HANDLE_VALUE) {
		Create();
	}

	OVERLAPPED overlapped;
	memset(&overlapped, 0, sizeof(OVERLAPPED));
	overlapped.hEvent = CreateEvent(0, FALSE, FALSE, NULL);

	if (ConnectNamedPipe(m_hServerPipe, &overlapped)) {
		// In success the overlapped ConnectNamedPipe() function must
		// return zero.
		return NULL;
	}
	else {
		int errCode = GetLastError();
		switch(errCode) {
		case ERROR_PIPE_CONNECTED:
			break;
		case ERROR_IO_PENDING:
			WaitForSingleObject(overlapped.hEvent, m_dwMilliseconds);
			DWORD cbRet; // Fake
			if (!GetOverlappedResult(m_hServerPipe, &overlapped, &cbRet, FALSE)) {
				return NULL;
			}
			break;
		default:
			return NULL;
		}
	}

	NamedPipe *result = new NamedPipe(m_hServerPipe);
	m_hServerPipe = INVALID_HANDLE_VALUE;
	return result;
}

VOID PipeServer::Close()
{
	if (INVALID_HANDLE_VALUE != m_hServerPipe) {
		CloseHandle(m_hServerPipe);
		m_hServerPipe = INVALID_HANDLE_VALUE;
	}
	/*
	if (m_isConnected) {
		if (DisconnectNamedPipe(hPipe)) {
		  m_isConnected = false;
		}
		else {
		  int errCode = GetLastError();
		  StringStorage errMess;
		  errMess.format(_T("DisconnectNamedPipe failed, error code = %d"), errCode);
		  throw Exception(errMess.getString());
		}
	}
	*/
}
