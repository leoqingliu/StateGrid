
// ScreenLocker.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "../Public/PipeServer.h"
#include "../MacConfig.h"
#include "../ConnConf.h"
#include "UDiskUtils.h"

// CScreenLockerApp:
// See ScreenLocker.cpp for the implementation of this class
//

class CScreenLockerApp : public CWinApp
{
public:
	CScreenLockerApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();

public:
	VOID ControlKey(BOOL bEnable);

public:
	PipeServer m_PipeServer;

	CString m_strTmpPath;
	CString m_strDriverUDiskDir;

	BOOL ControlUDisk();
	BOOL ControlDevice(BOOL bForceEnable = FALSE);
	BOOL ControlNetwork(BOOL bForceEnable = FALSE);

	CCriticalSection m_ConfigLock;
	CUDiskUtils m_uDiskUtils;
	std::deque<DWORD> m_dequeLockInfo;
	CCriticalSection m_dequeLockInfoLock;
	BOOL m_bLocked;
	_CLIENT_INFO m_Info;
	CCriticalSection m_InfoCS;
	BOOL m_bDiskWaitForConfirm;
	BOOL m_bDiskAllow;
};

extern CScreenLockerApp theApp;