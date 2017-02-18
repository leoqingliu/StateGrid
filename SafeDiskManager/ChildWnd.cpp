#include "StdAfx.h"
#include "SafeDiskManager.h"
#include "ChildWnd.h"
#include "resource.h"


CChildWnd::CChildWnd(void)
:m_wndTabs(0)
{
}

CChildWnd::~CChildWnd(void)
{
}

BEGIN_MESSAGE_MAP(CChildWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

int CChildWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	//
	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create tabs window:
	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_3D_ROUNDED, rectDummy, this, 1, CMFCTabCtrl::LOCATION_TOP, FALSE))
	{
		TRACE0("Failed to create output tab window\n");
		return -1;      // fail to create
	}
	m_wndTabs.HideNoTabs(TRUE);
	m_wndTabs.EnableTabSwap(FALSE);

	// Create output panes:
	CCreateContext Context;
	Context.m_pCurrentFrame = (CFrameWnd*)&m_wndTabs;
	Context.m_pCurrentDoc = NULL;
	Context.m_pNewViewClass = RUNTIME_CLASS(CHostInfoView);
	CView* pView = (CView*)(Context.m_pNewViewClass->CreateObject());
	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;
	if (!m_wndControlPanel.		Create(dwStyle, rectDummy, &m_wndTabs, ID_ITEM_CONTROL_PANEL)
		|| !pView->				Create(NULL, NULL, dwStyle, rectDummy, &m_wndTabs, ID_ITEM_HOSTINFO, &Context)
		|| !m_wndVersion.		Create(dwStyle, rectDummy, &m_wndTabs, ID_ITEM_VERSION)
//		|| !m_wndPassword.		Create(dwStyle, rectDummy, &m_wndTabs, ID_ITEM_PASSWORD)
		|| !m_wndScreen.		Create(IDD_DIALOG_SCREEN, &m_wndTabs)
		|| !m_wndBlackWhiteList.Create(IDD_DIALOG_BLACKWHITE_LIST, &m_wndTabs)
		|| !m_wndProcess.		Create(dwStyle, rectDummy, &m_wndTabs, ID_ITEM_PROCESS)
		|| !m_wndService.		Create(dwStyle, rectDummy, &m_wndTabs, ID_ITEM_SERVICE)
//		|| !m_wndKeyboard.		Create(dwStyle, rectDummy, &m_wndTabs, ID_ITEM_KEYBOARD)
		|| !m_wndNetwork.		Create(dwStyle, rectDummy, &m_wndTabs, ID_ITEM_NETWORK)
		|| !m_wndControl.		Create(IDD_DIALOG_CONTROL, &m_wndTabs)
		|| !m_wndLock.			Create(IDD_DIALOG_LOCK, &m_wndTabs)
		|| !m_wndControlDev.	Create(IDD_CONTROL_DEV_DIALOG, &m_wndTabs)
		|| !m_wndControlStorage.Create(IDD_CONTROL_STORAGE_DIALOG, &m_wndTabs)
		|| !m_wndControlSystem.	Create(IDD_CONTROL_SYSTEM_DIALOG, &m_wndTabs)
		|| !m_wndCmd.			Create(dwStyle, rectDummy, &m_wndTabs, ID_ITEM_CMD)
		|| !m_wndFile.			Create(dwStyle, rectDummy, &m_wndTabs, ID_ITEM_FILE)
//		|| !m_wndNetworkRate.	Create(IDD_DIALOG_NETWORK, &m_wndTabs)
//		|| !m_wndProgram.		Create(IDD_DIALOG_PROGRAM, &m_wndTabs)
		)
	{
		TRACE0("Failed to create output windows\n");
		return -1;      // fail to create
	}
	m_wndHostInfo = (CHostInfoView *)pView;

	m_wndProcess.SetContextMenuType(CViewListCtrl::CONTEXT_MENU_PROCESS);
	
	//CSize newSize(400,48);  //change 400 to bigger number does do any difference
	//m_wndTabs.SetItemSize(newSize);

	//CFont font;
	//font.CreateFont(-8, 0, 0, 0, 700,0,0,0,1,0,0,0,0,_T("Arial"));
	//m_wndTabs.SetFont(&font);
	m_wndTabs.SetActiveTabBoldFont(TRUE);
	//CMFCBaseTabCtrl::AFX_TAB_TEXT_MARGIN = 12;
	//m_wndTabs.SetActiveTabColor(RGB(255, 0, 0));
	//m_wndTabs.SetActiveTabTextColor(RGB(0, 255, 0));

	// Attach list windows to tab:
	struct TabItems_t
	{
		CWnd *pWnd;
		UINT uId;
	};
	TabItems_t Items[] = 
	{
		{&m_wndControlPanel,	IDS_TAB_CONTROL_PANEL}
		,{m_wndHostInfo,		IDS_TAB_HOSTINFO}
		,{&m_wndLock,			IDS_TAB_LOCK}
		,{&m_wndScreen,			IDS_TAB_SCREEN}
		,{&m_wndBlackWhiteList,	IDS_TAB_BLACKWHITE_LIST}
		,{&m_wndControl,		IDS_TAB_CONTROL}
		,{&m_wndProcess,		IDS_TAB_PROCESS}
		,{&m_wndControlDev,		IDS_TAB_CONTROL_DEV}
		,{&m_wndControlStorage,	IDS_TAB_CONTROL_STORAGE}
		,{&m_wndControlSystem,	IDS_TAB_CONTROL_SYSTEM}
		,{&m_wndVersion,		IDS_TAB_VERSION}
//		,{&m_wndPassword,		IDS_TAB_PASSWORD}
		,{&m_wndService,		IDS_TAB_SERVICE}
//		,{&m_wndKeyboard,		IDS_TAB_KEYBOARD}
		,{&m_wndNetwork,		IDS_TAB_NETWORK}
		,{&m_wndCmd,			IDS_TAB_CMD}
		,{&m_wndFile,			IDS_TAB_FILE}
//		,{&m_wndNetworkRate,	IDS_TAB_NETWORK_RATE}
//		,{&m_wndProgram,		IDS_TAB_PROGRAM}
	};
	for (int i = 0; i < _countof(Items); i++)
	{
		TabItems_t *pItem = Items + i;
		//Items[i].pWnd->SetFont(&afxGlobalData.fontRegular);
		pItem->pWnd->SetFont(&theApp.m_Font);

		CString strTabName;
		BOOL bNameValid;
		bNameValid = strTabName.LoadString(pItem->uId);
		ASSERT(bNameValid);
		m_wndTabs.AddTab(pItem->pWnd, strTabName, (UINT)0);
	}

	return 0;
}

void CChildWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// Tab control should cover the whole client area:
	m_wndTabs.SetWindowPos(this, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}


BOOL CChildWnd::CanBeClosed() const
{
	return FALSE;
}

void CChildWnd::OnContextMenu(CWnd* pWnd, CPoint point)
{
//	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_MENU_CHILD_WND, point.x, point.y, this, TRUE);
}

CTabbedPane* CChildWnd::CreateTabbedPane()
{
	CTabbedPane* pTabbedBar = CDockablePane::CreateTabbedPane();
//	pTabbedBar->SetMinSize(CSize(500, 500));

	return pTabbedBar;
}

void CChildWnd::ActiveTab(int iIndex)
{
	m_wndTabs.SetActiveTab(iIndex);
}

void CChildWnd::Close()
{
//	m_wndHostInfo.Close();
}

void CChildWnd::OnClose()
{
	if (NULL != m_wndHostInfo)
	{
		delete m_wndHostInfo;
		m_wndHostInfo = NULL;
	}

	CDockablePane::OnClose();
}
