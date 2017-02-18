// DialogLogin.cpp : implementation file
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "DialogUserLogin.h"


// CDialogLogin dialog

IMPLEMENT_DYNAMIC(CDialogUserLogin, CDialog)

CDialogUserLogin::CDialogUserLogin(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogUserLogin::IDD, pParent)
	, m_strUser(_T(""))
	, m_strPassword(_T(""))
{

}

CDialogUserLogin::~CDialogUserLogin()
{
}

void CDialogUserLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_USER, m_strUser);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPassword);
}


BEGIN_MESSAGE_MAP(CDialogUserLogin, CDialog)
	ON_BN_CLICKED(IDOK, &CDialogUserLogin::OnBnClickedOk)
END_MESSAGE_MAP()


// CDialogLogin message handlers

BOOL CDialogUserLogin::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);		// Set big icon
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);		// Set small icon

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogUserLogin::OnBnClickedOk()
{
	UpdateData();
	if (m_strUser.IsEmpty())
	{
		AfxMessageBox(_T("用户不能为空！"), MB_ICONERROR);
		return;
	}

	if (m_strPassword.IsEmpty())
	{
		AfxMessageBox(_T("密码不能为空！"), MB_ICONERROR);
		return;
	}

	OnOK();
}
