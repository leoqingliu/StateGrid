// ModifyCommentDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "ModifyCommentDlg.h"


// CModifyCommentDlg dialog

IMPLEMENT_DYNAMIC(CModifyCommentDlg, CDialog)

CModifyCommentDlg::CModifyCommentDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModifyCommentDlg::IDD, pParent)
	, m_strComment(_T(""))
{

}

CModifyCommentDlg::~CModifyCommentDlg()
{
}

void CModifyCommentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strComment);
}


BEGIN_MESSAGE_MAP(CModifyCommentDlg, CDialog)
END_MESSAGE_MAP()


// CModifyCommentDlg message handlers

BOOL CModifyCommentDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
