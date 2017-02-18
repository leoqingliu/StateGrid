// DialogModify.cpp : implementation file
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "DialogUserModify.h"


// CDialogModify dialog

IMPLEMENT_DYNAMIC(CDialogUserModify, CDialog)

CDialogUserModify::CDialogUserModify(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogUserModify::IDD, pParent)
	, m_strPassword(_T(""))
	, m_strPassword2(_T(""))
{

}

CDialogUserModify::~CDialogUserModify()
{
}

void CDialogUserModify::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPassword);
	DDX_Text(pDX, IDC_EDIT_PASSWORD2, m_strPassword2);
}


BEGIN_MESSAGE_MAP(CDialogUserModify, CDialog)
	ON_BN_CLICKED(IDOK, &CDialogUserModify::OnBnClickedOk)
END_MESSAGE_MAP()


// CDialogModify message handlers

BOOL CDialogUserModify::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);		// Set big icon
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);		// Set small icon

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogUserModify::OnBnClickedOk()
{
	UpdateData();
	if (m_strPassword != m_strPassword2)
	{
		AfxMessageBox(_T("ÃÜÂë²»Ò»ÖÂ£¡"), MB_ICONERROR);
		return;
	}

	OnOK();
}
