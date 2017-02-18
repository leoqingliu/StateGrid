#pragma once

#include <deque>
#include <vector>
#include <map>
#include "ThreadEx.h"
#include "Packet.h"
#include "Communicate.h"
#include "../Public/BinStorage.h"

class CProtoManagerChild;
class CProtoManager
{
public:
	CProtoManager(void);
	~CProtoManager(void);

public:
	VOID Initialize();

public:
//	BOOL GetClient(std::vector<ClientInfo_t>& vecClient);
//	BOOL GetRelayFileInfo(RelayFileInfo_t &Info);
	std::vector<ClientInfo_t> GetResClient(BOOL bIngnoreOffline = TRUE);
	CProtoManagerChild *GetChild(DWORD dwClientId);
	BOOL SetClientOnline(DWORD dwClient, BOOL bOnline);

protected:
	std::vector<ClientInfo_t> m_vecClientInfo;
	CCriticalSection m_LockClient;
	CSafeDeque<Packet_t> m_PackManager;
	CThreadEx *m_pThread;
	std::map<DWORD, CProtoManagerChild*> m_MapChild;
	static DWORD WINAPI _Thread(LPVOID lpThreadParameter);
	VOID Thread();

public:
//	VOID InvokeClient();
//	VOID InvokeFileUpDownInfo();

	VOID InvokeDir(DWORD dwClientId, LPCTSTR lpDir);
	VOID InvokeCmd(DWORD dwClientId, LPCTSTR lpCmd);
	VOID InvokeVersion(DWORD dwClientId);
	VOID InvokePassword(DWORD dwClientId);
//	VOID DownloadFile(DWORD dwClientId, LPCTSTR lpRelayPath, LPCTSTR lpRemoteFilePath);
	VOID InvokeProcess(DWORD dwClientId);
	VOID InvokeService(DWORD dwClientId);
	VOID InvokeNetwork(DWORD dwClientId);
	VOID InvokeClientLockInfo(DWORD dwClientId, LPCTSTR pContent);
	VOID InvokeConfigInfo(DWORD dwClientId, LPCTSTR pContent);


public:
	VOID CheckOnline();
	VOID CheckConfig(DWORD &dwCount);
	VOID HttpdUpdateStatus(DWORD dwClientIdParam, CString strIpIn, CString strIpOut, CString strHostname, CString strDeviceSeiral, CString strComment, std::string strSignParam, DWORD bLockState, DWORD dwVersion);
	BinStorage::STORAGE *HttpdProcessRequest(DWORD dwClientId);
	BinStorage::STORAGE *HttpdProcessResponse(DWORD dwEventId, DWORD dwClientId, CString strResponse, INT iResponseRet);
	BinStorage::STORAGE *HttpdProcessResponseAsync(DWORD dwClientId, DWORD dwType, CString strResponse);
	DWORD m_dwEventId;
	DWORD m_dwFileId;
	std::vector<DWORD> m_vecIdsToUpdateConfig;
	struct IDS_UPGRADE_INFO
	{
		CString strPath;
		CString strName;
		DWORD dwClientId;
	};
	std::vector<IDS_UPGRADE_INFO> m_vecIdsToUpgrade;
};

extern CProtoManager *theProtoManager;