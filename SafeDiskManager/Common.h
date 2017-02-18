#ifndef _COMMON_DEF_H
#define _COMMON_DEF_H

#ifndef SAFE_DELETE_AR
#define SAFE_DELETE_AR(ptr) if (ptr) { delete [] ptr; ptr = NULL; }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(ptr) if (ptr) { delete ptr; ptr = NULL; }
#endif

#define MUTEX_FLAG _T("SafeDiskManager_Mutex")
#define FIND_FLAG  _T("SafeDiskManager_Dlg_Find")

#include <WinInet.h>

struct RelayInfo_t
{
	char   szKey[128];
	TCHAR  szHost[128];
	INTERNET_PORT uPort;
	TCHAR  szPage[128];
	DWORD  uTimeout;
	TCHAR  szLocalDir[128];
};
extern RelayInfo_t theRelayInfo;

struct RelayFileInfo_t
{
	RelayFileInfo_t()
	{
		bUpdated = FALSE;
	}
	BOOL bUpdated;
	BOOL  bFileTrans;
	TCHAR szUpalodScript[128];
	TCHAR szUpalodDir[128];
};
extern RelayFileInfo_t theRelayUpDownInfo;

struct File_Transfer_t
{
	DWORD dwClientId;
	CString strLocalPath;
	CString strLocalName;
	CString strRemoteDir;
	BOOL bToRemote;
};

struct ClientInfo_t
{
	DWORD dwClientId;
	TCHAR szIPOut[128];
	TCHAR szIPIn[128];
	TCHAR szTime[128];
	TCHAR szHostname[128];
	TCHAR szDeviceSerial[1024];
	TCHAR szComment[256];
	DWORD dwVersion;
	BOOL bOnline;
	DWORD dwLastUpdatedTime;
	BOOL bLocked;
	ClientInfo_t()
	{
		ZeroMemory(this, sizeof(ClientInfo_t));
	}
};

/*
struct AsyncInfo_t
{
	DWORD dwType;
	CString strContent;
	DWORD dwClientId;
};
*/

#include <deque>
#include <vector>

template<class _Type>
class CSafeDeque
{
	// Result
public:
	std::deque<_Type> m_queResPacket;
	CCriticalSection m_ResLock;

public:
	BOOL Get(_Type &Packet)
	{
		return GetReq(Packet);
	}
	VOID Add(const _Type &Packet)
	{
		AddReq(Packet);
	}

public:
	BOOL GetRes(_Type &Packet)
	{
		return _Get(m_queResPacket, m_ResLock, Packet);
	}
	VOID AddRes(const _Type &Packet)
	{
		_Add(m_queResPacket, m_ResLock, Packet);
	}

	// Request
public:
	std::deque<_Type> m_queReqPacket;
	CCriticalSection m_ReqLock;

public:
	BOOL GetReq(_Type &Packet)
	{
		return _Get(m_queReqPacket, m_ReqLock, Packet);
	}
	VOID AddReq(const _Type &Packet)
	{
		_Add(m_queReqPacket, m_ReqLock, Packet);
	}
	BOOL GetReqFirst(_Type *&Packet)
	{
		return GetFirst(m_queReqPacket, Packet, m_ReqLock);
	}
	BOOL PopReqFirst()
	{
		return PopFirst(m_queReqPacket, m_ReqLock);
	}

private:
	BOOL _Get(std::deque<_Type> &quePacket, CCriticalSection &Lock, _Type &Packet)
	{
		Lock.Lock();
		BOOL bExist = FALSE;
		if (quePacket.size())
		{
			Packet = quePacket[0];
			quePacket.pop_front();
			bExist = TRUE;
		}
		Lock.Unlock();
		return bExist;
	}
	VOID _Add(std::deque<_Type> &quePacket, CCriticalSection &Lock, const _Type &Packet)
	{
		Lock.Lock();
		quePacket.push_back(Packet);
		Lock.Unlock();
	}

	BOOL GetFirst(std::deque<_Type> &quePacket, _Type *&Packet, CCriticalSection &Lock)
	{
		Lock.Lock();
		BOOL bExist = FALSE;
		if (quePacket.size())
		{
			Packet = &quePacket[0];
			bExist = TRUE;
		}
		Lock.Unlock();
		return bExist;
	}
	BOOL PopFirst(std::deque<_Type> &quePacket, CCriticalSection &Lock)
	{
		Lock.Lock();
		BOOL bExist = FALSE;
		if (quePacket.size())
		{
			quePacket.pop_front();
			bExist = TRUE;
		}
		Lock.Unlock();
		return bExist;
	}
};

#include <Debug.h>
#include "AppConfig.h"

#endif