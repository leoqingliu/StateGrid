// DialogControl.cpp : implementation file
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "DialogControl.h"
#include "ChildFrm.h"

// CDialogControl dialog
IMPLEMENT_DYNAMIC(CDialogControl, CDialog)

CDialogControl::CDialogControl(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogControl::IDD, pParent)
{
}

CDialogControl::~CDialogControl()
{
}

void CDialogControl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogControl, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_RESTART, &CDialogControl::OnBnClickedButtonRestart)
	ON_BN_CLICKED(IDC_BUTTON_SHUTDOWN, &CDialogControl::OnBnClickedButtonShutdown)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_LOGOFF, &CDialogControl::OnBnClickedButtonLogoff)
END_MESSAGE_MAP()


// CDialogControl message handlers

BOOL CDialogControl::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogControl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages

	CRect rect;
	GetClientRect(rect);
	dc.FillSolidRect(rect, RGB(255, 255, 255));
	dc.SetBkMode(TRANSPARENT);
}

void CDialogControl::OnBnClickedButtonRestart()
{
	CChildFrame *pChildFrame = (CChildFrame *)GetParentFrame();
	pChildFrame->OnControlRestart();
}

void CDialogControl::OnBnClickedButtonShutdown()
{
	CChildFrame *pChildFrame = (CChildFrame *)GetParentFrame();
	pChildFrame->OnControlShutdown();
}

void CDialogControl::OnBnClickedButtonLogoff()
{
	CChildFrame *pChildFrame = (CChildFrame *)GetParentFrame();
	pChildFrame->OnControlLogoff();
}
