
#include "stdafx.h"
#include "mainfrm.h"
#include "ConnectView.h"
#include "Resource.h"
#include "SafeDiskManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileView

CConnectView::CConnectView()
{
	m_dwLastClientId = 0;
}

CConnectView::~CConnectView()
{
}

BEGIN_MESSAGE_MAP(CConnectView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PROPERTIES, OnProperties)
	ON_COMMAND(ID_POLICY, OnPolicy)
	ON_COMMAND(ID_OPEN, OnFileOpen)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_UPDATE_COMMAND_UI(ID_OPEN, &CConnectView::OnUpdateOpen)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES, &CConnectView::OnUpdateProperties)
	ON_UPDATE_COMMAND_UI(ID_POLICY, &CConnectView::OnUpdatePolicy)
	ON_COMMAND(ID_REMOTE_OPEN, &CConnectView::OnRemoteOpen)
	ON_UPDATE_COMMAND_UI(ID_REMOTE_OPEN, &CConnectView::OnUpdateRemoteOpen)
	ON_COMMAND(ID_BATCH_LOCK, &CConnectView::OnBatchLock)
	ON_UPDATE_COMMAND_UI(ID_BATCH_LOCK, &CConnectView::OnUpdateBatchLock)
	ON_COMMAND(ID_COMMENT, &CConnectView::OnComment)
	ON_UPDATE_COMMAND_UI(ID_COMMENT, &CConnectView::OnUpdateComment)
	ON_COMMAND(ID_BATCH_UNLOCK, &CConnectView::OnBatchUnlock)
	ON_UPDATE_COMMAND_UI(ID_BATCH_UNLOCK, &CConnectView::OnUpdateBatchUnlock)
	ON_COMMAND(ID_BATCH_UPGRADE, &CConnectView::OnBatchUpgrade)
	ON_UPDATE_COMMAND_UI(ID_BATCH_UPGRADE, &CConnectView::OnUpdateBatchUpgrade)
	ON_COMMAND(ID_POLICY_EDIT, &CConnectView::OnPolicyEdit)
	ON_UPDATE_COMMAND_UI(ID_POLICY_EDIT, &CConnectView::OnUpdatePolicyEdit)
	ON_COMMAND(ID_GROUP_CREATE, &CConnectView::OnGroupCreate)
	ON_UPDATE_COMMAND_UI(ID_GROUP_CREATE, &CConnectView::OnUpdateGroupCreate)
	ON_COMMAND(ID_GROUP_EDIT, &CConnectView::OnGroupEdit)
	ON_UPDATE_COMMAND_UI(ID_GROUP_EDIT, &CConnectView::OnUpdateGroupEdit)
	ON_COMMAND(ID_GROUP_DEL, &CConnectView::OnGroupDel)
	ON_UPDATE_COMMAND_UI(ID_GROUP_DEL, &CConnectView::OnUpdateGroupDel)
	ON_COMMAND(ID_GROUP_ADDTO, &CConnectView::OnGroupAddto)
	ON_UPDATE_COMMAND_UI(ID_GROUP_ADDTO, &CConnectView::OnUpdateGroupAddto)
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_BATCH_AUTO, &CConnectView::OnBatchAuto)
	ON_UPDATE_COMMAND_UI(ID_BATCH_AUTO, &CConnectView::OnUpdateBatchAuto)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar 消息处理程序

int CConnectView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建视图:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS;

	if (!m_wndConnectView.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("未能创建文件视图\n");
		return -1;      // 未能创建
	}

	// 加载视图图像:
	m_FileConnectImages.Create(IDB_FILE_VIEW, 16, 0, RGB(255, 0, 255));
	m_wndConnectView.SetImageList(&m_FileConnectImages, TVSIL_NORMAL);

	m_wndConnectView.SetFont(&theApp.m_Font);
	m_wndConnectView.SetToolTips(NULL);

//	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER);
//	m_wndToolBar.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE /* 已锁定*/);

	OnChangeVisualStyle();

//	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
//	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
//	m_wndToolBar.SetOwner(this);
	// 所有命令将通过此控件路由，而不是通过主框架路由:
//	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	// 填入一些静态树视图数据(此处只需填入虚拟代码，而不是复杂的数据)
	//FillFileView();
	AdjustLayout();

	m_ToolTip.CreateEx(this, TTS_ALWAYSTIP);
	CRect rect;
	m_wndConnectView.GetClientRect(&rect);
	m_ToolTip.AddTool(&m_wndConnectView, _T(""), &rect, m_wndConnectView.GetDlgCtrlID());

	return 0;
}

void CConnectView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

/*
void CConnectView::FillFileView()
{
	HTREEITEM hRoot = m_wndConnectView.InsertItem(_T("FakeApp 文件"), 0, 0);
	m_wndConnectView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	HTREEITEM hSrc = m_wndConnectView.InsertItem(_T("FakeApp 源文件"), 0, 0, hRoot);

	m_wndConnectView.InsertItem(_T("FakeApp.cpp"), 1, 1, hSrc);
	m_wndConnectView.InsertItem(_T("FakeApp.rc"), 1, 1, hSrc);
	m_wndConnectView.InsertItem(_T("FakeAppDoc.cpp"), 1, 1, hSrc);
	m_wndConnectView.InsertItem(_T("FakeAppView.cpp"), 1, 1, hSrc);
	m_wndConnectView.InsertItem(_T("MainFrm.cpp"), 1, 1, hSrc);
	m_wndConnectView.InsertItem(_T("StdAfx.cpp"), 1, 1, hSrc);

	HTREEITEM hInc = m_wndConnectView.InsertItem(_T("FakeApp 头文件"), 0, 0, hRoot);

	m_wndConnectView.InsertItem(_T("FakeApp.h"), 2, 2, hInc);
	m_wndConnectView.InsertItem(_T("FakeAppDoc.h"), 2, 2, hInc);
	m_wndConnectView.InsertItem(_T("FakeAppView.h"), 2, 2, hInc);
	m_wndConnectView.InsertItem(_T("Resource.h"), 2, 2, hInc);
	m_wndConnectView.InsertItem(_T("MainFrm.h"), 2, 2, hInc);
	m_wndConnectView.InsertItem(_T("StdAfx.h"), 2, 2, hInc);

	HTREEITEM hRes = m_wndConnectView.InsertItem(_T("FakeApp 资源文件"), 0, 0, hRoot);

	m_wndConnectView.InsertItem(_T("FakeApp.ico"), 2, 2, hRes);
	m_wndConnectView.InsertItem(_T("FakeApp.rc2"), 2, 2, hRes);
	m_wndConnectView.InsertItem(_T("FakeAppDoc.ico"), 2, 2, hRes);
	m_wndConnectView.InsertItem(_T("FakeToolbar.bmp"), 2, 2, hRes);

	m_wndConnectView.Expand(hRoot, TVE_EXPAND);
	m_wndConnectView.Expand(hSrc, TVE_EXPAND);
	m_wndConnectView.Expand(hInc, TVE_EXPAND);
}
*/

void CConnectView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndConnectView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// 选择已单击的项:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}

		pWndTree->SetFocus();
#if 0
		CMenu menu;
		menu.LoadMenu(IDR_POPUP_EXPLORER);
		/*
		CMenu subMenu;
		subMenu.CreatePopupMenu();
		subMenu.AppendMenu(MF_BYPOSITION, 0, _T("123"));
		menu.InsertMenu();
		*/
		theApp.GetContextMenuManager()->ShowPopupMenu(menu.GetSafeHmenu(), point.x, point.y, this, TRUE);
#else
		theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);
#endif
	}
}

void CConnectView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

//	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;
//	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	int cyTlb = 0;
	m_wndConnectView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CConnectView::OnFileOpen()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnConnectCommand(ID_OPEN);
}

void CConnectView::OnProperties()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnConnectCommand(ID_PROPERTIES);
}

void CConnectView::OnPolicy()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnConnectCommand(ID_POLICY);
}

void CConnectView::OnRemoteOpen()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnConnectCommand(ID_REMOTE_OPEN);
}

void CConnectView::OnUpdateOpen(CCmdUI *pCmdUI)
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnConnectUpdate(ID_OPEN, pCmdUI);
}

void CConnectView::OnUpdateProperties(CCmdUI *pCmdUI)
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnConnectUpdate(ID_PROPERTIES, pCmdUI);
}

void CConnectView::OnUpdatePolicy(CCmdUI *pCmdUI)
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnConnectUpdate(ID_POLICY, pCmdUI);
}

void CConnectView::OnUpdateRemoteOpen(CCmdUI *pCmdUI)
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnConnectUpdate(ID_REMOTE_OPEN, pCmdUI);
}



void CConnectView::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	CRect rectTree;
	m_wndConnectView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CConnectView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndConnectView.SetFocus();
}

void CConnectView::OnChangeVisualStyle()
{
//	m_wndToolBar.CleanUpLockedImages();
//	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_EXPLORER_24 : IDR_EXPLORER, 0, 0, TRUE /* 锁定 */);

	m_FileConnectImages.DeleteImageList();

	//UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_FILE_VIEW_24 : IDB_FILE_VIEW;
	UINT uiBmpId = IDB_CONNECT_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("无法加载位图: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;
	m_FileConnectImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_FileConnectImages.Add(&bmp, RGB(255, 0, 0));

	m_wndConnectView.SetImageList(&m_FileConnectImages, TVSIL_NORMAL);
}



BOOL CConnectView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != NULL);
	switch (pNMHDR->code)
	{
	case NM_DBLCLK:
		{
			HTREEITEM hSel = m_wndConnectView.GetSelectedItem();
			HTREEITEM hRoot = m_wndConnectView.GetRootItem();
			HTREEITEM hParent = m_wndConnectView.GetParentItem(hSel);
			if (hSel == hRoot || hParent == hRoot)
			{
			}
			else
			{
				OnFileOpen();
			}
		}
		break;
	case NM_CLICK:
		{
		}
		break;
	case NM_TVSTATEIMAGECHANGING:
		{
		}
		break;
	case TVN_SELCHANGED:
		{
			LPNM_TREEVIEW pnmtv = (NM_TREEVIEW FAR *)lParam;
			CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
			pFrame->OnConnectSelChanged(pnmtv->itemNew.hItem);
		}
		break;
	default:
		break;
	}
	return CDockablePane::OnNotify(wParam, lParam, pResult);
}

void CConnectView::OnBatchLock()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnConnectCommand(ID_BATCH_LOCK);
}

void CConnectView::OnUpdateBatchLock(CCmdUI *pCmdUI)
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnConnectUpdate(ID_BATCH_LOCK, pCmdUI);
}

void CConnectView::OnComment()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnConnectCommand(ID_COMMENT);
}

void CConnectView::OnUpdateComment(CCmdUI *pCmdUI)
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnConnectUpdate(ID_COMMENT, pCmdUI);
}

void CConnectView::OnBatchUnlock()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnConnectCommand(ID_BATCH_UNLOCK);
}

void CConnectView::OnUpdateBatchUnlock(CCmdUI *pCmdUI)
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnConnectUpdate(ID_BATCH_UNLOCK, pCmdUI);
}

void CConnectView::OnBatchUpgrade()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnConnectCommand(ID_BATCH_UPGRADE);
}

void CConnectView::OnUpdateBatchUpgrade(CCmdUI *pCmdUI)
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnConnectUpdate(ID_BATCH_UPGRADE, pCmdUI);
}

BOOL CConnectView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_MOUSEMOVE)
	{
		CPoint pt = pMsg->pt;
		m_wndConnectView.ScreenToClient(&pt);
		UINT uFlags;
		HTREEITEM item;
		item = m_wndConnectView.HitTest(pt,&uFlags);
		if (item != NULL && (uFlags & TVHT_ONITEMLABEL))
		{
			if (item == m_wndConnectView.GetRootItem())
			{
				m_ToolTip.Activate(FALSE);
				m_dwLastClientId = 0;
			}
			else
			{
				DWORD dwId = m_wndConnectView.GetItemData(item);
				if (dwId != m_dwLastClientId)
				{
					m_dwLastClientId = dwId;
					std::vector<ClientInfo_t> vecInfo = theProtoManager->GetResClient();
					size_t i;
					for(i = 0; i < vecInfo.size(); i++)
					{
						if (vecInfo[i].dwClientId == dwId)
						{
							break;
						}
					}
					if (i == vecInfo.size())
					{
						m_ToolTip.Activate(FALSE);
						m_dwLastClientId = 0;
					}
					else
					{
						CString strDisplay = theApp.GenDisplayName(&vecInfo[i]);
						CString strText;
						strText.Format(_T("IP: %s\n机器名: %s\n名称: %s"), vecInfo[i].szIPOut, vecInfo[i].szHostname, strDisplay);
						//m_wndConnectView.GetItemText(item)
						m_ToolTip.Activate(TRUE);
						m_ToolTip.UpdateTipText(strText, &m_wndConnectView, m_wndConnectView.GetDlgCtrlID());
						m_ToolTip.RelayEvent(pMsg);
					}
				}
			}
		}
		else
		{
			m_ToolTip.Activate(FALSE);
			m_dwLastClientId = 0;
		}
	}

	return CDockablePane::PreTranslateMessage(pMsg);
}

void CConnectView::OnPolicyEdit()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnConnectCommand(ID_POLICY_EDIT);
}

void CConnectView::OnUpdatePolicyEdit(CCmdUI *pCmdUI)
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnConnectUpdate(ID_POLICY_EDIT, pCmdUI);
}

void CConnectView::OnGroupCreate()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnConnectCommand(ID_GROUP_CREATE);
}

void CConnectView::OnUpdateGroupCreate(CCmdUI *pCmdUI)
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnConnectUpdate(ID_GROUP_CREATE, pCmdUI);
}

void CConnectView::OnGroupEdit()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnConnectCommand(ID_GROUP_EDIT);
}

void CConnectView::OnUpdateGroupEdit(CCmdUI *pCmdUI)
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnConnectUpdate(ID_GROUP_EDIT, pCmdUI);
}

void CConnectView::OnGroupDel()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnConnectCommand(ID_GROUP_DEL);
}

void CConnectView::OnUpdateGroupDel(CCmdUI *pCmdUI)
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnConnectUpdate(ID_GROUP_DEL, pCmdUI);
}

void CConnectView::OnGroupAddto()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnConnectCommand(ID_GROUP_ADDTO);
}

void CConnectView::OnUpdateGroupAddto(CCmdUI *pCmdUI)
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnConnectUpdate(ID_GROUP_ADDTO, pCmdUI);
}

void CConnectView::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CDockablePane::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
}

void CConnectView::OnBatchAuto()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnConnectCommand(ID_BATCH_AUTO);
}

void CConnectView::OnUpdateBatchAuto(CCmdUI *pCmdUI)
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnConnectUpdate(ID_BATCH_AUTO, pCmdUI);
}
