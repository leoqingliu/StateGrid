// LoginModifyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "LoginModifyDlg.h"


// CLoginModifyDlg dialog

IMPLEMENT_DYNAMIC(CLoginModifyDlg, CDialog)

CLoginModifyDlg::CLoginModifyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoginModifyDlg::IDD, pParent)
	, m_strPassword(_T(""))
	, m_strPassword2(_T(""))
{

}

CLoginModifyDlg::~CLoginModifyDlg()
{
}

void CLoginModifyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPassword);
	DDX_Text(pDX, IDC_EDIT_PASSWORD2, m_strPassword2);
}


BEGIN_MESSAGE_MAP(CLoginModifyDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_OK, &CLoginModifyDlg::OnBnClickedBtnOk)
END_MESSAGE_MAP()


// CLoginModifyDlg message handlers

BOOL CLoginModifyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);		// Set big icon
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);		// Set small icon

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CLoginModifyDlg::OnBnClickedBtnOk()
{
	UpdateData();
	if (m_strPassword.IsEmpty())
	{
		AfxMessageBox(_T("密码不能为空！"), MB_ICONERROR);
		return;
	}

	if (m_strPassword != m_strPassword2)
	{
		AfxMessageBox(_T("密码不一致！"), MB_ICONERROR);
		return;
	}

	OnOK();
}
