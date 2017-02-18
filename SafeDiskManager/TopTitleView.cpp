// TopTitleView.cpp : implementation file
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "TopTitleView.h"


// CTopTitleView

IMPLEMENT_DYNAMIC(CTopTitleView, CDockablePane)

CTopTitleView::CTopTitleView()
{

}

CTopTitleView::~CTopTitleView()
{
}


BEGIN_MESSAGE_MAP(CTopTitleView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CTopTitleView message handlers



int CTopTitleView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	//
	CRect rectDummy;
	rectDummy.SetRectEmpty();

	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;
	if (!m_ImageStatic.Create(_T(""), dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("Can't Create Image View\n");
		return -1;
	}

	AdjustLayout();

	m_hBitmap = ::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_TOPTITLE));
	m_ImageStatic.ModifyStyle(0xF, SS_BITMAP | SS_LEFT /* SS_CENTERIMAGE*/);
	m_ImageStatic.SetBitmap(m_hBitmap);

	GetObject(m_hBitmap, sizeof (BITMAP), &m_bitmapInfo);
	
	return 0;
}

void CTopTitleView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	m_ImageStatic.SetWindowPos(NULL, rectClient.left, rectClient.top, m_bitmapInfo.bmWidth, m_bitmapInfo.bmHeight, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CTopTitleView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CTopTitleView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDockablePane::OnPaint() for painting messages

	CRect rect;
	GetClientRect(rect);
	dc.FillSolidRect(rect, afxGlobalData.clrBarFace);
	
	m_ImageStatic.SetBitmap(m_hBitmap);
	m_ImageStatic.Invalidate();
}

BOOL CTopTitleView::IsResizable() const
{
	return FALSE;
}