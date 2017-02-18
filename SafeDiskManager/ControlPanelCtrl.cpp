// ControlPanelCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "ControlPanelCtrl.h"
#include "ChildFrm.h"
#include "MainFrm.h"

// CControlPanelCtrl

IMPLEMENT_DYNAMIC(CControlPanelCtrl, CMFCListCtrl)

CControlPanelCtrl::CControlPanelCtrl()
{

}

CControlPanelCtrl::~CControlPanelCtrl()
{
}


BEGIN_MESSAGE_MAP(CControlPanelCtrl, CMFCListCtrl)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// CControlPanelCtrl message handlers

int CControlPanelCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMFCListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_ExLargeImageList.Create(128, 128, ILC_COLOR32|ILC_MASK, 0, 0);
	m_SmallImageList.Create(16, 16, ILC_COLOR32|ILC_MASK, 0, 0);

	SetImageList(&m_ExLargeImageList, LVSIL_NORMAL);
	SetImageList(&m_SmallImageList, LVSIL_SMALL);

	return 0;
}

void CControlPanelCtrl::OnSize(UINT nType, int cx, int cy)
{
	CMFCListCtrl::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}

BOOL CControlPanelCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= LVS_AUTOARRANGE;
	cs.style |= LVS_ICON;

	return CMFCListCtrl::PreCreateWindow(cs);
}

VOID CControlPanelCtrl::UpdateList()
{
	/*
	std::vector<ClientInfo_t> vecClient = theProtoManager->GetResClient(FALSE);
	DeleteAllItems();
	ClearExLargeImageList();
	ClearSmallImageList();
	int iPos = 0;
	for (int i = 0; i < (int)vecClient.size(); i++)
	{
		const ClientInfo_t& Info = vecClient[i];
		CBitmap *pBitmap = NULL;
		CBitmap bitmap;
		if (Info.bOnline)
			bitmap.LoadBitmap(IDB_BITMAP_COMPUTER_ONLINE);
		else
			bitmap.LoadBitmap(IDB_BITMAP_COMPUTER_OFFLINE);
		pBitmap = &bitmap;
		CString strName;
		strName.Format(_T("%s(%s)"), Info.szHostname, Info.szIPOut);
		m_ExLargeImageList.Add(pBitmap, RGB(255, 255, 255));
		LVITEM lvi;
		lvi.mask = LVIF_IMAGE|LVIF_TEXT;
		lvi.iItem	= iPos;
		lvi.iSubItem= 0;
		lvi.iImage	= iPos;
		lvi.pszText	= (LPTSTR)(LPCTSTR)strName;
		InsertItem(&lvi);
		//SetItemData(iPos, TRUE);
		SetItemData(iPos, Info.dwClientId);
		iPos++;
	}
	*/
	
	CMainFrame *pFrame = (CMainFrame *)AfxGetApp()->GetMainWnd();
	std::vector<ClientInfo_t> vecClient = theProtoManager->GetResClient(FALSE);
	for (int j = 0; j < (int)vecClient.size(); j++)
	{
		const ClientInfo_t &Info = vecClient[j];
		CString strNewName = theApp.GenDisplayName(&Info);
		int iImageId = (Info.bOnline ? (Info.bLocked ? IDB_BITMAP_COMPUTER_ONLINE_LOCKED : IDB_BITMAP_COMPUTER_ONLINE): IDB_BITMAP_COMPUTER_OFFLINE);
		BOOL bFound = FALSE;
		int iSelPos = -1;
		for (int i = 0; i < (int)GetItemCount(); i++)
		{
			DWORD dwClientId = this->GetItemData(i);
			if (Info.dwClientId == dwClientId)
			{
				bFound = TRUE;
				iSelPos = i;
				break;
			}
		}
		if (!bFound)
		{
			CBitmap *pBitmap = NULL;
			CBitmap bitmap;
			bitmap.LoadBitmap(iImageId);
			pBitmap = &bitmap;
			CString strName = theApp.GenDisplayName(&Info);
			m_ExLargeImageList.Add(pBitmap, RGB(255, 255, 255));
			LVITEM lvi;
			lvi.mask = LVIF_IMAGE | LVIF_TEXT;
			lvi.iItem = this->GetItemCount();
			lvi.iSubItem = 0;
			lvi.iImage = this->GetItemCount();
			lvi.pszText	= (LPTSTR)(LPCTSTR)strName;
			int iPos = InsertItem(&lvi);
			SetItemData(iPos, Info.dwClientId);
			m_mapItemImage.insert(std::map<INT, INT>::value_type(Info.dwClientId, iImageId));
		}
		else
		{
			// Image
			std::map<INT, INT>::iterator pIter = m_mapItemImage.find(Info.dwClientId);
			if (pIter != m_mapItemImage.end())
			{
				if (iImageId != pIter->second)
				{
					CBitmap *pBitmap = NULL;
					CBitmap bitmap;
					bitmap.LoadBitmap(iImageId);
					pBitmap = &bitmap;
					m_ExLargeImageList.Replace(iSelPos, pBitmap, NULL);
					this->RedrawItems(iSelPos, iSelPos);
					pIter->second = iImageId;
				}
			}

			// Text
			CString strCurText = GetItemText(iSelPos, 0);
			if (strCurText != strNewName)
			{
				SetItemText(iSelPos, 0, strNewName);
			}
		}
	}
}

void CControlPanelCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CMFCListCtrl::OnLButtonDblClk(nFlags, point);
	int iClickIndex = GetNextItem(-1, LVIS_SELECTED);
//	CChildFrame *pChildFrame = (CChildFrame *)GetParentFrame();
//	pChildFrame->OnFileViewDblClick(iClickIndex);
	if (-1 != iClickIndex)
	{
		DWORD dwClientId = GetItemData(iClickIndex);
		CMainFrame *pFrame = (CMainFrame *)AfxGetApp()->GetMainWnd();
		pFrame->onDbClickClient(dwClientId);
	}
}
