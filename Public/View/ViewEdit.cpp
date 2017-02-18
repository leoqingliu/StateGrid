// ViewCmd.cpp : implementation file
//

#include "stdafx.h"
#include "ViewEdit.h"

// CViewRichEdit

IMPLEMENT_DYNAMIC(CViewRichEdit, CRichEditCtrl)

CViewRichEdit::CViewRichEdit()
:m_dwCursor(0)
,m_pCallBack(NULL)
{

}

CViewRichEdit::~CViewRichEdit()
{
}

BEGIN_MESSAGE_MAP(CViewRichEdit, CRichEditCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(EN_MSGFILTER, &CViewRichEdit::OnEnMsgfilter)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

// CViewCmd message handlers
int CViewRichEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CRichEditCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 
	SetEventMask(ENM_KEYEVENTS);

	return 0;
}

void CViewRichEdit::SetEditCallBack(CViewRichEditCallBack* pCallBack)
{
	m_pCallBack = pCallBack;
}

CViewRichEditCallBack* CViewRichEdit::GetEditCallBack()
{
	return m_pCallBack;
}

BOOL CViewRichEdit::PreCreateWindow(CREATESTRUCT& cs)
{
	// 
	cs.style |= ES_MULTILINE|ES_WANTRETURN|ES_AUTOHSCROLL|ES_AUTOVSCROLL;
	return CRichEditCtrl::PreCreateWindow(cs);
}

void CViewRichEdit::OnEnMsgfilter(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_pCallBack->OnEnMsgfilter(pNMHDR, pResult);
}

void CViewRichEdit::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
}
