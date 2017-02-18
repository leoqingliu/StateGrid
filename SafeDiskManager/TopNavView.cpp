// TopNavView.cpp : implementation file
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "TopNavView.h"
#include "MainFrm.h"

// CTopNavView

IMPLEMENT_DYNAMIC(CTopNavView, CDockablePane)

CTopNavView::CTopNavView()
:m_wndTabs(30)
{
}

CTopNavView::~CTopNavView()
{
}


BEGIN_MESSAGE_MAP(CTopNavView, CDockablePane)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_REGISTERED_MESSAGE(AFX_WM_CHANGE_ACTIVE_TAB, OnChangeActiveTab)
END_MESSAGE_MAP()



// CTopNavView message handlers



void CTopNavView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDockablePane::OnPaint() for painting messages

	CRect rect;
	GetClientRect(rect);
	dc.FillSolidRect(rect, RGB(1, 71, 75));
	dc.SetBkMode(TRANSPARENT);
}

void CTopNavView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// Tab control should cover the whole client area:
	m_wndTabs.SetWindowPos(this, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CTopNavView::CanBeClosed() const
{
	return FALSE;
}

int CTopNavView::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
	
	// Create output panes:
	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_DISABLED | WS_HSCROLL | WS_VSCROLL;
	if (!m_wndEmpty.	Create(dwStyle, rectDummy, &m_wndTabs, 100)
		)
	{
		TRACE0("Failed to create output windows\n");
		return -1;      // fail to create
	}
	m_wndTabs.SetActiveTabBoldFont(TRUE);
	m_wndTabs.EnableTabSwap(FALSE);
	
	// Attach list windows to tab:
	struct TabItems_t
	{
		CWnd *pWnd;
		UINT uId;
	};
	TabItems_t Items[] = 
	{
		{&m_wndEmpty,		IDS_TAB_CONTROL_PANEL}
		,{&m_wndEmpty,		IDS_TAB_HOSTINFO}
		,{&m_wndEmpty,		IDS_TAB_LOCK}
		,{&m_wndEmpty,		IDS_TAB_SCREEN}
		,{&m_wndEmpty,		IDS_TAB_BLACKWHITE_LIST}
		,{&m_wndEmpty,		IDS_TAB_CONTROL}
		,{&m_wndEmpty,		IDS_TAB_PROCESS}
		,{&m_wndEmpty,		IDS_TAB_CONTROL_DEV}
		,{&m_wndEmpty,		IDS_TAB_CONTROL_STORAGE}
		,{&m_wndEmpty,		IDS_TAB_CONTROL_SYSTEM}
		,{&m_wndEmpty,		IDS_TAB_VERSION}
		,{&m_wndEmpty,		IDS_TAB_SERVICE}
		,{&m_wndEmpty,		IDS_TAB_NETWORK}
		,{&m_wndEmpty,		IDS_TAB_CMD}
		,{&m_wndEmpty,		IDS_TAB_FILE}
//		,{&m_wndEmpty,		IDS_TAB_NETWORK_RATE}
//		,{&m_wndEmpty,		IDS_TAB_PROGRAM}
	};
	for (int i = 0; i < _countof(Items); i++)
	{
		TabItems_t *pItem = Items + i;
		pItem->pWnd->SetFont(&theApp.m_Font);

		CString strTabName;
		BOOL bNameValid;
		bNameValid = strTabName.LoadString(pItem->uId);
		ASSERT(bNameValid);
		m_wndTabs.AddTab(pItem->pWnd, strTabName, (UINT)0);
	}

	return 0;
}

LRESULT CTopNavView::OnChangeActiveTab(WPARAM wParam, LPARAM lParam)
{
	CFrameWnd *pFrame = GetParentFrame();
	CMainFrame *pMainFrame = (CMainFrame *)pFrame;
	INT iIndex = (INT)wParam;
	pMainFrame->OnChangeActiveNavTab(iIndex);
	return TRUE;
}
