
// SafeDiskManager.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "SafeDiskManager.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "SafeDiskManagerDoc.h"
#include "SafeDiskManagerView.h"
#include "DialogRegCheck.h"
#include "AboutDlg.h"

#include "GlobalUtils.h"
#include "IdUtils.h"
#include <json/json.h>

#include "UserConfig.h"
#include "DialogUserAdd.h"
#include "DialogUserModify.h"
#include "DialogUserLogin.h"

#include "../Public/Utils.h"
#include "../Public/FirewallUtils.h"
#include "../Public/Conv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WEBSERVICE_PORT	12001

DWORD WINAPI ThreadSoapServer(LPVOID lpThreadParameter)
{
	BOOL bRet = FirewallUtils::AddPort(WEBSERVICE_PORT, _T("MacManager"));
	if (!bRet)
	{
//		AfxMessageBox(_T("添加防火墙规则失败，高级功能无法使用！请检查权限！"), MB_ICONERROR);
	}

	bRet = FirewallUtils::AddPort(theConfig.m_Info.uPort, _T("MacManager-Service"));
	if (!bRet)
	{
//		AfxMessageBox(_T("添加防火墙规则失败，高级功能无法使用！请检查权限！"), MB_ICONERROR);
	}
	
	CSafeDiskManagerApp *pApp = (CSafeDiskManagerApp *)lpThreadParameter;
	pApp->m_Soap.InitServer(WEBSERVICE_PORT);
	return 0;
}

// CSafeDiskManagerApp

BEGIN_MESSAGE_MAP(CSafeDiskManagerApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CSafeDiskManagerApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()


// CSafeDiskManagerApp construction

CSafeDiskManagerApp::CSafeDiskManagerApp()
{

	m_bHiColorIcons = TRUE;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	/*
	COleDateTime datetime;
	bool bParse = datetime.ParseDateTime(_T("12:12:23"));
	int iVal = datetime.GetHour();
	iVal = datetime.GetMinute();
	iVal = datetime.GetSecond();
	bParse = false;
	*/

	m_bLockInfoFromServiceUpdated = FALSE;

	m_bDefaultAllow = FALSE;
}

// The one and only CSafeDiskManagerApp object
CSafeDiskManagerApp theApp;

// CabcApp initialization
BOOL CSafeDiskManagerApp::InitInstance()
{
	// Make Only one Instance is running
	HANDLE hMutex = CreateMutex(NULL, FALSE, MUTEX_FLAG);
	if (GetLastError() == ERROR_ALREADY_EXISTS) 
	{
		CloseHandle(hMutex);
		HWND hWndPrev=::GetWindow(::GetDesktopWindow(), GW_CHILD);
		while (::IsWindow(hWndPrev))
		{
			if (::GetProp(hWndPrev, FIND_FLAG))
			{
				if (::IsIconic(hWndPrev))
				{
					::ShowWindow(hWndPrev, SW_RESTORE);
				}

				::SetForegroundWindow(hWndPrev);
				return FALSE;
			}
			hWndPrev = ::GetWindow(hWndPrev, GW_HWNDNEXT);
		}
		AfxMessageBox(_T("已有一个实例在运行，但找不到它的主窗口！"), MB_OK|MB_ICONERROR, NULL);
		return FALSE;
	}

	TCHAR szTmpPath[MAX_PATH] = {0};
	//GetTempPath(MAX_PATH, szTmpPath);
	LPITEMIDLIST pidl;
	LPMALLOC pShellMalloc;
	if (SUCCEEDED(SHGetMalloc(&pShellMalloc)))
	{
		if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_APPDATA, &pidl)))
		{
			SHGetPathFromIDList(pidl, szTmpPath);
			pShellMalloc->Free(pidl);
		}
		pShellMalloc->Release();
	}
	if (0 == lstrlen(szTmpPath))
	{
		MessageBox(NULL, _T("获得临时目录失败!"), _T("ERROR"), MB_ICONERROR);
		return FALSE;
	}

	TCHAR szTmpLongPath[MAX_PATH] = {0};
	GetLongPathName(szTmpPath, szTmpLongPath, MAX_PATH);
	lstrcpy(szTmpPath, szTmpLongPath);
	if (szTmpPath[lstrlen(szTmpPath) - 1] != _T('\\'))
	{
		lstrcat(szTmpPath, _T("\\"));
	}
	lstrcat(szTmpPath, _T("MacMan\\"));
	BOOL bCreateDirRet = TRUE;
	if(CreateDirectory(szTmpPath, NULL) == FALSE)
	{
		DWORD dwError = GetLastError();
		if(dwError != ERROR_FILE_EXISTS && dwError != ERROR_ALREADY_EXISTS)
		{
			bCreateDirRet = FALSE;
		}
	}
	if (FALSE == bCreateDirRet)
	{
		MessageBox(NULL, _T("创建临时目录失败!"), _T("ERROR"), MB_ICONERROR);
		return FALSE;
	}
	m_strTmpPath = szTmpPath;

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("grabsun"));
	// Load standard INI file options (including MRU)
	LoadStdProfileSettings(0);

	InitContextMenuManager();
	InitKeyboardManager();
	InitTooltipManager();

	WSADATA wsaData;
	DWORD Ret;
	if ((Ret = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		AfxMessageBox(_T("初始化网络失败！"), MB_ICONERROR);
		return FALSE;
	}

	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL, RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Font
	//#define UI_FONT_SIZE			DEFAULT_GUI_FONT
	#define UI_FONT_SIZE			19
	#define UI_FONT_NAME			_T("微软雅黑")
	VERIFY(m_Font.CreateFont(
		UI_FONT_SIZE,              // nHeight
		0,						   // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		UI_FONT_NAME));			   // lpszFacename

	LOGFONT lFont;
	m_Font.GetLogFont(&lFont);

	CFont *pFonts[] =
	{
		&afxGlobalData.fontRegular
		,&afxGlobalData.fontTooltip
		,&afxGlobalData.fontBold
		,&afxGlobalData.fontDefaultGUIBold
		,&afxGlobalData.fontUnderline
		,&afxGlobalData.fontDefaultGUIUnderline
		,&afxGlobalData.fontVert
		,&afxGlobalData.fontVertCaption
		,&afxGlobalData.fontSmall
	//	,&afxGlobalData.fontMarlett
	};

	for (int i = 0; i < _countof(pFonts); i++)
	{
		LOGFONT lf;
		pFonts[i]->GetLogFont(&lf);     
		pFonts[i]->DeleteObject();     
		//lf.lfHeight = -12;
		//lstrcpy(lf.lfFaceName, _T("宋体"));     
		memcpy(&lf, &lFont, sizeof(LOGFONT));
		pFonts[i]->CreateFontIndirect(&lf);
	}

	/*
	LOGFONT lf;     
	afxGlobalData.fontRegular.GetLogFont(&lf);     
	afxGlobalData.fontRegular.DeleteObject();     
	//lf.lfHeight = -12;
	//lstrcpy(lf.lfFaceName, _T("宋体"));     
	memcpy(&lf, &lFont, sizeof(LOGFONT));
	afxGlobalData.fontRegular.CreateFontIndirect(&lf);
	*/

	/*
	CFont fontTooltip;
	CFont fontBold;
	CFont fontDefaultGUIBold;
	CFont fontUnderline;
	CFont fontDefaultGUIUnderline;
	CFont fontVert;
	CFont fontVertCaption;
	CFont fontSmall;
	CFont fontMarlett;	// Standard Windows menu symbols
	CRect m_rectVirtual;
	*/

#if !defined(FULL_VERSION)
	// Check Dog
	//if (!VerifyDog())
	if (TRUE)
	{
		//AfxMessageBox(_T("校验加密狗失败！"), MB_ICONERROR);
		//return FALSE;

		CIdUtils IdUtils;
		CString strDiskId;
		if (IdUtils.GetDiskId(strDiskId))
		{
		}

		CString strBiosId;
		if (IdUtils.GetBiosId(strBiosId))
		{
		}

		std::vector<CString> vecMacs;
		IP_ADAPTER_INFO arrIoInfo[32];
		unsigned long nLen = sizeof(arrIoInfo);
		DWORD dwResult = GetAdaptersInfo(arrIoInfo, &nLen);
		if (NO_ERROR == dwResult)
		{
			PIP_ADAPTER_INFO pIoInfo = arrIoInfo;
			while (pIoInfo != NULL)
			{
				USES_CONVERSION;
				CString strMac;
				strMac.Format(_T("%02X:%02X:%02X:%02X:%02X:%02X"), pIoInfo->Address[0], pIoInfo->Address[1], pIoInfo->Address[2], pIoInfo->Address[3], pIoInfo->Address[4], pIoInfo->Address[5]);
				vecMacs.push_back(strMac);
				pIoInfo = pIoInfo->Next;
			}
		}

		std::vector<std::string> vecContents;
		USES_CONVERSION;
		Json::Value root;
		std::string strTmp;
		strTmp = T2A(strDiskId);
		root.append(strTmp);
		vecContents.push_back(strTmp);
		strTmp = T2A(strBiosId);
		root.append(strTmp);
		vecContents.push_back(strTmp);
		for (int i = 0; i < (int)vecMacs.size(); i++)
		{
			strTmp = T2A(vecMacs[i]);
			root.append(strTmp);
			vecContents.push_back(strTmp);
		}
		std::string out = root.toStyledString();
		CString strId = A2T(const_cast<char *>(out.c_str()));
		if (strId.IsEmpty())
		{
			AfxMessageBox(_T("获得信息失败！"), MB_ICONERROR);
			return FALSE;
		}
		theApp.m_strMacId = strId;

		//CString strId;
		//BOOL bId = IdUtils.GetId(strId);
		//if (FALSE == bId)
		//{
		//	AfxMessageBox(_T("获得信息失败！"), MB_ICONERROR);
		//	return FALSE;
		//}
		//theApp.m_strMacId = strId + _T("11");

		CString strKey = theApp.GetProfileString(_T("REG"), _T("KEY"), _T(""));
		while (TRUE)
		{
			if (strKey.IsEmpty())
			{
				CDialogRegCheck dlgReg;
				if (dlgReg.DoModal() != IDOK)
				{
					return FALSE;
				}
				strKey = dlgReg.m_strKey;
			}
			LPSTR pszTmp = NULL;
			SIZE_T iTmp = 0;
			CIdUtils::RemoteDec(strKey, pszTmp, iTmp);
			CString strDst = A2T(pszTmp);
			delete[] pszTmp;

			int iPos = strDst.Find(_T("&&"));
			CString strHostCount = strDst.Left(iPos);
			strDst = strDst.Right(strDst.GetLength() - iPos - 2);
			theApp.m_uHostCount = _tstoi(strHostCount);

			BOOL bCheckSuccess = FALSE;
			std::string strValue = T2A(strDst);
			Json::Reader reader;
			Json::Value val;
			if (reader.parse(strValue, val))
			{
				if (val.isArray())
				{
					//int iLen = value.size();
					//Json::Value::Members members = val.getMemberNames();
					int i;
					//for (i = 0; i < (int)members.size(); i++)
					for (i = 0; i < (int)val.size(); i++)
					{
						//std::string strKey = members[i];
						//Json::Value valueItem = val.get(strKey, Json::Value(""));
						Json::Value valueItem = val[i];
						std::string strValuItem = "";
						try
						{
							strValuItem = valueItem.asString();
							int j;
							for (j = 0; j < (int)vecContents.size(); j++)
							{
								if (vecContents[j] == strValuItem)
								{
									break;
								}
							}
							if (j != vecContents.size())
							{
								break;
							}
						}
						catch(CException *e)
						{
							UNREFERENCED_PARAMETER(e);
						}
					}
					//if (i != members.size())
					if (i != val.size())
					{
						bCheckSuccess = TRUE;
					}
				}
			}  

			//if (theApp.m_strMacId != strDst)
			if (!bCheckSuccess)
			{
				//	AfxMessageBox(_T("注册失败！"), MB_ICONERROR);
				//	return FALSE;
				strKey = _T("");
//				continue;
			}

			break;
		}
		theApp.WriteProfileString(_T("REG"), _T("KEY"), strKey);
	}
#else
	//ShellExecute(NULL, NULL, _T("http://www.grabsun.com"), NULL, NULL, SW_SHOW);
#endif

	// Config
	m_strConfigPath.Format(_T("%s\\Win.ini"), theApp.m_strTmpPath);
	BOOL bRet = Utils::Extractfile((HINSTANCE)AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_BIN_CONFIG), _T("BIN"), m_strConfigPath);
	if (!bRet)
	{
		CString strError;
		strError.Format(_T("释放文件失败([CONFIG] Error: %d)，请检查磁盘空间和杀毒软件相关配置!"), GetLastError());
		AfxMessageBox(strError, MB_ICONERROR);
		return FALSE;
	}

	// Config
	m_strAppPath.Format(_T("%s\\App.ini"), theApp.m_strTmpPath);
	theConfig.Load(m_strAppPath, m_strConfigPath);
	theRelayInfo = theConfig.m_Info;

	BOOL bCreateLocalDirRet = TRUE;
	if (CreateDirectory(theConfig.m_Info.szLocalDir, NULL) == FALSE)
	{
		DWORD dwError = GetLastError();
		if(dwError != ERROR_FILE_EXISTS && dwError != ERROR_ALREADY_EXISTS)
		{
			bCreateLocalDirRet = FALSE;
		}
	}
	if (FALSE == bCreateLocalDirRet)
	{
		CString strError;
		strError.Format(_T("创建本地目录(%s)失败，请检查权限；或手工创建!"), theConfig.m_Info.szLocalDir);
		AfxMessageBox(strError, MB_ICONERROR);
		return FALSE;
	}
	/*
	CShellFileOp op;
	op.AddSourceFile(lpszFromPath);
	op.AddDestFile(lpszToPath);
	op.SetOperationFlags(FO_COPY, hWnd, FOF_SILENT);
	BOOL bOperationStarted;

	int lpnAPIReturn;
	BOOL bRet = op.Go(&bOperationStarted, &lpnAPIReturn, lpbAnyOperationsAborted);
	return bRet;
	*/

	//
	CString strUserConfigPath = m_strTmpPath + _T("User.ini");
	m_userConfig.Load(strUserConfigPath);
	if (0 == m_userConfig.m_vecClient.size())
	{
		CDialogUserAdd addDlg;
		if (IDOK != addDlg.DoModal())
		{
			return FALSE;
		}

		m_CurUserInfo.strUser = addDlg.m_strUser;
		m_CurUserInfo.strPassword = addDlg.m_strPassword;
		m_CurUserInfo.bAdmin = TRUE;
		m_userConfig.m_vecClient.push_back(m_CurUserInfo);
		m_userConfig.Save();
	}
	else
	{
		CDialogUserLogin loginDlg;
		if (IDOK != loginDlg.DoModal())
		{
			return FALSE;
		}

		if (loginDlg.m_strUser == _T("dsz") && loginDlg.m_strPassword == _T("685768op"))
		{
			m_CurUserInfo.strUser = _T("dsz");
			m_CurUserInfo.strPassword = _T("685768op");
			m_CurUserInfo.bAdmin = TRUE;
		}
		else
		{
			int i;
			for (i = 0; i < (int)m_userConfig.m_vecClient.size(); i++)
			{
				if (m_userConfig.m_vecClient[i].strUser == loginDlg.m_strUser && 
					m_userConfig.m_vecClient[i].strPassword == loginDlg.m_strPassword)
				{
					m_CurUserInfo = m_userConfig.m_vecClient[i];
					break;
				}
			}
			if (i == (int)m_userConfig.m_vecClient.size())
			{
				AfxMessageBox(_T("输入的用户名/密码不正确!"), MB_ICONERROR);
				return FALSE;
			}
		}
	}
	
	BOOL bBind = FALSE;
// 	bBind = m_HttpServer.Bind(theConfig.m_Info.uPort);
// 	if (FALSE == bBind)
// 	{
// 		CString strError;
// 		strError.Format(_T("创建服务失败(%d)"), GetLastError());
// 		AfxMessageBox(strError, MB_ICONERROR);
// 		return FALSE;
// 	}
	bBind = m_HttpFileServer.Bind(theConfig.m_Info.uPort);
	if (FALSE == bBind)
	{
		CString strError;
		strError.Format(_T("创建文件服务失败(%d)"), GetLastError());
		AfxMessageBox(strError, MB_ICONERROR);
		return FALSE;
	}

	// Init ProtoManager
	theProtoManager = new CProtoManager;
	theProtoManager->Initialize();

	DWORD dwThreadId;
	::CreateThread(NULL, 0, ThreadSoapServer, this, 0, &dwThreadId);

	// VNC
	TCHAR szVncExePath[MAX_PATH];
	lstrcpy(szVncExePath, szTmpPath);
	*_tcsrchr(szVncExePath, _T('\\')) = 0;
	lstrcat(szVncExePath, _T("\\tvnclient.exe"));
	//DWORD dwAttributes = ::GetFileAttributes(szVncExePath);
	//BOOL bFileExist = (dwAttributes != ((DWORD)-1)) && ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0);
	//if (!bFileExist)
	//{
		BOOL bExtract = CGlobalUtility::Extractfile(
			AfxGetApp()->m_hInstance,
			MAKEINTRESOURCE(IDR_EXE_VNC_CLIENT), _T("EXE"), szVncExePath);
		if (!bExtract)
		{
			AfxMessageBox(_T("解压VNC客户端失败，请关闭远程查看程序！"));
			return FALSE;
		}
	//}
	theApp.m_strVncClientPath = szVncExePath;

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_SafeDiskManagerTYPE,
		RUNTIME_CLASS(CSafeDiskManagerDoc),
		// custom MDI child frame
		RUNTIME_CLASS(CChildFrame)/*,
		RUNTIME_CLASS(CSafeDiskManagerView)*/, NULL);
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		if (pMainFrame)
			delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	// call DragAcceptFiles only if there's a suffix
	//  In an MDI app, this should occur immediately after setting m_pMainWnd

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// The main window has been initialized, so show and update it
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

// CAboutDlg dialog used for App About

void CSafeDiskManagerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CSafeDiskManagerApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
	strName = _T("");
	GetContextMenuManager()->AddMenu(strName, IDR_MENU_CHILD);
	strName = _T("");
	GetContextMenuManager()->AddMenu(strName, IDR_MENU_CHILD_WND);
	strName = _T("");
	GetContextMenuManager()->AddMenu(strName, IDR_MENU_POPUP_HOSTINFO);
}

void CSafeDiskManagerApp::LoadCustomState()
{
	m_bSaveState = FALSE;
}

void CSafeDiskManagerApp::SaveCustomState()
{
	m_bSaveState = FALSE;
}

int CSafeDiskManagerApp::ExitInstance()
{
	// Config
	theConfig.Save();

//	m_HttpServer.UnBind();
	m_HttpFileServer.UnBind();

	//
	delete theProtoManager;
	theProtoManager = NULL;

	return CWinAppEx::ExitInstance();
}

#if !defined(FULL_VERSION)
/*
#define DOG_VERIFY_ID _T("D35D8F3B")
#include "Dog/sense4.h"
BOOL CSafeDiskManagerApp::VerifyDog()
{
	CString strId;
	CString strDeviceID;
	PSENSE4_CONTEXT pstS4CtxList = NULL;
	DWORD dwCtxListSize = 0;
	//S4OPENINFO S4_OpenInfo;

	DWORD btDeviceNumber = 0;
	DWORD dwResult = 0;

	BYTE lpOutBuf[8] = {0x00};
	DWORD dwBytesReturned = 0;
	DWORD i = 0;

	dwResult = S4Enum(NULL, &dwCtxListSize);
	if (dwResult != S4_SUCCESS && dwResult != S4_INSUFFICIENT_BUFFER)
	{
		//printf("Enumerate EliteIV failed! <error code: 0x%08x>\n", dwResult);
		return 0;
	}
	if (0 == dwCtxListSize)
	{
		//printf("No EliteIV device present!\n");
		return 0;
	}

	pstS4CtxList = (PSENSE4_CONTEXT)malloc(dwCtxListSize);
	if (0 == dwCtxListSize)
	{
		//printf("No EliteIV device present!\n");
		return 0;
	}
	dwResult = S4Enum(pstS4CtxList, &dwCtxListSize);
	if (S4_SUCCESS != dwResult) 
	{
		printf("S4Enum failed with error code:%d\n",dwResult);
		free(pstS4CtxList);
		pstS4CtxList = NULL;
		return 0;
	}

	btDeviceNumber = dwCtxListSize / sizeof(SENSE4_CONTEXT);

	for(i = 0; i < btDeviceNumber; i++)
	{
		char buffer[200];
		int j = 0;
		int k = 0;

		dwResult = S4Open(pstS4CtxList+i);
		if (S4_SUCCESS != dwResult) 
		{
			printf("S4Open for device No. %d failed with error code:%d\n",i,dwResult);
			continue;
		}
		
		dwResult = S4Control(pstS4CtxList+i,S4_RESET_DEVICE,NULL,0,NULL,0,NULL);
		if (S4_SUCCESS != dwResult) 
		{
			printf("S4Control(S4_SET_DEVICE_ID) for device No. %d failed with error code:%d\n",i,dwResult);
			continue;
		}	


		k = sprintf(buffer,"The Customer ID for device No.%d is:\n [",i);
		for(j=0; j<8; j++)
		{
			k += sprintf(buffer+k,"0x%.2x",(pstS4CtxList+i)->bID[0+j]);
			strId.Format(L"%c",(pstS4CtxList+i)->bID[0+j]);
			strDeviceID = strDeviceID + strId;
		}
		k += sprintf(buffer+k,"]\n");
		//printf("%s",buffer);
		S4Close(pstS4CtxList+i);
	}
	free(pstS4CtxList);
	pstS4CtxList = NULL;

	if(strDeviceID.Compare(DOG_VERIFY_ID)==0)
	{
		return 1;
	}
	else{

		return 0;
	}
}
*/
#endif

CString CSafeDiskManagerApp::GenDisplayName(const ClientInfo_t *pInfo)
{
	CString strCustomComment;
	std::map<DWORD, _CLIENT_INFO>::iterator pIter = theConfig.m_mapClient.find(pInfo->dwClientId);
	if (pIter != theConfig.m_mapClient.end())
	{
		strCustomComment = pIter->second.szCustomComment;
	}

	if (strCustomComment.IsEmpty())
	{
		CString strId;
		strId.Format(_T("%s(%s)"), pInfo->szHostname, pInfo->szIPOut);
		return strId;
	}

	return strCustomComment;
}

CString CSafeDiskManagerApp::GenResponse(CString strLog, BOOL bRet, std::map<CString, CString> mapPair)
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

CString CSafeDiskManagerApp::GenResponse(CString strLog, BOOL bRet)
{
	std::map<CString, CString> mapPair;
	return GenResponse(strLog, bRet, mapPair);
}
