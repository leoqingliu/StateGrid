// UDiskLoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ScreenLocker.h"
#include "UDiskLoginDlg.h"

#define TIMER_SET_SETFOREGROUND	1

// CUDiskLoginDlg dialog

IMPLEMENT_DYNAMIC(CUDiskLoginDlg, CDialog)

CUDiskLoginDlg::CUDiskLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUDiskLoginDlg::IDD, pParent)
	, m_strPassword(_T(""))
{

}

CUDiskLoginDlg::~CUDiskLoginDlg()
{
}

void CUDiskLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPassword);
}


BEGIN_MESSAGE_MAP(CUDiskLoginDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_OK, &CUDiskLoginDlg::OnBnClickedBtnOk)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CUDiskLoginDlg message handlers

BOOL CUDiskLoginDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);		// Set big icon
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);		// Set small icon

	CenterWindow();
	SetTimer(TIMER_SET_SETFOREGROUND, 500, NULL);

	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CUDiskLoginDlg::OnBnClickedBtnOk()
{
	UpdateData();
	if (m_strPassword.IsEmpty())
	{
		AfxMessageBox(_T("输入密码不能为空！"), MB_ICONERROR);
		return;
	}

	OnOK();
}

void CUDiskLoginDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMER_SET_SETFOREGROUND == nIDEvent)
	{
		SetForegroundWindow();
	}

	CDialog::OnTimer(nIDEvent);
}
