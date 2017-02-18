// GroupDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "GroupDialog.h"


// CGroupDialog dialog

IMPLEMENT_DYNAMIC(CGroupDialog, CDialog)

CGroupDialog::CGroupDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CGroupDialog::IDD, pParent)
	, m_strName(_T(""))
{

}

CGroupDialog::~CGroupDialog()
{
}

void CGroupDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_USER, m_strName);
}


BEGIN_MESSAGE_MAP(CGroupDialog, CDialog)
	ON_BN_CLICKED(IDOK, &CGroupDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CGroupDialog message handlers

BOOL CGroupDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CGroupDialog::OnBnClickedOk()
{
	UpdateData();
	if (m_strName.IsEmpty())
	{
		AfxMessageBox(_T("输入不能为空!"), MB_ICONERROR);
		return;
	}

	OnOK();
}
