// DialogProgram.cpp : implementation file
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "DialogProgram.h"


// CDialogProgram dialog

IMPLEMENT_DYNAMIC(CDialogProgram, CDialog)

CDialogProgram::CDialogProgram(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogProgram::IDD, pParent)
	, m_strPassword(_T(""))
{

}

CDialogProgram::~CDialogProgram()
{
}

void CDialogProgram::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPassword);
}


BEGIN_MESSAGE_MAP(CDialogProgram, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SET, &CDialogProgram::OnBnClickedButtonSet)
END_MESSAGE_MAP()


// CDialogProgram message handlers

void CDialogProgram::OnBnClickedButtonSet()
{
	// TODO: Add your control notification handler code here
}

BOOL CDialogProgram::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
