// WindowDlg.cpp : implementation file
//

#include "stdafx.h"
#include <resource.h>
#include "WindowDlg.h"
#include "../Public/Utils.h"

// CWindowDlg dialog

IMPLEMENT_DYNAMIC(CWindowDlg, CDialog)

CWindowDlg::CWindowDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWindowDlg::IDD, pParent)
	, m_strWindow(_T(""))
	, m_strClass(_T(""))
	, m_strProcess(_T(""))
	, m_strDesc(_T(""))
	, m_iType(0)
{
	m_bDisableExcept = FALSE;
}

CWindowDlg::~CWindowDlg()
{
}

void CWindowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_WINDOW, m_strWindow);
	DDX_Text(pDX, IDC_EDIT_CLASS, m_strClass);
	DDX_Control(pDX, IDC_LOOK, m_ctrlLook);
	DDX_Text(pDX, IDC_EDIT_PROCESS, m_strProcess);
	DDX_Text(pDX, IDC_EDIT_DESC, m_strDesc);
	DDX_Radio(pDX, IDC_RADIO_NORMAL, m_iType);
}


BEGIN_MESSAGE_MAP(CWindowDlg, CDialog)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDOK, &CWindowDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CWindowDlg message handlers

BOOL CWindowDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);		// Set big icon
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);		// Set small icon

	// On Top
	BOOL bAlwaysOnTop = TRUE;
	::SetWindowPos(GetSafeHwnd(), (bAlwaysOnTop) ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	m_bIsLooking = FALSE;
	m_hWndPrev = NULL;

	m_hIconBlank = (HICON)LoadImage(AfxGetApp()->m_hInstance,
		MAKEINTRESOURCE(IDI_BLANK_ICON),
		IMAGE_ICON,
		GetSystemMetrics(SM_CXICON),
		GetSystemMetrics(SM_CXICON),
		0);
	m_hIconScan = (HICON)LoadImage(AfxGetApp()->m_hInstance,
		MAKEINTRESOURCE(IDI_LOOK_ICON),
		IMAGE_ICON,
		GetSystemMetrics(SM_CXICON),
		GetSystemMetrics(SM_CXICON),
		0);
	m_hCursorScan = (HCURSOR)LoadImage(AfxGetApp()->m_hInstance,
		MAKEINTRESOURCE(IDC_LOOK_CUR),
		IMAGE_CURSOR,
		GetSystemMetrics(SM_CXCURSOR),
		GetSystemMetrics(SM_CXCURSOR),
		0);
	m_hCursorPrev = NULL;

	if (m_bDisableExcept)
	{
		GetDlgItem(IDC_RADIO_NORMAL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_BLACK)->ShowWindow(SW_HIDE);
//		GetDlgItem(IDC_RADIO_SPEC)->ShowWindow(SW_HIDE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CWindowDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd *pWnd = ChildWindowFromPoint(point);
	if(pWnd != NULL && pWnd->GetSafeHwnd() == m_ctrlLook.GetSafeHwnd())
		StartLooking();

	CDialog::OnLButtonDown(nFlags, point);
}

void CWindowDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_bIsLooking)
		StopLooking();

	CDialog::OnLButtonUp(nFlags, point);
}

void CWindowDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bIsLooking)
		Scan(point);

	CDialog::OnMouseMove(nFlags, point);
}

BOOL CWindowDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE && m_bIsLooking)
	{
		StopLooking();
		return TRUE;
	}
	else
	{
		return CDialog::PreTranslateMessage(pMsg);
	}
}

void CWindowDlg::StartLooking(void)
{
	SetCapture();
	m_bIsLooking = true;

	m_hCursorPrev = SetCursor(m_hCursorScan);
	m_ctrlLook.SetIcon(m_hIconBlank);
}

void CWindowDlg::StopLooking(void)
{
	ReleaseCapture();
	m_bIsLooking = false;

	if(m_hWndPrev != NULL)
	{
		InvertBorder(m_hWndPrev);
		m_hWndPrev = NULL;
	}

	SetCursor(m_hCursorPrev);
	m_ctrlLook.SetIcon(m_hIconScan);

	// Redraw the whole screen
	//::InvalidateRect(NULL, NULL, FALSE);
}

void CWindowDlg::Scan(CPoint point)
{
	if (!m_bIsLooking)
	{
		return;
	}

	bool bFound = false;
	ClientToScreen(&point);

	HWND hWnd = SmallestWindowFromPoint(point);
	if (hWnd != NULL)
	{
		// Check Window Is Not myself
		if (GetWindowThreadProcessId(GetSafeHwnd(), NULL) != GetWindowThreadProcessId(hWnd, NULL))
		{
			if (hWnd != m_hWndPrev)
			{	
				InvertBorder(m_hWndPrev);
				m_hWndPrev = hWnd;
				InvertBorder(m_hWndPrev);
			}

			TCHAR szBuffer[256];
			// Get Window
			if (::GetWindowText(hWnd, szBuffer, sizeof(szBuffer) / sizeof(TCHAR)))
				m_strWindow = szBuffer;
			
			// Get Class
			if (::GetClassName(hWnd, szBuffer, sizeof(szBuffer) / sizeof(TCHAR)))
				m_strClass = szBuffer;

			DWORD dwProcessId = 0;
			DWORD dwThreadId = GetWindowThreadProcessId(hWnd, &dwProcessId);
			if (0 != dwProcessId)
			{
				m_strProcess = Utils::GetProcessName(dwProcessId);

				CString strFileDesc;
				CString strProductDesc;
				CString strFileVersion;
				BOOL bRet = Utils::GetPidInfo(dwProcessId, strFileDesc, strProductDesc, strFileVersion);
				if (bRet)
				{
					m_strDesc = strFileDesc;
				}
			}
		}
		else
		{
			InvertBorder(m_hWndPrev);
			m_hWndPrev = NULL;
		}
	}

	UpdateData(FALSE);
}

HWND CWindowDlg::SmallestWindowFromPoint(const POINT point)
{
	RECT rect, rectSearch;
	HWND pWnd, hWnd, hSearchWnd;

	hWnd = ::WindowFromPoint(point);
	if (hWnd != NULL)
	{
		// Get Rect And Parent Wnd
		::GetWindowRect(hWnd, &rect);
		pWnd = ::GetParent(hWnd);

		// Find Parent
		if (pWnd != NULL)
		{
			// Find On Z Order
			hSearchWnd = hWnd;
			do
			{
				hSearchWnd = ::GetWindow(hSearchWnd, GW_HWNDNEXT);

				// Find Parent Contain Same Point And Same Parent
				::GetWindowRect(hSearchWnd, &rectSearch);
				if (::PtInRect(&rectSearch, point) && ::GetParent(hSearchWnd) == pWnd && ::IsWindowVisible(hSearchWnd))
				{
					// Find Small
					if (((rectSearch.right - rectSearch.left) * (rectSearch.bottom - rectSearch.top)) < ((rect.right - rect.left) * (rect.bottom - rect.top)))
					{
						// Replace Find
						hWnd = hSearchWnd;
						::GetWindowRect(hWnd, &rect);
					}
				}
			}
			while (hSearchWnd != NULL);
		}
	}

	return hWnd;
}

void CWindowDlg::InvertBorder(const HWND hWnd)
{
	if(!IsWindow(hWnd))
		return;

	RECT rect;

	// Get the coordinates of the window on the screen
	::GetWindowRect(hWnd, &rect);

	// Get a handle to the window's device context
	HDC hDC = ::GetWindowDC(hWnd);

	// Create an inverse pen that is the size of the window border
	SetROP2(hDC, R2_NOT);

	HPEN hPen = CreatePen(PS_INSIDEFRAME, 3 * GetSystemMetrics(SM_CXBORDER), RGB(0,0,0));

	// Draw the rectangle around the window
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));

	Rectangle(hDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top);

	SelectObject(hDC, hOldBrush);
	SelectObject(hDC, hOldPen);

	// Give the window its device context back, and destroy our pen
	::ReleaseDC(hWnd, hDC);

	DeleteObject(hPen);
}

void CWindowDlg::OnBnClickedOk()
{
	UpdateData();
	if (m_strWindow.IsEmpty() && m_strClass.IsEmpty() && m_strProcess.IsEmpty() && m_strDesc.IsEmpty())
	{
		AfxMessageBox(_T("输入不能为空！"), MB_ICONERROR);
		return;
	}

	OnOK();
}

BOOL CWindowDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)lParam;
	INT iCtrlId = LOWORD(wParam);
	INT iCmdId = HIWORD(wParam);
	if (IDC_RADIO_NORMAL == iCtrlId ||
		IDC_RADIO_BLACK == iCtrlId
		)
	{
		UpdateData();
	}

	return CDialog::OnCommand(wParam, lParam);
}
