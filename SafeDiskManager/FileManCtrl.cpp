// FileManCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "FileManCtrl.h"
#include "ChildFrm.h"

// CFileManCtrl

IMPLEMENT_DYNAMIC(CFileManCtrl, CMFCListCtrl)

CFileManCtrl::CFileManCtrl()
{

}

CFileManCtrl::~CFileManCtrl()
{
}


BEGIN_MESSAGE_MAP(CFileManCtrl, CMFCListCtrl)
	ON_WM_KEYDOWN()
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &CFileManCtrl::OnLvnItemchanged)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CONTEXTMENU()
	ON_UPDATE_COMMAND_UI(ID_CHILD_DOWNLOAD, &CFileManCtrl::OnUpdateChildDownload)
	ON_COMMAND(ID_CHILD_DOWNLOAD, &CFileManCtrl::OnChildDownload)
	ON_UPDATE_COMMAND_UI(ID_CHILD_UPLOAD, &CFileManCtrl::OnUpdateChildUpload)
	ON_COMMAND(ID_CHILD_UPLOAD, &CFileManCtrl::OnChildUpload)
	ON_UPDATE_COMMAND_UI(ID_CHILD_REFRESH, &CFileManCtrl::OnUpdateChildRefresh)
	ON_COMMAND(ID_CHILD_REFRESH, &CFileManCtrl::OnChildRefresh)
END_MESSAGE_MAP()



// CFileManCtrl message handlers

void CFileManCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CChildFrame *pChildFrame = (CChildFrame *)GetParentFrame();
	INT iClickIndex;
	if(nChar == VK_RETURN)
	{
		// Click Index 
		iClickIndex = GetNextItem(-1, LVIS_SELECTED);
		pChildFrame->OnFileViewDblClick(iClickIndex);
	}
	else if(nChar == VK_BACK)
	{
		/*
		if(m_iRemoteSelLevel == 0)
		{
			return;
		}
		else
		{
			OnDblClickFileView(0);
		}
		*/
	}
	else if(
		nChar == VK_LEFT ||
		nChar == VK_RIGHT || 
		nChar == VK_UP || 
		nChar == VK_DOWN || 
		nChar == VK_HOME || 
		nChar == VK_END )
	{
		// Click Index 
		iClickIndex = GetNextItem(-1, LVIS_SELECTED);
		pChildFrame->OnFileViewSelChanged(iClickIndex);
	}

	CMFCListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

int CFileManCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMFCListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	//
	lpCreateStruct->style = LVS_REPORT;

	//
	m_ImageList.Create(32, 32, ILC_COLOR32|ILC_MASK, 0, 0);
	SetImageList(&m_ImageList, LVSIL_NORMAL);

	return 0;
}

void CFileManCtrl::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	//
	CChildFrame *pChildFrame = (CChildFrame *)GetParentFrame();
	if((pNMLV->uOldState & LVIS_SELECTED) == 0 && (pNMLV->uNewState & LVIS_SELECTED) == LVIS_SELECTED) 
	{
		pChildFrame->OnFileViewSelChanged(pNMLV->iItem);
	}

	*pResult = 0;
}

void CFileManCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CMFCListCtrl::OnLButtonDown(nFlags, point);

	// Click Index 
	CChildFrame *pChildFrame = (CChildFrame *)GetParentFrame();
	INT iClickIndex = GetNextItem(-1, LVIS_SELECTED);
	if (-1 == iClickIndex)
	{
		pChildFrame->OnFileViewSelChanged(iClickIndex);
	}
}

void CFileManCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CMFCListCtrl::OnLButtonDblClk(nFlags, point);

	// Click Index
	CChildFrame *pChildFrame = (CChildFrame *)GetParentFrame();
	INT iClickIndex = GetNextItem(-1, LVIS_SELECTED);
	pChildFrame->OnFileViewDblClick(iClickIndex);
}

void CFileManCtrl::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_MENU_CHILD, point.x, point.y, this, TRUE);
}

void CFileManCtrl::OnUpdateChildDownload(CCmdUI *pCmdUI)
{
	CChildFrame *pChildFrame = (CChildFrame *)GetParentFrame();
	pChildFrame->OnUpdateChildDownload(pCmdUI);
}

void CFileManCtrl::OnChildDownload()
{
	CChildFrame *pChildFrame = (CChildFrame *)GetParentFrame();
	pChildFrame->OnChildDownload();
}

void CFileManCtrl::OnUpdateChildUpload(CCmdUI *pCmdUI)
{
	CChildFrame *pChildFrame = (CChildFrame *)GetParentFrame();
	pChildFrame->OnUpdateChildUpload(pCmdUI);
}

void CFileManCtrl::OnChildUpload()
{
	CChildFrame *pChildFrame = (CChildFrame *)GetParentFrame();
	pChildFrame->OnChildUpload();
}

void CFileManCtrl::OnUpdateChildRefresh(CCmdUI *pCmdUI)
{
	CChildFrame *pChildFrame = (CChildFrame *)GetParentFrame();
	pChildFrame->OnUpdateChildRefresh(pCmdUI);
}

void CFileManCtrl::OnChildRefresh()
{
	CChildFrame *pChildFrame = (CChildFrame *)GetParentFrame();
	pChildFrame->OnChildRefresh();
}
