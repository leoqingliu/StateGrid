#ifndef _PACKET_H
#define _PACKET_H

struct Packet_t
{
	DWORD dwClientId;
	DWORD dwRequestType;
	DWORD dwType;
	CString strRequest;
	CString strResponse;
	INT iResponseRet;
	DWORD dwEventId;
	HANDLE hEvent;
	HANDLE hSuccess;
	BOOL bSentAlready;
	Packet_t()
	{
		dwClientId = 0;
		dwRequestType = 0;
		dwType = 0;
		strRequest = _T("");
		strResponse = _T("");
		iResponseRet = 999;
		dwEventId = 0;
		hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		hSuccess = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		bSentAlready = FALSE;
	}
};


extern CSafeDeque<Packet_t> thePacketManager;
extern CCriticalSection thePacketManagerLock;
#endif