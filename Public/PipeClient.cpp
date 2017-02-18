#include "StdAfx.h"
#include "PipeClient.h"

PipeClient::PipeClient(void)
{
}

PipeClient::~PipeClient(void)
{
}

NamedPipe * PipeClient::Connect(LPCTSTR name)
{
	CString pipeName;
	pipeName.Format(_T("\\\\.\\pipe\\%s"), name);

	HANDLE hPipe;
	hPipe = CreateFile(
		pipeName,			   // pipe name
		GENERIC_READ |         // read and write access
		GENERIC_WRITE,
		0,                     // no sharing
		NULL,                  // default security attributes
		OPEN_EXISTING,         // opens existing pipe
		FILE_FLAG_OVERLAPPED,  // asynchronous mode
		NULL);                 // no template file

	if (hPipe == INVALID_HANDLE_VALUE) {
		return NULL;
	}

	DWORD dwMode = PIPE_READMODE_MESSAGE;
	if (!SetNamedPipeHandleState(
		hPipe,	   // pipe handle
		&dwMode,   // new pipe mode
		NULL,      // don't set maximum bytes
		NULL)      // don't set maximum time
		) {
		return NULL;
	}

	return new NamedPipe(hPipe, false);
}
