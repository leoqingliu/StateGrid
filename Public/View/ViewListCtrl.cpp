// ViewListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ViewListCtrl.h"
#include <assert.h>
#include "SafeDiskManager.h"
#include "ChildFrm.h"

// CViewListCtrl

IMPLEMENT_DYNAMIC(CViewListCtrl, CMFCListCtrl)
CViewListCtrl::CViewListCtrl()
:m_ContextMenuType(CONTEXT_MENU_NORMAL)
{

}

CViewListCtrl::~CViewListCtrl()
{
}

BEGIN_MESSAGE_MAP(CViewListCtrl, CMFCListCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_RCLICK, &CViewListCtrl::OnNMRClick)
	ON_WM_RBUTTONUP()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CHILD_UPDATE, &CViewListCtrl::OnChildUpdate)
	ON_COMMAND(ID_CHILD_SAVE, &CViewListCtrl::OnChildSave)
	ON_COMMAND(ID_CHILD_KILL_PROCESS, &CViewListCtrl::OnChildKillProcesss)
END_MESSAGE_MAP()

// CViewListCtrl message handlers
int CViewListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMFCListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 
	DWORD dwStyle = GetExtendedStyle();
	dwStyle |= LVS_SHAREIMAGELISTS|LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP;
	SetExtendedStyle(dwStyle);

	return 0;
}

void CViewListCtrl::AutoScroll()
{
	//
	assert(m_hWnd != NULL);

	::SendMessage(m_hWnd, WM_VSCROLL, SB_BOTTOM, 0);
}

void CViewListCtrl::AdjustColumnWidth()
{
	//
	assert(m_hWnd != NULL);

	// ReDraw
	::SendMessage(m_hWnd, WM_SETREDRAW, FALSE, 0);

	HWND hHeaderWnd = (HWND)::SendMessage(m_hWnd, LVM_GETHEADER, 0, 0);
	int nColumnCount = (int)::SendMessage(hHeaderWnd, HDM_GETITEMCOUNT, 0, 0L);
	for (int i = 0; i < nColumnCount; i++)
	{
		// Set Column Width
		::SendMessage(m_hWnd, LVM_SETCOLUMNWIDTH, i, MAKELPARAM(LVSCW_AUTOSIZE, 0));

		int nColumnWidth = (int)::SendMessage(m_hWnd, LVM_GETCOLUMNWIDTH, i, 0);
		::SendMessage(m_hWnd, LVM_SETCOLUMNWIDTH, i, MAKELPARAM(LVSCW_AUTOSIZE_USEHEADER, 0));

		int nHeaderWidth = (int)::SendMessage(m_hWnd, LVM_GETCOLUMNWIDTH, i, 0);
		::SendMessage(m_hWnd, LVM_SETCOLUMNWIDTH, i, MAKELPARAM(max(nColumnWidth, nHeaderWidth), 0));

		// ReDraw
		::SendMessage(m_hWnd, WM_SETREDRAW, TRUE, 0);
	} 
}

void CViewListCtrl::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	CFrameWnd *pWnd = GetParentFrame();
	if (pWnd->IsKindOf(RUNTIME_CLASS(CChildFrame)))
	{
		LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
		CPoint point;
		GetCursorPos(&point);
		CMenu menu;
		menu.LoadMenu(IDR_MENU_CHILD_WND);
		CMenu *pMenu = menu.GetSubMenu(0);
		if (CONTEXT_MENU_PROCESS != m_ContextMenuType)
		{
			pMenu->DeleteMenu(ID_CHILD_KILL_PROCESS, MF_BYCOMMAND);
		}
		//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_MENU_CHILD_WND, point.x, point.y, this, TRUE);
		theApp.GetContextMenuManager()->ShowPopupMenu(pMenu->GetSafeHmenu(), point.x, point.y, this, TRUE);
	}
	*pResult = 1;
}

void CViewListCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	// 
	CMFCListCtrl::OnRButtonUp(nFlags, point);
}

BOOL CViewListCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	// 
	cs.style|=	WS_HSCROLL|WS_VSCROLL|LVS_REPORT;
	//
	return CMFCListCtrl::PreCreateWindow(cs);
}

int CViewListCtrl::FindItem( LPCTSTR lpBuffer, int iColumn )
{
	int iCount = GetItemCount();
	int iRet = -1;
	TCHAR szBuffer[MAX_PATH] = {0};
	for(int i = 0; i < iCount; i++)
	{
		GetItemText(i, iColumn, szBuffer, MAX_PATH);
		if(lstrlen(szBuffer) == 0)
		{
			continue;
		}
		if(lstrcmp(lpBuffer, szBuffer) == 0)
		{
			iRet = i;
			break;
		}
	}
	return iRet;
}

int CViewListCtrl::FindItem( int iCode, int iColumn )
{
	TCHAR szBuffer[MAX_PATH] = {0};
	wsprintf(szBuffer, _T("%d"), iCode);
	return FindItem(szBuffer, iColumn);
}

void CViewListCtrl::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CFrameWnd *pWnd = GetParentFrame();
	if (pWnd->IsKindOf(RUNTIME_CLASS(CChildFrame)))
	{
		CMenu menu;
		menu.LoadMenu(IDR_MENU_CHILD_WND);
		CMenu *pMenu = menu.GetSubMenu(0);
		if (CONTEXT_MENU_PROCESS != m_ContextMenuType)
		{
			pMenu->DeleteMenu(ID_CHILD_KILL_PROCESS, MF_BYCOMMAND);
		}
		theApp.GetContextMenuManager()->ShowPopupMenu(pMenu->GetSafeHmenu(), point.x, point.y, this, TRUE);
	}
}

void CViewListCtrl::OnChildSave()
{
	CFrameWnd *pWnd = GetParentFrame();
	if (pWnd->IsKindOf(RUNTIME_CLASS(CChildFrame)))
	{
		CChildFrame *pChildFrame = (CChildFrame *)pWnd;
		pChildFrame->OnSave(GetDlgCtrlID());
	}
}

void CViewListCtrl::OnChildUpdate()
{
	CFrameWnd *pWnd = GetParentFrame();
	if (pWnd->IsKindOf(RUNTIME_CLASS(CChildFrame)))
	{
		CChildFrame *pChildFrame = (CChildFrame *)pWnd;
		pChildFrame->OnUpdate(GetDlgCtrlID());
	}
}

void CViewListCtrl::OnChildKillProcesss()
{
	CFrameWnd *pWnd = GetParentFrame();
	if (pWnd->IsKindOf(RUNTIME_CLASS(CChildFrame)))
	{
		CChildFrame *pChildFrame = (CChildFrame *)pWnd;
		pChildFrame->OnKillProcess(GetDlgCtrlID());
	}
}

void CViewListCtrl::SetContextMenuType(CONTEXT_MENU menu)
{
	m_ContextMenuType = menu;
}
