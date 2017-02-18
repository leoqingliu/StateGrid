
#include "stdafx.h"
#include "SafeDiskManager.h"
#include "LogWnd.h"
#include "Resource.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar

CLogWnd::CLogWnd()
	:m_pFont(NULL)
{
}

CLogWnd::~CLogWnd()
{
}

BEGIN_MESSAGE_MAP(CLogWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

int CLogWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 
	//
	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create output panes:
	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | LVS_SHOWSELALWAYS;
	if (!m_List.Create(dwStyle, rectDummy, this, 2))
	{
		TRACE0("Failed to create output windows\n");
		return -1;      // fail to create
	}

	// Set Font
	if (m_pFont)
	{
		//m_List.SetFont(m_pFont);
	}
	m_List.SetFont(&theApp.m_Font);

	return 0;
}

void CLogWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// Tab control should cover the whole client area:
	m_List.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

