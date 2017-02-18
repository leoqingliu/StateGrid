// SafeProcess.h: interface for the CSafeProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAFEPROCESS_H__4CD9E869_74BB_4DAE_B533_C597DC047631__INCLUDED_)
#define AFX_SAFEPROCESS_H__4CD9E869_74BB_4DAE_B533_C597DC047631__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HttpQuery.h"
#include "BinStorage.h"
#include "crypt.h"

#include "RedCmd.h"
#include "CriticalSectionEx.h"
#include <atlstr.h>
#include "../ConnConf.h"
#include "../Public/PipeServer.h"

extern CommunicationInfo theInfo;

class CSafeProcess : public NCmd::CRedCmdCallBack
{
public:
	CSafeProcess();
	virtual ~CSafeProcess();

public:
	void Do(HINSTANCE hInstance);
	static DWORD WINAPI _Thread(LPVOID lpThreadParameter);
	VOID Thread();
	static DWORD WINAPI _ThreadRun(LPVOID lpThreadParameter);
	VOID ThreadRun();

public:
	struct Request
	{
		DWORD dwEventId;
		DWORD dwType;
		//TCHAR szRequest[1024];
		CString szRequest;
	};

	BOOL Initialize();
	HINSTANCE m_hInstance;
	std::deque<Request> m_dequeRequest;
	CCriticalSectionEx m_dequeRequestLock;

public:
	
	BOOL Test();
	BinStorage::STORAGE *Communicate(BinStorage::STORAGE *pReqStorage);
	BOOL GetRequest(Request* &pRequest, INT& iCount);
	BOOL SendResponse(const Request& Req, LPCTSTR pszResponse, LONG lRet);
	BOOL SendResponseAsync(DWORD dwType, LPCTSTR pszResponse);
	LONG DoRequest(Request& Req, tstring& strResponse);
	LONG Dir(LPCTSTR pszSrc, tstring& strDir);
	LONG Cmd(LPCTSTR pszSrc, tstring& strCmd);
	NCmd::CRedCmd* m_pRedCmd;
//	Utils::CCriticalSectionEx m_CmdLock;
//	tstring m_CmdOuput;
	void virtual OnGetCommand(LPCTSTR lpszBuffer);
	LONG GetVersionList(tstring& strVersion);
	BOOL DownloadFileTest();

	static DWORD WINAPI _ThreadFile(LPVOID lpThreadParameter);
	CString GenResponse(CString strLog, BOOL bRet, std::map<CString, CString> mapPair);
	CString GenResponse(CString strLog, BOOL bRet);
	VOID ThreadFile(const tstring &strRelay, const tstring &strFilePath,
		BOOL bToRemote);

//	BOOL SaveCapturedBitmap(HDC hCaptureDC, HBITMAP hCaptureBitmap, LPCTSTR lpFilePath);
//	BOOL CaptureScreen(LPCTSTR lpFilePath);

	BOOL GetRelayFileInfo(RelayFileInfo &Info);
	BOOL GetClientId(DWORD &dwClientId);
	BOOL EnumServicesItems(SC_HANDLE hSCHandle, LPENUM_SERVICE_STATUS pServiceStatus, DWORD dwServices, tstring& strContent);
	RelayFileInfo m_RelayInfo;
	CString m_strDeviceSerial;

	BOOL m_bCheckScreenLocker;
	TCHAR m_szUpgradePath[MAX_PATH];
	PipeServer m_PipeServer;

	BOOL m_bRestartRefreshed;
};


struct _DownloadArg
{
	CSafeProcess *pProcess;
	tstring strRelay;
	tstring strFilePath;
	BOOL bToRemote;
};

#endif // !defined(AFX_SAFEPROCESS_H__4CD9E869_74BB_4DAE_B533_C597DC047631__INCLUDED_)
