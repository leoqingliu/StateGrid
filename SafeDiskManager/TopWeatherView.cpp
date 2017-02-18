// TopWeatherView.cpp : implementation file
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "TopWeatherView.h"


// CTopWeatherView

IMPLEMENT_DYNAMIC(CTopWeatherView, CDockablePane)

CTopWeatherView::CTopWeatherView()
{

}

CTopWeatherView::~CTopWeatherView()
{
}


BEGIN_MESSAGE_MAP(CTopWeatherView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CTopWeatherView message handlers

int CTopWeatherView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	//
// 	CRect rectDummy;
// 	rectDummy.SetRectEmpty();
// 
// 	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;
// 	if (!m_TextStatic.Create(_T(""), dwViewStyle, rectDummy, this, 4))
// 	{
// 		TRACE0("Can't Create Image View\n");
// 		return -1;
// 	}
// 
// 	AdjustLayout();

	VERIFY(m_Font.CreateFont(
		14,						   // nHeight
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
		_T("ËÎÌו")));			   // lpszFacename

	return 0;
}

void CTopWeatherView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

//	AdjustLayout();
}

void CTopWeatherView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

//	m_TextStatic.SetWindowPos(NULL, rectClient.left, rectClient.top, m_bitmapInfo.bmWidth, m_bitmapInfo.bmHeight, SWP_NOACTIVATE | SWP_NOZORDER);
//	m_TextStatic.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CTopWeatherView::IsResizable() const
{
//	return FALSE;
	return TRUE;
}

void CTopWeatherView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDockablePane::OnPaint() for painting messages

	CRect rect;
	GetClientRect(rect);
//	dc.FillSolidRect(rect, afxGlobalData.clrBarFace);
	dc.FillSolidRect(rect, RGB(77, 126, 131));
//	CBrush BGBrush;
//	BGBrush.CreateSolidBrush(RGB(255, 0, 0));
//	dc.SelectObject(&BGBrush);
//	dc.Rectangle(&rect);
	dc.SetBkMode(TRANSPARENT);
	
	//CFont *pFont = GetParent()->GetFont();
	
	//CFont *pOldFont = dc.SelectObject(pFont);
	CFont *pOldFont = dc.SelectObject(&m_Font);
	dc.SetTextColor(RGB(240, 240, 240));
	dc.DrawText(m_strText, &rect, DT_SINGLELINE|DT_LEFT|DT_VCENTER);
	dc.SelectObject(pOldFont);

//	m_TextStatic.SetBitmap(m_hBitmap);
//	m_TextStatic.Invalidate();

//	BGBrush.DeleteObject();

	CBrush brushLine(RGB(227, 255, 255));
	CRect rectTop = rect;
	rectTop.bottom = rectTop.top + 1;
	dc.FillRect(rectTop, &brushLine);
	CRect rectBottom = rect;
	rectBottom.top = rectBottom.bottom - 1;
	dc.FillRect(rectBottom, &brushLine);
}

VOID CTopWeatherView::SetText(CString strText)
{
	m_strText = strText;
}
