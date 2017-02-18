// DialogLock.cpp : implementation file
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "DialogLock.h"
#include "ChildFrm.h"

// CDialogLock dialog

IMPLEMENT_DYNAMIC(CDialogLock, CDialog)

CDialogLock::CDialogLock(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogLock::IDD, pParent)
{

}

CDialogLock::~CDialogLock()
{
}

void CDialogLock::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogLock, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_LOCK, &CDialogLock::OnBnClickedButtonLock)
	ON_BN_CLICKED(IDC_BUTTON_UNLOCK, &CDialogLock::OnBnClickedButtonUnlock)
	ON_BN_CLICKED(IDC_BUTTON_LOCK_AUTO, &CDialogLock::OnBnClickedButtonLockAuto)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_SET_PASSWORD, &CDialogLock::OnBnClickedButtonSetPassword)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDialogLock message handlers

BOOL CDialogLock::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogLock::OnBnClickedButtonLock()
{
	CChildFrame *pChildFrame = (CChildFrame *)GetParentFrame();
	pChildFrame->OnControlLock();
}

void CDialogLock::OnBnClickedButtonUnlock()
{
	CChildFrame *pChildFrame = (CChildFrame *)GetParentFrame();
	pChildFrame->OnControlUnLock();
}

void CDialogLock::OnBnClickedButtonLockAuto()
{
	CChildFrame *pChildFrame = (CChildFrame *)GetParentFrame();
	pChildFrame->OnControlLockAuto();
}

void CDialogLock::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages

	CRect rect;
	GetClientRect(rect);
	dc.FillSolidRect(rect, RGB(255, 255, 255));
	dc.SetBkMode(TRANSPARENT);
}

void CDialogLock::OnBnClickedButtonSetPassword()
{
	CString strPassword;
	GetDlgItemText(IDC_EDIT_PASSWORD, strPassword);
	CString strIdleTime;
	GetDlgItemText(IDC_EDIT_IDLE_TIME, strIdleTime);
	if (0 != strPassword.GetLength() && 0 != strIdleTime.GetLength())
	{
		CChildFrame *pChildFrame = (CChildFrame *)GetParentFrame();
		pChildFrame->OnControlSetPassword(strPassword, _tstol(strIdleTime));
	}

//	DWORD dwIdleTime = GetDlgItemInt(IDC_EDIT_IDLE_TIME);
}

void CDialogLock::SetLockInfo(CString strPassword, DWORD dwIdleTime)
{
	GetDlgItem(IDC_EDIT_PASSWORD)->SetWindowText(strPassword);
	SetDlgItemInt(IDC_EDIT_IDLE_TIME, dwIdleTime);
}

HBRUSH CDialogLock::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
