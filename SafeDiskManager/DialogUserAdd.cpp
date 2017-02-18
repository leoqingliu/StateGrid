// DialogAddUser.cpp : implementation file
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "DialogUserAdd.h"


// CDialogAddUser dialog

IMPLEMENT_DYNAMIC(CDialogUserAdd, CDialog)

CDialogUserAdd::CDialogUserAdd(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogUserAdd::IDD, pParent)
	, m_strUser(_T(""))
	, m_strPassword(_T(""))
	, m_strPassword2(_T(""))
{

}

CDialogUserAdd::~CDialogUserAdd()
{
}

void CDialogUserAdd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_USER, m_strUser);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPassword);
	DDX_Text(pDX, IDC_EDIT_PASSWORD2, m_strPassword2);
}


BEGIN_MESSAGE_MAP(CDialogUserAdd, CDialog)
	ON_BN_CLICKED(IDOK, &CDialogUserAdd::OnBnClickedOk)
END_MESSAGE_MAP()


// CDialogAddUser message handlers

BOOL CDialogUserAdd::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);		// Set big icon
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);		// Set small icon

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDialogUserAdd::OnBnClickedOk()
{
	UpdateData();
	if (m_strPassword.IsEmpty())
	{
		AfxMessageBox(_T("输入密码为空！"), MB_ICONERROR);
		return;
	}

	if (m_strPassword != m_strPassword2)
	{
		AfxMessageBox(_T("输入密码不一致！"), MB_ICONERROR);
		return;
	}

	OnOK();
}
