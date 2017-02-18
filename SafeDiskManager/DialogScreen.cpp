// DialogScreen.cpp : implementation file
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "DialogScreen.h"
#include "ChildWnd.h"
#include "ChildFrm.h"

// CDialogScreen dialog

IMPLEMENT_DYNAMIC(CDialogScreen, CDialog)

CDialogScreen::CDialogScreen(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogScreen::IDD, pParent)
{

}

CDialogScreen::~CDialogScreen()
{
}

void CDialogScreen::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogScreen, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SCREEN, &CDialogScreen::OnBnClickedButtonScreen)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CDialogScreen::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_VNC_VIEW, &CDialogScreen::OnBnClickedButtonVncView)
	ON_BN_CLICKED(IDC_BUTTON_VNC_CONTROL, &CDialogScreen::OnBnClickedButtonVncControl)
	ON_BN_CLICKED(IDC_BUTTON_SCREEN_AUTO, &CDialogScreen::OnBnClickedButtonScreenAuto)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDialogScreen message handlers


BOOL CDialogScreen::OnInitDialog()
{
	CDialog::OnInitDialog();

	//
	if (theRelayUpDownInfo.bUpdated &&
		theRelayUpDownInfo.bFileTrans)
	{
	}
	else
	{
		GetDlgItem(IDC_BUTTON_SCREEN)->EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogScreen::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages

	CRect rect;
	GetClientRect(rect);
	dc.FillSolidRect(rect, RGB(255, 255, 255));
	dc.SetBkMode(TRANSPARENT);
}

void CDialogScreen::OnBnClickedButtonScreen()
{
	CChildFrame *pChildFrame = (CChildFrame *)GetParentFrame();
	pChildFrame->CaptureScreen();
}

void CDialogScreen::OnBnClickedButtonOpen()
{
	CChildFrame *pChildFrame = (CChildFrame *)GetParentFrame();
	DWORD dwId = pChildFrame->GetId();
	TCHAR szFilePath[MAX_PATH];
	CString strLocalDir = theConfig.m_Info.szLocalDir;
	if(strLocalDir.Right(1) != _T('\\'))
	{
		strLocalDir += _T("\\");
	}
	wsprintf(szFilePath, _T("%s%d.Screen.bmp"), (LPCTSTR)strLocalDir, dwId);

	//
	// Execute
	//
	BOOL bOK = FALSE;
	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize		= sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask		= SEE_MASK_NOCLOSEPROCESS;
//	ShExecInfo.fMask		|=SEE_MASK_FLAG_NO_UI;	//don't display error dialog
	ShExecInfo.hwnd			= ::GetDesktopWindow();
	ShExecInfo.lpVerb		= NULL;
	ShExecInfo.lpFile		= szFilePath;
	ShExecInfo.lpParameters = _T("");
	ShExecInfo.lpDirectory	= NULL;
	ShExecInfo.nShow		= SW_SHOW;
	ShExecInfo.hInstApp		= NULL;	
	ShellExecuteEx(&ShExecInfo);
}

void CDialogScreen::OnBnClickedButtonVncView()
{
	CChildFrame *pChildFrame = (CChildFrame *)GetParentFrame();
	pChildFrame->ScreenRealtimeView();
}

void CDialogScreen::OnBnClickedButtonVncControl()
{
	CChildFrame *pChildFrame = (CChildFrame *)GetParentFrame();
	pChildFrame->ScreenRealtimeControl();
}

void CDialogScreen::OnBnClickedButtonScreenAuto()
{
	UINT uTime = GetDlgItemInt(IDC_EDIT_AUTO_TIME);
	CChildFrame *pChildFrame = (CChildFrame *)GetParentFrame();
	pChildFrame->OnControlSetScreenTime(uTime);
}

void CDialogScreen::SetAutoTime(UINT uTime)
{
	SetDlgItemInt(IDC_EDIT_AUTO_TIME, uTime);
}

HBRUSH CDialogScreen::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(NULL_BRUSH);
	}

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}
