#pragma once

#include "ThreadEx.h"
#include "Packet.h"
#include "Communicate.h"
#include "ProtoManager.h"

class CProtoManagerChild
{
public:
	CProtoManagerChild(DWORD dwClientId, CString strHostName);
	~CProtoManagerChild(void);

public:
	VOID Initialize();

private:
//	BOOL GetFileId(DWORD &dwFileId);
//	BOOL Request(Packet_t& Packet);

public:
	VOID InvokeReq(
		DWORD dwType,
		LPCTSTR lpRequest = _T("*"),
		DWORD dwRequestType = REQUEST_TYPE_MANAGE_REQUEST)
	{
		Packet_t Packet;
		Packet.dwRequestType = dwRequestType;
		Packet.dwType = dwType;
		Packet.strRequest = lpRequest;
		Packet.dwClientId = m_dwClientId;
		Packet.dwEventId = theProtoManager->m_dwEventId;
		theProtoManager->m_dwEventId++;
		m_LockThread.Lock();
		m_ReqManager.AddReq(Packet);
		m_LockThread.Unlock();
	}

public:
	CSafeDeque<Packet_t> m_ReqManager;
	DWORD m_dwClientId;
	CString m_strHostName;
	
	// Request
	#define CHILD_THREAD_COUNT 1
	CCriticalSection m_LockThread;
	CThreadEx *m_Thread[CHILD_THREAD_COUNT];
	static DWORD WINAPI _Thread(LPVOID lpThreadParameter);
	VOID Thread();

	// File
	CThreadEx *m_ThreadFile;
	static DWORD WINAPI _ThreadFile(LPVOID lpThreadParameter);
	VOID ThreadFile();
	CSafeDeque<File_Transfer_t> m_queFileTransfer;
	CCriticalSection m_LockFileTransfer;

	struct _DownloadArg
	{
		CProtoManagerChild *pProcess;
		File_Transfer_t File;
	};
public:
	VOID InvokeFile(File_Transfer_t FileTransInfo);
//	VOID DownloadFile(LPCTSTR lpRelayPath, LPCTSTR lpRemoteFilePath);
private:
//	static DWORD WINAPI _ThreadDownloadFile( LPVOID lpThreadParameter );
//	VOID ThreadDownloadFile(const File_Transfer_t& File);

	// Async
//	CThreadEx *m_ThreadAsync;
//	static DWORD WINAPI _ThreadAsync(LPVOID lpThreadParameter);
//	VOID ThreadAsync();
//	BOOL GetAsync(vector<AsyncInfo_t> &vecInfo);
};


